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
 * Grace generic raster format driver
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "utils.h"
#include "draw.h"
#include "device.h"
#include "devlist.h"
#include "patterns.h"
#include "rstdrv.h"
#include "protos.h"

#include "gd.h"

#ifdef HAVE_LIBJPEG
#  define JPEG_INTERNAL_OPTIONS
#  include <jpeglib.h>
#endif

#ifdef HAVE_LIBPNG
#  include <zlib.h>
#  include <png.h>
#endif

#ifndef NONE_GUI
#  include "motifinc.h"
#endif

static void rstImagePnm(gdImagePtr ihandle, FILE *prstream);

extern FILE *prstream;

/* Declare the image */
static gdImagePtr ihandle = NULL;

static int curformat = DEFAULT_RASTER_FORMAT;

static int rst_colors[MAXCOLORS];
static int rst_drawbrush, rst_fillbrush;

static Pen rstpen;
static int rstlines, rstlinew;

static int rst_dash_array_length;

static unsigned long page_scale;

#ifdef HAVE_LIBJPEG
static void rstImageJpg(gdImagePtr ihandle, FILE *prstream);

static int jpg_setup_quality = 75;
static int jpg_setup_grayscale = FALSE;
static int jpg_setup_baseline = FALSE;
static int jpg_setup_progressive = FALSE;
static int jpg_setup_optimize = FALSE;
static int jpg_setup_smoothing = 0;
static int jpg_setup_dct = JPEG_DCT_DEFAULT;
#endif

#ifdef HAVE_LIBPNG
static void rstImagePng(gdImagePtr ihandle, FILE *prstream);

static int png_setup_interlaced = FALSE;
static int png_setup_transparent = FALSE;
static int png_setup_compression = 4;
#endif

static Device_entry dev_pnm = {DEVICE_FILE,
          "PNM",
          pnminitgraphics,
          pnm_op_parser,
          pnm_gui_setup,
          "pnm",
          FALSE,
          TRUE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };

#ifdef HAVE_LIBJPEG
static Device_entry dev_jpg = {DEVICE_FILE,
          "JPEG",
          jpginitgraphics,
          jpg_op_parser,
          jpg_gui_setup,
          "jpg",
          FALSE,
          TRUE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };
#endif

#ifdef HAVE_LIBPNG
static Device_entry dev_png = {DEVICE_FILE,
          "PNG",
          pnginitgraphics,
          png_op_parser,
          png_gui_setup,
          "png",
          FALSE,
          TRUE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };
#endif

int register_pnm_drv(void)
{
    return register_device(dev_pnm);
}

#ifdef HAVE_LIBJPEG
int register_jpg_drv(void)
{
    return register_device(dev_jpg);
}
#endif

#ifdef HAVE_LIBPNG
int register_png_drv(void)
{
    return register_device(dev_png);
}
#endif

static void rst_updatecmap(void)
{
    int i, c;
    RGB *prgb;
    int red, green, blue;
    
    if (!ihandle) {
        return;
    }
    
    for (i = 0; i < number_of_colors(); i++) {
        prgb = get_rgb(i);
        if (prgb != NULL) {
            red = prgb->red >> (GRACE_BPP - 8);
            green = prgb->green >> (GRACE_BPP - 8);
            blue = prgb->blue >> (GRACE_BPP - 8);
            if ((c = gdImageColorExact(ihandle, red, green, blue))    == -1 &&
                (c = gdImageColorAllocate(ihandle, red, green, blue)) == -1 &&
                (c = gdImageColorClosest(ihandle, red, green, blue))  == -1) {
                c = rst_colors[0];
            }
            rst_colors[i] = c;
        }
    }
}

static gdPoint VPoint2gdPoint(VPoint vp)
{
    gdPoint gdp;
    
    gdp.x = (int) rint(page_scale * vp.x);
    gdp.y = (int) rint(page_height - page_scale * vp.y);
    
    return (gdp);
}

