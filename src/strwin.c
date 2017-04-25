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
 * strings, lines, boxes and elipses
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/RowColumn.h>

#include "globals.h"
#include "utils.h"
#include "graphs.h"
#include "events.h"
#include "protos.h"
#include "motifinc.h"

static char buf[256];

static Widget objects_frame;
static Widget strings_frame;
static Widget lines_frame;
static Widget boxes_frame;
static Widget ellip_frame;

static OptionStructure *strings_font_item;
static Widget strings_rot_item;
static Widget strings_size_item;
static Widget *strings_loc_item;
static OptionStructure *strings_color_item;
static OptionStructure *strings_just_item;

static Widget *lines_arrow_item;
static SpinStructure *lines_asize_item;
static Widget *lines_atype_item;
static SpinStructure *lines_a_dL_ff_item;
static SpinStructure *lines_a_lL_ff_item;
static OptionStructure *lines_color_item;
static OptionStructure *lines_style_item;
static SpinStructure *lines_width_item;
static Widget *lines_loc_item;

static OptionStructure *boxes_color_item;
static OptionStructure *boxes_lines_item;
static SpinStructure *boxes_linew_item;
static OptionStructure *boxes_fillpat_item;
static OptionStructure *boxes_fillcol_item;
static Widget *boxes_loc_item;

static OptionStructure *ellip_color_item;
static OptionStructure *ellip_lines_item;
static SpinStructure *ellip_linew_item;
static OptionStructure *ellip_fillpat_item;
static OptionStructure *ellip_fillcol_item;
static Widget *ellip_loc_item;

static void define_ellip_popup(void *data);
static void define_strings_popup(void *data);
static void define_lines_popup(void *data);
static void define_boxes_popup(void *data);

void ellip_def_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    ellipse_color = GetOptionChoice(ellip_color_item);
    ellipse_loctype = GetChoice(ellip_loc_item) ? COORD_VIEW : COORD_WORLD;
    ellipse_lines = GetOptionChoice(ellip_lines_item);
    ellipse_linew = GetSpinChoice(ellip_linew_item);
    ellipse_fillcolor = GetOptionChoice(ellip_fillcol_item);
    ellipse_fillpat = GetOptionChoice(ellip_fillpat_item);
}

void update_ellip(void)
{
    if (ellip_frame) {
	SetOptionChoice(ellip_color_item, ellipse_color);
	SetOptionChoice(ellip_lines_item, ellipse_lines);
	SetSpinChoice(ellip_linew_item, ellipse_linew);
	SetOptionChoice(ellip_fillpat_item, ellipse_fillpat);
	SetOptionChoice(ellip_fillcol_item, ellipse_fillcolor);
	SetChoice(ellip_loc_item, ellipse_loctype == COORD_VIEW ? 1 : 0);
    }
}

void boxes_def_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    box_color = GetOptionChoice(boxes_color_item);
    box_loctype = GetChoice(boxes_loc_item) ? COORD_VIEW : COORD_WORLD;
    box_lines = GetOptionChoice(boxes_lines_item);
    box_linew = GetSpinChoice(boxes_linew_item);
    box_fillcolor = GetOptionChoice(boxes_fillcol_item);
    box_fillpat = GetOptionChoice(boxes_fillpat_item);
}

void lines_def_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    line_asize = GetSpinChoice(lines_asize_item);
    line_color = GetOptionChoice(lines_color_item);
    line_arrow_end = GetChoice(lines_arrow_item);
    line_atype = GetChoice(lines_atype_item);
    line_a_dL_ff = GetSpinChoice(lines_a_dL_ff_item);
    line_a_lL_ff = GetSpinChoice(lines_a_lL_ff_item);
    line_lines = GetOptionChoice(lines_style_item);
    line_linew = GetSpinChoice(lines_width_item);
    line_loctype = GetChoice(lines_loc_item) ? COORD_VIEW : COORD_WORLD;
}

void updatestrings(void)
{
    if (strings_frame) {
	SetOptionChoice(strings_font_item, string_font);
	SetOptionChoice(strings_color_item, string_color);
	SetCharSizeChoice(strings_size_item, string_size);
	SetAngleChoice(strings_rot_item, string_rot);
	SetChoice(strings_loc_item, string_loctype == COORD_VIEW ? 1 : 0);
	SetOptionChoice(strings_just_item, string_just);
    }
}

void update_lines(void)
{
    if (lines_frame) {
	SetOptionChoice(lines_color_item, line_color);
	SetOptionChoice(lines_style_item, line_lines);
	SetSpinChoice(lines_width_item, line_linew);
	SetChoice(lines_arrow_item, line_arrow_end);
	SetChoice(lines_atype_item, line_atype);
	SetSpinChoice(lines_asize_item, line_asize);
        SetSpinChoice(lines_a_dL_ff_item, line_a_dL_ff);
        SetSpinChoice(lines_a_lL_ff_item, line_a_lL_ff);
	SetChoice(lines_loc_item, line_loctype == COORD_VIEW ? 1 : 0);
    }
}

