/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-99 Grace Development Team
 * 
 * Maintained by Evgeny Stambulchik
 * 
 * 
 *                           All Rights Reserved
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "missing.h"

/* To make ANSI C happy about non-empty file */
void _missing_c_dummy_func(void) {}

#ifndef HAVE_DRAND48
double drand48(void)
{
    return (double) (1.0/RAND_MAX)*rand();
}
#endif

#ifndef HAVE_GETTIMEOFDAY
int gettimeofday (tv, tz)
struct timeval *tv;
void *tz;
{
    timeb_t tmp_time;

    ftime(&tmp_time);

    if (tv != NULL)
    {
      tv->tv_sec  = tmp_time.time;
      tv->tv_usec = tmp_time.millitm * 1000;
    }

    return (0);
}
#endif

#ifndef HAVE_POPEN
#  ifdef __VMS

/* drop in popen() / pclose() for VMS
 * originally written for port of perl to vms
 */

/* (to aid porting) - how are errors dealt with */

#define ERROR(msg) { fprintf(stderr, "%s\nFile %s line %d\n", msg, __FILE__, __LINE__); }
#define FATAL(msg) { fprintf(stderr, "%s\nFile %s line %d\n", msg, __FILE__, __LINE__); exit(RETURN_FAILURE); }

#include <unistd.h>
#include <string.h>
#include <descrip.h>
#include <starlet.h>
#include <lib$routines.h>
#include <dvidef.h>
#include <syidef.h>
#include <jpidef.h>
#include <ssdef.h>

#define _cksts(call) \
  if (!(sts=(call))&1) FATAL("Internal error") else {}

static void
create_mbx(unsigned short int *chan, struct dsc$descriptor_s *namdsc)
{
  static unsigned long int mbxbufsiz;
  long int syiitm = SYI$_MAXBUF, dviitm = DVI$_DEVNAM;
  unsigned long sts;  /* for _cksts */
  
  if (!mbxbufsiz) {
    /*
     * Get the SYSGEN parameter MAXBUF, and the smaller of it and the
     * preprocessor consant BUFSIZ from stdio.h as the size of the
     * 'pipe' mailbox.
     */

    _cksts(lib$getsyi(&syiitm, &mbxbufsiz, 0, 0, 0, 0));
    if (mbxbufsiz > BUFSIZ) mbxbufsiz = BUFSIZ; 
  }
  _cksts(sys$crembx(0,chan,mbxbufsiz,mbxbufsiz,0,0,0));

  _cksts(lib$getdvi(&dviitm, chan, NULL, NULL, namdsc, &namdsc->dsc$w_length));
  namdsc->dsc$a_pointer[namdsc->dsc$w_length] = '\0';

}  /* end of create_mbx() */

struct pipe_details
{
    struct pipe_details *next;
    FILE *fp;
    int pid;
    unsigned long int completion;
};

static struct pipe_details *open_pipes = NULL;
static $DESCRIPTOR(nl_desc, "NL:");
static int waitpid_asleep = 0;

static void
popen_completion_ast(unsigned long int unused)
{
  if (waitpid_asleep) {
    waitpid_asleep = 0;
    sys$wake(0,0);
  }
}

#pragma message save
#pragma message disable (ADDRCONSTEXT)
FILE *
popen(char *cmd, char *mode)
{
    char mbxname[64];
    unsigned short int chan;
    unsigned long int flags=1;  /* nowait - gnu c doesn't allow &1 */
    struct pipe_details *info;
    struct dsc$descriptor_s namdsc = {sizeof mbxname, DSC$K_DTYPE_T,
                                      DSC$K_CLASS_S, mbxname},
                            cmddsc = {0, DSC$K_DTYPE_T,
                                      DSC$K_CLASS_S, 0};
        unsigned long sts;                            

    if (!(info=malloc(sizeof(struct pipe_details))))
    {
        ERROR("Cannot malloc space");
        return NULL;
    }

    info->completion=0;  /* I assume this will remain 0 until terminates */
        
    /* create mailbox */
    create_mbx(&chan,&namdsc);

    /* open a FILE* onto it */
    info->fp=fopen(mbxname, mode);

    /* give up other channel onto it */
    _cksts(sys$dassgn(chan));

    if (!info->fp)
        return NULL;
        
    cmddsc.dsc$w_length=strlen(cmd);
    cmddsc.dsc$a_pointer=cmd;

    if (strcmp(mode,"r")==0) {
      _cksts(lib$spawn(&cmddsc, &nl_desc, &namdsc, &flags,
                     0  /* name */, &info->pid, &info->completion,
                     0, popen_completion_ast,0,0,0,0));
    }
    else {
      _cksts(lib$spawn(&cmddsc, &namdsc, 0 /* sys$output */, &flags,
                     0  /* name */, &info->pid, &info->completion));
    }

    info->next=open_pipes;  /* prepend to list */
    open_pipes=info;
        
    return info->fp;
}
#pragma message restore

