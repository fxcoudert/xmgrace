/*
 * convcal : dates conversion utility
 * 
 * Copyright (c) 1999 Luc Maisonobe
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
 * This programs allows you to convert dates between calendar format
 * and numerical format.

 * The following command will compile the program :
 *  cc -o convcal convcal.c -lm

 */

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

#define REFDATE "-4713-01-01T12:00:00"

typedef enum   { FMT_iso,
                 FMT_european,
                 FMT_us,
                 FMT_days,
                 FMT_seconds,
                 FMT_nohint
               } Dates_format;

typedef struct { int value;
                 int digits;
               } Int_token;


/*
 * set of functions to convert julian calendar elements
 * with negative years to julian day
 */
static int neg_julian_non_leap (int year)
{
    /* one leap year every four years, leap years : -4713, -4709, ..., -5, -1 */
    return (3 - year) & 3;
}

static long neg_julian_cal_to_jul(int y, int m, int d)
{
    /* day 0       : -4713-01-01
     * day 1721423 :    -1-12-31
     */
    return (1461L*(y + 1L))/4L
        + (m*489)/16 - ((m > 2) ? (neg_julian_non_leap(y) ? 32L : 31L) : 30L)
        + d + 1721057L;

}

static int neg_julian_year_estimate(long n)
{
    /* year bounds : 4n - 6887153 <= 1461y <= 4n - 6885693
     * lower bound reached 31st December of leap years
     * upper bound reached 1st January of leap years
     * the lower bound gives a low estimate of the year
     */
    return (int) ((4L*n - 6887153L)/1461L);
}


/*
 * set of functions to convert julian calendar elements
 * with positive years to julian day
 */
static int pos_julian_non_leap(int year)
{
    /* one leap year every four years, leap years : 4, 8, ..., 1576, 1580 */
    return year & 3;
}

static long pos_julian_cal_to_jul(int y, int m, int d)
{
    /* day 1721424 :     1-01-01
     * day 2299160 :  1582-10-04
     */
    return (1461L*(y -1L))/4L
        + (m*489)/16 - ((m > 2) ? (pos_julian_non_leap(y) ? 32L : 31L) : 30L)
        + d + 1721423L;

}

static int pos_julian_year_estimate(long n)
{
    /* year bounds : 4n - 6885692 <= 1461y <= 4n - 6884232
     * lower bound reached 31st December of leap years
     * upper bound reached 1st January of leap years
     * the lower bound gives a low estimate of the year
     */
    int y = (int) ((4L*n - 6885692L)/1461L);

    /* make sure we stay in the positive model even with our underestimate */
    return (y < 1) ? 1 : y;

}


/*
 * set of functions to convert gregorian calendar elements to julian day
 */
static int gregorian_non_leap(int year)
{
    /* one leap year every four years, except for multiple of 100 that
     * are not also multiple of 400 (so 1600, 1896, 1904, and 2000 are
     * leap years, but 1700, 1800 and 1900 are non leap years
     */
    return (year & 3) || ((year % 100) == 0 && ((year/100 & 3)));
}

static long gregorian_cal_to_jul(int y, int m, int d)
{
    long c;

    /* day 2299161 : 1582-10-15 */
    c = (long) ((y - 1)/100);
    return (1461L*(y - 1))/4 + c/4 - c
        + (m*489)/16 - ((m > 2) ? (gregorian_non_leap(y) ? 32L : 31L) : 30L)
        + d + 1721425L;

}

static int gregorian_year_estimate(long n)
{
    /*
     * year bounds : 400n - 688570288 <= 146097y <= 400n - 688423712
     * lower bound reached on : 1696-12-31, 2096-12-31, 2496-12-31 ...
     * upper bound reached on : 1904-01-01, 2304-01-01, 2704-01-01 ...
     * the lower bound gives a low estimate of the year
     */
    return (int) ((400L*n - 688570288L)/146097L);
}


/*
 * convert calendar elements to Julian day
 */
long cal_to_jul(int y, int m, int d)
{

    long n;

    n = gregorian_cal_to_jul(y, m, d);

    if (n < 2299161L) {
        /* the date belongs to julian calendar */
        n = (y < 0)
            ? neg_julian_cal_to_jul(y, m, d)
            : pos_julian_cal_to_jul(y, m, d);
    }

    return n;

}


/*
 * convert julian day to calendar elements
 */