void update_boxes(void)
{
    if (boxes_frame) {
	SetOptionChoice(boxes_color_item, box_color);
	SetOptionChoice(boxes_lines_item, box_lines);
	SetSpinChoice(boxes_linew_item, box_linew);
	SetOptionChoice(boxes_fillpat_item, box_fillpat);
	SetOptionChoice(boxes_fillcol_item, box_fillcolor);
	SetChoice(boxes_loc_item, box_loctype == COORD_VIEW ? 1 : 0);
    }
}

void define_string_defaults(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (strings_frame) {
	string_font = GetOptionChoice(strings_font_item);
	string_color = GetOptionChoice(strings_color_item);
	string_size = GetCharSizeChoice(strings_size_item);
	string_rot = GetAngleChoice(strings_rot_item);
	string_loctype = GetChoice(strings_loc_item) ? COORD_VIEW : COORD_WORLD;
	string_just = GetOptionChoice(strings_just_item);
    }
}

static void close_objects_cb(void *data)
{
    set_action(DO_NOTHING);
    destroy_dialog_cb((Widget) data);
}

static void clear_objects_cb(void *data)
{
    int type = (int) data;
    
    switch (type) {
    case OBJECT_LINE:
        if (yesno("Delete all lines?", NULL, NULL, NULL)) {
            do_clear_lines();
            xdrawgraph();
        }
        break;
    case OBJECT_BOX:
        if (yesno("Delete all boxes?", NULL, NULL, NULL)) {
            do_clear_boxes();
            xdrawgraph();
        }
        break;
    case OBJECT_ELLIPSE:
        if (yesno("Delete all ellipses?", NULL, NULL, NULL)) {
            do_clear_ellipses();
            xdrawgraph();
        }
        break;
    case OBJECT_STRING:
        if (yesno("Delete all text strings?", NULL, NULL, NULL)) {
            do_clear_text();
            xdrawgraph();
        }
        break;
    default:
        break;
    }
}

void define_objects_popup(void *data)
{
    Widget wbut;
    Widget panel, rc;
    set_wait_cursor();
    if (objects_frame == NULL) {
	objects_frame = XmCreateDialogShell(app_shell, "Objects", NULL, 0);
	handle_close(objects_frame);
	panel = XmCreateRowColumn(objects_frame, "ticks_rc", NULL, 0);
        XtVaSetValues(panel, XmNorientation, XmHORIZONTAL, NULL);

        rc = XmCreateRowColumn(panel, "rc", NULL, 0);
	wbut = CreateButton(rc, "Text");
        AddButtonCB(wbut, set_actioncb, (void *) STR_LOC);
	wbut = CreateButton(rc, "Text props...");
        AddButtonCB(wbut, define_strings_popup, NULL);
	wbut = CreateButton(rc, "Line");
        AddButtonCB(wbut, set_actioncb, (void *) MAKE_LINE_1ST);
	wbut = CreateButton(rc, "Line props...");
        AddButtonCB(wbut, define_lines_popup, NULL);
	wbut = CreateButton(rc, "Box");
        AddButtonCB(wbut, set_actioncb, (void *) MAKE_BOX_1ST);
	wbut = CreateButton(rc, "Box props...");
        AddButtonCB(wbut, define_boxes_popup, NULL);
	wbut = CreateButton(rc, "Ellipse");
        AddButtonCB(wbut, set_actioncb, (void *) MAKE_ELLIP_1ST);
	wbut = CreateButton(rc, "Ellipse props...");
        AddButtonCB(wbut, define_ellip_popup, NULL);
	ManageChild(rc);

        rc = XmCreateRowColumn(panel, "rc", NULL, 0);
	wbut = CreateButton(rc, "Edit object");
        AddButtonCB(wbut, set_actioncb, (void *) EDIT_OBJECT);
	wbut = CreateButton(rc, "Move object");
        AddButtonCB(wbut, set_actioncb, (void *) MOVE_OBJECT_1ST);
	wbut = CreateButton(rc, "Copy object");
        AddButtonCB(wbut, set_actioncb, (void *) COPY_OBJECT1ST);
	wbut = CreateButton(rc, "Delete object");
        AddButtonCB(wbut, set_actioncb, (void *) DEL_OBJECT);
	wbut = CreateButton(rc, "Clear all text");
        AddButtonCB(wbut, clear_objects_cb, (void *) OBJECT_STRING);
	wbut = CreateButton(rc, "Clear all lines");
        AddButtonCB(wbut, clear_objects_cb, (void *) OBJECT_LINE);
	wbut = CreateButton(rc, "Clear all boxes");
        AddButtonCB(wbut, clear_objects_cb, (void *) OBJECT_BOX);
	wbut = CreateButton(rc, "Clear all ellipses");
        AddButtonCB(wbut, clear_objects_cb, (void *) OBJECT_ELLIPSE);
	wbut = CreateButton(rc, "Close");
	AddButtonCB(wbut, close_objects_cb, objects_frame);
	ManageChild(rc);

	ManageChild(panel);
    }
    RaiseWindow(objects_frame);
    unset_wait_cursor();
}

