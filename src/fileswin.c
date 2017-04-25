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
 * read/write data/parameter files
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#if defined(HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Text.h>

#include "globals.h"
#include "graphs.h"
#include "graphutils.h"
#include "utils.h"
#include "files.h"
#include "motifinc.h"
#include "protos.h"

static int open_proc(char *filename, void *data);
static int save_proc(char *filename, void *data);

static int read_sets_proc(char *filename, void *data);
static void set_load_proc(int value, void *data);
static void set_src_proc(Widget w, XtPointer client_data, XtPointer call_data);
static int write_sets_proc(char *filename, void *data);

static int read_params_proc(char *filename, void *data);
static int write_params_proc(char *filename, void *data);

typedef struct {
    Widget format_item;    /* format */
    Widget descr_item;     /* description */
} saveGUI;

static saveGUI save_gui;

static void update_save_gui(saveGUI *gui)
{
    xv_setstr(gui->format_item, sformat);
    xv_setstr(gui->descr_item, get_project_description());
}

void create_saveproject_popup(void)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        Widget fr, rc;
	
        fsb = CreateFileSelectionBox(app_shell, "Save project");

	fr = CreateFrame(fsb->rc, NULL);
	rc = XmCreateRowColumn(fr, "rc", NULL, 0);
	save_gui.descr_item  = CreateScrollTextItem2(rc, 5, "Project description:");
	save_gui.format_item = CreateTextItem2(rc, 15, "Data format:");
        ManageChild(rc);

	AddFileSelectionBoxCB(fsb, save_proc, &save_gui);
        ManageChild(fsb->FSB);
    }
    
    update_save_gui(&save_gui);
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

/*
 *  save project to a file
 */
static int save_proc(char *filename, void *data)
{
    char *s;
    saveGUI *gui = (saveGUI *) data;
    
    strcpy(sformat, xv_getstr(gui->format_item));
    s = XmTextGetString(gui->descr_item);
    set_project_description(s);
    XtFree(s);
    if (save_project(filename) == RETURN_SUCCESS) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void create_openproject_popup(void)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        fsb = CreateFileSelectionBox(app_shell, "Open project");
	AddFileSelectionBoxCB(fsb, open_proc, NULL);
        ManageChild(fsb->FSB);
    }
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

/*
 *  open project from a file
 */
static int open_proc(char *filename, void *data)
{
    if (load_project(filename) == RETURN_SUCCESS) {
        update_all();
        xdrawgraph();
        return TRUE;
    } else {
        return FALSE;
    }
}


typedef struct {
    ListStructure *graph_item;     /* graph choice item */
    OptionStructure *ftype_item;   /* set type choice item */
    OptionStructure *load_item;    /* load as single/nxy/block */
    OptionStructure *auto_item;    /* autoscale on read */
} rdataGUI;

