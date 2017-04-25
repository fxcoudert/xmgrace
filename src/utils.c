/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2007 Grace Development Team
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

/*
 * misc utilities
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
#ifdef TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  ifdef HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif
#include <signal.h>
#include <sys/types.h>
#include <sys/resource.h>
#ifdef HAVE_SYS_SELECT_H
#  include <sys/select.h>
#endif
#include <limits.h>

#ifdef HAVE_SETLOCALE
#  include <locale.h>
#endif

#include "buildinfo.h"

#include "globals.h"
#include "utils.h"
#include "files.h"
#include "protos.h"

static void rereadConfig(void);
static RETSIGTYPE actOnSignal(int signo);
static void bugwarn(char *signame);

/*
 * free and check for NULL pointer
 */
void xfree(void *ptr)
{
    if (ptr != NULL) {
	free(ptr);
    }
}

void *xmalloc(size_t size)
{
    void *retval;

    if (size == 0) {
        retval = NULL;
    } else {
        retval = malloc(size);
    }

    if (retval == NULL && size != 0) {
        errmsg("Memory storage exceeded!");
    }
    return retval;
}

void *xcalloc(size_t nmemb, size_t size)
{
    void *retval;

    if (nmemb == 0) {
        retval = NULL;
    } else {
        retval = calloc(nmemb, size);
    }
    
    if (retval == NULL && nmemb != 0) {
        errmsg("Memory storage exceeded!");
    }
    return retval;
}

void *xrealloc(void *ptr, size_t size)
{
    void *retval;

#if defined(REALLOC_IS_BUGGY)
    if (ptr == NULL) {
        retval = malloc(size);
    } else if (size == 0) {
        xfree(ptr);
        retval = NULL;
    } else {
        retval = realloc(ptr, size); 
    }
#else
    retval = realloc(ptr, size);
    if (size == 0) {
        retval = NULL;
    }
#endif
    
    if (retval == NULL && size != 0) {
        errmsg("Memory storage exceeded!");
    }
    return retval;
}

/*
 * swap doubles and ints
 */
void fswap(double *x, double *y)
{
    double tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;
}

void iswap(int *x, int *y)
{
    int tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;
}

int isoneof(int c, char *s)
{
    while (*s) {
	if (c == *s) {
	    return 1;
	} else {
	    s++;
	}
    }
    return 0;
}

int argmatch(char *s1, char *s2, int atleast)
{
    int l1 = strlen(s1);
    int l2 = strlen(s2);

    if (l1 < atleast) {
	return 0;
    }
    if (l1 > l2) {
	return 0;
    }
    return (strncmp(s1, s2, l1) == 0);
}

/*
 * convert a string from lower to upper case
 * leaving quoted strings alone
 */
void lowtoupper(char *s)
{
    int i, quoteon = FALSE;

    for (i = 0; i < strlen(s); i++) {
	if (s[i] == '"') {
	    if (!quoteon) {
		quoteon = TRUE;
	    } else if ((i > 0) && (s[i-1] != '\\')) {
		quoteon = FALSE;
	    }
	}
	if (quoteon == FALSE) {
            if (!isprint(s[i])) {
                s[i] = ' ';
            } else if (s[i] >= 'a' && s[i] <= 'z') {
	        s[i] -= ' ';
	    }
        }
    }
}

/*
 * remove all that fortran nastiness
 */
void convertchar(char *s)
{
    while (*s++) {
	if (*s == ',')
	    *s = ' ';
	if (*s == 'D' || *s == 'd')
	    *s = 'e';
    }
}

/*
 * log base 2
 */
int ilog2(int n)
{
    int i = 0;
    int n1 = n;

    while (n1 >>= 1)
	i++;
    if (1 << i != n)
	return -1;
    else
	return i;
}

/*
 * compute the area bounded by the polygon (xi,yi)
 */
double comp_area(int n, double *x, double *y)
{
    int i;
    double sum = 0.0;

    for (i = 0; i < n; i++) {
	sum = sum + x[i] * y[(i + 1) % n] - y[i] * x[(i + 1) % n];
    }
    return sum * 0.5;
}

/*
 * compute the perimeter bounded by the polygon (xi,yi)
 */
double comp_perimeter(int n, double *x, double *y)
{
    int i;
    double sum = 0.0;

    for (i = 0; i < n - 1; i++) {
	sum = sum + hypot(x[i] - x[(i + 1) % n], y[i] - y[(i + 1) % n]);
    }
    return sum;
}

