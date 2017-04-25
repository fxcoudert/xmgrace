/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2001 Grace Development Team
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
 * transformations, curve fitting, etc.
 *
 * formerly, this was all one big popup, now it is several.
 * All are created as needed
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>

#include "globals.h"
#include "graphs.h"
#include "utils.h"
#include "ssdata.h"
#include "motifinc.h"
#include "protos.h"

static Widget but1[3];
static Widget but2[3];

static void compute_aac(void *data);
static void do_digfilter_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_linearc_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_xcor_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_int_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_differ_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_seasonal_proc(Widget w, XtPointer client_data, XtPointer call_data);
static int do_interp_proc(void *data);
static void do_regress_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_runavg_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_fourier_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_fft_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_window_proc(Widget w, XtPointer client_data, XtPointer call_data);
static int do_histo_proc(void *data);
static void do_sample_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void do_prune_toggle(Widget w, XtPointer client_data, XtPointer call_data);
static void do_prune_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void set_regr_sensitivity(Widget , XtPointer , XtPointer );

typedef struct _Eval_ui {
    Widget top;
    SrcDestStructure *srcdest;
    Widget formula_item;
    RestrictionStructure *restr_item;
} Eval_ui;

static Eval_ui eui;

void create_eval_frame(void *data)
{
    set_wait_cursor();
    if (eui.top == NULL) {
        Widget dialog, rc_trans, fr;

	eui.top = XmCreateDialogShell(app_shell, "evaluateExpression", NULL, 0);
        XtVaSetValues(eui.top, XmNallowShellResize, True, NULL);
	handle_close(eui.top);
        dialog = XtVaCreateWidget("dialog",
            xmFormWidgetClass, eui.top,
            XmNresizePolicy, XmRESIZE_ANY,
            NULL);


        eui.srcdest = CreateSrcDestSelector(dialog, LIST_TYPE_MULTIPLE);

        XtVaSetValues(eui.srcdest->form,
            XmNtopAttachment, XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            NULL);

	rc_trans = XtVaCreateWidget("rc",
            xmRowColumnWidgetClass, dialog,
            XmNrecomputeSize, True,
            NULL);

	CreateSeparator(rc_trans);
	eui.formula_item = CreateScrollTextItem2(rc_trans, 3, "Formula:");

        eui.restr_item =
            CreateRestrictionChoice(rc_trans, "Source data filtering");

        ManageChild(rc_trans);
	fr = CreateFrame(dialog, NULL);
        XtVaSetValues(fr,
            XmNtopAttachment, XmATTACH_NONE,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_FORM,
            NULL);
        CreateAACButtons(fr, dialog, compute_aac);

        XtVaSetValues(rc_trans,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, eui.srcdest->form,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_WIDGET,
            XmNbottomWidget, fr,
            NULL);


	ManageChild(dialog);
    }
    RaiseWindow(eui.top);
    unset_wait_cursor();
}

/*
 * evaluate a formula
 */
static void compute_aac(void *data)
{
    int aac_mode, error, resno;
    int i, g1_ok, g2_ok, ns1, ns2, *svalues1, *svalues2,
        gno1, gno2, setno1, setno2;
    char fstr[256];
    int restr_type, restr_negate;
    char *rarray;

    aac_mode = (int) data;
    
    if (aac_mode == AAC_CLOSE) {
        UnmanageChild(eui.top);
        return;
    }

    set_wait_cursor();
    
    restr_type = GetOptionChoice(eui.restr_item->r_sel);
    restr_negate = GetToggleButtonState(eui.restr_item->negate);
    
    g1_ok = GetSingleListChoice(eui.srcdest->src->graph_sel, &gno1);
    g2_ok = GetSingleListChoice(eui.srcdest->dest->graph_sel, &gno2);
    ns1 = GetListChoices(eui.srcdest->src->set_sel, &svalues1);
    ns2 = GetListChoices(eui.srcdest->dest->set_sel, &svalues2);
    
    error = FALSE;
    if (g1_ok == RETURN_FAILURE || g2_ok == RETURN_FAILURE) {
        error = TRUE;
        errmsg("Please select single source and destination graphs");
    } else if (ns1 == 0) {
        error = TRUE;
        errmsg("No source sets selected");
    } else if (ns1 != ns2 && ns2 != 0) {
        error = TRUE;
        errmsg("Different number of source and destination sets");
    } else {
        strcpy(fstr, xv_getstr(eui.formula_item));
        for (i = 0; i < ns1; i++) {
	    setno1 = svalues1[i];
	    if (ns2 != 0) {
                setno2 = svalues2[i];
            } else {
                setno2 = nextset(gno2);
                set_set_hidden(gno2, setno2, FALSE);
            }
	    
            resno = get_restriction_array(gno1, setno1,
                restr_type, restr_negate, &rarray);
	    if (resno != RETURN_SUCCESS) {
	        errmsg("Error in evaluation restriction");
	        break;
	    }
            
            resno = do_compute(gno1, setno1, gno2, setno2, rarray, fstr);
	    XCFREE(rarray);
	    if (resno != RETURN_SUCCESS) {
	        errmsg("Error in do_compute(), check formula");
                break;
	    }
        }
    }
    
    if (aac_mode == AAC_ACCEPT && error == FALSE) {
        UnmanageChild(eui.top);
    }

    if (ns1 > 0) {
        xfree(svalues1);
    }
    if (ns2 > 0) {
        xfree(svalues2);
    }
    if (error == FALSE) {
        if (gno1 != gno2) {
            update_set_lists(gno1);
            update_set_lists(gno2);
        } else {
            update_set_lists(gno1);
        }
        xdrawgraph();
    }
    unset_wait_cursor();
}


#define SAMPLING_MESH   0
#define SAMPLING_SET    1

/* interpolation */

typedef struct _Interp_ui {
    TransformStructure *tdialog;
    OptionStructure *method;
    OptionStructure *sampling;
    Widget strict;
    Widget mrc;
    Widget mstart;
    Widget mstop;
    Widget mlength;
    ListStructure *sset_sel;
} Interp_ui;

static Interp_ui *interpui = NULL;

static void sampling_cb(int value, void *data)
{
    Interp_ui *ui = (Interp_ui *) data;
    
    if (value == SAMPLING_MESH) {
        SetSensitive(ui->mrc, True);
        SetSensitive(ui->sset_sel->list, False);
    } else {
        SetSensitive(ui->mrc, False);
        SetSensitive(ui->sset_sel->list, True);
    }
}

void create_interp_frame(void *data)
{
    set_wait_cursor();

    if (interpui == NULL) {
        Widget fr, rc, rc2;
        OptionItem opitems[3];
        
        interpui = xmalloc(sizeof(Interp_ui));
        interpui->tdialog = CreateTransformDialogForm(app_shell,
            "Interpolation", LIST_TYPE_MULTIPLE);
        fr = CreateFrame(interpui->tdialog->form, NULL);
        rc = CreateVContainer(fr);
        
        rc2 = CreateHContainer(rc);
        opitems[0].value = INTERP_LINEAR;
        opitems[0].label = "Linear";
        opitems[1].value = INTERP_SPLINE;
        opitems[1].label = "Cubic spline";
        opitems[2].value = INTERP_ASPLINE;
        opitems[2].label = "Akima spline";
        interpui->method = CreateOptionChoice(rc2, "Method:", 0, 3, opitems);
        
        interpui->strict =
            CreateToggleButton(rc2, "Strict (within source set bounds)");
        
        CreateSeparator(rc);
        
        opitems[0].value = SAMPLING_MESH;
        opitems[0].label = "Linear mesh";
        opitems[1].value = SAMPLING_SET;
        opitems[1].label = "Abscissas of another set";
        interpui->sampling = CreateOptionChoice(rc, "Sampling:", 0, 2, opitems);
        AddOptionChoiceCB(interpui->sampling, sampling_cb, interpui);

        interpui->mrc = CreateHContainer(rc);
	interpui->mstart  = CreateTextItem2(interpui->mrc, 10, "Start at:");
	interpui->mstop   = CreateTextItem2(interpui->mrc, 10, "Stop at:");
	interpui->mlength = CreateTextItem2(interpui->mrc, 6, "Length:");
        
        interpui->sset_sel = CreateSetChoice(rc,
            "Sampling set", LIST_TYPE_SINGLE, TRUE);
        SetSensitive(interpui->sset_sel->list, False);
        
        CreateAACDialog(interpui->tdialog->form, fr, do_interp_proc, interpui);
    }
    
    RaiseWindow(GetParent(interpui->tdialog->form));
    unset_wait_cursor();
}


