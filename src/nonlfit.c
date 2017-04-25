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

/* 
 *
 * nonlinear curve fitting
 *
 */

#include <config.h>

#include <cmath.h>

#include "globals.h"
#include "graphs.h"
#include "utils.h"
#include "parser.h"
#include "protos.h"

/* Needed only for `integer' and `doublereal' definitions */
#include "f2c.h"

extern int lmdif_(U_fp, integer *, integer *, doublereal *,
            doublereal *, doublereal *, doublereal *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, integer *, integer *, doublereal *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *);

static double *xp, *yp, *y_saved;
static double *wts;
static char *ra;

int lmdif_drv(U_fp fcn, integer m, integer n, doublereal *x, 
	doublereal *fvec, doublereal *tol, integer *iwa, 
	doublereal *wa, integer lwa, integer nsteps);

void a_to_parms (double *a);
void parms_to_a (double *a);

void reset_nonl(void)
{
    int i;
    
    for (i = 0; i < MAXPARM; i++) {
    	nonl_parms[i].value = 1.0;
    	nonl_parms[i].constr = FALSE;
    	nonl_parms[i].min = 1.0;
    	nonl_parms[i].max = 1.0;
    }
    
    nonl_opts.title   = copy_string(nonl_opts.title, "A fit");
    nonl_opts.formula = copy_string(nonl_opts.formula, "y = ");
    nonl_opts.parnum = 0;
    nonl_opts.tolerance = 0.01;

    return;
}

void initialize_nonl(void)
{
    nonl_opts.title   = NULL;
    nonl_opts.formula = NULL;
    reset_nonl();
}

void a_to_parms (double *a)
{
    int i;
    double t;
    
    for (i = 0; i < nonl_opts.parnum; i++) {
    	if (nonl_parms[i].constr) {
    	    /* map (-inf, inf) to (-1, 1) */
    	    t = a[i]/(abs(a[i]) + 1.0);
    	    
    	    /* map (-1, 1) to (nonl_lowb[i], nonl_uppb[i]) */
    	    nonl_parms[i].value = (nonl_parms[i].min + nonl_parms[i].max)/2.0 +
    	    	    (nonl_parms[i].max - nonl_parms[i].min)/2.0 * t;
    	} else {
    	    nonl_parms[i].value = a[i];
    	}
    }
}


void parms_to_a (double *a)
{
    int i;
    double t;
        
    double eps  = 1.e-6;
       
    for (i = 0; i < nonl_opts.parnum; i++) {
    	if (nonl_parms[i].constr) {
    	    t = (nonl_parms[i].value - (nonl_parms[i].min + nonl_parms[i].max)/2.0)/
    	    	  ((nonl_parms[i].max - nonl_parms[i].min)/2.0);
    	    if (t < -(1.0 - eps)){
    	    	t = -(1.0 - eps);
    	    	nonl_parms[i].value = (nonl_parms[i].min + nonl_parms[i].max)/2.0 +
    	    	    (nonl_parms[i].max - nonl_parms[i].min)/2.0 * t;
    	    }
    	    if (t > (1.0 - eps)){
    	    	t = (1.0 - eps);
    	    	nonl_parms[i].value = (nonl_parms[i].min + nonl_parms[i].max)/2.0 +
    	    	    (nonl_parms[i].max - nonl_parms[i].min)/2.0 * t;
    	    }
    	    a[i] = t/(1.0 - abs(t)); 
    	} else {
    	    a[i] = nonl_parms[i].value;
    	}
    }
}


void fcn(int * m, int * n, double * x, double * fvec,
		int * iflag)
{
    int errpos;
    int i;

    a_to_parms(x);

    errpos = scanner(nonl_opts.formula);
    if (errpos) {
	errmsg("error in fcn");
	*iflag = -1;
	return;
    }
    for (i = 0; i < *m; ++i) {
    	fvec[i] = yp[i] - y_saved[i];
    }
    /* apply weigh function, if any */
    if (wts != NULL) {
         for (i = 0; i < *m; ++i) {
             fvec[i] *= sqrt(fabs(wts[i]));
         }
    }
    /* apply restriction, if any */
    if (ra != NULL) {
         for (i = 0; i < *m; ++i) {
             fvec[i] *= ra[i];
         }
    }
}

/*
 * find correlation coefficient
 */
int correlation(double *x, double *y, int n, double *cor)
{
    double xbar, xsd;
    double ybar, ysd;
    int i;

    *cor = 0.0;
    if (n < 2) {
        return RETURN_FAILURE;
    }
    
    stasum(x, n, &xbar, &xsd);
    stasum(y, n, &ybar, &ysd);
    if (xsd == 0.0 || ysd == 0.0) {
        return RETURN_FAILURE;
    }
    
    for (i = 0; i < n; i++) {
        *cor += (x[i] - xbar)*(y[i] - ybar);
    }
    *cor /= ((n - 1)*xsd*ysd);
    
    return RETURN_SUCCESS;
}

