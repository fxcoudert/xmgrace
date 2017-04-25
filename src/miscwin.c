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
 * Misc properties
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "graphs.h"
#include "motifinc.h"
#include "protos.h"

extern int cursortype;

#if defined WITH_XMHTML || defined WITH_LIBHELP
extern int force_external_viewer;
#endif

static Widget props_frame;

/*
 * Panel item declarations
 */
#ifdef DEBUG
static SpinStructure *debug_item;
#endif
static Widget noask_item;
static Widget dc_item;

static Widget *graph_focus_choice_item;
static Widget graph_drawfocus_choice_item;

static Widget autoredraw_type_item;
static Widget cursor_type_item;
static SpinStructure *max_path_item;
static Widget safe_mode_item;
static Widget scrollper_item;
static Widget shexper_item;
static Widget linkscroll_item;

static Widget *hint_item;
static Widget date_item;
static Widget wrap_year_item;
static Widget two_digits_years_item;
#if defined WITH_XMHTML || defined WITH_LIBHELP
static Widget force_external_viewer_item;
#endif

/*
 * Event and Notify proc declarations
 */
static int props_define_notify_proc(void *data);

static void wrap_year_cb(int onoff, void *data)
{
    Widget wrap_year = (Widget) data;
    
    SetSensitive(wrap_year, onoff);
}

void create_props_frame(void *data)
{
    set_wait_cursor();

    if (props_frame == NULL) {
        Widget fr, rc, rc1;

	props_frame = CreateDialogForm(app_shell, "Preferences");

	fr = CreateFrame(props_frame, "Responsiveness");
        AddDialogFormChild(props_frame, fr);
        rc1 = CreateVContainer(fr);

#ifdef DEBUG
	debug_item = CreateSpinChoice(rc1,
            "Debug level:", 1, SPIN_TYPE_INT, 0.0, 8.0, 1.0);
#endif
	noask_item = CreateToggleButton(rc1, "Don't ask questions");
	dc_item = CreateToggleButton(rc1, "Allow double clicks on canvas");

	graph_focus_choice_item = CreatePanelChoice(rc1,
            "Graph focus switch",
	    4,
	    "Button press",
	    "As set",
	    "Follows mouse",
	    NULL);

        graph_drawfocus_choice_item =
            CreateToggleButton(rc1, "Display focus markers");
	autoredraw_type_item = CreateToggleButton(rc1, "Auto redraw");
	cursor_type_item = CreateToggleButton(rc1, "Crosshair cursor");
#if defined WITH_XMHTML || defined WITH_LIBHELP
	force_external_viewer_item = CreateToggleButton(rc1,
            "Use external help viewer for local documents");
#endif        
	fr = CreateFrame(props_frame, "Restrictions");
        AddDialogFormChild(props_frame, fr);
        rc1 = CreateVContainer(fr);
	max_path_item = CreateSpinChoice(rc1,
            "Max drawing path length:", 6, SPIN_TYPE_INT, 0.0, 1.0e6, 1000);
	safe_mode_item = CreateToggleButton(rc1, "Run in safe mode");
        
	fr = CreateFrame(props_frame, "Scroll/zoom");
        AddDialogFormChild(props_frame, fr);
        rc1 = CreateVContainer(fr);
	scrollper_item = CreateScale(rc1, "Scroll %", 0, 200, 20);
	shexper_item   = CreateScale(rc1, "Zoom %",   0, 200, 20);
	linkscroll_item = CreateToggleButton(rc1, "Linked scrolling");

	fr = CreateFrame(props_frame, "Dates");
        rc1 = CreateVContainer(fr);
        hint_item = CreatePanelChoice(rc1, "Date hint",
                                      5,
                                      "ISO",
                                      "European",
                                      "US",
                                      "None",
                                      NULL);
	date_item = CreateTextItem2(rc1, 20, "Reference date:");
	rc = CreateHContainer(rc1);
        two_digits_years_item = CreateToggleButton(rc, "Two-digit year span");
        wrap_year_item = CreateTextItem2(rc, 4, "Wrap year:");
	AddToggleButtonCB(two_digits_years_item,
            wrap_year_cb, (void *) wrap_year_item);

	CreateAACDialog(props_frame, fr, props_define_notify_proc, NULL);
    }
    
    update_props_items();
    
    RaiseWindow(GetParent(props_frame));
    unset_wait_cursor();
}