int pclose(FILE *fp)
{
    struct pipe_details *info, *last = NULL;
    unsigned long int abort = SS$_TIMEOUT, retsts;
    unsigned long sts;
    
    for (info = open_pipes; info != NULL; last = info, info = info->next)
        if (info->fp == fp) break;

    if (info == NULL)
      /* get here => no such pipe open */
      /* FATAL("pclose() - no such pipe open ???"); too extreme, removed*/
                return -1;                                                              /* standard behaviour */

    if (!info->completion) { /* Tap them gently on the shoulder . . .*/
      _cksts(sys$forcex(&info->pid,0,&abort));
      sleep(1);
    }
    if (!info->completion)  /* We tried to be nice . . . */
      _cksts(sys$delprc(&info->pid));
    
    fclose(info->fp);
    /* remove from list of open pipes */
    if (last) last->next = info->next;
    else open_pipes = info->next;
    retsts = info->completion;
    free(info);

    return retsts;
}  /* end of pclose() */


/* sort-of waitpid; use only with popen() */
/*{{{unsigned long int waitpid(unsigned long int pid, int *statusp, int flags)*/
unsigned long int
waitpid(unsigned long int pid, int *statusp, int flags)
{
    struct pipe_details *info;
    unsigned long int abort = SS$_TIMEOUT;
    unsigned long sts;
    
    for (info = open_pipes; info != NULL; info = info->next)
        if (info->pid == pid) break;

    if (info != NULL) {  /* we know about this child */
      while (!info->completion) {
        waitpid_asleep = 1;
        sys$hiber();
      }

      *statusp = info->completion;
      return pid;
    }
    else {  /* we haven't heard of this child */
      $DESCRIPTOR(intdsc,"0 00:00:01");
      unsigned long int ownercode = JPI$_OWNER, ownerpid, mypid;
      unsigned long int interval[2];

      _cksts(lib$getjpi(&ownercode,&pid,0,&ownerpid,0,0));
      _cksts(lib$getjpi(&ownercode,0,0,&mypid,0,0));
      if (ownerpid != mypid)
        FATAL("pid not a child");

      _cksts(sys$bintim(&intdsc,interval));
      while ((sts=lib$getjpi(&ownercode,&pid,0,&ownerpid,0,0)) & 1) {
        _cksts(sys$schdwk(0,0,interval,0));
        _cksts(sys$hiber());
      }
      _cksts(sts);

      /* There's no easy way to find the termination status a child we're
       * not aware of beforehand.  If we're really interested in the future,
       * we can go looking for a termination mailbox, or chase after the
       * accounting record for the process.
       */
      *statusp = 0;
      return pid;
    }
                    
}  /* end of waitpid() */

#  else /* not __VMS */

/* temporary filename */
static char tfile[GR_MAXPATHLEN];
/* filter action */
static enum filtact { FILTER_NONE, FILTER_READ, FILTER_WRITE } filt_act;

FILE *popen(char *cmd, char *mode)
{
    switch (mode[0]) {
    case 'r':
        filt_act = FILTER_READ;
        strcpy(buf, cmd);
        strcat(buf, " > ");
        break;
    case 'w':
        filt_act = FILTER_WRITE;
        strcpy(buf, cmd);
        strcat(buf, " < ");
        break;
    default:
        filt_act = FILTER_NONE;
        return NULL;
    }
    
    strcat(buf, tmpnam(tfile));
    if (system(buf) == -1) {
        tfile[0] = '\0';
        filt_act = FILTER_NONE;
        return NULL;            
    } else {
        return fopen(tfile, mode);
    }
}

