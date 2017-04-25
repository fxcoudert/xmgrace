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
 *
 *  list of device init functions
 *
 */

#ifndef __DEVLIST_H_
#define __DEVLIST_H_

#include <config.h>

int register_dummy_drv(void);
#ifndef NONE_GUI
int register_x11_drv(void);
#endif
int register_ps_drv(void);
int register_eps_drv(void);
int register_mf_drv(void);
int register_mif_drv(void);
int register_svg_drv(void);
#ifdef HAVE_LIBPDF
int register_pdf_drv(void);
#endif
int register_pnm_drv(void);
#ifdef HAVE_LIBJPEG
int register_jpg_drv(void);
#endif
#ifdef HAVE_LIBPNG
int register_png_drv(void);
#endif

#endif /* __DEVLIST_H_ */