void update_props_items(void)
{
    int itest = 0;
    int iv;
    int y, m, d, h, mm, sec;
    char date_string[64], wrap_year_string[64];
    
    if (props_frame) {
#ifdef DEBUG
	if (get_debuglevel() > 8) {
	    errwin("Debug level > 8, resetting to 0");
	    set_debuglevel(0);
	}
	SetSpinChoice(debug_item, (double) get_debuglevel());
#endif
	SetToggleButtonState(noask_item, noask);
	SetToggleButtonState(dc_item, allow_dc);

	if (focus_policy == FOCUS_SET) {
	    itest = 1;
	} else if (focus_policy == FOCUS_CLICK) {
	    itest = 0;
	} else if (focus_policy == FOCUS_FOLLOWS) {
	    itest = 2;
	}
	SetChoice(graph_focus_choice_item, itest);
	SetToggleButtonState(graph_drawfocus_choice_item, draw_focus_flag);

	SetToggleButtonState(linkscroll_item, scrolling_islinked);
	SetToggleButtonState(autoredraw_type_item, auto_redraw);
	SetToggleButtonState(cursor_type_item, cursortype);
#if defined WITH_XMHTML || defined WITH_LIBHELP
	SetToggleButtonState(force_external_viewer_item, force_external_viewer);
#endif
	SetSpinChoice(max_path_item, (double) get_max_path_limit());
	SetToggleButtonState(safe_mode_item, safe_mode);
	iv = (int) rint(100*scrollper);
	SetScaleValue(scrollper_item, iv);
	iv = (int) rint(100*shexper);
	SetScaleValue(shexper_item, iv);
        switch (get_date_hint()) {
        case FMT_iso :
            itest = 0;
            break;
        case FMT_european :
            itest = 1;
            break;
        case FMT_us :
            itest = 2;
            break;
        default :
            itest = FMT_nohint;
            break;
        }
    	SetChoice(hint_item, itest);
	jul_to_cal_and_time(0.0, ROUND_SECOND, &y, &m, &d, &h, &mm, &sec);
	sprintf(date_string, "%d-%02d-%02d %02d:%02d:%02d",
                y, m, d, h, mm, sec);
        xv_setstr(date_item, date_string);
        SetToggleButtonState(two_digits_years_item, two_digits_years_allowed());
        sprintf(wrap_year_string, "%04d", get_wrap_year());
        xv_setstr(wrap_year_item, wrap_year_string);
        SetSensitive(wrap_year_item, two_digits_years_allowed() ? True:False);
    }
}

static int props_define_notify_proc(void *data)
{
    double jul;
    
#ifdef DEBUG
    set_debuglevel((int) GetSpinChoice(debug_item));
#endif
    noask = GetToggleButtonState(noask_item);
    allow_dc = GetToggleButtonState(dc_item);

    switch (GetChoice(graph_focus_choice_item)) {
    case 0:
	focus_policy = FOCUS_CLICK;
	break;
    case 1:
	focus_policy = FOCUS_SET;
	break;
    case 2:
	focus_policy = FOCUS_FOLLOWS;
	break;
    }
    draw_focus_flag = GetToggleButtonState(graph_drawfocus_choice_item);

    scrolling_islinked = GetToggleButtonState(linkscroll_item);
    auto_redraw = GetToggleButtonState(autoredraw_type_item);
    cursortype = GetToggleButtonState(cursor_type_item);
#if defined WITH_XMHTML || defined WITH_LIBHELP
    force_external_viewer = GetToggleButtonState(force_external_viewer_item);
#endif
    set_max_path_limit((int) GetSpinChoice(max_path_item));
    safe_mode = GetToggleButtonState(safe_mode_item);
    scrollper = (double) GetScaleValue(scrollper_item)/100.0;
    shexper   = (double) GetScaleValue(shexper_item)/100.0;

    switch (GetChoice(hint_item)) {
    case 0 :
        set_date_hint(FMT_iso);
        break;
    case 1 :
        set_date_hint(FMT_european);
        break;
    case 2 :
        set_date_hint(FMT_us);
        break;
    default :
        set_date_hint(FMT_nohint);
        break;
    }
    if (parse_date_or_number(xv_getstr(date_item), TRUE, &jul)
        == RETURN_SUCCESS) {
        set_ref_date(jul);
    } else {
        errmsg("Invalid date");
    }
    allow_two_digits_years(GetToggleButtonState(two_digits_years_item));
    set_wrap_year(atoi(xv_getstr(wrap_year_item)));
    
    xdrawgraph();
    
    return RETURN_SUCCESS;
}
