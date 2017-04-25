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
 * setwin - GUI for operations on sets and datasets
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>

#include <Xbae/Matrix.h>

#include "globals.h"
#include "graphs.h"
#include "utils.h"
#include "plotone.h"
#include "ssdata.h"
#include "parser.h"
#include "motifinc.h"
#include "protos.h"

#define cg get_cg()

static void enterCB(Widget w, XtPointer client_data, XtPointer call_data);
static void changetypeCB(int n, int *values, void *data);
static int datasetprop_aac_cb(void *data);

static int datasetop_aac_cb(void *data);
static void datasetoptypeCB(int value, void *data);
static int setop_aac_cb(void *data);

static int leval_aac_cb(void *data);

typedef struct _Type_ui {
    Widget top;
    ListStructure *sel;
    Widget comment_item;
    Widget length_item;
    OptionStructure *datatype_item;
    Widget mw;
    char *rows[MAX_SET_COLS][6];
} Type_ui;

static Type_ui tui;

void create_datasetprop_popup(void *data)
{
    set_wait_cursor();

    if (tui.top == NULL) {
        Widget menubar, menupane, submenupane, dialog, rc, fr;
        int i, j;
        char *rowlabels[MAX_SET_COLS];
        char *collabels[6] = {"Min", "at", "Max", "at", "Mean", "Stdev"};
        short column_widths[6] = {10, 6, 10, 6, 10, 10};
        unsigned char column_alignments[6];
        unsigned char column_label_alignments[6];
	tui.top = CreateDialogForm(app_shell, "Data set properties");

        menubar = CreateMenuBar(tui.top);
        ManageChild(menubar);
        AddDialogFormChild(tui.top, menubar);

	dialog = CreateVContainer(tui.top);

	tui.sel = CreateSetChoice(dialog,
            "Data sets:", LIST_TYPE_MULTIPLE, TRUE);
	AddListChoiceCB(tui.sel, changetypeCB, (void *) tui.sel);


        menupane = CreateMenu(menubar, "File", 'F', FALSE);
        CreateMenuButton(menupane,
            "Close", 'C', destroy_dialog_cb, GetParent(tui.top));

        menupane = CreateMenu(menubar, "Edit", 'E', FALSE);

        CreateMenuButton(menupane, "Duplicate", 'D',
            duplicate_set_proc, (void *) tui.sel);
        CreateMenuButton(menupane, "Kill data", 'a',
            killd_set_proc, (void *) tui.sel);
        CreateMenuSeparator(menupane);
        submenupane = CreateMenu(menupane, "Edit data", 'E', FALSE);
        CreateMenuButton(submenupane, "In spreadsheet", 's',
            editS_set_proc, (void *) tui.sel);
        CreateMenuButton(submenupane, "In text editor", 'e',
            editE_set_proc, (void *) tui.sel);
        submenupane = CreateMenu(menupane, "Create new", 'n', FALSE);
        CreateMenuButton(submenupane, "By formula", 'f',
            newF_set_proc, (void *) tui.sel);
        CreateMenuButton(submenupane, "In spreadsheet", 's',
            newS_set_proc, (void *) tui.sel);
        CreateMenuButton(submenupane, "In text editor", 'e',
            newE_set_proc, (void *) tui.sel);
        CreateMenuButton(submenupane, "From block data", 'b',
            newB_set_proc, (void *) tui.sel);
        CreateMenuSeparator(menupane);
        CreateMenuButton(menupane, "Set appearance...", 'S',
            define_symbols_popup, (void *) -1);
        CreateMenuButton(menupane, "Set operations...", 'o',
            create_setop_popup, NULL);
 
        menupane = CreateMenu(menubar, "Help", 'H', TRUE);
        CreateMenuHelpButton(menupane, "On data sets", 's',
            tui.top, "doc/UsersGuide.html#data-sets");

	rc = CreateHContainer(dialog);
	tui.datatype_item = CreateSetTypeChoice(rc, "Type:");
	tui.length_item = CreateTextItem2(rc, 6, "Length:");
	tui.comment_item = CreateTextItem2(dialog, 26, "Comment:");

        for (i = 0; i < 6; i++) {
            column_alignments[i] = XmALIGNMENT_END;
            column_label_alignments[i] = XmALIGNMENT_CENTER;
        }
        
        for (i = 0; i < MAX_SET_COLS; i++) {
            rowlabels[i] = copy_string(NULL, dataset_colname(i));
            for (j = 0; j < 6; j++) {
                tui.rows[i][j] = NULL;
            }
        }

	fr = CreateFrame(dialog, "Statistics");
        tui.mw = XtVaCreateManagedWidget("mw",
            xbaeMatrixWidgetClass, fr,
            XmNrows, MAX_SET_COLS,
            XmNcolumns, 6,
            XmNvisibleRows, MAX_SET_COLS,
            XmNvisibleColumns, 4,
            XmNcolumnLabels, collabels,
            XmNcolumnWidths, column_widths,
            XmNcolumnAlignments, column_alignments,
            XmNcolumnLabelAlignments, column_label_alignments,
            XmNrowLabels, rowlabels,
	    XmNrowLabelWidth, 3,
            XmNrowLabelAlignment, XmALIGNMENT_CENTER,
            XmNshowArrows, True,
            XmNallowColumnResize, True,
            XmNgridType, XmGRID_COLUMN_SHADOW,
            XmNcellShadowType, XmSHADOW_OUT,
            XmNcellShadowThickness, 1,
            XmNaltRowCount, 1,
            XmNtraversalOn, False,
            NULL);

        XtAddCallback(tui.mw, XmNenterCellCallback, enterCB, NULL);	

        CreateAACDialog(tui.top, dialog, datasetprop_aac_cb, NULL);
    }
    
    RaiseWindow(GetParent(tui.top));
    unset_wait_cursor();
}