char *dayofweekstrs[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char *dayofweekstrl[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char *monthl[] = {"January", "February", "March", "April", "May", "June",
"July", "August", "September", "October", "November", "December"};

int dayofweek(double j)
{
    int i = (int) floor(j + 1.5);
    return (i <= 0) ? 6 - (6 - i)%7 : i%7;
}

/*
 * escape quotes
 */
char *escapequotes (char *s)
{
    static char *es = NULL;
    int i, k, n, len, elen;
    
    if (s == NULL)
        return NULL;
    
    len = strlen(s);
    es = xrealloc(es, (len + 1)*SIZEOF_CHAR);
    strcpy(es, s);
    n = 0;
    while ((es = strchr(es, '\"'))) {
    	es++;
    	n++;
    }
    
    elen = len + n + 1;
    es = xrealloc(es, elen*SIZEOF_CHAR);
    
    i = k = 0;
    while (i < len) {
        if (s[i] == '\"') {
            es[k] = '\\';
            k++;
        }
        es[k] = s[i];
        i++; k++;
    }
    es[elen-1] = '\0';
    return es;
}

int sign(double a)
{
    if (a > 0.0) {
        return +1;
    } else if (a < 0.0) {
        return -1;
    } else {
        return 0;
    }
}

double mytrunc(double a)
{
    if (a > 0.0) {
        return floor(a);
    } else {
        return ceil(a);
    }
}

/*
 * exit grace
 */
void bailout(void)
{
    if (!is_dirtystate() || yesno("Exit losing unsaved changes?", NULL, NULL, NULL)) {
         if (resfp) {
             grace_close(resfp);
         }
         exit(0);
    }
}

/*
 * Reread config (TODO)
 */
static void rereadConfig(void)
{
    getparms("gracerc");
}

static void please_report_the_bug(void)
{
    fprintf(stderr, "\nPlease use \"Help/Comments\" to report the bug.\n");
#ifdef HAVE_LESSTIF
    fprintf(stderr, "NB. This version of Grace was compiled with LessTif.\n");
    fprintf(stderr, "    Make sure to read the FAQ carefully prior to\n");
    fprintf(stderr, "    reporting the bug, ESPECIALLY is the problem might\n");
    fprintf(stderr, "    be related to the graphical interface.\n");
#endif
}

/*
 * Warn about bug (TODO X message)
 */
static void bugwarn(char *signame)
{
    static int emergency_save = FALSE;
/*
 *  Since we got so far, memory is probably corrupted so it's better to use
 *  a static storage
 */
    static char buf[GR_MAXPATHLEN];
/* number of interrupts received during the emergency save */
    static int interrupts;
    
    if (emergency_save != FALSE) {
        /* don't mind signals anymore: we're in emergency save mode already */
        interrupts++;
        if (interrupts > 10) {
            fprintf(stderr, "oh, no luck :-(\n");
            please_report_the_bug();
            abort();
        }
        return;
    } else {
        emergency_save = TRUE;
        interrupts = 0;
        fprintf(stderr, "\a\nOops! Got %s\n", signame);
        if (is_dirtystate()) {
            strcpy(buf, get_docname());
            strcat(buf, "$");
            fprintf(stderr, "Trying to save your work into file \"%s\"... ", buf);
            fflush(stderr);
            noask = TRUE;
            if (save_project(buf) == RETURN_SUCCESS) {
                fprintf(stderr, "ok!\n");
            } else {
                fprintf(stderr, "oh, no luck :-(\n");
            }
        }
        please_report_the_bug();
        abort();
    }
}

/*
 * Signal-handling routines
 */
 
static RETSIGTYPE actOnSignal(int signo)
{
    char signame[16];
     
    installSignal();
    
    switch (signo) {
#ifdef SIGHUP
    case SIGHUP:
    	rereadConfig();
    	break;
#endif
#ifdef SIGINT
    case SIGINT:
#endif
#ifdef SIGQUIT
    case SIGQUIT:
#endif
#ifdef SIGTERM
    case SIGTERM:
#endif
        bailout();
        break;
#ifdef SIGILL
    case SIGILL:
        strcpy(signame, "SIGILL");
#endif
#ifdef SIGFPE
    case SIGFPE:
        strcpy(signame, "SIGFPE");
#endif
#ifdef SIGBUS
    case SIGBUS:
        strcpy(signame, "SIGBUS");
#endif
#ifdef SIGSEGV
    case SIGSEGV:
        strcpy(signame, "SIGSEGV");
#endif
#ifdef SIGSYS
    case SIGSYS:
        strcpy(signame, "SIGSYS");
#endif
        bugwarn(signame);
        break;
    default:
        break;
    }
}

void installSignal(void){
#ifdef SIGHUP
    signal(SIGHUP,  actOnSignal);   /* hangup */
#endif
#ifdef SIGINT
    signal(SIGINT,  actOnSignal);   /* interrupt */
#endif
#ifdef SIGQUIT
    signal(SIGQUIT, actOnSignal);   /* quit */
#endif
#ifdef SIGILL
    signal(SIGILL,  actOnSignal);   /* illegal instruction */
#endif
#ifdef SIGFPE
    signal(SIGFPE,  actOnSignal);   /* floating point exception */
#endif
#ifdef SIGBUS
    signal(SIGBUS,  actOnSignal);   /* bus error */
#endif
#ifdef SIGSEGV
    signal(SIGSEGV, actOnSignal);   /* segmentation violation */
#endif
#ifdef SIGSYS
    signal(SIGSYS,  actOnSignal);   /* bad argument to system call */
#endif
#ifdef SIGTERM
    signal(SIGTERM, actOnSignal);   /* software termination signal */
#endif
#ifdef SIGALRM
    signal(SIGALRM, actOnSignal);   /* timer */
#endif
#ifdef SIGIO
    signal(SIGIO, actOnSignal);     /* input/output ready */
#endif
}


/* create format string */
char *create_fstring(int form, int prec, double loc, int type)
{
    char format[64], *eng_prefix,*comp_prefix;
    static char s[MAX_STRING_LENGTH];
    double tmp;
    int m, d, y, h, mm, sec;
    double arcmin, arcsec;
    int exponent;
    double mantissa;
    int yprec;
    
    if (two_digits_years_allowed()) {
        yprec = 2;
    } else {
        yprec = 4;
    }

    /* for locale decimal points */
    set_locale_num(TRUE);

    strcpy(format, "%.*lf");
    switch (form) {
    case FORMAT_DECIMAL:
	sprintf(s, format, prec, loc);
	tmp = atof(s);		/* fix reverse axes problem when loc == -0.0 */
	if (tmp == 0.0) {
	    strcpy(format, "%.*lf");
	    loc = 0.0;
	    sprintf(s, format, prec, loc);
	}
	break;
    case FORMAT_EXPONENTIAL:
	strcpy(format, "%.*le");
	sprintf(s, format, prec, loc);
	tmp = atof(s);		/* fix reverse axes problem when loc == -0.0 */
	if (tmp == 0.0) {
	    strcpy(format, "%.*le");
	    loc = 0.0;
	    sprintf(s, format, prec, loc);
	}
	break;
    case FORMAT_SCIENTIFIC:
	if (loc != 0.0) {
            exponent = (int) floor(log10(fabs(loc)));
            mantissa = loc/pow(10.0, (double) exponent);
            if (type == LFORMAT_TYPE_EXTENDED) {
	        strcpy(format, "%.*f\\x\\c4\\C\\f{}10\\S%d\\N");
	    } else {
	        strcpy(format, "%.*fx10(%d)");
            }
	    sprintf(s, format, prec, mantissa, exponent);
        } else {
	    strcpy(format, "%.*f");
	    sprintf(s, format, prec, 0.0);
        }
	break;
    case FORMAT_COMPUTING:
        /* As per FORMAT_GENERAL but uses computer notation (K,M,G,...)
         * to give the value in multiples of the powers of 1024
         */       
        if (loc != 0.0) {
            exponent = (int) floor(log2(fabs(loc)));
            if (exponent < 10) {
                exponent = 0;
            } else if (exponent > 80) {
                exponent = 80;
            } else {
                exponent = (int) floor((double) exponent/10)*10;
            }
        } else {
            exponent = 0;
        }

        /* use next prefix if we would get 1024 because
        ** of the print precision requested.  This happens
        ** for values slightly less than 1024.
        */
        sprintf(s, "%.*g", prec, loc/(pow(2.0, exponent)));
        if ((exponent < 80) && (strcmp(s, "1024") == 0)){
	    exponent += 10;
	}

        switch (exponent) {
        case 10: /* kilo */
            comp_prefix = "K";
            break;
        case 20: /* Mega */
            comp_prefix = "M";
            break;
        case 30: /* Giga */
            comp_prefix = "G";
            break;
        case 40: /* Tera */
            comp_prefix = "T";
            break;
        case 50: /* Peta */
            comp_prefix = "P";
            break;
        case 60: /* Exa */
            comp_prefix = "E";
            break;
        case 70: /* Zetta */
            comp_prefix = "Z";
            break;
        case 80: /* Yotta */
            comp_prefix = "Y";
            break;
        default:
            comp_prefix = "";
            break;
        }
        sprintf(s,"%.*g%s", prec, loc/(pow(2.0, exponent)), comp_prefix);
        tmp = atof(s);          /* fix reverse axes problem when loc == -0.0 */
        if (tmp == 0.0) {
            strcpy(format, "%lg");
            loc = 0.0;
            sprintf(s, format, loc);
        }
	break;
    case FORMAT_ENGINEERING:
	if (loc != 0.0) {
            exponent = (int) floor(log10(fabs(loc)));
            if (exponent < -24) {
                exponent = -24;
            } else if (exponent > 24) {
                exponent = 24;
            } else {
                exponent = (int) floor((double) exponent/3)*3;
            }
        } else {
            exponent = 0;
        }
        switch (exponent) {
        case -24: /* yocto */
            eng_prefix = "y";
            break;
        case -21: /* zepto */
            eng_prefix = "z";
            break;
        case -18: /* atto */
            eng_prefix = "a";
            break;
        case -15: /* fempto */
            eng_prefix = "f";
            break;
        case -12: /* pico */
            eng_prefix = "p";
            break;
        case -9: /* nano */
            eng_prefix = "n";
            break;
        case -6: /* micro */
            if (type == LFORMAT_TYPE_EXTENDED) {
                eng_prefix = "\\xm\\f{}";
            } else {
                eng_prefix = "mk";
            }
            break;
        case -3: /* milli */
            eng_prefix = "m";
            break;
        case 3: /* kilo */
            eng_prefix = "k";
            break;
        case 6: /* Mega */
            eng_prefix = "M";
            break;
        case 9: /* Giga */
            eng_prefix = "G";
            break;
        case 12: /* Tera */
            eng_prefix = "T";
            break;
        case 15: /* Peta */
            eng_prefix = "P";
            break;
        case 18: /* Exa */
            eng_prefix = "E";
            break;
        case 21: /* Zetta */
            eng_prefix = "Z";
            break;
        case 24: /* Yotta */
            eng_prefix = "Y";
            break;
        default:
            eng_prefix = "";
            break;
        }
	strcpy(format, "%.*f %s");
	sprintf(s, format, prec, loc/(pow(10.0, exponent)), eng_prefix);
	break;
    case FORMAT_POWER:
        if (loc < 0.0) {
            loc = log10(-loc);
            if (type == LFORMAT_TYPE_EXTENDED) {
                strcpy(format, "-10\\S%.*lf\\N");
            } else {
                strcpy(format, "-10(%.*lf)\\N");
            }
        } else if (loc == 0.0) {
            sprintf(format, "%.*f", prec, 0.0);
        } else {
            loc = log10(loc);
            if (type == LFORMAT_TYPE_EXTENDED) {
                strcpy(format, "10\\S%.*lf\\N");
            } else {
                strcpy(format, "10(%.*lf)\\N");
            }
        }
        sprintf(s, format, prec, loc);
        break;
    case FORMAT_GENERAL:
	strcpy(format, "%.*lg");
	sprintf(s, format, prec, loc);
	tmp = atof(s);
	if (tmp == 0.0) {
	    strcpy(format, "%lg");
	    loc = 0.0;
	    sprintf(s, format, loc);
	}
	break;
    case FORMAT_DDMMYY:
	strcpy(format, "%02d-%02d-%0*d");
	jul_to_cal_and_time(loc, ROUND_DAY, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, d, m, yprec, y);
	break;
    case FORMAT_MMDDYY:
	strcpy(format, "%02d-%02d-%0*d");
	jul_to_cal_and_time(loc, ROUND_DAY, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, m, d, yprec, y);
	break;
    case FORMAT_YYMMDD:
	strcpy(format, "%0*d-%02d-%02d");
	jul_to_cal_and_time(loc, ROUND_DAY, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, yprec, y, m, d);
	break;
    case FORMAT_MMYY:
	strcpy(format, "%02d-%0*d");
	jul_to_cal_and_time(loc, ROUND_MONTH, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, m, yprec, y);
	break;
    case FORMAT_MMDD:
	strcpy(format, "%02d-%02d");
	jul_to_cal_and_time(loc, ROUND_DAY, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, m, d);
	break;
    case FORMAT_MONTHDAY:
	strcpy(format, "%s-%02d");
	jul_to_cal_and_time(loc, ROUND_DAY, &y, &m, &d, &h, &mm, &sec);
	if (m - 1 < 0 || m - 1 > 11) {
	    sprintf(s, format, "???");
	} else {
	    sprintf(s, format, months[m - 1], d);
	}
	break;
    case FORMAT_DAYMONTH:
	strcpy(format, "%02d-%s");
	jul_to_cal_and_time(loc, ROUND_DAY, &y, &m, &d, &h, &mm, &sec);
	if (m - 1 < 0 || m - 1 > 11) {
	    sprintf(s, format, "???");
	} else {
	    sprintf(s, format, d, months[m - 1]);
	}
	break;
    case FORMAT_MONTHS:
	strcpy(format, "%s");
	jul_to_cal_and_time(loc, ROUND_MONTH, &y, &m, &d, &h, &mm, &sec);
	if (m - 1 < 0 || m - 1 > 11) {
	    sprintf(s, format, "???");
	} else {
	    sprintf(s, format, months[m - 1]);
	}
	break;
    case FORMAT_MONTHSY:
	strcpy(format, "%s-%0*d");
	jul_to_cal_and_time(loc, ROUND_MONTH, &y, &m, &d, &h, &mm, &sec);
	if (m - 1 < 0 || m - 1 > 11) {
	    sprintf(s, format, "???");
	} else {
	    sprintf(s, format, months[m - 1], yprec, y);
	}
	break;
    case FORMAT_MONTHL:
	strcpy(format, "%s");
	jul_to_cal_and_time(loc, ROUND_MONTH, &y, &m, &d, &h, &mm, &sec);
	if (m - 1 < 0 || m - 1 > 11) {
	    sprintf(s, format, "???");
	} else {
	    sprintf(s, format, monthl[m - 1]);
	}
	break;
    case FORMAT_DAYOFWEEKS:
	strcpy(format, "%s");
	sprintf(s, format, dayofweekstrs[dayofweek(loc + get_ref_date())]);
	break;
    case FORMAT_DAYOFWEEKL:
	strcpy(format, "%s");
	sprintf(s, format, dayofweekstrl[dayofweek(loc + get_ref_date())]);
	break;
    case FORMAT_DAYOFYEAR:
	strcpy(format, "%d");
        jul_to_cal_and_time(loc, ROUND_DAY, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format,
                1 + (int) (cal_to_jul(y, m, d) - cal_to_jul(y, 1, 1)));
	break;
    case FORMAT_HMS:
	strcpy(format, "%02d:%02d:%02d");
	jul_to_cal_and_time(loc, ROUND_SECOND, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, h, mm, sec);
	break;
    case FORMAT_MMDDHMS:
	strcpy(format, "%02d-%02d %02d:%02d:%02d");
	jul_to_cal_and_time(loc, ROUND_SECOND, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, m, d, h, mm, sec);
	break;
    case FORMAT_MMDDYYHMS:
	strcpy(format, "%02d-%02d-%d %02d:%02d:%02d");
	jul_to_cal_and_time(loc, ROUND_SECOND, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, m, d, y, h, mm, sec);
	break;
    case FORMAT_YYMMDDHMS:
	strcpy(format, "%0*d-%02d-%02d %02d:%02d:%02d");
	jul_to_cal_and_time(loc, ROUND_SECOND, &y, &m, &d, &h, &mm, &sec);
	sprintf(s, format, yprec, y, m, d, h, mm, sec);
	break;
    case FORMAT_DEGREESLON:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%.*lfW");
	} else if (loc > 0.0) {
	    strcpy(format, "%.*lfE");
	} else {
	    strcpy(format, "0");
	}
	sprintf(s, format, prec, loc);
	break;
    case FORMAT_DEGREESMMLON:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%d %.*lf' W");
	} else if (loc > 0.0) {
	    strcpy(format, "%d %.*lf' E");
	} else {
	    strcpy(format, "0 0'");
	}
	y = loc;
	arcmin = (loc - y) * 60.0;
	sprintf(s, format, y, prec, arcmin);
	break;
    case FORMAT_DEGREESMMSSLON:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%d %d' %.*lf\" W");
	} else if (loc > 0.0) {
	    strcpy(format, "%d %d' %.*lf\" E");
	} else {
	    strcpy(format, "0 0' 0\"");
	}
	y = loc;
	arcsec = (loc - y) * 3600.0;
	m = arcsec / 60.0;
	arcsec = (arcsec - m * 60);
	sprintf(s, format, y, m, prec, arcsec);
	break;
    case FORMAT_MMSSLON:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%d' %.*lf\" W");
	} else if (loc > 0.0) {
	    strcpy(format, "%d' %.*lf\" E");
	} else {
	    strcpy(format, "0 0' 0\"");
	}
	y = loc;
	arcsec = (loc - y) * 3600.0;
	m = arcsec / 60.0;
	arcsec = (arcsec - m * 60);
	sprintf(s, format, m, prec, arcsec);
	break;
    case FORMAT_DEGREESLAT:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%.*lfS");
	} else if (loc > 0.0) {
	    strcpy(format, "%.*lfN");
	} else {
	    strcpy(format, "0");
	}
	sprintf(s, format, prec, loc);
	break;
    case FORMAT_DEGREESMMLAT:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%d %.*lf' S");
	} else if (loc > 0.0) {
	    strcpy(format, "%d %.*lf' N");
	} else {
	    strcpy(format, "0 0'");
	}
	y = loc;
	arcsec = (loc - y) * 60.0;
	sprintf(s, format, y, prec, arcsec);
	break;
    case FORMAT_DEGREESMMSSLAT:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%d %d' %.*lf\" S");
	} else if (loc > 0.0) {
	    strcpy(format, "%d %d' %.*lf\" N");
	} else {
	    strcpy(format, "0 0' 0\"");
	}
	y = loc;
	arcsec = (loc - y) * 3600.0;
	m = arcsec / 60.0;
	arcsec = (arcsec - m * 60);
	sprintf(s, format, y, m, prec, arcsec);
	break;
    case FORMAT_MMSSLAT:
	if (loc < 0.0) {
	    loc *= -1.0;
	    strcpy(format, "%d' %.*lf\" S");
	} else if (loc > 0.0) {
	    strcpy(format, "%d' %.*lf\" N");
	} else {
	    strcpy(format, "0 0' 0\"");
	}
	y = loc;
	arcsec = (loc - y) * 3600.0;
	m = arcsec / 60.0;
	arcsec = (arcsec - m * 60);
	sprintf(s, format, m, prec, arcsec);
	break;
    default:
	sprintf(s, format, prec, loc);
	break;
    }

    /* revert to POSIX */
    set_locale_num(FALSE);
    
    return(s);
}

