/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2002 Grace Development Team
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
 * Global variables of Grace - should be empty :-(
 *
 */

#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "graphs.h"
#include "draw.h"

#ifdef MAIN
#  define GLOBAL(var, type, val) type var = val
#  define GLOBALARR(arr, type, dim, val) type arr[dim] = val

/* target set */
target target_set;
/* timestamp */
plotstr timestamp;
/* default properties */
defaults grdefaults;

/* parameters for non-linear fit */
nonlparms nonl_parms[MAXPARM];
nonlopts nonl_opts;

/* region definition */
region rg[MAXREGION];

#else
#  define GLOBAL(var, type, val) extern type var
#  define GLOBALARR(arr, type, dim, val) extern type arr[]

extern target target_set;
extern defaults grdefaults;
extern plotstr timestamp;

extern nonlparms nonl_parms[];
extern nonlopts nonl_opts;

extern region rg[];

#endif

/* real-time input delay (prevents getting stuck reading) */
GLOBAL(timer_delay, int, 200);

/* linked scroll */
GLOBAL(scrolling_islinked, int, FALSE);
/* scroll fraction */
GLOBAL(scrollper, double, 0.05);
/* expand/shrink fraction */
GLOBAL(shexper, double, 0.05);

/* terminal device */
GLOBAL(tdevice, int, 0);
/* hardcopy device */
GLOBAL(hdevice, int, 0);

/* set mono mode */
GLOBAL(monomode, int, FALSE);
/* use GXxor or GXinvert for xor'ing */
GLOBAL(invert, int, TRUE);
/* if true, redraw graph each time action is performed */
GLOBAL(auto_redraw, int, TRUE);
/* allow double click ops */
GLOBAL(allow_dc, int, TRUE);
/* if TRUE, assume yes for everything */
GLOBAL(noask, int, FALSE);

/* true if running X */
GLOBAL(inwin, int, FALSE);

/* autoscale after reading in data sets */
GLOBAL(autoscale_onread, int, AUTOSCALE_XY);

GLOBAL(focus_policy, int, FOCUS_CLICK);
GLOBAL(draw_focus_flag, int, TRUE);

/* lines */
GLOBAL(lines, linetype *, NULL);
/* boxes */
GLOBAL(boxes, boxtype *, NULL);
/* ellipses */
GLOBAL(ellip, ellipsetype *, NULL);
/* strings */
GLOBAL(pstr, plotstr *, NULL);

/* lines, boxes, ellipses and strings flags */
GLOBAL(box_color, int, 1);
GLOBAL(box_lines, int, 1);
GLOBAL(box_linew, double, 1.0);
GLOBAL(box_fillpat, int, 0);
GLOBAL(box_fillcolor, int, 1);
GLOBAL(box_loctype, int, COORD_VIEW);

GLOBAL(ellipse_color, int, 1);
GLOBAL(ellipse_lines, int, 1);
GLOBAL(ellipse_linew, double, 1.0);
GLOBAL(ellipse_fillpat, int, 0);
GLOBAL(ellipse_fillcolor, int, 1);
GLOBAL(ellipse_loctype, int, COORD_VIEW);

GLOBAL(line_color, int, 1);
GLOBAL(line_lines, int, 1);
GLOBAL(line_linew, double, 1.0);
GLOBAL(line_arrow_end, int, 0);
GLOBAL(line_atype, int, 0);
GLOBAL(line_asize, double, 1.0);
GLOBAL(line_a_dL_ff, double, 1.0);
GLOBAL(line_a_lL_ff, double, 1.0);
GLOBAL(line_loctype, int, COORD_VIEW);

GLOBAL(string_color, int, 1);
GLOBAL(string_font, int, 0);
GLOBAL(string_rot, int, 0);
GLOBAL(string_just, int, 0);
GLOBAL(string_size, double, 1.0);
GLOBAL(string_loctype, int, COORD_VIEW);


/* used in the parser */
GLOBAL(curtype, int, SET_XY);
GLOBAL(cursource, int, SOURCE_DISK);

/* the current region */
GLOBAL(nr, int, 0);

/* file for results */
GLOBAL(resfp, FILE *, NULL);

/* format for saving data sets */
GLOBALARR(sformat, char, 128, "%.8g");

#ifdef HAVE_NETCDF
GLOBALARR(netcdf_name, char, 512, "");
GLOBALARR(xvar_name, char, 128, "");
GLOBALARR(yvar_name, char, 128, "");
#endif

/* the safe mode flag */
GLOBAL(safe_mode, int, TRUE);

#endif /* __GLOBALS_H_ */