static void jul_to_some_cal(long n,
                            int (*some_non_leap) (int),
                            long (*some_cal_to_jul) (int, int, int),
                            int (*some_year_estimate) (long),
                            int *y, int *m, int *d)
{
    int non_leap, day_of_year, days_until_end_of_year;

    /* lower estimation of year */
    *y = some_year_estimate(n);
    non_leap = some_non_leap(*y);
    days_until_end_of_year = (int) (some_cal_to_jul(*y, 12, 31) - n);

    while (days_until_end_of_year < 0) {
        /* correction of the estimate */
        (*y)++;
        non_leap = some_non_leap(*y);
        days_until_end_of_year += non_leap ? 365 : 366;
    }

    day_of_year = (non_leap ? 365 : 366) - days_until_end_of_year;

    /* estimate of the month : one too high only on last days of January */
    *m = (16*(day_of_year + (non_leap ? 32 : 31))) / 489;

    /* day of month */
    *d = day_of_year
       - (*m*489)/16 + ((*m > 2) ? (non_leap ? 32 : 31) : 30);
    if (*d < 1) {
        /* no luck, our estimate is false near end of January */
        *m = 1;
        *d += 31;
    }

}


/*
 * convert julian day to calendar elements
 */
void jul_to_cal(long n, int *y, int *m, int *d)
{
    if (n < 1721424L) {
       jul_to_some_cal(n, neg_julian_non_leap,
                       neg_julian_cal_to_jul, neg_julian_year_estimate,
                       y, m, d);
    } else if (n < 2299161L) {
       jul_to_some_cal(n, pos_julian_non_leap,
                       pos_julian_cal_to_jul, pos_julian_year_estimate,
                       y, m, d);
    } else {
       jul_to_some_cal(n, gregorian_non_leap,
                       gregorian_cal_to_jul, gregorian_year_estimate,
                       y, m, d);
    }
}


/*
 * convert julian day and hourly elements to julian day
 */
double jul_and_time_to_jul(long jul, int hour, int min, double sec)
{
    return ((double) jul)
        + (((double) (((hour - 12)*60 + min)*60)) + sec)/86400.0;

}


/*
 * convert calendar and hourly elements to julian day
 */
double cal_and_time_to_jul(int y, int m, int d,
                           int hour, int min, double sec)
{
    return jul_and_time_to_jul (cal_to_jul(y, m, d), hour, min, sec);
}


/*
 * convert julian day to calendar and hourly elements
 * rounding_tol allows to say 1999-12-31T23:59:59.501
 * should be rounded to 2000-01-01T00:00:00.000 assuming
 * it is set to 0.5 second. It is wise to set it according
 * to the display accuracy of seconds.
 */
void jul_to_cal_and_time(double jday, double rounding_tol,
                         int *y, int *m, int *d,
                         int *hour, int *min, double *sec)
{
    long n;

    /* find the time of the day */
    n = (long) floor(jday + 0.5);
    *sec = 24.0*(jday + 0.5 - n);
    *hour = (int) floor(*sec);
    *sec = 60.0*(*sec - *hour);
    *min = (int) floor(*sec);
    *sec = 60.0*(*sec - *min);
    if (*sec + rounding_tol >= 60.0) {
        /* we should round to next minute */
        *sec = 0.0;
        *min += 1;
        if (*min == 60) {
            *min = 0;
            *hour += 1;
            if (*hour == 24) {
                *hour = 0;
                n++;
            }
        }
    }

    /* now find the date */
    jul_to_cal(n, y, m, d);

}

/*
 * check the existence of given calendar elements
 * this includes either number of day in the month
 * and calendars pecularities (year 0 and October 1582)
 */
static int check_date(int century, int wy,
                      Int_token y, Int_token m, Int_token d,
                      long *jul)
{
    int y_expand, y_check, m_check, d_check;

    /* expands years written with two digits only */
    if (y.value >= 0 && y.value < wy && y.digits <= 2) {
        y_expand = century + y.value;
    } else if (y.value >= wy && y.value < 100 && y.digits <= 2) {
        y_expand = century - 100 + y.value;
    } else {
        y_expand = y.value;
    }

    if (m.digits > 2 || d.digits > 2) {
        /* this should be the year instead of either the month or the day */
        return EXIT_FAILURE;
    }

    *jul = cal_to_jul(y_expand, m.value, d.value);
    jul_to_cal(*jul, &y_check, &m_check, &d_check);
    if (y_expand != y_check || m.value != m_check || d.value != d_check) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }

}


