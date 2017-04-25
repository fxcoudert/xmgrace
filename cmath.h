/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2000 Grace Development Team
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

/* cmath.h - replacement for math.h or missing in libm functions */

#include <config.h>

#if defined(HAVE_MATH_H)
#  include <math.h>
#endif
#if defined(HAVE_FLOAT_H)
#  include <float.h>
#endif
#if defined(HAVE_IEEEFP_H)
#  include <ieeefp.h>
#endif

#ifndef __GRACE_SOURCE_

#ifndef MACHEP
extern double MACHEP;
#endif

#ifndef UFLOWTHRESH
extern double UFLOWTHRESH;
#endif

#ifndef MAXNUM
extern double MAXNUM;
#endif

#endif /* __GRACE_SOURCE_ */

#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

#ifndef M_SQRT2
#  define M_SQRT2     1.41421356237309504880      /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#  define M_SQRT1_2   0.70710678118654752440      /* 1/sqrt(2) */
#endif

#ifndef M_SQRT1_3
#  define M_SQRT1_3   0.57735026918962576451      /* 1/sqrt(3) */
#endif

#ifndef HAVE_HYPOT
#  define hypot(x, y) sqrt((x)*(x) + (y)*(y))
#endif

extern double round ( double x );
#ifndef HAVE_RINT
#  define rint round
#else
#  ifndef HAVE_RINT_DECL
extern double rint ( double x );
#  endif
#endif

#ifndef HAVE_CBRT_DECL
extern double cbrt ( double x );
#endif

/* Cygnus gnuwin32 has the log2 macro */
#ifdef log2
#  undef log2
#endif

#ifndef HAVE_LOG2_DECL
extern double log2 ( double x );
#endif

#ifndef HAVE_LGAMMA
extern int sgngam;
#  define lgamma lgam
#  define signgam sgngam
extern double lgam ( double x );
#else
#  ifndef HAVE_LGAMMA_DECL
extern double lgamma ( double x );
#  endif
#  ifndef HAVE_SIGNGAM_DECL
extern int signgam;
#  endif
#  define lgam lgamma
#  define sgngam signgam
#endif

#ifndef HAVE_ACOSH_DECL
extern double acosh ( double x );
#endif

#ifndef HAVE_ASINH_DECL
extern double asinh ( double x );
#endif

#ifndef HAVE_ATANH_DECL
extern double atanh ( double x );
#endif

#ifndef HAVE_ERF_DECL
extern double erf ( double x );
#endif

#ifndef HAVE_ERFC_DECL
extern double erfc ( double x );
#endif

#ifndef HAVE_Y0_DECL
extern double y0 ( double x );
#endif
#ifndef HAVE_Y1_DECL
extern double y1 ( double x );
#endif
#ifndef HAVE_YN_DECL
extern double yn ( int n, double x );
#endif
#ifndef HAVE_J0_DECL
extern double j0 ( double x );
#endif
#ifndef HAVE_J1_DECL
extern double j1 ( double x );
#endif
#ifndef HAVE_JN_DECL
extern double jn ( int n, double x );
#endif

/* isfinite is a macro */
#ifdef isfinite
#  define HAVE_ISFINITE_MACRO
#endif

#ifndef HAVE_FINITE
#  define finite isfinite
#  if !defined(HAVE_ISFINITE_DECL) && !defined(HAVE_ISFINITE_MACRO)
extern int isfinite ( double x );
#  endif
#else
#  ifndef HAVE_FINITE_DECL
extern int finite ( double x );
#  endif
#endif

/* isnan is a macro */
#ifdef isnan
#  define HAVE_ISNAN_MACRO
#endif

#if !defined(HAVE_ISNAN_DECL) && !defined(HAVE_ISNAN_MACRO)
extern int isnan ( double x );
#endif