static void define_ellip_popup(void *data)
{
    Widget rc;
    Widget buts[2];
    Widget panel;

    set_wait_cursor();
    if (ellip_frame == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	ellip_frame = XmCreateDialogShell(app_shell, "Ellipses", NULL, 0);
	handle_close(ellip_frame);
	panel = XmCreateRowColumn(ellip_frame, "ellip_rc", NULL, 0);

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	ellip_color_item = CreateColorChoice(rc, "Color: ");

	ellip_linew_item = CreateLineWidthChoice(rc, "Line width:");

	ellip_lines_item = CreateLineStyleChoice(rc, "Line style:");

	ellip_fillpat_item = CreatePatternChoice(rc, "Fill pattern:");
	ellip_fillcol_item = CreateColorChoice(rc, "Fill color: ");
	ellip_loc_item = CreatePanelChoice(rc, "Position in:",
					   3,
					   "World coordinates",
					   "Viewport coordinates",
					   NULL);
	ManageChild(rc);

	CreateSeparator(panel);

	CreateCommandButtons(panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		      ellip_def_proc, (XtPointer) 0);
	XtAddCallback(buts[1], XmNactivateCallback,
		  destroy_dialog, (XtPointer) ellip_frame);

	ManageChild(panel);
    }
    RaiseWindow(ellip_frame);
    update_ellip();
    unset_wait_cursor();
}

static void define_strings_popup(void *data)
{
    Widget rc;
    Widget buts[2];
    Widget panel;

    set_wait_cursor();
    if (strings_frame == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	strings_frame = XmCreateDialogShell(app_shell, "Strings", NULL, 0);
	handle_close(strings_frame);
	panel = XmCreateRowColumn(strings_frame, "strings_rc", NULL, 0);

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	strings_font_item = CreateFontChoice(rc, "Font:");

	strings_color_item = CreateColorChoice(rc, "Color: ");

	strings_just_item = CreateJustChoice(rc, "Justification:");

	strings_loc_item = CreatePanelChoice(rc, "Position in:",
					     3,
					     "World coordinates",
					     "Viewport coordinates",
					     NULL);
	ManageChild(rc);

	strings_rot_item = CreateAngleChoice(panel, "Rotation");

	strings_size_item = CreateCharSizeChoice(panel, "Size");

	CreateSeparator(panel);

	CreateCommandButtons(panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		    define_string_defaults, (XtPointer) 0);
	XtAddCallback(buts[1], XmNactivateCallback,
		destroy_dialog, (XtPointer) strings_frame);

	ManageChild(panel);
    }
    RaiseWindow(strings_frame);
    updatestrings();
    unset_wait_cursor();
}

static void define_lines_popup(void *data)
{
    Widget rc, fr, rc2;
    Widget buts[2];
    Widget panel;

    set_wait_cursor();
    if (lines_frame == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	lines_frame = XmCreateDialogShell(app_shell, "Lines", NULL, 0);
	handle_close(lines_frame);
	panel = XmCreateRowColumn(lines_frame, "lines_rc", NULL, 0);

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	lines_color_item = CreateColorChoice(rc, "Color: ");

	lines_width_item = CreateLineWidthChoice(rc, "Line width:");

	lines_style_item = CreateLineStyleChoice(rc, "Line style:");

	fr = CreateFrame(rc, "Arrow");
        rc2 = XtVaCreateWidget("rc2", xmRowColumnWidgetClass, fr, NULL);
        lines_arrow_item = CreatePanelChoice(rc2, "Place at:",
					     5,
					     "None",
					     "Start",
					     "End",
					     "Both ends",
					     NULL);
	lines_atype_item = CreatePanelChoice(rc2, "Type:",
					     4,
					     "Line",
					     "Filled",
					     "Opaque",
					     NULL);
	lines_asize_item = CreateSpinChoice(rc2, "Length",
            4, SPIN_TYPE_FLOAT, -10.0, 10.0, 0.5);
	lines_a_dL_ff_item = CreateSpinChoice(rc2, "d/L form factor",
            4, SPIN_TYPE_FLOAT, 0.0, 10.0, 0.1);
	lines_a_lL_ff_item = CreateSpinChoice(rc2, "l/L form factor",
            4, SPIN_TYPE_FLOAT, -1.0, 1.0, 0.1);
	ManageChild(rc2);

	lines_loc_item = CreatePanelChoice(rc, "Position in:",
					   3,
					   "World coordinates",
					   "Viewport coordinates",
					   NULL);
	ManageChild(rc);

	CreateSeparator(panel);

	CreateCommandButtons(panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		      lines_def_proc, (XtPointer) 0);
	XtAddCallback(buts[1], XmNactivateCallback,
		  destroy_dialog, (XtPointer) lines_frame);

	ManageChild(panel);
    }
    update_lines();
    RaiseWindow(lines_frame);
    unset_wait_cursor();
}

static void define_boxes_popup(void *data)
{
    Widget rc;
    Widget buts[2];
    Widget panel;

    set_wait_cursor();
    if (boxes_frame == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	boxes_frame = XmCreateDialogShell(app_shell, "Boxes", NULL, 0);
	handle_close(boxes_frame);
	panel = XmCreateRowColumn(boxes_frame, "boxes_rc", NULL, 0);

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	boxes_color_item = CreateColorChoice(rc, "Color: ");

	boxes_linew_item = CreateLineWidthChoice(rc, "Line width:");

	boxes_lines_item = CreateLineStyleChoice(rc, "Line style:");

	boxes_fillpat_item = CreatePatternChoice(rc, "Fill pattern:");
	boxes_fillcol_item = CreateColorChoice(rc, "Fill color: ");
	boxes_loc_item = CreatePanelChoice(rc, "Position in:",
					   3,
					   "World coordinates",
					   "Viewport coordinates",
					   NULL);
	ManageChild(rc);

	CreateSeparator(panel);

	CreateCommandButtons(panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		      boxes_def_proc, (XtPointer) 0);
	XtAddCallback(buts[1], XmNactivateCallback,
		  destroy_dialog, (XtPointer) boxes_frame);

	ManageChild(panel);
    }
    RaiseWindow(boxes_frame);
    update_boxes();
    unset_wait_cursor();
}

