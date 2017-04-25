/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
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
 * Driver for the Maker Interchange Format
 */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "utils.h"
#include "cmath.h"
#include "draw.h"
#include "device.h"
#include "devlist.h"
#include "patterns.h"
#include "mifdrv.h"

#define MIF_MARGIN 15.0

extern FILE *prstream;

static Device_entry dev_mif = {DEVICE_FILE,
                               "MIF",
                               mifinitgraphics,
                               NULL,
                               NULL,
                               "mif",
                               TRUE,
                               FALSE,
                               {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
                               NULL
                              };

/* mapping between Grace and MIF fill patterns. This is really ugly but
 * MIF uses only 16 patterns which can only be customised on UNIX platforms
 * and there only for the whole FrameMaker-product and not for a single
 * document. */
static int mif_fillpattern(int fillpattern)
{
  switch (fillpattern) {
  case 0 :
    return 15;
  case 1 :
    return 0;
  case 2 :
    return 1;
  case 3 :
    return 2;
  case 4 :
    return 3;
  case 5 :
    return 4;
  case 6 :
    return 5;
  case 7 :
    return 6;
  case 8 :
    return 7;
  case 9 :
    return 8;
  case 10 :
    return 9;
  case 11 :
    return 10;
  case 12 :
    return 11;
  case 13 :
    return 12;
  case 14 :
    return 13;
  case 15 :
    return 14;
  case 16 :
    return 10;
  case 17 :
    return 11;
  case 18 :
    return 12;
  case 19 :
    return 2;
  case 20 :
    return 3;
  case 21 :
    return 4;
  case 22 :
    return 5;
  case 23 :
    return 6;
  case 24 :
    return 7;
  case 25 :
    return 8;
  case 26 :
    return 9;
  case 27 :
    return 10;
  case 28 :
    return 11;
  case 29 :
    return 12;
  case 30 :
    return 13;
  case 31 :
    return 14;
  default :
    return 0;
  }
}

/*
 * escape special characters
 */
static char *escape_specials(unsigned char *s, int len)
{
    static char *es = NULL;
    int i, elen = 0;
    
    /* Define Array with all charactercodes from 128 to 255 for the
       conversion of the ISOLatin1 codes to FrameMaker codes.
       Characters, which are not part of the FrameMaker characterset
       are coded as \xc0 (exclamdown)
       The following conversions are defined
       onesuperior -> 1
       twosuperior -> 2
       threesuperior -> 3
       degree -> ring
       multiply -> x
       Yacute -> Y
       divide -> :
       yacute -> y
       Matthias Dillier, 10.1.2001 */
    static char *code[128] = {
        "80","81","82","83","84","85","86","87",
        "88","89","8a","8b","8c","8d","8e","8f",
        "f5","d4","d5","f6","f7","f8","f9","fa",
        "ac","99","fb","fc","9c","fd","fe","c0",
        "a0","c1","a2","a3","db","b4","c0","a4",
        "ac","a9","bb","c7","c2","2d","a8","f8",
        "fb","c0","32","33","ab","c0","a6","e1",
        "fc","31","bc","c8","c0","c0","c0","c0",
        "cb","e7","e5","cc","80","81","ae","82",
        "e9","83","e6","e8","ed","ea","eb","ec",
        "c0","84","f1","ee","ef","cd","85","78",
        "af","f4","f2","f3","86","59","c0","a7",
        "88","87","89","8b","8a","8c","be","8d",
        "8f","8e","90","91","93","92","94","95",
        "c0","96","98","97","99","9b","9a","3a",
        "bf","9d","9c","9e","9f","79","c0","d8"
    };
    
    elen = 0;
    for (i = 0; i < len; i++) {
        if (s[i] == '\t' || s[i] == '>' || s[i] == '`'
            || s[i] == '\'' || s[i] == '\\') {
            elen++;
        } else if (s[i] > 0x7f) {
            elen += 4;
        }
        elen++;
    }
    
    es = xrealloc(es, (elen + 1)*SIZEOF_CHAR);
    
    elen = 0;
    
    for (i = 0; i < len; i++) {
        if (s[i] == '\t') {
            es[elen++] = '\\';
            es[elen++] = 't';
        } else if (s[i] == '>') {
            es[elen++] = '\\';
            es[elen++] = '>';
        } else if (s[i] == '`') {
            es[elen++] = '\\';
            es[elen++] = 'Q';
        } else if (s[i] == '\'') {
            es[elen++] = '\\';
            es[elen++] = 'q';
        } else if (s[i] == '\\') {
            es[elen++] = '\\';
            es[elen++] = '\\';
        } else if (s[i] > 0x7f) {
            es[elen++] = '\\';
            es[elen++] = 'x';

            /* Convert special characters to mif-charactercodes */
            es[elen++] = code[s[i] - 128][0];
            es[elen++] = code[s[i] - 128][1];
            es[elen++] = ' ';
        } else {
            es[elen++] = (char) s[i];
        }
    }
          
    es[elen] = '\0';
    
    return (es);
}

int register_mif_drv(void)
{
    return register_device(dev_mif);
}

int mifinitgraphics(void)
{
    int i;
    double *data;
    double c, m, y, k;
    fRGB *frgb;

    /* device-dependent routines */
    devupdatecmap   = NULL;
    
    devdrawpixel    = mif_drawpixel;
    devdrawpolyline = mif_drawpolyline;
    devfillpolygon  = mif_fillpolygon;
    devdrawarc      = mif_drawarc;
    devfillarc      = mif_fillarc;
    devputpixmap    = mif_putpixmap;
    devputtext      = mif_puttext;

    devleavegraphics = mif_leavegraphics;

    data = (double *) xrealloc(get_curdevice_data(), SIZEOF_DOUBLE);
    set_curdevice_data((void *) data);
    if (data == NULL) {
        return RETURN_FAILURE;
    }
    *data = MIN2(page_width_pp, page_height_pp);

    fprintf(prstream, "<MIFFile 5.50> # Generated by %s\n",
            bi_version_string());
    fprintf(prstream, "<Units Upt>\n");

    fprintf(prstream, "<ColorCatalog\n");
    for (i = 0; i < number_of_colors(); i++) {
        frgb = get_frgb(i);
        if (frgb != NULL) {

            /* convert RGB to CMYK */
            if (frgb->red > 1e-3 || frgb->green > 1e-3 || frgb->blue > 1e-3) {
                c = 100.0 - 100.0*frgb->red;
                m = 100.0 - 100.0*frgb->green;
                y = 100.0 - 100.0*frgb->blue;
                k = 0.0;
            } else {
                c = 0.0;
                m = 0.0;
                y = 0.0;
                k = 100.0;
            }

            fprintf(prstream, " <Color\n");
            fprintf(prstream, "  <ColorTag `%s'>\n", get_colorname(i));
            fprintf(prstream, "  <ColorCyan %10.6f>\n", c);
            fprintf(prstream, "  <ColorMagenta %10.6f>\n", m);
            fprintf(prstream, "  <ColorYellow %10.6f>\n", y);
            fprintf(prstream, "  <ColorBlack %10.6f>\n", k);
            if (c < 0.1 && m < 0.1 && y < 0.1 && k > 99.9) {
                fprintf(prstream, "  <ColorAttribute ColorIsBlack>\n");
            } else if (c < 0.1 && m < 0.1 && y < 0.1 && k < 0.1) {
                fprintf(prstream, "  <ColorAttribute ColorIsWhite>\n");
            } else if (c < 0.1 && m > 99.9 && y > 99.9 && k < 0.1) {
                fprintf(prstream, "  <ColorAttribute ColorIsRed>\n");
            } else if (c > 99.9 && m < 0.1 && y > 99.9 && k < 0.1) {
                fprintf(prstream, "  <ColorAttribute ColorIsGreen>\n");
            } else if (c > 99.9 && m > 99.9 && y < 0.1 && k < 0.1) {
                fprintf(prstream, "  <ColorAttribute ColorIsBlue>\n");
            } else if (c > 99.9 && m < 0.1 && y < 0.1 && k < 0.1) {
                fprintf(prstream, "  <ColorAttribute ColorIsCyan>\n");
            } else if (c < 0.1 && m > 99.9 && y < 0.1 && k < 0.1) {
                fprintf(prstream, "  <ColorAttribute ColorIsMagenta>\n");
            } else if (c < 0.1 && m < 0.1 && y > 99.9 && k < 0.1) {
                fprintf(prstream, "  <ColorAttribute ColorIsYellow>\n");
            }
            fprintf(prstream, " > # end of Color\n");
        }
    }
    fprintf(prstream, "> # end of ColorCatalog\n");

    fprintf(prstream, "<Document\n");
    fprintf(prstream, " <DPageSize %8.3f pt %8.3f pt>\n",
            page_width_pp + 2*MIF_MARGIN, page_height_pp + 2*MIF_MARGIN);
    fprintf(prstream, " <DMargins 0 pt 0 pt 0 pt 0 pt>\n");
    fprintf(prstream, " <DColumns 1>\n");
    fprintf(prstream, "> # end of Document\n");

    fprintf(prstream, "<Page # Create a right master page.\n");
    fprintf(prstream, " <PageType RightMasterPage>\n");
    fprintf(prstream, " <PageTag `Right'>\n");
    fprintf(prstream, " <TextRect\n");
    fprintf(prstream, "   <ID 10>\n");
    fprintf(prstream, "   <Pen 15>\n");
    fprintf(prstream, "   <Fill 15>\n");
    fprintf(prstream, "   <ShapeRect 0 pt 0 pt %8.3f pt %8.3f pt>\n",
            page_width_pp + 2*MIF_MARGIN, page_height_pp + 2*MIF_MARGIN);
    fprintf(prstream, "   <TRNumColumns 1>\n");
    fprintf(prstream, "   <TRColumnGap 0.0 pt>\n");
    fprintf(prstream, " > # end of TextRect\n");
    fprintf(prstream, "> # end of Page\n");

    fprintf(prstream, "<Page # Create a body page.\n");
    fprintf(prstream, " <PageType BodyPage>\n");
    fprintf(prstream, " <PageNum `1'>\n");
    fprintf(prstream, " <PageAngle 0>\n");
    fprintf(prstream, " <PageBackground `Default'>\n");
    fprintf(prstream, " <TextRect\n");
    fprintf(prstream, "   <ID 20>\n");
    fprintf(prstream, "   <ShapeRect 0 pt 0 pt %8.3f pt %8.3f pt>\n",
            page_width_pp + 2*MIF_MARGIN, page_height_pp + 2*MIF_MARGIN);
    fprintf(prstream, "   <TRNumColumns 1> \n");
    fprintf(prstream, "   <TRColumnGap 0.0 pt>\n");
    fprintf(prstream, " > # end TextRect\n");
    fprintf(prstream, "> # end Page\n");

    fprintf(prstream, "<AFrames\n");
    fprintf(prstream, " <Frame\n");
    fprintf(prstream, "  <ID 30>\n");
    fprintf(prstream, "  <Pen 15>\n");
    fprintf(prstream, "  <Fill 15>\n");
    fprintf(prstream, "  <RunaroundGap  0 pt>\n");
    fprintf(prstream, "  <RunaroundType None>\n");
    fprintf(prstream, "   <ShapeRect 0 pt 0 pt %8.3f pt %8.3f pt>\n",
            page_width_pp + 2*MIF_MARGIN, page_height_pp + 2*MIF_MARGIN);
    fprintf(prstream, "  <FrameType RunIntoParagraph>\n");
    fprintf(prstream, "  <NSOffset  0.0 mm>\n");
    fprintf(prstream, "  <BLOffset  0.0 mm>\n");
    fprintf(prstream, "  <AnchorAlign Left>\n");

    return RETURN_SUCCESS;
}

void mif_object_props (int draw, int fill)
{
    int i, ls;
    double lw;
    Pen pen;
    double side;

    pen = getpen();
    if (draw) {
        fprintf(prstream, "   <Pen 0>\n");
        side = *((double *) get_curdevice_data());
        lw = side*getlinewidth();
        fprintf(prstream, "   <PenWidth %8.3f pt>\n", lw);

        fprintf(prstream, "   <DashedPattern\n");
        ls = getlinestyle();

        if (ls <= 1) {
        fprintf(prstream, "    <DashedStyle Solid>\n");
        } else {
          fprintf(prstream, "   <DashedStyle Dashed>\n");
          for (i = 0; i < dash_array_length[ls]; i++) {
            fprintf(prstream, "   <DashSegment %8.3f pt>\n",
                    lw*dash_array[ls][i]);
          }
        }
        fprintf(prstream, "   > # end of DashedPattern\n");
    } else {
        fprintf(prstream, "   <Pen 15>\n");
    }

    if (fill) {
        fprintf(prstream, "   <Fill %d>\n", mif_fillpattern(pen.pattern));
    } else {
        fprintf(prstream, "   <Fill 15>\n");
    }
    fprintf(prstream, "   <ObColor `%s'>\n", get_colorname(pen.color));
    fprintf(prstream, "   <GroupID 1>\n");

}

void mif_drawpixel(VPoint vp)
{
    double side;

    side = *((double *) get_curdevice_data());

    fprintf(prstream,
            "  <Rectangle\n");
    mif_object_props(FALSE, TRUE);
    fprintf(prstream,
            "   <ShapeRect %8.3f pt %8.3f pt %8.3f pt %8.3f pt>\n",
            vp.x*side + MIF_MARGIN, (1.0 - vp.y)*side + MIF_MARGIN,
            72.0/page_dpi, 72.0/page_dpi);
    fprintf(prstream, "  > # end of Rectangle\n");

}

void mif_drawpolyline(VPoint *vps, int n, int mode)
{
    int i;
    double side;

    side = *((double *) get_curdevice_data());
    
    if (mode == POLYLINE_CLOSED) {
        fprintf(prstream, "  <Polygon\n");
    } else {
        fprintf(prstream, "  <PolyLine\n");
    }
    mif_object_props(TRUE, FALSE);
    for (i = 0; i < n; i++) {
        fprintf(prstream, "   <Point %8.3f pt %8.3f>\n",
                vps[i].x*side + MIF_MARGIN, (1.0 - vps[i].y)*side + MIF_MARGIN);
    }
    if (mode == POLYLINE_CLOSED) {
        fprintf(prstream, "  > # end of Polygon\n");
    } else {
        switch (getlinecap()) {
        case LINECAP_BUTT :
          fprintf(prstream, "   <HeadCap Butt>\n");
          fprintf(prstream, "   <TailCap Butt>\n");
          break;
        case LINECAP_ROUND :
          fprintf(prstream, "   <HeadCap Round>\n");
          fprintf(prstream, "   <TailCap Round>\n");
          break;
        case LINECAP_PROJ :
          fprintf(prstream, "   <HeadCap Square>\n");
          fprintf(prstream, "   <TailCap Square>\n");
          break;
        default :
          fprintf(prstream, "   <HeadCap Butt>\n");
          fprintf(prstream, "   <TailCap Butt>\n");
          break;
        }
        fprintf(prstream, "  > # end of PolyLine\n");
    }
}

void mif_fillpolygon(VPoint *vps, int nc)
{
    int i;
    double side;

    side = *((double *) get_curdevice_data());
    
    fprintf(prstream, "  <Polygon\n");
    mif_object_props(FALSE, TRUE);
    for (i = 0; i < nc; i++) {
        fprintf(prstream, "   <Point %8.3f pt %8.3f>\n",
                vps[i].x*side + MIF_MARGIN, (1.0 - vps[i].y)*side + MIF_MARGIN);
    }
    fprintf(prstream, "  > # end of Polygon\n");
}

static void mif_arc(int draw, int fill, VPoint vp1, VPoint vp2, int a1, int a2)
{

    double side;

    side = *((double *) get_curdevice_data());

    fprintf(prstream, "  <Arc\n");
    mif_object_props(draw, fill);
    fprintf(prstream, "   <ArcRect %8.3f pt %8.3f pt %8.3f pt %8.3f pt>\n",
            MIN2(vp1.x, vp2.x)*side + MIF_MARGIN,
            (1.0 - MAX2(vp1.y, vp2.y))*side + MIF_MARGIN,
            fabs(vp2.x - vp1.x)*side, fabs(vp2.y - vp1.y)*side);
    fprintf(prstream, "   <ArcTheta %d>\n",
            (a2 > 90) ? (450 - a2) : (90 - a2));
    fprintf(prstream, "   <ArcDTheta %d>\n", a2 - a1);
    switch (getlinecap()) {
    case LINECAP_BUTT :
        fprintf(prstream, "   <HeadCap Butt>\n");
        fprintf(prstream, "   <TailCap Butt>\n");
        break;
    case LINECAP_ROUND :
        fprintf(prstream, "   <HeadCap Round>\n");
        fprintf(prstream, "   <TailCap Round>\n");
        break;
    case LINECAP_PROJ :
        fprintf(prstream, "   <HeadCap Square>\n");
        fprintf(prstream, "   <TailCap Square>\n");
        break;
    default :
        fprintf(prstream, "   <HeadCap Butt>\n");
        fprintf(prstream, "   <TailCap Butt>\n");
        break;
    }
    fprintf(prstream, "  > # end of Arc\n");
}

void mif_drawarc(VPoint vp1, VPoint vp2, int a1, int a2)
{
    mif_arc(TRUE, FALSE, vp1, vp2, a1, a2);
}

void mif_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode)
{
    int old_color;
    double rx, ry;
    VPoint vp[3];

    mif_arc(FALSE, TRUE, vp1, vp2, a1, a2);

    if (mode == ARCFILL_CHORD) {

        /* compute the associated triangle */
        rx      = fabs(vp2.x - vp1.x)/2;
        ry      = fabs(vp2.y - vp1.y)/2;
        vp[0].x = (vp1.x + vp2.x)/2;
        vp[0].y = (vp1.y + vp2.y)/2;
        vp[1].x = vp[0].x + rx * cos(a1*M_PI/180.0);
        vp[1].y = vp[0].y + ry * sin(a1*M_PI/180.0);
        vp[2].x = vp[0].x + rx * cos(a2*M_PI/180.0);
        vp[2].y = vp[0].y + ry * sin(a2*M_PI/180.0);

        if (a2 - a1 > 180) {
            /* the chord is larger than the default pieslice */

            if (a2 - a1 < 360) {
                /* the triangle is not degenerated, we need to fill it */
                mif_fillpolygon(vp, 3);
            }

        } else {
            /* the chord is smaller than the default pieslice */

            /* this is a terrible hack ! MIF does not support filling only
               the chord of an arc so we overwrite with the background
               color, thus erasing underlying objects ... */
            old_color = getcolor();
            setcolor(getbgcolor());
            mif_fillpolygon(vp, 3);
            setcolor(old_color);

        }

    }
}