void rst_setdrawbrush(void)
{
    static gdImagePtr brush = NULL;
    int i, j, k;
    int *tmp_dash_array;
    RGB *prgb;
    int red, green, blue, bcolor;
    int scale;
    int on, off;

    rstpen = getpen();
    rstlinew = MAX2((int) rint(getlinewidth()*page_scale), 1);
    rstlines = getlinestyle();
    
    if (rstlines == 0 || rstpen.pattern == 0) {
        /* Should never come to here */
        rst_drawbrush = gdTransparent;
        return;
    }
    
    if (rstlinew > 1) {
        if (brush != NULL) {
            gdImageDestroy(brush);
        }
        brush = gdImageCreate(rstlinew, rstlinew);

        prgb = get_rgb(rstpen.color);
        red = prgb->red >> (GRACE_BPP - 8);
        green = prgb->green >> (GRACE_BPP - 8);
        blue = prgb->blue >> (GRACE_BPP - 8);
        bcolor = gdImageColorAllocate(brush, red, green, blue);

        gdImageFilledRectangle(brush, 0, 0, rstlinew, rstlinew, bcolor);

        gdImageSetBrush(ihandle, brush);
    }

    if (rstlines > 1) {
        rst_dash_array_length = 0;
        for (i = 0; i < dash_array_length[rstlines]; i++) {
            rst_dash_array_length += dash_array[rstlines][i];
        }
    
        if (rstlinew <= 1) {
            scale = 1;
            on = rstpen.color;
            off = gdTransparent;
            rst_drawbrush = gdStyled;
        } else {
            scale = rstlinew;
            on = 1;
            off = 0;
            rst_drawbrush = gdStyledBrushed;
        }
        
        tmp_dash_array = (int *) xmalloc((scale*rst_dash_array_length + 1)*SIZEOF_INT);
        if (tmp_dash_array == NULL) {
            return;
        }
        
        k = 0;
        for (i = 0; i < dash_array_length[rstlines]; i++) {
            if (i % 2 == 0) {
                /* black */
                for (j = 0; j < (dash_array[rstlines][i] - 1)*scale + 1; j++) {
                    tmp_dash_array[k++] = on;
                }
            } else {
                /* white */
                for (j = 0; j < (dash_array[rstlines][i] + 1)*scale - 1; j++) {
                    tmp_dash_array[k++] = off;
                }
            }
        }
        gdImageSetStyle(ihandle, tmp_dash_array, k);
        xfree(tmp_dash_array);
            
    } else {
        if (rstlinew <= 1) {
            rst_drawbrush = rst_colors[rstpen.color];
        } else {
            rst_drawbrush = gdBrushed;
        }
    }
}

void rst_setfillbrush(void)
{
    static gdImagePtr brush = NULL;
    int i, j, k;
    RGB *prgb;
    int red, green, blue, fgcolor, bgcolor;
    unsigned char p;
    
    rstpen = getpen();

    if (rstpen.pattern == 0) {
        /* Should never come to here */
        rst_fillbrush = gdTransparent;
    } else if (rstpen.pattern == 1) {
        rst_fillbrush = rst_colors[rstpen.color];
    } else {
        /* TODO */
        if (brush != NULL) {
            gdImageDestroy(brush);
        }
        brush = gdImageCreate(16, 16);
        
        prgb = get_rgb(rstpen.color);
        red = prgb->red >> (GRACE_BPP - 8);
        green = prgb->green >> (GRACE_BPP - 8);
        blue = prgb->blue >> (GRACE_BPP - 8);
        fgcolor = gdImageColorAllocate(brush, red, green, blue);
        
        prgb = get_rgb(getbgcolor());
        red = prgb->red >> (GRACE_BPP - 8);
        green = prgb->green >> (GRACE_BPP - 8);
        blue = prgb->blue >> (GRACE_BPP - 8);
        bgcolor = gdImageColorAllocate(brush, red, green, blue);
        
        for (k = 0; k < 16; k++) {
            for (j = 0; j < 2; j++) {
                for (i = 0; i < 8; i++) {
                    p = pat_bits[rstpen.pattern][k*2+j];
                    if ((p >> i) & 0x01) {
                        gdImageSetPixel(brush, 8*j + i, k, fgcolor);
                    } else {
                        gdImageSetPixel(brush, 8*j + i, k, bgcolor);
                    }
                }
            }
        }
        gdImageSetTile(ihandle, brush);
        
        rst_fillbrush = gdTiled;
    }
}