static int do_interp_proc(void *data)
{
    int error, res;
    int nssrc, nsdest, *svaluessrc, *svaluesdest, gsrc, gdest;
    int method, sampling, strict;
    int i, meshlen;
    double *mesh = NULL;
    Interp_ui *ui = (Interp_ui *) data;

    res = GetTransformDialogSettings(ui->tdialog, TRUE,
        &gsrc, &gdest,
        &nssrc, &svaluessrc, &nsdest, &svaluesdest);
    
    if (res != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }

    error = FALSE;
    
    method = GetOptionChoice(ui->method);
    sampling = GetOptionChoice(ui->sampling);
    strict = GetToggleButtonState(ui->strict);

    if (sampling == SAMPLING_SET) {
        int gsampl, setnosampl;
        gsampl = get_cg();
        res = GetSingleListChoice(ui->sset_sel, &setnosampl);
        if (res != RETURN_SUCCESS) {
            errmsg("Please select single sampling set");
            error = TRUE;
        } else {
            meshlen = getsetlength(gsampl, setnosampl);
            mesh = getcol(gsampl, setnosampl, DATA_X);
        }
    } else {
        double start, stop;
        if (xv_evalexpr(ui->mstart, &start)     != RETURN_SUCCESS ||
            xv_evalexpr(ui->mstop,  &stop)      != RETURN_SUCCESS ||
            xv_evalexpri(ui->mlength, &meshlen) != RETURN_SUCCESS ) {
             errmsg("Can't parse mesh settings");
             error = TRUE;
        } else {
            mesh = allocate_mesh(start, stop, meshlen);
            if (mesh == NULL) {
	        errmsg("Can't allocate mesh");
                error = TRUE;
            }
        }
    }
    
    if (error) {
        xfree(svaluessrc);
        if (nsdest > 0) {
            xfree(svaluesdest);
        }
        return RETURN_FAILURE;
    }

    for (i = 0; i < nssrc; i++) {
	int setnosrc, setnodest;
        setnosrc = svaluessrc[i];
	if (nsdest != 0) {
            setnodest = svaluesdest[i];
        } else {
            setnodest = SET_SELECT_NEXT;
        }
        
        res = do_interp(gsrc, setnosrc, gdest, setnodest,
            mesh, meshlen, method, strict);
	
        if (res != RETURN_SUCCESS) {
	    errmsg("Error in do_interp()");
	    error = TRUE;
            break;
	}
    }
    
    xfree(svaluessrc);
    if (nsdest > 0) {
        xfree(svaluesdest);
    }
    if (sampling == SAMPLING_MESH) {
        xfree(mesh);
    }

    update_set_lists(gdest);
    xdrawgraph();
    
    if (error) {
        return RETURN_FAILURE;
    } else {
        return RETURN_SUCCESS;
    }
}


/* histograms */

typedef struct _Histo_ui {
    TransformStructure *tdialog;
    Widget cumulative;
    Widget normalize;
    OptionStructure *sampling;
    Widget mrc;
    Widget mstart;
    Widget mstop;
    Widget mlength;
    ListStructure *sset_sel;
} Histo_ui;

static Histo_ui *histoui = NULL;

static void binsampling_cb(int value, void *data)
{
    Histo_ui *ui = (Histo_ui *) data;
    
    if (value == SAMPLING_MESH) {
        SetSensitive(ui->mrc, True);
        SetSensitive(ui->sset_sel->list, False);
    } else {
        SetSensitive(ui->mrc, False);
        SetSensitive(ui->sset_sel->list, True);
    }
}

void create_histo_frame(void *data)
{
    set_wait_cursor();

    if (histoui == NULL) {
        Widget fr, rc, rc2;
        OptionItem opitems[2];
        
        histoui = xmalloc(sizeof(Histo_ui));
        histoui->tdialog = CreateTransformDialogForm(app_shell,
            "Histograms", LIST_TYPE_MULTIPLE);
        fr = CreateFrame(histoui->tdialog->form, NULL);
        rc = CreateVContainer(fr);
        
        rc2 = CreateHContainer(rc);
        histoui->cumulative = CreateToggleButton(rc2, "Cumulative histogram");
        histoui->normalize = CreateToggleButton(rc2, "Normalize");
        
        CreateSeparator(rc);
        
        opitems[0].value = SAMPLING_MESH;
        opitems[0].label = "Linear mesh";
        opitems[1].value = SAMPLING_SET;
        opitems[1].label = "Abscissas of another set";
        histoui->sampling = CreateOptionChoice(rc, "Bin sampling:", 0, 2, opitems);
        AddOptionChoiceCB(histoui->sampling, binsampling_cb, histoui);

        histoui->mrc = CreateHContainer(rc);
	histoui->mstart  = CreateTextItem2(histoui->mrc, 10, "Start at:");
	histoui->mstop   = CreateTextItem2(histoui->mrc, 10, "Stop at:");
	histoui->mlength = CreateTextItem2(histoui->mrc, 6, "# of bins");
        
        histoui->sset_sel = CreateSetChoice(rc,
            "Sampling set", LIST_TYPE_SINGLE, TRUE);
        SetSensitive(histoui->sset_sel->list, False);
        
        CreateAACDialog(histoui->tdialog->form, fr, do_histo_proc, histoui);
    }
    
    RaiseWindow(GetParent(histoui->tdialog->form));
    unset_wait_cursor();
}


static int do_histo_proc(void *data)
{
    int error, res;
    int nssrc, nsdest, *svaluessrc, *svaluesdest, gsrc, gdest;
    int cumulative, normalize, sampling;
    int i, nbins;
    double *bins = NULL;
    Histo_ui *ui = (Histo_ui *) data;

    res = GetTransformDialogSettings(ui->tdialog, TRUE,
        &gsrc, &gdest,
        &nssrc, &svaluessrc, &nsdest, &svaluesdest);
    
    if (res != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }

    error = FALSE;
    
    cumulative = GetToggleButtonState(ui->cumulative);
    normalize  = GetToggleButtonState(ui->normalize);
    sampling   = GetOptionChoice(ui->sampling);

    if (sampling == SAMPLING_SET) {
        int gsampl, setnosampl;
        gsampl = get_cg();
        res = GetSingleListChoice(ui->sset_sel, &setnosampl);
        if (res != RETURN_SUCCESS) {
            errmsg("Please select single sampling set");
            error = TRUE;
        } else {
            nbins = getsetlength(gsampl, setnosampl) - 1;
            bins = getcol(gsampl, setnosampl, DATA_X);
        }
    } else {
        double start, stop;
        if (xv_evalexpr(ui->mstart, &start)   != RETURN_SUCCESS ||
            xv_evalexpr(ui->mstop,  &stop)    != RETURN_SUCCESS ||
            xv_evalexpri(ui->mlength, &nbins) != RETURN_SUCCESS ){
            errmsg("Can't parse mesh settings");
            error = TRUE;
        } else {
            bins = allocate_mesh(start, stop, nbins + 1);
            if (bins == NULL) {
	        errmsg("Can't allocate mesh");
                error = TRUE;
            }
        }
    }
    
    if (error) {
        xfree(svaluessrc);
        if (nsdest > 0) {
            xfree(svaluesdest);
        }
        return RETURN_FAILURE;
    }

    for (i = 0; i < nssrc; i++) {
	int setnosrc, setnodest;
        setnosrc = svaluessrc[i];
	if (nsdest != 0) {
            setnodest = svaluesdest[i];
        } else {
            setnodest = SET_SELECT_NEXT;
        }
        
        res = do_histo(gsrc, setnosrc, gdest, setnodest,
            bins, nbins, cumulative, normalize);
	
        if (res != RETURN_SUCCESS) {
	    errmsg("Error in do_histo()");
	    error = TRUE;
            break;
	}
    }
    
    xfree(svaluessrc);
    if (nsdest > 0) {
        xfree(svaluesdest);
    }
    if (sampling == SAMPLING_MESH) {
        xfree(bins);
    }

    update_set_lists(gdest);
    xdrawgraph();
    
    if (error) {
        return RETURN_FAILURE;
    } else {
        return RETURN_SUCCESS;
    }
}

