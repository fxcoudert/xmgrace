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
 *  GRACE PostScript driver
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "defines.h"
#include "utils.h"
#include "draw.h"
#include "device.h"
#include "devlist.h"
#include "patterns.h"
#include "psdrv.h"
#include "protos.h"

#ifndef NONE_GUI
#  include "motifinc.h"
#endif

extern FILE *prstream;

static void put_string(FILE *fp, char *s, int len);

static int curformat = DEFAULT_PS_FORMAT;

static unsigned long page_scale;
static double pixel_size;
static float page_scalef;
static int page_orientation;

static int *psfont_status = NULL;

static int ps_color;
static int ps_pattern;
static double ps_linew;
static int ps_lines;
static int ps_linecap;
static int ps_linejoin;

static int ps_grayscale = FALSE;
static int ps_level2 = TRUE;
static int docdata = DOCDATA_8BIT;

static int ps_setup_offset_x = 0;
static int ps_setup_offset_y = 0;

static int ps_setup_grayscale = FALSE;
static int ps_setup_level2 = TRUE;
static int ps_setup_docdata = DOCDATA_8BIT;

static int ps_setup_feed = MEDIA_FEED_AUTO;
static int ps_setup_hwres = FALSE;

static int eps_setup_grayscale = FALSE;
static int eps_setup_level2 = TRUE;
static int eps_setup_tight_bb = TRUE;
static int eps_setup_docdata = DOCDATA_8BIT;

static int tight_bb;

static Device_entry dev_ps = {DEVICE_PRINT,
          "PostScript",
          psprintinitgraphics,
          ps_op_parser,
          ps_gui_setup,
          "ps",
          TRUE,
          FALSE,
          {3300, 2550, 300.0},
          NULL
         };

static Device_entry dev_eps = {DEVICE_FILE,
          "EPS",
          epsinitgraphics,
          eps_op_parser,
          eps_gui_setup,
          "eps",
          TRUE,
          FALSE,
          {2500, 2500, 300.0},
          NULL
         };

int register_ps_drv(void)
{
    return register_device(dev_ps);
}

int register_eps_drv(void)
{
    return register_device(dev_eps);
}

