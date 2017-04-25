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

#include "defines.h"

#define PS_FORMAT   0
#define EPS_FORMAT  1
/* #define EPSI_FORMAT  2 */

#define DEFAULT_PS_FORMAT PS_FORMAT

#define MEDIA_FEED_AUTO    0  
#define MEDIA_FEED_MATCH   1
#define MEDIA_FEED_MANUAL  2

#define DOCDATA_7BIT    0  
#define DOCDATA_8BIT    1  
#define DOCDATA_BINARY  2  

#define MAX_PS_LINELEN   70

int psprintinitgraphics(void);
int epsinitgraphics(void);

void ps_drawpixel(VPoint vp);
void ps_drawpolyline(VPoint *vps, int n, int mode);
void ps_fillpolygon(VPoint *vps, int nc);
void ps_drawarc(VPoint vp1, VPoint vp2, int a1, int a2);
void ps_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode);
void ps_putpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type);
void ps_puttext(VPoint vp, char *s, int len, int font,
     TextMatrix *tm, int underline, int overline, int kerning);

void ps_leavegraphics(void);

int ps_op_parser(char *opstring);
int eps_op_parser(char *opstring);

#if defined(NONE_GUI)
#  define ps_gui_setup NULL
#else
void ps_gui_setup(void);
#endif

#if defined(NONE_GUI)
#  define eps_gui_setup NULL
#else
void eps_gui_setup(void);
#endif