/*
 * the following function does not work yet :-(
 */
void mif_putpixmap(VPoint vp, int width, int height, char *databits, 
                   int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    int i, j, k, paddedW;
    double side;
    fRGB *frgb;
    unsigned char tmpbyte;

    if (pixmap_bpp != 1 && pixmap_bpp != 8) {
        /* MIF supports only black and white or 256 colors images */
        return;
    }

    side = *((double *) get_curdevice_data());

    fprintf(prstream,
            "  <ImportObject\n");
    mif_object_props(FALSE, FALSE);
    fprintf(prstream,
            "   <ShapeRect %8.3f pt %8.3f pt %8.3f pt %8.3f pt>\n",
            vp.x*side + MIF_MARGIN, (1.0 - vp.y)*side + MIF_MARGIN,
            72.0*width/page_dpi, 72.0*height/page_dpi);
    fprintf(prstream, "   <ImportObFixedSize Yes>\n");
    fprintf(prstream, "=FrameImage\n");
    fprintf(prstream, "&%%v\n");
    fprintf(prstream, "&\\x\n");

    /* image header */
    fprintf(prstream, "&59a66a95\n");
    fprintf(prstream, "&%.8x\n", (unsigned int) width);
    fprintf(prstream, "&%.8x\n", (unsigned int) height);
    fprintf(prstream, "&%.8x\n", (unsigned int) pixmap_bpp);
    fprintf(prstream, "&00000000\n");
    fprintf(prstream, "&00000001\n");
    if (pixmap_bpp == 1) {
        fprintf(prstream, "&00000000\n");
        fprintf(prstream, "&00000000\n");

        /* image data */
        paddedW = PAD(width, bitmap_pad);

        for (k = 0; k < height; k++) {
            fprintf(prstream, "&");
            for (j = 0; j < paddedW/bitmap_pad; j++) {
                tmpbyte = reversebits((unsigned char) (databits)[k*paddedW/bitmap_pad + j]);
                fprintf(prstream, "%.2x", tmpbyte);
            }
            fprintf(prstream, "\n");
        }
    } else {
        fprintf(prstream, "&00000001\n");
        fprintf(prstream, "&00000300\n");

        /* colormap */
        for (i = 0; i < 256; i++) {
            /* red intensities */
            frgb = get_frgb(i);
            fprintf(prstream, "&%.2x\n",
                    (frgb != NULL) ? ((unsigned int) frgb->red) : 0);
        }
        for (i = 0; i < 256; i++) {
            /* green intensities */
            frgb = get_frgb(i);
            fprintf(prstream, "&%.2x\n",
                    (frgb != NULL) ? ((unsigned int) frgb->green) : 0);
        }
        for (i = 0; i < 256; i++) {
            /* blue intensities */
            frgb = get_frgb(i);
            fprintf(prstream, "&%.2x\n",
                    (frgb != NULL) ? ((unsigned int) frgb->blue) : 0);
        }

        /* image data */
        for (k = 0; k < height; k++) {
            fprintf(prstream, "&");
            for (j = 0; j < width; j++) {
                fprintf(prstream, "%.2x",
                        (unsigned int) (databits)[k*width+j]);
            }
            fprintf(prstream, "\n");
        }

    }

    fprintf(prstream, "&\\x\n");
    fprintf(prstream, "=EndInset\n");
    fprintf(prstream, "   <GroupID 1>\n");
    fprintf(prstream, "  > # end of ImportObject\n");
}

