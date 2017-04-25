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
 * driver for X11 for Grace
 *
 */

#include <config.h>
#include <cmath.h>
#include "defines.h"

#include <stdlib.h>

#include <X11/Xlib.h>

#include "globals.h"
#include "utils.h"
#include "device.h"
#include "devlist.h"
#include "draw.h"
#include "graphs.h"
#include "patterns.h"

#include "x11drv.h"

#include "protos.h"

extern Display *disp;
extern Window xwin;

Window root;
int screennumber;
GC gc, gcxor;
int depth;

static Visual *visual;
static int pixel_size;

int install_cmap = CMAP_INSTALL_AUTO;

static int private_cmap = FALSE;

unsigned long xvlibcolors[MAXCOLORS];
Colormap cmap;

static Pixmap displaybuff = (Pixmap) NULL;

static int xlibcolor;
static int xlibbgcolor;
static int xlibpatno;
static int xliblinewidth;
static int xliblinestyle;
static int xlibfillrule;
static int xlibarcfillmode;
static int xliblinecap;
static int xliblinejoin;

unsigned int win_h = 0, win_w = 0;
#define win_scale ((win_h < win_w) ? win_h:win_w)

Pixmap resize_bufpixmap(unsigned int w, unsigned int h);

static Device_entry dev_x11 = {DEVICE_TERM,
          "X11",
          xlibinitgraphics,
          NULL,
          NULL,
          "",
          FALSE,
          TRUE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };

int register_x11_drv(void)
{
    long mrsize;
    int max_path_limit;
    
    /* XExtendedMaxRequestSize() appeared in X11R6 */
#if XlibSpecificationRelease > 5
    mrsize = XExtendedMaxRequestSize(disp);
#else
    mrsize = 0;
#endif
    if (mrsize <= 0) {
        mrsize = XMaxRequestSize(disp);
    }
    max_path_limit = (mrsize - 3)/2;
    if (max_path_limit < get_max_path_limit()) {
        char buf[128];
        sprintf(buf,
            "Setting max drawing path length to %d (limited by the X server)",
            max_path_limit);
        errmsg(buf);
        set_max_path_limit(max_path_limit);
    }
    
    dev_x11.pg.dpi = rint(MM_PER_INCH*DisplayWidth(disp, screennumber)/
        DisplayWidthMM(disp, screennumber));
    
    return register_device(dev_x11);
}

int xlibinit(void)
{
    XGCValues gc_val;
    XPixmapFormatValues *pmf;
    int i, n;
    
    screennumber = DefaultScreen(disp);
    visual = DefaultVisual(disp, screennumber);
    root = RootWindow(disp, screennumber);
 
    gc = DefaultGC(disp, screennumber);
    
    depth = DisplayPlanes(disp, screennumber);

    pixel_size = 0;
    pmf = XListPixmapFormats (disp, &n);
    if (pmf) {
        for (i = 0; i < n; i++) {
            if (pmf[i].depth == depth) {
                pixel_size = pmf[i].bits_per_pixel/8;
                break;
            }
        }
        XFree ((char *) pmf);
    }
    if (pixel_size == 0) {
        monomode = TRUE;
    }

/*
 * init colormap
 */
    cmap = DefaultColormap(disp, screennumber);
    /* redefine colormap, if needed */
    if (install_cmap == CMAP_INSTALL_ALWAYS) {
        cmap = XCopyColormapAndFree(disp, cmap);
        private_cmap = TRUE;
    }
    xlibinitcmap();
    
/*
 * set GCs
 */
    gc_val.foreground = xvlibcolors[0];
    gc_val.background = xvlibcolors[1];
    if (invert) {
        gc_val.function = GXinvert;
    } else {
        gc_val.function = GXxor;
    }
    gcxor = XCreateGC(disp, root, GCFunction | GCForeground, &gc_val);

    displaybuff = resize_bufpixmap(win_w, win_h);
    
/*
 * disable font AA in mono mode
 */
    if (monomode == TRUE) {
        Device_entry dev;
        dev = get_device_props(tdevice);
        dev.fontaa = FALSE;
        set_device_props(tdevice, dev);
    }

    return RETURN_SUCCESS;
}