void create_file_popup(void *data)
{
    static FSBStructure *rdata_dialog = NULL;

    set_wait_cursor();

    if (rdata_dialog == NULL) {
        int i;
        Widget lab, rc, rc2, fr, rb, w[2];
        rdataGUI *gui;
        OptionItem option_items[3];
        
        gui = xmalloc(sizeof(rdataGUI));
        
	rdata_dialog = CreateFileSelectionBox(app_shell, "Read sets");
	AddFileSelectionBoxCB(rdata_dialog, read_sets_proc, (void *) gui);

	fr = CreateFrame(rdata_dialog->rc, NULL);
	rc = XmCreateRowColumn(fr, "rc", NULL, 0);

	gui->graph_item = CreateGraphChoice(rc,
            "Read to graph:", LIST_TYPE_SINGLE);

	rc2 = XmCreateRowColumn(rc, "rc2", NULL, 0);
	XtVaSetValues(rc2, XmNorientation, XmHORIZONTAL, NULL);
 
	option_items[0].value = LOAD_SINGLE;
	option_items[0].label = "Single set";
	option_items[1].value = LOAD_NXY;
	option_items[1].label = "NXY";
	option_items[2].value = LOAD_BLOCK;
	option_items[2].label = "Block data";
	gui->load_item = CreateOptionChoice(rc2, "Load as", 1, 3, option_items);
        AddOptionChoiceCB(gui->load_item, set_load_proc, (void *) gui);
	gui->ftype_item = CreateSetTypeChoice(rc2, "Set type:");
	ManageChild(rc2);

	rc2 = XmCreateRowColumn(rc, "rc2", NULL, 0);
	XtVaSetValues(rc2, XmNorientation, XmHORIZONTAL, NULL);
	lab = CreateLabel(rc2, "Data source:");
	rb = XmCreateRadioBox(rc2, "radio_box_2", NULL, 0);
	XtVaSetValues(rb, XmNorientation, XmHORIZONTAL, NULL);
	w[0] = CreateToggleButton(rb, "Disk");
	w[1] = CreateToggleButton(rb, "Pipe");
	for (i = 0; i < 2; i++) {
	    XtAddCallback(w[i],
                XmNvalueChangedCallback, set_src_proc, (XtPointer) i);
	}
	ManageChild(rb);
	ManageChild(w[0]);
	ManageChild(w[1]);
	SetToggleButtonState(w[0], TRUE);
	ManageChild(rc2);

	gui->auto_item = CreateASChoice(rc, "Autoscale on read:");

	ManageChild(rc);
        ManageChild(rdata_dialog->FSB);
    }
    
    
    RaiseWindow(rdata_dialog->dialog);
    
    unset_wait_cursor();
}

static int read_sets_proc(char *filename, void *data)
{
    int graphno;
    int load;
    
    rdataGUI *gui = (rdataGUI *) data;
    
    load = GetOptionChoice(gui->load_item);
    if (GetSingleListChoice(gui->graph_item, &graphno) != RETURN_SUCCESS) {
        errmsg("Please select a single graph");
    } else {
        if (load == LOAD_SINGLE) {
            curtype = GetOptionChoice(gui->ftype_item);
        }

        autoscale_onread = GetOptionChoice(gui->auto_item);
        
        getdata(graphno, filename, cursource, load);

	if (load == LOAD_BLOCK) {
            create_eblock_frame(graphno);
        } else {
            update_all();
            xdrawgraph();
        }
    }
    /* never close the popup */
    return FALSE;
}

static void set_src_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int which = (int) client_data;
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data;

    if (state->set) {
        cursource = which;
    }
}

static void set_load_proc(int value, void *data)
{
    rdataGUI *gui = (rdataGUI *) data;
    
    if (value == LOAD_SINGLE) {
        SetSensitive(gui->ftype_item->menu, True);
    } else {
        SetOptionChoice(gui->ftype_item, SET_XY);
        SetSensitive(gui->ftype_item->menu, False);
    }
}


typedef struct {
    ListStructure *sel;
    Widget format_item;
} wdataGUI;

void create_write_popup(void *data)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        Widget fr, rc;
        wdataGUI *gui;
        
	gui = xmalloc(sizeof(wdataGUI));
	
        fsb = CreateFileSelectionBox(app_shell, "Write sets");
	AddFileSelectionBoxCB(fsb, write_sets_proc, (void *) gui);
	
	fr = CreateFrame(fsb->rc, NULL);
	rc = XmCreateRowColumn(fr, "rc", NULL, 0);
        gui->sel = CreateSetChoice(rc,
            "Write set(s):", LIST_TYPE_MULTIPLE, TRUE);
	gui->format_item = CreateTextItem2(rc, 15, "Format: ");
        xv_setstr(gui->format_item, sformat);
        ManageChild(rc);

        ManageChild(fsb->FSB);
    }
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

/*
 *  write a set or sets to a file
 */