static void changetypeCB(int n, int *values, void *data)
{
    int i, j, ncols;
    double *datap;
    int imin, imax;
    double dmin, dmax, dmean, dsd;
    ListStructure *listp;
    SetChoiceData *sdata;
    int gno, setno;
    char buf[32];
    char **cells[MAX_SET_COLS];
    
    listp = (ListStructure *) data;
    if (listp == NULL) {
        return;
    }
    
    sdata = (SetChoiceData *) listp->anydata;
    gno = sdata->gno;
    
    if (n == 1 && is_valid_setno(gno, setno = values[0]) == TRUE) {
	ncols = dataset_cols(gno, setno);
        xv_setstr(tui.comment_item, getcomment(gno, setno));
	sprintf(buf, "%d", getsetlength(gno, setno));
        xv_setstr(tui.length_item, buf);
        SetOptionChoice(tui.datatype_item, dataset_type(gno, setno));
        SetSensitive(tui.datatype_item->menu, TRUE);
    } else {
	setno = -1;
        ncols = 0;
        xv_setstr(tui.comment_item, "");
        xv_setstr(tui.length_item, "");
        SetSensitive(tui.datatype_item->menu, FALSE);
    }
    for (i = 0; i < MAX_SET_COLS; i++) {
        datap = getcol(gno, setno, i);
	minmax(datap, getsetlength(gno, setno), &dmin, &dmax, &imin, &imax);
	stasum(datap, getsetlength(gno, setno), &dmean, &dsd);
        for (j = 0; j < 6; j++) {
            if (i < ncols) {
                switch (j) {
                case 0:
                    sprintf(buf, "%g", dmin);
                    break;
                case 1:
                    sprintf(buf, "%d", imin);
                    break;
                case 2:
                    sprintf(buf, "%g", dmax);
                    break;
                case 3:
                    sprintf(buf, "%d", imax);
                    break;
                case 4:
                    sprintf(buf, "%g", dmean);
                    break;
                case 5:
                    sprintf(buf, "%g", dsd);
                    break;
                default:
                    strcpy(buf, "");
                    break;
                }
                tui.rows[i][j] = copy_string(tui.rows[i][j], buf);
            } else {
                tui.rows[i][j] = copy_string(tui.rows[i][j], "");
            }
        }
        cells[i] = &tui.rows[i][0];
    }
    XtVaSetValues(tui.mw, XmNcells, cells, NULL);
}

static void enterCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XbaeMatrixEnterCellCallbackStruct *cbs =
        (XbaeMatrixEnterCellCallbackStruct *) call_data;
    
    cbs->doit = False;
    cbs->map  = False;
}

/*
 * change dataset properties
 */