static int rst_initgraphics(int format)
{
    Page_geometry pg;
    
    curformat = format;
    
    /* device-dependent routines */
    devupdatecmap = rst_updatecmap;
    
    devdrawpixel = rst_drawpixel;
    devdrawpolyline = rst_drawpolyline;
    devfillpolygon = rst_fillpolygon;
    devdrawarc = rst_drawarc;
    devfillarc = rst_fillarc;
    devputpixmap = rst_putpixmap;
    
    devleavegraphics = rst_leavegraphics;
    
    pg = get_page_geometry();
    
    page_scale = MIN2(pg.height,pg.width);

    /* Allocate the image */
    ihandle = gdImageCreate(pg.width, pg.height);
    if (ihandle == NULL) {
        return RETURN_FAILURE;
    }
    
    rst_updatecmap();
    
    return RETURN_SUCCESS;
}

void rst_drawpixel(VPoint vp)
{
    gdPoint gdp;
    
    gdp = VPoint2gdPoint(vp);
    gdImageSetPixel(ihandle, gdp.x, gdp.y, rst_colors[getcolor()]);
}

void rst_drawpolyline(VPoint *vps, int n, int mode)
{
    int i;
    gdPointPtr gdps;
    
    gdps = (gdPointPtr) xmalloc(n*sizeof(gdPoint));
    if (gdps == NULL) {
        return;
    }
    
    for (i = 0; i < n; i++) {
        gdps[i] = VPoint2gdPoint(vps[i]);
    }
    
    rst_setdrawbrush();
    
    if (mode == POLYLINE_CLOSED) {
        gdImagePolygon(ihandle, gdps, n, rst_drawbrush);
    } else {
         for (i = 0; i < n - 1; i++) {
             gdImageLine(ihandle, gdps[i].x,     gdps[i].y, 
                                  gdps[i + 1].x, gdps[i + 1].y, 
                                  rst_drawbrush);
         }
    }
    
    xfree(gdps);
}

void rst_fillpolygon(VPoint *vps, int nc)
{
    int i;
    gdPointPtr gdps;
    
    gdps = (gdPointPtr) xmalloc(nc*sizeof(gdPoint));
    if (gdps == NULL) {
        return;
    }
    
    for (i = 0; i < nc; i++) {
        gdps[i] = VPoint2gdPoint(vps[i]);
    }
    
    rst_setfillbrush();
    gdImageFilledPolygon(ihandle, gdps, nc, rst_fillbrush);
    
    xfree(gdps);
}

void rst_drawarc(VPoint vp1, VPoint vp2, int a1, int a2)
{
    gdPoint gdp1, gdp2, gdc;
    int w, h;
    
    gdp1 = VPoint2gdPoint(vp1);
    gdp2 = VPoint2gdPoint(vp2);
    gdc.x = (gdp1.x + gdp2.x)/2;
    gdc.y = (gdp1.y + gdp2.y)/2;
    w = (gdp2.x - gdp1.x);
    h = (gdp2.y - gdp1.y);
    
    rst_setdrawbrush();
    
    gdImageArc(ihandle, gdc.x, gdc.y, w, h, a1, a2, rst_drawbrush);
}

void rst_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode)
{
    gdPoint gdp1, gdp2, gdc;
    int w, h;
    
    gdp1 = VPoint2gdPoint(vp1);
    gdp2 = VPoint2gdPoint(vp2);
    gdc.x = (gdp1.x + gdp2.x)/2;
    gdc.y = (gdp1.y + gdp2.y)/2;
    w = (gdp2.x - gdp1.x);
    h = (gdp2.y - gdp1.y);
    
    rst_setfillbrush();
    gdImageFilledArc(ihandle, gdc.x, gdc.y, w, h, a1, a2,
        mode == ARCFILL_CHORD ? gdArcFillChord:gdArcFillPieSlice, rst_fillbrush);
}

