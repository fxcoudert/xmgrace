/*
 * Grace - Graphics for Exploratory Data Analysis
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

/*
 *
 * routines to allocate, manipulate, and return
 * information about regions.
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"

#include "draw.h"
#include "graphs.h"
#include "utils.h"
#include "protos.h"

int regiontype = 0;

/*
 * see if (x,y) lies inside the plot
 */
int inbounds(int gno, double x, double y)
{
    WPoint wp;
    
    wp.x = x;
    wp.y = y;
    return is_validWPoint(wp);
}

int isactive_region(int regno)
{
    return (regno == MAXREGION || regno == MAXREGION + 1 || rg[regno].active == TRUE);
}

char *region_types(int it, int which)
{
    char *s;

    s = "UNDEFINED";
    switch (it) {
    case REGION_TOLEFT:
	s = "REGION_TOLEFT";
	break;
    case REGION_TORIGHT:
	s = "REGION_TORIGHT";
	break;
    case REGION_ABOVE:
	s = "REGION_ABOVE";
	break;
    case REGION_BELOW:
	s = "REGION_BELOW";
	break;
    case REGION_POLYI:
	if (which) {
	    s = "REGION_POLYI";
	} else {
	    s = "INSIDE POLY";
	}
	break;
    case REGION_POLYO:
	if (which) {
	    s = "REGION_POLYO";
	} else {
	    s = "OUTSIDE POLY";
	}
	break;
    case REGION_HORIZI:
      s ="REGION_HORIZI";
      break;
    case REGION_VERTI:
      s ="REGION_VERTI";
      break;
    case REGION_HORIZO:
       s ="REGION_HORIZO";
      break;
    case REGION_VERTO:
      s ="REGION_VERTO";
      break;

    }
    return s;
}

void kill_region(int r)
{
    if (rg[r].active) {
	XCFREE(rg[r].x);
	XCFREE(rg[r].y);
        rg[r].active = FALSE;
        rg[r].n = 0;
        set_dirtystate();
    }
}

void kill_all_regions(void)
{
    int r;
    for (r = 0; r < MAXREGION; r++) {
        kill_region(r);
    }
}

void activate_region(int r, int type, int linkto)
{
    kill_region(r);
    rg[r].active = TRUE;
    rg[r].type = type;
    rg[r].linkto = linkto;
    set_dirtystate();
}


/*
 * report on sets in a region
 */
void reporton_region(int gno, int rno, int type)
{
    char buf[256];
    int i, j, first, contained;
    double *x, *y;
    sprintf(buf, "\nRegion R%1d contains:\n", rno);
    stufftext(buf);
    for (j = 0; j < number_of_sets(gno); j++) {
	if (is_set_active(gno, j)) {
	    x = getx(gno, j);
	    y = gety(gno, j);
	    first = 1;
	    contained = 0;
	    for (i = 0; i < getsetlength(gno, j); i++) {
		if (inregion(rno, x[i], y[i])) {
		    contained = 1;
		    switch (type) {
		    case 0:	/* report on sets */
			if (first) {
			    first = 0;
			    sprintf(buf, "  Set S%1d\n", j);
			    stufftext(buf);
			}
			break;
		    case 1:	/* points */
			if (first) {
			    first = 0;
			    sprintf(buf, "  Set S%1d\n", j);
			    stufftext(buf);
			}
			sprintf(buf, "    %d %f %f\n", i + 1, x[i], y[i]);
			stufftext(buf);
			break;
		    }
		} else {
		    contained = 0;
		}
	    }
	}
    }
    stufftext("\n");
}

void load_poly_region(int r, int gno, int n, WPoint *wps)
{
    int i;

    if (n > 2) {
	activate_region(r, regiontype, gno);
	rg[r].n = n;
	rg[r].x = xcalloc(n, SIZEOF_DOUBLE);
	rg[r].y = xcalloc(n, SIZEOF_DOUBLE);
	for (i = 0; i < n; i++) {
	    rg[r].x[i] = wps[i].x;
	    rg[r].y[i] = wps[i].y;
	}
    }
}

/*
 * routines to determine if a point lies in a polygon
*/
int intersect_to_left(double x, double y, double x1, double y1, double x2, double y2)
{
    double xtmp, m, b;

    /* ignore horizontal lines */
    if (y1 == y2) {
	return 0;
    }
    /* not contained vertically */
    if (((y < y1) && (y < y2)) || ((y > y1) && (y > y2))) {
	return 0;
    }
    /* none of the above, compute the intersection */
    if ((xtmp = x2 - x1) != 0.0) {
	m = (y2 - y1) / xtmp;
	b = y1 - m * x1;
	xtmp = (y - b) / m;
    } else {
	xtmp = x1;
    }
    if (xtmp <= x) {
	/* check for intersections at a vertex */
	/* if this is the max ordinate then accept */
	if (y == y1) {
	    if (y1 > y2) {
		return 1;
	    } else {
		return 0;
	    }
	}
	/* check for intersections at a vertex */
	if (y == y2) {
	    if (y2 > y1) {
		return 1;
	    } else {
		return 0;
	    }
	}
	/* no vertices intersected */
	return 1;
    }
    return 0;
}

