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
 *
 * Plot properties
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "protos.h"
#include "motifinc.h"

static Widget plot_frame;

/*
 * Panel item declarations
 */
static OptionStructure *bg_color_item;
static Widget bg_fill_item;

static Widget timestamp_active_item;
static OptionStructure *timestamp_font_item;
static Widget timestamp_size_item;
static Widget timestamp_rotate_item;
static OptionStructure *timestamp_color_item;
Widget timestamp_x_item;
Widget timestamp_y_item;

static int plot_define_notify_proc(void *data);
static void update_plot_items(void);

void create_plot_frame_cb(void *data)
{
    create_plot_frame();
}

void create_plot_frame(void)
{
    set_wait_cursor();
    
    if (plot_frame == NULL) {
        Widget panel, fr, rc;
    
	plot_frame = CreateDialogForm(app_shell, "Plot appearance");

	panel = CreateVContainer(plot_frame);

	fr = CreateFrame(panel, "Page background");
        rc = CreateHContainer(fr);
        bg_color_item = CreateColorChoice(rc, "Color:");
	bg_fill_item = CreateToggleButton(rc, "Fill");

	fr = CreateFrame(panel, "Time stamp");
        rc = CreateVContainer(fr);

	timestamp_active_item = CreateToggleButton(rc, "Enable");
	timestamp_font_item = CreateFontChoice(rc, "Font:");
	timestamp_color_item = CreateColorChoice(rc, "Color:");
	timestamp_size_item = CreateCharSizeChoice(rc, "Character size");
	timestamp_rotate_item = CreateAngleChoice(rc, "Angle");
	timestamp_x_item = CreateTextItem2(rc, 10, "Timestamp X:");
	timestamp_y_item = CreateTextItem2(rc, 10, "Timestamp Y:");

	CreateAACDialog(plot_frame, panel, plot_define_notify_proc, NULL);
    }
    update_plot_items();
    
    RaiseWindow(GetParent(plot_frame));
    unset_wait_cursor();
}

static void update_plot_items(void)
{
    char buf[32];

    if (plot_frame) {
	SetOptionChoice(bg_color_item, getbgcolor());
	SetToggleButtonState(bg_fill_item, getbgfill());

	SetToggleButtonState(timestamp_active_item, timestamp.active);
	SetOptionChoice(timestamp_font_item, timestamp.font);
	SetOptionChoice(timestamp_color_item, timestamp.color);

	SetCharSizeChoice(timestamp_size_item, timestamp.charsize);

	SetAngleChoice(timestamp_rotate_item, timestamp.rot);

	sprintf(buf, "%g", timestamp.x);
	xv_setstr(timestamp_x_item, buf);
	sprintf(buf, "%g", timestamp.y);
	xv_setstr(timestamp_y_item, buf);
    }
}

static int plot_define_notify_proc(void *data)
{
    setbgcolor(GetOptionChoice(bg_color_item));
    setbgfill(GetToggleButtonState(bg_fill_item));

    timestamp.active = GetToggleButtonState(timestamp_active_item);
    timestamp.font = GetOptionChoice(timestamp_font_item);
    timestamp.color = GetOptionChoice(timestamp_color_item);
    
    timestamp.charsize = GetCharSizeChoice(timestamp_size_item);
    
    timestamp.rot = GetAngleChoice(timestamp_rotate_item);
    
    xv_evalexpr(timestamp_x_item, &timestamp.x);
    xv_evalexpr(timestamp_y_item, &timestamp.y);
    set_dirtystate();
    xdrawgraph();
    
    return RETURN_SUCCESS;
}