void rst_putpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    int cindex, bg;
    int color, bgcolor;
    
    int	i, k, j;
    long paddedW;
    
    gdPoint gdp;
    int x, y;
    
    bg = getbgcolor();
    bgcolor = rst_colors[bg];
    
    gdp = VPoint2gdPoint(vp);
    
    y = gdp.y;
    if (pixmap_bpp == 1) {
        color = getcolor();
        paddedW = PAD(width, bitmap_pad);
        for (k = 0; k < height; k++) {
            x = gdp.x;
            y++;
            for (j = 0; j < paddedW/bitmap_pad; j++) {
                for (i = 0; i < bitmap_pad && j*bitmap_pad + i < width; i++) {
                    x++;
                    if (bin_dump(&(databits)[k*paddedW/bitmap_pad+j], i, bitmap_pad)) {
                        gdImageSetPixel(ihandle, x, y, color);
                    } else {
                        if (pixmap_type == PIXMAP_OPAQUE) {
                            gdImageSetPixel(ihandle, x, y, bgcolor);
                        }
                    }
                }
            }
        }
    } else {
        for (k = 0; k < height; k++) {
            x = gdp.x;
            y++;
            for (j = 0; j < width; j++) {
                x++;
                cindex = (databits)[k*width+j];
                if (cindex != bg || pixmap_type == PIXMAP_OPAQUE) {
                    color = rst_colors[cindex];
                    gdImageSetPixel(ihandle, x, y, color);
                }
            }
        }
    }
}
     
void rst_leavegraphics(void)
{
    /* Output the image to the disk file. */
    switch (curformat) {
    case RST_FORMAT_PNM:
        rstImagePnm(ihandle, prstream);
        break;   
#ifdef HAVE_LIBJPEG
    case RST_FORMAT_JPG:
        rstImageJpg(ihandle, prstream);
        break;   
#endif
#ifdef HAVE_LIBPNG
    case RST_FORMAT_PNG:
        if (png_setup_transparent == TRUE) {
            gdImageColorTransparent(ihandle, rst_colors[getbgcolor()]);
        }
        gdImageInterlace(ihandle, png_setup_interlaced);
        rstImagePng(ihandle, prstream);
        break;
#endif
    default:
        errmsg("Invalid raster format");  
        break;
    }
    
    /* Destroy the image in memory. */
    gdImageDestroy(ihandle);
    ihandle = NULL;
}

int pnminitgraphics(void)
{
    int result;
    
    result = rst_initgraphics(RST_FORMAT_PNM);
    
    if (result == RETURN_SUCCESS) {
        curformat = RST_FORMAT_PNM;
    }
    
    return (result);
}

static int pnm_setup_format = DEFAULT_PNM_FORMAT;
static int pnm_setup_rawbits = TRUE;

static void rstImagePnm(gdImagePtr ihandle, FILE *prstream)
{
    int w, h;
    int i, j, k;
    int c;
    unsigned char r, g, b;
    unsigned char y, pbm_buf;
    
    if (pnm_setup_rawbits == TRUE) {
        switch (pnm_setup_format) {
        case PNM_FORMAT_PBM:
            fprintf(prstream, "P4\n");
            break;
        case PNM_FORMAT_PGM:
            fprintf(prstream, "P5\n");
            break;
        case PNM_FORMAT_PPM:
            fprintf(prstream, "P6\n");
            break;
        }
    } else {
        switch (pnm_setup_format) {
        case PNM_FORMAT_PBM:
            fprintf(prstream, "P1\n");
            break;
        case PNM_FORMAT_PGM:
            fprintf(prstream, "P2\n");
            break;
        case PNM_FORMAT_PPM:
            fprintf(prstream, "P3\n");
            break;
        }
    }
    
    fprintf(prstream, "#Creator: %s\n", bi_version_string());
    
    w = gdImageSX(ihandle);
    h = gdImageSY(ihandle);
    fprintf(prstream, "%d %d\n", w, h);
    
    if (pnm_setup_format != PNM_FORMAT_PBM) {
        fprintf(prstream, "255\n");
    }
    
    k = 0;
    pbm_buf = 0;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            c = gdImageGetPixel(ihandle, j, i);
            r = (unsigned char) gdImageRed(ihandle, c);
            g = (unsigned char) gdImageGreen(ihandle, c);
            b = (unsigned char) gdImageBlue(ihandle, c);
            if (pnm_setup_rawbits == TRUE) {
                switch (pnm_setup_format) {
                case PNM_FORMAT_PBM:
                    y = (r == 255 &&  g == 255 && b == 255 ? 0x00:0x01);
                    pbm_buf |= (y << (7 - k));
                    k++;
                    /* completed byte or padding line */
                    if (k == 8 || j == w - 1) {
                        fwrite(&pbm_buf, 1, 1, prstream);
                        k = 0;
                        pbm_buf = 0;
                    }
                    break;
                case PNM_FORMAT_PGM:
                    y = INTENSITY(r, g, b);
                    fwrite(&y, 1, 1, prstream);
                    break;
                case PNM_FORMAT_PPM:
                    fwrite(&r, 1, 1, prstream);
                    fwrite(&g, 1, 1, prstream);
                    fwrite(&b, 1, 1, prstream);
                    break;
                }
            } else {
                switch (pnm_setup_format) {
                case PNM_FORMAT_PBM:
                    y = (r == 255 &&  g == 255 && b == 255 ? 0:1);
                    fprintf(prstream, "%1d\n", y);
                    break;
                case PNM_FORMAT_PGM:
                    y = INTENSITY(r, g, b);
                    fprintf(prstream, "%3d\n", y);
                    break;
                case PNM_FORMAT_PPM:
                    fprintf(prstream, "%3d %3d %3d\n", r, g, b);
                    break;
                }
            }
        }
    }
}


