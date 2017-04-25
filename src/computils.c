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
 * procedures for performing transformations from the command
 * line interpreter and the GUI.
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "utils.h"
#include "draw.h"
#include "ssdata.h"
#include "graphs.h"
#include "parser.h"
#include "protos.h"

static void forwarddiff(double *x, double *y, double *resx, double *resy, int n);
static void backwarddiff(double *x, double *y, double *resx, double *resy, int n);
static void centereddiff(double *x, double *y, double *resx, double *resy, int n);
int get_points_inregion(int rno, int invr, int len, double *x, double *y, int *cnt, double **xt, double **yt);

static char buf[256];


void do_fourier_command(int gno, int setno, int ftype, int ltype)
{
    switch (ftype) {
    case FFT_DFT:
	do_fourier(gno, setno, 0, 0, ltype, 0, 0, 0);
	break;
    case FFT_INVDFT       :
	do_fourier(gno, setno, 0, 0, ltype, 1, 0, 0);
	break;
    case FFT_FFT:
	do_fourier(gno, setno, 1, 0, ltype, 0, 0, 0);
	break;
    case FFT_INVFFT       :
	do_fourier(gno, setno, 1, 0, ltype, 1, 0, 0);
	break;
    }
}


/*
 * evaluate a formula
 */
int do_compute(int gno, int setno, int graphto, int loadto, char *rarray, char *fstr)
{
    if (is_set_active(gno, setno)) {
	if (gno != graphto || setno != loadto) {
	    if (copysetdata(gno, setno, graphto, loadto) != RETURN_SUCCESS) {
	        return RETURN_FAILURE;
            }
        }
	filter_set(graphto, loadto, rarray);
        set_parser_setno(graphto, loadto);
        if (scanner(fstr) != RETURN_SUCCESS) {
	    if (graphto != gno || loadto != setno) {
		killset(graphto, loadto);
	    }
	    return RETURN_FAILURE;
	} else {
	    set_dirtystate();
            return RETURN_SUCCESS;
        }
    } else {
        return RETURN_FAILURE;
    }
}

/*
 * forward, backward and centered differences
 */
static void forwarddiff(double *x, double *y, double *resx, double *resy, int n)
{
    int i, eflag = 0;
    double h;

    for (i = 1; i < n; i++) {
	resx[i - 1] = x[i - 1];
	h = x[i - 1] - x[i];
	if (h == 0.0) {
	    resy[i - 1] = - MAXNUM;
	    eflag = 1;
	} else {
	    resy[i - 1] = (y[i - 1] - y[i]) / h;
	}
    }
    if (eflag) {
	errmsg("Warning: infinite slope, check set status before proceeding");
    }
}

static void backwarddiff(double *x, double *y, double *resx, double *resy, int n)
{
    int i, eflag = 0;
    double h;

    for (i = 0; i < n - 1; i++) {
	resx[i] = x[i];
	h = x[i + 1] - x[i];
	if (h == 0.0) {
	    resy[i] = - MAXNUM;
	    eflag = 1;
	} else {
	    resy[i] = (y[i + 1] - y[i]) / h;
	}
    }
    if (eflag) {
	errmsg("Warning: infinite slope, check set status before proceeding");
    }
}

static void centereddiff(double *x, double *y, double *resx, double *resy, int n)
{
    int i, eflag = 0;
    double h1, h2;

    for (i = 1; i < n - 1; i++) {
	resx[i - 1] = x[i];
	h1 = x[i] - x[i - 1];
	h2 = x[i + 1] - x[i];
	if (h1 + h2 == 0.0) {
	    resy[i - 1] = - MAXNUM;
	    eflag = 1;
	} else {
	    resy[i - 1] = (y[i + 1] - y[i - 1]) / (h1 + h2);
	}
    }
    if (eflag) {
	errmsg("Warning: infinite slope, check set status before proceeding");
    }
}

static void seasonaldiff(double *x, double *y,
			 double *resx, double *resy, int n, int period)
{
    int i;

    for (i = 0; i < n - period; i++) {
	resx[i] = x[i];
	resy[i] = y[i] - y[i + period];
    }
}

/*
 * trapezoidal rule
 */
double trapint(double *x, double *y, double *resx, double *resy, int n)
{
    int i;
    double sum = 0.0;
    double h;

    if (n < 2) {
        return 0.0;
    }
    
    if (resx != NULL) {
        resx[0] = x[0];
    }
    if (resy != NULL) {
        resy[0] = 0.0;
    }
    for (i = 1; i < n; i++) {
	h = (x[i] - x[i - 1]);
	if (resx != NULL) {
	    resx[i] = x[i];
	}
	sum = sum + h * (y[i - 1] + y[i]) * 0.5;
	if (resy != NULL) {
	    resy[i] = sum;
	}
    }
    return sum;
}

/*
 * apply a digital filter
 */
void do_digfilter(int set1, int set2)
{
    int digfiltset;

    if (!(is_set_active(get_cg(), set1) && is_set_active(get_cg(), set2))) {
	errmsg("Set not active");
	return;
    }
    if ((getsetlength(get_cg(), set1) < 3) || (getsetlength(get_cg(), set2) < 3)) {
	errmsg("Set length < 3");
	return;
    }
    digfiltset = nextset(get_cg());
    if (digfiltset != (-1)) {
	activateset(get_cg(), digfiltset);
	setlength(get_cg(), digfiltset, getsetlength(get_cg(), set1) - getsetlength(get_cg(), set2) + 1);
	sprintf(buf, "Digital filter from set %d applied to set %d", set2, set1);
	filterser(getsetlength(get_cg(), set1),
		  getx(get_cg(), set1),
		  gety(get_cg(), set1),
		  getx(get_cg(), digfiltset),
		  gety(get_cg(), digfiltset),
		  gety(get_cg(), set2),
		  getsetlength(get_cg(), set2));
	setcomment(get_cg(), digfiltset, buf);
    }
}

/*
 * linear convolution
 */
void do_linearc(int gno1, int set1, int gno2, int set2)
{
    int linearcset, i, itmp, cg = get_cg();
    double *xtmp;

    if (!(is_set_active(gno1, set1) && is_set_active(gno2, set2))) {
	errmsg("Set not active");
	return;
    }
    if ((getsetlength(gno1, set1) < 3) || (getsetlength(gno2, set2) < 3)) {
	errmsg("Set length < 3");
	return;
    }
    linearcset = nextset(cg);
    if (linearcset != (-1)) {
	activateset(cg, linearcset);
	setlength(cg, linearcset, (itmp = getsetlength(gno1, set1) + getsetlength(gno2, set2) - 1));
	linearconv(gety(gno2, set2), gety(gno1, set1), gety(cg, linearcset), getsetlength(gno2, set2), getsetlength(gno1, set1));
	xtmp = getx(cg, linearcset);
	for (i = 0; i < itmp; i++) {
	    xtmp[i] = i;
	}
	sprintf(buf, "Linear convolution of set %d with set %d", set1, set2);
	setcomment(cg, linearcset, buf);
    }
}

