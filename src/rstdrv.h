/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-99 Grace Development Team
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

typedef enum {
    RST_FORMAT_PNM,
    RST_FORMAT_JPG,
    RST_FORMAT_PNG
} RasterFormat;

#  define DEFAULT_RASTER_FORMAT RST_FORMAT_PNM

/* PNM sub-formats */
#define PNM_FORMAT_PBM  0
#define PNM_FORMAT_PGM  1
#define PNM_FORMAT_PPM  2

#define DEFAULT_PNM_FORMAT PNM_FORMAT_PPM

#ifdef HAVE_LIBJPEG
#  define JPEG_DCT_IFAST  0
#  define JPEG_DCT_ISLOW  1
#  define JPEG_DCT_FLOAT  2

#define JPEG_DCT_DEFAULT    JPEG_DCT_ISLOW
#endif

#define INTENSITY(r, g, b) ((299*r + 587*g + 114*b)/1000)

void rst_drawpixel(VPoint vp);
void rst_drawpolyline(VPoint *vps, int n, int mode);
void rst_fillpolygon(VPoint *vps, int nc);
void rst_drawarc(VPoint vp1, VPoint vp2, int a1, int a2);
void rst_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode);
void rst_putpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type);
void rst_leavegraphics(void);

int pnminitgraphics(void);
int pnm_op_parser(char *opstring);
#ifdef NONE_GUI
#  define pnm_gui_setup NULL
#else
void pnm_gui_setup(void);
#endif

#ifdef HAVE_LIBJPEG
int jpginitgraphics(void);
int jpg_op_parser(char *opstring);
#  ifdef NONE_GUI
#    define jpg_gui_setup NULL
#  else
void jpg_gui_setup(void);
#  endif
#endif

#ifdef HAVE_LIBPNG
int pnginitgraphics(void);
int png_op_parser(char *opstring);
#  ifdef NONE_GUI
#    define png_gui_setup NULL
#  else
void png_gui_setup(void);
#  endif
#endif