#ifdef HAVE_LIBJPEG
int jpginitgraphics(void)
{
    int result;
    
    result = rst_initgraphics(RST_FORMAT_JPG);
    
    if (result == RETURN_SUCCESS) {
        curformat = RST_FORMAT_JPG;
    }
    
    return (result);
}

static void rstImageJpg(gdImagePtr ihandle, FILE *prstream)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    J_DCT_METHOD dct_method;
    JSAMPROW row_pointer;        /* pointer to a single row */
    int w, h;
    int i, j, k;
    int c;
    int r, g, b;
    unsigned char y;

    w = gdImageSX(ihandle);
    h = gdImageSY(ihandle);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, prstream);
    
    cinfo.image_width  = w;
    cinfo.image_height = h;
    if (jpg_setup_grayscale) {
        cinfo.input_components = 1;
        cinfo.in_color_space = JCS_GRAYSCALE;
    } else {
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
    }

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, jpg_setup_quality, jpg_setup_baseline);

    cinfo.smoothing_factor = jpg_setup_smoothing;

    switch (jpg_setup_dct) {
    case JPEG_DCT_IFAST:
        dct_method = JDCT_IFAST;
        break;
    case JPEG_DCT_ISLOW:
        dct_method = JDCT_ISLOW;
        break;
    case JPEG_DCT_FLOAT:
        dct_method = JDCT_FLOAT;
        break;
    default:
        dct_method = JDCT_DEFAULT;
    }
    cinfo.dct_method = dct_method;

    if (jpg_setup_progressive) {
#ifdef C_PROGRESSIVE_SUPPORTED
        jpeg_simple_progression(&cinfo);
#else
        errmsg("jpeglib: sorry, progressive output was not compiled");
#endif
    }

    if (jpg_setup_optimize) {
#ifdef ENTROPY_OPT_SUPPORTED
        cinfo.optimize_coding = TRUE;
#else
        errmsg("jpeglib: sorry, entropy optimization was not compiled");
#endif
    }

    jpeg_start_compress(&cinfo, TRUE);
    
    if (jpg_setup_grayscale) {
        row_pointer = xmalloc(w);
    } else {
        row_pointer = xmalloc(3*w);
    }
    while ((i = cinfo.next_scanline) < h) {
        k = 0;
        for (j = 0; j < w; j++) {
            c = gdImageGetPixel(ihandle, j, i);
            r = gdImageRed(ihandle, c);
            g = gdImageGreen(ihandle, c);
            b = gdImageBlue(ihandle, c);
            if (jpg_setup_grayscale) {
                y = INTENSITY(r, g, b);
                row_pointer[k++] = y;
            } else {
                row_pointer[k++] = r;
                row_pointer[k++] = g;
                row_pointer[k++] = b;
            }
        }
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }
    xfree(row_pointer);
    
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}

