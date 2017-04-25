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

#include "defines.h"

#define GMF_VERSION "0.9"

int mfinitgraphics(void);

void mf_drawpixel(VPoint vp);
void mf_drawpolyline(VPoint *vps, int n, int mode);
void mf_fillpolygon(VPoint *vps, int nc);
void mf_drawarc(VPoint vp1, VPoint vp2, int a1, int a2);
void mf_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode);
void mf_putpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type);
void mf_puttext (VPoint vp, char *s, int len, int font,
     TextMatrix *tm, int underline, int overline, int kerning);

void mf_leavegraphics(void);