void mif_puttext(VPoint vp, char *s, int len, int font,
     TextMatrix *tm, int underline, int overline, int kerning)
{
    char *fontalias, *dash, *family, *fontfullname;
    double angle, side, size;
    Pen pen;

    side = *((double *) get_curdevice_data());
    pen = getpen();

    fprintf(prstream, "  <TextLine\n");
    mif_object_props(FALSE, FALSE);
    angle = atan2(tm->cyx, tm->cyy)*180.0/M_PI;
    if (angle < 0.0) {
        angle += 360.0;
    }
    fprintf(prstream, "   <Angle %f>\n", angle);
    fprintf(prstream, "   <TLOrigin %9.3f pt %9.3f pt>\n",
            side*vp.x + MIF_MARGIN, side*(1.0 - vp.y) + MIF_MARGIN);
    fprintf(prstream, "   <Font\n");
    fprintf(prstream, "    <FTag `'>\n");

    fontalias = get_fontalias(font);
    fontfullname = get_fontfullname(font);

    family  = NULL;
    if ((dash = strchr(fontalias, '-')) == NULL) {
        family = copy_string(family, fontalias);
    } else {
        family    = xrealloc(family, dash - fontalias + 1);
        strncpy(family, fontalias, dash - fontalias);
        family[dash - fontalias] = '\0';
    }

    fprintf(prstream, "    <FFamily `%s'>\n", family);
    copy_string(family, NULL);
    
    fprintf(prstream, "    <FWeight `%s'>\n", get_fontweight(font));

    if (strstr(fontfullname, "UltraCompressed") != NULL) {
        fprintf(prstream, "    <FVar `UltraCompressed'>\n");
    } else if (strstr(fontfullname, "ExtraCompressed") != NULL) {
        fprintf(prstream, "    <FVar `ExtraCompressed'>\n");
    } else if (strstr(fontfullname, "Compressed") != NULL) {
        fprintf(prstream, "    <FVar `Compressed'>\n");
    } else if (strstr(fontfullname, "UltraCondensed") != NULL) {
        fprintf(prstream, "    <FVar `UltraCondensed'>\n");
    } else if (strstr(fontfullname, "ExtraCondensed") != NULL) {
        fprintf(prstream, "    <FVar `ExtraCondensed'>\n");
    } else if (strstr(fontfullname, "Condensed") != NULL) {
        fprintf(prstream, "    <FVar `Condensed'>\n");
    } else if (strstr(fontfullname, "Narrow") != NULL) {
        fprintf(prstream, "    <FVar `Narrow'>\n");
    } else if (strstr(fontfullname, "Wide") != NULL) {
        fprintf(prstream, "    <FVar `Wide'>\n");
    } else if (strstr(fontfullname, "Poster") != NULL) {
        fprintf(prstream, "    <FVar `Poster'>\n");
    } else if (strstr(fontfullname, "Expanded") != NULL) {
        fprintf(prstream, "    <FVar `Expanded'>\n");
    } else if (strstr(fontfullname, "ExtraExtended") != NULL) {
        fprintf(prstream, "    <FVar `ExtraExtended'>\n");
    } else if (strstr(fontfullname, "Extended") != NULL) {
        fprintf(prstream, "    <FVar `Extended'>\n");
    }

    if (get_italic_angle(font) != 0) {
        if (strstr(fontfullname, "Italic") != NULL) {
            fprintf(prstream, "    <FAngle `Italic'>\n");
        } else if (strstr(fontfullname, "Obliqued") != NULL) {
            fprintf(prstream, "    <FAngle `Obliqued'>\n");
        } else if (strstr(fontfullname, "Oblique") != NULL) {
            fprintf(prstream, "    <FAngle `Oblique'>\n");
        } else if (strstr(fontfullname, "Upright") != NULL) {
            fprintf(prstream, "    <FAngle `Upright'>\n");
        } else if (strstr(fontfullname, "Kursiv") != NULL) {
            fprintf(prstream, "    <FAngle `Kursiv'>\n");
        } else if (strstr(fontfullname, "Cursive") != NULL) {
            fprintf(prstream, "    <FAngle `Cursive'>\n");
        } else if (strstr(fontfullname, "Slanted") != NULL) {
            fprintf(prstream, "    <FAngle `Slanted'>\n");
        } else if (strstr(fontfullname, "Inclined") != NULL) {
            fprintf(prstream, "    <FAngle `Inclined'>\n");
        } else {
            fprintf(prstream, "    <FAngle `Italic'>\n");
        }
    }

    size = fabs((tm->cxx*tm->cyy - tm->cxy*tm->cyx)
             / sqrt(tm->cxx*tm->cxx + tm->cyx*tm->cyx));
    fprintf(prstream, "    <FSize %9.3f pt>\n", side*size);

    fprintf(prstream, "    <FPostScriptName `%s'>\n", fontalias);

    fprintf(prstream, "    <FUnderlining %s>\n",
            (underline == TRUE) ? "FSingle" : "FNoUnderlining");
    fprintf(prstream, "    <FOverline %s>\n",
            (overline == TRUE) ? "Yes" : "No");
    fprintf(prstream, "    <FPairKern %s>\n",
            (kerning == TRUE) ? "Yes" : "No");
    fprintf(prstream, "    <FColor `%s'>\n", get_colorname(pen.color));
    fprintf(prstream, "   > # end of Font\n");
    fprintf(prstream, "   <String `%s'>\n",
            escape_specials((unsigned char *) s, len));
    fprintf(prstream, "  > # end of TextLine\n");
}