/*
 * lexical analyser for float data (knows about fortran exponent
 * markers, return address of following data)
 */
int parse_float(const char* s, double *value, const char **after)
{
    int neg_mant, neg_exp, digits, dot_exp, raw_exp;
    const char *after_dot;

    /* we skip leading whitespace */
    while (isspace(*s)) {
        s++;
    }

    /* sign */
    if (*s == '-') {
       neg_mant = 1;
       s++;
    } else {
        neg_mant = 0;
    }

    /* mantissa */
    digits = 0;
    *value = 0.0;
    while (isdigit(*s)) {
        *value = *value*10.0 + (*s++ - '0');
        digits++;
    }
    if (*s == '.') {
        after_dot = ++s;
        while (isdigit(*s)) {
            *value = *value*10.0 + (*s++ - '0');
            digits++;
        }
        dot_exp = after_dot - s;
    } else {
        dot_exp = 0;
    }
    if (digits == 0) {
        /* there should be at least one digit (either before or after dot) */
        return EXIT_FAILURE;
    }

    /* exponent (d and D are fortran exponent markers) */
    raw_exp = 0;
    if (*s == 'e' || *s == 'E' || *s == 'd' || *s == 'D') {
        s++;
        if (*s == '-') {
            neg_exp = 1;
            s++;
        } else {
            neg_exp = 0;
            if (*s == '+') {
                s++;
            }
        }
        while (isdigit(*s)) {
            raw_exp = raw_exp*10 + (*s++ - '0');
        }
        if (neg_exp) {
            raw_exp = -raw_exp;
        }
    }

    /* read float */
    *value = (neg_mant ? -(*value) : (*value)) * pow (10.0, dot_exp + raw_exp);

    if (after != NULL) {
        /* the caller wants to know what follows the float number */
        *after = s;
    }

    return EXIT_SUCCESS;

}


/*
 * lexical analyser for calendar dates
 * return the number of read elements, or -1 on failure
 */
static int parse_calendar_date(const char* s,
                               Int_token tab [5], double *sec)
{
    int i, waiting_separator, negative;

    negative = 0;
    waiting_separator = 0;
    i = 0;
    while (i < 5) {
        /* loop from year to minute elements : all integers */

        switch (*s) {
          case '\0': /* end of string */
              return i;

          case ' ' : /* repeatable separator */
              s++;
              negative = 0;
              break;

          case '/' : case ':' : case '.' : case 'T' : /* non-repeatable separator */
              if (waiting_separator) {
                  if ((*s == 'T') && (i != 3)) {
                      /* the T separator is only allowed between date
                         and time (mainly for iso8601) */
                      return -1;
                  }
                  s++;
                  negative = 0;
                  waiting_separator = 0;
              } else {
                  return -1;
              }
              break;

          case '-' : /* either separator or minus sign */
              s++;
              if (waiting_separator) {
                  negative = 0;
                  waiting_separator = 0;
              } else if ((*s >= '0') && (*s <= '9')) {
                  negative = 1;
              } else {
                  return -1;
              }
              break;

          case '0' : case '1' : case '2' : case '3' : case '4' :
          case '5' : case '6' : case '7' : case '8' : case '9' : /* digit */
              tab[i].value  = ((int) *s) - '0';
              tab[i].digits = 1;
              while (isdigit(*++s)) {
                  tab[i].value = tab[i].value*10 + (((int) *s) - '0');
                  tab[i].digits++;
              }
              if (negative) {
                  tab[i].value = -tab[i].value;
              }
              i++;
              negative = 0;
              waiting_separator = 1;
              break;

          default  :
              return -1;

        }

    }

    while (isspace(*s)) {
        s++;
    }
    if (*s == '\0') {
        return 5;
    }

    if ((*s == '/') || (*s == ':') || (*s == '.') || (*s == '-')) {
        /* this was the seconds separator */
        s++;

        /* seconds are read in float format */
        if (parse_float(s, sec, &s) == EXIT_SUCCESS) {
            while (isspace(*s)) {
                s++;
            }
            if (*s == '\0') {
                return 6;
            }
        }

    }

    /* something is wrong */
    return -1;

}