static int ps_initgraphics(int format)
{
    int i, j;
    Page_geometry pg;
    fRGB *frgb;
    int width_pp, height_pp, page_offset_x, page_offset_y;
    char **enc;
    
    time_t time_value;
    
    curformat = format;
    
    /* device-dependent routines */
    devupdatecmap = NULL;
    
    devdrawpixel = ps_drawpixel;
    devdrawpolyline = ps_drawpolyline;
    devfillpolygon = ps_fillpolygon;
    devdrawarc = ps_drawarc;
    devfillarc = ps_fillarc;
    devputpixmap = ps_putpixmap;
    devputtext = ps_puttext;
    
    devleavegraphics = ps_leavegraphics;

    pg = get_page_geometry();
    
    page_scale = MIN2(pg.height, pg.width);
    pixel_size = 1.0/page_scale;
    page_scalef = (float) page_scale*72.0/pg.dpi;

    if (curformat == PS_FORMAT && pg.height < pg.width) {
        page_orientation = PAGE_ORIENT_LANDSCAPE;
    } else {
        page_orientation = PAGE_ORIENT_PORTRAIT;
    }
    
    /* undefine all graphics state parameters */
    ps_color = -1;
    ps_pattern = -1;
    ps_linew = -1.0;
    ps_lines = -1;
    ps_linecap = -1;
    ps_linejoin = -1;

    /* Font status table */
    if (psfont_status != NULL) {
        xfree(psfont_status);
    }
    psfont_status = xmalloc(number_of_fonts()*SIZEOF_INT);
    for (i = 0; i < number_of_fonts(); i++) {
        psfont_status[i] = FALSE;
    }
    
    switch (curformat) {
    case PS_FORMAT:
        fprintf(prstream, "%%!PS-Adobe-3.0\n");
        tight_bb = FALSE;
        page_offset_x = ps_setup_offset_x;
        page_offset_y = ps_setup_offset_y;
        break;
    case EPS_FORMAT:
        fprintf(prstream, "%%!PS-Adobe-3.0 EPSF-3.0\n");
        tight_bb = eps_setup_tight_bb;
        page_offset_x = 0;
        page_offset_y = 0;
        break;
    default:
        errmsg("Invalid PS format");
        return RETURN_FAILURE;
    }
    
    if (page_orientation == PAGE_ORIENT_LANDSCAPE) {
        width_pp  = (int) rint(72.0*pg.height/pg.dpi);
        height_pp = (int) rint(72.0*pg.width/pg.dpi);
    } else {
        width_pp  = (int) rint(72.0*pg.width/pg.dpi);
        height_pp = (int) rint(72.0*pg.height/pg.dpi);
    }
    
    if (tight_bb == TRUE) {
        fprintf(prstream, "%%%%BoundingBox: (atend)\n");
    } else {
        fprintf(prstream, "%%%%BoundingBox: %d %d %d %d\n", 
            page_offset_x, page_offset_y,
            width_pp + page_offset_x, height_pp + page_offset_y);
    }
    
    if (ps_level2 == TRUE) {
        fprintf(prstream, "%%%%LanguageLevel: 2\n");
    } else {
        fprintf(prstream, "%%%%LanguageLevel: 1\n");
    }
    
    fprintf(prstream, "%%%%Creator: %s\n", bi_version_string());

    time(&time_value);
    fprintf(prstream, "%%%%CreationDate: %s", ctime(&time_value));
    switch (docdata) {
    case DOCDATA_7BIT:
        fprintf(prstream, "%%%%DocumentData: Clean7Bit\n");
        break;
    case DOCDATA_8BIT:
        fprintf(prstream, "%%%%DocumentData: Clean8Bit\n");
        break;
    default:
        fprintf(prstream, "%%%%DocumentData: Binary\n");
        break;
    }
    if (page_orientation == PAGE_ORIENT_LANDSCAPE) {
        fprintf(prstream, "%%%%Orientation: Landscape\n");
    } else {
        fprintf(prstream, "%%%%Orientation: Portrait\n");
    }
    
    if (curformat == PS_FORMAT) {
        fprintf(prstream, "%%%%Pages: 1\n");
        fprintf(prstream, "%%%%PageOrder: Ascend\n");
    }
    fprintf(prstream, "%%%%Title: %s\n", get_docname());
    fprintf(prstream, "%%%%For: %s\n", get_username());
    fprintf(prstream, "%%%%DocumentNeededResources: (atend)\n");
    fprintf(prstream, "%%%%EndComments\n");

    /* Definitions */
    fprintf(prstream, "%%%%BeginProlog\n");
    if (curformat == PS_FORMAT) {
        fprintf(prstream, "/PAGE_OFFSET_X %d def\n", page_offset_x);
        fprintf(prstream, "/PAGE_OFFSET_Y %d def\n", page_offset_y);
    }
    fprintf(prstream, "/m {moveto} def\n");
    fprintf(prstream, "/l {lineto} def\n");
    fprintf(prstream, "/s {stroke} def\n");
    fprintf(prstream, "/n {newpath} def\n");
    fprintf(prstream, "/c {closepath} def\n");
    fprintf(prstream, "/RL {rlineto} def\n");
    fprintf(prstream, "/SLW {setlinewidth} def\n");
    fprintf(prstream, "/GS {gsave} def\n");
    fprintf(prstream, "/GR {grestore} def\n");
    fprintf(prstream, "/SC {setcolor} def\n");
    fprintf(prstream, "/SGRY {setgray} def\n");
    fprintf(prstream, "/SRGB {setrgbcolor} def\n");
    fprintf(prstream, "/SD {setdash} def\n");
    fprintf(prstream, "/SLC {setlinecap} def\n");
    fprintf(prstream, "/SLJ {setlinejoin} def\n");
    fprintf(prstream, "/SCS {setcolorspace} def\n");
    fprintf(prstream, "/FFSF {findfont setfont} def\n");
    fprintf(prstream, "/CC {concat} def\n");
    fprintf(prstream, "/PXL {n m 0 0 RL s} def\n");
    
    for (i = 0; i < number_of_colors(); i++) {
        fprintf(prstream,"/Color%d {", i);
        if (ps_grayscale == TRUE) {
            fprintf(prstream,"%.4f", get_colorintensity(i));
        } else {
            frgb = get_frgb(i);
            if (frgb != NULL) {
                fprintf(prstream, "%.4f %.4f %.4f",
                                    frgb->red,frgb->green, frgb->blue);
            }
        }
        fprintf(prstream,"} def\n");
    }
       
    if (ps_level2 == TRUE) {
        fprintf(prstream, "/PTRN {\n");
        fprintf(prstream, " /pat_bits exch def \n");
        fprintf(prstream, " <<\n");
        fprintf(prstream, "  /PaintType 2\n");
        fprintf(prstream, "  /PatternType 1 /TilingType 1\n");
        fprintf(prstream, "  /BBox[0 0 16 16]\n");
        fprintf(prstream, "  /XStep 16 /YStep 16\n");
        fprintf(prstream, "  /PaintProc {\n");
        fprintf(prstream, "   pop\n");
        fprintf(prstream, "   16 16 true [-1 0 0 -1 16 16] pat_bits imagemask\n");
        fprintf(prstream, "  }\n");
        fprintf(prstream, " >>\n");
        fprintf(prstream, " [%.4f 0 0 %.4f 0 0]\n", 1.0/page_scalef, 1.0/page_scalef);
        fprintf(prstream, " makepattern\n");
        fprintf(prstream, "} def\n");
        for (i = 0; i < number_of_patterns(); i++) {
            fprintf(prstream, "/Pattern%d {<", i);
            for (j = 0; j < 32; j++) {
                fprintf(prstream, "%02x", pat_bits[i][j]);
            }
            fprintf(prstream, "> PTRN} bind def\n");
        }
    }
    
    /* Elliptic arc */
    fprintf(prstream, "/ellipsedict 8 dict def\n");
    fprintf(prstream, "ellipsedict /mtrx matrix put\n");
    fprintf(prstream, "/EARC {\n");
    fprintf(prstream, " ellipsedict begin\n");
    fprintf(prstream, "  /endangle exch def\n");
    fprintf(prstream, "  /startangle exch def\n");
    fprintf(prstream, "  /yrad exch def\n");
    fprintf(prstream, "  /xrad exch def\n");
    fprintf(prstream, "  /y exch def\n");
    fprintf(prstream, "  /x exch def\n");
    fprintf(prstream, "  /savematrix mtrx currentmatrix def\n");
    fprintf(prstream, "  x y translate\n");
    fprintf(prstream, "  xrad yrad scale\n");
    fprintf(prstream, "  0 0 1 startangle endangle arc\n");
    fprintf(prstream, "  savematrix setmatrix\n");
    fprintf(prstream, " end\n");
    fprintf(prstream, "} def\n");

    /* Text under/overlining etc */
    fprintf(prstream, "/TL {\n");
    fprintf(prstream, "  /kcomp exch def\n");
    fprintf(prstream, "  /linewidth exch def\n");
    fprintf(prstream, "  /offset exch def\n");
    fprintf(prstream, "  GS\n");
    fprintf(prstream, "  0 offset rmoveto\n");
    fprintf(prstream, "  linewidth SLW\n");
    fprintf(prstream, "  dup stringwidth exch kcomp add exch RL s\n");
    fprintf(prstream, "  GR\n");
    fprintf(prstream, "} def\n");

    /* Kerning stuff */
    fprintf(prstream, "/KINIT\n");
    fprintf(prstream, "{\n");
    fprintf(prstream, " /kvector exch def\n");
    fprintf(prstream, " /kid 0 def\n");
    fprintf(prstream, "} def\n");
    fprintf(prstream, "/KPROC\n");
    fprintf(prstream, "{\n");
    fprintf(prstream, " pop pop\n");
    fprintf(prstream, " kvector kid get\n");
    fprintf(prstream, " 0 rmoveto\n");
    fprintf(prstream, " /kid 1 kid add def\n");
    fprintf(prstream, "} def\n");

    /* Default encoding */
    enc = get_default_encoding();
    fprintf(prstream, "/DefEncoding [\n");
    for (i = 0; i < 256; i++) {
        fprintf(prstream, " /%s\n", enc[i]);
    }
    fprintf(prstream, "] def\n");

    fprintf(prstream, "%%%%EndProlog\n");

    fprintf(prstream, "%%%%BeginSetup\n");
    if (ps_level2 == TRUE && curformat == PS_FORMAT) {
        /* page size feed */
        switch (ps_setup_feed) {
        case MEDIA_FEED_AUTO:
            break;
        case MEDIA_FEED_MATCH:
            fprintf(prstream, "%%%%BeginFeature: *PageSize\n");
            fprintf(prstream,
                "<</PageSize [%d %d] /ImagingBBox null>> setpagedevice\n",
                width_pp, height_pp);
            fprintf(prstream, "%%%%EndFeature\n");
            break;
        case MEDIA_FEED_MANUAL:
            fprintf(prstream, "%%%%BeginFeature: *ManualFeed\n");
            fprintf(prstream, "<</ManualFeed true>> setpagedevice\n");
            fprintf(prstream, "%%%%EndFeature\n");
            break;
        }
        
        /* force HW resolution */
        if (ps_setup_hwres == TRUE) {
            fprintf(prstream, "%%%%BeginFeature: *HWResolution\n");
            fprintf(prstream, "<</HWResolution [%d %d]>> setpagedevice\n",
                (int) pg.dpi, (int) pg.dpi);
            fprintf(prstream, "%%%%EndFeature\n");
        }
    }
    fprintf(prstream, "%%%%EndSetup\n");

    if (curformat == PS_FORMAT) {
        fprintf(prstream, "%%%%Page: 1 1\n");
    }

    /* compensate for printer page offsets */
    if (curformat == PS_FORMAT) {
        fprintf(prstream, "PAGE_OFFSET_X PAGE_OFFSET_Y translate\n");
    }
    fprintf(prstream, "%.2f %.2f scale\n", page_scalef, page_scalef);
    /* rotate to get landscape on hardcopy */
    if (page_orientation == PAGE_ORIENT_LANDSCAPE) {
        fprintf(prstream, "90 rotate\n");
        fprintf(prstream, "0.0 -1.0 translate\n");
    }
    return RETURN_SUCCESS;
}

