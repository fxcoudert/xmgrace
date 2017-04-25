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

/* replacements for some functions */

#ifndef __MISSING_H_
#define __MISSING_H_

#include <config.h>

#include <stdio.h>

#if defined(__VMS)
#  ifndef __CRTL_VER
#    define __CRTL_VER __VMS_VER
#  endif
   int system_spawn(const char *command);
#  if __ALPHA || __DECC_VER >= 60000000
#    include <builtins.h>
#  endif
#  if __CRTL_VER < 70000000 
#    define O_NONBLOCK O_NDELAY
struct passwd {
    char  *pw_name;
    char  *pw_passwd;
    int   pw_uid;
    int   pw_gid;
    short pw_salt;
    int   pw_encrypt;
    char  *pw_age;
    char  *pw_comment;
    char  *pw_gecos;
    char  *pw_dir;
    char  *pw_shell;
};
char *getlogin();
struct passwd *getpwnam(char *name);
#  endif  /* __CRTL_VER */
#endif /* __VMS */

#ifndef HAVE_MEMMOVE
#  define memmove(a, b, c) bcopy((b), (a), (c))
#endif

#ifndef HAVE_MEMCPY
#  define memcpy(a, b, c) bcopy ((b), (a), (c))
#endif

#ifndef HAVE_GETHOSTNAME
#  define gethostname(a, n) (strncpy((a), "localhost", n)?0:1)
#endif

#ifndef HAVE_DRAND48
#  define srand48 srand
#  define lrand48 rand
double drand48(void);
#else
#  ifndef HAVE_DRAND48_DECL
extern double drand48(void);
#  endif
#endif

#ifndef HAVE_GETCWD
#  ifdef OS2
#    define getcwd _getcwd2
#    define chdir _chdir2
#  endif
#endif

#ifndef HAVE_UNLINK
#  ifdef VMS
#    include <unixio.h>
#    define unlink delete
#  endif
#endif

#ifndef HAVE_POPEN
FILE *popen(char *cmd, char *mode);
int   pclose(FILE *fp);
#endif

#ifndef HAVE_GETTIMEOFDAY
#  include <time.h>
int gettimeofday (struct timeval *tp, void *tzp);
#endif

#ifndef HAVE_ALLOCA
void *alloca(unsigned int);
#endif
#if defined(__VMS) && (__ALPHA || __DECC_VER >= 60000000)
#  define alloca __ALLOCA
#endif

#ifdef __EMX__
char *exe_path_translate(char *path);
#else
#  define exe_path_translate(p) (p)
#endif

#ifdef __VMS
char *path_translate(const char *path);
#else
#  define path_translate(p) (p)
#endif



#endif /* __MISSING_H_ */