int jpg_op_parser(char *opstring)
{
    char *bufp;
    
    if (!strcmp(opstring, "grayscale")) {
        jpg_setup_grayscale = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "color")) {
        jpg_setup_grayscale = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "optimize:on")) {
        jpg_setup_optimize = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "optimize:off")) {
        jpg_setup_optimize = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "baseline:on")) {
        jpg_setup_baseline = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "baseline:off")) {
        jpg_setup_baseline = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "progressive:on")) {
        jpg_setup_progressive = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "progressive:off")) {
        jpg_setup_progressive = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "dct:ifast")) {
        jpg_setup_dct = JPEG_DCT_IFAST;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "dct:islow")) {
        jpg_setup_dct = JPEG_DCT_ISLOW;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "dct:float")) {
        jpg_setup_dct = JPEG_DCT_FLOAT;
        return RETURN_SUCCESS;
    } else if (!strncmp(opstring, "quality:", 8)) {
        bufp = strchr(opstring, ':');
        bufp++;
        if (bufp != NULL && *bufp != '\0') {
            jpg_setup_quality = atoi(bufp);
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    } else if (!strncmp(opstring, "smoothing:", 10)) {
        bufp = strchr(opstring, ':');
        bufp++;
        if (bufp != NULL && *bufp != '\0') {
            jpg_setup_smoothing = atoi(bufp);
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    } else {
        return RETURN_FAILURE;
    }
}
#endif

int pnm_op_parser(char *opstring)
{
    if (!strcmp(opstring, "rawbits:on")) {
        pnm_setup_rawbits = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "rawbits:off")) {
        pnm_setup_rawbits = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "format:pbm")) {
        pnm_setup_format = PNM_FORMAT_PBM;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "format:pgm")) {
        pnm_setup_format = PNM_FORMAT_PGM;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "format:ppm")) {
        pnm_setup_format = PNM_FORMAT_PPM;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

#ifdef HAVE_LIBPNG
int pnginitgraphics(void)
{
    int result;
    
    result = rst_initgraphics(RST_FORMAT_PNG);
    
    if (result == RETURN_SUCCESS) {
        curformat = RST_FORMAT_PNG;
    }
    
    return (result);
}

static void rstImagePng(gdImagePtr ihandle, FILE *prstream)
{
    png_structp png_ptr;
    png_infop info_ptr;
    int w, h;
    int interlace_type;
    int i, num_palette;
    png_color *palette;
    png_byte trans;
    int num_text;
    png_text text_ptr[4];
    char *s;
    png_uint_32 res_meter;
    
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);
    if (png_ptr == NULL) {
        return;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    png_init_io(png_ptr, prstream);

    /* set the zlib compression level */
    png_set_compression_level(png_ptr, png_setup_compression);

    w = gdImageSX(ihandle);
    h = gdImageSY(ihandle);

    if (png_setup_interlaced) {
        interlace_type = PNG_INTERLACE_ADAM7;
    } else {
        interlace_type = PNG_INTERLACE_NONE;
    }

    png_set_IHDR(png_ptr, info_ptr, w, h,
        8, PNG_COLOR_TYPE_PALETTE, interlace_type,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    num_palette = gdImageColorsTotal(ihandle);
    palette = xmalloc(num_palette*sizeof(png_color));
    if (palette == NULL) {
        return;
    }
    for (i = 0; i < num_palette; i++) {
        palette[i].red   = gdImageRed(ihandle, i);
        palette[i].green = gdImageGreen(ihandle, i);
        palette[i].blue  = gdImageBlue(ihandle, i);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
    
    res_meter = (png_uint_32) rint(page_dpi/MM_PER_INCH*1000.0);
    png_set_pHYs(png_ptr, info_ptr, res_meter, res_meter, PNG_RESOLUTION_METER);

#ifdef PNG_WRITE_tRNS_SUPPORTED
    if (png_setup_transparent) {
        trans = gdImageGetTransparent(ihandle);
        png_set_tRNS(png_ptr, info_ptr, &trans, 1, NULL);
    }
#endif
    
#if (defined(PNG_WRITE_tEXt_SUPPORTED) || defined(PNG_WRITE_zTXt_SUPPORTED))
    text_ptr[0].key         = "Title";
    text_ptr[0].text        = get_docname();
    text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
    text_ptr[1].key         = "Author";
    text_ptr[1].text        = get_username();
    text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
    text_ptr[2].key         = "Software";
    text_ptr[2].text        = bi_version_string();
    text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
    num_text = 3;
    if ((s = get_project_description())) {
        text_ptr[3].key         = "Description";
        text_ptr[3].text        = s;
        if (strlen(s) > 1024) {
            text_ptr[3].compression = PNG_TEXT_COMPRESSION_zTXt;
        } else {
            text_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;
        }
        num_text++;
    }
    png_set_text(png_ptr, info_ptr, text_ptr, num_text);
#endif
    
    png_write_info(png_ptr, info_ptr);
    
    png_write_image(png_ptr, (png_byte **) ihandle->pixels);
    
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    xfree(palette);
}

int png_op_parser(char *opstring)
{
    char *bufp;

    if (!strcmp(opstring, "interlaced:on")) {
        png_setup_interlaced = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "interlaced:off")) {
        png_setup_interlaced = FALSE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "transparent:on")) {
        png_setup_transparent = TRUE;
        return RETURN_SUCCESS;
    } else if (!strcmp(opstring, "transparent:off")) {
        png_setup_transparent = FALSE;
        return RETURN_SUCCESS;
    } else if (!strncmp(opstring, "compression:", 12)) {
        bufp = strchr(opstring, ':');
        bufp++;
        if (bufp != NULL && *bufp != '\0') {
            png_setup_compression = atoi(bufp);
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    } else {
        return RETURN_FAILURE;
    }
}
#endif

#ifndef NONE_GUI

static void update_pnm_setup_frame(void);
static int set_pnm_setup_proc(void *data);
static Widget pnm_setup_frame;
static Widget pnm_setup_rawbits_item;
static Widget *pnm_setup_format_item;

#ifdef HAVE_LIBPNG
static void update_png_setup_frame(void);
static int set_png_setup_proc(void *data);

static Widget png_setup_frame;
static Widget png_setup_interlaced_item;
static Widget png_setup_transparent_item;
static SpinStructure *png_setup_compression_item;

void png_gui_setup(void)
{
    set_wait_cursor();
    
    if (png_setup_frame == NULL) {
        Widget fr, rc;
        
	png_setup_frame = CreateDialogForm(app_shell, "PNG options");

	fr = CreateFrame(png_setup_frame, "PNG options");
        rc = CreateVContainer(fr);
	png_setup_interlaced_item = CreateToggleButton(rc, "Interlaced");
	png_setup_transparent_item = CreateToggleButton(rc, "Transparent");
	png_setup_compression_item = CreateSpinChoice(rc,
            "Compression:", 1, SPIN_TYPE_INT,
            (double) Z_NO_COMPRESSION, (double) Z_BEST_COMPRESSION, 1.0);

	CreateAACDialog(png_setup_frame, fr, set_png_setup_proc, NULL);
    }
    update_png_setup_frame();
    
    RaiseWindow(GetParent(png_setup_frame));
    unset_wait_cursor();
}

static void update_png_setup_frame(void)
{
    if (png_setup_frame) {
        SetToggleButtonState(png_setup_interlaced_item, png_setup_interlaced);
        SetToggleButtonState(png_setup_transparent_item, png_setup_transparent);
        SetSpinChoice(png_setup_compression_item, png_setup_compression);
    }
}

static int set_png_setup_proc(void *data)
{
    png_setup_interlaced = GetToggleButtonState(png_setup_interlaced_item);
    png_setup_transparent = GetToggleButtonState(png_setup_transparent_item);
    png_setup_compression = GetSpinChoice(png_setup_compression_item);
    
    return RETURN_SUCCESS;
}
#endif

void pnm_gui_setup(void)
{
    set_wait_cursor();
    
    if (pnm_setup_frame == NULL) {
        Widget fr, rc;
        
	pnm_setup_frame = CreateDialogForm(app_shell, "PNM options");

	fr = CreateFrame(pnm_setup_frame, "PNM options");
        rc = CreateVContainer(fr);
	pnm_setup_format_item = CreatePanelChoice(rc, "Format: ",
					 4,
					 "1-bit mono (PBM)",
					 "8-bit grayscale (PGM)",
					 "8-bit color (PPM)",
                                         NULL);
	pnm_setup_rawbits_item = CreateToggleButton(rc, "\"Rawbits\"");

	CreateAACDialog(pnm_setup_frame, fr, set_pnm_setup_proc, NULL);
    }
    update_pnm_setup_frame();

    RaiseWindow(GetParent(pnm_setup_frame));
    unset_wait_cursor();
}

static void update_pnm_setup_frame(void)
{
    if (pnm_setup_frame) {
        SetChoice(pnm_setup_format_item, pnm_setup_format);
        SetToggleButtonState(pnm_setup_rawbits_item, pnm_setup_rawbits);
    }
}

static int set_pnm_setup_proc(void *data)
{
    pnm_setup_format = GetChoice(pnm_setup_format_item);
    pnm_setup_rawbits = GetToggleButtonState(pnm_setup_rawbits_item);
    
    return RETURN_SUCCESS;
}

#ifdef HAVE_LIBJPEG
static void update_jpg_setup_frame(void);
static int set_jpg_setup_proc(void *data);

static Widget jpg_setup_frame;
static Widget jpg_setup_grayscale_item;
static Widget jpg_setup_baseline_item;
static Widget jpg_setup_optimize_item;
static Widget jpg_setup_progressive_item;
static SpinStructure *jpg_setup_quality_item;
static SpinStructure *jpg_setup_smoothing_item;
static Widget *jpg_setup_dct_item;

void jpg_gui_setup(void)
{
    set_wait_cursor();
    
    if (jpg_setup_frame == NULL) {
        Widget jpg_setup_rc, fr, rc;
        
	jpg_setup_frame = CreateDialogForm(app_shell, "JPEG options");

        jpg_setup_rc = CreateVContainer(jpg_setup_frame);

	fr = CreateFrame(jpg_setup_rc, "JPEG options");
        rc = CreateVContainer(fr);
	jpg_setup_quality_item = CreateSpinChoice(rc,
            "Quality:", 3, SPIN_TYPE_INT, 0.0, 100.0, 5.0);
	jpg_setup_optimize_item = CreateToggleButton(rc, "Optimize");
	jpg_setup_progressive_item = CreateToggleButton(rc, "Progressive");
	jpg_setup_grayscale_item = CreateToggleButton(rc, "Grayscale");

	fr = CreateFrame(jpg_setup_rc, "JPEG advanced options");
        rc = CreateVContainer(fr);
	jpg_setup_smoothing_item = CreateSpinChoice(rc,
            "Smoothing:", 3, SPIN_TYPE_INT, 0.0, 100.0, 10.0);
	jpg_setup_baseline_item = CreateToggleButton(rc, "Force baseline");
	jpg_setup_dct_item = CreatePanelChoice(rc, "DCT: ",
					 4,
					 "Fast integer",
					 "Slow integer",
					 "Float",
                                         NULL);

	CreateAACDialog(jpg_setup_frame, jpg_setup_rc, set_jpg_setup_proc, NULL);
    }
    update_jpg_setup_frame();

    RaiseWindow(GetParent(jpg_setup_frame));
    unset_wait_cursor();
}

static void update_jpg_setup_frame(void)
{
    if (jpg_setup_frame) {
        SetToggleButtonState(jpg_setup_grayscale_item, jpg_setup_grayscale);
        SetToggleButtonState(jpg_setup_baseline_item, jpg_setup_baseline);
        SetToggleButtonState(jpg_setup_optimize_item, jpg_setup_optimize);
        SetToggleButtonState(jpg_setup_progressive_item, jpg_setup_progressive);
        SetSpinChoice(jpg_setup_quality_item, jpg_setup_quality);
        SetSpinChoice(jpg_setup_smoothing_item, jpg_setup_smoothing);
        SetChoice(jpg_setup_dct_item, jpg_setup_dct);
    }
}

static int set_jpg_setup_proc(void *data)
{
    jpg_setup_grayscale = GetToggleButtonState(jpg_setup_grayscale_item);
    jpg_setup_baseline = GetToggleButtonState(jpg_setup_baseline_item);
    jpg_setup_optimize = GetToggleButtonState(jpg_setup_optimize_item);
    jpg_setup_progressive = GetToggleButtonState(jpg_setup_progressive_item);
    jpg_setup_quality = (int) GetSpinChoice(jpg_setup_quality_item);
    jpg_setup_smoothing = (int) GetSpinChoice(jpg_setup_smoothing_item);
    jpg_setup_dct = GetChoice(jpg_setup_dct_item);
    
    return RETURN_SUCCESS;
}
#endif

#endif