void ps_setpen(void)
{
    Pen pen;
    
    pen = getpen();
    
    if (pen.color != ps_color || pen.pattern != ps_pattern) {
        if (ps_level2 == TRUE) {
            if (pen.pattern == 1) {
                if (ps_grayscale == TRUE) {
                    fprintf(prstream, "[/DeviceGray] SCS\n");
                } else {
                    fprintf(prstream, "[/DeviceRGB] SCS\n");
                }
                fprintf(prstream, "Color%d SC\n", pen.color);
            } else {
                if (ps_grayscale == TRUE) {
                    fprintf(prstream, "[/Pattern /DeviceGray] SCS\n");
                } else {
                    fprintf(prstream, "[/Pattern /DeviceRGB] SCS\n");
                }
                fprintf(prstream,
                    "Color%d Pattern%d SC\n", pen.color, pen.pattern);
            }
        } else {
            if (ps_grayscale == TRUE) {
                fprintf(prstream, "Color%d SGRY\n", pen.color);
            } else {
                fprintf(prstream, "Color%d SRGB\n", pen.color);
            }
        }
        ps_color = pen.color;
        ps_pattern = pen.pattern;
    }
}

void ps_setdrawbrush(void)
{
    int i;
    int ls;
    double lw;
    
    ps_setpen();

    ls = getlinestyle();
    lw = MAX2(getlinewidth(), pixel_size);
    
    if (ls != ps_lines || lw != ps_linew) {    
        fprintf(prstream, "[");
        if (ls > 1) {
            for (i = 0; i < dash_array_length[ls]; i++) {
                fprintf(prstream, "%.4f ", lw*dash_array[ls][i]);
            }
        }
        fprintf(prstream, "] 0 SD\n");
        fprintf(prstream, "%.4f SLW\n", lw);
        ps_linew = lw;
        ps_lines = ls;
    }
}