int pclose(FILE *fp)
{
    int result = -1;
    
    result = fclose( fp );
    switch( filt_act ) {
    case FILTER_READ:
        remove( tfile );
        tfile[0] = '\0';
        break;
    case FILTER_WRITE:
        remove( tfile );
        tfile[0] = '\0';
        break;
    default:
        break;
    }
    
    return result;
}

#  endif /* __VMS */

#endif /* HAVE_POPEN */

#ifdef __EMX__
char *exe_path_translate(char *path)
{
    static char absfn[GR_MAXPATHLEN];
    
    _abspath(absfn, path, GR_MAXPATHLEN);
    
    return(absfn);
}
#endif

#ifdef __VMS
#  include <unixlib.h>
char *path_translate(const char *path)
{
    char *st;

    st = decc$translate_vms(path);
    if ((int) st == 0 || (int) st == -1) {
        return NULL;
    } else {
        return st;
    }
}
#endif

#ifdef __VMS
#  ifndef __CRTL_VER
#    define __CRTL_VER __VMS_VER
#  endif

/* this is a "system" function for VMS because 
   system("spawn/nowait mosaic ...") dosn't work --
   system creates a subprocess, which runs spawn and
   creates another subprocess; when the spawn command
   completes, the first subprocess exits and the second
   one (which is a subprocess of the subprocess) also
   exits. */

#include <string.h>
#include <descrip.h>
#include <lib$routines.h>
#include <clidef.h>

int system_spawn(const char *command)
{
  $DESCRIPTOR(dstr, ""); int retval;
  
  dstr.dsc$a_pointer = malloc(1024);
  strcpy(dstr.dsc$a_pointer, command);
  dstr.dsc$w_length = strlen(dstr.dsc$a_pointer);

  retval = lib$spawn(&dstr, 0, 0, &CLI$M_NOWAIT); 
  
  free(dstr.dsc$a_pointer);
  
  return retval; 
}

# if __CRTL_VER < 70000000 

/* Define a getlogin function for VMS before version 7. */

# include <starlet.h>
# include <jpidef.h>

  typedef struct
  {
    unsigned short buffer_length, item_code;
    char    *buffer;
    int     *return_len;
  } itmlst_item;

  char *getlogin()
  {
    int ret, i; itmlst_item itmlst[2];
    static char username[13];

    itmlst[0].buffer_length = sizeof(username)-1;
    itmlst[0].item_code = JPI$_USERNAME;
    itmlst[0].buffer = username;
    itmlst[0].return_len = NULL;

    itmlst[1].buffer_length = 0;
    itmlst[1].item_code = 0;
    itmlst[1].buffer = NULL;
    itmlst[1].return_len = NULL;

    ret = sys$getjpiw(NULL, NULL, NULL, itmlst, NULL, NULL, NULL);

    if ((ret && 1) == 0) strcpy(username, "");
    else
    {
      for (i=0; i<sizeof(username); i++)
      {
        if (username[i] == ' ')
        {
          username[i] = '\0'; break;
        }
      }
      if (i == sizeof(username)) username[--i] = '\0';
    }

    return username; 
  }

/*
 *      getpwnam(name) - retrieves a UAF entry
 *
 *      Author:         Patrick L. Mahan
 *      Location:       TGV, Inc
 *      Date:           15-Nov-1991
 *
 *      Purpose:        Provides emulation for the UNIX getpwname routine.
 *
 *      Modification History
 *
 *      Date        | Who       | Version       | Reason
 *      ------------+-----------+---------------+---------------------------
 *      15-Nov-1991 | PLM       | 1.0           | First Write
 *      20-Nov-1997 | RN        |               | #include <stdlib.h>
 */

#pragma message save
#pragma message disable (CVTDIFTYPES,ADDRCONSTEXT,NEEDCONSTEXT)

#include <uaidef.h>

struct uic {
   unsigned short uid;
   unsigned short gid;
};