/*
 * cross correlation/covariance
 */
void do_xcor(int gno1, int set1, int gno2, int set2, int maxlag, int covar)
{
    int xcorset, i, ierr, len, cg = get_cg();
    double *xtmp;

    if (!(is_set_active(gno1, set1) && is_set_active(gno2, set2))) {
	errmsg("Set not active");
	return;
    }
    
    len = getsetlength(gno1, set1);
    if (getsetlength(gno2, set2) != len) {
	errmsg("Sets must be of the same length");
    }
    if (len < 2) {
	errmsg("Set length < 2");
	return;
    }
    
    if (maxlag < 1 || maxlag > len) {
	errmsg("Lag incorrectly specified");
	return;
    }
    
    xcorset = nextset(cg);
    
    if (xcorset != (-1)) {
	char *fname;
        activateset(cg, xcorset);
	setlength(cg, xcorset, maxlag);
	if (covar) {
            fname = "covariance";
        } else {
            fname = "correlation";
        }
        if (set1 != set2) {
	    sprintf(buf, "X-%s of G%d.S%d and G%d.S%d at maximum lag %d",
                    fname, gno1, set1, gno2, set2, maxlag);
	} else {
	    sprintf(buf, "Auto-%s of G%d.S%d at maximum lag %d",
                    fname, gno1, set1, maxlag);
	}
	ierr = crosscorr(gety(gno1, set1), gety(gno2, set2), len,
                         maxlag, covar, gety(cg, xcorset));
	xtmp = getx(cg, xcorset);
	for (i = 0; i < maxlag; i++) {
	    xtmp[i] = i;
	}
	setcomment(cg, xcorset, buf);
    }
}


/*
 * numerical integration
 */
double do_int(int gno, int setno, int itype)
{
    int intset;
    double sum = 0;

    if (!is_set_active(gno, setno)) {
	errmsg("Set not active");
	return 0.0;
    }
    if (getsetlength(gno, setno) < 3) {
	errmsg("Set length < 3");
	return 0.0;
    }
    if (itype == 0) {
	intset = nextset(gno);
	if (intset != (-1)) {
	    activateset(gno, intset);
	    setlength(gno, intset, getsetlength(gno, setno));
	    sprintf(buf, "Cumulative sum of set %d", setno);
	    sum = trapint(getx(gno, setno), gety(gno, setno), getx(gno, intset), gety(gno, intset), getsetlength(gno, setno));
	    setcomment(gno, intset, buf);
	}
    } else {
	sum = trapint(getx(gno, setno), gety(gno, setno), NULL, NULL, getsetlength(gno, setno));
    }
    return sum;
}

/*
 * difference a set
 * itype means
 *  0 - forward
 *  1 - backward
 *  2 - centered difference
 */
void do_differ(int gno, int setno, int itype)
{
    int diffset;

    if (!is_set_active(gno, setno)) {
	errmsg("Set not active");
	return;
    }
    if (getsetlength(gno, setno) < 3) {
	errmsg("Set length < 3");
	return;
    }
    diffset = nextset(gno);
    if (diffset != (-1)) {
	activateset(gno, diffset);
	switch (itype) {
	case 0:
	    sprintf(buf, "Forward difference of set %d", setno);
	    setlength(gno, diffset, getsetlength(gno, setno) - 1);
	    forwarddiff(getx(gno, setno), gety(gno, setno), getx(gno, diffset), gety(gno, diffset), getsetlength(gno, setno));
	    break;
	case 1:
	    sprintf(buf, "Backward difference of set %d", setno);
	    setlength(gno, diffset, getsetlength(gno, setno) - 1);
	    backwarddiff(getx(gno, setno), gety(gno, setno), getx(gno, diffset), gety(gno, diffset), getsetlength(gno, setno));
	    break;
	case 2:
	    sprintf(buf, "Centered difference of set %d", setno);
	    setlength(gno, diffset, getsetlength(gno, setno) - 2);
	    centereddiff(getx(gno, setno), gety(gno, setno), getx(gno, diffset), gety(gno, diffset), getsetlength(gno, setno));
	    break;
	}
	setcomment(gno, diffset, buf);
    }
}

/*
 * seasonally difference a set
 */
void do_seasonal_diff(int setno, int period)
{
    int diffset;

    if (!is_set_active(get_cg(), setno)) {
	errmsg("Set not active");
	return;
    }
    if (getsetlength(get_cg(), setno) < 2) {
	errmsg("Set length < 2");
	return;
    }
    diffset = nextset(get_cg());
    if (diffset != (-1)) {
	activateset(get_cg(), diffset);
	setlength(get_cg(), diffset, getsetlength(get_cg(), setno) - period);
	seasonaldiff(getx(get_cg(), setno), gety(get_cg(), setno),
		     getx(get_cg(), diffset), gety(get_cg(), diffset),
		     getsetlength(get_cg(), setno), period);
	sprintf(buf, "Seasonal difference of set %d, period %d", setno, period);
	setcomment(get_cg(), diffset, buf);
    }
}

/*
 * regression with restrictions to region rno if rno >= 0
 */
