/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2003 Grace Development Team
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
 * utilities for graphs
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>

#include "globals.h"
#include "utils.h"
#include "draw.h"
#include "device.h"
#include "graphs.h"
#include "graphutils.h"
#include "protos.h"

extern char print_file[];

static void auto_ticks(int gno, int axis);

char *get_format_types(int f)
{
    char *s;

    s = "decimal";
    switch (f) {
    case FORMAT_DECIMAL:
	s = "decimal";
	break;
    case FORMAT_EXPONENTIAL:
	s = "exponential";
	break;
    case FORMAT_GENERAL:
	s = "general";
	break;
    case FORMAT_POWER:
	s = "power";
	break;
    case FORMAT_SCIENTIFIC:
	s = "scientific";
	break;
    case FORMAT_ENGINEERING:
	s = "engineering";
	break;
    case FORMAT_COMPUTING:
	s = "computing";
	break;
    case FORMAT_DDMMYY:
	s = "ddmmyy";
	break;
    case FORMAT_MMDDYY:
	s = "mmddyy";
	break;
    case FORMAT_YYMMDD:
	s = "yymmdd";
	break;
    case FORMAT_MMYY:
	s = "mmyy";
	break;
    case FORMAT_MMDD:
	s = "mmdd";
	break;
    case FORMAT_MONTHDAY:
	s = "monthday";
	break;
    case FORMAT_DAYMONTH:
	s = "daymonth";
	break;
    case FORMAT_MONTHS:
	s = "months";
	break;
    case FORMAT_MONTHSY:
	s = "monthsy";
	break;
    case FORMAT_MONTHL:
	s = "monthl";
	break;
    case FORMAT_DAYOFWEEKS:
	s = "dayofweeks";
	break;
    case FORMAT_DAYOFWEEKL:
	s = "dayofweekl";
	break;
    case FORMAT_DAYOFYEAR:
	s = "dayofyear";
	break;
    case FORMAT_HMS:
	s = "hms";
	break;
    case FORMAT_MMDDHMS:
	s = "mmddhms";
	break;
    case FORMAT_MMDDYYHMS:
	s = "mmddyyhms";
	break;
    case FORMAT_YYMMDDHMS:
	s = "yymmddhms";
	break;
    case FORMAT_DEGREESLON:
	s = "degreeslon";
	break;
    case FORMAT_DEGREESMMLON:
	s = "degreesmmlon";
	break;
    case FORMAT_DEGREESMMSSLON:
	s = "degreesmmsslon";
	break;
    case FORMAT_MMSSLON:
	s = "mmsslon";
	break;
    case FORMAT_DEGREESLAT:
	s = "degreeslat";
	break;
    case FORMAT_DEGREESMMLAT:
	s = "degreesmmlat";
	break;
    case FORMAT_DEGREESMMSSLAT:
	s = "degreesmmsslat";
	break;
    case FORMAT_MMSSLAT:
	s = "mmsslat";
	break;
    default:
	s = "unknown";
        errmsg("Internal error in get_format_types()");
	break;
    }
    return s;
}


int wipeout(void)
{
    if (!noask && is_dirtystate()) {
        if (!yesno("Abandon unsaved changes?", NULL, NULL, NULL)) {
            return 1;
        }
    }
    kill_all_graphs();
    do_clear_lines();
    do_clear_boxes();
    do_clear_ellipses();
    do_clear_text();
    kill_all_regions();
    reset_project_version();
    map_fonts(FONT_MAP_DEFAULT);
    set_docname(NULL);
    set_project_description(NULL);
    print_file[0] = '\0';
    /* a hack! the global "curtype" (as well as all others) should be removed */
    curtype = SET_XY;
    clear_dirtystate();
    return 0;
}


/* The following routines determine default axis range and tickmarks */

static void autorange_byset(int gno, int setno, int autos_type);
static double nicenum(double x, int nrange, int round);

#define NICE_FLOOR   0
#define NICE_CEIL    1
#define NICE_ROUND   2