int bin_dump(char *value, int i, int pad)
{
    char *word;
    
    if (i > pad - 1) {
        return 0;
    }
    
    word = value;
    
#ifdef WORDS_BIGENDIAN
    return (((*word)>>i)&0x01);
#else
    switch (pad) {
    case 8:
        return (((*word)>>i)&0x01);
        break;
    case 16:
        if (i < 8) {
            word++;
            return (((*word)>>i)&0x01);
        } else {
            return (((*word)>>(8 - i))&0x01);
        }
        break;
    case 32:
        if (i < 8) {
            word += 2;
            return (((*word)>>i)&0x01);
        } else if (i < 16) {
            word++;
            return (((*word)>>(8 - i))&0x01);
        } else {
            return (((*word)>>(16 - i))&0x01);
        }
        break;
    default:
        return 0;
    }
#endif
}

unsigned char reversebits(unsigned char inword)
{
    int i;
    unsigned char result = 0;
    
    for (i = 0; i <= 7; i++) {
        result |= (((inword)>>i)&0x01)<<(7 - i);
    }
    
    return (result);
}

char *copy_string(char *dest, const char *src)
{
    if (src == dest) {
        ;
    } else if (src == NULL) {
        xfree(dest);
        dest = NULL;
    } else {
        dest = xrealloc(dest, (strlen(src) + 1)*SIZEOF_CHAR);
        strcpy(dest, src);
    }
    return(dest);
}

