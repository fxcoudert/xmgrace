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
 * routines to allocate, manipulate, and return
 * information about sets.
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "utils.h"
#include "files.h"
#include "graphs.h"
#include "protos.h"

extern graph *g;

/*
 * return the string version of the set type
 */
char *set_types(int it)
{
    char *s = "xy";

    switch (it) {
    case SET_XY:
	s = "xy";
	break;
    case SET_BAR:
	s = "bar";
	break;
    case SET_BARDY:
	s = "bardy";
	break;
    case SET_BARDYDY:
	s = "bardydy";
	break;
    case SET_XYZ:
	s = "xyz";
	break;
    case SET_XYDX:
	s = "xydx";
	break;
    case SET_XYDY:
	s = "xydy";
	break;
    case SET_XYDXDX:
	s = "xydxdx";
	break;
    case SET_XYDYDY:
	s = "xydydy";
	break;
    case SET_XYDXDY:
	s = "xydxdy";
	break;
    case SET_XYDXDXDYDY:
	s = "xydxdxdydy";
	break;
    case SET_XYHILO:
	s = "xyhilo";
	break;
    case SET_XYR:
	s = "xyr";
	break;
    case SET_XYCOLOR:
	s = "xycolor";
	break;
    case SET_XYCOLPAT:
	s = "xycolpat";
	break;
    case SET_XYVMAP:
	s = "xyvmap";
	break;
    case SET_BOXPLOT:
	s = "xyboxplot";
	break;
    case SET_XYSIZE:
	s = "xysize";
	break;
    }
    return s;
}

int get_settype_by_name(char *s)
{
    int i;
    
    for (i = 0; i < NUMBER_OF_SETTYPES; i++) {
        if (strcmp(set_types(i), s) == 0) {
            return i;
        }
    }
    return SET_BAD;
}

int settype_cols(int type)
{
    int ncols;
    
    switch (type) {
    case SET_XY:
    case SET_BAR:
	ncols = 2;
	break;
    case SET_XYDX:
    case SET_XYDY:
    case SET_XYZ:
    case SET_BARDY:
    case SET_XYR:
    case SET_XYCOLOR:
    case SET_XYSIZE:
	ncols = 3;
	break;
    case SET_XYDXDX:
    case SET_XYDYDY:
    case SET_XYDXDY:
    case SET_BARDYDY:
    case SET_XYCOLPAT:
    case SET_XYVMAP:
	ncols = 4;
	break;
    case SET_XYHILO:
	ncols = 5;
	break;
    case SET_XYDXDXDYDY:
    case SET_BOXPLOT:
	ncols = 6;
	break;
    default:
        ncols = 0;
        break;
    }
    
    return ncols;
}

/*
 * return the string version of the dataset column
 */
char *dataset_colname(int col)
{
    char *s;

    switch (col) {
    case 0:
	s = "X";
	break;
    case 1:
	s = "Y";
	break;
    case 2:
	s = "Y1";
	break;
    case 3:
	s = "Y2";
	break;
    case 4:
	s = "Y3";
	break;
    case 5:
	s = "Y4";
	break;
    default:
	s = "?";
	errmsg("Internal error in dataset_colname()");
        break;
    }
    return s;
}