void do_regress(int gno, int setno, int ideg, int iresid, int rno, int invr, int fitset)
	/* 
	 * gno, setno  - set to perform fit on
	 * ideg   - degree of fit
	 * irisid - 0 -> whole set, 1-> subset of setno
	 * rno    - region number of subset
	 * invr   - 1->invert region, 0 -> do nothing
	 * fitset - set to which fitted function will be loaded
	 * 			Y values are computed at the x values in the set
	 *          if -1 is specified, a set with the same x values as the
	 *          one being fitted will be created and used
	 */
{
    int len, i, sdeg = ideg;
    int cnt = 0, fitlen = 0;
    double *x, *y, *xt = NULL, *yt = NULL, *xr = NULL, *yr = NULL;
    char buf[256];
    double c[20];   /* coefficients of fitted polynomial */

    if (!is_set_active(gno, setno)) {
		errmsg("Set not active");
		return;
    }
    len = getsetlength(gno, setno);
    x = getx(gno, setno);
    y = gety(gno, setno);
    if (rno == -1) {
		xt = x;
		yt = y;
    } else if (isactive_region(rno)) {
		if (!get_points_inregion(rno, invr, len, x, y, &cnt, &xt, &yt)) {
			if (cnt == 0) {
			errmsg("No points found in region, operation cancelled");
			}
			return;
		}
		len = cnt;
    } else {
		errmsg("Selected region is not active");
		return;
    }
    /*
     * first part for polynomials, second part for linear fits to transformed
     * data
     */
    if ((len < ideg && ideg <= 10) || (len < 2 && ideg > 10)) {
		errmsg("Too few points in set, operation cancelled");
		return;
    }
	/* determine is set provided or use abscissa from fitted set */
    if( fitset == -1 ) {		        /* create set */
    	if( (fitset = nextset(gno)) != -1 ) {
			activateset(gno, fitset);
			setlength(gno, fitset, len);
			fitlen = len;
			xr = getx(gno, fitset);
			for (i = 0; i < len; i++) {
	    		xr[i] = xt[i];
			}	
			yr = gety(gno, fitset);
		}
    } else {									/* set has been provided */
		fitlen = getsetlength( gno, fitset );
		xr = getx(gno, fitset);
		yr = gety(gno, fitset);
    }

	/* transform data so that system has the form y' = A + B * x' */
    if (fitset != -1) {
	if (ideg == 12) {	/* y=A*x^B -> ln(y) = ln(A) + B * ln(x) */
	    ideg = 1;
	    for (i = 0; i < len; i++) {
			if (xt[i] <= 0.0) {
				errmsg("One of X[i] <= 0.0");
				return;
			}
			if (yt[i] <= 0.0) {
				errmsg("One of Y[i] <= 0.0");
				return;
			}
	    }
	    for (i = 0; i < len; i++) {
			xt[i] = log(xt[i]);
			yt[i] = log(yt[i]);
	    }
		for( i=0; i<fitlen; i++ )
			if( xr[i] <= 0.0 ) {
				errmsg("One of X[i] <= 0.0");
				return;
			} 
		for( i=0; i<fitlen; i++ )
			xr[i] = log( xr[i] );
	} else if (ideg == 13) {   /*y=Aexp(B*x) -> ln(y) = ln(A) + B * x */
	    ideg = 1;
	    for (i = 0; i < len; i++) {
			if (yt[i] <= 0.0) {
				errmsg("One of Y[i] <= 0.0");
				return;
		 	}
	    }
	    for (i = 0; i < len; i++) {
			yt[i] = log(yt[i]);
	    }
	} else if (ideg == 14) {	/* y = A + B * ln(x) */
	    ideg = 1;
	    for (i = 0; i < len; i++) {
			if (xt[i] <= 0.0) {
				errmsg("One of X[i] <= 0.0");
				return;
			}
	    }
		for (i = 0; i < len; i++) {
			xt[i] = log(xt[i]);	
		}
		for( i=0; i<fitlen; i++ )
			if( xr[i] <= 0.0 ) {
				errmsg("One of X[i] <= 0.0");
				return;
			} 
		for( i=0; i<fitlen; i++ ){
			xr[i] = log( xr[i] );
		}	  
	} else if (ideg == 15) {	/* y = 1/( A + B*x ) -> 1/y = a + B*x */
	    ideg = 1;
	    for (i = 0; i < len; i++) {
			if (yt[i] == 0.0) {
			    errmsg("One of Y[i] = 0.0");
			    return;
			}
	    }
	    for (i = 0; i < len; i++) {
			yt[i] = 1.0 / yt[i];
	    }
	}

	if (fitcurve(xt, yt, len, ideg, c)) {
	    killset(gno, fitset);
	    goto bustout;
	}

	/* compute function at requested x ordinates */
	for( i=0; i<fitlen; i++ )
		yr[i] = leasev( c, ideg, xr[i] );

	sprintf(buf, "\nN.B. Statistics refer to the transformed model\n");
	/* apply inverse transform, output fitted function in human readable form */
	if( sdeg<11 ) {
		sprintf(buf, "\ny = %.5g %c %.5g * x", c[0], c[1]>=0?'+':'-', fabs(c[1]));
		for( i=2; i<=ideg; i++ )
			sprintf( buf+strlen(buf), " %c %.5g * x^%d", c[i]>=0?'+':'-', 
															fabs(c[i]), i );
		strcat( buf, "\n" );
	} else if (sdeg == 12) {	/* ln(y) = ln(A) + b * ln(x) */
		sprintf( buf, "\ny = %.5g * x^%.5g\n", exp(c[0]), c[1] );
	    for (i = 0; i < len; i++) {
			xt[i] = exp(xt[i]);
			yt[i] = exp(yt[i]);
	    }
	    for (i = 0; i < fitlen; i++){
			yr[i] = exp(yr[i]);
			xr[i] = exp(xr[i]);
		}
	} else if (sdeg == 13) { 	/* ln(y) = ln(A) + B * x */
		sprintf( buf, "\ny = %.5g * exp( %.5g * x )\n", exp(c[0]), c[1] );
	    for (i = 0; i < len; i++) {
			yt[i] = exp(yt[i]);
	    }
	    for (i = 0; i < fitlen; i++)
			yr[i] = exp(yr[i]);
	} else if (sdeg == 14) {	/* y = A + B * ln(x) */
		sprintf(buf, "\ny = %.5g %c %.5g * ln(x)\n", c[0], c[1]>=0?'+':'-',
											fabs(c[1]) );
	    for (i = 0; i < len; i++) {
			xt[i] = exp(xt[i]);
	    }
	    for (i = 0; i < fitlen; i++)
			xr[i] = exp(xr[i]);
	} else if (sdeg == 15) {	/* y = 1/( A + B*x ) */
		sprintf( buf, "\ny = 1/(%.5g %c %.5g * x)\n", c[0], c[1]>=0?'+':'-',
													fabs(c[1]) );
	    for (i = 0; i < len; i++) {
			yt[i] = 1.0 / yt[i];
	    }
	    for (i = 0; i < fitlen; i++)
			yr[i] = 1.0 / yr[i];
	}
	stufftext(buf);
	sprintf(buf, "\nRegression of set %d results to set %d\n", setno, fitset);
	stufftext(buf);
	
	switch (iresid) {
	case 1:
		/* determine residual */
	    for (i = 0; i < len; i++) {
			yr[i] = yt[i] - yr[i];
	    }
	    break;
	case 2:
	    break;
	}
	sprintf(buf, "%d deg fit of set %d", ideg, setno);
	setcomment(gno, fitset, buf);
    }
    bustout:;
    if (rno >= 0 && cnt != 0) {	/* had a region and allocated memory there */
	xfree(xt);
	xfree(yt);
    }
}

/*
 * running averages, medians, min, max, std. deviation
 */