/* DFTs */

typedef struct _Four_ui {
    Widget top;
    SetChoiceItem sel;
    Widget *load_item;
    Widget *window_item;
    Widget *loadx_item;
    Widget *inv_item;
    Widget *type_item;
    Widget *graph_item;
} Four_ui;

static Four_ui fui;

void create_fourier_frame(void *data)
{
    Widget dialog;
    Widget rc;
    Widget buts[4];

    set_wait_cursor();
    if (fui.top == NULL) {
	char *l[4];
	l[0] = "DFT";
	l[1] = "FFT";
	l[2] = "Window only";
	l[3] = "Close";
	fui.top = XmCreateDialogShell(app_shell, "Fourier transforms", NULL, 0);
	handle_close(fui.top);
	dialog = XmCreateRowColumn(fui.top, "dialog_rc", NULL, 0);

	fui.sel = CreateSetSelector(dialog, "Apply to set:",
				    SET_SELECT_ALL,
				    FILTER_SELECT_NONE,
				    GRAPH_SELECT_CURRENT,
				    SELECTION_TYPE_MULTIPLE);

	rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, dialog,
			      XmNpacking, XmPACK_COLUMN,
			      XmNnumColumns, 5,
			      XmNorientation, XmHORIZONTAL,
			      XmNisAligned, True,
			      XmNadjustLast, False,
			      XmNentryAlignment, XmALIGNMENT_END,
			      NULL);

	XtVaCreateManagedWidget("Data window: ", xmLabelWidgetClass, rc, NULL);
	fui.window_item = CreatePanelChoice(rc,
					    " ",
					    8,
					    "None (Rectangular)",
					    "Triangular",
					    "Hanning",
					    "Welch",
					    "Hamming",
					    "Blackman",
					    "Parzen",
					    NULL);

	XtVaCreateManagedWidget("Load result as: ", xmLabelWidgetClass, rc, NULL);

	fui.load_item = CreatePanelChoice(rc,
					  " ",
					  4,
					  "Magnitude",
					  "Phase",
					  "Coefficients",
					  NULL);

	XtVaCreateManagedWidget("Let result X = ", xmLabelWidgetClass, rc, NULL);
	fui.loadx_item = CreatePanelChoice(rc,
					   " ",
					   4,
					   "Index",
					   "Frequency",
					   "Period",
					   NULL);

	XtVaCreateManagedWidget("Perform: ", xmLabelWidgetClass, rc, NULL);
	fui.inv_item = CreatePanelChoice(rc,
					 " ",
					 3,
					 "Transform",
					 "Inverse transform",
					 NULL);

	XtVaCreateManagedWidget("Data is: ", xmLabelWidgetClass, rc, NULL);
	fui.type_item = CreatePanelChoice(rc,
					  " ",
					  3,
					  "Real",
					  "Complex",
					  NULL);
	ManageChild(rc);

	CreateSeparator(dialog);
	CreateCommandButtons(dialog, 4, buts, l);
	XtAddCallback(buts[0], XmNactivateCallback, (XtCallbackProc) do_fourier_proc, (XtPointer) & fui);
	XtAddCallback(buts[1], XmNactivateCallback, (XtCallbackProc) do_fft_proc, (XtPointer) & fui);
	XtAddCallback(buts[2], XmNactivateCallback, (XtCallbackProc) do_window_proc, (XtPointer) & fui);
	XtAddCallback(buts[3], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) fui.top);

	ManageChild(dialog);
    }
    RaiseWindow(fui.top);
    unset_wait_cursor();
}

/*
 * DFT
 */
static void do_fourier_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int gno = get_cg();
    int *selsets;
    int i, cnt;
    int setno, load, loadx, invflag, type, wind;
    Four_ui *ui = (Four_ui *) client_data;
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    wind = GetChoice(ui->window_item);
    load = GetChoice(ui->load_item);
    loadx = GetChoice(ui->loadx_item);
    invflag = GetChoice(ui->inv_item);
    type = GetChoice(ui->type_item);
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
	setno = selsets[i];
	do_fourier(gno, setno, 0, load, loadx, invflag, type, wind);
    }
    update_set_lists(gno);
    xfree(selsets);
    unset_wait_cursor();
    xdrawgraph();
}

/*
 * DFT by FFT
 */
static void do_fft_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int gno = get_cg();
    int *selsets;
    int i, cnt;
    int setno, load, loadx, invflag, type, wind;
    Four_ui *ui = (Four_ui *) client_data;
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
	errwin("No sets selected");
	return;
    }
    wind = GetChoice(ui->window_item);
    load = GetChoice(ui->load_item);
    loadx = GetChoice(ui->loadx_item);
    invflag = GetChoice(ui->inv_item);
    type = GetChoice(ui->type_item);
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
	setno = selsets[i];
	do_fourier(gno, setno, 1, load, loadx, invflag, type, wind);
    }
    update_set_lists(gno);
    xfree(selsets);
    unset_wait_cursor();
    xdrawgraph();
}

/*
 * Apply data window only
 */
static void do_window_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int *selsets;
    int i, cnt;
    int setno, type, wind;
    Four_ui *ui = (Four_ui *) client_data;
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    wind = GetChoice(ui->window_item);
    type = GetChoice(ui->type_item);
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
	setno = selsets[i];
	do_window(setno, type, wind);
    }
    update_set_lists(get_cg());
    xfree(selsets);
    unset_wait_cursor();
    xdrawgraph();
}

/* running averages */

typedef struct _Run_ui {
    Widget top;
    SetChoiceItem sel;
    Widget len_item;
    Widget *type_item;
    Widget *region_item;
    Widget rinvert_item;
} Run_ui;

static Run_ui rui;