char *concat_strings(char *dest, const char *src)
{
    if (src != NULL) {
        if (dest == NULL) {
            dest = copy_string(NULL, src);
        } else {
            dest = xrealloc(dest, (strlen(dest) + strlen(src) + 1)*SIZEOF_CHAR);
            if (dest != NULL) {
                strcat(dest, src);
            }
        }
    }
    return(dest);
}

int compare_strings(const char *s1, const char *s2)
{
    if (s1 == NULL && s2 == NULL) {
        return TRUE;
    } else if (s1 == NULL || s2 == NULL) {
        return FALSE;
    } else {
        return (strcmp(s1, s2) == 0);
    }
}

/* location of Grace home directory */
static char grace_home[GR_MAXPATHLEN] = GRACE_HOME;	

char *get_grace_home(void)
{
    return grace_home;
}

void set_grace_home(const char *dir)
{
    strncpy(grace_home, dir, GR_MAXPATHLEN - 1);
}

/* print command */
static char print_cmd[GR_MAXPATHLEN] = GRACE_PRINT_CMD;	

char *get_print_cmd(void)
{
    return print_cmd;
}

void set_print_cmd(const char *cmd)
{
    strncpy(print_cmd, cmd, GR_MAXPATHLEN - 1);
}

/* editor */
static char grace_editor[GR_MAXPATHLEN] = GRACE_EDITOR;	