static int datasetprop_aac_cb(void *data)
{
    int error = FALSE;
    int *selset, nsets, i, len, setno, type;
    char *s;
    
    nsets = GetListChoices(tui.sel, &selset);
    
    if (nsets < 1) {
        errmsg("No set selected");
        return RETURN_FAILURE;
    } else {
        type = GetOptionChoice(tui.datatype_item);
        xv_evalexpri(tui.length_item, &len);
        if (len < 0) {
            errmsg("Negative set length!");
            error = TRUE;
        }
        s = xv_getstr(tui.comment_item);
        
 
        if (error == FALSE) {
            for (i = 0; i < nsets; i++) {
                setno = selset[i];
                set_dataset_type(cg, setno, type);
                setlength(cg, setno, len);
                setcomment(cg, setno, s);
            }
        }
 
        xfree(selset);

        if (error == FALSE) {
            update_set_lists(cg);
            xdrawgraph();
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    }
}


typedef enum {
    DATASETOP_SORT,
    DATASETOP_REVERSE,
    DATASETOP_JOIN,
    DATASETOP_SPLIT,
    DATASETOP_DROP
}dataSetOpType;

typedef struct _Datasetop_ui {
    Widget top;
    ListStructure *sel;
    OptionStructure *optype_item;
    Widget *xy_item;
    Widget *up_down_item;
    Widget length_item;
    Widget start_item;
    Widget stop_item;
} Datasetop_ui;

static Datasetop_ui datasetopui;

static Widget datasettype_controls[5];

void create_datasetop_popup(void *data)
{
    Widget dialog, menubar, menupane, rc;
    OptionItem optype_items[5];

    set_wait_cursor();
    if (datasetopui.top == NULL) {
        optype_items[0].value = DATASETOP_SORT;
        optype_items[0].label = "Sort";
        optype_items[1].value = DATASETOP_REVERSE;
        optype_items[1].label = "Reverse";
        optype_items[2].value = DATASETOP_JOIN;
        optype_items[2].label = "Join";
        optype_items[3].value = DATASETOP_SPLIT;
        optype_items[3].label = "Split";
        optype_items[4].value = DATASETOP_DROP;
        optype_items[4].label = "Drop points";
        
	datasetopui.top = CreateDialogForm(app_shell, "Data set operations");
        SetDialogFormResizable(datasetopui.top, TRUE);

        menubar = CreateMenuBar(datasetopui.top);
        ManageChild(menubar);
        AddDialogFormChild(datasetopui.top, menubar);
        

	dialog = CreateVContainer(datasetopui.top);
        XtVaSetValues(dialog, XmNrecomputeSize, True, NULL);

	datasetopui.sel = CreateSetChoice(dialog,
            "Data sets:", LIST_TYPE_MULTIPLE, TRUE);

        menupane = CreateMenu(menubar, "File", 'F', FALSE);
        CreateMenuButton(menupane,
            "Close", 'C', destroy_dialog_cb, GetParent(datasetopui.top));

        menupane = CreateMenu(menubar, "Help", 'H', TRUE);
        CreateMenuHelpButton(menupane, "On dataset operations", 's',
            datasetopui.top, "doc/UsersGuide.html#data-set-operations");

	datasetopui.optype_item = CreateOptionChoice(dialog,
						"Operation type:",
						1, 5, optype_items);
   	AddOptionChoiceCB(datasetopui.optype_item, datasetoptypeCB, NULL);

	rc = CreateHContainer(dialog);
        XtVaSetValues(rc, XmNrecomputeSize, True, NULL);
	
        datasetopui.xy_item = CreatePanelChoice(rc,
					   "Sort on:",
					   7,
					   "X",
					   "Y",
					   "Y1",
					   "Y2",
					   "Y3",
					   "Y4",
					   NULL);
	datasetopui.up_down_item = CreatePanelChoice(rc,
						"Order:",
						3,
						"Ascending",
						"Descending",
                                                NULL);
        datasettype_controls[0] = rc;

	/* Reverse */
        rc = CreateVContainer(dialog);
        CreateSeparator(rc);
        datasettype_controls[1] = rc;

	/* Join */
	rc = CreateVContainer(dialog);
        CreateSeparator(rc);
        datasettype_controls[2] = rc;

	/* Split */
	rc = CreateVContainer(dialog);
        datasetopui.length_item = CreateTextItem2(rc, 6, "Length:");
        datasettype_controls[3] = rc;

	/* Drop points */
	rc = CreateHContainer(dialog);
        datasetopui.start_item = CreateTextItem2(rc, 6, "Start at:");
        datasetopui.stop_item  = CreateTextItem2(rc, 6, "Stop at:");
        datasettype_controls[4] = rc;

	ManageChild(datasettype_controls[0]);
	UnmanageChild(datasettype_controls[1]);
	UnmanageChild(datasettype_controls[2]);
	UnmanageChild(datasettype_controls[3]);
	UnmanageChild(datasettype_controls[4]);

        CreateAACDialog(datasetopui.top, dialog, datasetop_aac_cb, NULL);
    }
    
    RaiseWindow(GetParent(datasetopui.top));
    
    unset_wait_cursor();
}

static void datasetoptypeCB(int value, void *data)
{
    int i;
    dataSetOpType type = value;
    
    for (i = 0; i < 5; i++) {
        if (i == type) {
            ManageChild(datasettype_controls[i]);
        } else {
            UnmanageChild(datasettype_controls[i]);
        }
    }
}

static int datasetop_aac_cb(void *data)
{
    int *selset, nsets, i, setno;
    int sorton, stype;
    int lpart;
    int startno, endno;
    static int son[MAX_SET_COLS] = {DATA_X, DATA_Y, DATA_Y1, DATA_Y2, DATA_Y3, DATA_Y4};
    dataSetOpType optype;
       
    nsets = GetListChoices(datasetopui.sel, &selset);
    if (nsets < 1) {
        errmsg("No set selected");
        return RETURN_FAILURE;
    } else {
        optype = GetOptionChoice(datasetopui.optype_item);
 
        switch (optype) {
        case DATASETOP_SORT:
            sorton = son[GetChoice(datasetopui.xy_item)];
            stype = GetChoice(datasetopui.up_down_item);

            for (i = 0; i < nsets; i++) {
                setno = selset[i];
	        do_sort(setno, sorton, stype);
            }
            break;
        case DATASETOP_REVERSE:
            for (i = 0; i < nsets; i++) {
                setno = selset[i];
	        reverse_set(cg, setno);
            }
            break;
        case DATASETOP_JOIN:
            join_sets(cg, selset, nsets);
            break;
        case DATASETOP_SPLIT:
            xv_evalexpri(datasetopui.length_item, &lpart);
            for (i = 0; i < nsets; i++) {
                setno = selset[i];
                do_splitsets(cg, setno, lpart);
            }
            break;
        case DATASETOP_DROP:
            xv_evalexpri(datasetopui.start_item, &startno);
            xv_evalexpri(datasetopui.stop_item, &endno);
            for (i = 0; i < nsets; i++) {
                setno = selset[i];
		do_drop_points(cg, setno, startno, endno);
            }
            break;
        }
        
        xfree(selset);

        update_set_lists(cg);
        xdrawgraph();
        
        return RETURN_SUCCESS;
    }
}


typedef struct _Setop_ui {
    Widget top;
    SrcDestStructure *srcdest;
    OptionStructure *optype_item;
} Setop_ui;

static Setop_ui setopui;

#define OPTYPE_COPY 0
#define OPTYPE_MOVE 1
#define OPTYPE_SWAP 2

void create_setop_popup(void *data)
{
    set_wait_cursor();
    
    if (setopui.top == NULL) {
        OptionItem opitems[3];

	setopui.top = CreateDialogForm(app_shell, "Set operations");

        setopui.srcdest =
            CreateSrcDestSelector(setopui.top, LIST_TYPE_MULTIPLE);
        AddDialogFormChild(setopui.top, setopui.srcdest->form);
        
        opitems[0].value = OPTYPE_COPY;
        opitems[0].label = "Copy";
        opitems[1].value = OPTYPE_MOVE;
        opitems[1].label = "Move";
        opitems[2].value = OPTYPE_SWAP;
        opitems[2].label = "Swap";
        setopui.optype_item = CreateOptionChoice(setopui.top,
            "Type of operation:", 0, 3, opitems);
        
        CreateAACDialog(setopui.top,
            setopui.optype_item->menu, setop_aac_cb, NULL);
    }
    
    RaiseWindow(GetParent(setopui.top));
    
    unset_wait_cursor();
}

static int setop_aac_cb(void *data)
{
    int optype, error;
    int i, g1_ok, g2_ok, ns1, ns2, *svalues1, *svalues2, gno1, gno2, setno2;

    optype = GetOptionChoice(setopui.optype_item);
    
    g1_ok = GetSingleListChoice(setopui.srcdest->src->graph_sel, &gno1);
    g2_ok = GetSingleListChoice(setopui.srcdest->dest->graph_sel, &gno2);
    ns1 = GetListChoices(setopui.srcdest->src->set_sel, &svalues1);
    ns2 = GetListChoices(setopui.srcdest->dest->set_sel, &svalues2);
    
    error = FALSE;
    if (g1_ok == RETURN_FAILURE || g2_ok == RETURN_FAILURE) {
        error = TRUE;
        errmsg("Please select single source and destination graphs");
    } else if (ns1 == 0) {
        error = TRUE;
        errmsg("No source sets selected");
    } else if (ns2 == 0 && optype == OPTYPE_SWAP) {
        error = TRUE;
        errmsg("No destination sets selected");
    } else if (ns1 != ns2 && (optype == OPTYPE_SWAP || ns2 != 0)) {
        error = TRUE;
        errmsg("Different number of source and destination sets");
    } else if (gno1 == gno2 && ns2 == 0 && optype == OPTYPE_MOVE) {
        error = TRUE;
        errmsg("Can't move a set to itself");
    } else {
        for (i = 0; i < ns1; i++) {
            switch (optype) {
            case OPTYPE_SWAP:
                if (do_swapset(gno1, svalues1[i], gno2, svalues2[i])
                                                != RETURN_SUCCESS) {
                    error = TRUE;
                }
                break;
            case OPTYPE_COPY:
                if (ns2 == 0) {
                    setno2 = nextset(gno2);
                } else {
                    setno2 = svalues2[i];
                }
                if (do_copyset(gno1, svalues1[i], gno2, setno2)
                                                != RETURN_SUCCESS) {
                    error = TRUE;
                }
                break;
            case OPTYPE_MOVE:
                if (ns2 == 0) {
                    setno2 = nextset(gno2);
                } else {
                    setno2 = svalues2[i];
                }
                if (do_moveset(gno1, svalues1[i], gno2, setno2)
                                                != RETURN_SUCCESS) {
                    error = TRUE;
                }
                break;
            }
        }
    }
    
    if (ns1 > 0) {
        xfree(svalues1);
    }
    if (ns2 > 0) {
        xfree(svalues2);
    }
    
    if (error == FALSE) {
        update_all();
        xdrawgraph();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

typedef struct _Leval_ui {
    Widget top;
    OptionStructure *set_type;
    Widget start;
    Widget stop;
    Widget npts;
    Widget mw;
    int gno;
} Leval_ui;

void set_type_cb(int type, void *data)
{
    int i, nmrows, nscols;
    char *rowlabels[MAX_SET_COLS];
    Leval_ui *ui = (Leval_ui *) data;
    
    nmrows = XbaeMatrixNumRows(ui->mw);
    nscols = settype_cols(type);
    
    if (nmrows > nscols) {
        XbaeMatrixDeleteRows(ui->mw, nscols, nmrows - nscols);
    } else if (nmrows < nscols) {
	for (i = nmrows; i < nscols; i++) {
            rowlabels[i - nmrows] = copy_string(NULL, dataset_colname(i));
            rowlabels[i - nmrows] = concat_strings(rowlabels[i - nmrows], " = ");
        }
        XbaeMatrixAddRows(ui->mw, nmrows, NULL, rowlabels, NULL, nscols - nmrows);
    }
}

static Leval_ui levalui;

static void leaveCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Leval_ui *ui = (Leval_ui *) client_data;
    
    XbaeMatrixLeaveCellCallbackStruct *cs =
    	    (XbaeMatrixLeaveCellCallbackStruct *) call_data;

    XbaeMatrixSetCell(ui->mw, cs->row, cs->column, cs->value);
}

void create_leval_frame(void *data)
{
    int gno = (int) data;

    set_wait_cursor();

    if (is_valid_gno(gno)) {
        levalui.gno = gno;
    } else {
        levalui.gno = get_cg();
    }

    if (levalui.top == NULL) {
        int i;
        Widget fr, rc1;
        int nscols;
        char *rows[MAX_SET_COLS][1];
        char **cells[MAX_SET_COLS];
        char *rowlabels[MAX_SET_COLS];
        short column_widths[1] = {50};
        int column_maxlengths[1] = {256};

	levalui.top = CreateDialogForm(app_shell, "Load & evaluate");

	fr = CreateFrame(levalui.top, "Parameter mesh ($t)");
        AddDialogFormChild(levalui.top, fr);
        rc1 = CreateHContainer(fr);
	levalui.start = CreateTextItem2(rc1, 10, "Start at:");
	levalui.stop = CreateTextItem2(rc1, 10, "Stop at:");
	levalui.npts = CreateTextItem2(rc1, 6, "Length:");

	levalui.set_type = CreateSetTypeChoice(levalui.top, "Set type:");
        AddDialogFormChild(levalui.top, levalui.set_type->menu);
        AddOptionChoiceCB(levalui.set_type, set_type_cb, (void *) &levalui);
	
        nscols = settype_cols(curtype);
	for (i = 0; i < nscols; i++) {
            rowlabels[i] = copy_string(NULL, dataset_colname(i));
            rowlabels[i] = concat_strings(rowlabels[i], " = ");
            if (i == 0) {
                rows[i][0] = "$t";
            } else {
                rows[i][0] = "";
            }
            cells[i] = &rows[i][0];
        }

        levalui.mw = XtVaCreateManagedWidget("mw",
            xbaeMatrixWidgetClass, levalui.top,
            XmNrows, nscols,
            XmNcolumns, 1,
            XmNvisibleRows, MAX_SET_COLS,
            XmNvisibleColumns, 1,
            XmNcolumnWidths, column_widths,
            XmNcolumnMaxLengths, column_maxlengths,
            XmNrowLabels, rowlabels,
	    XmNrowLabelWidth, 6,
            XmNrowLabelAlignment, XmALIGNMENT_CENTER,
            XmNcells, cells,
            XmNgridType, XmGRID_CELL_SHADOW,
            XmNcellShadowType, XmSHADOW_ETCHED_OUT,
            XmNcellShadowThickness, 2,
            XmNaltRowCount, 0,
            XmNallowColumnResize, True,
            NULL);

        XtAddCallback(levalui.mw, XmNleaveCellCallback, leaveCB, &levalui);
        
        CreateAACDialog(levalui.top, levalui.mw, leval_aac_cb, NULL);
    }
    
    RaiseWindow(GetParent(levalui.top));
    unset_wait_cursor();
}

static int leval_aac_cb(void *data)
{
    int i, nscols, type;
    double start, stop;
    int npts;
    char *formula[MAX_SET_COLS];
    int res;
    int setno, gno;
    grarr *t;
    
    gno = levalui.gno;
    type = GetOptionChoice(levalui.set_type);
    nscols = settype_cols(type);

    if (xv_evalexpr(levalui.start, &start) != RETURN_SUCCESS) {
	errmsg("Start item undefined");
        return RETURN_FAILURE;
    }

    if (xv_evalexpr(levalui.stop, &stop) != RETURN_SUCCESS) {
	errmsg("Stop item undefined");
        return RETURN_FAILURE;
    }

    if (xv_evalexpri(levalui.npts, &npts) != RETURN_SUCCESS) {
	errmsg("Number of points undefined");
        return RETURN_FAILURE;
    }

    XbaeMatrixCommitEdit(levalui.mw, False);
    for (i = 0; i < nscols; i++) {
        formula[i] = XbaeMatrixGetCell(levalui.mw, i, 0);
    }
    
    t = get_parser_arr_by_name("$t");
    if (t == NULL) {
        t = define_parser_arr("$t");
        if (t == NULL) {
	    errmsg("Internal error");
            return RETURN_FAILURE;
        }
    }
    
    if (t->length != 0) {
        xfree(t->data);
        t->length = 0;
    }
    t->data = allocate_mesh(start, stop, npts);
    if (t->data == NULL) {
        return RETURN_FAILURE;
    }
    t->length = npts;
    
    setno = nextset(gno);
    set_dataset_type(gno, setno, type);
    set_set_hidden(gno, setno, FALSE);
    if (setlength(gno, setno, npts) != RETURN_SUCCESS) {
        killset(gno, setno);
        XCFREE(t->data);
        t->length = 0;
        return RETURN_FAILURE;
    }
    
    set_parser_setno(gno, setno);

    for (i = 0; i < nscols; i++) {
        char buf[32], *expr;
        
        /* preparing the expression */
        sprintf(buf, "GRAPH[%d].SET[%d].%s = ", gno, setno, dataset_colname(i));
        expr = copy_string(NULL, buf);
        expr = concat_strings(expr, formula[i]);
        
        /* evaluate the expression */
        res = scanner(expr);
        
        xfree(expr);
        
        if (res != RETURN_SUCCESS) {
            killset(gno, setno);
            
            XCFREE(t->data);
            t->length = 0;
            
            return RETURN_FAILURE;
        }
    }
    
    XCFREE(t->data);
    t->length = 0;
    
    update_set_lists(gno);
    xdrawgraph();
    
    return RETURN_SUCCESS;
}