void do_runavg(int gno, int setno, int runlen, int runtype, int rno, int invr)
{
    int runset;
    int len, cnt = 0;
    double *x, *y, *xt = NULL, *yt = NULL, *xr, *yr;

    if (!is_set_active(gno, setno)) {
	errmsg("Set not active");
	return;
    }
    if (runlen < 2) {
	errmsg("Length of running average < 2");
	return;
    }
    len = getsetlength(gno, setno);
    x = getx(gno, setno);
    y = gety(gno, setno);
    if (rno == -1) {
	xt = x;
	yt = y;
    } else if (isactive_region(rno)) {
	if (!get_points_inregion(rno, invr, len, x, y, &cnt, &xt, &yt)) {
	    if (cnt == 0) {
		errmsg("No points found in region, operation cancelled");
	    }
	    return;
	}
	len = cnt;
    } else {
	errmsg("Selected region is not active");
	return;
    }
    if (runlen >= len) {
	errmsg("Length of running average > set length");
	goto bustout;
    }
    runset = nextset(gno);
    if (runset != (-1)) {
	activateset(gno, runset);
	setlength(gno, runset, len - runlen + 1);
	xr = getx(gno, runset);
	yr = gety(gno, runset);
	switch (runtype) {
	case 0:
	    runavg(xt, yt, xr, yr, len, runlen);
	    sprintf(buf, "%d-pt. avg. on set %d ", runlen, setno);
	    break;
	case 1:
	    runmedian(xt, yt, xr, yr, len, runlen);
	    sprintf(buf, "%d-pt. median on set %d ", runlen, setno);
	    break;
	case 2:
	    runminmax(xt, yt, xr, yr, len, runlen, 0);
	    sprintf(buf, "%d-pt. min on set %d ", runlen, setno);
	    break;
	case 3:
	    runminmax(xt, yt, xr, yr, len, runlen, 1);
	    sprintf(buf, "%d-pt. max on set %d ", runlen, setno);
	    break;
	case 4:
	    runstddev(xt, yt, xr, yr, len, runlen);
	    sprintf(buf, "%d-pt. std dev., set %d ", runlen, setno);
	    break;
	}
	setcomment(gno, runset, buf);
    }
  bustout:;
    if (rno >= 0 && cnt != 0) {	/* had a region and allocated memory there */
	xfree(xt);
	xfree(yt);
    }
}

/*
 * DFT by FFT or definition
 */
void do_fourier(int gno, int setno, int fftflag, int load, int loadx, int invflag, int type, int wind)
{
    int i, ilen;
    double *x, *y, *xx, *yy, delt, T;
    int i2 = 0, specset;

    if (!is_set_active(get_cg(), setno)) {
	errmsg("Set not active");
	return;
    }
    ilen = getsetlength(get_cg(), setno);
    if (ilen < 2) {
	errmsg("Set length < 2");
	return;
    }
    if (fftflag) {
	if ((i2 = ilog2(ilen)) <= 0) {
	    errmsg("Set length not a power of 2");
	    return;
	}
    }
    specset = nextset(get_cg());
    if (specset != -1) {
	activateset(get_cg(), specset);
	setlength(get_cg(), specset, ilen);
	xx = getx(get_cg(), specset);
	yy = gety(get_cg(), specset);
	x = getx(get_cg(), setno);
	y = gety(get_cg(), setno);
	copyx(get_cg(), setno, specset);
	copyy(get_cg(), setno, specset);
	if (wind != 0) {	/* apply data window if needed */
	    apply_window(xx, yy, ilen, type, wind);
	}
	if (type == 0) {	/* real data */
	    for (i = 0; i < ilen; i++) {
		xx[i] = yy[i];
		yy[i] = 0.0;
	    }
	}
	if (fftflag) {
	    fft(xx, yy, ilen, i2, invflag);
	} else {
	    dft(xx, yy, ilen, invflag);
	}
	switch (load) {
	case 0:
	    delt = (x[ilen-1] - x[0])/(ilen -1.0);
	    T = (x[ilen - 1] - x[0]);
	    xx = getx(get_cg(), specset);
	    yy = gety(get_cg(), specset);
	    for (i = 0; i < ilen / 2; i++) {
	      /* carefully get amplitude of complex xform: 
		 use abs(a[i]) + abs(a[-i]) except for zero term */
	      if(i) yy[i] = hypot(xx[i], yy[i])+hypot(xx[ilen-i], yy[ilen-i]);
	      else yy[i]=2*fabs(xx[i]);
		switch (loadx) {
		case 0:
		    xx[i] = i;
		    break;
		case 1:
		    /* xx[i] = 2.0 * M_PI * i / ilen; */
		    xx[i] = i / T;
		    break;
		case 2:
		    if (i == 0) {
			xx[i] = T + delt;	/* the mean */
		    } else {
			/* xx[i] = (double) ilen / (double) i; */
			xx[i] = T / i;
		    }
		    break;
		}
	    }
	    setlength(get_cg(), specset, ilen / 2);
	    break;
	case 1:
	    delt = (x[ilen-1] - x[0])/(ilen -1.0);
	    T = (x[ilen - 1] - x[0]);
	    setlength(get_cg(), specset, ilen / 2);
	    xx = getx(get_cg(), specset);
	    yy = gety(get_cg(), specset);
	    for (i = 0; i < ilen / 2; i++) {
		yy[i] = -atan2(yy[i], xx[i]);
		switch (loadx) {
		case 0:
		    xx[i] = i;
		    break;
		case 1:
		    /* xx[i] = 2.0 * M_PI * i / ilen; */
		    xx[i] = i / T;
		    break;
		case 2:
		    if (i == 0) {
			xx[i] = T + delt;
		    } else {
			/* xx[i] = (double) ilen / (double) i; */
			xx[i] = T / i;
		    }
		    break;
		}
	    }
	    break;
	}
	if (fftflag) {
	    sprintf(buf, "FFT of set %d", setno);
	} else {
	    sprintf(buf, "DFT of set %d", setno);
	}
	setcomment(get_cg(), specset, buf);
    }
}

/*
 * Apply a window to a set, result goes to a new set.
 */
void do_window(int setno, int type, int wind)
{
    int ilen;
    double *xx, *yy;
    int specset;

    if (!is_set_active(get_cg(), setno)) {
	errmsg("Set not active");
	return;
    }
    ilen = getsetlength(get_cg(), setno);
    if (ilen < 2) {
	errmsg("Set length < 2");
	return;
    }
    specset = nextset(get_cg());
    if (specset != -1) {
	char *wtype[6];
	wtype[0] = "Triangular";
	wtype[1] = "Hanning";
	wtype[2] = "Welch";
	wtype[3] = "Hamming";
	wtype[4] = "Blackman";
	wtype[5] = "Parzen";

	activateset(get_cg(), specset);
	setlength(get_cg(), specset, ilen);
	xx = getx(get_cg(), specset);
	yy = gety(get_cg(), specset);
	copyx(get_cg(), setno, specset);
	copyy(get_cg(), setno, specset);
	if (wind != 0) {
	    apply_window(xx, yy, ilen, type, wind);
	    sprintf(buf, "%s windowed set %d", wtype[wind - 1], setno);
	} else {		/* shouldn't happen */
	}
	setcomment(get_cg(), specset, buf);
    }
}