void create_run_frame(void *data)
{
    Widget dialog;
    Widget rc;

    set_wait_cursor();
    if (rui.top == NULL) {
	char *label2[2];
	label2[0] = "Accept";
	label2[1] = "Close";
	rui.top = XmCreateDialogShell(app_shell, "Running averages", NULL, 0);
	handle_close(rui.top);
	dialog = XmCreateRowColumn(rui.top, "dialog_rc", NULL, 0);

	rui.sel = CreateSetSelector(dialog, "Apply to set:",
				    SET_SELECT_ALL,
				    FILTER_SELECT_NONE,
				    GRAPH_SELECT_CURRENT,
				    SELECTION_TYPE_MULTIPLE);

	rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, dialog,
			      XmNpacking, XmPACK_COLUMN,
			      XmNnumColumns, 5,
			      XmNorientation, XmHORIZONTAL,
			      XmNisAligned, True,
			      XmNadjustLast, False,
			      XmNentryAlignment, XmALIGNMENT_END,
			      NULL);

	XtVaCreateManagedWidget("Running:", xmLabelWidgetClass, rc, NULL);
	rui.type_item = CreatePanelChoice(rc,
					  " ",
					  6,
					  "Average",
					  "Median",
					  "Minimum",
					  "Maximum",
					  "Std. dev.",
                                          NULL);
	rui.len_item = CreateTextItem4(rc, 10, "Length of average:");

	XtVaCreateManagedWidget("Restrictions:", xmLabelWidgetClass, rc, NULL);
	rui.region_item = CreatePanelChoice(rc,
					    " ",
					    9,
					    "None",
					    "Region 0",
					    "Region 1",
					    "Region 2",
					    "Region 3",
					    "Region 4",
					    "Inside graph",
					    "Outside graph",
					    NULL);

	rui.rinvert_item = CreateToggleButton(rc, "Invert region");

	ManageChild(rc);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc) do_runavg_proc, (XtPointer) & rui);
	XtAddCallback(but2[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) rui.top);

	ManageChild(dialog);
    }
    RaiseWindow(rui.top);
    unset_wait_cursor();
}

/*
 * running averages, medians, min, max, std. deviation
 */
static void do_runavg_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int gno = get_cg();
    int *selsets;
    int i, cnt;
    int runlen, runtype, setno, rno, invr;
    Run_ui *ui = (Run_ui *) client_data;
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    if (xv_evalexpri(ui->len_item, &runlen ) != RETURN_SUCCESS) {
        return;
    }
    runtype = GetChoice(ui->type_item);
    rno = GetChoice(ui->region_item) - 1;
    invr = GetToggleButtonState(ui->rinvert_item);
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
	setno = selsets[i];
	do_runavg(gno, setno, runlen, runtype, rno, invr);
    }
    update_set_lists(gno);
    unset_wait_cursor();
    xfree(selsets);
    xdrawgraph();
}

/* TODO finish this */
void do_eval_regress()
{
}

typedef struct _Reg_ui {
    Widget top;
    SetChoiceItem sel;
    Widget *degree_item;
    Widget zero_item;
    Widget *resid_item;
    Widget *region_item;
    Widget rinvert_item;
    Widget start_item;
    Widget stop_item;
    Widget step_item;
	Widget fload_rc;
    Widget method_item;
} Reg_ui;

static Reg_ui regui;


/*
 * set sensitivity of start, stop iand load buttons
 */
static void set_regr_sensitivity(Widget w, XtPointer client_data,
												XtPointer call_data)
{
	if( (int)client_data == 2 )
		SetSensitive( regui.fload_rc, True );
	else
		SetSensitive( regui.fload_rc, False );
}
	

void create_reg_frame(void *data)
{
    Widget dialog;
    Widget rc, rc2;
    Widget buts[2];
	int i;

    set_wait_cursor();
    if (regui.top == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	regui.top = XmCreateDialogShell(app_shell, "Regression", NULL, 0);
	handle_close(regui.top);
	dialog = XmCreateRowColumn(regui.top, "dialog_rc", NULL, 0);

	regui.sel = CreateSetSelector(dialog, "Apply to set:",
				      SET_SELECT_ALL,
				      FILTER_SELECT_NONE,
				      GRAPH_SELECT_CURRENT,
				      SELECTION_TYPE_MULTIPLE);

	rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, dialog,
			      XmNorientation, XmVERTICAL,
 			      NULL);

	rc2 = XtVaCreateWidget("rc2", xmRowColumnWidgetClass, rc,
			      XmNorientation, XmHORIZONTAL,
 			      NULL);
			      
	XtVaCreateManagedWidget("Type of fit:", xmLabelWidgetClass, rc2, NULL);
	regui.degree_item = CreatePanelChoice(rc2,
					      " ",
					      16,
					      "Linear",
					      "Quadratic",
					      "Cubic",
					      "4th degree",
					      "5th degree",
					      "6th degree",
					      "7th degree",
					      "8th degree",
					      "9th degree",
					      "10th degree",
					      "1-10",
					      "Power y=A*x^B",
					      "Exponential y=A*exp(B*x)",
					      "Logarithmic y=A+B*ln(x)",
					      "Inverse y=1/(A+Bx)",
					      NULL);
	ManageChild(rc2);
	
	rc2 = XtVaCreateWidget("rc2", xmRowColumnWidgetClass, rc,
			      XmNorientation, XmHORIZONTAL,
 			      NULL);
			      
	XtVaCreateManagedWidget("Load:", xmLabelWidgetClass, rc2, NULL);
	regui.resid_item = CreatePanelChoice(rc2,
					     " ",
					     4,
					     "Fitted values",
					     "Residuals",
					     "Function",
					     NULL);
        ManageChild(rc2);
	for( i=2; i<5; i++ )
		XtAddCallback( regui.resid_item[i], XmNactivateCallback, 
					set_regr_sensitivity, (XtPointer)(i-2) );


	
	rc2 = XtVaCreateWidget("rc2", xmRowColumnWidgetClass, rc,
			      XmNorientation, XmHORIZONTAL,
 			      NULL);
	XtVaCreateManagedWidget("Restrictions:", xmLabelWidgetClass, rc2, NULL);
	regui.region_item = CreatePanelChoice(rc2,
					      " ",
					      9,
					      "None",
					      "Region 0",
					      "Region 1",
					      "Region 2",
					      "Region 3",
					      "Region 4",
					      "Inside graph",
					      "Outside graph",
					      NULL);
	
        regui.rinvert_item = CreateToggleButton(rc2, "Invert region");
	ManageChild(rc2);
	
	CreateSeparator(rc);

	regui.fload_rc = XmCreateRowColumn(rc, "nonl_fload_rc", NULL, 0);
	XtVaSetValues(regui.fload_rc, XmNorientation, XmHORIZONTAL, NULL);
	regui.start_item = CreateTextItem2(regui.fload_rc, 6, "Start load at:");
	regui.stop_item  = CreateTextItem2(regui.fload_rc, 6, "Stop load at:");
	regui.step_item  = CreateTextItem2(regui.fload_rc, 4, "# of points:");
	ManageChild(regui.fload_rc);

	ManageChild(rc);
	SetSensitive(regui.fload_rc, False);
	
	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback, (XtCallbackProc) do_regress_proc, (XtPointer) & regui);
	XtAddCallback(buts[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) regui.top);

	ManageChild(dialog);
    }
    RaiseWindow(regui.top);
    unset_wait_cursor();
}

/*
 * regression
 */