int zero_set_data(Dataset *dsp)
{
    int k;
    
    if (dsp) {
        dsp->len = 0;
        for (k = 0; k < MAX_SET_COLS; k++) {
	    dsp->ex[k] = NULL;
        }
        dsp->s = NULL;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

/*
 * free set data
 */
int free_set_data(Dataset *dsp)
{
    int k;
    
    if (dsp) {
        if (dsp->len) {
            for (k = 0; k < MAX_SET_COLS; k++) {
	        XCFREE(dsp->ex[k]);
            }
            if (dsp->s) {
	        for (k = 0; k < dsp->len; k++) {
		    XCFREE(dsp->s[k]);
	        }
                XCFREE(dsp->s);
            }
            dsp->len = 0;
	    set_dirtystate();
        }
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}
 
/*
 * free set data, but preserve the parameter settings
 */
void killsetdata(int gno, int setno)
{
    if (is_valid_setno(gno, setno)) {
        free_set_data(&g[gno].p[setno].data);
    }
}

/*
 * (re)allocate data arrays for a set of length len.
 */
int setlength(int gno, int setno, int len)
{
    plotarr *p;
    int i, j, ncols, oldlen;

    if (is_valid_setno(gno, setno) != TRUE) {
        return RETURN_FAILURE;
    }
    
    p = &g[gno].p[setno];

    oldlen = p->data.len;
    if (len == oldlen) {
	return RETURN_SUCCESS;
    }
    if (len < 0) {
	return RETURN_FAILURE;
    }
    
    ncols = settype_cols(p->type);
    
    if (ncols == 0) {
	errmsg("Set type not found in setlength()!");
	return RETURN_FAILURE;
    }
    
    for (i = 0; i < ncols; i++) {
	if ((p->data.ex[i] = xrealloc(p->data.ex[i], len*SIZEOF_DOUBLE)) == NULL
            && len != 0) {
	    return RETURN_FAILURE;
	}
        for (j = oldlen; j < len; j++) {
            p->data.ex[i][j] = 0.0;
        }
    }
    
    if (p->data.s != NULL) {
        for (i = len; i < oldlen; i++) {
            xfree(p->data.s[i]);
        }
        p->data.s = xrealloc(p->data.s, len*sizeof(char *));
        for (j = oldlen; j < len; j++) {
            p->data.s[j] = copy_string(NULL, "");
        }
    }
    
    p->data.len = len;

    set_dirtystate();
    
    return RETURN_SUCCESS;
}

/*
 * moveset 
 */
int moveset(int gnofrom, int setfrom, int gnoto, int setto)
{
    if (gnoto == gnofrom && setfrom == setto) {
	return RETURN_FAILURE;
    }

    if (is_valid_setno(gnofrom, setfrom) != TRUE) {
        return RETURN_FAILURE;
    }

    if (is_set_active(gnoto, setto)) {
	killset(gnoto, setto);
    }
    activateset(gnoto, setto);

    memcpy(&g[gnoto].p[setto], &g[gnofrom].p[setfrom], sizeof(plotarr));

    zero_set_data(&g[gnofrom].p[setfrom].data);
        
    g[gnofrom].p[setfrom].hidden = TRUE;
    
    set_dirtystate();
    return RETURN_SUCCESS;
}


/*
 * copy a set to another set, if the to set doesn't exist allocate it
 */
int copyset(int gfrom, int setfrom, int gto, int setto)
{
    int i, k, len, ncols;
    double *savec[MAX_SET_COLS];
    char **saves;
    char buf[256];

    if (!is_set_active(gfrom, setfrom)) {
	return RETURN_FAILURE;
    }
    if (!is_valid_gno(gto)) {
	return RETURN_FAILURE;
    }
    if (setfrom == setto && gfrom == gto) {
	return RETURN_FAILURE;
    }
    if (is_set_active(gto, setto)) {
	killset(gto, setto);
    }
    len = getsetlength(gfrom, setfrom);
    ncols = dataset_cols(gfrom, setfrom);
    activateset(gto, setto);
    set_dataset_type(gto, setto, dataset_type(gfrom, setfrom));
    if (setlength(gto, setto, len) != RETURN_SUCCESS) {
	return RETURN_FAILURE;
    }
    if (g[gfrom].p[setfrom].data.s != NULL) {
        if ((g[gto].p[setto].data.s = xmalloc(len*sizeof(char *))) == NULL) {
	    return RETURN_FAILURE;
        }
    }

    for (k = 0; k < MAX_SET_COLS; k++) {
	savec[k] = getcol(gto, setto, k);
    }
    saves = get_set_strings(gto, setto);
    memcpy(&g[gto].p[setto], &g[gfrom].p[setfrom], sizeof(plotarr));
    for (k = 0; k < ncols; k++) {
	g[gto].p[setto].data.ex[k] = savec[k];
	memcpy(g[gto].p[setto].data.ex[k],
               g[gfrom].p[setfrom].data.ex[k],
               len*SIZEOF_DOUBLE);
    }
    g[gto].p[setto].data.s = saves;
    if (g[gfrom].p[setfrom].data.s != NULL) {
        for (i = 0; i < len; i++) {
	     g[gto].p[setto].data.s[i] =
                copy_string(NULL, g[gfrom].p[setfrom].data.s[i]);
        }
    }

    sprintf(buf, "copy of set G%d.S%d", gfrom, setfrom);
    setcomment(gto, setto, buf);

    set_dirtystate();
    
    return RETURN_SUCCESS;
}

/*
 * same as copyset(), but doesn't alter the to set appearance
 */
int copysetdata(int gfrom, int setfrom, int gto, int setto)
{
    int i, k, len, ncols;
    char buf[256];

    if (!is_set_active(gfrom, setfrom)) {
	return RETURN_FAILURE;
    }
    if (!is_valid_gno(gto)) {
	return RETURN_FAILURE;
    }
    if (setfrom == setto && gfrom == gto) {
	return RETURN_FAILURE;
    }
    if (is_set_active(gto, setto)) {
	killsetdata(gto, setto);
    }
    len = getsetlength(gfrom, setfrom);
    ncols = dataset_cols(gfrom, setfrom);
    activateset(gto, setto);
    if (dataset_cols(gto, setto) != ncols) {
        set_dataset_type(gto, setto, dataset_type(gfrom, setfrom));
    }
    if (setlength(gto, setto, len) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }
    if (g[gfrom].p[setfrom].data.s != NULL) {
        if ((g[gto].p[setto].data.s = xmalloc(len*sizeof(char *))) == NULL) {
	    return RETURN_FAILURE;
        }
    }

    for (k = 0; k < ncols; k++) {
	memcpy(g[gto].p[setto].data.ex[k],
               g[gfrom].p[setfrom].data.ex[k],
               len*SIZEOF_DOUBLE);
    }
    if (g[gfrom].p[setfrom].data.s != NULL) {
        for (i = 0; i < len; i++) {
	     g[gto].p[setto].data.s[i] =
                copy_string(NULL, g[gfrom].p[setfrom].data.s[i]);
        }
    }

    sprintf(buf, "copy of setdata G%d.S%d", gfrom, setfrom);
    setcomment(gto, setto, buf);

    set_dirtystate();
    
    return RETURN_SUCCESS;
}

/*
 * swap a set with another set
 */
int swapset(int gno1, int setno1, int gno2, int setno2)
{
    plotarr p;

    if (is_valid_setno(gno1, setno1) == FALSE ||
        is_valid_setno(gno2, setno2) == FALSE) {
	return RETURN_FAILURE;
    }
    if (setno1 == setno2 && gno1 == gno2) {
	return RETURN_FAILURE;
    }

    memcpy(&p, &g[gno2].p[setno2], sizeof(plotarr));
    memcpy(&g[gno2].p[setno2], &g[gno1].p[setno1], sizeof(plotarr));
    memcpy(&g[gno1].p[setno1], &p, sizeof(plotarr));

    set_dirtystate();
    
    return RETURN_SUCCESS;
}

/*
 * kill a set
 */
void killset(int gno, int setno)
{
    if (is_valid_setno(gno, setno)) {
	killsetdata(gno, setno);
	set_default_plotarr(&g[gno].p[setno]);
    }
}

double *getcol(int gno, int setno, int col)
{
    if (is_valid_setno(gno, setno)) {
        return g[gno].p[setno].data.ex[col];
    } else {
        return NULL;
    }
}

void setcol(int gno, int setno, int col, double *x, int len)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return;
    }
    g[gno].p[setno].data.ex[col] = x;
    g[gno].p[setno].data.len = len;
    set_dirtystate();
}

char **get_set_strings(int gno, int setno)
{
    if (is_valid_setno(gno, setno)) {
        return g[gno].p[setno].data.s;
    } else {
        return NULL;
    }
}

int set_set_strings(int gno, int setno, int len, char **s)
{
    if (is_valid_setno(gno, setno) && len > 0 && s!= NULL) {
        g[gno].p[setno].data.s = s;
        g[gno].p[setno].data.len = len;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int getsetlength(int gno, int setno)
{
    if (is_valid_setno(gno, setno)) {
        return g[gno].p[setno].data.len;
    } else {
        return -1;
    }
}

int setcomment(int gno, int setno, char *s)
{ 
    if (is_valid_setno(gno, setno) && s != NULL) {
        strncpy(g[gno].p[setno].comments, s, MAX_STRING_LENGTH - 1);
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

char *getcomment(int gno, int setno)
{ 
    if (is_valid_setno(gno, setno)) {
        return g[gno].p[setno].comments;
    } else {
        return NULL;
    }
}

int set_legend_string(int gno, int setno, char *s)
{ 
    if (is_valid_setno(gno, setno) && s != NULL) {
        strncpy(g[gno].p[setno].lstr, s, MAX_STRING_LENGTH - 1);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

char *get_legend_string(int gno, int setno)
{ 
    if (is_valid_setno(gno, setno)) {
        return g[gno].p[setno].lstr;
    } else {
        return NULL;
    }
}

int set_dataset_type(int gno, int setno, int type)
{ 
    int old_type = dataset_type(gno, setno);
    
    if (old_type < 0) {
        /* wrong gno/setno */
        return RETURN_FAILURE;
    } else if (old_type == type) {
        /* nothing changed */
        return RETURN_SUCCESS;
    } else {
        int i, len, ncols_old, ncols_new;
        
        len = getsetlength(gno, setno);
        ncols_old = dataset_cols(gno, setno);
        ncols_new = settype_cols(type);
        for (i = ncols_old; i < ncols_new; i++) {
            g[gno].p[setno].data.ex[i] = xcalloc(len, SIZEOF_DOUBLE);
        }
        for (i = ncols_new; i < ncols_old; i++) {
            XCFREE(g[gno].p[setno].data.ex[i]);
        }

        g[gno].p[setno].type = type;
        
        set_dirtystate();
        return RETURN_SUCCESS;
    }
}

int dataset_type(int gno, int setno)
{ 
    if (is_valid_setno(gno, setno)) {
        return g[gno].p[setno].type;
    } else {
        return -1;
    }
}



void set_hotlink(int gno, int setno, int onoroff, char *fname, int src)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return;
    }
    
    g[gno].p[setno].hotlink = onoroff;
    if (onoroff && fname != NULL) {
	strcpy(g[gno].p[setno].hotfile, fname);
	g[gno].p[setno].hotsrc = src;
    }
    set_dirtystate();
}

int is_hotlinked(int gno, int setno)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return FALSE;
    }
    
    if (g[gno].p[setno].hotlink && strlen(g[gno].p[setno].hotfile)) {
        return g[gno].p[setno].hotlink;
    } else { 
        return FALSE;
    }
}

char *get_hotlink_file(int gno, int setno)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return NULL;
    } else {
        return g[gno].p[setno].hotfile;
    }
}

int get_hotlink_src(int gno, int setno)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return -1;
    } else {
        return g[gno].p[setno].hotsrc;
    }
}