char *get_editor(void)
{
    return grace_editor;
}

void set_editor(const char *cmd)
{
    strncpy(grace_editor, cmd, GR_MAXPATHLEN - 1);
}

static char help_viewer[GR_MAXPATHLEN] = GRACE_HELPVIEWER;	

char *get_help_viewer(void)
{
    return help_viewer;
}

void set_help_viewer(const char *dir)
{
    strncpy(help_viewer, dir, GR_MAXPATHLEN - 1);
}

/* project file name */
static char docname[GR_MAXPATHLEN] = NONAME;	

char *get_docname(void)
{
    return docname;
}

char *get_docbname(void)
{
    static char buf[GR_MAXPATHLEN];
    char *bufp;
    
    strcpy(buf, mybasename(docname)); 
    bufp = strrchr(buf, '.');
    if (bufp) {
        *(bufp) = '\0';
    }
    
    return buf;
}

void set_docname(const char *s)
{
    if (s != NULL) {
        strncpy(docname, s, GR_MAXPATHLEN - 1);
    } else {
        strcpy(docname, NONAME);
    }
}


void errmsg(const char *buf)
{
#ifdef NONE_GUI
    fprintf(stderr, "%s\n", buf);
#else
    if (inwin) {
        errwin(buf);
    } else {
        fprintf(stderr, "%s\n", buf);
    }
#endif
}