void mif_leavegraphics(void)
{
    fprintf(prstream, " <Group\n");
    fprintf(prstream, "  <ID 1>\n");
    fprintf(prstream, " > # end of Group\n");

    fprintf(prstream, " > # end of Frame\n");
    fprintf(prstream, "> # end of AFrames\n");
    fprintf(prstream, "<TextFlow\n");
    fprintf(prstream, " <TFTag `A'>\n");
    fprintf(prstream, " <TFAutoConnect Yes>\n");
    fprintf(prstream, " <Para\n");
    fprintf(prstream, "  <ParaLine\n");
    fprintf(prstream, "   <TextRectID 10>\n");
    fprintf(prstream, "  > # end of ParaLine\n");
    fprintf(prstream, " > # end of Para\n");
    fprintf(prstream, "> # end of TextFlow\n");
    fprintf(prstream, "<TextFlow\n");
    fprintf(prstream, " <TFTag `A'>\n");
    fprintf(prstream, " <TFAutoConnect Yes>\n");
    fprintf(prstream, " <Para\n");
    fprintf(prstream, " <TextRectID 20>\n");
    fprintf(prstream, "   <ParaLine\n");
    fprintf(prstream, "     <AFrame 30>\n");
    fprintf(prstream, "  > # end of ParaLine\n");
    fprintf(prstream, " > # end of Para\n");
    fprintf(prstream, "> # end of TextFlow\n");
          
    fprintf(prstream, "# End of MIFFile\n");
}