void do_update_hotlink(int gno, int setno)
{
    if (is_hotlinked(gno, setno) != TRUE) {
        return;
    } else {
        plotarr *p;
        p = &g[gno].p[setno];
        update_set_from_file(gno, setno, p->hotfile, p->hotsrc);
    }
}


/*
 * get the min/max fields of a set
 */
int getsetminmax(int gno, int setno, 
                    double *xmin, double *xmax, double *ymin, double *ymax)
{
    double *x, *y;
    int len;
    double x1, x2, y1, y2;
    int i, first = TRUE;
    int imin, imax; /* dummy */

    if (setno == ALL_SETS) {
        for (i = 0; i < number_of_sets(gno); i++) {
            if (is_set_drawable(gno, i)) {
                x = getcol(gno, i, 0);
                y = getcol(gno, i, 1);
                len = getsetlength(gno, i);
                minmax(x, len, &x1, &x2, &imin, &imax);
                minmax(y, len, &y1, &y2, &imin, &imax);
                if (first) {
                    *xmin = x1;
                    *xmax = x2;
                    *ymin = y1;
                    *ymax = y2;
                    first = FALSE;
                } else {
                    *xmin = (x1 < *xmin) ? x1 : *xmin;
                    *xmax = (x2 > *xmax) ? x2 : *xmax;
                    *ymin = (y1 < *ymin) ? y1 : *ymin;
                    *ymax = (y2 > *ymax) ? y2 : *ymax;
                }
            }
        }
    } else if (is_valid_setno(gno, setno)) {
        x = getcol(gno, setno, 0);
        y = getcol(gno, setno, 1);
        len = getsetlength(gno, setno);
        minmax(x, len, xmin, xmax, &imin, &imax);
        minmax(y, len, ymin, ymax, &imin, &imax);
        first = FALSE;
    }
    
    if (first == FALSE) {
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

/*
 * get the min/max fields of a set with fixed x/y range
 */
int getsetminmax_c(int gno, int setno, 
            double *xmin, double *xmax, double *ymin, double *ymax, int ivec)
{
    double vmin_t, vmax_t, *vmin, *vmax, bvmin, bvmax, *vec, *bvec;
    int i, start, stop, n;
    int first = TRUE, hits;

    if (ivec == 1) {    
        bvmin = *xmin;
        bvmax = *xmax;
        vmin  = ymin; 
        vmax  = ymax; 
    } else {
        bvmin = *ymin;
        bvmax = *ymax;
        vmin  = xmin;
        vmax  = xmax;
    }
    if (setno == ALL_SETS) {
        start = 0;
        stop  = number_of_sets(gno) - 1;
    } else if (is_valid_setno(gno, setno)) {
        start = setno;
        stop  = setno;
    } else {
        return RETURN_FAILURE;
    }
    
    for (i = start; i <= stop; i++) {
        if (is_set_drawable(gno, i)) {
            
            if (ivec == 1) {
                bvec = getx(gno, i);
                vec  = gety(gno, i);
            } else {
                bvec = gety(gno, i);
                vec  = getx(gno, i);
            }
            
            n = getsetlength(gno, i);
            hits = minmaxrange(bvec, vec, n, bvmin, bvmax, &vmin_t, &vmax_t);
            if (hits == RETURN_SUCCESS) {
                if (first) {
                    *vmin = vmin_t;
                    *vmax = vmax_t;
                    first = FALSE;
                } else {
                    *vmin = MIN2(vmin_t, *vmin);
                    *vmax = MAX2(vmax_t, *vmax);
                }
            }
        }
    }
    
    if (first == FALSE) {
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}


/*
 * compute the mins and maxes of a vector x
 */
void minmax(double *x, int n, double *xmin, double *xmax, int *imin, int *imax)
{
    int i;
    
    *imin = 0;
    *imax = 0;

    if (x == NULL) {
        *xmin = 0.0;
        *xmax = 0.0;
        return;
    }
    
    *xmin = x[0];
    *xmax = x[0];
    
    for (i = 1; i < n; i++) {
	if (x[i] < *xmin) {
	    *xmin = x[i];
	    *imin = i;
	}
	if (x[i] > *xmax) {
	    *xmax = x[i];
	    *imax = i;
	}
    }
}


/*
 * compute the min and max of vector vec calculated for indices such that
 * bvec values lie within [bmin, bmax] range
 * returns RETURN_FAILURE if none found
 */
int minmaxrange(double *bvec, double *vec, int n, double bvmin, double bvmax,
              	   double *vmin, double *vmax)
{
    int i, first = TRUE;
    
    if ((vec == NULL) || (bvec == NULL)) {
        return RETURN_FAILURE;
    }
    
    for (i = 0; i < n; i++) {
        if ((bvec[i] >= bvmin) && (bvec[i] <= bvmax)) {
	    if (first == TRUE) {
                *vmin = vec[i];
                *vmax = vec[i];
                first = FALSE;
            } else {
                if (vec[i] < *vmin) {
                    *vmin = vec[i];
  	        } else if (vec[i] > *vmax) {
                    *vmax = vec[i];
       	        }
            }
        }
    }
    
    if (first == FALSE) {
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}


/*
 * compute the mins and maxes of a vector x
 */
double vmin(double *x, int n)
{
    int i;
    double xmin;
    if (n <= 0) {
	return 0.0;
    }
    xmin = x[0];
    for (i = 1; i < n; i++) {
	if (x[i] < xmin) {
	    xmin = x[i];
	}
    }
    return xmin;
}

double vmax(double *x, int n)
{
    int i;
    double xmax;
    if (n <= 0) {
	return 0.0;
    }
    xmax = x[0];
    for (i = 1; i < n; i++) {
	if (x[i] > xmax) {
	    xmax = x[i];
	}
    }
    return xmax;
}

int set_point(int gno, int setno, int seti, WPoint wp)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return RETURN_FAILURE;
    }
    if (seti >= getsetlength(gno, setno) || seti < 0) {
        return RETURN_FAILURE;
    }
    (getcol(gno, setno, DATA_X))[seti] = wp.x;
    (getcol(gno, setno, DATA_Y))[seti] = wp.y;
    set_dirtystate();
    return RETURN_SUCCESS;
}

int get_point(int gno, int setno, int seti, WPoint *wp)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return RETURN_FAILURE;
    }
    if (seti >= getsetlength(gno, setno) || seti < 0) {
        return RETURN_FAILURE;
    }
    wp->x = (getcol(gno, setno, DATA_X))[seti];
    wp->y = (getcol(gno, setno, DATA_Y))[seti];
    return RETURN_SUCCESS;
}