void autotick_axis(int gno, int axis)
{
    switch (axis) {
    case ALL_AXES:
        auto_ticks(gno, X_AXIS);
        auto_ticks(gno, ZX_AXIS);
        auto_ticks(gno, Y_AXIS);
        auto_ticks(gno, ZY_AXIS);
        break;
    case ALL_X_AXES:
        auto_ticks(gno, X_AXIS);
        auto_ticks(gno, ZX_AXIS);
        break;
    case ALL_Y_AXES:
        auto_ticks(gno, Y_AXIS);
        auto_ticks(gno, ZY_AXIS);
        break;
    default:
        auto_ticks(gno, axis);
        break;
    }
}

void autoscale_byset(int gno, int setno, int autos_type)
{
    if ((setno == ALL_SETS && is_valid_gno(gno)) || is_set_active(gno, setno)) {
	autorange_byset(gno, setno, autos_type);
	switch (autos_type) {
        case AUTOSCALE_X:
            autotick_axis(gno, ALL_X_AXES);
            break;
        case AUTOSCALE_Y:
            autotick_axis(gno, ALL_Y_AXES);
            break;
        case AUTOSCALE_XY:
            autotick_axis(gno, ALL_AXES);
            break;
        }
    }
}

int autoscale_graph(int gno, int autos_type)
{
    if (number_of_active_sets(gno) > 0) {
        autoscale_byset(gno, ALL_SETS, autos_type);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

static void round_axis_limits(double *amin, double *amax, int scale)
{
    double smin, smax;
    int nrange;
    
    if (*amin == *amax) {
        switch (sign(*amin)) {
        case 0:
            *amin = -1.0;
            *amax = +1.0;
            break;
        case 1:
            *amin /= 2.0;
            *amax *= 2.0;
            break;
        case -1:
            *amin *= 2.0;
            *amax /= 2.0;
            break;
        }
    } 
    
    if (scale == SCALE_LOG) {
        if (*amax <= 0.0) {
            errmsg("Can't autoscale a log axis by non-positive data");
            *amax = 10.0;
            *amin = 1.0;
            return;
        } else if (*amin <= 0.0) {
            errmsg("Data have non-positive values");
            *amin = *amax/1.0e3;
        }
        smin = log10(*amin);
        smax = log10(*amax);
    } else if (scale == SCALE_LOGIT) {
	if (*amax <= 0.0) {
            errmsg("Can't autoscale a logit axis by non-positive data");
            *amax = 0.9;
            *amin = 0.1;
            return;
        } else if (*amin <= 0.0) {
            errmsg("Data have non-positive values");
            *amin = 0.1;
        }
        smin = log(*amin/(1-*amin));
        smax = log(*amax/(1-*amax));	
    } else {
        smin = *amin;
        smax = *amax;
    }

    if (sign(smin) == sign(smax)) {
        nrange = -rint(log10(fabs(2*(smax - smin)/(smax + smin))));
        nrange = MAX2(0, nrange);
    } else {
        nrange = 0;
    }
    smin = nicenum(smin, nrange, NICE_FLOOR);
    smax = nicenum(smax, nrange, NICE_CEIL);
    if (sign(smin) == sign(smax)) {
        if (smax/smin > 5.0) {
            smin = 0.0;
        } else if (smin/smax > 5.0) {
            smax = 0.0;
        }
    }
    
    if (scale == SCALE_LOG) {
        *amin = pow(10.0, smin);
        *amax = pow(10.0, smax);
    } else if (scale == SCALE_LOGIT) {
	*amin = exp(smin)/(1.0 + exp(smin));
        *amax = exp(smax)/(1.0 + exp(smax));	
    } else {
        *amin = smin;
        *amax = smax;
    }
}

static void autorange_byset(int gno, int setno, int autos_type)
{
    world w;
    double xmax, xmin, ymax, ymin;
    int scale;

    if (autos_type == AUTOSCALE_NONE) {
        return;
    }
    
    get_graph_world(gno, &w);
    
    if (get_graph_type(gno) == GRAPH_SMITH) {
        if (autos_type == AUTOSCALE_X || autos_type == AUTOSCALE_XY) {
            w.xg1 = -1.0;
            w.yg1 = -1.0;
        }
        if (autos_type == AUTOSCALE_Y || autos_type == AUTOSCALE_XY) {
            w.xg2 = 1.0;
            w.yg2 = 1.0;
	}
        set_graph_world(gno, w);
        return;
    }

    xmin=w.xg1;
    xmax=w.xg2;
    ymin=w.yg1;
    ymax=w.yg2;
    if (autos_type == AUTOSCALE_XY) {
        getsetminmax(gno, setno, &xmin, &xmax, &ymin, &ymax);
    } else if (autos_type == AUTOSCALE_X) {
        getsetminmax_c(gno, setno, &xmin, &xmax, &ymin, &ymax, 2);
    } else if (autos_type == AUTOSCALE_Y) {
        getsetminmax_c(gno, setno, &xmin, &xmax, &ymin, &ymax, 1);
    }

    if (autos_type == AUTOSCALE_X || autos_type == AUTOSCALE_XY) {
        scale = get_graph_xscale(gno);
        round_axis_limits(&xmin, &xmax, scale);
        w.xg1 = xmin;
        w.xg2 = xmax;
    }

    if (autos_type == AUTOSCALE_Y || autos_type == AUTOSCALE_XY) {
        scale = get_graph_yscale(gno);
        round_axis_limits(&ymin, &ymax, scale);
        w.yg1 = ymin;
        w.yg2 = ymax;
    }

    set_graph_world(gno, w);
}

static void auto_ticks(int gno, int axis)
{
    tickmarks *t;
    world w;
    double range, d, tmpmax, tmpmin;
    int axis_scale;

    t = get_graph_tickmarks(gno, axis);
    if (t == NULL) {
        return;
    }
    get_graph_world(gno, &w);

    if (is_xaxis(axis)) {
        tmpmin = w.xg1;
        tmpmax = w.xg2;
        axis_scale = get_graph_xscale(gno);
    } else {
        tmpmin = w.yg1;
        tmpmax = w.yg2;
        axis_scale = get_graph_yscale(gno);
    }

    if (axis_scale == SCALE_LOG) {
	if (t->tmajor <= 1.0) {
            t->tmajor = 10.0;
        }
        tmpmax = log10(tmpmax)/log10(t->tmajor);
	tmpmin = log10(tmpmin)/log10(t->tmajor);
    } else if (axis_scale == SCALE_LOGIT) {
    	if (t->tmajor >= 0.5) {
            t->tmajor = 0.4;
        }
        tmpmax = log(tmpmax/(1-tmpmax))/log(t->tmajor/(1-t->tmajor));
	tmpmin = log(tmpmin/(1-tmpmin))/log(t->tmajor/(1-t->tmajor)); 
    } else if (t->tmajor <= 0.0) {
        t->tmajor = 1.0;
    }
    
    range = tmpmax - tmpmin;
    if (axis_scale == SCALE_LOG) {
	d = ceil(range/(t->t_autonum - 1));
	t->tmajor = pow(t->tmajor, d);
    } 
    else if (axis_scale == SCALE_LOGIT ){
        d = ceil(range/(t->t_autonum - 1));
	t->tmajor = exp(d)/(1.0 + exp(d));
    } 
    else {
	d = nicenum(range/(t->t_autonum - 1), 0, NICE_ROUND);
	t->tmajor = d;
    }

    /* alter # of minor ticks only if the current value is anomalous */
    if (t->nminor < 0 || t->nminor > 10) {
        if (axis_scale != SCALE_LOG) {
	    t->nminor = 1;
        } else {
            t->nminor = 8;
        }
    }
    
    set_dirtystate();
}

/*
 * nicenum: find a "nice" number approximately equal to x
 */

static double nicenum(double x, int nrange, int round)
{
    int xsign;
    double f, y, fexp, rx, sx;
    
    if (x == 0.0) {
        return(0.0);
    }

    xsign = sign(x);
    x = fabs(x);

    fexp = floor(log10(x)) - nrange;
    sx = x/pow(10.0, fexp)/10.0;            /* scaled x */
    rx = floor(sx);                         /* rounded x */
    f = 10*(sx - rx);                       /* fraction between 0 and 10 */

    if ((round == NICE_FLOOR && xsign == +1) ||
        (round == NICE_CEIL  && xsign == -1)) {
        y = (int) floor(f);
    } else if ((round == NICE_FLOOR && xsign == -1) ||
               (round == NICE_CEIL  && xsign == +1)) {
	y = (int) ceil(f);
    } else {    /* round == NICE_ROUND */
	if (f < 1.5)
	    y = 1;
	else if (f < 3.)
	    y = 2;
	else if (f < 7.)
	    y = 5;
	else
	    y = 10;
    }
    
    sx = rx + (double) y/10.0;
    
    return (xsign*sx*10.0*pow(10.0, fexp));
}

/*
 * set scroll amount
 */
void scroll_proc(int value)
{
    scrollper = value / 100.0;
}

void scrollinout_proc(int value)
{
    shexper = value / 100.0;
}

/*
 * pan through world coordinates
 */
int graph_scroll(int type)
{
    world w;
    double xmax, xmin, ymax, ymin;
    double dwc;
    int gstart, gstop, i;

    if (scrolling_islinked) {
        gstart = 0;
        gstop = number_of_graphs() - 1;
    } else {
        gstart = get_cg();
        gstop = gstart;
    }
    
    for (i = gstart; i <= gstop; i++) {
        if (get_graph_world(i, &w) == RETURN_SUCCESS) {
	    if (islogx(i) == TRUE) {
		xmin = log10(w.xg1);
		xmax = log10(w.xg2);
	    } else {
		xmin = w.xg1;
		xmax = w.xg2;
	    }
	    
	    if (islogy(i) == TRUE) {
		ymin = log10(w.yg1);
		ymax = log10(w.yg2);
	    } else {
		ymin = w.yg1;
		ymax = w.yg2;
	    }

	    dwc = 1.0;
            switch (type) {
            case GSCROLL_LEFT:
		dwc = -1.0;
	    case GSCROLL_RIGHT:    
                dwc *= scrollper * (xmax - xmin);
                xmin += dwc;
                xmax += dwc;
                break;
            case GSCROLL_DOWN:
		dwc = -1.0;
	    case GSCROLL_UP:    
                dwc *= scrollper * (ymax - ymin);
                ymin += dwc;
                ymax += dwc;
                break;
            }

	    if (islogx(i) == TRUE) {
		w.xg1 = pow(10.0, xmin);
		w.xg2 = pow(10.0, xmax);
	    } else {
		w.xg1 = xmin;
		w.xg2 = xmax;
	    }
	    
	    if (islogy(i) == TRUE) {
		w.yg1 = pow(10.0, ymin);
		w.yg2 = pow(10.0, ymax);
	    } else {
		w.yg1 = ymin;
		w.yg2 = ymax;
	    }
            set_graph_world(i, w);
        }
    }
    
    return RETURN_SUCCESS;
}

int graph_zoom(int type)
{
    double dx, dy;
    double xmax, xmin, ymax, ymin;
    world w;
    int gstart, gstop, gno;

    if (scrolling_islinked) {
        gstart = 0;
        gstop = number_of_graphs() - 1;
    } else {
        gstart = get_cg();
        gstop = gstart;
    }
    
    for (gno = gstart; gno <= gstop; gno++) {
	if (get_graph_world(gno, &w) == RETURN_SUCCESS) {
	    if (islogx(gno) == TRUE) {
		xmin = log10(w.xg1);
		xmax = log10(w.xg2);
	    } else {
		xmin = w.xg1;
		xmax = w.xg2;
	    }
	    
	    if (islogy(gno) == TRUE) {
		ymin = log10(w.yg1);
		ymax = log10(w.yg2);
	    } else {
		ymin = w.yg1;
		ymax = w.yg2;
	    }
	    
	    dx = shexper * (xmax - xmin);
	    dy = shexper * (ymax - ymin);
	    if (type == GZOOM_SHRINK) {
		dx *= -1;
		dy *= -1;
	    }

	    xmin -= dx;
	    xmax += dx;
	    ymin -= dy;
	    ymax += dy;
	
	    if (islogx(gno) == TRUE) {
		w.xg1 = pow(10.0, xmin);
		w.xg2 = pow(10.0, xmax);
	    } else {
		w.xg1 = xmin;
		w.xg2 = xmax;
	    }
	    
	    if (islogy(gno) == TRUE) {
		w.yg1 = pow(10.0, ymin);
		w.yg2 = pow(10.0, ymax);
	    } else {
		w.yg1 = ymin;
		w.yg2 = ymax;
	    }
 
            set_graph_world(gno, w);
	}
    }
    
    return RETURN_SUCCESS;
}

/*
 *  Arrange graphs
 */
int arrange_graphs(int *graphs, int ngraphs,
                   int nrows, int ncols, int order, int snake,
                   double loff, double roff, double toff, double boff,
                   double vgap, double hgap,
                   int hpack, int vpack)
{
    int i, imax, j, jmax, iw, ih, ng, gno;
    double pw, ph, w, h;
    view v;

    if (hpack) {
        hgap = 0.0;
    }
    if (vpack) {
        vgap = 0.0;
    }
    if (ncols < 1 || nrows < 1) {
	errmsg("# of rows and columns must be > 0");
        return RETURN_FAILURE;
    }
    if (hgap < 0.0 || vgap < 0.0) {
	errmsg("hgap and vgap must be >= 0");
        return RETURN_FAILURE;
    }
    
    get_page_viewport(&pw, &ph);
    w = (pw - loff - roff)/(ncols + (ncols - 1)*hgap);
    h = (ph - toff - boff)/(nrows + (nrows - 1)*vgap);
    if (h <= 0.0 || w <= 0.0) {
	errmsg("Page offsets are too large");
        return RETURN_FAILURE;
    }
    
    ng = 0;
    if (order & GA_ORDER_HV_INV) {
        imax = ncols;
        jmax = nrows;
    } else {
        imax = nrows;
        jmax = ncols;
    }
    for (i = 0; i < imax && ng < ngraphs; i++) {
        for (j = 0; j < jmax && ng < ngraphs; j++) {
            gno = graphs[ng];
            set_graph_active(gno);
            
            if (order & GA_ORDER_HV_INV) {
                iw = i;
                ih = j;
                if (snake && (iw % 2)) {
                    ih = nrows - ih - 1;
                }
            } else {
                iw = j;
                ih = i;
                if (snake && (ih % 2)) {
                    iw = ncols - iw - 1;
                }
            }
            if (order & GA_ORDER_H_INV) {
                iw = ncols - iw - 1;
            }
            /* viewport y coord goes bottom -> top ! */
            if (!(order & GA_ORDER_V_INV)) {
                ih = nrows - ih - 1;
            }
            
            v.xv1 = loff + iw*w*(1.0 + hgap);
            v.xv2 = v.xv1 + w;
            v.yv1 = boff + ih*h*(1.0 + vgap);
            v.yv2 = v.yv1 + h;
            set_graph_viewport(gno, v);
            
            if (hpack) {
                if (iw == 0) {
	            tickmarks *t = get_graph_tickmarks(gno, Y_AXIS);
	            if (!t) {
                        continue;
                    }
                    t->active = TRUE;
	            t->label_op = PLACEMENT_NORMAL;
	            t->t_op = PLACEMENT_NORMAL;
	            t->tl_op = PLACEMENT_NORMAL;
                } else {
                    activate_tick_labels(gno, Y_AXIS, FALSE);
                }
            }
            if (vpack) {
                if (ih == 0) {
	            tickmarks *t = get_graph_tickmarks(gno, X_AXIS);
	            if (!t) {
                        continue;
                    }
	            t->active = TRUE;
	            t->label_op = PLACEMENT_NORMAL;
	            t->t_op = PLACEMENT_NORMAL;
	            t->tl_op = PLACEMENT_NORMAL;
                } else {
                    activate_tick_labels(gno, X_AXIS, FALSE);
                }
            }
            
            ng++;
        }
    }
    return RETURN_SUCCESS;
}

int arrange_graphs_simple(int nrows, int ncols,
    int order, int snake, double offset, double hgap, double vgap)
{
    int *graphs, i, ngraphs, retval;
    
    ngraphs = nrows*ncols;
    graphs = xmalloc(ngraphs*SIZEOF_INT);
    if (graphs == NULL) {
        return RETURN_FAILURE;
    }
    
    for (i = 0; i < ngraphs; i++) {
        graphs[i] = i;
    }
    
    for (i = number_of_graphs() - 1; i >= ngraphs; i--) {
        kill_graph(i);
    }
    
    retval = arrange_graphs(graphs, ngraphs, nrows, ncols, order, snake,
        offset, offset, offset, offset, vgap, hgap, FALSE, FALSE);
    
    xfree(graphs);
    
    return retval;
}

void move_legend(int gno, VVector shift)
{
    double xtmp, ytmp;
    legend l;

    if (is_valid_gno(gno)) {
        get_graph_legend(gno, &l);
        if (l.loctype == COORD_VIEW) {
            l.legx += shift.x;
            l.legy += shift.y;
        } else {
            world2view(l.legx, l.legy, &xtmp, &ytmp);
            xtmp += shift.x;
            ytmp += shift.y;
            view2world(xtmp, ytmp, &l.legx, &l.legy);
        }
        set_graph_legend(gno, &l);
        set_dirtystate();
    }
}

void move_timestamp(VVector shift)
{
    timestamp.x += shift.x;
    timestamp.y += shift.y;
    set_dirtystate();
}

void rescale_viewport(double ext_x, double ext_y)
{
    int i, gno;
    view v;
    legend leg;
    linetype l;
    boxtype b;
    ellipsetype e;
    plotstr s;
    
    for (gno = 0; gno < number_of_graphs(); gno++) {
        get_graph_viewport(gno, &v);
        v.xv1 *= ext_x;
        v.xv2 *= ext_x;
        v.yv1 *= ext_y;
        v.yv2 *= ext_y;
        set_graph_viewport(gno, v);
        
        get_graph_legend(gno, &leg);
        if (leg.loctype == COORD_VIEW) {
            leg.legx *= ext_x;
            leg.legy *= ext_y;
            set_graph_legend(gno, &leg);
        }
        
        /* TODO: tickmark offsets */
    }

    for (i = 0; i < number_of_lines(); i++) {
        get_graph_line(i, &l);
        if (l.loctype == COORD_VIEW) {
            l.x1 *= ext_x;
            l.x2 *= ext_x;
            l.y1 *= ext_y;
            l.y2 *= ext_y;
            set_graph_line(i, &l);
        }
    }
    for (i = 0; i < number_of_boxes(); i++) {
        get_graph_box(i, &b);
        if (b.loctype == COORD_VIEW) {
            b.x1 *= ext_x;
            b.x2 *= ext_x;
            b.y1 *= ext_y;
            b.y2 *= ext_y;
            set_graph_box(i, &b);
        }
    }
    for (i = 0; i < number_of_ellipses(); i++) {
        get_graph_ellipse(i, &e);
        if (e.loctype == COORD_VIEW) {
            e.x1 *= ext_x;
            e.x2 *= ext_x;
            e.y1 *= ext_y;
            e.y2 *= ext_y;
            set_graph_ellipse(i, &e);
        }
    }
    for (i = 0; i < number_of_strings(); i++) {
        get_graph_string(i, &s);
        if (s.loctype == COORD_VIEW) {
            s.x *= ext_x;
            s.y *= ext_y;
            set_graph_string(i, &s);
        }
    }
}

int overlay_graphs(int gsec, int gpri, int type)
{
    int i;
    tickmarks *tsec, *tpri;
    world wpri, wsec;
    view v;
    
    if (gsec == gpri) {
        return RETURN_FAILURE;
    }
    if (is_valid_gno(gpri) == FALSE || is_valid_gno(gsec) == FALSE) {
        return RETURN_FAILURE;
    }
    
    get_graph_viewport(gpri, &v);
    get_graph_world(gpri, &wpri);
    get_graph_world(gsec, &wsec);

    switch (type) {
    case GOVERLAY_SMART_AXES_XY:
        wsec = wpri;
	for (i = 0; i < MAXAXES; i++) {
	    tpri = get_graph_tickmarks(gpri, i);
	    tsec = get_graph_tickmarks(gsec, i);
            switch(i) {
            case X_AXIS:
            case Y_AXIS:
                tpri->active = TRUE;
	        tpri->label_op = PLACEMENT_NORMAL;
	        tpri->t_op = PLACEMENT_BOTH;
	        tpri->tl_op = PLACEMENT_NORMAL;

	        tsec->active = FALSE;
                break;
            default:
                /* don't touch alternative axes */
                break;
            }
	}
	break;
    case GOVERLAY_SMART_AXES_X:
        wsec.xg1 = wpri.xg1;
        wsec.xg2 = wpri.xg2;
	for (i = 0; i < MAXAXES; i++) {
	    tpri = get_graph_tickmarks(gpri, i);
	    tsec = get_graph_tickmarks(gsec, i);
	    switch(i) {
            case X_AXIS:
                tpri->active = TRUE;
	        tpri->label_op = PLACEMENT_NORMAL;
	        tpri->t_op = PLACEMENT_BOTH;
	        tpri->tl_op = PLACEMENT_NORMAL;

	        tsec->active = FALSE;
                break;
            case Y_AXIS:
	        tpri->active = TRUE;
	        tpri->label_op = PLACEMENT_NORMAL;
	        tpri->t_op = PLACEMENT_NORMAL;
	        tpri->tl_op = PLACEMENT_NORMAL;

                tsec->active = TRUE;
	        tsec->label_op = PLACEMENT_OPPOSITE;
	        tsec->t_op = PLACEMENT_OPPOSITE;
	        tsec->tl_op = PLACEMENT_OPPOSITE;
                break;
            default:
                /* don't touch alternative axes */
                break;
            }
	}
	break;
    case GOVERLAY_SMART_AXES_Y:
        wsec.yg1 = wpri.yg1;
        wsec.yg2 = wpri.yg2;
	for (i = 0; i < MAXAXES; i++) {
	    tpri = get_graph_tickmarks(gpri, i);
	    tsec = get_graph_tickmarks(gsec, i);
	    switch(i) {
            case X_AXIS:
	        tpri->active = TRUE;
	        tpri->label_op = PLACEMENT_NORMAL;
	        tpri->t_op = PLACEMENT_NORMAL;
	        tpri->tl_op = PLACEMENT_NORMAL;

                tsec->active = TRUE;
	        tsec->label_op = PLACEMENT_OPPOSITE;
	        tsec->t_op = PLACEMENT_OPPOSITE;
	        tsec->tl_op = PLACEMENT_OPPOSITE;
                break;
            case Y_AXIS:
                tpri->active = TRUE;
	        tpri->label_op = PLACEMENT_NORMAL;
	        tpri->t_op = PLACEMENT_BOTH;
	        tpri->tl_op = PLACEMENT_NORMAL;

	        tsec->active = FALSE;
                break;
            default:
                /* don't touch alternative axes */
                break;
            }
	}
	break;
    case GOVERLAY_SMART_AXES_NONE:
	for (i = 0; i < MAXAXES; i++) {
	    tpri = get_graph_tickmarks(gpri, i);
	    tsec = get_graph_tickmarks(gsec, i);
	    switch(i) {
            case X_AXIS:
            case Y_AXIS:
	        tpri->active = TRUE;
	        tpri->label_op = PLACEMENT_NORMAL;
	        tpri->t_op = PLACEMENT_NORMAL;
	        tpri->tl_op = PLACEMENT_NORMAL;

                tsec->active = TRUE;
	        tsec->label_op = PLACEMENT_OPPOSITE;
	        tsec->t_op = PLACEMENT_OPPOSITE;
	        tsec->tl_op = PLACEMENT_OPPOSITE;
                break;
            default:
                /* don't touch alternative axes */
                break;
            }
	}
	break;
    default:
        break;
    }
    
    /* set identical viewports */
    set_graph_viewport(gsec, v);
    
    /* update world coords */
    set_graph_world(gsec, wsec);

    return RETURN_SUCCESS;
}
