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

/*
 * Dummy (Null) driver for Grace
 */

#include <stdio.h>
#include <stdlib.h>
 
#include "defines.h"
#include "draw.h"
#include "device.h"
#include "devlist.h"
#include "dummydrv.h"

static Device_entry dev_dummy = {DEVICE_TERM,
          "Dummy",
          dummyinitgraphics,
          NULL,
          NULL,
          "",
          TRUE,
          FALSE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };

int register_dummy_drv(void)
{
    return register_device(dev_dummy);
}

int dummyinitgraphics(void)
{
    /* device-dependent routines */
    devupdatecmap   = NULL;
    
    devdrawpixel    = dummy_drawpixel;
    devdrawpolyline = dummy_drawpolyline;
    devfillpolygon  = dummy_fillpolygon;
    devdrawarc      = dummy_drawarc;
    devfillarc      = dummy_fillarc;
    devputpixmap    = dummy_putpixmap;
    devputtext      = dummy_puttext;
    
    devleavegraphics = dummy_leavegraphics;

    return RETURN_SUCCESS;
}

void dummy_drawpixel(VPoint vp){}
void dummy_drawpolyline(VPoint *vps, int n, int mode){}
void dummy_fillpolygon(VPoint *vps, int nc){}
void dummy_drawarc(VPoint vp1, VPoint vp2, int a1, int a2){}
void dummy_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode){}
void dummy_putpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type){}
void dummy_puttext(VPoint vp, char *s, int len, int font,
     TextMatrix *tm, int underline, int overline, int kerning){}

void dummy_leavegraphics(void){}