void copycol2(int gfrom, int setfrom, int gto, int setto, int col)
{
    int i, n1, n2;
    double *x1, *x2;

    if (is_valid_setno(gfrom, setfrom) != TRUE ||
        is_valid_setno(gto, setto) != TRUE) {
        return;
    }
    n1 = getsetlength(gfrom, setfrom);
    n2 = getsetlength(gto, setto);
    if (n1 != n2) {
        return;
    }
    x1 = getcol(gfrom, setfrom, col);
    x2 = getcol(gto, setto, col);
    for (i = 0; i < n1; i++) {
	x2[i] = x1[i];
    }
    set_dirtystate();
}


int pushset(int gno, int setno, int push_type)
{
    int i, newsetno;
    
    if (is_valid_setno(gno, setno) != TRUE) {
        return RETURN_FAILURE;
    } else {
        switch (push_type) {
        case PUSH_SET_TOFRONT:
            newsetno = number_of_sets(gno) - 1;
            for (i = setno; i < newsetno; i++) {
                if (swapset(gno, i, gno, i + 1) != RETURN_SUCCESS) {
                    return RETURN_FAILURE;
                }
            }
            break;
        case PUSH_SET_TOBACK:
            newsetno = 0;
            for (i = setno; i > newsetno; i--) {
                if (swapset(gno, i, gno, i - 1) != RETURN_SUCCESS) {
                    return RETURN_FAILURE;
                }
            }
            break;
        default:
            return RETURN_FAILURE;
            break;
        }
        return RETURN_SUCCESS;
    }
}