static int write_sets_proc(char *filename, void *data)
{
    wdataGUI *gui = (wdataGUI *) data;
    int *selset, cd, i;
    int gno, setno;
    char format[32];
    FILE *cp;
    
    cp = grace_openw(filename);
    if (cp == NULL) {
        return FALSE;
    }

    cd = GetListChoices(gui->sel, &selset);
    if (cd < 1) {
        errmsg("No set selected");
    } else {
        gno = get_cg();
        strncpy(format, xv_getstr(gui->format_item), 31);
        for(i = 0; i < cd; i++) {
            setno = selset[i];
            write_set(gno, setno, cp, format, TRUE);
        }
        xfree(selset);
    }
    grace_close(cp);

    /* never close the popup */
    return FALSE;
}


void create_rparams_popup(void *data)
{
    static FSBStructure *rparams_dialog = NULL;

    set_wait_cursor();

    if (rparams_dialog == NULL) {
	rparams_dialog = CreateFileSelectionBox(app_shell, "Read parameters");
	AddFileSelectionBoxCB(rparams_dialog, read_params_proc, NULL);
        ManageChild(rparams_dialog->FSB);
    }
    
    RaiseWindow(rparams_dialog->dialog);

    unset_wait_cursor();
}

static int read_params_proc(char *filename, void *data)
{
    getparms(filename);
    update_all();
    xdrawgraph();

    /* never close the popup */
    return FALSE;
}

/*
 * Create the wparam Frame and the wparam Panel
 */
void create_wparam_frame(void *data)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        Widget fr, *graph_item;
	
        fsb = CreateFileSelectionBox(app_shell, "Write parameters");
	fr = CreateFrame(fsb->rc, NULL);
	graph_item = CreatePanelChoice(fr,
            "Write parameters from graph:",
            3,
            "Current",
            "All",
            NULL);
	AddFileSelectionBoxCB(fsb, write_params_proc, graph_item);
        ManageChild(fsb->FSB);
    }
    
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

static int write_params_proc(char *filename, void *data)
{
    Widget *graph_item = (Widget *) data;
    int gno;
    FILE *pp;

    if (GetChoice(graph_item) == 0) {
	gno = get_cg();
    } else {
	gno = ALL_GRAPHS;
    }
    
    pp = grace_openw(filename);
    if (pp != NULL) {
        putparms(gno, pp, 0);
        grace_close(pp);
    }

    /* never close the popup */
    return FALSE;
}


#ifdef HAVE_NETCDF

#include <netcdf.h>

/*
 *
 * netcdf reader
 *
 */

static Widget netcdf_frame = (Widget) NULL;

static Widget netcdf_listx_item;
static Widget netcdf_listy_item;
static Widget netcdf_file_item;

void create_netcdffiles_popup(Widget w, XtPointer client_data, XtPointer call_data);

static void do_netcdfquery_proc(Widget w, XtPointer client_data, XtPointer call_data);

void update_netcdfs(void);

int getnetcdfvars(void);

static void do_netcdf_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int setno;
    char fname[256];
    char xvar[256], yvar[256];
    XmString *s, cs;
    int *pos_list;
    int j, pos_cnt, cnt, retval;
    char *cstr;

    set_wait_cursor();