void ps_setlineprops(void)
{
    int lc, lj;
    
    lc = getlinecap();
    lj = getlinejoin();
    
    if (lc != ps_linecap) {
        switch (lc) {
        case LINECAP_BUTT:
            fprintf(prstream, "0 SLC\n");
            break;
        case LINECAP_ROUND:
            fprintf(prstream, "1 SLC\n");
            break;
        case LINECAP_PROJ:
            fprintf(prstream, "2 SLC\n");
            break;
        }
        ps_linecap = lc;
    }

    if (lj != ps_linejoin) {
        switch (lj) {
        case LINEJOIN_MITER:
            fprintf(prstream, "0 SLJ\n");
            break;
        case LINEJOIN_ROUND:
            fprintf(prstream, "1 SLJ\n");
            break;
        case LINEJOIN_BEVEL:
            fprintf(prstream, "2 SLJ\n");
            break;
        }
        ps_linejoin = lj;
    }
}

void ps_drawpixel(VPoint vp)
{
    ps_setpen();
    
    if (ps_linew != pixel_size) {
        fprintf(prstream, "%.4f SLW\n", pixel_size);
        ps_linew = pixel_size;
    }
    if (ps_linecap != LINECAP_ROUND) {
        fprintf(prstream, "1 SLC\n");
        ps_linecap = LINECAP_ROUND;
    }
    if (ps_lines != 1) {
        fprintf(prstream, "[] 0 SD\n");
        ps_lines = 1;
    }
    
    fprintf(prstream, "%.4f %.4f PXL\n", vp.x, vp.y);
}

void ps_drawpolyline(VPoint *vps, int n, int mode)
{
    int i;
    
    ps_setdrawbrush();
    
    ps_setlineprops();
    
    fprintf(prstream, "n\n");
    fprintf(prstream, "%.4f %.4f m\n", vps[0].x, vps[0].y);
    for (i = 1; i < n; i++) {
        fprintf(prstream, "%.4f %.4f l\n", vps[i].x, vps[i].y);
    }
    if (mode == POLYLINE_CLOSED) {
        fprintf(prstream, "%.4f %.4f l\n", vps[0].x, vps[0].y);
        fprintf(prstream, "c\n");
    }
    fprintf(prstream, "s\n");
}

void ps_fillpolygon(VPoint *vps, int nc)
{
    int i;
    Pen pen = getpen();
    
    if (pen.pattern == 0 || nc < 3) {
        return;
    }
    
    fprintf(prstream, "n\n");
    fprintf(prstream, "%.4f %.4f m\n", vps[0].x, vps[0].y);
    for (i = 1; i < nc; i++) {
        fprintf(prstream, "%.4f %.4f l\n", vps[i].x, vps[i].y);
    }
    fprintf(prstream, "c\n");

    /* fill bg first if the pattern != solid */
    if (pen.pattern != 1 && ps_level2 == TRUE) {
        fprintf(prstream, "GS\n");
        if (ps_grayscale == TRUE) {
            if (ps_pattern != 1) {
                fprintf(prstream, "[/DeviceGray] SCS\n");
            }
            fprintf(prstream, "Color%d SGRY\n", getbgcolor());
        } else {
            if (ps_pattern != 1) {
                fprintf(prstream, "[/DeviceRGB] SCS\n");
            }
            fprintf(prstream, "Color%d SRGB\n", getbgcolor());
        }
        if (getfillrule() == FILLRULE_WINDING) {
            fprintf(prstream, "fill\n");
        } else {
            fprintf(prstream, "eofill\n");
        }
        fprintf(prstream, "GR\n");
    }
    
    ps_setpen();
    if (getfillrule() == FILLRULE_WINDING) {
        fprintf(prstream, "fill\n");
    } else {
        fprintf(prstream, "eofill\n");
    }
}