/*
 * pack all sets leaving no gaps in the set structure
 */
void packsets(int gno)
{
    int i, j;

    for (i = 0; i < number_of_sets(gno); i++) {
	if (is_set_active(gno, i)) {
	    for (j = 0; j < i; j++) {
		if (is_set_active(gno, j) != TRUE) {
		    moveset(gno, i, gno, j);
		}
	    }
	}
    }
}

int allocate_set(int gno, int setno)
{
    if (is_valid_setno(gno, setno)) {
        return RETURN_SUCCESS;
    } else
    if (setno >= 0) {
        return realloc_graph_plots(gno, setno + 1);
    } else {
        return RETURN_FAILURE;
    }
}    

int activateset(int gno, int setno)
{
    int retval;
    
    if (is_valid_gno(gno) != TRUE) {
        return RETURN_FAILURE;
    } else {
        retval = allocate_set(gno, setno);
        if (retval == RETURN_SUCCESS) {
            set_set_hidden(gno, setno, FALSE);
        }
        return retval;
    }
}

static target recent_target = {-1, -1};

int get_recent_setno(void)
{
    return recent_target.setno;
}

int get_recent_gno(void)
{
    return recent_target.gno;
}

/*
 * return the next available set in graph gno
 * If target is allocated but with no data, choose it (used for loading sets
 * from project files when sets aren't packed)
 */
int nextset(int gno)
{
    int setno;
    int maxplot;

    if (is_valid_gno(gno) != TRUE) {
        return (-1);
    }
    
    if ( (target_set.gno == gno) &&
         is_valid_setno(target_set.gno, target_set.setno) &&
         !is_set_active(gno, target_set.setno)) {
	setno = target_set.setno;
	target_set.gno = -1;
	target_set.setno = -1;
    } else {
        maxplot = number_of_sets(gno);
        for (setno = 0; setno < maxplot; setno++) {
            if (!is_set_active(gno, setno)) {
                break;
            }
        }
        /* if no sets found, try allocating new one */
        if (setno == maxplot && allocate_set(gno, setno) != RETURN_SUCCESS) {
            return (-1);
        }
    }
    recent_target.gno = gno;
    recent_target.setno = setno;
    return (setno);
}