static void do_regress_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int gno = get_cg();
    int *selsets;
    int cnt;
    Reg_ui *ui = (Reg_ui *) client_data;
    int setno, ideg, iresid, i, j, k;
    int rno = GetChoice(ui->region_item) - 1;
    int invr = GetToggleButtonState(ui->rinvert_item);
    int nstep = 0, rx, rset = 0;
    double xstart, xstop, stepsize = 0.0, *xr;

    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    ideg = (int) GetChoice(ui->degree_item) + 1;
	switch(rx=GetChoice(ui->resid_item) ){
		case 0:				/* evaluate fitted function at original x's */
			iresid = 0;
			rset = -1;
			break;
		case 1:				/* load residue at original x points */
			iresid = 1;
			rset = -1;
			break;
		case 2:		/* evaluate fitted function at new x points */
		    iresid = 0;
		    if(xv_evalexpri(ui->step_item, &nstep) != RETURN_SUCCESS || nstep < 2 ) {
		            errwin("Number points < 2");
		            return;         
		    }
		    if(xv_evalexpr(ui->start_item, &xstart ) != RETURN_SUCCESS) {
		            errwin("Specify starting value");
		            return;
		    }               
		    if(xv_evalexpr(ui->stop_item, &xstop) != RETURN_SUCCESS) {
		            errwin("Specify stopping value");
		            return;
		    } else {
                        stepsize = (xstop - xstart)/(nstep-1);
                    }
		    break;
                default:
                    errwin("Internal error");
		    return;
	}	
    set_wait_cursor();
	for (i = (ideg==11?1:ideg); i <= (ideg==11?10:ideg); i++) {
    	for (j = 0; j < cnt; j++) {
			setno = selsets[j];
			if( rx == 2 ) {
				if( (rset = nextset( gno )) == -1 ){
				     errwin("Not enough sets");
				     return;
				}
				activateset( gno, rset );
				setlength( gno, rset, nstep);
				xr = getx( gno, rset );
				for( k=0; k<nstep; k++ )
					xr[k] = xstart+k*stepsize;
			}
			do_regress(gno, setno, i, iresid, rno, invr, rset);
	    }
    }
    update_set_lists(gno);
    unset_wait_cursor();
    xfree(selsets);
    xdrawgraph();
}

/* finite differencing */

typedef struct _Diff_ui {
    Widget top;
    SetChoiceItem sel;
    Widget *type_item;
    Widget *region_item;
    Widget rinvert_item;
} Diff_ui;

static Diff_ui dui;

void create_diff_frame(void *data)
{
    Widget dialog;

    set_wait_cursor();
    if (dui.top == NULL) {
	char *label2[2];
	label2[0] = "Accept";
	label2[1] = "Close";
	dui.top = XmCreateDialogShell(app_shell, "Differences", NULL, 0);
	handle_close(dui.top);
	dialog = XmCreateRowColumn(dui.top, "dialog_rc", NULL, 0);

	dui.sel = CreateSetSelector(dialog, "Apply to set:",
				    SET_SELECT_ALL,
				    FILTER_SELECT_NONE,
				    GRAPH_SELECT_CURRENT,
				    SELECTION_TYPE_MULTIPLE);
	dui.type_item = CreatePanelChoice(dialog,
					  "Method:",
					  4,
					  "Forward difference",
					  "Backward difference",
					  "Centered difference",
					  NULL);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc) do_differ_proc, (XtPointer) & dui);
	XtAddCallback(but2[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) dui.top);

	ManageChild(dialog);
    }
    RaiseWindow(dui.top);
    unset_wait_cursor();
}

/*
 * finite differences
 */
static void do_differ_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int gno = get_cg();
    int *selsets;
    int i, cnt;
    int setno, itype;
    Diff_ui *ui = (Diff_ui *) client_data;
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    itype = (int) GetChoice(ui->type_item);
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
	setno = selsets[i];
	do_differ(gno, setno, itype);
    }
    update_set_lists(gno);
    unset_wait_cursor();
    xfree(selsets);
    xdrawgraph();
}

/* numerical integration */

typedef struct _Int_ui {
    Widget top;
    SetChoiceItem sel;
    Widget *type_item;
    Widget sum_item;
    Widget *region_item;
    Widget rinvert_item;
} Int_ui;

static Int_ui iui;

void create_int_frame(void *data)
{
    Widget dialog;

    set_wait_cursor();
    if (iui.top == NULL) {
	char *label2[2];
	label2[0] = "Accept";
	label2[1] = "Close";
	iui.top = XmCreateDialogShell(app_shell, "Integration", NULL, 0);
	handle_close(iui.top);
	dialog = XmCreateRowColumn(iui.top, "dialog_rc", NULL, 0);
	iui.sel = CreateSetSelector(dialog, "Apply to set:",
				    SET_SELECT_ALL,
				    FILTER_SELECT_NONE,
				    GRAPH_SELECT_CURRENT,
				    SELECTION_TYPE_MULTIPLE);

	iui.type_item = CreatePanelChoice(dialog,
					  "Load:",
					  3,
					  "Cumulative sum",
					  "Sum only",
					  NULL);
	iui.sum_item = CreateTextItem2(dialog, 10, "Sum:");

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc) do_int_proc, (XtPointer) & iui);
	XtAddCallback(but2[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) iui.top);

	ManageChild(dialog);
    }
    RaiseWindow(iui.top);
    unset_wait_cursor();
}

/*
 * numerical integration
 */
static void do_int_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int gno = get_cg();
    int *selsets;
    int i, cnt;
    int setno, itype;
    double sum;
    Int_ui *ui = (Int_ui *) client_data;
    char buf[32];
    
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    itype = GetChoice(ui->type_item);
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
	setno = selsets[i];
	sum = do_int(gno, setno, itype);
	sprintf(buf, "%g", sum);
	xv_setstr(ui->sum_item, buf);
    }
    update_set_lists(gno);
    unset_wait_cursor();
    xfree(selsets);
    xdrawgraph();
}

/* seasonal differencing */

typedef struct _Seas_ui {
    Widget top;
    SetChoiceItem sel;
    Widget *type_item;
    Widget period_item;
    Widget *region_item;
    Widget rinvert_item;
} Seas_ui;

static Seas_ui sui;

void create_seasonal_frame(void *data)
{
    Widget dialog;

    set_wait_cursor();
    if (sui.top == NULL) {
	char *label2[2];
	label2[0] = "Accept";
	label2[1] = "Close";
	sui.top = XmCreateDialogShell(app_shell, "Seasonal differences", NULL, 0);
	handle_close(sui.top);
	dialog = XmCreateRowColumn(sui.top, "dialog_rc", NULL, 0);

	sui.sel = CreateSetSelector(dialog, "Apply to set:",
				    SET_SELECT_ALL,
				    FILTER_SELECT_NONE,
				    GRAPH_SELECT_CURRENT,
				    SELECTION_TYPE_MULTIPLE);
	sui.period_item = CreateTextItem2(dialog, 10, "Period:");

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc) do_seasonal_proc, (XtPointer) & sui);
	XtAddCallback(but2[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) sui.top);

	ManageChild(dialog);
    }
    RaiseWindow(sui.top);
    unset_wait_cursor();
}

/*
 * seasonal differences
 */
static void do_seasonal_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int *selsets;
    int i, cnt;
    int setno, period;
    Seas_ui *ui = (Seas_ui *) client_data;
    cnt = GetSelectedSets(ui->sel, &selsets);
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    if(xv_evalexpri(ui->period_item, &period ) != RETURN_SUCCESS)
		return;
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
		setno = selsets[i];
		do_seasonal_diff(setno, period);
    }
    update_set_lists(get_cg());
    xfree(selsets);
    unset_wait_cursor();
    xdrawgraph();
}


/* cross correlation */

typedef struct _Cross_ui {
    Widget top;
    SetChoiceItem sel1;
    SetChoiceItem sel2;
    Widget lag_item;
    Widget covar_item;
} Cross_ui;

static Cross_ui crossui;

void create_xcor_frame(void *data)
{
    Widget dialog;

    set_wait_cursor();
    if (crossui.top == NULL) {
	char *label2[3];
	label2[0] = "Accept";
	label2[1] = "Close";
	crossui.top = XmCreateDialogShell(app_shell, "Correlation/Covariance", NULL, 0);
	handle_close(crossui.top);
	dialog = XmCreateRowColumn(crossui.top, "dialog_rc", NULL, 0);

	crossui.sel1 = CreateSetSelector(dialog, "Select set:",
					 SET_SELECT_ACTIVE,
					 FILTER_SELECT_NONE,
					 GRAPH_SELECT_CURRENT,
					 SELECTION_TYPE_SINGLE);
	crossui.sel2 = CreateSetSelector(dialog, "Select set:",
					 SET_SELECT_ACTIVE,
					 FILTER_SELECT_NONE,
					 GRAPH_SELECT_CURRENT,
					 SELECTION_TYPE_SINGLE);
	crossui.lag_item = CreateTextItem2(dialog, 10, "Maximum lag:");
	crossui.covar_item = CreateToggleButton(dialog, "Calculate covariance");

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc) do_xcor_proc, (XtPointer) & crossui);
	XtAddCallback(but2[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) crossui.top);

	ManageChild(dialog);
    }
    RaiseWindow(crossui.top);
    unset_wait_cursor();
}