int xconvxlib(double x)
{
    return ((int) rint(win_scale * x));
}

int yconvxlib(double y)
{
    return ((int) rint(win_h - win_scale * y));
}

void xlibVPoint2dev(VPoint vp, int *x, int *y)
{
    *x = xconvxlib(vp.x);
    *y = yconvxlib(vp.y);
}

XPoint VPoint2XPoint(VPoint vp)
{
    XPoint xp;
    
    xp.x = xconvxlib(vp.x);
    xp.y = yconvxlib(vp.y);
    
    return(xp);
}

/*
 * xlibdev2VPoint - given (x,y) in screen coordinates, return the 
 * viewport coordinates
 */
VPoint xlibdev2VPoint(int x, int y)
{
    VPoint vp;

    if (win_scale == 0) {
        vp.x = (double) 0.0;
        vp.y = (double) 0.0;
    } else {
        vp.x = (double) x / win_scale;
        vp.y = (double) (win_h - y) / win_scale;
    }

    return (vp);        
}


void xlibupdatecmap(void)
{
    /* TODO: replace!!! */
    if (inwin) {
        xlibinitcmap();
    }
}


void xlibinitcmap(void)
{
    int i;
    RGB *prgb;
    XColor xc[MAXCOLORS];
    
    for (i = 0; i < MAXCOLORS; i++) {
        xc[i].pixel = 0;
        xc[i].flags = DoRed | DoGreen | DoBlue;
    }
    
    for (i = 0; i < number_of_colors(); i++) {
        /* even in mono, b&w must be allocated */
        if (monomode == FALSE || i < 2) { 
            prgb = get_rgb(i);
            if (prgb != NULL) {
                xc[i].red = prgb->red << (16 - GRACE_BPP);
                xc[i].green = prgb->green << (16 - GRACE_BPP);
                xc[i].blue = prgb->blue << (16 - GRACE_BPP);
                if (XAllocColor(disp, cmap, &xc[i])) {
                    xvlibcolors[i] = xc[i].pixel;
                } else {
                    if (install_cmap != CMAP_INSTALL_NEVER && 
                                        private_cmap == FALSE) {
                        cmap = XCopyColormapAndFree(disp, cmap);
                        private_cmap = TRUE;
                        /* will try to allocate the same color 
                         * in the private colormap
                         */
                        i--; 
                    } else {
                        /* really bad */
                        xvlibcolors[i] = xvlibcolors[1];
/*
 *                         errmsg("Can't allocate color");
 */
                    }
                }
            }
        } else {
            xvlibcolors[i] = xvlibcolors[1];
        }
    }
}

int xlibinitgraphics(void)
{
    int i, j;
    double step;
    XPoint xp;
    
    if (inwin == FALSE) {
        return RETURN_FAILURE;
    }

    xlibcolor = BAD_COLOR;
    xlibbgcolor = BAD_COLOR;
    xlibpatno = -1;
    xliblinewidth = -1;
    xliblinestyle = -1;
    xlibfillrule = -1;
    xlibarcfillmode = -1;
    xliblinecap   = -1;
    xliblinejoin  = -1;
    
    /* device-dependent routines */    
    devupdatecmap = xlibupdatecmap;
    
    devdrawpixel = xlibdrawpixel;
    devdrawpolyline = xlibdrawpolyline;
    devfillpolygon = xlibfillpolygon;
    devdrawarc = xlibdrawarc;
    devfillarc = xlibfillarc;
    devputpixmap = xlibputpixmap;
    
    devleavegraphics = xlibleavegraphics;

    /* init settings specific to X11 driver */    
    
    if (get_pagelayout() == PAGE_FIXED) {
        sync_canvas_size(&win_w, &win_h, FALSE);
    } else {
        sync_canvas_size(&win_w, &win_h, TRUE);
    }
    
    displaybuff = resize_bufpixmap(win_w, win_h);
    
    xlibupdatecmap();
    
    XSetForeground(disp, gc, xvlibcolors[0]);
    XSetFillStyle(disp, gc, FillSolid);
    XFillRectangle(disp, displaybuff, gc, 0, 0, win_w, win_h);
    XSetForeground(disp, gc, xvlibcolors[1]);
    
    step = (double) win_scale/10;
    for (i = 0; i < win_w/step; i++) {
        for (j = 0; j < win_h/step; j++) {
            xp.x = rint(i*step);
            xp.y = win_h - rint(j*step);
            XDrawPoint(disp, displaybuff, gc, xp.x, xp.y);
        }
    }
    
    XSetLineAttributes(disp, gc, 1, LineSolid, CapButt, JoinMiter);
    XDrawRectangle(disp, displaybuff, gc, 0, 0, win_w - 1, win_h - 1);
    
    return RETURN_SUCCESS;
}