int yesnoterm(char *msg)
{
    return 1;
}

int yesno(char *msg, char *s1, char *s2, char *help_anchor)
{
    if (noask) {
	return 1;
    }
#ifdef NONE_GUI
    return (yesnoterm(msg));
#else
    if (inwin) {
        return (yesnowin(msg, s1, s2, help_anchor));
    } else {
        return (yesnoterm(msg));
    }
#endif
}
 
void stufftext(char *s)
{
#ifdef NONE_GUI
    printf(s);
#else
    if (inwin) {
        stufftextwin(s);
    } else {
        printf(s);
    }
#endif
    /* log results to file */
    if (resfp != NULL) {
	fprintf(resfp, s);
    }
}


char *mybasename(const char *s)
{
    int start, end;
    static char basename[GR_MAXPATHLEN];
    
    s = path_translate(s);
    if (s == NULL) {
        errmsg("Could not translate basename:");
        return "???";
    }
    
    end = strlen(s) - 1;
    
    /* root is a special case */
    if (end == 0 && *s == '/'){
        basename[0] = '/';
        return basename;
    }

    /* strip trailing white space and slashes */
    while (s[end] == '/' || s[end] == ' ' || s[end] == '\t') {
        end--;
    }
    /* find start of basename */
    start = end;
    do {
        start--;
    } while (start >= 0 && s[start] != '/');

    strncpy(basename, s + (start + 1), end - start);
    basename[end - start] = '\0';
    return basename;
}