typedef struct {
    Widget top;
    OptionStructure *color_item;
    SpinStructure *linew_item;
    OptionStructure *lines_item;
    OptionStructure *fill_color_item;
    OptionStructure *fill_pattern_item;
    Widget *loc_item;
    Widget x1_item;
    Widget x2_item;
    Widget y1_item;
    Widget y2_item;
    int boxno;
} EditBoxUI;

static EditBoxUI box_ui,ellip_ui;

void update_box_edit(EditBoxUI *ui)
{
    if (ui->top) {
	int boxno = ui->boxno;
	SetOptionChoice(ui->color_item, boxes[boxno].color);
	SetOptionChoice(ui->lines_item, boxes[boxno].lines);
	SetSpinChoice(ui->linew_item, boxes[boxno].linew);
	SetOptionChoice(ui->fill_pattern_item, boxes[boxno].fillpattern);
	SetOptionChoice(ui->fill_color_item, boxes[boxno].fillcolor);
	SetChoice(ui->loc_item, boxes[boxno].loctype == COORD_VIEW ? 1 : 0);
	sprintf(buf, "%.12f", boxes[boxno].x1);
	xv_setstr(ui->x1_item, buf);
	sprintf(buf, "%.12f", boxes[boxno].x2);
	xv_setstr(ui->x2_item, buf);
	sprintf(buf, "%.12f", boxes[boxno].y1);
	xv_setstr(ui->y1_item, buf);
	sprintf(buf, "%.12f", boxes[boxno].y2);
	xv_setstr(ui->y2_item, buf);
    }
}

void update_ellipse_edit(EditBoxUI *ui)
{
    if (ui->top) {
	int ellipno = ui->boxno;
	SetOptionChoice(ui->color_item, ellip[ellipno].color);
	SetOptionChoice(ui->lines_item, ellip[ellipno].lines);
	SetSpinChoice(ui->linew_item, ellip[ellipno].linew);
	SetOptionChoice(ui->fill_pattern_item, ellip[ellipno].fillpattern);
	SetOptionChoice(ui->fill_color_item, ellip[ellipno].fillcolor);
	SetChoice(ui->loc_item, ellip[ellipno].loctype == COORD_VIEW ? 1 : 0);
	sprintf(buf, "%.12f", 0.5*(ellip[ellipno].x1+ellip[ellipno].x2));
	xv_setstr(ui->x1_item, buf);
	sprintf(buf, "%.12f", 0.5*(ellip[ellipno].y1+ellip[ellipno].y2));
	xv_setstr(ui->y1_item, buf);
	sprintf(buf, "%.12f", fabs(ellip[ellipno].x1-ellip[ellipno].x2) );
	xv_setstr(ui->x2_item, buf);
	sprintf(buf, "%.12f", fabs(ellip[ellipno].y1-ellip[ellipno].y2) );
	xv_setstr(ui->y2_item, buf);
    }
}


void swap_ellipwv_coords(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditBoxUI *ui = (EditBoxUI *)client_data;
    int ellipno = ui->boxno;
    double x1, x2, y1, y2;
	
    if(((int)GetChoice(ui->loc_item)?COORD_VIEW : COORD_WORLD) == ellip[ellipno].loctype) {
        return;
    }
               
    xv_evalexpr(ui->x1_item, &x1);
    xv_evalexpr(ui->x2_item, &x2);
    xv_evalexpr(ui->y1_item, &y1);
    xv_evalexpr(ui->y2_item, &y2);
	
    if( ellip[ellipno].loctype == COORD_VIEW ) {
    ellip[ellipno].gno = get_cg();
	ellip[ellipno].loctype = COORD_WORLD;
	view2world( x1-x2/2., y1-y2/2., &ellip[ellipno].x1,&ellip[ellipno].y1 );
	view2world( x1+x2/2., y1+y2/2., &ellip[ellipno].x2,&ellip[ellipno].y2 );
    } else {
	ellip[ellipno].loctype = COORD_VIEW;
	world2view( x1-x2/2., y1-y2/2., &ellip[ellipno].x1,&ellip[ellipno].y1 );
	world2view( x1+x2/2., y1+y2/2., &ellip[ellipno].x2,&ellip[ellipno].y2 );
    }
    update_ellipse_edit( ui );
}


