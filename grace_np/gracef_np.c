#include <config.h>

#if defined(WITH_F77_WRAPPER)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grace_np.h"

#ifdef NEED_F77_UNDERSCORE
#  define F77_FNAME(fname) fname ## _
#else
#  define F77_FNAME(fname) fname
#endif

typedef void (*GraceFortranFunctionType) (const char *str, int len);
static GraceFortranFunctionType fortran_error = (GraceFortranFunctionType) 0;

static void GraceFortranWrapper(const char *str)
{
    if (fortran_error == (GraceFortranFunctionType) 0) {
        fprintf(stderr, "%s\n", str);
    } else {
        fortran_error(str, strlen(str));
    }
}

void F77_FNAME(graceregistererrorfunctionf) (GraceFortranFunctionType f)
{
    fortran_error = f;
    GraceRegisterErrorFunction(GraceFortranWrapper);
}

int F77_FNAME(graceopenf) (const int *arg)
{
    return (GraceOpen (*arg));
}

int F77_FNAME(graceisopenf) (void)
{
    return (GraceIsOpen ());
}

int F77_FNAME(graceclosef) (void)
{
    return (GraceClose ());
}

int F77_FNAME(graceclosepipef) (void)
{
    return (GraceClosePipe());
}

int F77_FNAME(graceflushf) (void)
{
    return (GraceFlush ());
}


int F77_FNAME(gracecommandf) (const char* arg, int length)
{
    char* str;
    int res;

    str = (char*) malloc ((size_t) (length + 1));
    if (str == NULL) {
        fprintf (stderr, "GraceCommandf: Not enough memory\n");
        return (-1);
    }
    strncpy (str, arg, length);
    str[length] = 0;
    res = GraceCommand (str);
    free (str);
    return (res);
}

#else /* don't include Fortran wrapper */

/* To make ANSI C happy about non-empty file */
void F77_FNAME(_gracef_np_c_dummy_func) (void) {}

#endif /* WITH_F77_WRAPPER */