void ps_drawarc(VPoint vp1, VPoint vp2, int a1, int a2)
{
    VPoint vpc;
    double rx, ry;
    
    ps_setdrawbrush();

    vpc.x = (vp1.x + vp2.x)/2;
    vpc.y = (vp1.y + vp2.y)/2;
    rx = fabs(vp2.x - vp1.x)/2;
    ry = fabs(vp2.y - vp1.y)/2;
    
    fprintf(prstream, "n %.4f %.4f %.4f %.4f %d %d EARC s\n",
                       vpc.x, vpc.y, rx, ry, a1, a2);
}

void ps_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode)
{
    VPoint vpc;
    double rx, ry;
    Pen pen = getpen();
    
    if (pen.pattern == 0) {
        return;
    }

    vpc.x = (vp1.x + vp2.x)/2;
    vpc.y = (vp1.y + vp2.y)/2;
    rx = fabs(vp2.x - vp1.x)/2;
    ry = fabs(vp2.y - vp1.y)/2;
    
    fprintf(prstream, "n\n");
    
    if (mode == ARCFILL_PIESLICE) {
        fprintf(prstream, "%.4f %.4f m\n", vpc.x, vpc.y);
    }
    fprintf(prstream, "%.4f %.4f %.4f %.4f %d %d EARC c\n",
                       vpc.x, vpc.y, rx, ry, a1, a2);

    /* fill bg first if the pattern != solid */
    if (pen.pattern != 1 && ps_level2 == TRUE) {
        fprintf(prstream, "GS\n");
        if (ps_grayscale == TRUE) {
            if (ps_pattern != 1) {
                fprintf(prstream, "[/DeviceGray] SCS\n");
            }
            fprintf(prstream, "Color%d SGRY\n", getbgcolor());
        } else {
            if (ps_pattern != 1) {
                fprintf(prstream, "[/DeviceRGB] SCS\n");
            }
            fprintf(prstream, "Color%d SRGB\n", getbgcolor());
        }
        fprintf(prstream, "fill\n");
        fprintf(prstream, "GR\n");
    }

    ps_setpen();
    fprintf(prstream, "fill\n");
}

void ps_putpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    int j, k;
    int cindex;
    int paddedW;
    RGB *rgb;
    fRGB *frgb;
    unsigned char tmpbyte;
    int linelen;

    ps_setpen();
    
    fprintf(prstream, "GS\n");
    fprintf(prstream, "%.4f %.4f translate\n", vp.x, vp.y);
    fprintf(prstream, "%.4f %.4f scale\n", (float) width/page_scale, 
                                           (float) height/page_scale);    
    if (pixmap_bpp != 1) {
        if (pixmap_type == PIXMAP_TRANSPARENT) {
            /* TODO: mask */
        }
        if (ps_grayscale == TRUE) {
            fprintf(prstream, "/picstr %d string def\n", width);
            fprintf(prstream, "%d %d %d\n", width, height, 8);
        } else {
            fprintf(prstream, "/picstr %d string def\n", 3*width);
            fprintf(prstream, "%d %d %d\n", width, height, GRACE_BPP);
        }
        fprintf(prstream, "[%d 0 0 %d 0 0]\n", width, -height);
        fprintf(prstream, "{currentfile picstr readhexstring pop}\n");
        if (ps_grayscale == TRUE || ps_level2 == FALSE) {
            /* No color images in Level1 */
            fprintf(prstream, "image\n");
        } else {
            fprintf(prstream, "false 3\n");
            fprintf(prstream, "colorimage\n");
        }
        for (k = 0; k < height; k++) {
            linelen = 0;
            for (j = 0; j < width; j++) {
                cindex = (databits)[k*width+j];
                if (ps_grayscale == TRUE || ps_level2 == FALSE) {
                    linelen += fprintf(prstream,"%02x",
                                      (int) (255*get_colorintensity(cindex)));
                } else {
                    rgb = get_rgb(cindex);
                    linelen += fprintf(prstream, "%02x%02x%02x",
                                       rgb->red, rgb->green, rgb->blue);
                }
                if (linelen >= MAX_PS_LINELEN) {
                    fprintf(prstream, "\n");
                    linelen = 0;
                }
            }
            fprintf(prstream, "\n");
        }
    } else { /* monocolor bitmap */
        paddedW = PAD(width, bitmap_pad);
        if (pixmap_type == PIXMAP_OPAQUE) {
            if (ps_grayscale == TRUE) {
                fprintf(prstream,"%.4f SGRY\n",
                                  get_colorintensity(getbgcolor()));
            } else {
                frgb = get_frgb(getbgcolor());
                fprintf(prstream,"%.4f %.4f %.4f SRGB\n",
                                  frgb->red, frgb->green, frgb->blue);
            }
            fprintf(prstream, "0 0 1 -1 rectfill\n");
        }
        if (ps_grayscale == TRUE) {
            fprintf(prstream,"%.4f SGRY\n", get_colorintensity(getcolor()));
        } else {
            frgb = get_frgb(getcolor());
            fprintf(prstream,"%.4f %.4f %.4f SRGB\n",
                              frgb->red, frgb->green, frgb->blue);
        }
        fprintf(prstream, "/picstr %d string def\n", paddedW/8);
        fprintf(prstream, "%d %d true\n", paddedW, height);
        fprintf(prstream, "[%d 0 0 %d 0 0]\n", paddedW, -height);
        fprintf(prstream, "{currentfile picstr readhexstring pop}\n");
        fprintf(prstream, "imagemask\n");
        for (k = 0; k < height; k++) {
            linelen = 0;
            for (j = 0; j < paddedW/bitmap_pad; j++) {
                tmpbyte = reversebits((unsigned char) (databits)[k*paddedW/bitmap_pad + j]);
                linelen += fprintf(prstream, "%02x", tmpbyte);
                if (linelen >= MAX_PS_LINELEN) {
                    fprintf(prstream, "\n");
                    linelen = 0;
                }
            }
            fprintf(prstream, "\n");
        }
    }
    fprintf(prstream, "GR\n");
}