void ellipse_edit_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditBoxUI *ui = (EditBoxUI *) client_data;
    int ellipno = ui->boxno;
    double a, b, c, d;

    if (xv_evalexpr( ui->x1_item, &a ) != RETURN_SUCCESS ||
        xv_evalexpr( ui->x2_item, &b ) != RETURN_SUCCESS ||
        xv_evalexpr( ui->y1_item, &c ) != RETURN_SUCCESS ||
        xv_evalexpr( ui->y2_item, &d ) != RETURN_SUCCESS ) {
        return;
    }
    ellip[ellipno].color = GetOptionChoice(ui->color_item);
    ellip[ellipno].loctype = GetChoice(ui->loc_item) ? COORD_VIEW : COORD_WORLD;
    ellip[ellipno].lines = GetOptionChoice(ui->lines_item);
    ellip[ellipno].linew = GetSpinChoice(ui->linew_item);
    ellip[ellipno].fillcolor = GetOptionChoice(ui->fill_color_item);
    ellip[ellipno].fillpattern = GetOptionChoice(ui->fill_pattern_item);
    ellip[ellipno].x1 = a - b/2.;
    ellip[ellipno].x2 = a + b/2.;
    ellip[ellipno].y1 = c - d/2.;
    ellip[ellipno].y2 = c + d/2.;
    
    set_dirtystate();
    xdrawgraph();
}


void swap_boxwv_coords(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditBoxUI *ui = (EditBoxUI *)client_data;
    int boxno = ui->boxno;

    if( ((int)GetChoice(ui->loc_item)?COORD_VIEW:COORD_WORLD) == boxes[boxno].loctype ) {
        return;
    }
	
    if( boxes[boxno].loctype == COORD_VIEW ) {
    boxes[boxno].gno = get_cg();
	boxes[boxno].loctype = COORD_WORLD;
	view2world( atof(xv_getstr(ui->x1_item)), atof(xv_getstr(ui->y1_item)),
				&boxes[boxno].x1,&boxes[boxno].y1 );
	view2world( atof(xv_getstr(ui->x2_item)), atof(xv_getstr(ui->y2_item)),
				&boxes[boxno].x2,&boxes[boxno].y2 );
    } else {
	boxes[boxno].loctype = COORD_VIEW;
	world2view( atof(xv_getstr(ui->x1_item)), atof(xv_getstr(ui->y1_item)),
				&boxes[boxno].x1,&boxes[boxno].y1 );
	world2view( atof(xv_getstr(ui->x2_item)), atof(xv_getstr(ui->y2_item)),
				&boxes[boxno].x2,&boxes[boxno].y2 );
    }
    update_box_edit( ui );
}


void box_edit_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditBoxUI *ui = (EditBoxUI *) client_data;
    int boxno = ui->boxno;

    boxes[boxno].color = GetOptionChoice(ui->color_item);
    boxes[boxno].loctype = GetChoice(ui->loc_item) ? COORD_VIEW : COORD_WORLD;
    boxes[boxno].lines = GetOptionChoice(ui->lines_item);
    boxes[boxno].linew = GetSpinChoice(ui->linew_item);
    boxes[boxno].fillcolor = GetOptionChoice(ui->fill_color_item);
    boxes[boxno].fillpattern = GetOptionChoice(ui->fill_pattern_item);
    xv_evalexpr( ui->x1_item, &boxes[boxno].x1 );
    xv_evalexpr( ui->x2_item, &boxes[boxno].x2 );
    xv_evalexpr( ui->y1_item, &boxes[boxno].y1 );
    xv_evalexpr( ui->y2_item, &boxes[boxno].y2 );
    
    set_dirtystate();
    xdrawgraph();
}

void box_edit_popup(int boxno)
{
    Widget rc;
    Widget panel;
    Widget buts[2];

    set_wait_cursor();
    if (box_ui.top == NULL) {
	char *label1[3];
	label1[0] = "Accept";
	label1[1] = "Close";
	box_ui.top = XmCreateDialogShell(app_shell, "Edit box", NULL, 0);

	handle_close(box_ui.top);
	panel = XmCreateRowColumn(box_ui.top, "boxes_rc", NULL, 0);

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	box_ui.color_item = CreateColorChoice(rc, "Color: ");

	box_ui.linew_item = CreateLineWidthChoice(rc, "Line width:");

	box_ui.lines_item = CreateLineStyleChoice(rc, "Line style:");

	box_ui.fill_pattern_item = CreatePatternChoice(rc, "Fill pattern:");
	box_ui.fill_color_item = CreateColorChoice(rc, "Fill color: ");
	box_ui.loc_item = CreatePanelChoice(rc, "Position in:",
					   3,
					   "World coordinates",
					   "Viewport coordinates",
					   NULL);
                                          
        XtAddCallback(box_ui.loc_item[2], XmNactivateCallback, 
               swap_boxwv_coords, (XtPointer) &box_ui);
        XtAddCallback(box_ui.loc_item[3], XmNactivateCallback, 
               swap_boxwv_coords, (XtPointer) &box_ui);


	box_ui.x1_item = CreateTextItem2(rc, 12, "Xmin = ");
	box_ui.y1_item = CreateTextItem2(rc, 12, "Ymin = ");
	box_ui.x2_item = CreateTextItem2(rc, 12, "Xmax = ");
	box_ui.y2_item = CreateTextItem2(rc, 12, "Ymax = ");
	ManageChild(rc);

	CreateSeparator(panel);

	CreateCommandButtons(panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
	  	box_edit_proc, (XtPointer) &box_ui);
	XtAddCallback(buts[1], XmNactivateCallback,
		destroy_dialog, (XtPointer) box_ui.top);
	ManageChild(panel);
    }
    box_ui.boxno = boxno;
    update_box_edit(&box_ui);
   	RaiseWindow(box_ui.top);
    unset_wait_cursor();
}