void apply_window(double *xx, double *yy, int ilen, int type, int wind)
{
    int i;

    for (i = 0; i < ilen; i++) {
	switch (wind) {
	case 1:		/* triangular */
	    if (type != 0) {
		xx[i] *= 1.0 - fabs((i - 0.5 * (ilen - 1.0)) / (0.5 * (ilen - 1.0)));
	    }
	    yy[i] *= 1.0 - fabs((i - 0.5 * (ilen - 1.0)) / (0.5 * (ilen - 1.0)));

	    break;
	case 2:		/* Hanning */
	    if (type != 0) {
		xx[i] = xx[i] * (0.5 - 0.5 * cos(2.0 * M_PI * i / (ilen - 1.0)));
	    }
	    yy[i] = yy[i] * (0.5 - 0.5 * cos(2.0 * M_PI * i / (ilen - 1.0)));
	    break;
	case 3:		/* Welch (from Numerical Recipes) */
	    if (type != 0) {
		xx[i] *= 1.0 - pow((i - 0.5 * (ilen - 1.0)) / (0.5 * (ilen + 1.0)), 2.0);
	    }
	    yy[i] *= 1.0 - pow((i - 0.5 * (ilen - 1.0)) / (0.5 * (ilen + 1.0)), 2.0);
	    break;
	case 4:		/* Hamming */
	    if (type != 0) {
		xx[i] = xx[i] * (0.54 - 0.46 * cos(2.0 * M_PI * i / (ilen - 1.0)));
	    }
	    yy[i] = yy[i] * (0.54 - 0.46 * cos(2.0 * M_PI * i / (ilen - 1.0)));
	    break;
	case 5:		/* Blackman */
	    if (type != 0) {
		xx[i] = xx[i] * (0.42 - 0.5 * cos(2.0 * M_PI * i / (ilen - 1.0)) + 0.08 * cos(4.0 * M_PI * i / (ilen - 1.0)));
	    }
	    yy[i] = yy[i] * (0.42 - 0.5 * cos(2.0 * M_PI * i / (ilen - 1.0)) + 0.08 * cos(4.0 * M_PI * i / (ilen - 1.0)));
	    break;
	case 6:		/* Parzen (from Numerical Recipes) */
	    if (type != 0) {
		xx[i] *= 1.0 - fabs((i - 0.5 * (ilen - 1)) / (0.5 * (ilen + 1)));
	    }
	    yy[i] *= 1.0 - fabs((i - 0.5 * (ilen - 1)) / (0.5 * (ilen + 1)));
	    break;
	}
    }
}


/*
 * histograms
 */
int do_histo(int fromgraph, int fromset, int tograph, int toset,
	      double *bins, int nbins, int cumulative, int normalize)
{
    int i, ndata;
    int *hist;
    double *x, *y, *data;
    plotarr p;
    
    if (!is_set_active(fromgraph, fromset)) {
	errmsg("Set not active");
	return RETURN_FAILURE;
    }
    if (nbins <= 0) {
	errmsg("Number of bins <= 0");
	return RETURN_FAILURE;
    }
    if (toset == SET_SELECT_NEXT) {
	toset = nextset(tograph);
    }
    if (!is_valid_setno(tograph, toset)) {
	errmsg("Can't activate destination set");
        return RETURN_FAILURE;
    }
    
    ndata = getsetlength(fromgraph, fromset);
    data = gety(fromgraph, fromset);
    
    hist = xmalloc(nbins*SIZEOF_INT);
    if (hist == NULL) {
        errmsg("xmalloc failed in do_histo()");
        return RETURN_FAILURE;
    }

    if (histogram(ndata, data, nbins, bins, hist) == RETURN_FAILURE){
        xfree(hist);
        return RETURN_FAILURE;
    }
    
    activateset(tograph, toset);
    setlength(tograph, toset, nbins + 1);
    x = getx(tograph, toset);
    y = gety(tograph, toset);
    
    x[0] = bins[0];
    y[0] = 0.0;
    for (i = 1; i < nbins + 1; i++) {
        x[i] = bins[i];
        y[i] = hist[i - 1];
        if (cumulative) {
            y[i] += y[i - 1];
        }
    }
    
    if (normalize) {
        for (i = 1; i < nbins + 1; i++) {
            double factor;
            if (cumulative) {
                factor = 1.0/ndata;
            } else {
                factor = 1.0/((bins[i] - bins[i - 1])*ndata);
            }
            y[i] *= factor;
        }
    }
    
    xfree(hist);

    get_graph_plotarr(tograph, toset, &p);
    p.sym = SYM_NONE;
    p.linet = LINE_TYPE_LEFTSTAIR;
    p.dropline = TRUE;
    p.baseline = FALSE;
    p.baseline_type = BASELINE_TYPE_0;
    p.lines = 1;
    p.symlines = 1;
    sprintf(p.comments, "Histogram from G%d.S%d", fromgraph, fromset);
    set_graph_plotarr(tograph, toset, &p);

    return RETURN_SUCCESS;
}

int histogram(int ndata, double *data, int nbins, double *bins, int *hist)
{
    int i, j, bsign;
    double minval, maxval;
    
    if (nbins < 1) {
        errmsg("Number of bins < 1");
        return RETURN_FAILURE;
    }
    
    bsign = monotonicity(bins, nbins + 1, TRUE);
    if (bsign == 0) {
        errmsg("Non-monotonic bins");
        return RETURN_FAILURE;
    }
    
    for (i = 0; i < nbins; i++) {
        hist[i] = 0;
    }
    
    /* TODO: binary search */
    for (i = 0; i < ndata; i++) {
        for (j = 0; j < nbins; j++) {
            if (bsign > 0) {
                minval = bins[j];
                maxval = bins[j + 1];
            } else {
                minval = bins[j + 1];
                maxval = bins[j];
            }
            if (data[i] >= minval && data[i] <= maxval) {
                hist[j] += 1;
                break;
            }
        }
    }
    return RETURN_SUCCESS;
}


/*
 * sample a set, by start/step or logical expression
 */