/*
 * cross correlation
 */
static void do_xcor_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int gno = get_cg();
    int set1, set2, maxlag, covar;
    Cross_ui *ui = (Cross_ui *) client_data;
    set1 = GetSelectedSet(ui->sel1);
    set2 = GetSelectedSet(ui->sel2);
    if (set1 == SET_SELECT_ERROR || set2 == SET_SELECT_ERROR) {
	errwin("Select 2 sets");
	return;
    }
    if(xv_evalexpri(ui->lag_item, &maxlag) != RETURN_SUCCESS) { 
        return;
    }
    covar = GetToggleButtonState(ui->covar_item);
    set_wait_cursor();
    do_xcor(gno, set1, gno, set2, maxlag, covar);
    update_set_lists(gno);
    xdrawgraph();
    unset_wait_cursor();
}


/* sample a set */

typedef struct _Samp_ui {
    Widget top;
    SetChoiceItem sel;
    Widget *type_item;
    Widget start_item;
    Widget step_item;
    Widget expr_item;
    Widget *region_item;
    Widget rinvert_item;
} Samp_ui;

static Samp_ui sampui;

void create_samp_frame(void *data)
{
    static Widget dialog;
    Widget rc;

    set_wait_cursor();
    if (sampui.top == NULL) {
	char *label2[2];
	label2[0] = "Accept";
	label2[1] = "Close";
	sampui.top = XmCreateDialogShell(app_shell, "Sample points", NULL, 0);
	handle_close(sampui.top);
	dialog = XmCreateRowColumn(sampui.top, "dialog_rc", NULL, 0);

	sampui.sel = CreateSetSelector(dialog, "Apply to set:",
				       SET_SELECT_ALL,
				       FILTER_SELECT_NONE,
				       GRAPH_SELECT_CURRENT,
				       SELECTION_TYPE_MULTIPLE);

	rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, dialog,
			      XmNpacking, XmPACK_COLUMN,
			      XmNnumColumns, 5,
			      XmNorientation, XmHORIZONTAL,
			      XmNisAligned, True,
			      XmNadjustLast, False,
			      XmNentryAlignment, XmALIGNMENT_END,
			      NULL);

	XtVaCreateManagedWidget("Sample type:", xmLabelWidgetClass, rc, NULL);
	sampui.type_item = CreatePanelChoice(rc,
					     " ",
					     3,
					     "Start/step",
					     "Expression",
					     NULL);
	sampui.start_item = CreateTextItem4(rc, 10, "Start:");
	sampui.step_item = CreateTextItem4(rc, 10, "Step:");
	sampui.expr_item = CreateTextItem4(rc, 10, "Logical expression:");
	ManageChild(rc);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc) do_sample_proc, (XtPointer) & sampui);
	XtAddCallback(but2[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) sampui.top);

	ManageChild(dialog);
    }
    RaiseWindow(sampui.top);
    unset_wait_cursor();
}

/*
 * sample a set, by start/step or logical expression
 */
static void do_sample_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int *selsets;
    int i, cnt;
    int setno, typeno;
    char *exprstr;
    int startno, stepno;
    Samp_ui *ui = (Samp_ui *) client_data;
    
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errmsg("No sets selected");
        return;
    }
    
    typeno = GetChoice(ui->type_item);
    
    if (typeno == 0) {
        exprstr = "";
        if (xv_evalexpri(ui->start_item, &startno) != RETURN_SUCCESS ||
            xv_evalexpri(ui->step_item, &stepno)   != RETURN_SUCCESS) {
            errmsg("Please select start and step values");
            return;
        }
    } else {
        exprstr = xv_getstr(ui->expr_item);
        startno = stepno = 1;
    }
    
    set_wait_cursor();
    
    for (i = 0; i < cnt; i++) {
        setno = selsets[i];
        do_sample(setno, typeno, exprstr, startno, stepno);
    }
    
    xfree(selsets);
    
    update_set_lists(get_cg());
    xdrawgraph();
    
    unset_wait_cursor();
}

/* Prune data */

typedef struct _Prune_ui {
    Widget top;
    SetChoiceItem sel;
    Widget *type_item;
    Widget *dxtype_item;
    Widget *dytype_item;
    Widget *deltatype_item;
    Widget dx_rc;
    Widget dy_rc;
    Widget dx_item;
    Widget dy_item;
} Prune_ui;

static Prune_ui pruneui;

void create_prune_frame(void *data)
{
    int i;
    static Widget dialog;

    set_wait_cursor();
    if (pruneui.top == NULL) {
	char *label2[2];
	label2[0] = "Accept";
	label2[1] = "Close";
	pruneui.top = XmCreateDialogShell(app_shell, "Prune data", NULL, 0);
	handle_close(pruneui.top);
	dialog = XmCreateRowColumn(pruneui.top, "dialog_rc", NULL, 0);

	pruneui.sel = CreateSetSelector(dialog, "Apply to set:",
	    SET_SELECT_ALL, FILTER_SELECT_NONE, GRAPH_SELECT_CURRENT,
	    SELECTION_TYPE_MULTIPLE);

	pruneui.type_item = CreatePanelChoice(dialog,
	    "Prune type: ", 5,
	    "Interpolation", "Circle", "Ellipse", "Rectangle",
	    NULL);

	pruneui.dx_rc = XtVaCreateWidget("dx_rc",
            xmRowColumnWidgetClass, dialog,
            XmNorientation, XmHORIZONTAL,
	    NULL);
	pruneui.dx_item = CreateTextItem4(pruneui.dx_rc, 17, "Delta X:");
        ManageChild(pruneui.dx_rc);

	pruneui.dy_rc = XtVaCreateWidget("dy_rc",
            xmRowColumnWidgetClass, dialog,
            XmNorientation, XmHORIZONTAL,
	    NULL);
	pruneui.dy_item = CreateTextItem4(pruneui.dy_rc, 17, "Delta Y:");
        ManageChild(pruneui.dy_rc);

	CreateSeparator(dialog);

        pruneui.deltatype_item = CreatePanelChoice(dialog,
	    "Type of Delta coordinates:", 3, "Viewport", "World", NULL);
	
	pruneui.dxtype_item = CreatePanelChoice(dialog,
            "Scaling of Delta X:", 3, "Linear", "Logarithmic", NULL);
	
	pruneui.dytype_item = CreatePanelChoice(dialog,
            "Scaling of Delta Y:", 3, "Linear", "Logarithmic", NULL);

        update_prune_frame();

        for (i = 0; i <= 3; i++) {
            XtAddCallback(pruneui.type_item[2 + i], XmNactivateCallback,
                (XtCallbackProc) do_prune_toggle, (XtPointer) &pruneui);
        }
	for (i = 0; i <= 1; i++) {
            XtAddCallback(pruneui.deltatype_item[2 + i], XmNactivateCallback,
                (XtCallbackProc) do_prune_toggle, (XtPointer) &pruneui);
        }
        do_prune_toggle ((Widget) NULL, (XtPointer) &pruneui, 0);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc) do_prune_proc, (XtPointer) & pruneui);
	XtAddCallback(but2[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) pruneui.top);

	ManageChild(dialog);
    }
    RaiseWindow(pruneui.top);
    unset_wait_cursor();
}