/*
 * determine if (x,y) is in the polygon xlist[], ylist[]
 */
int inbound(double x, double y, double *xlist, double *ylist, int n)
{
    int i, l = 0;

    for (i = 0; i < n; i++) {
	l += intersect_to_left(x, y, xlist[i], ylist[i], xlist[(i + 1) % n], ylist[(i + 1) % n]);
    }
    return l % 2;
}

/*
 * routines to determine if a point lies to the left of an infinite line
*/
int isleft(double x, double y, double x1, double y1, double x2, double y2)
{
    double xtmp, m, b;

    /* horizontal lines */
    if (y1 == y2) {
	return 0;
    }
    /* none of the above, compute the intersection */
    if ((xtmp = x2 - x1) != 0.0) {
	m = (y2 - y1) / xtmp;
	b = y1 - m * x1;
	xtmp = (y - b) / m;
    } else {
	xtmp = x1;
    }
    if (xtmp >= x) {
	return 1;
    }
    return 0;
}


/*
 * routines to determine if a point lies to the left of an infinite line
*/
int isright(double x, double y, double x1, double y1, double x2, double y2)
{
    double xtmp, m, b;

    /* horizontal lines */
    if (y1 == y2) {
	return 0;
    }
    if ((xtmp = x2 - x1) != 0.0) {
	m = (y2 - y1) / xtmp;
	b = y1 - m * x1;
	xtmp = (y - b) / m;
    } else {
	xtmp = x1;
    }
    if (xtmp <= x) {
	return 1;
    }
    return 0;
}

/*
 * routines to determine if a point lies above an infinite line
*/
int isabove(double x, double y, double x1, double y1, double x2, double y2)
{
    double ytmp, m, b;

    /* vertical lines */
    if (x1 == x2) {
	return 0;
    }
    if ((ytmp = y2 - y1) != 0.0) {
	m = ytmp / (x2 - x1);
	b = y1 - m * x1;
	ytmp = m * x + b;
    } else {
	ytmp = y1;
    }
    if (ytmp <= y) {
	return 1;
    }
    return 0;
}

/*
 * routines to determine if a point lies below an infinite line
*/
int isbelow(double x, double y, double x1, double y1, double x2, double y2)
{
    double ytmp, m, b;

    /* vertical lines */
    if (x1 == x2) {
	return 0;
    }
    if ((ytmp = y2 - y1) != 0.0) {
	m = ytmp / (x2 - x1);
	b = y1 - m * x1;
	ytmp = m * x + b;
    } else {
	ytmp = y1;
    }
    if (ytmp >= y) {
	return 1;
    }
    return 0;
}

int inregion(int regno, double x, double y)
{
    if (regno == MAXREGION) {
	return (inbounds(get_cg() , x, y));
    }
    if (regno == MAXREGION + 1) {
	return (!inbounds(get_cg() , x, y));
    }
    if (rg[regno].active == TRUE) {
	switch (rg[regno].type) {
	case REGION_POLYI:
	    if (inbound(x, y, rg[regno].x, rg[regno].y, rg[regno].n)) {
		return 1;
	    }
	    break;
	case REGION_POLYO:
	    if (!inbound(x, y, rg[regno].x, rg[regno].y, rg[regno].n)) {
		return 1;
	    }
	    break;
	case REGION_TORIGHT:
	    if (isright(x, y, rg[regno].x1, rg[regno].y1, rg[regno].x2, rg[regno].y2)) {
		return 1;
	    }
	    break;
	case REGION_TOLEFT:
	    if (isleft(x, y, rg[regno].x1, rg[regno].y1, rg[regno].x2, rg[regno].y2)) {
		return 1;
	    }
	    break;
	case REGION_ABOVE:
	    if (isabove(x, y, rg[regno].x1, rg[regno].y1, rg[regno].x2, rg[regno].y2)) {
		return 1;
	    }
	    break;
	case REGION_BELOW:
	    if (isbelow(x, y, rg[regno].x1, rg[regno].y1, rg[regno].x2, rg[regno].y2)) {
		return 1;
	    }
	    break;
	case REGION_HORIZI:
	  return (x >= rg[regno].x1) && ( x <= rg[regno].x2);
	  break;
	case REGION_VERTI:
	  return (y >= rg[regno].y1) && ( y <= rg[regno].y2);
	  break;
	case REGION_HORIZO:
	  return !( (x >= rg[regno].x1) && ( x <= rg[regno].x2) );
	  break;
	case REGION_VERTO:
	  return !( (y >= rg[regno].y1) && ( y <= rg[regno].y2) );
	  break;

	}
    }
    return 0;
}