void ellipse_edit_popup(int boxno)
{
/*    static EditBoxUI ui;*/
    Widget rc;
    Widget panel;
    Widget buts[2];

    set_wait_cursor();
    if (ellip_ui.top == NULL) {
		char *label1[2];
		label1[0] = "Accept";
		label1[1] = "Close";
		ellip_ui.top = XmCreateDialogShell(app_shell, "Edit ellipse", NULL, 0);
		handle_close(ellip_ui.top);
		panel = XmCreateRowColumn(ellip_ui.top, "ellipses_rc", NULL, 0);

    	 rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	ellip_ui.color_item = CreateColorChoice(rc, "Color: ");

	ellip_ui.linew_item = CreateLineWidthChoice(rc, "Line width:");

	ellip_ui.lines_item = CreateLineStyleChoice(rc, "Line style:");

	ellip_ui.fill_pattern_item = CreatePatternChoice(rc, "Fill pattern:");
	ellip_ui.fill_color_item = CreateColorChoice(rc, "Fill color: ");
	ellip_ui.loc_item = CreatePanelChoice(rc, "Position in:",
					   3,
					   "World coordinates",
					   "Viewport coordinates",
					   NULL);
        XtAddCallback(ellip_ui.loc_item[2], XmNactivateCallback, 
               swap_ellipwv_coords, (XtPointer) &ellip_ui );
        XtAddCallback(ellip_ui.loc_item[3], XmNactivateCallback, 
               swap_ellipwv_coords, (XtPointer) &ellip_ui );


	ellip_ui.x1_item = CreateTextItem2(rc, 12, "Xcentre = ");
	ellip_ui.y1_item = CreateTextItem2(rc, 12, "Ycentre = ");
	ellip_ui.x2_item = CreateTextItem2(rc, 12, "Width = ");
	ellip_ui.y2_item = CreateTextItem2(rc, 12, "Height = ");
    	ManageChild(rc);

    	CreateSeparator(panel);

    	ellip_ui.boxno = boxno;
    	CreateCommandButtons(panel, 2, buts, label1);
    	XtAddCallback(buts[0], XmNactivateCallback,
		    	  ellipse_edit_proc, (XtPointer) &ellip_ui);
    	XtAddCallback(buts[1], XmNactivateCallback,
			  destroy_dialog, (XtPointer) ellip_ui.top);
    	ManageChild(panel);
    }
    ellip_ui.boxno = boxno;
    update_ellipse_edit(&ellip_ui);
    RaiseWindow(ellip_ui.top);
    unset_wait_cursor();
}

typedef struct {
    Widget top;
    OptionStructure *color_item;
    SpinStructure *linew_item;
    OptionStructure *lines_item;
    Widget *loc_item;
    Widget *arrow_item;
    Widget *atype_item;
    SpinStructure *asize_item;
    SpinStructure *dL_ff_item;
    SpinStructure *lL_ff_item;
    Widget x1_item;
    Widget y1_item;
    Widget x2_item;
    Widget y2_item;
    int lineno;
} EditLineUI;

void update_line_edit(EditLineUI *ui)
{
    if (ui->top) {
	int lineno = ui->lineno;
	SetOptionChoice(ui->color_item, lines[lineno].color);
	SetOptionChoice(ui->lines_item, lines[lineno].lines);
	SetSpinChoice(ui->linew_item, lines[lineno].linew);
	SetChoice(ui->arrow_item, lines[lineno].arrow_end);
	SetChoice(ui->atype_item, lines[lineno].arrow.type);
	SetSpinChoice(ui->asize_item, lines[lineno].arrow.length);
	SetSpinChoice(ui->dL_ff_item, lines[lineno].arrow.dL_ff);
	SetSpinChoice(ui->lL_ff_item, lines[lineno].arrow.lL_ff);
	SetChoice(ui->loc_item, lines[lineno].loctype == COORD_VIEW ? 1 : 0);
	sprintf(buf, "%.12f", lines[lineno].x1);
	xv_setstr(ui->x1_item, buf);
	sprintf(buf, "%.12f", lines[lineno].y1);
	xv_setstr(ui->y1_item, buf);
	sprintf(buf, "%.12f", lines[lineno].x2);
	xv_setstr(ui->x2_item, buf);
	sprintf(buf, "%.12f", lines[lineno].y2);
	xv_setstr(ui->y2_item, buf);
    }
}

