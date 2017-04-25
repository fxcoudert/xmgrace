/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2001 Grace Development Team
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

/* Wrappers for some math functions */

#include <stdlib.h>

#include "cmath.h"
#include "protos.h"
#include "defines.h"
#include "utils.h"
#include "cephes/cephes.h"

#include "mathstuff.h"

extern double ai_wrap(double x)
{
    double retval, dummy1, dummy2, dummy3;
    (void) airy(x, &retval, &dummy1, &dummy2, &dummy3);
    return retval;
}

extern double bi_wrap(double x)
{
    double retval, dummy1, dummy2, dummy3;
    (void) airy(x, &dummy1, &dummy2, &retval, &dummy3);
    return retval;
}

extern double ci_wrap(double x)
{
    double retval, dummy1;
    (void) sici(x, &dummy1, &retval);
    return retval;
}

extern double si_wrap(double x)
{
    double retval, dummy1;
    (void) sici(x, &retval, &dummy1);
    return retval;
}

extern double chi_wrap(double x)
{
    double retval, dummy1;
    (void) shichi(x, &dummy1, &retval);
    return retval;
}

extern double shi_wrap(double x)
{
    double retval, dummy1;
    (void) shichi(x, &retval, &dummy1);
    return retval;
}

extern double fresnlc_wrap(double x)
{
    double retval, dummy1;
    (void) fresnl(x, &dummy1, &retval);
    return retval;
}

extern double fresnls_wrap(double x)
{
    double retval, dummy1;
    (void) fresnl(x, &retval, &dummy1);
    return retval;
}

extern double iv_wrap(double v, double x)
{
    double retval;
    if (v == 0) {
	retval = i0(x);
    } else if (v == 1) {
	retval = i1(x);
    } else {
	retval = iv(v, x);
    }
    return retval;
}

extern double jv_wrap(double v, double x)
{
    double retval;
    if (v == rint(v)) {
	retval = jn((int) v, x);
    } else {
	retval = jv(v, x);
    }
    return retval;
}

extern double kn_wrap(int n, double x)
{
    double retval;
    if (n == 0) {
	retval = k0(x);
    } else if (n == 1) {
	retval = k1(x);
    } else {
	retval = kn(n, x);
    }
    return retval;
}

extern double yv_wrap(double v, double x)
{
    double retval;
    if (v == rint(v)) {
	retval = yn((int) v, x);
    } else {
	retval = yv(v, x);
    }
    return retval;
}

extern double sqr_wrap(double x)
{
    return x*x;
}

extern double max_wrap(double x, double y)
{
    return MAX2(x, y);
}

extern double min_wrap(double x, double y)
{
    return MIN2(x, y);
}

extern double irand_wrap(int x)
{
    return (double) (lrand48() % x);
}

extern double pi_const(void)
{
    return M_PI;
}

extern double deg_uconst(void)
{
    return M_PI / 180.0;
}

extern double rad_uconst(void)
{
    return 1.0;
}

#define C1 0.1978977093962766
#define C2 0.1352915131768107

extern double rnorm(double mean, double sdev)
{
    double u = drand48();

    return mean + sdev * (pow(u, C2) - pow(1.0 - u, C2)) / C1;
}

extern double fx(double x)
{
    return 1.0 / sqrt(2.0 * M_PI) * exp(-x * x * 0.5);
}


/* Wrapper around the cephes ellpe() call which definition
   is different to the standard definition. See cephes/ellpe.c */
double ellpe_wrap ( double x ) {
    double y;
   
    y = ellpe(1.0 - x);
    return y;
}


/* Wrapper around the cephes ellpk() call which definition
   is different to the standard definition. See cephes/ellpk.c */
double ellpk_wrap ( double x ) {
    double y;
   
    y = ellpk(1.0 - x);
    return y;    
}

double sign_wrap(double x)
{
    return (double) sign(x);
}


/* Double-precision wrapper around humlik(), as the rest of funcs in Grace */
double voigt(double gamma, double sigma, double x)
{
    double v, X, Y;

    if (sigma == 0.0) {
        v = gamma/M_PI/(gamma*gamma + x*x);
    } else {
        X = x/sigma*M_SQRT1_2;
        Y = gamma/sigma*M_SQRT1_2;

        v = humlik(X, Y);
        v /= sigma*sqrt(2*M_PI);
    }
    
    return v;
}