static char workingdir[GR_MAXPATHLEN];

int set_workingdir(const char *wd)
{
    char buf[GR_MAXPATHLEN];
    
    if (wd == NULL) {
        getcwd(workingdir, GR_MAXPATHLEN - 1);
        if (workingdir[strlen(workingdir)-1] != '/') {
            strcat(workingdir, "/");
        }
        return RETURN_SUCCESS;
    }
    
    strncpy(buf, wd, GR_MAXPATHLEN - 1);
    if (buf[0] == '~') {
        expand_tilde(buf);
    }
    if (chdir(buf) >= 0) {
        strncpy(workingdir, buf, GR_MAXPATHLEN - 1);
        if (workingdir[strlen(workingdir)-1] != '/') {
            strcat(workingdir, "/");
        }
	return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

char *get_workingdir(void)
{
    return workingdir;
}

static char *username = NULL;

void init_username(void)
{
    char *s;

/*
 *     We don't use it for any kind of authentication, so why not let
 *     user to customize her name? :)
 */
    s = getenv("LOGNAME");
    if (s == NULL || s[0] == '\0') {
        s = getlogin();
        if (s == NULL || s[0] == '\0') {
            s = "a user";
        }
    }
    username = copy_string(username, s);
}

char *get_username(void)
{
    return username;
}

static char *userhome = NULL;

void init_userhome(void)
{
    userhome = copy_string(NULL, getenv("HOME"));
    if (userhome == NULL || userhome[strlen(userhome) - 1] != '/') {
        userhome = concat_strings(userhome, "/");
    }
}

char *get_userhome(void)
{
    return userhome;
}

/* TODO this needs some work */
void expand_tilde(char *buf)
{
    char buf2[GR_MAXPATHLEN];

    if (buf[0] == '~') {
	if (strlen(buf) == 1) {
            strcpy(buf, get_userhome());
	} else if (buf[1] == '/') {
            if (strlen(buf) > 2) {
                strcpy(buf2, get_userhome());
	        strcat(buf2, buf + 1);
	        strcpy(buf, buf2);
            } else {
                strcpy(buf, get_userhome());
            }
	} else {
	    char tmp[128], *pp = tmp, *q = buf + 1;
	    struct passwd *pent;

	    while (*q && (*q != '/')) {
		*pp++ = *q++;
	    }
	    *pp = 0;
	    if ((pent = getpwnam(tmp)) != NULL) {
		strcpy(buf2, pent->pw_dir);
		strcat(buf2, "/");
		strcat(buf2, q);
		strcpy(buf, buf2);
	    } else {
		errmsg("No user by that name");
	    }
	}
    }
}

void echomsg(char *msg)
{
    if (inwin) {
#ifndef NONE_GUI
        set_left_footer(msg);
#endif
    } else {
        printf("%s\n", msg);
    }
}

static void update_timestamp(void)
{
    struct tm tm;
    time_t time_value;
    char *str;

    (void) time(&time_value);
    tm = *localtime(&time_value);
    str = asctime(&tm);
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1]= '\0';
    }
    set_plotstr_string(&timestamp, str);
}

