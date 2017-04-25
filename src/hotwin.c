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
 *
 * hot links
 *
 */

#include <config.h>

#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>

#include "globals.h"
#include "graphs.h"
#include "parser.h"
#include "motifinc.h"
#include "protos.h"

static Widget hotlink_frame = (Widget) NULL;
static SetChoiceItem hotlink_set_item;
static Widget hotlink_list_item;
static Widget hotlink_file_item;
static Widget *hotlink_source_item;

void create_hotfiles_popup(Widget w, XtPointer client_data, XtPointer call_data);

/*
 * establish hotlinks by associating a set with a column in a file
 * or the stdout of a process
 */
static void do_hotlink_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int i, numset, src, *sets;
    char fname[256];
    char buf[256];
    XmString xms;

    set_wait_cursor();

    numset = GetSelectedSets(hotlink_set_item, &sets);
    src = GetChoice(hotlink_source_item);
    strcpy(fname, xv_getstr(hotlink_file_item));

    if (numset == SET_SELECT_ERROR) {
        errwin("No set selected");
        unset_wait_cursor();
        return;
    }
    if (fname[0] == '\0') {
        errwin("No source selected");
        unset_wait_cursor();
        return;
    }

	for( i=0; i<numset; i++ ) {
		if( numset == 1 )
			sprintf(buf, "G%d.S%d -> %s -> %s", get_cg(), sets[i],
								src==0 ? "DISK" : "PIPE", fname );
		else
			sprintf(buf, "G%d.S%d -> %s -> %s:%d", get_cg(), sets[i], 
						src == 0 ? "DISK" : "PIPE", fname, i+1);

		xms = XmStringCreateLocalized(buf);
		XmListAddItemUnselected(hotlink_list_item, xms, 0);
    	        XmStringFree(xms);
		set_hotlink(get_cg(), sets[i], i+1, fname, src==0?SOURCE_DISK:SOURCE_PIPE);
		if( numset == 1 )
			setcomment( get_cg(), sets[i], fname );
		else {
			sprintf( buf, "%s:%d", fname, i+1 );
			setcomment( get_cg(), sets[i], buf );
		}
	}

    unset_wait_cursor();
}

/*
 * unlink sets
 */
static void do_hotunlink_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmString *s, cs;
    int  cnt, setno;
    char *cstr;

    set_wait_cursor();

	XtVaGetValues(hotlink_list_item, XmNselectedItemCount, &cnt,
							  XmNselectedItems, &s,
							  NULL);
    if( cnt ) {
		for( ; cnt; cnt-- ) {
			cs = XmStringCopy(s[cnt-1]);
			if ((cstr = GetStringSimple(cs))) {
				sscanf(cstr, "G%*d.S%d", &setno);
				if (setno >= 0 && setno < number_of_sets(get_cg())) {
					set_hotlink(get_cg(), setno, FALSE, NULL, 0);
				}
			}
			XmStringFree(cs);
			XtFree(cstr);
		}
		update_hotlinks();
	}
    unset_wait_cursor();
}

/*
 * update hot links displayed in scrolled list
 */
void update_hotlinks(void)
{
    int j;
    char buf[256];
    XmString xms;

    if (hotlink_frame != NULL) {
		set_wait_cursor();
		XmListDeleteAllItems(hotlink_list_item);
		for (j = 0; j < number_of_sets(get_cg()); j++) {
			if (is_hotlinked(get_cg(), j)) {
				sprintf(buf, "G%d.S%d -> %s -> %s:%d", get_cg(), j, 
					get_hotlink_src(get_cg(), j) == SOURCE_DISK ? "DISK" : "PIPE", 
					get_hotlink_file(get_cg(), j), is_hotlinked(get_cg(),j) );
				xms = XmStringCreateLocalized(buf);
				XmListAddItemUnselected(hotlink_list_item, xms, 0);
				XmStringFree(xms);
			}
		}
		unset_wait_cursor();
    }
}

/*
 * update the sets in the current graph
 */
void do_hotupdate_proc(void *data)
{
    int gno, setno;

    set_wait_cursor();

    /* do links */
    for (gno = 0; gno < number_of_graphs(); gno++) {
        for (setno = 0; setno < number_of_sets(gno); setno++) {
            do_update_hotlink(gno, setno);
        }
    }

    unset_wait_cursor();
    xdrawgraph();
}

/*
 * create the big hot links widget
 */
void create_hotlinks_popup(void *data)
{
    static Widget top, dialog;
    Arg args[3];
    set_wait_cursor();
    if (top == NULL) {
	char *label1[5];
	Widget but1[5];
	label1[0] = "Link";
	label1[1] = "Files...";
	label1[2] = "Unlink";
	label1[3] = "Update";
	label1[4] = "Close";
	top = XmCreateDialogShell(app_shell, "Hot links", NULL, 0);
	handle_close(top);
	dialog = XmCreateRowColumn(top, "dialog_rc", NULL, 0);

	XtSetArg(args[0], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE);
	XtSetArg(args[1], XmNvisibleItemCount, 5);
	XtSetArg(args[2], XmNselectionPolicy, XmEXTENDED_SELECT );
	hotlink_list_item = XmCreateScrolledList(dialog, "list", args, 3);
	ManageChild(hotlink_list_item);

        hotlink_set_item = CreateSetSelector(dialog, "Link set:",
                SET_SELECT_ACTIVE,
                FILTER_SELECT_ALL,
                GRAPH_SELECT_CURRENT,
                SELECTION_TYPE_MULTIPLE);

	hotlink_file_item = CreateTextItem2(dialog, 30, "To file or SOURCE_PIPE:");
	hotlink_source_item = CreatePanelChoice(dialog, "Source: ", 3,
						"Disk file",
						"Pipe",
						NULL);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 5, but1, label1);
	XtAddCallback(but1[0], XmNactivateCallback, (XtCallbackProc) do_hotlink_proc,
		      (XtPointer) NULL);
	XtAddCallback(but1[1], XmNactivateCallback, (XtCallbackProc) create_hotfiles_popup,
		      (XtPointer) NULL);
	XtAddCallback(but1[2], XmNactivateCallback, (XtCallbackProc) do_hotunlink_proc,
		      (XtPointer) NULL);
	XtAddCallback(but1[3], XmNactivateCallback, (XtCallbackProc) do_hotupdate_proc,
		      (XtPointer) NULL);
	XtAddCallback(but1[4], XmNactivateCallback, (XtCallbackProc) destroy_dialog,
		      (XtPointer) top);

	ManageChild(dialog);
	hotlink_frame = top;
    }
    RaiseWindow(top);
    update_hotlinks();
    unset_wait_cursor();
}


/*
 * callback for the file selection widget
 * this routine copies the selected file into the text widget on the main
 * hot links widget
 */
static int do_hotlinkfile_proc(char *filename, void *data)
{
    xv_setstr(hotlink_file_item, filename);
    return TRUE;
}

/*
 * create file selection pop up to choose the file to hot link
 */
void create_hotfiles_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        fsb = CreateFileSelectionBox(app_shell, "Select hot link file");
	AddFileSelectionBoxCB(fsb, do_hotlinkfile_proc, NULL);
        ManageChild(fsb->FSB);
    }
    
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}