void update_prune_frame(void)
{
    if (pruneui.top != NULL) {
        SetChoice(pruneui.dxtype_item,
            (get_graph_xscale(get_cg()) == SCALE_LOG) ? 1 : 0);
        SetChoice(pruneui.dytype_item,
            (get_graph_yscale(get_cg()) == SCALE_LOG) ? 1 : 0);
    }
}

/*
 * Toggle prune type
 */
static void do_prune_toggle(Widget w, XtPointer client_data, XtPointer call_data)
{
    Prune_ui *ui = (Prune_ui *) client_data;
    int typeno = (int) GetChoice(ui->type_item);
    int deltatypeno = (int) GetChoice(ui->deltatype_item);

    switch (typeno) {
        case PRUNE_CIRCLE:
	    SetSensitive(pruneui.dx_rc, TRUE);
	    SetSensitive(pruneui.dy_rc, FALSE);
	    switch (deltatypeno) {
		case PRUNE_VIEWPORT:
		    SetSensitive(*pruneui.dxtype_item, FALSE);
		    SetSensitive(*pruneui.dytype_item, FALSE);
		    break;
		case PRUNE_WORLD:
		    SetSensitive(*pruneui.dxtype_item, TRUE);
		    SetSensitive(*pruneui.dytype_item, FALSE);
		    break;
	    }
	    break;
        case PRUNE_ELLIPSE:
        case PRUNE_RECTANGLE:
	    SetSensitive(pruneui.dx_rc, TRUE);
	    SetSensitive(pruneui.dy_rc, TRUE);
	    switch (deltatypeno) {
		case PRUNE_VIEWPORT:
		    SetSensitive(*pruneui.dxtype_item, FALSE);
		    SetSensitive(*pruneui.dytype_item, FALSE);
		    break;
		case PRUNE_WORLD:
		    SetSensitive(*pruneui.dxtype_item, TRUE);
		    SetSensitive(*pruneui.dytype_item, TRUE);
		    break;
	    }
	    break;
        case PRUNE_INTERPOLATION:
	    SetSensitive(pruneui.dx_rc, FALSE);
	    SetSensitive(pruneui.dy_rc, TRUE);
	    switch (deltatypeno) {
		case PRUNE_VIEWPORT:
		    SetSensitive(*pruneui.dxtype_item, FALSE);
		    SetSensitive(*pruneui.dytype_item, FALSE);
		    break;
		case PRUNE_WORLD:
		    SetSensitive(*pruneui.dxtype_item, FALSE);
		    SetSensitive(*pruneui.dytype_item, TRUE);
		    break;
	    }
	    break;
    }
}

/*
 * Prune data
 */
static void do_prune_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int *selsets;
    int i, cnt;
    int setno, typeno, deltatypeno;
    int dxtype, dytype;
    double deltax, deltay;

    Prune_ui *ui = (Prune_ui *) client_data;
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    typeno = (int) GetChoice(ui->type_item);
    deltatypeno = (int) GetChoice(ui->deltatype_item);
    dxtype = (int) GetChoice(ui->dxtype_item);
    dytype = (int) GetChoice(ui->dytype_item);

	if( XtIsSensitive(ui->dx_rc)== True ){
		if(xv_evalexpr(ui->dx_item, &deltax) != RETURN_SUCCESS)
			return;
	} else
		deltax = 0;
	if( XtIsSensitive(ui->dy_rc)== True ){
		if(xv_evalexpr(ui->dy_item, &deltay) != RETURN_SUCCESS )
			return;
	} else
		deltay = 0;	
	
    set_wait_cursor();
    for (i = 0; i < cnt; i++) {
	setno = selsets[i];

	do_prune(setno, typeno, deltatypeno, deltax, deltay, dxtype, dytype);
    }
    update_set_lists(get_cg());
    unset_wait_cursor();
    xfree(selsets);
    xdrawgraph();
}

/* apply a digital filter in set 2 to set 1 */

typedef struct _Digf_ui {
    Widget top;
    SetChoiceItem sel1;
    SetChoiceItem sel2;
    Widget *type_item;
    Widget *region_item;
    Widget rinvert_item;
} Digf_ui;

static Digf_ui digfui;

void create_digf_frame(void *data)
{
    Widget dialog;

    set_wait_cursor();
    if (digfui.top == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	digfui.top = XmCreateDialogShell(app_shell, "Digital filter", NULL, 0);
	handle_close(digfui.top);
	dialog = XmCreateRowColumn(digfui.top, "dialog_rc", NULL, 0);

	digfui.sel1 = CreateSetSelector(dialog, "Filter set:",
					SET_SELECT_ACTIVE,
					FILTER_SELECT_NONE,
					GRAPH_SELECT_CURRENT,
					SELECTION_TYPE_SINGLE);
	digfui.sel2 = CreateSetSelector(dialog, "With weights from set:",
					SET_SELECT_ACTIVE,
					FILTER_SELECT_NONE,
					GRAPH_SELECT_CURRENT,
					SELECTION_TYPE_SINGLE);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but1, label1);
	XtAddCallback(but1[0], XmNactivateCallback, (XtCallbackProc) do_digfilter_proc, (XtPointer) & digfui);
	XtAddCallback(but1[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) digfui.top);

	ManageChild(dialog);
    }
    RaiseWindow(digfui.top);
    unset_wait_cursor();
}

/*
 * apply a digital filter
 */
static void do_digfilter_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int set1, set2;
    Digf_ui *ui = (Digf_ui *) client_data;
    set1 = GetSelectedSet(ui->sel1);
    set2 = GetSelectedSet(ui->sel2);
    if (set1 == SET_SELECT_ERROR || set2 == SET_SELECT_ERROR) {
	errwin("Select 2 sets");
	return;
    }
    set_wait_cursor();
    do_digfilter(set1, set2);
    update_set_lists(get_cg());
    unset_wait_cursor();
}

/* linear convolution */

typedef struct _Lconv_ui {
    Widget top;
    SetChoiceItem sel1;
    SetChoiceItem sel2;
    Widget *type_item;
    Widget lag_item;
    Widget *region_item;
    Widget rinvert_item;
} Lconv_ui;

static Lconv_ui lconvui;

void create_lconv_frame(void *data)
{
    Widget dialog;

    set_wait_cursor();
    if (lconvui.top == NULL) {
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
	lconvui.top = XmCreateDialogShell(app_shell, "Linear convolution", NULL, 0);
	handle_close(lconvui.top);
	dialog = XmCreateRowColumn(lconvui.top, "dialog_rc", NULL, 0);

	lconvui.sel1 = CreateSetSelector(dialog, "Convolve set:",
					 SET_SELECT_ACTIVE,
					 FILTER_SELECT_NONE,
					 GRAPH_SELECT_CURRENT,
					 SELECTION_TYPE_SINGLE);
	lconvui.sel2 = CreateSetSelector(dialog, "With set:",
					 SET_SELECT_ACTIVE,
					 FILTER_SELECT_NONE,
					 GRAPH_SELECT_CURRENT,
					 SELECTION_TYPE_SINGLE);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but1, label1);
	XtAddCallback(but1[0], XmNactivateCallback, (XtCallbackProc) do_linearc_proc, (XtPointer) & lconvui);
	XtAddCallback(but1[1], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) lconvui.top);

	ManageChild(dialog);
    }
    RaiseWindow(lconvui.top);
    unset_wait_cursor();
}

/*
 * linear convolution
 */