void xlib_setpen(void)
{
    int fg, bg, p;
    
    fg = getcolor();
    bg = getbgcolor();
    p = getpattern();
    
    if ((fg == xlibcolor) && (bg == xlibbgcolor) && (p == xlibpatno)) {
        return;
    }
        
    if (fg != xlibcolor) {
        XSetForeground(disp, gc, xvlibcolors[fg]);
        xlibcolor = fg;
    }
    
    if (bg != xlibbgcolor) {
        XSetBackground(disp, gc, xvlibcolors[bg]);
        xlibbgcolor = bg;
    }

    if (p >= number_of_patterns() || p < 0) {
        p = 0;
    }
    xlibpatno = p;
    
    if (p == 0) { /* TODO: transparency !!!*/
        return;
    } else if (p == 1) {
        /* To make X faster */
        XSetFillStyle(disp, gc, FillSolid);
    } else {
        /* TODO: implement cache ? */
        Pixmap ptmp = XCreatePixmapFromBitmapData(disp, root,
            (char *) pat_bits[p], 16, 16,
            xvlibcolors[fg], xvlibcolors[bg],
            PlanesOfScreen(DefaultScreenOfDisplay(disp)));
        
        XSetFillStyle(disp, gc, FillTiled);
        XSetTile(disp, gc, ptmp);
        
        XFreePixmap(disp, ptmp);
        
        return;
    }
}

void xlib_setdrawbrush(void)
{
    unsigned int iw;
    int style;
    int lc, lj;
    int i, scale, darr_len;
    char *xdarr;

    xlib_setpen();
    
    iw = (unsigned int) rint(getlinewidth()*win_scale);
    if (iw == 1) {
        iw = 0;
    }
    style = getlinestyle();
    lc = getlinecap();
    lj = getlinejoin();
    
    switch (lc) {
    case LINECAP_BUTT:
        lc = CapButt;
        break;
    case LINECAP_ROUND:
        lc = CapRound;
        break;
    case LINECAP_PROJ:
        lc = CapProjecting;
        break;
    }

    switch (lj) {
    case LINEJOIN_MITER:
        lj = JoinMiter;
        break;
    case LINEJOIN_ROUND:
        lj = JoinRound;
        break;
    case LINEJOIN_BEVEL:
        lj = JoinBevel;
        break;
    }
    
    if (iw != xliblinewidth || style != xliblinestyle ||
        lc != xliblinecap   || lj    != xliblinejoin) {
        if (style > 1) {
            darr_len = dash_array_length[style];
            xdarr = xmalloc(darr_len*SIZEOF_CHAR);
            if (xdarr == NULL) {
                return;
            }
            scale = MAX2(1, iw);
            for (i = 0; i < darr_len; i++) {
                xdarr[i] = scale*dash_array[style][i];
            }
            XSetLineAttributes(disp, gc, iw, LineOnOffDash, lc, lj);
            XSetDashes(disp, gc, 0, xdarr, darr_len);
            xfree(xdarr);
        } else if (style == 1) {
            XSetLineAttributes(disp, gc, iw, LineSolid, lc, lj);
        }
 
        xliblinestyle = style;
        xliblinewidth = iw;
        xliblinecap   = lc;
        xliblinejoin  = lj;
    }

    return;
}