void ps_puttext(VPoint vp, char *s, int len, int font,
     TextMatrix *tm, int underline, int overline, int kerning)
{
    char *fontname;
    char *encscheme;
    double *kvector;
    int i;
    int linelen;
    
    if (psfont_status[font] == FALSE) {
        fontname = get_fontalias(font);
        encscheme = get_encodingscheme(font);
        fprintf(prstream, "/%s findfont\n", fontname);
        if (strcmp(encscheme, "FontSpecific") != 0) {
            fprintf(prstream, "dup length dict begin\n");
            fprintf(prstream, " {1 index /FID ne {def} {pop pop} ifelse} forall\n");
            fprintf(prstream, " /Encoding DefEncoding def\n");
            fprintf(prstream, " currentdict\n");
            fprintf(prstream, "end\n");
        }
        fprintf(prstream, "/Font%d exch definefont pop\n", font);
        psfont_status[font] = TRUE;
    }
    fprintf(prstream, "/Font%d FFSF\n", font);

    ps_setpen();
    
    fprintf(prstream, "%.4f %.4f m\n", vp.x, vp.y);
    fprintf(prstream, "GS\n");
    fprintf(prstream, "[%.4f %.4f %.4f %.4f 0 0] CC\n",
                        tm->cxx, tm->cyx, tm->cxy, tm->cyy);
    
    if (kerning) {
        kvector = get_kerning_vector(s, len, font);
    } else {
        kvector = NULL;
    }
    
    if (kvector) {
        linelen = 0;
        linelen += fprintf(prstream, "[");
        for (i = 0; i < len - 1; i++) {
            linelen += fprintf(prstream, "%.4f ", kvector[i]);
            if (linelen >= MAX_PS_LINELEN) {
                fprintf(prstream, "\n");
                linelen = 0;
            }
        }
        fprintf(prstream, "] KINIT\n");
        fprintf(prstream, "{KPROC}\n");
    }
    
    put_string(prstream, s, len);

    if (underline | overline) {
        double w, pos, kcomp;
        
        if (kvector) {
            kcomp = kvector[len - 1];
        } else {
            kcomp = 0.0;
        }
        w = get_textline_width(font);
        if (underline) {
            pos = get_underline_pos(font);
            fprintf(prstream, " %.4f %.4f %.4f TL", pos, w, kcomp);
        }
        if (overline) {
            pos = get_overline_pos(font);
            fprintf(prstream, " %.4f %.4f %.4f TL", pos, w, kcomp);
        }
    }
    
    if (kvector) {
        fprintf(prstream, " kshow\n");
        xfree(kvector);
    } else {
        fprintf(prstream, " show\n");
    }
    
    fprintf(prstream, "GR\n");
}


void ps_leavegraphics(void)
{
    view v;
    int i, first;
    
    if (curformat == PS_FORMAT) {
        fprintf(prstream, "showpage\n");
        fprintf(prstream, "%%%%PageTrailer\n");
    }
    fprintf(prstream, "%%%%Trailer\n");
    
    if (tight_bb == TRUE) {
        v = get_bbox(BBOX_TYPE_GLOB);
        if (page_orientation == PAGE_ORIENT_LANDSCAPE) {
            fprintf(prstream, "%%%%BoundingBox: %d %d %d %d\n",
                                         (int) (page_scalef*(1.0 - v.yv2)) - 1,
                                         (int) (page_scalef*v.xv1) - 1,
                                         (int) (page_scalef*(1.0 - v.yv1)) + 2,
                                         (int) (page_scalef*v.xv2) + 2);
        } else {
            fprintf(prstream, "%%%%BoundingBox: %d %d %d %d\n",
                                         (int) (page_scalef*v.xv1) - 1,
                                         (int) (page_scalef*v.yv1) - 1,
                                         (int) (page_scalef*v.xv2) + 2,
                                         (int) (page_scalef*v.yv2) + 2);
        }
    }
    
    first = TRUE;
    for (i = 0; i < number_of_fonts(); i++) {
        if (psfont_status[i] == TRUE) {
            if (first) {
                fprintf(prstream, "%%%%DocumentNeededResources: font %s\n",
                    get_fontalias(i));
                first = FALSE;
            } else {
                fprintf(prstream, "%%%%+ font %s\n", get_fontalias(i));
            }
        }
    }

    fprintf(prstream, "%%%%EOF\n");
}