#define TEST(ptr, str)  { if (ptr == NULL) {    \
                                fprintf(stderr, "getpwnam: memory allocation failure for \"%s\"\n",     \
                                        str);   \
                                exit(-1);       \
                          } }

struct passwd *getpwnam(name)
char *name;
{
   int  istatus;
   int  UserNameLen;
   int  UserOwnerLen;
   int  UserDeviceLen;
   int  UserDirLen;
   char UserName[13];
   char UserOwner[32];
   char UserDevice[32];
   char UserDir[64];
   char *cptr, *sptr;
   unsigned long int UserPwd[2];
   unsigned short int UserSalt;
   unsigned long int UserEncrypt;
   struct uic UicValue;
   struct passwd *entry;

   struct dsc$descriptor_s VMSNAME =
        {strlen(name), DSC$K_DTYPE_T, DSC$K_CLASS_S, name};

   struct itmlist3 {
        unsigned short int length;
        unsigned short int item;
        unsigned long  int addr;
        unsigned long  int retaddr;
   } ItemList[] = {
        {12, UAI$_USERNAME, &UserName, &UserNameLen},
        {8,  UAI$_PWD, &UserPwd, 0},
        {4,  UAI$_UIC, &UicValue, 0},
        {32, UAI$_OWNER, &UserOwner, &UserOwnerLen},
        {32, UAI$_DEFDEV, &UserDevice, &UserDeviceLen},
        {64, UAI$_DEFDIR, &UserDir, &UserDirLen},
        {2,  UAI$_SALT, &UserSalt, 0},
        {4,  UAI$_ENCRYPT, &UserEncrypt, 0},
        {0, 0, 0, 0}
   };

   UserNameLen = 0;
   istatus = sys$getuai (0, 0, &VMSNAME, &ItemList, 0, 0, 0);

   if (!(istatus & 1)) {
#ifdef DEBUG
        lib$signal(istatus);
#endif /* DEBUG */
        fprintf (stderr, "getpwnam: unable to retrieve passwd entry for %s\n",
                 name);
        fprintf (stderr, "getpwnam: vms error number is 0x%x\n", istatus);
        return ((struct passwd *)NULL);
   }

   entry = (struct passwd *) calloc (1, sizeof(struct passwd));
   TEST(entry, "PASSWD_ENTRY");

   entry->pw_uid = UicValue.uid;
   entry->pw_gid = UicValue.gid;
   entry->pw_salt = UserSalt;
   entry->pw_encrypt = UserEncrypt;

   sptr = UserName;
   cptr = calloc (UserNameLen+1, sizeof(char));
   TEST(cptr, "USERNAME");
   strncpy (cptr, sptr, UserNameLen);
   cptr[UserNameLen] = '\0';
   entry->pw_name = cptr;

   cptr = calloc(8, sizeof(char));
   TEST(cptr, "PASSWORD");
   memcpy(cptr, UserPwd, 8);
   entry->pw_passwd = cptr;

   sptr = UserOwner; sptr++;
   cptr = calloc ((int)UserOwner[0]+1, sizeof(char));
   TEST(cptr, "FULLNAME");
   strncpy (cptr, sptr, (int)UserOwner[0]);
   cptr[(int)UserOwner[0]] = '\0';
   entry->pw_gecos = cptr;

   cptr = calloc ((int)UserDevice[0]+(int)UserDir[0]+1, sizeof(char));
   TEST(cptr, "HOME");
   sptr = UserDevice; sptr++;
   strncpy (cptr, sptr, (int)UserDevice[0]);
   sptr = UserDir; sptr++;
   strncat (cptr, sptr, (int)UserDir[0]);
   cptr[(int)UserDevice[0]+(int)UserDir[0]] = '\0';
   entry->pw_dir = cptr;

   cptr = calloc (strlen("SYS$SYSTEM:LOGINOUT.EXE")+1, sizeof(char));
   TEST(cptr,"SHELL");
   strcpy (cptr, "SYS$SYSTEM:LOGINOUT.EXE");
   entry->pw_shell = cptr;

   return (entry);
}

#pragma message restore

#  endif  /* __CRTL_VER */

#endif  /* __VMS */
