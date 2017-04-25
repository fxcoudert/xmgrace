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
 * Driver for the Grace Metafile format
 */

#include <config.h>

#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "utils.h"
#include "draw.h"
#include "patterns.h"
#include "device.h"
#include "devlist.h"
#include "mfdrv.h"

extern FILE *prstream;

static Device_entry dev_mf = {DEVICE_FILE,
          "Metafile",
          mfinitgraphics,
          NULL,
          NULL,
          "gmf",
          TRUE,
          FALSE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };

int register_mf_drv(void)
{
    return register_device(dev_mf);
}

int mfinitgraphics(void)
{
    int i, j;
    Page_geometry pg;
    
    /* device-dependent routines */
    devupdatecmap   = NULL;
    
    devdrawpixel    = mf_drawpixel;
    devdrawpolyline = mf_drawpolyline;
    devfillpolygon  = mf_fillpolygon;
    devdrawarc      = mf_drawarc;
    devfillarc      = mf_fillarc;
    devputpixmap    = mf_putpixmap;
    devputtext      = mf_puttext;
    
    devleavegraphics = mf_leavegraphics;

    fprintf(prstream, "#GMF-%s\n", GMF_VERSION);

    fprintf(prstream, "FontResources {\n");
    for (i = 0; i < number_of_fonts(); i++) {
        fprintf(prstream, "\t( %d , \"%s\" , \"%s\" )\n", 
            i, get_fontalias(i), get_fontfallback(i));
    }
    fprintf(prstream, "}\n");

    fprintf(prstream, "ColorResources {\n");
    for (i = 0; i < number_of_colors(); i++) {
        RGB *rgb = get_rgb(i);
        fprintf(prstream, "\t( %d , \"%s\" , %d , %d , %d )\n", 
            i, get_colorname(i), rgb->red, rgb->green, rgb->blue);
    }
    fprintf(prstream, "}\n");

    fprintf(prstream, "PatternResources {\n");
    for (i = 0; i < number_of_patterns(); i++) {
        fprintf(prstream, "\t( %d , ", i);
        for (j = 0; j < 32; j++) {
            fprintf(prstream, "%02x", pat_bits[i][j]);
        }
        fprintf(prstream, " )\n");
    }
    fprintf(prstream, "}\n");

    fprintf(prstream, "DashResources {\n");
    for (i = 0; i < number_of_linestyles(); i++) {
        fprintf(prstream, "\t( %d , [ ", i);
        for (j = 0; j < dash_array_length[i]; j++) {
            fprintf(prstream, "%d ", dash_array[i][j]);
        }
        fprintf(prstream, "] )\n");
    }
    fprintf(prstream, "}\n");
    
    pg = get_page_geometry();
    fprintf(prstream, "InitGraphics { %.4f %ld %ld }\n",
        pg.dpi, pg.width, pg.height);
    
    return RETURN_SUCCESS;
}

void mf_setpen(void)
{
    Pen pen;
    
    pen = getpen();
    fprintf(prstream, "SetPen { %d %d }\n", pen.color, pen.pattern);
}

void mf_setdrawbrush(void)
{
    fprintf(prstream, "SetLineWidth { %.4f }\n", getlinewidth());
    fprintf(prstream, "SetLineStyle { %d }\n", getlinestyle());
}

void mf_drawpixel(VPoint vp)
{
    mf_setpen();

    fprintf(prstream, "DrawPixel { ( %.4f , %.4f ) }\n", vp.x, vp.y);
}

void mf_drawpolyline(VPoint *vps, int n, int mode)
{
    int i;
    
    mf_setpen();
    mf_setdrawbrush();
    
    fprintf(prstream, "DrawPolyline {\n");
    if (mode == POLYLINE_CLOSED) {
        fprintf(prstream, "\tClosed\n");
    } else {
        fprintf(prstream, "\tOpen\n");
    }
    for (i = 0; i < n; i++) {
        fprintf(prstream, "\t( %.4f , %.4f )\n", vps[i].x, vps[i].y);
    }
    fprintf(prstream, "}\n");
}