/*
 * setno == -1, then next set
 */
    setno = -1;
    strcpy(fname, xv_getstr(netcdf_file_item));
    if (XmListGetSelectedPos(netcdf_listx_item, &pos_list, &pos_cnt)) {
	XtVaGetValues(netcdf_listx_item,
		      XmNselectedItemCount, &cnt,
		      XmNselectedItems, &s,
		      NULL);
	cs = XmStringCopy(*s);
	if ((cstr = GetStringSimple(cs))) {
	    strcpy(xvar, cstr);
	    XtFree(cstr);
	}
	XmStringFree(cs);
    } else {
	errmsg("Need to select X, either variable name or INDEX");
	unset_wait_cursor();
	return;
    }
    if (XmListGetSelectedPos(netcdf_listy_item, &pos_list, &pos_cnt)) {
	j = pos_list[0];
	XtVaGetValues(netcdf_listy_item,
		      XmNselectedItemCount, &cnt,
		      XmNselectedItems, &s,
		      NULL);
	cs = XmStringCopy(*s);
	if ((cstr = GetStringSimple(cs))) {
	    strcpy(yvar, cstr);
	    XtFree(cstr);
	}
	XmStringFree(cs);
    } else {
	errmsg("Need to select Y");
	unset_wait_cursor();
	return;
    }
    if (strcmp(xvar, "INDEX") == 0) {
	retval = readnetcdf(get_cg(), setno, fname, NULL, yvar, -1, -1, 1);
    } else {
	retval = readnetcdf(get_cg(), setno, fname, xvar, yvar, -1, -1, 1);
    }
    if (retval) {
	xdrawgraph();
    }
    unset_wait_cursor();
}

void update_netcdfs(void)
{
    int i;
    char buf[256], fname[512];
    XmString xms;
    int cdfid;			/* netCDF id */
    int ndims, nvars, ngatts, recdim;
    int var_id;
    char varname[256];
    nc_type datatype = 0;
    int dim[100], natts;
    long dimlen[100];
    long len;

    ncopts = 0;			/* no crash on error */

    if (netcdf_frame != NULL) {
	strcpy(fname, xv_getstr(netcdf_file_item));
	set_wait_cursor();
	XmListDeleteAllItems(netcdf_listx_item);
	XmListDeleteAllItems(netcdf_listy_item);
	xms = XmStringCreateLocalized("INDEX");
	XmListAddItemUnselected(netcdf_listx_item, xms, 0);
	XmStringFree(xms);

	if (strlen(fname) < 2) {
	    unset_wait_cursor();
	    return;
	}
	if ((cdfid = ncopen(fname, NC_NOWRITE)) == -1) {
	    errmsg("Can't open file.");
	    unset_wait_cursor();
	    return;
	}
	ncinquire(cdfid, &ndims, &nvars, &ngatts, &recdim);
	for (i = 0; i < ndims; i++) {
	    ncdiminq(cdfid, i, NULL, &dimlen[i]);
	}
	for (i = 0; i < nvars; i++) {
	    ncvarinq(cdfid, i, varname, &datatype, &ndims, dim, &natts);
	    if ((var_id = ncvarid(cdfid, varname)) == -1) {
		char ebuf[256];
		sprintf(ebuf, "update_netcdfs(): No such variable %s", varname);
		errmsg(ebuf);
		continue;
	    }
	    if (ndims != 1) {
		continue;
	    }
	    ncdiminq(cdfid, dim[0], (char *) NULL, &len);
	    sprintf(buf, "%s", varname);
	    xms = XmStringCreateLocalized(buf);
	    XmListAddItemUnselected(netcdf_listx_item, xms, 0);
	    XmListAddItemUnselected(netcdf_listy_item, xms, 0);
	    XmStringFree(xms);
	}
	ncclose(cdfid);
	
	unset_wait_cursor();
    }
}

static void do_netcdfupdate_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    set_wait_cursor();
    update_netcdfs();
    unset_wait_cursor();
}