void xlibdrawpixel(VPoint vp)
{
    XPoint xp;
    
    xp = VPoint2XPoint(vp);
    xlib_setpen();
    XDrawPoint(disp, displaybuff, gc, xp.x, xp.y);
}

void xlibdrawpolyline(VPoint *vps, int n, int mode)
{
    int i, xn = n;
    XPoint *p;
    
    if (n <= 1 || getlinestyle() == 0 || getpattern() == 0) {
        return;
    }
    
    if (mode == POLYLINE_CLOSED) {
        xn++;
    }
    
    p = xmalloc(xn*sizeof(XPoint));
    if (p == NULL) {
        return;
    }
    
    for (i = 0; i < n; i++) {
        p[i] = VPoint2XPoint(vps[i]);
    }
    if (mode == POLYLINE_CLOSED) {
        p[n] = p[0];
    }
    
    xlib_setdrawbrush();
    
    XDrawLines(disp, displaybuff, gc, p, xn, CoordModeOrigin);
    
    xfree(p);
}


void xlibfillpolygon(VPoint *vps, int npoints)
{
    int i;
    XPoint *p;
    
    if (npoints < 3 || getpattern() == 0) {
        return;
    }
    
    p = (XPoint *) xmalloc(npoints*sizeof(XPoint));
    if (p == NULL) {
        return;
    }
    
    for (i = 0; i < npoints; i++) {
        p[i] = VPoint2XPoint(vps[i]);
    }
    
    xlib_setpen();

    if (getfillrule() != xlibfillrule) {
        xlibfillrule = getfillrule();
        if (getfillrule() == FILLRULE_WINDING) {
            XSetFillRule(disp, gc, WindingRule);
        } else {
            XSetFillRule(disp, gc, EvenOddRule);
        }
    }

    XFillPolygon(disp, displaybuff, gc, p, npoints, Complex, CoordModeOrigin);
    
    xfree(p);
}

/*
 *  xlibdrawarc
 */
void xlibdrawarc(VPoint vp1, VPoint vp2, int angle1, int angle2)
{
    int x1, y1, x2, y2;
    
    xlibVPoint2dev(vp1, &x1, &y2);
    xlibVPoint2dev(vp2, &x2, &y1);

    if (getlinestyle() == 0 || getpattern() == 0) {
        return;
    }

    xlib_setdrawbrush();
    
    if (x1 != x2 || y1 != y2) {
        XDrawArc(disp, displaybuff, gc, MIN2(x1, x2), MIN2(y1, y2),
              abs(x2 - x1), abs(y2 - y1), 64 * angle1, 64 * (angle2 - angle1));
    } else { /* zero radius */
        XDrawPoint(disp, displaybuff, gc, x1, y1);
    }
}

/*
 *  xlibfillarc
 */
void xlibfillarc(VPoint vp1, VPoint vp2, int angle1, int angle2, int mode)
{
    int x1, y1, x2, y2;
    
    xlibVPoint2dev(vp1, &x1, &y2);
    xlibVPoint2dev(vp2, &x2, &y1);
    
    if (getpattern() != 0) {
        xlib_setpen();
        if (x1 != x2 || y1 != y2) {
            if (xlibarcfillmode != mode) {
                xlibarcfillmode = mode;
                if (mode == ARCFILL_CHORD) {
                    XSetArcMode(disp, gc, ArcChord);
                } else {
                    XSetArcMode(disp, gc, ArcPieSlice);
                }
            }
            XFillArc(disp, displaybuff, gc, MIN2(x1, x2), MIN2(y1, y2),
               abs(x2 - x1), abs(y2 - y1), 64 * angle1, 64 * (angle2 - angle1));
        } else { /* zero radius */
            XDrawPoint(disp, displaybuff, gc, x1, y1);
        }
    }
}


void xlibputpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    int j, k, l;
    
    XPoint xp;

    static XImage *ximage;
 
    Pixmap clipmask = 0;
    char *pixmap_ptr;
    char *clipmask_ptr = NULL;
    
    int line_off;

    int cindex, fg, bg;
    
    xp = VPoint2XPoint(vp);
      
    if (pixmap_bpp != 1) {
        if (monomode == TRUE) {
            /* TODO: dither pixmaps on mono displays */
            return;
        }
        pixmap_ptr = xcalloc(PAD(width, 8) * height, pixel_size);
        if (pixmap_ptr == NULL) {
            errmsg("xmalloc failed in xlibputpixmap()");
            return;
        }
 
        /* re-index pixmap */
        for (k = 0; k < height; k++) {
            for (j = 0; j < width; j++) {
                cindex = (unsigned char) (databits)[k*width+j];
                for (l = 0; l < pixel_size; l++) {
                    pixmap_ptr[pixel_size*(k*width+j) + l] =
                                        (char) (xvlibcolors[cindex] >> (8*l));
                }
            }
        }

        ximage=XCreateImage(disp, visual,
                           depth, ZPixmap, 0, pixmap_ptr, width, height,
                           bitmap_pad,  /* lines padded to bytes */
                           0 /* number of bytes per line */
                           );

        if (pixmap_type == PIXMAP_TRANSPARENT) {
            clipmask_ptr = xcalloc((PAD(width, 8)>>3)
                                              * height, SIZEOF_CHAR);
            if (clipmask_ptr == NULL) {
                errmsg("xmalloc failed in xlibputpixmap()");
                return;
            } else {
                /* Note: We pad the clipmask always to byte boundary */
                bg = getbgcolor();
                for (k = 0; k < height; k++) {
                    line_off = k*(PAD(width, 8) >> 3);
                    for (j = 0; j < width; j++) {
                        cindex = (unsigned char) (databits)[k*width+j];
                        if (cindex != bg) {
                            clipmask_ptr[line_off+(j>>3)] |= (0x01 << (j%8));
                        }
                    }
                }
        
                clipmask=XCreateBitmapFromData(disp, root, clipmask_ptr, 
                                                            width, height);
                xfree(clipmask_ptr);
            }
        }
    } else {
        pixmap_ptr = xcalloc((PAD(width, bitmap_pad)>>3) * height,
                                                        sizeof(unsigned char));
        if (pixmap_ptr == NULL) {
            errmsg("xmalloc failed in xlibputpixmap()");
            return;
        }
        memcpy(pixmap_ptr, databits, ((PAD(width, bitmap_pad)>>3) * height));

        fg = getcolor();
        if (fg != xlibcolor) {
            XSetForeground(disp, gc, xvlibcolors[fg]);
            xlibcolor = fg;
        }
        ximage=XCreateImage(disp, visual,
                            1, XYBitmap, 0, pixmap_ptr, width, height,
                            bitmap_pad, /* lines padded to bytes */
                            0 /* number of bytes per line */
                            );
        if (pixmap_type == PIXMAP_TRANSPARENT) {
            clipmask=XCreateBitmapFromData(disp, root, pixmap_ptr, 
                                              PAD(width, bitmap_pad), height);
        }
    }

    if (pixmap_type == PIXMAP_TRANSPARENT) {
        XSetClipMask(disp, gc, clipmask);
        XSetClipOrigin(disp, gc, xp.x, xp.y);
    }
        
        
    /* Force bit and byte order */
    ximage->bitmap_bit_order=LSBFirst;
    ximage->byte_order=LSBFirst;
    
    XPutImage(disp, displaybuff, gc, ximage, 0, 0, xp.x, xp.y, width, height);
    XDestroyImage(ximage);
     
    if (pixmap_type == PIXMAP_TRANSPARENT) {
        XFreePixmap(disp, clipmask);
        clipmask = 0;
        XSetClipMask(disp, gc, None);
        XSetClipOrigin(disp, gc, 0, 0);
    }    
}

void xlibleavegraphics(void)
{
    int cg = get_cg();
    
    if (is_graph_hidden(cg) == FALSE) {
        draw_focus(cg);
    }
    reset_crosshair();
    xlibredraw(xwin, 0, 0, win_w, win_h);
    
    XFlush(disp);
}