static void do_linearc_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int set1, set2;
    Lconv_ui *ui = (Lconv_ui *) client_data;
    set1 = GetSelectedSet(ui->sel1);
    set2 = GetSelectedSet(ui->sel2);
    if (set1 == SET_SELECT_ERROR || set2 == SET_SELECT_ERROR) {
	errwin("Select 2 sets");
	return;
    }
    set_wait_cursor();
    do_linearc(get_cg(), set1, get_cg(), set2);
    update_set_lists(get_cg());
    xdrawgraph();
    unset_wait_cursor();
}


/*
 * Rotate, scale, translate
 */

typedef struct _Geom_ui {
    Widget top;
    SetChoiceItem sel;
    SetChoiceItem sel2;
    Widget *order_item;
    Widget degrees_item;
    Widget rotx_item;
    Widget roty_item;
    Widget scalex_item;
    Widget scaley_item;
    Widget transx_item;
    Widget transy_item;
    Widget *region_item;
    Widget rinvert_item;
} Geom_ui;

static Geom_ui gui;

static void do_geom_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void reset_geom_proc(Widget, XtPointer, XtPointer);

void create_geom_frame(void *data)
{
    Widget dialog;
    Widget rc;

    set_wait_cursor();
    if (gui.top == NULL) {
	char *label1[3];
	label1[0] = "Accept";
	label1[1] = "Reset";
	label1[2] = "Close";
	gui.top = XmCreateDialogShell(app_shell, "Geometric transformations", NULL, 0);
	handle_close(gui.top);
	dialog = XmCreateRowColumn(gui.top, "dialog_rc", NULL, 0);

	gui.sel = CreateSetSelector(dialog, "Apply to set:",
				    SET_SELECT_ALL,
				    FILTER_SELECT_NONE,
				    GRAPH_SELECT_CURRENT,
				    SELECTION_TYPE_MULTIPLE);

	rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, dialog,
			      XmNpacking, XmPACK_COLUMN,
			      XmNnumColumns, 8,
			      XmNorientation, XmHORIZONTAL,
			      XmNisAligned, True,
			      XmNadjustLast, False,
			      XmNentryAlignment, XmALIGNMENT_END,
			      NULL);

	gui.order_item = CreatePanelChoice(dialog,
					   "Apply in order:",
					   7,
					   "Rotate, translate, scale",
					   "Rotate, scale, translate",
					   "Translate, scale, rotate",
					   "Translate, rotate, scale",
					   "Scale, translate, rotate",
					   "Scale, rotate, translate",
					   NULL);

	gui.degrees_item = CreateTextItem4(rc, 10, "Rotation (degrees):");
	gui.rotx_item = CreateTextItem4(rc, 10, "Rotate about X = :");
	gui.roty_item = CreateTextItem4(rc, 10, "Rotate about Y = :");
	gui.scalex_item = CreateTextItem4(rc, 10, "Scale X:");
	gui.scaley_item = CreateTextItem4(rc, 10, "Scale Y:");
	gui.transx_item = CreateTextItem4(rc, 10, "Translate X:");
	gui.transy_item = CreateTextItem4(rc, 10, "Translate Y:");
	ManageChild(rc);

	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 3, but1, label1);
	XtAddCallback(but1[0], XmNactivateCallback, (XtCallbackProc) do_geom_proc, (XtPointer) & gui);
	XtAddCallback(but1[1], XmNactivateCallback, (XtCallbackProc) reset_geom_proc, (XtPointer)
	& gui.top);
	XtAddCallback(but1[2], XmNactivateCallback, (XtCallbackProc) destroy_dialog, (XtPointer) gui.top);

	ManageChild(dialog);
	xv_setstr(gui.degrees_item, "0.0");
	xv_setstr(gui.rotx_item, "0.0");
	xv_setstr(gui.roty_item, "0.0");
	xv_setstr(gui.scalex_item, "1.0");
	xv_setstr(gui.scaley_item, "1.0");
	xv_setstr(gui.transx_item, "0.0");
	xv_setstr(gui.transy_item, "0.0");
    }
    RaiseWindow(gui.top);
    unset_wait_cursor();
}

/*
 * compute geom
 */
static void do_geom_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int i, j, k, cnt, order[3], setno, ord;
    int *selsets;
    double degrees, sx, sy, rotx, roty, tx, ty, xtmp, ytmp, *x, *y;
    double cosd, sind;
    Geom_ui *ui = (Geom_ui *) client_data;
    
    cnt = GetSelectedSets(ui->sel, &selsets);
    if (cnt == SET_SELECT_ERROR) {
        errwin("No sets selected");
        return;
    }
    ord = (int) GetChoice(ui->order_item);
    switch (ord) {
    case 0:
	order[0] = 0;		/* rotate */
	order[1] = 1;		/* translate */
	order[2] = 2;		/* scale */
	break;
    case 1:
	order[0] = 0;
	order[1] = 2;
	order[2] = 1;
	break;
    case 2:
	order[0] = 1;
	order[1] = 2;
	order[2] = 0;
	break;
    case 3:
	order[0] = 1;
	order[1] = 0;
	order[2] = 2;
	break;
    case 4:
	order[0] = 2;
	order[1] = 1;
	order[2] = 0;
	break;
    case 5:
	order[0] = 2;
	order[1] = 0;
	order[2] = 1;
	break;
    }
	/* check input fields */
    if (xv_evalexpr(ui->degrees_item, &degrees) != RETURN_SUCCESS ||
        xv_evalexpr(ui->rotx_item, &rotx)       != RETURN_SUCCESS ||
        xv_evalexpr(ui->roty_item, &roty)       != RETURN_SUCCESS ||
        xv_evalexpr(ui->transx_item, &tx)       != RETURN_SUCCESS ||
        xv_evalexpr(ui->transy_item, &ty)       != RETURN_SUCCESS ||
        xv_evalexpr(ui->scalex_item, &sx)       != RETURN_SUCCESS ||
        xv_evalexpr(ui->scaley_item, &sy)       != RETURN_SUCCESS )
		return;
   	
	degrees = M_PI / 180.0 * degrees;
	cosd = cos(degrees);
	sind = sin(degrees);
	
    set_wait_cursor();
    for (k = 0; k < cnt; k++) {
	setno = selsets[k];
	if (is_set_active(get_cg(), setno)) {
	    x = getx(get_cg(), setno);
	    y = gety(get_cg(), setno);
	    for (j = 0; j < 3; j++) {
		switch (order[j]) {
		case 0:			/* rotate */
		    if (degrees == 0.0) {
				break;
		    }
		    for (i = 0; i < getsetlength(get_cg(), setno); i++) {
				xtmp = x[i] - rotx;
				ytmp = y[i] - roty;
				x[i] = rotx + cosd * xtmp - sind * ytmp;
				y[i] = roty + sind * xtmp + cosd * ytmp;
		    }
		    break;
		case 1:			/* translate */
		    for (i = 0; i < getsetlength(get_cg(), setno); i++) {
			x[i] += tx;
			y[i] += ty;
		    }
		    break;
		case 2:					/* scale */
		    for (i = 0; i < getsetlength(get_cg(), setno); i++) {
				x[i] *= sx;
				y[i] *= sy;
		    }
		    break;
		}		/* end case */
	    }			/* end for j */

	    update_set_lists(get_cg());
	}			/* end if */
    }				/* end for k */
    update_set_lists(get_cg());
    xfree(selsets);
    set_dirtystate();
    unset_wait_cursor();
    xdrawgraph();
}

static void reset_geom_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    Geom_ui *tui = (Geom_ui *) client_data;
	xv_setstr(tui->degrees_item, "0.0");
	xv_setstr(tui->rotx_item, "0.0");
	xv_setstr(tui->roty_item, "0.0");
	xv_setstr(tui->scalex_item, "1.0");
	xv_setstr(tui->scaley_item, "1.0");
	xv_setstr(tui->transx_item, "0.0");
	xv_setstr(tui->transy_item, "0.0");
}