void create_netcdfs_popup(void *data)
{
    static Widget top, dialog;
    Widget lab;
    Arg args[3];

    set_wait_cursor();
    if (top == NULL) {
	char *label1[5];
	Widget but1[5];

	label1[0] = "Accept";
	label1[1] = "Files...";
	label1[2] = "Update";
	label1[3] = "Query";
	label1[4] = "Close";
	top = XmCreateDialogShell(app_shell, "netCDF", NULL, 0);
	handle_close(top);
	dialog = XmCreateRowColumn(top, "dialog_rc", NULL, 0);

	XtSetArg(args[0], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE);
	XtSetArg(args[1], XmNvisibleItemCount, 5);

	lab = CreateLabel(dialog, "Select set X:");
	netcdf_listx_item = XmCreateScrolledList(dialog, "list", args, 2);
	ManageChild(netcdf_listx_item);

	lab = CreateLabel(dialog, "Select set Y:");
	netcdf_listy_item = XmCreateScrolledList(dialog, "list", args, 2);
	ManageChild(netcdf_listy_item);

	netcdf_file_item = CreateTextItem2(dialog, 30, "netCDF file:");

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 5, but1, label1);
	XtAddCallback(but1[0], XmNactivateCallback, (XtCallbackProc) do_netcdf_proc,
		      (XtPointer) NULL);
	XtAddCallback(but1[1], XmNactivateCallback, (XtCallbackProc) create_netcdffiles_popup,
		      (XtPointer) NULL);
	XtAddCallback(but1[2], XmNactivateCallback, (XtCallbackProc) do_netcdfupdate_proc,
		      (XtPointer) NULL);
	XtAddCallback(but1[3], XmNactivateCallback, (XtCallbackProc) do_netcdfquery_proc,
		      (XtPointer) NULL);
	XtAddCallback(but1[4], XmNactivateCallback, (XtCallbackProc) destroy_dialog,
		      (XtPointer) top);

	ManageChild(dialog);
	netcdf_frame = top;
	if (strlen(netcdf_name)) {
	    xv_setstr(netcdf_file_item, netcdf_name);
	}
    }
    update_netcdfs();
    RaiseWindow(top);
    unset_wait_cursor();
}

static int do_netcdffile_proc(char *filename, void *data)
{
    xv_setstr(netcdf_file_item, filename);
    update_netcdfs();
    
    return TRUE;
}

void create_netcdffiles_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        fsb = CreateFileSelectionBox(app_shell, "Select netCDF file");
	AddFileSelectionBoxCB(fsb, do_netcdffile_proc, NULL);
        ManageChild(fsb->FSB);
    }
    
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

char *getcdf_type(nc_type datatype)
{
    switch (datatype) {
    case NC_SHORT:
	return "NC_SHORT";
	break;
    case NC_LONG:
	return "NC_LONG";
	break;
    case NC_FLOAT:
	return "NC_FLOAT";
	break;
    case NC_DOUBLE:
	return "NC_DOUBLE";
	break;
    default:
	return "UNKNOWN (can't read this)";
	break;
    }
}