int do_nonlfit(int gno, int setno, double *warray, char *rarray, int nsteps)
{
    int info = -1;
    double *fvec, *wa;
    int i, n;
    integer lwa, iwa[MAXPARM];
    double a[MAXPARM];
    int parnum = nonl_opts.parnum;
    char buf[128];
    double cor, chisq, rms_pe, ysq, theil;
    int rms_ok;

    if (set_parser_setno(gno, setno) != RETURN_SUCCESS) {
	return RETURN_FAILURE;
    }
    n = getsetlength(gno, setno);
    
    lwa = (integer) n * parnum + 5 * parnum + n;
        
    fvec = xcalloc(n, SIZEOF_DOUBLE);
    if (fvec == NULL) {
	return RETURN_FAILURE;
    }
      
    y_saved = xcalloc(n, SIZEOF_DOUBLE);
    if (y_saved == NULL) {
	xfree(fvec);
	return RETURN_FAILURE;
    }

    wa = xcalloc(lwa, sizeof(doublereal));
    if (wa == NULL) {
	xfree(y_saved);
	xfree(fvec);
	return RETURN_FAILURE;
    }

    stufftext("Fitting with formula: ");
    stufftext(nonl_opts.formula);
    stufftext("\n");
    stufftext("Initial guesses:\n");
    for (i = 0; i < nonl_opts.parnum; i++) {
        sprintf(buf, "\ta%1d = %g\n", i, nonl_parms[i].value);
        stufftext(buf);
    }
    sprintf(buf, "Tolerance = %g\n", nonl_opts.tolerance);
    stufftext(buf);
    
    xp = getx(gno, setno);
    yp = gety(gno, setno);
    
    for (i = 0; i < n; ++i) {
    	y_saved[i] = yp[i];
    }
   
    ra = rarray;
    wts = warray;

    parms_to_a(a);
    
    info = lmdif_drv((U_fp) fcn, (integer) n, (integer) parnum, a, fvec, 
        &nonl_opts.tolerance, iwa, wa, lwa, (integer) nsteps);
    
    a_to_parms(a);
    
    correlation(yp, y_saved, n, &cor);
    
    chisq = 0.0;
    theil = 0.0;
    ysq = 0.0;
    rms_ok = TRUE;
    for (i = 0; i < n; ++i) {
    	chisq += (yp[i] - y_saved[i])*(yp[i] - y_saved[i]);
        ysq += (y_saved[i]*y_saved[i]);
    	theil += (yp[i] - y_saved[i])*(yp[i] - y_saved[i]);
    	if (y_saved[i] == 0.0) {
            rms_ok = FALSE;
        }
    }
    theil = sqrt(theil/ysq);

    rms_pe = 0.0;
    if (rms_ok) {
        for (i = 0; i < n; ++i) {
    	    rms_pe += (yp[i] - y_saved[i])*(yp[i] - y_saved[i])/
                (y_saved[i]*y_saved[i]);
        }
        rms_pe = sqrt(rms_pe/n);
    }

    for (i = 0; i < n; ++i) {
    	yp[i] = y_saved[i];
    }
    
    xfree(y_saved);
    xfree(fvec);
    xfree(wa);

    if (info >= 0 && info <= 7) {
        char *s;
        switch (info) {
        case 0:
            s = "Improper input parameters.\n";
            break;
        case 1:
            s = "Relative error in the sum of squares is at most tol.\n";
            break;
        case 2:
            s = "Relative error between A and the solution is at most tol.\n";
            break;
        case 3:
            s = "Relative error in the sum of squares and A and the solution is at most tol.\n";
            break;
        case 4:
            s = "Fvec is orthogonal to the columns of the jacobian to machine precision.\n";
            break;
        case 5:
            s = "\n";
            break;
        case 6:
            s = "Tol is too small. No further reduction in the sum of squares is possible.\n";
            break;
        case 7:
            s = "Tol is too small. No further improvement in the approximate solution A is possible.\n";
            break;
        default:
            s = "\n";
            errmsg("Internal error in do_nonlfit()");
            break;
        }
        stufftext(s);
    }
    
    if ((info > 0 && info < 4) || (info == 5)) {
        stufftext("Computed values:\n");
        for (i = 0; i < nonl_opts.parnum; i++) {
            sprintf(buf, "\ta%1d = %g\n", i, nonl_parms[i].value);
            stufftext(buf);
        }
        stufftext("\n");
        sprintf(buf, "Chi-square: %g\n", chisq);
        stufftext(buf);
        sprintf(buf, "Correlation coefficient: %f\n", cor);
        stufftext(buf);
        if (rms_ok) {
            sprintf(buf, "RMS relative error: %g\n", rms_pe);
            stufftext(buf);
        }
        sprintf(buf, "Theil U coefficent: %g\n", theil);
        stufftext(buf);
    }

    return RETURN_SUCCESS;
}