int is_set_active(int gno, int setno)
{
    if (is_valid_setno(gno, setno) && getsetlength(gno, setno) > 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * return number of active set(s) in gno
 */
int number_of_active_sets(int gno)
{
    int setno, na;

    if (is_valid_gno(gno) != TRUE) {
        return -1;
    }
    
    na = 0;
    for (setno = 0; setno < number_of_sets(gno); setno++) {
	if (is_set_active(gno, setno) == TRUE) {
	    na++;
	}
    }
    return na;
}

/*
 * drop points from a set
 */
void droppoints(int gno, int setno, int startno, int endno)
{
    double *x;
    char **s;
    int i, j, len, ncols, dist;

    if (is_valid_setno(gno, setno) != TRUE) {
        return;
    }

    dist = endno - startno + 1;
    if (dist <= 0) {
        return;
    }
    
    len = getsetlength(gno, setno);
    
    if (dist == len) {
        killsetdata(gno, setno);
        return;
    }
    
    ncols = dataset_cols(gno, setno);
    for (j = 0; j < ncols; j++) {
	x = getcol(gno, setno, j);
	for (i = endno + 1; i < len; i++) {
	    x[i - dist] = x[i];
	}
    }
    if ((s = get_set_strings(gno, setno)) != NULL) {
	for (i = endno + 1; i < len; i++) {
	    s[i - dist] = copy_string(s[i - dist], s[i]);
	}
    }
    setlength(gno, setno, len - dist);
}

/*
 * join several sets together; all but the first set in the list will be killed 
 */
int join_sets(int gno, int *sets, int nsets)
{
    int i, j, n, setno, setno_final, ncols, old_length, new_length;
    double *x1, *x2;
    char **s1, **s2;

    if (nsets < 2) {
        errmsg("nsets < 2");
        return RETURN_FAILURE;
    }
    
    setno_final = sets[0];
    ncols = dataset_cols(gno, setno_final);
    for (i = 0; i < nsets; i++) {
        setno = sets[i];
        if (is_valid_setno(gno, setno) != TRUE) {
            errmsg("Invalid setno in the list");
            return RETURN_FAILURE;
        }
        if (dataset_cols(gno, setno) != ncols) {
            errmsg("Can't join datasets with different number of cols");
            return RETURN_FAILURE;
        }
    }
    
    new_length = getsetlength(gno, setno_final);
    for (i = 1; i < nsets; i++) {
        setno = sets[i];
        old_length = new_length;
        new_length += getsetlength(gno, setno);
        if (setlength(gno, setno_final, new_length) != RETURN_SUCCESS) {
            return RETURN_FAILURE;
        }
        for (j = 0; j < ncols; j++) {
            x1 = getcol(gno, setno_final, j);
            x2 = getcol(gno, setno, j);
            for (n = old_length; n < new_length; n++) {
                x1[n] = x2[n - old_length];
            }
        }
        s1 = get_set_strings(gno, setno_final);
        s2 = get_set_strings(gno, setno);
        if (s1 != NULL && s2 != NULL) {
            for (n = old_length; n < new_length; n++) {
                s1[n] = copy_string(s1[n], s2[n - old_length]);
            }
        }
        killset(gno, setno);
    }
    
    return RETURN_SUCCESS;
}

void reverse_set(int gno, int setno)
{
    int n, i, j, k, ncols;
    double *x;
    char **s;

    if (!is_valid_setno(gno, setno)) {
	return;
    }
    n = getsetlength(gno, setno);
    ncols = dataset_cols(gno, setno);
    for (k = 0; k < ncols; k++) {
	x = getcol(gno, setno, k);
	for (i = 0; i < n / 2; i++) {
	    j = (n - 1) - i;
	    fswap(&x[i], &x[j]);
	}
    }
    if ((s = get_set_strings(gno, setno)) != NULL) {
	char *stmp;
        for (i = 0; i < n / 2; i++) {
	    j = (n - 1) - i;
	    stmp = s[i];
            s[i] = s[j];
            s[j] = stmp;
	}
    }
    set_dirtystate();
}
/*
 * sort a set
 */
static double *vptr;

/*
 * for ascending and descending sorts
 */
 
static int compare_points1(const void *p1, const void *p2)
{
    const int *i1, *i2;
    double a, b;
    i1 = (const int *)p1;
    i2 = (const int *)p2;
    a = vptr[*i1];
    b = vptr[*i2];
    if (a < b) {
	return -1;
    }
    if (a > b) {
	return 1;
    }
    return 0;
}

static int compare_points2(const void *p1, const void *p2)
{
    const int *i1, *i2;
    double a, b;
    i1 = (const int *)p1;
    i2 = (const int *)p2;
    a = vptr[*i1];
    b = vptr[*i2];
    if (a > b) {
	return -1;
    }
    if (a < b) {
	return 1;
    }
    return 0;
}

void sortset(int gno, int setno, int sorton, int stype)
{
    int i, j, nc, len, *ind;
    double *x, *xtmp;
    char **s, **stmp;

    /* get the vector to sort on */
    vptr = getcol(gno, setno, sorton);
    if (vptr == NULL) {
	errmsg("NULL vector in sort, operation cancelled, check set type");
	return;
    }

    len = getsetlength(gno, setno);
    if (len <= 1) {
	return;
    }
    
    /* allocate memory for permuted indices */
    ind = xmalloc(len*SIZEOF_INT);
    if (ind == NULL) {
	return;
    }
    /* allocate memory for temporary array */
    xtmp = xmalloc(len*SIZEOF_DOUBLE);
    if (xtmp == NULL) {
	xfree(ind);
	return;
    }
    
    s = get_set_strings(gno, setno);
    if (s != NULL) {
        stmp = xmalloc(len*sizeof(char *));
        if (stmp == NULL) {
	    xfree(xtmp);
	    xfree(ind);
        }
    } else {
        stmp = NULL;
    }
    
    /* initialize indices */
    for (i = 0; i < len; i++) {
	ind[i] = i;
    }

    /* sort */
    qsort(ind, len, SIZEOF_INT, stype ? compare_points2 : compare_points1);

    /* straighten things out - done one vector at a time for storage */
    
    nc = dataset_cols(gno, setno);
    /* loop over the number of columns */
    for (j = 0; j < nc; j++) {
        /* get this vector and put into the temporary vector in the right order */
	x = getcol(gno, setno, j);
	for (i = 0; i < len; i++) {
	    xtmp[i] = x[ind[i]];
	}
        
        /* load it back to the set */
	for (i = 0; i < len; i++) {
	    x[i] = xtmp[i];
	}
    }
    
    /* same with strings, if any */
    if (s != NULL) {
	for (i = 0; i < len; i++) {
	    stmp[i] = s[ind[i]];
	}

	for (i = 0; i < len; i++) {
	    s[i] = stmp[i];
	}
    }
    
    /* free allocated temporary arrays */
    xfree(stmp);
    xfree(xtmp);
    xfree(ind);

    set_dirtystate();
}

/*
 * sort two arrays
 */
void sort_xy(double *tmp1, double *tmp2, int up, int sorton, int stype)
{

    int d, i, j;
    int lo = 0;
    double t1, t2;

    if (sorton == 1) {
	double *ttmp;

	ttmp = tmp1;
	tmp1 = tmp2;
	tmp2 = ttmp;
    }
    up--;

    for (d = up - lo + 1; d > 1;) {
	if (d < 5)
	    d = 1;
	else
	    d = (5 * d - 1) / 11;
	for (i = up - d; i >= lo; i--) {
	    t1 = tmp1[i];
	    t2 = tmp2[i];
	    if (!stype) {
		for (j = i + d; j <= up && (t1 > tmp1[j]); j += d) {
		    tmp1[j - d] = tmp1[j];
		    tmp2[j - d] = tmp2[j];
		}
		tmp1[j - d] = t1;
		tmp2[j - d] = t2;
	    } else {
		for (j = i + d; j <= up && (t1 < tmp1[j]); j += d) {
		    tmp1[j - d] = tmp1[j];
		    tmp2[j - d] = tmp2[j];
		}
		tmp1[j - d] = t1;
		tmp2[j - d] = t2;
	    }
	}
    }
    set_dirtystate();
}

/*
 * delete the point pt in setno
 */
void del_point(int gno, int setno, int pt)
{
    droppoints(gno, setno, pt, pt);
}

/*
 * add a point to setno
 */
void add_point(int gno, int setno, double px, double py)
{
    int len;
    double *x, *y;

    if (is_valid_setno(gno, setno)) {
	 len = getsetlength(gno, setno);
	 setlength(gno, setno, len + 1);
	 x = getx(gno, setno);
	 y = gety(gno, setno);
	 x[len] = px;
	 y[len] = py;
    }
}

void zero_datapoint(Datapoint *dpoint)
{
    int k;
    
    for (k = 0; k < MAX_SET_COLS; k++) {
        dpoint->ex[k] = 0.0;
    }
    dpoint->s = NULL;
}

/*
 * add a point to setno at ind
 */
int add_point_at(int gno, int setno, int ind, const Datapoint *dpoint)
{
    int len, col, ncols;
    double *ex;
    char **s;

    if (is_valid_setno(gno, setno)) {
        len = getsetlength(gno, setno);
        if (ind < 0 || ind > len) {
            return RETURN_FAILURE;
        }
        len++;
        setlength(gno, setno, len);
        ncols = dataset_cols(gno, setno);
        for (col = 0; col < ncols; col++) {
            ex = getcol(gno, setno, col);
            if (ind < len - 1) {
                memmove(ex + ind + 1, ex + ind, (len - ind - 1)*SIZEOF_DOUBLE);
            }
            ex[ind] = dpoint->ex[col];
        }
        s = get_set_strings(gno, setno);
        if (s != NULL) {
            if (ind < len - 1) {
                memmove(s + ind + 1, s + ind, (len - ind - 1)*sizeof(char *));
            }
            s[ind] = copy_string(NULL, dpoint->s);
        }
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_datapoint(int gno, int setno, int ind, int *ncols, Datapoint *dpoint)
{
    int n, col;
    double *ex;
    char **s;
    
    n = getsetlength(gno, setno);
    if (ind < 0 || ind >= n) {
        return RETURN_FAILURE;
    } else {
        *ncols = dataset_cols(gno, setno);
        for (col = 0; col < *ncols; col++) {
            ex = getcol(gno, setno, col);
            dpoint->ex[col] = ex[ind];
        }
        s = get_set_strings(gno, setno);
        if (s != NULL) {
            dpoint->s = s[ind];
        } else {
            dpoint->s = NULL;
        }
        return RETURN_SUCCESS;
    }
}

void delete_byindex(int gno, int setno, int *ind)
{
    int i, j, cnt = 0;
    int ncols = dataset_cols(gno, setno);

    if (is_valid_setno(gno, setno) != TRUE) {
        return;
    }
    
    for (i = 0; i < getsetlength(gno, setno); i++) {
	if (ind[i]) {
	    cnt++;
	}
    }
    if (cnt == getsetlength(gno, setno)) {
	killset(gno, setno);
	return;
    }
    cnt = 0;
    for (i = 0; i < getsetlength(gno, setno); i++) {
	if (ind[i] == 0) {
	    for (j = 0; j < ncols; j++) {
                (getcol(gno, setno, j))[cnt] = (getcol(gno, setno, j))[i];
	    }
	    cnt++;
	}
    }
    setlength(gno, setno, cnt);
}


/*
 * move a set to another set, in possibly another graph
 */
int do_moveset(int gfrom, int setfrom, int gto, int setto)
{
    int retval;
    char buf[64];
    
    retval = moveset(gfrom, setfrom, gto, setto);
    if (retval != RETURN_SUCCESS) {
        sprintf(buf,
            "Error moving G%d.S%d to G%d.S%d",
            gfrom, setfrom, gto, setto);
        errmsg(buf);
    }
    return retval;
}

/*
 * do_copyset
 */
int do_copyset(int gfrom, int setfrom, int gto, int setto)
{
    int retval;
    char buf[64];
    
    retval = copyset(gfrom, setfrom, gto, setto);
    if (retval != RETURN_SUCCESS) {
        sprintf(buf,
            "Error copying G%d.S%d to G%d.S%d",
            gfrom, setfrom, gto, setto);
        errmsg(buf);
    }
    return retval;
}

/*
 * do_swapset
 */
int do_swapset(int gfrom, int setfrom, int gto, int setto)
{
    int retval;
    char buf[64];
    
    retval = swapset(gfrom, setfrom, gto, setto);
    if (retval != RETURN_SUCCESS) {
        sprintf(buf,
            "Error swapping G%d.S%d with G%d.S%d",
            gfrom, setfrom, gto, setto);
        errmsg(buf);
    }
    return retval;
}

/*
 * split a set into lpart length sets
 */
void do_splitsets(int gno, int setno, int lpart)
{
    int i, j, k, ncols, len, plen, tmpset, npsets;
    double *x;
    char s[256];
    plotarr p;
    Dataset ds, dstmp;

    if ((len = getsetlength(gno, setno)) < 2) {
	errmsg("Set length < 2");
	return;
    }
    if (lpart >= len) {
	errmsg("Split length >= set length");
	return;
    }
    if (lpart <= 0) {
	errmsg("Split length <= 0");
	return;
    }

    npsets = (len - 1)/lpart + 1;

    /* get number of columns in this set */
    ncols = dataset_cols(gno, setno);

    p = g[gno].p[setno];

    /* save the contents to a temporary buffer */
    memcpy(&ds, &p.data, sizeof(Dataset));

    /* zero data contents of the original set */
    zero_set_data(&g[gno].p[setno].data);

    /* now load each set */
    for (i = 0; i < npsets; i++) {
	plen = MIN2(lpart, len - i*lpart); 
        tmpset = nextset(gno);
        if (!is_valid_setno(gno, tmpset)) {
            errmsg("Can't create new set");
            return;
        }
	
        /* set the plot parameters */
	dstmp = g[gno].p[tmpset].data;
        g[gno].p[tmpset] = p;
	g[gno].p[tmpset].data = dstmp;

	set_set_hidden(gno, tmpset, FALSE);
	if (setlength(gno, tmpset, plen) != RETURN_SUCCESS) {
            /* should not happen */
            return;
        }
        if (ds.s) {
            g[gno].p[tmpset].data.s = xmalloc(plen*sizeof(char *));
        }
        
        /* load the data into each column */
	for (k = 0; k < ncols; k++) {
	    x = getcol(gno, tmpset, k);
	    for (j = 0; j < plen; j++) {
		x[j] = ds.ex[k][i*lpart + j];
	    }
	}
        if (ds.s) {
	    for (j = 0; j < plen; j++) {
		g[gno].p[tmpset].data.s[j] =
                    copy_string(NULL, ds.s[i*lpart + j]);
	    }
        }
	
        sprintf(s, "partition %d of set G%d.S%d", i + 1, gno, setno);
	setcomment(gno, tmpset, s);
    }
    
    free_set_data(&ds);
}

/*
 * drop points from an active set
 */
void do_drop_points(int gno, int setno, int startno, int endno)
{
    int setlength;
    char buf[256];

    if (!is_set_active(gno, setno)) {
	sprintf(buf, "Set %d not active", setno);
	errmsg(buf);
	return;
    }

    setlength = getsetlength(gno, setno);
    if (startno < 0) {
        startno = setlength + 1 + startno;
    }
    if (endno < 0) {
        endno = setlength + 1 + endno;
    }

    if (startno > endno) {
        iswap(&startno, &endno);
    }

    if (startno < 0) {
	errmsg("Start # < 0");
	return;
    }
    if (endno >= setlength) {
	errmsg("Ending # >= set length");
	return;
    }

    droppoints(gno, setno, startno, endno);
}


/*
 * sort sets, only works on sets of type XY
 */
void do_sort(int setno, int sorton, int stype)
{
    int i, gno = get_cg();
    char buf[256];

    if (setno == -1) {
	for (i = 0; i < number_of_sets(gno); i++) {
	    if (is_set_active(gno, i)) {
		sortset(gno, i, sorton, stype);
	    }
	}
    } else {
	if (!is_set_active(gno, setno)) {
	    sprintf(buf, "Set %d not active", setno);
	    errmsg(buf);
	    return;
	} else {
	    sortset(gno, setno, sorton, stype);
	}
    }
}


double setybase(int gno, int setno)
{
    double dummy, *y, ybase = 0.0;
    int len;

    if (is_valid_setno(gno, setno) != TRUE) {
        return 0.0;
    }
    
    y = getcol(gno, setno, DATA_Y);
    len = getsetlength(gno, setno);
    
    switch (g[gno].p[setno].baseline_type) {
    case BASELINE_TYPE_0:
        ybase = 0.0;
        break;
    case BASELINE_TYPE_SMIN:
        ybase = vmin(y, len);
        break;
    case BASELINE_TYPE_SAVG:
        stasum(y, len, &ybase, &dummy);
        break;
    case BASELINE_TYPE_SMAX:
        ybase = vmax(y, len);
        break;
    case BASELINE_TYPE_GMIN:
        ybase = g[gno].w.yg1;
        break;
    case BASELINE_TYPE_GMAX:
        ybase = g[gno].w.yg2;
        break;
    default:
        errmsg("Wrong type of baseline");
    }
    
    return(ybase);
}


int dataset_cols(int gno, int setno)
{
    return settype_cols(dataset_type(gno, setno));
}

int load_comments_to_legend(int gno, int setno)
{
    return set_legend_string(gno, setno, getcomment(gno, setno));
}

int filter_set(int gno, int setno, char *rarray)
{
    int i, ip, j, ncols;
    Dataset *dsp;
    
    if (is_valid_setno(gno, setno) != TRUE) {
        return RETURN_FAILURE;
    }
    if (rarray == NULL) {
        return RETURN_SUCCESS;
    }
    ncols = dataset_cols(gno, setno);
    dsp = &(g[gno].p[setno].data);
    ip = 0;
    for (i = 0; i < dsp->len; i++) {
        if (rarray[i]) {
            for (j = 0; j < ncols; j++) {
                dsp->ex[j][ip] = dsp->ex[j][i];
            }
            if (dsp->s != NULL) {
                dsp->s[ip] = copy_string(dsp->s[ip], dsp->s[i]);
            }
            ip++;
        }
    }
    setlength(gno, setno, ip);
    return RETURN_SUCCESS;
}