void swap_linewv_coords(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditLineUI *ui = (EditLineUI *)client_data;
    int lineno = ui->lineno;
	
    if( lines[lineno].loctype == ((int) GetChoice(ui->loc_item)?COORD_VIEW:COORD_WORLD) ) {
        return;
    }
	   
	if( lines[lineno].loctype == COORD_VIEW ) {
		lines[lineno].gno = get_cg();
		lines[lineno].loctype = COORD_WORLD;
		view2world( atof(xv_getstr(ui->x1_item)), atof(xv_getstr(ui->y1_item)),
										&lines[lineno].x1,&lines[lineno].y1 );
		view2world( atof(xv_getstr(ui->x2_item)), atof(xv_getstr(ui->y2_item)),
										&lines[lineno].x2,&lines[lineno].y2 );
	} else {
		lines[lineno].loctype = COORD_VIEW;
		world2view( atof(xv_getstr(ui->x1_item)), atof(xv_getstr(ui->y1_item)),
										&lines[lineno].x1,&lines[lineno].y1 );
		world2view( atof(xv_getstr(ui->x2_item)), atof(xv_getstr(ui->y2_item)),
										&lines[lineno].x2,&lines[lineno].y2 );
	}
	update_line_edit( ui );
}


void line_edit_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditLineUI *ui = (EditLineUI *) client_data;
    int lineno = ui->lineno;

    lines[lineno].color = GetOptionChoice(ui->color_item);
    lines[lineno].loctype = GetChoice(ui->loc_item) ? COORD_VIEW : COORD_WORLD;
    lines[lineno].lines = GetOptionChoice(ui->lines_item);
    lines[lineno].linew = GetSpinChoice(ui->linew_item);
    xv_evalexpr(ui->x1_item, &lines[lineno].x1);
    xv_evalexpr(ui->y1_item, &lines[lineno].y1);
    xv_evalexpr(ui->x2_item, &lines[lineno].x2);
    xv_evalexpr(ui->y2_item, &lines[lineno].y2);
    lines[lineno].arrow_end = GetChoice(ui->arrow_item);
    lines[lineno].arrow.type = GetChoice(ui->atype_item);
    lines[lineno].arrow.length = GetSpinChoice(ui->asize_item);
    lines[lineno].arrow.dL_ff = GetSpinChoice(ui->dL_ff_item);
    lines[lineno].arrow.lL_ff = GetSpinChoice(ui->lL_ff_item);
    
    set_dirtystate();
    xdrawgraph();
}

static EditLineUI line_ui;

void line_edit_popup(int lineno)
{
    Widget rc, fr, rc2;
    Widget panel;
    Widget buts[2];

    set_wait_cursor();
    if (line_ui.top == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	line_ui.top = XmCreateDialogShell(app_shell, "Edit Line", NULL, 0);
	handle_close(line_ui.top);
	panel = XmCreateRowColumn(line_ui.top, "lines_rc", NULL, 0);

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	line_ui.color_item = CreateColorChoice(rc, "Color: ");

	line_ui.linew_item = CreateLineWidthChoice(rc, "Line width:");

	line_ui.lines_item = CreateLineStyleChoice(rc, "Line style:");

	fr = CreateFrame(rc, "Arrow");
        rc2 = XtVaCreateWidget("rc2", xmRowColumnWidgetClass, fr, NULL);
        line_ui.arrow_item = CreatePanelChoice(rc2, "Place at:",
					     5,
					     "None",
					     "Start",
					     "End",
					     "Both ends",
					     NULL);

	line_ui.atype_item = CreatePanelChoice(rc2, "Type:",
					     4,
					     "Line",
					     "Filled",
					     "Opaque",
					     NULL);

	line_ui.asize_item = CreateSpinChoice(rc2, "Length",
            4, SPIN_TYPE_FLOAT, -10.0, 10.0, 0.5);
	line_ui.dL_ff_item = CreateSpinChoice(rc2, "d/L form factor",
            4, SPIN_TYPE_FLOAT, 0.0, 10.0, 0.1);
	line_ui.lL_ff_item = CreateSpinChoice(rc2, "l/L form factor",
            4, SPIN_TYPE_FLOAT, -1.0, 1.0, 0.1);
	ManageChild(rc2);

	line_ui.loc_item = CreatePanelChoice(rc, "Position in:",
					   3,
					   "World coordinates",
					   "Viewport coordinates",
					   NULL);
        XtAddCallback(line_ui.loc_item[2], XmNactivateCallback,
                   swap_linewv_coords, (XtPointer) &line_ui);
        XtAddCallback(line_ui.loc_item[3], XmNactivateCallback,
                   swap_linewv_coords, (XtPointer) &line_ui);


	line_ui.x1_item = CreateTextItem2(rc, 12, "X1 = ");
	line_ui.y1_item = CreateTextItem2(rc, 12, "Y1 = ");
	line_ui.x2_item = CreateTextItem2(rc, 12, "X2 = ");
	line_ui.y2_item = CreateTextItem2(rc, 12, "Y2 = ");
	ManageChild(rc);

	CreateSeparator(panel);

	CreateCommandButtons(panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		line_edit_proc, (XtPointer) &line_ui);
	XtAddCallback(buts[1], XmNactivateCallback,
		destroy_dialog, (XtPointer) line_ui.top);

	ManageChild(panel);
    }
    RaiseWindow(line_ui.top);
    line_ui.lineno = lineno;
    update_line_edit(&line_ui);
    unset_wait_cursor();
}

typedef struct {
    Widget top;
    TextStructure *string_item;
    OptionStructure *color_item;
    Widget *loc_item;
    OptionStructure *font_item;
    Widget size_item;
    Widget rot_item;
    OptionStructure *just_item;
    Widget x1_item;
    Widget y1_item;
    int stringno;
} EditStringUI;


