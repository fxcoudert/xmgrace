/*
 * Grace - Graphics for Exploratory Data Analysis
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-98 GRACE Development Team
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

#ifndef __AS274C_H_
#define __AS274C_H_

int includ(int np, int nrbar, double w, 
           double *xrow, double yelem, double *d, double *rbar, 
           double *thetab, double *sserr);

int clear(int np, int nrbar, double *d, double *rbar,
          double *thetab, double *sserr);

int regcf(int np, int nrbar, double *d, double *rbar,
          double *thetab, double *tol, double *beta,
          int nreq);

int tolset(int np, int nrbar, double *d, double *rbar, double *tol);

int sing(int np, int nrbar, double *d, 
         double *rbar, double *thetab, double *sserr,
         double *tol, int *lindep);

int ss(int np, double *d, double *thetab, 
       double *sserr, double *rss);

int cov(int np, int nrbar, double *d, 
        double *rbar, int nreq, double *rinv, double *var, 
        double *covmat, int dimcov, double *sterr);

void inv(int np, int nrbar, double *rbar, int nreq, double *rinv);

int pcorr(int np, int nrbar, double *d, 
          double *rbar, double *thetab, double *sserr, int in, 
          double *cormat, int dimc, double *ycorr);

void cor(int np, double *d, double *rbar, 
         double *thetab, double *sserr, double *work,
         double *cormat, double *ycorr);

int vmove(int np, int nrbar, int *vorder, 
          double *d, double *rbar, double *thetab, double *rss, 
          int from, int to, double *tol);

int reordr(int np, int nrbar, int *vorder, 
	double *d, double *rbar, double *thetab, double *rss, 
           double *tol, int *list, int n, int pos1);

int hdiag(double *xrow, int np, int nrbar, double *d,
          double *rbar, double *tol, int nreq, double *hii);

void putdvec(const char *s, double *x, int l, int h);
void pr_utdm_v(double *x, int N, int width, int precision);

double *dvector(int l, int h);
int *ivector(int l, int h);
double **dmatrix(int rl, int rh, int cl, int ch);

int dofitcurve(int cnt, double *xd, double *yd, int nd, double *c);


#endif /* __AS274C_H_ */