void mf_fillpolygon(VPoint *vps, int nc)
{
    int i;
    
    mf_setpen();
    
    fprintf(prstream, "FillPolygon {\n");
    for (i = 0; i < nc; i++) {
        fprintf(prstream, "\t( %.4f , %.4f )\n", vps[i].x, vps[i].y);
    }
    fprintf(prstream, "}\n"); 
}

void mf_drawarc(VPoint vp1, VPoint vp2, int a1, int a2)
{
    mf_setpen();
    mf_setdrawbrush();
    
    fprintf(prstream, "DrawArc { ( %.4f , %.4f ) ( %.4f , %.4f ) %d %d }\n", 
                                   vp1.x, vp1.y,   vp2.x, vp2.y, a1, a2);
}

void mf_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode)
{
    char *name;
    
    mf_setpen();
    
    /* FIXME - mode */
    if (mode == ARCFILL_CHORD) {
        name = "FillChord";
    } else {
        name = "FillPieSlice";
    }
    fprintf(prstream, "%s { ( %.4f , %.4f ) ( %.4f , %.4f ) %d %d }\n", 
        name, vp1.x, vp1.y,   vp2.x, vp2.y, a1, a2);
}

void mf_putpixmap(VPoint vp, int width, int height, char *databits, 
                             int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    int i, j, k;
    long paddedW;
    int bit;
    char buf[16];
    
    if (pixmap_bpp == 1) {
        strcpy(buf, "Bitmap");
    } else {
        strcpy(buf, "Pixmap");
    }
    fprintf(prstream, "Put%s {\n", buf);
   
    if (pixmap_type == PIXMAP_TRANSPARENT) {
        strcpy(buf, "Transparent");
    } else {
        strcpy(buf, "Opaque");
    }
    
    fprintf(prstream, "\t( %.4f , %.4f ) %dx%d %s\n", 
                           vp.x, vp.y, width, height, buf);
    if (pixmap_bpp != 1) {
        for (k = 0; k < height; k++) {
            fprintf(prstream, "\t");
            for (j = 0; j < width; j++) {
                fprintf(prstream, "%02x", (databits)[k*width+j]);
            }
            fprintf(prstream, "\n");
        }
    } else {
        paddedW = PAD(width, bitmap_pad);
        for (k = 0; k < height; k++) {
            fprintf(prstream, "\t");
            for (j = 0; j < paddedW/bitmap_pad; j++) {
                for (i = 0; i < bitmap_pad; i++) {
                    bit = bin_dump(&databits[k*paddedW/bitmap_pad + j], i, bitmap_pad);
                    if (bit) {
                        fprintf(prstream, "X");
                    } else {
                        fprintf(prstream, ".");
                    }
                }
            } 
            fprintf(prstream, "\n");
        }
    }

    fprintf(prstream, "}\n"); 
}

void mf_puttext(VPoint vp, char *s, int len, int font,
     TextMatrix *tm, int underline, int overline, int kerning)
{
    int i;
    
    mf_setpen();
    
    fprintf(prstream, "PutText {\n");
    fprintf(prstream, "\t( %.4f , %.4f )\n", vp.x, vp.y); 

    fprintf(prstream, "\t %d %.4f %.4f %.4f %.4f %d %d %d %d \"", 
                        font,
                        tm->cxx, tm->cxy, tm->cyx, tm->cyy, 
                        underline, overline, kerning, len);
    for (i = 0; i < len; i++) {
        fputc(s[i], prstream);
    }
    fprintf(prstream, "\"\n");

    fprintf(prstream, "}\n"); 
}

void mf_leavegraphics(void)
{
    view v;
    
    v = get_bbox(BBOX_TYPE_GLOB);
    fprintf(prstream, "LeaveGraphics { %.4f %.4f %.4f %.4f }\n",
        v.xv1, v.yv1, v.xv2, v.yv2);
}

