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
 * track/edit points etc.
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "draw.h"
#include "graphs.h"
#include "events.h"
#include "protos.h"
#include "motifinc.h"

static int track_setno = -1;
static int track_add_at;    /* where to begin inserting points in the set */
static int track_move_dir;  /* direction on point movement */

static Widget points_frame;
static ListStructure *track_set_sel;
static TextStructure *locate_point_item;
static Widget locate_point_message;
static Widget goto_index_item;

/*
 * set tracker
 */
static void do_track_proc(void *data)
{
    set_action(DO_NOTHING);
    SetLabel(locate_point_message, "Track points");
    set_action(TRACKER);
}

/*
 * activate the add point item in the canvas event proc
 */
static void do_add_proc(void *data)
{
    char *s;

    set_action(DO_NOTHING);

    track_add_at = (int) data;

    switch (track_add_at) {
    case ADD_POINT_BEGINNING:
        s = "Add points at the beginning of set";
        break;
    case ADD_POINT_END:
        s = "Add points to the end of set";
        break;
    case ADD_POINT_NEAREST:
        s = "Add points to the nearest position";
        break;
    default:
        return;
    }

    SetLabel(locate_point_message, s);

    set_action(ADD_POINT);
}

/*
 * activate the delete point item in the canvas event proc
 */
static void do_del_proc(void *data)
{
    set_action(DO_NOTHING);
    SetLabel(locate_point_message, "Delete points");
    set_action(DEL_POINT);
}

/*
 * move a point
 */
static void do_ptsmove_proc(void *data)
{
    char *s;
    
    set_action(DO_NOTHING);

    track_move_dir = (int) data;

    switch (track_move_dir) {
    case MOVE_POINT_XY:
        s = "Move points";
        break;
    case MOVE_POINT_X:
        s = "Move points along x";
        break;
    case MOVE_POINT_Y:
        s = "Move points along y";
        break;
    default:
        return;
    }
    SetLabel(locate_point_message, s);

    set_action(MOVE_POINT1ST);
}

static void do_gotopt_proc(void *data)
{
    int ind;
    WPoint wp;
    VPoint vp;
    int cg = get_cg();

    if (!is_set_active(cg, track_setno)) {
        errmsg("No or inactive set selected");
        return;
    }

    xv_evalexpri(goto_index_item, &ind);
    if (get_point(cg, track_setno, ind, &wp) == RETURN_SUCCESS) {
        vp = Wpoint2Vpoint(wp);
        setpointer(vp);
    } else {
        errmsg("Point index out of range");
    }
}


static void points_done_proc(void *data)
{
    set_action(DO_NOTHING);
    UnmanageChild(GetParent((Widget) data));
}

static void track_set_cbproc(int n, int *values, void *data)
{
    if (n == 1) {
        track_setno = values[0];
    } else {
        track_setno = -1;
    }
}

void create_points_frame(void *data)
{
    set_wait_cursor();
    
    if (points_frame == NULL) {
        Widget dialog, wbut, rc, fr;
        
	points_frame = CreateDialogForm(app_shell, "Point explorer");
	
	fr = CreateFrame(points_frame, NULL);
        AddDialogFormChild(points_frame, fr);
	locate_point_message = CreateLabel(fr, "Point explorer");
        
        dialog = CreateVContainer(points_frame);
        AddDialogFormChild(points_frame, dialog);

        track_set_sel = CreateSetChoice(dialog,
            "Restrict to set:", LIST_TYPE_SINGLE, TRUE);
        AddListChoiceCB(track_set_sel, track_set_cbproc, NULL);
        
	rc = CreateHContainer(dialog);
	goto_index_item = CreateTextItem2(rc, 6, "Point location:");
	wbut = CreateButton(rc, "Goto point");
	AddButtonCB(wbut, do_gotopt_proc, NULL);

	locate_point_item = CreateTextInput(dialog, "Point data:");

	CreateSeparator(dialog);

	rc = CreateHContainer(dialog);

	wbut = CreateButton(rc, "Track");
	AddButtonCB(wbut, do_track_proc, NULL);

	wbut = CreateButton(rc, "Move");
	AddButtonCB(wbut, do_ptsmove_proc, (void *) MOVE_POINT_XY);
	wbut = CreateButton(rc, "Move X");
	AddButtonCB(wbut, do_ptsmove_proc,  (void *) MOVE_POINT_X);
	wbut = CreateButton(rc, "Move Y");
	AddButtonCB(wbut, do_ptsmove_proc,  (void *) MOVE_POINT_Y);

	wbut = CreateButton(rc, "Prepend");
	AddButtonCB(wbut, do_add_proc, (void *) ADD_POINT_BEGINNING);
	wbut = CreateButton(rc, "Append");
	AddButtonCB(wbut, do_add_proc, (void *) ADD_POINT_END);
	wbut = CreateButton(rc, "Insert");
	AddButtonCB(wbut, do_add_proc, (void *) ADD_POINT_NEAREST);

	wbut = CreateButton(rc, "Delete");
	AddButtonCB(wbut, do_del_proc, NULL);

	wbut = CreateButton(rc, "Close");
	AddButtonCB(wbut, points_done_proc, (void *) points_frame);
        
        ManageChild(points_frame);
    }
    
    RaiseWindow(GetParent(points_frame));
    unset_wait_cursor();
}

void update_point_locator(int gno, int setno, int loc)
{
    int col, ncols;
    Datapoint dpoint;
    char *s, buf[64];
    
    if (points_frame == NULL) {
        return;
    }
    
    if (get_datapoint(gno, setno, loc, &ncols, &dpoint) == RETURN_SUCCESS) {
        SelectListChoice(track_set_sel, setno);

        s = copy_string(NULL, "(");
        for (col = 0; col < ncols; col++) {
            sprintf(buf, "%g", dpoint.ex[col]);
            s = concat_strings(s, buf);
            if (col != ncols - 1) {
                s = concat_strings(s, ", ");
            }
        }
        if (dpoint.s != NULL) {
            s = concat_strings(s, ", \"");
            s = concat_strings(s, dpoint.s);
            s = concat_strings(s, "\"");
        }
        s = concat_strings(s, ")");
        SetTextString(locate_point_item, s);
        xfree(s);

        sprintf(buf, "%d", loc);
        xv_setstr(goto_index_item, buf);
    } else {
        track_setno = -1;
        SelectListChoices(track_set_sel, 0, NULL);
        SetTextString(locate_point_item, "");
        xv_setstr(goto_index_item, "");
    }
}

void get_tracking_props(int *setno, int *move_dir, int *add_at)
{
    *setno = track_setno;
    *move_dir = track_move_dir;
    *add_at = track_add_at;
}
