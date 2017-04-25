/* buildinfo.c */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#ifndef NONE_GUI
#  include <Xm/Xm.h>
#  include <Xbae/patchlevel.h>
#endif

#include <t1lib.h>
#ifdef HAVE_LIBPNG
#include <png.h>
#endif
#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#endif
#ifdef HAVE_LIBPDF
#include <pdflib.h>
#endif

#define MAJOR_REV 5
#define MINOR_REV 1
#define PATCHLEVEL 25
/* #define BETA_VER "dev" */

#ifndef GRACE_HOME
#  define GRACE_HOME "/usr/local/grace"
#endif

#ifndef GRACE_PRINT_CMD
#  define GRACE_PRINT_CMD ""
#endif

#ifndef GRACE_EDITOR
#  define GRACE_EDITOR "xterm -e vi"
#endif

#ifndef GRACE_HELPVIEWER
#  define GRACE_HELPVIEWER "mozilla -remote openURL\\\\(%s,new-window\\\\) >>/dev/null 2>&1 || mozilla %s"
#endif


static void VersionInfo(FILE *outfile)
{

    struct utsname u_info;
    time_t time_info;
    char *ctime_string;

    fprintf(outfile, "#define BI_VERSION_ID %d\n",
            MAJOR_REV*10000 + MINOR_REV*100 + PATCHLEVEL);
#ifdef BETA_VER
    fprintf(outfile, "#define BI_VERSION \"Grace-%d.%d.%d %s\"\n",
	    MAJOR_REV, MINOR_REV, PATCHLEVEL, BETA_VER);
#else
    fprintf(outfile, "#define BI_VERSION \"Grace-%d.%d.%d\"\n",
	    MAJOR_REV, MINOR_REV, PATCHLEVEL);
#endif

/* We don't want to reproduce the complete config.h,
   but those settings which may be related to problems at runtime */

#ifdef NONE_GUI
    fprintf(outfile, "#define BI_GUI \"none\"\n");
#else
    fprintf(outfile, "#define BI_GUI \"%s\"\n", XmVERSION_STRING);
    fprintf(outfile, "#define BI_GUI_XBAE \"%i\"\n", XbaeVersion);
#endif
    
    fprintf(outfile, "#define BI_T1LIB \"%s\"\n",
        T1_GetLibIdent());

#ifdef HAVE_LIBPNG
    fprintf(outfile, "#define BI_PNGLIB \"%s\"\n",
        PNG_LIBPNG_VER_STRING);
#else
    fprintf(outfile, "#define BI_PNGLIB \"\"\n");
#endif

#ifdef HAVE_LIBJPEG
    fprintf(outfile, "#define BI_LIBJPEG \"%i\"\n",
        JPEG_LIB_VERSION);
#else
    fprintf(outfile, "#define BI_LIBJPEG \"\"\n");
#endif

#ifdef HAVE_LIBPDF
    fprintf(outfile, "#define BI_LIBPDF \"%s\"\n",
        PDFLIB_VERSIONSTRING);
#else
    fprintf(outfile, "#define BI_LIBPDF \"\"\n");
#endif

    fprintf(outfile, "#define BI_CCOMPILER \"%s\"\n",
        CCOMPILER);
    
    uname(&u_info);
    fprintf(outfile, "#define BI_SYSTEM \"%s %s %s %s\"\n",
        u_info.sysname, u_info.version, u_info.release, u_info.machine);

    time_info = time(NULL);
    ctime_string = ctime(&time_info);
    if (ctime_string[strlen(ctime_string) - 1] == '\n') {
        ctime_string[strlen(ctime_string) - 1] = '\0';
    }
    fprintf(outfile, "#define BI_DATE \"%s\"\n", ctime_string);

    fprintf(outfile, "\n");

    fprintf(outfile, "#define GRACE_HOME \"%s\"\n", GRACE_HOME);
    fprintf(outfile, "#define GRACE_EDITOR \"%s\"\n", GRACE_EDITOR);
    fprintf(outfile, "#define GRACE_PRINT_CMD \"%s\"\n", GRACE_PRINT_CMD);
    fprintf(outfile, "#define GRACE_HELPVIEWER \"%s\"\n", GRACE_HELPVIEWER);

    return;
}


int main(int argc, char *argv[])
{
    FILE *outfile;

    if (argc == 1) {
	outfile = stdout;
    } else {
        if (!(outfile = fopen(argv[1], "w"))) {
            fprintf(stderr, "Failed to open %s for writing!\a\n", argv[1]);
            exit(1);
        }
    }

    VersionInfo(outfile);
   
    if (outfile != stdout) {
        fclose(outfile);
    }
    exit(0);
}