void update_app_title(void)
{
#ifndef NONE_GUI
    set_title(mybasename(get_docname()));
#endif
}

/*
 * dirtystate routines
 */

static int dirtystate = 0;
static int dirtystate_lock = FALSE;

void set_dirtystate(void)
{
    if (dirtystate_lock == FALSE) {
        dirtystate++;
        update_timestamp();
        update_app_title();

/*
 * TODO:
 * 	if ( (dirtystate > SOME_LIMIT) || 
 *           (current_time - autosave_time > ANOTHER_LIMIT) ) {
 * 	    autosave();
 * 	}
 */
    }
}

void clear_dirtystate(void)
{
    dirtystate = 0;
    dirtystate_lock = FALSE;
    update_app_title();
}

void lock_dirtystate(flag)
{
    dirtystate_lock = flag;
}

int is_dirtystate(void)
{
    return (dirtystate ? TRUE:FALSE);
}

int system_wrap(const char *string)
{
    return system(string);
}

void msleep_wrap(unsigned int msec)
{
    struct timeval timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = 1000 * (msec % 1000);
    select(0, NULL, NULL, NULL, &timeout);    
}

#ifdef HAVE_SETLOCALE
static int need_locale = FALSE;
static char *system_locale_string, *posix_locale_string;

int init_locale(void)
{
    char *s;
    s = setlocale(LC_NUMERIC, "");
    if (s == NULL) {
        /* invalid/unsupported locale */
        return RETURN_FAILURE;
    } else if (!strcmp(s, "C")) {
        /* don't enable need_locale, since the system locale is C */
        return RETURN_SUCCESS;
    } else {
        system_locale_string = copy_string(NULL, s);
        s = setlocale(LC_NUMERIC, "C");
        posix_locale_string = copy_string(NULL, s);
        need_locale = TRUE;
        return RETURN_SUCCESS;
    }
}

void set_locale_num(int flag)
{
    if (need_locale) {
        if (flag == TRUE) {
            setlocale(LC_NUMERIC, system_locale_string);
        } else {
            setlocale(LC_NUMERIC, posix_locale_string);
        }
    }
}
#else
int init_locale(void)
{
    return RETURN_SUCCESS;
}

void set_locale_num(int flag)
{
}
#endif

/*
 * Build info stuff
 */
long bi_version_id(void)
{
    return BI_VERSION_ID;
}

char *bi_version_string(void)
{
    return BI_VERSION;
}

char *bi_system(void)
{
    return BI_SYSTEM;
}

char *bi_date(void)
{
    return BI_DATE;
}

char *bi_gui(void)
{
    return BI_GUI;
}

#ifdef MOTIF_GUI
char *bi_gui_xbae(void)
{
    return BI_GUI_XBAE;
}
#endif

char *bi_t1lib(void)
{
    return BI_T1LIB;
}

#ifdef HAVE_LIBPNG
char *bi_pnglib(void)
{
    return BI_PNGLIB;
}
#endif

#ifdef HAVE_LIBJPEG
char *bi_libjpeg(void)
{
    return BI_LIBJPEG;
}
#endif

#ifdef HAVE_LIBPDF
char *bi_libpdf(void)
{
    return BI_LIBPDF;
}
#endif

char *bi_ccompiler(void)
{
    return BI_CCOMPILER;
}

#ifdef DEBUG
static int debuglevel = 0;

void set_debuglevel(int level)
{
    debuglevel = level;
}

int get_debuglevel(void)
{
    return debuglevel;
}
#endif