/*
 *     lmdif1 - modified by E. Stambulchik  to suit ACE/gr needs
 *
 *     argonne national laboratory. minpack project. march 1980.
 *     burton s. garbow, kenneth e. hillstrom, jorge j. more
 */
int lmdif_drv(U_fp fcn, integer m, integer n, doublereal *x, 
	doublereal *fvec, doublereal *tol, integer *iwa, 
	doublereal *wa, integer lwa, integer nsteps)
{
    /* Initialized data */

    static doublereal factor = 100.;
    static doublereal zero = 0.;

    static integer mp5n, mode, nfev;
    static doublereal ftol, gtol, xtol;
    static doublereal epsfcn;
    static integer maxfev, nprint;
    static integer info;


/*
 *       fcn is the name of the user-supplied subroutine which
 *         calculates the functions. fcn must be declared
 *         in an external statement in the user calling
 *         program, and should be written as follows.
 * 
 *         subroutine fcn(m,n,x,fvec,iflag)
 *         integer m,n,iflag
 *         double precision x(n),fvec(m)
 *         ----------
 *         calculate the functions at x and
 *         return this vector in fvec.
 *         ----------
 *         return
 *         end
 * 
 *         the value of iflag should not be changed by fcn unless
 *         the user wants to terminate execution of lmdif1.
 *         in this case set iflag to a negative integer.
 * 
 *       m is a positive integer input variable set to the number
 *         of functions.
 * 
 *       n is a positive integer input variable set to the number
 *         of variables. n must not exceed m.
 * 
 *       x is an array of length n. on input x must contain
 *         an initial estimate of the solution vector. on output x
 *         contains the final estimate of the solution vector.
 * 
 *       fvec is an output array of length m which contains
 *         the functions evaluated at the output x.
 * 
 *       tol is a nonnegative input variable. termination occurs
 *         when the algorithm estimates either that the relative
 *         error in the sum of squares is at most tol or that
 *         the relative error between x and the solution is at
 *         most tol.
 * 
 *       info is an integer output variable. if the user has
 *         terminated execution, info is set to the (negative)
 *         value of iflag. see description of fcn. otherwise,
 *         info is set as follows.
 * 
 *         info = 0  improper input parameters.
 * 
 *         info = 1  algorithm estimates that the relative error
 *                   in the sum of squares is at most tol.
 * 
 *         info = 2  algorithm estimates that the relative error
 *                   between x and the solution is at most tol.
 * 
 *         info = 3  conditions for info = 1 and info = 2 both hold.
 * 
 *         info = 4  fvec is orthogonal to the columns of the
 *                   jacobian to machine precision.
 * 
 *         info = 5  number of calls to fcn has reached or
 *                   exceeded nsteps*(n+1).
 * 
 *         info = 6  tol is too small. no further reduction in
 *                   the sum of squares is possible.
 * 
 *         info = 7  tol is too small. no further improvement in
 *                   the approximate solution x is possible.
 * 
 *       iwa is an integer work array of length n.
 * 
 *       wa is a work array of length lwa.
 * 
 *       lwa is a positive integer input variable not less than
 *         m*n+5*n+m.
 * 
 *     subprograms called
 * 
 *       user-supplied ...... fcn
 * 
 *       minpack-supplied ... lmdif
 */


    /* Fortran parameter adjustments */
    --fvec;
    --iwa;
    --x;
    --wa;


    info = 0;

/*
 *     Check the input parameters for errors.
 */

    if (n <= 0 || m < n || *tol < zero || lwa < m * n + n * 5 + m) {
	return (int) info;
    }

    maxfev = (n + 1) * nsteps;
    
    ftol = *tol;
    xtol = zero;
    gtol = zero;
    epsfcn = zero;
    mode = 1;
    nprint = 0;
    mp5n = m + n * 5;
    
    lmdif_((U_fp)fcn, &m, &n, &x[1], &fvec[1], &ftol, &xtol, &gtol, &maxfev, &
	    epsfcn, &wa[1], &mode, &factor, &nprint, &info, &nfev, &wa[mp5n + 
	    1], &m, &iwa[1], &wa[n + 1], &wa[(n << 1) + 1], &wa[n * 3 + 1], 
	    &wa[(n << 2) + 1], &wa[n * 5 + 1]);
	    
    if (info == 8) {
	info = 4;
    }
    
    return (int) info;
}