static int is7bit(unsigned char uc)
{
    if (uc >= 0x1b && uc <= 0x7e) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static int is8bit(unsigned char uc)
{
    if (is7bit(uc) || uc >= 0x80) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Put a NOT NULL-terminated string escaping parentheses and backslashes
 */
static void put_string(FILE *fp, char *s, int len)
{
    int i, linelen = 0;
    
    fputc('(', fp);
    linelen++;
    for (i = 0; i < len; i++) {
        char c = s[i];
        unsigned char uc = (unsigned char) c;
        if (c == '(' || c == ')' || c == '\\') {
            fputc('\\', fp);
            linelen++;
        }
        if ((docdata == DOCDATA_7BIT && !is7bit(uc)) ||
            (docdata == DOCDATA_8BIT && !is8bit(uc))) {
            linelen += fprintf(fp, "\\%03o", uc);
        } else {
            fputc(c, fp);
            linelen++;
        }
        if (linelen >= MAX_PS_LINELEN) {
            fprintf(prstream, "\\\n");
            linelen = 0;
        }
    }
    fputc(')', fp);
}

int psprintinitgraphics(void)
{
    int result;
    
    ps_grayscale = ps_setup_grayscale;
    ps_level2 = ps_setup_level2;
    docdata = ps_setup_docdata;
    result = ps_initgraphics(PS_FORMAT);
    
    if (result == RETURN_SUCCESS) {
        curformat = PS_FORMAT;
    }
    
    return (result);
}

int epsinitgraphics(void)
{
    int result;
    
    ps_grayscale = eps_setup_grayscale;
    ps_level2 = eps_setup_level2;
    docdata = eps_setup_docdata;
    result = ps_initgraphics(EPS_FORMAT);
    
    if (result == RETURN_SUCCESS) {
        curformat = EPS_FORMAT;
    }
    
    return (result);
}

int ps_op_parser(char *opstring)
{
    if (!strcmp(opstring, "grayscale")) {
        ps_setup_grayscale = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "color")) {
        ps_setup_grayscale = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "level2")) {
        ps_setup_level2 = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "level1")) {
        ps_setup_level2 = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "docdata:7bit")) {
        ps_setup_docdata = DOCDATA_7BIT;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "docdata:8bit")) {
        ps_setup_docdata = DOCDATA_8BIT;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "docdata:binary")) {
        ps_setup_docdata = DOCDATA_BINARY;
        return RETURN_SUCCESS;
    } else if (!strncmp(opstring, "xoffset:", 8)) {
        ps_setup_offset_x = atoi(opstring + 8);
        return RETURN_SUCCESS;
    } else if (!strncmp(opstring, "yoffset:", 8)) {
        ps_setup_offset_y = atoi(opstring + 8);
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "hwresolution:on")) {
        ps_setup_hwres = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "hwresolution:off")) {
        ps_setup_hwres = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "mediafeed:auto")) {
        ps_setup_feed = MEDIA_FEED_AUTO;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "mediafeed:match")) {
        ps_setup_feed = MEDIA_FEED_MATCH;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "mediafeed:manual")) {
        ps_setup_feed = MEDIA_FEED_MANUAL;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int eps_op_parser(char *opstring)
{
    if (!strcmp(opstring, "grayscale")) {
        eps_setup_grayscale = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "color")) {
        eps_setup_grayscale = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "level2")) {
        eps_setup_level2 = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "level1")) {
        eps_setup_level2 = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "docdata:7bit")) {
        eps_setup_docdata = DOCDATA_7BIT;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "docdata:8bit")) {
        eps_setup_docdata = DOCDATA_8BIT;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "docdata:binary")) {
        eps_setup_docdata = DOCDATA_BINARY;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "bbox:tight")) {
        eps_setup_tight_bb = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "bbox:page")) {
        eps_setup_tight_bb = FALSE;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

#ifndef NONE_GUI

static void update_ps_setup_frame(void);
static int set_ps_setup_proc(void *data);

static Widget ps_setup_frame;
static Widget ps_setup_grayscale_item;
static Widget ps_setup_level2_item;
static SpinStructure *ps_setup_offset_x_item;
static SpinStructure *ps_setup_offset_y_item;
static OptionStructure *ps_setup_feed_item;
static Widget ps_setup_hwres_item;
static OptionStructure *ps_setup_docdata_item;

void ps_gui_setup(void)
{
    set_wait_cursor();
    
    if (ps_setup_frame == NULL) {
        Widget ps_setup_rc, fr, rc;
        OptionItem op_items[3] = {
            {MEDIA_FEED_AUTO,   "Automatic" },
            {MEDIA_FEED_MATCH,  "Match size"},
            {MEDIA_FEED_MANUAL, "Manual"    }
        };
        OptionItem docdata_op_items[3] = {
            {DOCDATA_7BIT,   "7bit" },
            {DOCDATA_8BIT,   "8bit"},
            {DOCDATA_BINARY, "Binary"    }
        };
        
	ps_setup_frame = CreateDialogForm(app_shell, "PS options");

        ps_setup_rc = CreateVContainer(ps_setup_frame);

	fr = CreateFrame(ps_setup_rc, "PS options");
        rc = CreateVContainer(fr);
	ps_setup_grayscale_item = CreateToggleButton(rc, "Grayscale output");
	ps_setup_level2_item = CreateToggleButton(rc, "PS Level 2");
	ps_setup_docdata_item =
            CreateOptionChoice(rc, "Document data:", 1, 3, docdata_op_items);

	fr = CreateFrame(ps_setup_rc, "Page offsets (pt)");
        rc = CreateHContainer(fr);
	ps_setup_offset_x_item = CreateSpinChoice(rc,
            "X: ", 4, SPIN_TYPE_INT, -999.0, 999.0, 10.0);
	ps_setup_offset_y_item = CreateSpinChoice(rc,
            "Y: ", 4, SPIN_TYPE_INT, -999.0, 999.0, 10.0);

	fr = CreateFrame(ps_setup_rc, "Hardware");
        rc = CreateVContainer(fr);
	ps_setup_feed_item = CreateOptionChoice(rc, "Media feed:", 1, 3, op_items);
	ps_setup_hwres_item = CreateToggleButton(rc, "Set hardware resolution");

	CreateAACDialog(ps_setup_frame, ps_setup_rc, set_ps_setup_proc, NULL);
    }
    update_ps_setup_frame();
    
    RaiseWindow(GetParent(ps_setup_frame));
    unset_wait_cursor();
}

static void update_ps_setup_frame(void)
{
    if (ps_setup_frame) {
        SetToggleButtonState(ps_setup_grayscale_item, ps_setup_grayscale);
        SetToggleButtonState(ps_setup_level2_item, ps_setup_level2);
        SetSpinChoice(ps_setup_offset_x_item, (double) ps_setup_offset_x);
        SetSpinChoice(ps_setup_offset_y_item, (double) ps_setup_offset_y);
        SetOptionChoice(ps_setup_feed_item, ps_setup_feed);
        SetToggleButtonState(ps_setup_hwres_item, ps_setup_hwres);
        SetOptionChoice(ps_setup_docdata_item, ps_setup_docdata);
    }
}

static int set_ps_setup_proc(void *data)
{
    ps_setup_grayscale = GetToggleButtonState(ps_setup_grayscale_item);
    ps_setup_level2    = GetToggleButtonState(ps_setup_level2_item);
    ps_setup_offset_x  = (int) GetSpinChoice(ps_setup_offset_x_item);
    ps_setup_offset_y  = (int) GetSpinChoice(ps_setup_offset_y_item);
    ps_setup_feed      = GetOptionChoice(ps_setup_feed_item);
    ps_setup_hwres     = GetToggleButtonState(ps_setup_hwres_item);
    ps_setup_docdata   = GetOptionChoice(ps_setup_docdata_item);
    
    return RETURN_SUCCESS;
}

static void update_eps_setup_frame(void);
static int set_eps_setup_proc(void *data);
static Widget eps_setup_frame;
static Widget eps_setup_grayscale_item;
static Widget eps_setup_level2_item;
static Widget eps_setup_tight_bb_item;
static OptionStructure *eps_setup_docdata_item;

void eps_gui_setup(void)
{
    set_wait_cursor();
    
    if (eps_setup_frame == NULL) {
        Widget fr, rc;
        OptionItem docdata_op_items[3] = {
            {DOCDATA_7BIT,   "7bit" },
            {DOCDATA_8BIT,   "8bit"},
            {DOCDATA_BINARY, "Binary"    }
        };
	
        eps_setup_frame = CreateDialogForm(app_shell, "EPS options");

        fr = CreateFrame(eps_setup_frame, "EPS options");
        rc = CreateVContainer(fr);
	eps_setup_grayscale_item = CreateToggleButton(rc, "Grayscale output");
	eps_setup_level2_item = CreateToggleButton(rc, "PS Level 2");
	eps_setup_tight_bb_item = CreateToggleButton(rc, "Tight BBox");
	eps_setup_docdata_item =
            CreateOptionChoice(rc, "Document data:", 1, 3, docdata_op_items);
	CreateAACDialog(eps_setup_frame, fr, set_eps_setup_proc, NULL);
    }
    update_eps_setup_frame();
    RaiseWindow(GetParent(eps_setup_frame));
    
    unset_wait_cursor();
}

static void update_eps_setup_frame(void)
{
    if (eps_setup_frame) {
        SetToggleButtonState(eps_setup_grayscale_item, eps_setup_grayscale);
        SetToggleButtonState(eps_setup_level2_item, eps_setup_level2);
        SetToggleButtonState(eps_setup_tight_bb_item, eps_setup_tight_bb);
        SetOptionChoice(eps_setup_docdata_item, eps_setup_docdata);
    }
}

static int set_eps_setup_proc(void *data)
{
    eps_setup_grayscale = GetToggleButtonState(eps_setup_grayscale_item);
    eps_setup_level2 = GetToggleButtonState(eps_setup_level2_item);
    eps_setup_tight_bb = GetToggleButtonState(eps_setup_tight_bb_item);
    eps_setup_docdata = GetOptionChoice(eps_setup_docdata_item);
    
    return RETURN_SUCCESS;
}

#endif
