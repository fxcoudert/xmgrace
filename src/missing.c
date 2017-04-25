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

#endif /* HAVE_POPEN */