static void do_netcdfquery_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    char xvar[256], yvar[256];
    char buf[256], fname[512];
    XmString *s, cs;
    int *pos_list;
    int i, pos_cnt, cnt;
    char *cstr;

    int cdfid;			/* netCDF id */
    nc_type datatype = 0;
    float f;
    double d;

    int x_id, y_id;
    nc_type xdatatype = 0;
    nc_type ydatatype = 0;
    int xndims, xdim[10], xnatts;
    int yndims, ydim[10], ynatts;
    long nx, ny;

    int atlen;
    char attname[256];
    char atcharval[256];

    ncopts = 0;			/* no crash on error */

    set_wait_cursor();

    strcpy(fname, xv_getstr(netcdf_file_item));

    if ((cdfid = ncopen(fname, NC_NOWRITE)) == -1) {
	errmsg("Can't open file.");
	unset_wait_cursor();
	return;
    }
    if (XmListGetSelectedPos(netcdf_listx_item, &pos_list, &pos_cnt)) {
	XtVaGetValues(netcdf_listx_item,
		      XmNselectedItemCount, &cnt,
		      XmNselectedItems, &s,
		      NULL);
	cs = XmStringCopy(*s);
	if ((cstr = GetStringSimple(cs))) {
	    strcpy(xvar, cstr);
	    XtFree(cstr);
	}
	XmStringFree(cs);
    } else {
	errmsg("Need to select X, either variable name or INDEX");
	goto out1;
    }
    if (XmListGetSelectedPos(netcdf_listy_item, &pos_list, &pos_cnt)) {
	XtVaGetValues(netcdf_listy_item,
		      XmNselectedItemCount, &cnt,
		      XmNselectedItems, &s,
		      NULL);
	cs = XmStringCopy(*s);
	if ((cstr = GetStringSimple(cs))) {
	    strcpy(yvar, cstr);
	    XtFree(cstr);
	}
	XmStringFree(cs);
    } else {
	errmsg("Need to select Y");
	goto out1;
    }
    if (strcmp(xvar, "INDEX") == 0) {
	stufftext("X is the index of the Y variable\n");
    } else {
	if ((x_id = ncvarid(cdfid, xvar)) == -1) {
	    char ebuf[256];
	    sprintf(ebuf, "do_query(): No such variable %s for X", xvar);
	    errmsg(ebuf);
	    goto out1;
	}
	ncvarinq(cdfid, x_id, NULL, &xdatatype, &xndims, xdim, &xnatts);
	ncdiminq(cdfid, xdim[0], NULL, &nx);
	sprintf(buf, "X is %s, data type %s \t length [%ld]\n", xvar, getcdf_type(xdatatype), nx);
	stufftext(buf);
	sprintf(buf, "\t%d Attributes:\n", xnatts);
	stufftext(buf);
	for (i = 0; i < xnatts; i++) {
	    atcharval[0] = 0;
	    ncattname(cdfid, x_id, i, attname);
	    ncattinq(cdfid, x_id, attname, &datatype, &atlen);
	    switch (datatype) {
	    case NC_CHAR:
		ncattget(cdfid, x_id, attname, (void *) atcharval);
		atcharval[atlen] = 0;
		sprintf(buf, "\t\t%s: %s\n", attname, atcharval);
		stufftext(buf);
		break;
	    case NC_FLOAT:
		ncattget(cdfid, x_id, attname, (void *) &f);
		sprintf(buf, "\t\t%s: %f\n", attname, f);
		stufftext(buf);
		break;
	    case NC_DOUBLE:
		ncattget(cdfid, x_id, attname, (void *) &d);
		sprintf(buf, "\t\t%s: %f\n", attname, d);
		stufftext(buf);
		break;
	       default:
                break;
            }
	}
    }
    if ((y_id = ncvarid(cdfid, yvar)) == -1) {
	char ebuf[256];
	sprintf(ebuf, "do_query(): No such variable %s for Y", yvar);
	errmsg(ebuf);
	goto out1;
    }
    ncvarinq(cdfid, y_id, NULL, &ydatatype, &yndims, ydim, &ynatts);
    ncdiminq(cdfid, ydim[0], NULL, &ny);
    sprintf(buf, "Y is %s, data type %s \t length [%ld]\n", yvar, getcdf_type(ydatatype), ny);
    stufftext(buf);
    sprintf(buf, "\t%d Attributes:\n", ynatts);
    stufftext(buf);
    for (i = 0; i < ynatts; i++) {
	atcharval[0] = 0;
	ncattname(cdfid, y_id, i, attname);
	ncattinq(cdfid, y_id, attname, &datatype, &atlen);
	switch (datatype) {
	case NC_CHAR:
	    ncattget(cdfid, y_id, attname, (void *) atcharval);
	    atcharval[atlen] = 0;
	    sprintf(buf, "\t\t%s: %s\n", attname, atcharval);
	    stufftext(buf);
	    break;
	case NC_FLOAT:
	    ncattget(cdfid, y_id, attname, (void *) &f);
	    sprintf(buf, "\t\t%s: %f\n", attname, f);
	    stufftext(buf);
	    break;
	case NC_DOUBLE:
	    ncattget(cdfid, y_id, attname, (void *) &d);
	    sprintf(buf, "\t\t%s: %f\n", attname, d);
	    stufftext(buf);
	    break;
          default:
            break;
	}
    }

  out1:;
    ncclose(cdfid);
    stufftext("\n");
    unset_wait_cursor();
}

#endif