void do_sample(int setno, int typeno, char *exprstr, int startno, int stepno)
{
    int len, npts = 0, i, resset;
    double *x, *y;
    int reslen;
    double *result;
    int gno = get_cg();

    if (!is_set_active(gno, setno)) {
	errmsg("Set not active");
	return;
    }

    len = getsetlength(gno, setno);

    resset = nextset(gno);
    if (resset < 0) {
	return;
    }

    x = getx(gno, setno);
    y = gety(gno, setno);

    if (typeno == 0) {
	if (len <= 2) {
	    errmsg("Set has <= 2 points");
	    return;
	}
	if (startno < 1) {
	    errmsg("Start point < 1 (locations in sets are numbered starting from 1)");
	    return;
	}
	if (stepno < 1) {
	    errmsg("Step < 1");
	    return;
	}
	for (i = startno - 1; i < len; i += stepno) {
	    add_point(gno, resset, x[i], y[i]);
	    npts++;
	}
	sprintf(buf, "Sample, %d, %d set #%d", startno, stepno, setno);
    } else {
        if (set_parser_setno(gno, setno) != RETURN_SUCCESS) {
	    errmsg("Bad set");
            killset(gno, resset);
	    return;
        }
        if (v_scanner(exprstr, &reslen, &result) != RETURN_SUCCESS) {
            killset(gno, resset);
	    return;
        }
        if (reslen != len) {
	    errmsg("Internal error");
            killset(gno, resset);
	    return;
        }

        npts = 0;
	sprintf(buf, "Sample from %d, using '%s'", setno, exprstr);
	for (i = 0; i < len; i++) {
	    if ((int) rint(result[i])) {
		add_point(gno, resset, x[i], y[i]);
		npts++;
	    }
	}
        xfree(result);
    }
    if (npts > 0) {
	setcomment(gno, resset, buf);
    }
}

#define prune_xconv(res,x,xtype)	\
    switch (deltatypeno) {		\
    case PRUNE_VIEWPORT:		\
	res = xy_xconv(x);			\
	break;				\
    case PRUNE_WORLD:			\
	switch (xtype) {		\
	case PRUNE_LIN:			\
	    res = x;			\
	    break;			\
	case PRUNE_LOG:			\
	    res = log(x);		\
	    break;			\
	}				\
    }

#define prune_yconv(res,y,ytype)	\
    switch (deltatypeno) {		\
    case PRUNE_VIEWPORT:		\
	res = xy_yconv(y);			\
	break;				\
    case PRUNE_WORLD:			\
	switch (ytype) {		\
	case PRUNE_LIN:			\
	    res = y;			\
	    break;			\
	case PRUNE_LOG:			\
	    res = log(y);		\
	    break;			\
	}				\
    }

/*
 * Prune data
 */
void do_prune(int setno, int typeno, int deltatypeno, float deltax, float deltay, int dxtype, int dytype)
{
    int len, npts = 0, d, i, j, k, drop, resset;
    double *x, *y, *resx, *resy, xtmp, ytmp, ddx = 0.0, ddy = 0.0;
    double xj = 0.0, xjm = 0.0, xjp = 0.0, yj = 0.0, yjm = 0.0, yjp = 0.0;

    if (!is_set_active(get_cg(), setno)) {
        errmsg("Set not active");
        return;
    }
    len = getsetlength(get_cg(), setno);
    if (len <= 2) {
	errmsg("Set has <= 2 points");
	return;
    }
    x = getx(get_cg(), setno);
    y = gety(get_cg(), setno);
    switch (typeno) {
    case PRUNE_CIRCLE:
    case PRUNE_ELLIPSE:
    case PRUNE_RECTANGLE:
	deltax = fabs(deltax);
	if (deltax == 0)
	    return;
	break;
    }
    switch (typeno) {
    case PRUNE_CIRCLE:
	deltay = deltax;
	break;
    case PRUNE_ELLIPSE:
    case PRUNE_RECTANGLE:
    case PRUNE_INTERPOLATION:
	deltay = fabs(deltay);
	if (deltay == 0)
	    return;
	break;
    }
    if (deltatypeno == PRUNE_WORLD) {
	if (dxtype == PRUNE_LOG && deltax < 1.0) {
	    deltax = 1.0 / deltax;
	}
	if (dytype == PRUNE_LOG && deltay < 1.0) {
	    deltay = 1.0 / deltay;
	}
    }
    resset = nextset(get_cg());
    if (resset < 0) {
        return;
    }
    add_point(get_cg(), resset, x[0], y[0]);
    npts++;
    resx = getx(get_cg(), resset);
    resy = gety(get_cg(), resset);
    switch (typeno) {
    case PRUNE_CIRCLE:
    case PRUNE_ELLIPSE:
	for (i = 1; i < len; i++) {
	    xtmp = x[i];
	    ytmp = y[i];
	    drop = FALSE;
	    for (j = npts - 1; j >= 0 && drop == FALSE; j--) {
		switch (deltatypeno) {
		case PRUNE_VIEWPORT:
		    ddx = (xy_xconv(xtmp) - xy_xconv(resx[j])) / deltax;
		    if (fabs(ddx) < 1.0) {
			ddy = (xy_yconv(ytmp) - xy_yconv(resy[j])) / deltay;
			if (ddx * ddx + ddy * ddy < 1.0) {
			    drop = TRUE;
			}
		    }
		    break;
		case PRUNE_WORLD:
		    switch (dxtype) {
		    case PRUNE_LIN:
			ddx = (xtmp - resx[j]) / deltax;
			break;
		    case PRUNE_LOG:
			ddx = (xtmp / resx[j]);
			if (ddx < 1.0) {
			    ddx = 1.0 / ddx;
			}
			ddx /= deltax;
			break;
		    }
		    if (fabs(ddx) < 1.0) {
			switch (dytype) {
			case PRUNE_LIN:
			    ddy = (ytmp - resy[j]) / deltay;
			    break;
			case PRUNE_LOG:
			    ddy = (ytmp / resy[j]);
			    if (ddy < 1.0) {
				ddy = 1.0 / ddy;
			    }
			    ddy /= deltay;
			    break;
			}
			if (ddx * ddx + ddy * ddy < 1.0) {
			    drop = TRUE;
			}
		    }
		    break;
		}
	    }
	    if (drop == FALSE) {
		add_point(get_cg(), resset, xtmp, ytmp);
		npts++;
		resx = getx(get_cg(), resset);
		resy = gety(get_cg(), resset);
	    }
	}
	sprintf(buf, "Prune from %d, %s dx = %g dy = %g", setno, 
	    (typeno == 0) ? "Circle" : "Ellipse", deltax, deltay);
	break;
    case PRUNE_RECTANGLE:
	for (i = 1; i < len; i++) {
	    xtmp = x[i];
	    ytmp = y[i];
	    drop = FALSE;
	    for (j = npts - 1; j >= 0 && drop == FALSE; j--) {
		switch (deltatypeno) {
		case PRUNE_VIEWPORT:
		    ddx = fabs(xy_xconv(xtmp) - xy_xconv(resx[j]));
		    if (ddx < deltax) {
			ddy = fabs(xy_yconv(ytmp) - xy_yconv(resy[j]));
			if (ddy < deltay) {
			    drop = TRUE;
			}
		    }
		    break;
		case PRUNE_WORLD:
		    switch (dxtype) {
		    case PRUNE_LIN:
			ddx = fabs(xtmp - resx[j]);
			break;
		    case PRUNE_LOG:
			ddx = (xtmp / resx[j]);
			if (ddx < 1.0) {
			    ddx = 1.0 / ddx;
			}
			break;
		    }
		    if (ddx < deltax) {
			switch (dytype) {
			case PRUNE_LIN:
			    ddy = fabs(ytmp - resy[j]);
			    break;
			case PRUNE_LOG:
			    ddy = (ytmp / resy[j]);
			    if (ddy < 1.0) {
				ddy = 1.0 / ddy;
			    }
			    break;
			}
			if (ddy < deltay) {
			    drop = TRUE;
			}
		    }
		    break;
		}
	    }
	    if (drop == FALSE) {
		add_point(get_cg(), resset, xtmp, ytmp);
		npts++;
		resx = getx(get_cg(), resset);
		resy = gety(get_cg(), resset);
	    }
	}
	sprintf(buf, "Prune from %d, %s dx = %g dy = %g", setno, 
	    "Rectangle", deltax, deltay);
	break;
    case PRUNE_INTERPOLATION:
	k = 0;
	prune_xconv(xjm, x[0], dxtype);
	prune_yconv(yjm, y[0], dytype);
	while (k < len - 2) {
	    d = 1;
	    i = k + d + 1;
	    drop = TRUE;
	    while (TRUE) {
		prune_xconv(xjp, x[i], dxtype);
		prune_yconv(yjp, y[i], dytype);
		for (j = k + 1; j < i && drop == TRUE; j++) {
		    prune_xconv(xj, x[j], dxtype);
		    prune_yconv(yj, y[j], dytype);
		    if (xjp == xjm) {
			ytmp = 0.5 * (yjp + yjm);
		    } else {
			ytmp = (yjp*(xj-xjm)+yjm*(xjp-xj))/(xjp-xjm);
		    }
		    switch (deltatypeno) {
		    case PRUNE_VIEWPORT:
			ddy = fabs(ytmp - yj);
			break;
		    case PRUNE_WORLD:
			switch (dytype) {
			case PRUNE_LIN:
			    ddy = fabs(ytmp - yj);
			    break;
			case PRUNE_LOG:
			    ddy = exp(fabs(ytmp - yj));
			    break;
			}
		    }
		    if (ddy > deltay) {
			drop = FALSE;
		    }
		}
		if (drop == FALSE || i == len - 1) {
		    break;
		}
		d *= 2;
		i = k + d + 1;
		if (i >= len) {
		    i = len - 1;
		}
	    }
	    if (drop == FALSE) {
		i = k + 1;
		drop = TRUE;
		while (d > 1) {
		    d /= 2;
		    i += d;
		    prune_xconv(xjp, x[i], dxtype);
		    prune_yconv(yjp, y[i], dytype);
		    drop = TRUE;
		    for (j = k + 1; j < i && drop == TRUE; j++) {
			prune_xconv(xj, x[j], dxtype);
			prune_yconv(yj, y[j], dytype);
			ytmp = (yjp*(xj-xjm)+yjm*(xjp-xj))/(xjp-xjm);
			switch (deltatypeno) {
			case PRUNE_VIEWPORT:
			    ddy = fabs(ytmp - yj);
			    break;
			case PRUNE_WORLD:
			    switch (dytype) {
			    case PRUNE_LIN:
				ddy = fabs(ytmp - yj);
				break;
			    case PRUNE_LOG:
				ddy = exp(fabs(ytmp - yj));
				break;
			    }
			}
			if (ddy > deltay) {
			    drop = FALSE;
			}
		    }
		    if (drop == FALSE) {
			i -= d;
		    }
		}
	    }
	    k = i;
	    prune_xconv(xjm, x[k], dxtype);
	    prune_yconv(yjm, y[k], dytype);
	    add_point(get_cg(), resset, x[k], y[k]);
	    npts++;
	    resx = getx(get_cg(), resset);
	    resy = gety(get_cg(), resset);
	}
	if (k == len - 2) {
	    add_point(get_cg(), resset, x[len-1], y[len-1]);
	    npts++;
	}
	sprintf(buf, "Prune from %d, %s dy = %g", setno, 
	    "Interpolation", deltay);
	break;
    }
    setcomment(get_cg(), resset, buf);
}

