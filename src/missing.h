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

#endif /* __MISSING_H_ */