/*
 * parse a date given either in calendar or numerical format
 */
int parse_date(const char* s, int century, int wy, Dates_format preferred,
               double *jul, Dates_format *recognized)
{
    int i, n;
    int ky, km, kd;
    static Dates_format trials [] = {FMT_nohint, FMT_iso, FMT_european, FMT_us};
    Int_token tab [5];
    long j;
    double sec;
    const char *after;

    /* first guess : is it a date in calendar format ? */
    n = parse_calendar_date(s, tab, &sec);
    switch (n) {
        /* we consider hours, minutes and seconds as optional items */
      case -1 : /* parse error */
          break;

      case 3 :
          tab[3].value  = 0; /* adding hours */
          tab[3].digits = 1;

      case 4 :
          tab[4].value  = 0; /* adding minutes */
          tab[4].digits = 1;

      case 5 :
          sec = 0.0;  /* adding seconds */

      case 6 :
          /* we now have a complete date */

          /* try the user's choice first */
          trials[0] = preferred;

          for (i = 0; i < 4; i++) {
              if (trials[i] == FMT_iso) {
                  /* YYYY-MM-DD */
                  ky = 0;
                  km = 1;
                  kd = 2;
              } else if (trials[i] == FMT_european) {
                  /* DD/MM/(YY)YY */
                  ky = 2;
                  km = 1;
                  kd = 0;
              } else if (trials[i] == FMT_us) {
                  /* MM/DD/(YY)YY */
                  ky = 2;
                  km = 0;
                  kd = 1;
              } else {
                  /* the user didn't choose a calendar format */
                  continue;
              }

              if (check_date(century, wy, tab[ky], tab[km], tab[kd], &j)
                  == EXIT_SUCCESS) {
                  *jul = jul_and_time_to_jul(j, tab[3].value, tab[4].value,
                                             sec);
                  *recognized = trials[i];
                  return EXIT_SUCCESS;
              }
          }
          break;

      default :
          /* probably a julian date (integer if n == 1, real otherwise) */
          break;

    }

    /* second guess : is it a date in numerical format ? */
    if (parse_float(s, jul, &after) == EXIT_SUCCESS) {
        while (isspace(*after)) {
            after++;
        }
        if (*after == '\0') {
            if (preferred == FMT_seconds) {
                *recognized = FMT_seconds;
                *jul /= 86400.0;
            } else {
                *recognized = FMT_days;
            }
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;

}

int convert_and_write(const char *s,
                      int century, int wy, double reference_date,
                      Dates_format input_format, Dates_format output_format)
{
    Dates_format recognized;
    int    y, m, d, hour, min;
    double jul;
    double sec;

    if (parse_date(s, century, wy, input_format, &jul, &recognized)
        != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if (recognized == FMT_days || recognized == FMT_seconds) {
        /* the parsed value is relative to the reference date */
        jul += reference_date;
    }

    if (output_format == FMT_nohint) {
        /* choose a format that really convert calendar and numerical */
        if ((recognized == FMT_days) || (recognized == FMT_seconds)) {
            output_format = FMT_iso;
        } else {
            output_format = FMT_days;
        }
    }

    switch (output_format) {
      case FMT_iso :
          jul_to_cal_and_time(jul, 0.0005, &y, &m, &d, &hour, &min, &sec);
          fprintf(stdout, "%04d-%02d-%02dT%02d:%02d:%06.3f\n",
                  y, m, d, hour, min, sec);
          break;

      case FMT_european :
          jul_to_cal_and_time(jul, 0.0005, &y, &m, &d, &hour, &min, &sec);
          fprintf(stdout, "%02d/%02d/%04d %02d:%02d:%06.3f\n",
                  d, m, y, hour, min, sec);
          break;

      case FMT_us :
          jul_to_cal_and_time(jul, 0.0005, &y, &m, &d, &hour, &min, &sec);
          fprintf(stdout, "%02d/%02d/%04d %02d:%02d:%06.3f\n",
                  m, d, y, hour, min, sec);
          break;

      case FMT_days :
          fprintf(stdout, "%17.8f\n", jul - reference_date);
          break;

      case FMT_seconds :
          fprintf(stdout, "%17.3f\n", 86400.0 * (jul - reference_date));
          break;

      default :
          fprintf(stderr, "%s:%d: internal error\n", __FILE__, __LINE__);
          break;

    }

    return EXIT_SUCCESS;

}

int string_equal(const char *c1, const char *c2)
{
    return (strlen(c1) == strlen(c2)) && (strcmp(c1, c2) == 0);
}

int parse_format(const char *s, Dates_format *f)
{

    if (string_equal(s, "iso")) {
        *f = FMT_iso;
    } else if (string_equal(s, "european")) {
        *f = FMT_european;
    } else if (string_equal(s, "us")) {
        *f = FMT_us;
    } else if (string_equal(s, "days")) {
        *f = FMT_days;
    } else if (string_equal(s, "seconds")) {
        *f = FMT_seconds;
    } else if (string_equal(s, "nohint")) {
        *f = FMT_nohint;
    } else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}

/*
 * expand a line buffer
 */
static void expand_line_buffer(char **adrBuf, int *ptrSize, char **adrPtr)
{
    char *newbuf;
    int   newsize;

    newsize = *ptrSize + 512;
    newbuf = (char *) malloc(newsize);
    if (newbuf == 0) {
        fprintf(stderr, "Insufficient memory for line");
        exit (EXIT_FAILURE);
    }

    if (*ptrSize == 0) {
        /* this is the first time through */
        if (adrPtr) {
            *adrPtr = newbuf;
        }
    } else {
        /* we are expanding an existing line */
        strncpy(newbuf, *adrBuf, *ptrSize);
        if (adrPtr) {
            *adrPtr += newbuf - *adrBuf;
        }
        free(*adrBuf);
    }

    *adrBuf  = newbuf;
    *ptrSize = newsize;    

}

/*
 * help message
 */
static void usage (FILE *stream, const char *progname)
{
    fprintf (stream,
             "%s reads the dates either on the command line or in the\n", progname);
    fprintf (stream,
             "standard input if the command line contains no date. The following\n");
    fprintf (stream,
             "date formats are supported (hour, minutes and seconds are always optional):\n");
    fprintf (stream,
             "\n");
    fprintf (stream,
             "iso       : 1999-12-31T23:59:59.999\n");
    fprintf (stream,
             "european  : 31/12/1999 23:59:59.999 or 31/12/99 23:59:59.999\n");
    fprintf (stream,
             "us        : 12/31/1999 23:59:59.999 or 12/31/99 23:59:59.999\n");
    fprintf (stream,
             "days      : 123456.789\n");
    fprintf (stream,
             "seconds   : 123456.789\n");
    fprintf (stream,
             "\n");
    fprintf (stream,
             "The formats are tried in the following order : users's choice,\n");
    fprintf (stream,
             "iso, european and us (there is no ambiguity between calendar\n");
    fprintf (stream,
             "formats and numerical formats and therefore no order is specified\n");
    fprintf (stream,
             "for them). The default user's choice (nohint) does nothing so the\n");
    fprintf (stream,
             "following formats of the list are used ; the main use of user's\n");
    fprintf (stream,
             "choice is to put another format before the other ones. The\n");
    fprintf (stream,
             "separators between various fields can be any characters in the set:\n");
    fprintf (stream,
             "\" :/.-T\". One or more spaces act as one separator, other characters\n");
    fprintf (stream,
             "can not be repeated, the T separator is allowed only between date and\n");
    fprintf (stream,
             "time, mainly for iso8601. So the string \"1999-12 31:23-59\" is allowed\n");
    fprintf (stream,
             "(but not recommended).  The '-' character is used both as a\n");
    fprintf (stream,
             "separator (it is traditionally used in iso8601 format) and as the\n");
    fprintf (stream,
             "unary minus (for dates in the far past or for numerical\n");
    fprintf (stream,
             "dates). When the year is between 0 and 99 and is written with two\n");
    fprintf (stream,
             "or less digits, it is mapped to the era beginning at wrap year and\n");
    fprintf (stream,
             "ending at wrap year + 99 as follows :\n");
    fprintf (stream,
             "  [wy ; 99] -> [ wrap_year ; 100*(1 + wrap_year/100) - 1 ]\n");
    fprintf (stream,
             "  [00 ; wy-1] -> [ 100*(1 + wrap_year/100) ; wrap_year + 99]\n");
    fprintf (stream,
             "so for example if the wrap year is set to 1950 (which is the default\n");
    fprintf (stream,
             "value), then the mapping is :\n");
    fprintf (stream,
             "   range [00 ; 49] is mapped to [2000 ; 2049]\n");
    fprintf (stream,
             "   range [50 ; 99] is mapped to [1950 ; 1999]\n");
    fprintf (stream,
             "this is reasonably Y2K compliant and is consistent with current use.\n");
    fprintf (stream,
             "Specifying year 1 is still possible using more than two digits as\n");
    fprintf (stream,
             "follows : \"0001-03-04\" is unambiguously March the 4th, year 1, even\n");
    fprintf (stream,
             "if the user's choice is us format. However using two digits only is\n");
    fprintf (stream,
             "not recommended (we introduce a 2050 bug here so this feature\n");
    fprintf (stream,
             "should be removed at some point in the future ;-)\n");
    fprintf (stream,
             "\n");
    fprintf (stream,
             "Numerical dates (days and seconds formats) can be specified using\n");
    fprintf (stream,
             "integral, real or exponential formats (the 'd' and 'D' exponant\n");
    fprintf (stream,
             "markers from fortran are supported in addition to 'e' and 'E').\n");
    fprintf (stream,
             "They are computed according to a customizable reference date.\n");
    fprintf (stream,
             "The default value is given by the REFDATE constant in the source file.\n");
    fprintf (stream,
             "You can change this value as you want before compiling, and you can\n");
    fprintf (stream,
             "change it at will using the -r command line option. The default\n");
    fprintf (stream,
             "value in the distributed file is \"-4713-01-01T12:00:00\", it is a\n");
    fprintf (stream,
             "classical reference for astronomical events (note that the '-' is\n");
    fprintf (stream,
             "used here both as a unary minus and as a separator).\n");
    fprintf (stream,
             "\n");
    fprintf (stream,
             "The program can be used either for Denys's and gregorian\n");
    fprintf (stream,
             "calendars. It does not take into account leap seconds : you can\n");
    fprintf (stream,
             "think it works only in International Atomic Time (TAI) and not in\n");
    fprintf (stream,
             "Coordinated Unified Time (UTC) ...  Inexistant dates are detected,\n");
    fprintf (stream,
             "they include year 0, dates between 1582-10-05 and 1582-10-14,\n");
    fprintf (stream,
             "February 29th of non leap years, months below 1 or above 12, ...\n");
    fprintf (stream,
             "\n");
    fprintf (stream,
             "The following command line options are supported. Apart from the -h\n");
    fprintf (stream,
             "flag, all of these options can be used several times, each new\n");
    fprintf (stream,
             "value overriding the preceding one.\n");
    fprintf (stream,
             "\n");
    fprintf (stream,
             "-i format : set user's choice for input format, supported formats are\n");
    fprintf (stream,
             "            iso, european, us, days, seconds and nohint.\n");
    fprintf (stream,
             "            At the beginning the input format is nohint, which means\n");
    fprintf (stream,
             "            the program try to guess the format by itself, if the\n");
    fprintf (stream,
             "            user's choice does not allow to parse the date, other\n");
    fprintf (stream,
             "            formats are tried\n");
    fprintf (stream,
             "-o format : force output format, supported formats are\n");
    fprintf (stream,
             "            iso, european, us, days, seconds and nohint.\n");
    fprintf (stream,
             "            At the beginning, the output format is nohint, which means\n");
    fprintf (stream,
             "            the program uses days format for dates read in any\n");
    fprintf (stream,
             "            calendar format and uses iso8601 for dates read in\n");
    fprintf (stream,
             "            numerical format\n");
    fprintf (stream,
             "-r date   : set reference date (the date is read using the current\n");
    fprintf (stream,
             "            input format) at the beginning the reference is set\n");
    fprintf (stream,
             "            according to the REFDATE constant below.\n");
    fprintf (stream,
             "-w year   : set the wrap year to year\n");
    fprintf (stream,
             "-h        : prints this help message on stderr and exits successfully\n");

    exit(0);
}

/*
 * driver program
 */
int main(int argc, char *argv[])
{
    double reference_date;
    Dates_format input_format;
    Dates_format output_format;
    Dates_format recognized;
    int    century, wy;

    int    i, j, converted;
    int    retval = EXIT_SUCCESS;

    /* initial values */
    century = 2000;
    wy      = 50;
    if (parse_date(REFDATE, century, wy, FMT_iso, &reference_date, &recognized)
        != EXIT_SUCCESS) {
        fprintf(stderr,
                "%s: unable to parse compiled in reference date (%s) !\n",
                argv[0], REFDATE);
        return EXIT_FAILURE;
    }
    input_format  = FMT_nohint;
    output_format = FMT_nohint;

    /* command line parsing */
    converted = 0;
    for (i = 1; i < argc; i = j) {
        j = i + 1;

        if (string_equal(argv[i], "-i")) {
            /* input format */

            if (argc < j + 1) {
                fprintf(stderr, "%s: missing argument for %s flag\n",
                        argv[0], argv[i]);
                return EXIT_FAILURE;
            }

            if (parse_format(argv[j], &input_format) != EXIT_SUCCESS) {
                fprintf(stderr, "%s: unknown date format \"%s\"\n",
                        argv[0], argv[j]);
                return EXIT_FAILURE;
            }

            ++j;

        } else if (string_equal(argv[i], "-o")) {
            /* output format */

            if (argc < j + 1) {
                fprintf(stderr, "%s: missing argument for %s flag\n",
                        argv[0], argv[i]);
                return EXIT_FAILURE;
            }

            if (parse_format(argv[j], &output_format) != EXIT_SUCCESS) {
                fprintf(stderr, "%s: unknown date format \"%s\"\n",
                        argv[0], argv[j]);
                return EXIT_FAILURE;
            }

            ++j;

        } else if (string_equal(argv[i], "-r")) {
            /* reference date */

            if (argc < j + 1) {
                fprintf(stderr,
                        "%s: missing argument for %s flag\n",
                        argv[0], argv[i]);
                return EXIT_FAILURE;
            }

            if (parse_date(argv[j], century, wy, input_format,
                           &reference_date, &recognized) != EXIT_SUCCESS) {
                fprintf(stderr,
                        "%s: unable to parse reference date (%s)\n",
                        argv[0], REFDATE);
                return EXIT_FAILURE;
            }

            ++j;

        } else if (string_equal(argv[i], "-w")) {
            /* wrap year */

            if (argc < j + 1) {
                fprintf(stderr,
                        "%s: missing argument for %s flag\n",
                        argv[0], argv[i]);
                return EXIT_FAILURE;
            }

            century = 100*(1 + atoi(argv[j])/100);
            wy      = atoi(argv[j]) - (century - 100);

            ++j;

        } else if (string_equal(argv[i], "-h")) {
            /* help */
            usage(stderr, argv[0]);
        } else {
            /* date */
            converted = 1;
            if (convert_and_write (argv[i], century, wy, reference_date,
                                   input_format, output_format)
                != EXIT_SUCCESS) {
                fprintf(stderr,
                        "%s: unable to parse date (%s)\n",
                        argv[0], argv[i]);
                retval = EXIT_FAILURE;
            }

        }

    }


    if (converted == 0) {
        /* there was no date in the command line : use standard input */
        int   reading  = 1;
        int   num_line = 0;
        int   size = 0;
        char *line = 0;
        expand_line_buffer (&line, &size, NULL);

        while (reading) {
            /* input lines reading loop */
            char *cursor = line + 1;
            ++num_line;
            line[0] = ' ';
            line[1] = '\0';

            while (reading != 0 && *(cursor - 1) != '\n') {
                /* trying to read until end of line */

                if (size - (cursor - line) < 2) {
                    /* there is not enough room left */
                    expand_line_buffer(&line, &size, &cursor);
                }

                if (fgets(cursor, size - (cursor - line), stdin) == NULL) {
                    if (cursor == line + 1) {
                        /* we are at end */
                        reading = 0;
                    } else {
                        /* something went wrong */
                        fprintf(stderr,
                                "%s: read error on line %d: %s\n",
                                argv[0], num_line, line + 1);
                        retval = EXIT_FAILURE;
                    }
                } else {
                    /* something has been successfully read */
                    cursor += strlen(cursor);
                }
            }
            *(cursor - 1) = '\0';

            if (reading) {
                /* converting the date */
                if (convert_and_write (line + 1, century, wy, reference_date,
                                       input_format, output_format)
                    != EXIT_SUCCESS) {
                    fprintf(stderr,
                            "%s: unable to parse date (%s)\n",
                            argv[0], line + 1);
                    retval = EXIT_FAILURE;
                }
            }
        }
    }

    return retval;

}