int get_points_inregion(int rno, int invr, int len, double *x, double *y, int *cnt, double **xt, double **yt)
{
    int i, clen = 0;
    double *xtmp, *ytmp;
    *cnt = 0;
    if (isactive_region(rno)) {
	for (i = 0; i < len; i++) {
	    if (invr) {
		if (!inregion(rno, x[i], y[i])) {
		    clen++;
		}
	    } else {
		if (inregion(rno, x[i], y[i])) {
		    clen++;
		}
	    }
	}
	if (clen == 0) {
	    return 0;
	}
	xtmp = (double *) xcalloc(clen, SIZEOF_DOUBLE);
	if (xtmp == NULL) {
	    return 0;
	}
	ytmp = (double *) xcalloc(clen, SIZEOF_DOUBLE);
	if (ytmp == NULL) {
	    xfree(xtmp);
	    return 0;
	}
	clen = 0;
	for (i = 0; i < len; i++) {
	    if (invr) {
		if (!inregion(rno, x[i], y[i])) {
		    xtmp[clen] = x[i];
		    ytmp[clen] = y[i];
		    clen++;
		}
	    } else {
		if (inregion(rno, x[i], y[i])) {
		    xtmp[clen] = x[i];
		    ytmp[clen] = y[i];
		    clen++;
		}
	    }
	}
    } else {
	return 0;
    }
    *cnt = clen;
    *xt = xtmp;
    *yt = ytmp;
    return 1;
}

int interpolate(double *mesh, double *yint, int meshlen,
    double *x, double *y, int len, int method)
{
    double *b, *c, *d;
    double dx;
    int i, ifound;
    int m;

    /* For linear interpolation, non-strict monotonicity is fine */
    m = monotonicity(x, len, method == INTERP_LINEAR ? FALSE:TRUE);
    if (m == 0) {
        errmsg("Can't interpolate a set with non-monotonic abscissas");
        return RETURN_FAILURE;
    }

    switch (method) {
    case INTERP_SPLINE:
    case INTERP_ASPLINE:
        b = xcalloc(len, SIZEOF_DOUBLE);
        c = xcalloc(len, SIZEOF_DOUBLE);
        d = xcalloc(len, SIZEOF_DOUBLE);
        if (b == NULL || c == NULL || d == NULL) {
            xfree(b);
            xfree(c);
            xfree(d);
            return RETURN_FAILURE;
        }
        if (method == INTERP_ASPLINE){
            /* Akima spline */
            aspline(len, x, y, b, c, d);
        } else {
            /* Plain cubic spline */
            spline(len, x, y, b, c, d);
        }

	seval(mesh, yint, meshlen, x, y, b, c, d, len);

        xfree(b);
        xfree(c);
        xfree(d);
        break;
    default:
        /* linear interpolation */

        for (i = 0; i < meshlen; i++) {
            ifound = find_span_index(x, len, m, mesh[i]);
            if (ifound < 0) {
                ifound = 0;
            } else if (ifound > len - 2) {
                ifound = len - 2;
            }
            dx = x[ifound + 1] - x[ifound];
            if (dx != 0.0) {
                yint[i] = y[ifound] + (mesh[i] - x[ifound])*
                    ((y[ifound + 1] - y[ifound])/dx);
            } else {
                yint[i] = (y[ifound] + y[ifound + 1])/2;
            }
        }
        break;
    }
    
    return RETURN_SUCCESS;
}