void update_string_edit(EditStringUI *ui)
{
    if (ui->top) {
	plotstr *pstring = &pstr[ui->stringno];
	SetTextString(ui->string_item, pstring->s);
	SetOptionChoice(ui->color_item, pstring->color);
	SetOptionChoice(ui->just_item, pstring->just);
	SetOptionChoice(ui->font_item, pstring->font );
        SetCharSizeChoice(ui->size_item, pstring->charsize);
        SetAngleChoice(ui->rot_item, pstring->rot);
	SetChoice(ui->loc_item, pstring->loctype == COORD_VIEW ? 1 : 0);
	sprintf(buf, "%.12f", pstring->x);
	xv_setstr(ui->x1_item, buf);
	sprintf(buf, "%.12f", pstring->y);
	xv_setstr(ui->y1_item, buf);
    }
}

void swap_stringwv_coords(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditStringUI *ui = (EditStringUI *)client_data;
    int stringno = ui->stringno;
	
    if( pstr[stringno].loctype == ((int)GetChoice(ui->loc_item)?COORD_VIEW:COORD_WORLD) ) {
        return;
    }
	   
	if( pstr[stringno].loctype == COORD_VIEW ) {
		pstr[stringno].gno = get_cg();
		pstr[stringno].loctype = COORD_WORLD;
		view2world( atof(xv_getstr(ui->x1_item)), atof(xv_getstr(ui->y1_item)),
								&pstr[stringno].x,&pstr[stringno].y );
	} else {
		pstr[stringno].loctype = COORD_VIEW;
		world2view( atof(xv_getstr(ui->x1_item)), atof(xv_getstr(ui->y1_item)),
								&pstr[stringno].x,&pstr[stringno].y );
	}
	update_string_edit( ui );
}


void string_edit_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditStringUI *ui = (EditStringUI *) client_data;
    int stringno = ui->stringno;

    pstr[stringno].s = copy_string(pstr[stringno].s, GetTextString(ui->string_item));
    pstr[stringno].color = GetOptionChoice(ui->color_item);
    pstr[stringno].loctype = GetChoice(ui->loc_item) ? COORD_VIEW : COORD_WORLD;
    pstr[stringno].font = GetOptionChoice(ui->font_item);
    pstr[stringno].just = GetOptionChoice(ui->just_item);
    xv_evalexpr(ui->x1_item, &pstr[stringno].x);
    xv_evalexpr(ui->y1_item, &pstr[stringno].y);
    pstr[stringno].charsize = GetCharSizeChoice(ui->size_item);
    pstr[stringno].rot = GetAngleChoice(ui->rot_item);
    
    set_dirtystate();
    xdrawgraph();
}

static EditStringUI string_ui;

void string_edit_popup(int stringno)
{
    Widget rc;
    Widget buts[2];
    Widget panel;

    set_wait_cursor();
    if (string_ui.top == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	string_ui.top = XmCreateDialogShell(app_shell, "Edit String", NULL, 0);
	handle_close(string_ui.top);
	panel = XmCreateRowColumn(string_ui.top, "strings_rc", NULL, 0);

	string_ui.string_item = CreateCSText(panel, "String:");

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, panel, NULL);

	string_ui.color_item = CreateColorChoice(rc, "Color: ");
	
	
	string_ui.font_item = CreateFontChoice(rc, "Font:");

        string_ui.just_item = CreateJustChoice(rc, "Justification:");

        string_ui.loc_item = CreatePanelChoice(rc, "Position in:",
					     3,
					     "World coordinates",
					     "Viewport coordinates",
					     NULL);
        XtAddCallback(string_ui.loc_item[2], XmNactivateCallback,
                  swap_stringwv_coords, (XtPointer) &string_ui);
        XtAddCallback(string_ui.loc_item[3], XmNactivateCallback,
                  swap_stringwv_coords, (XtPointer) &string_ui);
					     
	string_ui.x1_item = CreateTextItem2(rc, 12, "X = ");
	string_ui.y1_item = CreateTextItem2(rc, 12, "Y = ");	

	ManageChild(rc);

	string_ui.rot_item = CreateAngleChoice(panel, "Rotation");

	string_ui.size_item = CreateCharSizeChoice(panel, "Size");
						    
	CreateSeparator(panel);

	CreateCommandButtons(panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		    string_edit_proc, (XtPointer) &string_ui );
	XtAddCallback(buts[1], XmNactivateCallback,
		destroy_dialog, (XtPointer) string_ui.top);

	ManageChild(panel);
    }
    RaiseWindow(string_ui.top);
    string_ui.stringno = stringno;
    update_string_edit(&string_ui);
    unset_wait_cursor();
}

int object_edit_popup(int type, int id)
{
    switch (type) {
    case OBJECT_BOX:
        box_edit_popup(id);
        break;
    case OBJECT_ELLIPSE:
        ellipse_edit_popup(id);
        break;
    case OBJECT_LINE:
        line_edit_popup(id);
        break;
    case OBJECT_STRING:
        string_edit_popup(id);
        break;
    default:
        return RETURN_FAILURE;
        break;
    }
    return RETURN_SUCCESS;
}