/* interpolate a set at abscissas from mesh
 * method - type of spline (or linear interpolation)
 * if strict is set, perform interpolation only within source set bounds
 * (i.e., no extrapolation)
 */
int do_interp(int gno_src, int setno_src, int gno_dest, int setno_dest,
    double *mesh, int meshlen, int method, int strict)
{
    int len, n, ncols;
    double *x, *xint;
    char *s;
	
    if (!is_valid_setno(gno_src, setno_src)) {
	errmsg("Interpolated set not active");
	return RETURN_FAILURE;
    }
    if (mesh == NULL || meshlen < 1) {
        errmsg("NULL sampling mesh");
        return RETURN_FAILURE;
    }
    
    len = getsetlength(gno_src, setno_src);
    ncols = dataset_cols(gno_src, setno_src);

    if (setno_dest == SET_SELECT_NEXT) {
	setno_dest = nextset(gno_dest);
    }
    if (!is_valid_setno(gno_dest, setno_dest)) {
	errmsg("Can't activate destination set");
	return RETURN_FAILURE;
    }

    if (dataset_cols(gno_dest, setno_dest) != ncols) {
        copyset(gno_src, setno_src, gno_dest, setno_dest);
    }
    
    setlength(gno_dest, setno_dest, meshlen);
    activateset(gno_dest, setno_dest);
    
    x = getcol(gno_src, setno_src, DATA_X);
    for (n = 1; n < ncols; n++) {
        int res;
        double *y, *yint;
        
        y    = getcol(gno_src, setno_src, n);
        yint = getcol(gno_dest, setno_dest, n);
        
        res = interpolate(mesh, yint, meshlen, x, y, len, method);
        if (res != RETURN_SUCCESS) {
            killset(gno_dest, setno_dest);
            return RETURN_FAILURE;
        }
    }

    xint = getcol(gno_dest, setno_dest, DATA_X);
    memcpy(xint, mesh, meshlen*SIZEOF_DOUBLE);

    if (strict) {
        double xmin, xmax;
        int i, imin, imax;
        minmax(x, len, &xmin, &xmax, &imin, &imax);
        for (i = meshlen - 1; i >= 0; i--) {
            if (xint[i] < xmin || xint[i] > xmax) {
                del_point(gno_dest, setno_dest, i);
            }
        }
    }
    
    switch (method) {
    case INTERP_SPLINE:
        s = "cubic spline";
        break;
    case INTERP_ASPLINE:
        s = "Akima spline";
        break;
    default:
        s = "linear interpolation";
        break;
    }
    sprintf(buf, "Interpolated from G%d.S%d using %s", gno_src, setno_src, s);
    setcomment(gno_dest, setno_dest, buf);
    
    return RETURN_SUCCESS;
}

int get_restriction_array(int gno, int setno,
    int rtype, int negate, char **rarray)
{
    int i, n, regno;
    double *x, *y;
    world w;
    WPoint wp;
    
    if (rtype == RESTRICT_NONE) {
        *rarray = NULL;
        return RETURN_SUCCESS;
    }
    
    n = getsetlength(gno, setno);
    if (n <= 0) {
        *rarray = NULL;
        return RETURN_FAILURE;
    }
    
    *rarray = xmalloc(n*SIZEOF_CHAR);
    if (*rarray == NULL) {
        return RETURN_FAILURE;
    }
    
    x = getcol(gno, setno, DATA_X);
    y = getcol(gno, setno, DATA_Y);
    
    switch (rtype) {
    case RESTRICT_REG0:
    case RESTRICT_REG1:
    case RESTRICT_REG2:
    case RESTRICT_REG3:
    case RESTRICT_REG4:
        regno = rtype - RESTRICT_REG0;
        for (i = 0; i < n; i++) {
            (*rarray)[i] = inregion(regno, x[i], y[i]) ? !negate : negate;
        }
        break;
    case RESTRICT_WORLD:
        get_graph_world(gno, &w);
        for (i = 0; i < n; i++) {
            wp.x = x[i];
            wp.y = y[i];
            (*rarray)[i] = is_wpoint_inside(&wp, &w) ? !negate : negate;
        }
        break;
    default:
        errmsg("Internal error in get_restriction_array()");
        XCFREE(*rarray);
        return RETURN_FAILURE;
        break;
    }
    return RETURN_SUCCESS;
}

int monotonicity(double *array, int len, int strict)
{
    int i;
    int s0, s1;
    
    if (len < 2) {
        errmsg("Monotonicity of an array of length < 2 is meaningless");
        return 0;
    }
    
    s0 = sign(array[1] - array[0]);
    for (i = 2; i < len; i++) {
        s1 = sign(array[i] - array[i - 1]);
        if (s1 != s0) {
            if (strict) {
                return 0;
            } else if (s0 == 0) {
                s0 = s1;
            } else if (s1 != 0) {
                return 0;
            }
        }
    }
    
    return s0;
}

int find_span_index(double *array, int len, int m, double x)
{
    int ind, low = 0, high = len - 1;
    
    if (len < 2 || m == 0) {
        errmsg("find_span_index() called with a non-monotonic array");
        return -2;
    } else if (m > 0) {
        /* ascending order */
        if (x < array[0]) {
            return -1;
        } else if (x > array[len - 1]) {
            return len - 1;
        } else {
            while (low <= high) {
	        ind = (low + high) / 2;
	        if (x < array[ind]) {
	            high = ind - 1;
	        } else if (x > array[ind + 1]) {
	            low = ind + 1;
	        } else {
	            return ind;
	        }
            }
        }
    } else {
        /* descending order */
        if (x > array[0]) {
            return -1;
        } else if (x < array[len - 1]) {
            return len - 1;
        } else {
            while (low <= high) {
	        ind = (low + high) / 2;
	        if (x > array[ind]) {
	            high = ind - 1;
	        } else if (x < array[ind + 1]) {
	            low = ind + 1;
	        } else {
	            return ind;
	        }
            }
        }
    }

    /* should never happen */
    errmsg("internal error in find_span_index()");
    return -2;
}
