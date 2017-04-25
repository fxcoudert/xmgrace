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
 * Contents:
 *     arrange graphs popup
 *     overlay graphs popup
 *     autoscaling popup
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "mbitmaps.h"

#include "globals.h"
#include "graphutils.h"
#include "device.h"
#include "utils.h"
#include "motifinc.h"
#include "protos.h"


static Widget overlay_dialog = NULL;

/*
 * Panel item declarations
 */

static ListStructure *graph_overlay1_choice_item;
static ListStructure *graph_overlay2_choice_item;
static OptionStructure *graph_overlaytype_item;

static int define_arrange_proc(void *data);
static int define_overlay_proc(void *data);
static int define_autos_proc(void *data);

typedef struct _Arrange_ui {
    Widget top;
    ListStructure *graphs;
    SpinStructure *nrows;
    SpinStructure *ncols;
    OptionStructure *order;
    Widget snake;
    SpinStructure *toff;
    SpinStructure *loff;
    SpinStructure *roff;
    SpinStructure *boff;
    SpinStructure *hgap;
    SpinStructure *vgap;
    Widget hpack;
    Widget vpack;
    Widget add;
    Widget kill;
} Arrange_ui;


/*
 * Arrange graphs popup routines
 */
static int define_arrange_proc(void *data)
{
    Arrange_ui *ui = (Arrange_ui *) data;
    int ngraphs, *graphs;
    int nrows, ncols, order, snake;
    int hpack, vpack, add, kill;
    double toff, loff, roff, boff, vgap, hgap;

    nrows = (int) GetSpinChoice(ui->nrows);
    ncols = (int) GetSpinChoice(ui->ncols);
    if (nrows < 1 || ncols < 1) {
	errmsg("# of rows and columns must be > 0");
	return RETURN_FAILURE;
    }
    
    ngraphs = GetListChoices(ui->graphs, &graphs);
    if (ngraphs == 0) {
        graphs = NULL;
    }
    
    order = GetOptionChoice(ui->order);
    snake = GetToggleButtonState(ui->snake);
    
    toff = GetSpinChoice(ui->toff);
    loff = GetSpinChoice(ui->loff);
    roff = GetSpinChoice(ui->roff);
    boff = GetSpinChoice(ui->boff);

    hgap = GetSpinChoice(ui->hgap);
    vgap = GetSpinChoice(ui->vgap);
    
    add  = GetToggleButtonState(ui->add);
    kill = GetToggleButtonState(ui->kill);
    
    hpack = GetToggleButtonState(ui->hpack);
    vpack = GetToggleButtonState(ui->vpack);

    if (add && ngraphs < nrows*ncols) {
        int gno;
        graphs = xrealloc(graphs, nrows*ncols*SIZEOF_INT);
        for (gno = number_of_graphs(); ngraphs < nrows*ncols; ngraphs++, gno++) {
            graphs[ngraphs] = gno;
        }
    }
    
    if (kill && ngraphs > nrows*ncols) {
        for (; ngraphs > nrows*ncols; ngraphs--) {
            kill_graph(graphs[ngraphs - 1]);
        }
    }
    
    arrange_graphs(graphs, ngraphs,
        nrows, ncols, order, snake,
        loff, roff, toff, boff, vgap, hgap,
        hpack, vpack);
    
    update_all();
    
    SelectListChoices(ui->graphs, ngraphs, graphs);
    xfree(graphs);
    
    xdrawgraph();
    
    return RETURN_SUCCESS;
}

void hpack_cb(int onoff, void *data)
{
    Arrange_ui *ui = (Arrange_ui *) data;
    SetSensitive(ui->hgap->rc, !onoff);
}
void vpack_cb(int onoff, void *data)
{
    Arrange_ui *ui = (Arrange_ui *) data;
    SetSensitive(ui->vgap->rc, !onoff);
}

void create_arrange_frame(void *data)
{
    static Arrange_ui *ui = NULL;
    set_wait_cursor();

    if (ui == NULL) {
        Widget arrange_panel, fr, gr, rc;
        BitmapOptionItem opitems[8] = {
            {0               | 0              | 0             , m_hv_lr_tb_bits},
            {0               | 0              | GA_ORDER_V_INV, m_hv_lr_bt_bits},
            {0               | GA_ORDER_H_INV | 0             , m_hv_rl_tb_bits},
            {0               | GA_ORDER_H_INV | GA_ORDER_V_INV, m_hv_rl_bt_bits},
            {GA_ORDER_HV_INV | 0              | 0             , m_vh_lr_tb_bits},
            {GA_ORDER_HV_INV | 0              | GA_ORDER_V_INV, m_vh_lr_bt_bits},
            {GA_ORDER_HV_INV | GA_ORDER_H_INV | 0             , m_vh_rl_tb_bits},
            {GA_ORDER_HV_INV | GA_ORDER_H_INV | GA_ORDER_V_INV, m_vh_rl_bt_bits}
        };
        
        ui = xmalloc(sizeof(Arrange_ui));
    
	ui->top = CreateDialogForm(app_shell, "Arrange graphs");

	arrange_panel = CreateVContainer(ui->top);
        
	fr = CreateFrame(arrange_panel, NULL);
        rc = CreateVContainer(fr);
        ui->graphs = CreateGraphChoice(rc,
            "Arrange graphs:", LIST_TYPE_MULTIPLE);
        ui->add = CreateToggleButton(rc,
            "Add graphs as needed to fill the matrix");
        ui->kill = CreateToggleButton(rc, "Kill extra graphs");

        fr = CreateFrame(arrange_panel, "Matrix");
        gr = CreateGrid(fr, 4, 1);
        ui->ncols = CreateSpinChoice(gr,
            "Cols:", 2, SPIN_TYPE_INT, (double) 1, (double) 99, (double) 1);
        PlaceGridChild(gr, ui->ncols->rc, 0, 0);
        ui->nrows = CreateSpinChoice(gr,
            "Rows:", 2, SPIN_TYPE_INT, (double) 1, (double) 99, (double) 1);
        PlaceGridChild(gr, ui->nrows->rc, 1, 0);
        ui->order = CreateBitmapOptionChoice(gr,
            "Order:", 2, 8, MBITMAP_WIDTH, MBITMAP_HEIGHT, opitems);
        PlaceGridChild(gr, ui->order->menu, 2, 0);
        rc = CreateHContainer(gr);
        ui->snake = CreateToggleButton(rc, "Snake fill");
        PlaceGridChild(gr, rc, 3, 0);

	fr = CreateFrame(arrange_panel, "Page offsets");
        gr = CreateGrid(fr, 3, 3);
        ui->toff = CreateSpinChoice(gr, "", 4, SPIN_TYPE_FLOAT, 0.0, 1.0, 0.05);
        PlaceGridChild(gr, ui->toff->rc, 1, 0);
        ui->loff = CreateSpinChoice(gr, "", 4, SPIN_TYPE_FLOAT, 0.0, 1.0, 0.05);
        PlaceGridChild(gr, ui->loff->rc, 0, 1);
        ui->roff = CreateSpinChoice(gr, "", 4, SPIN_TYPE_FLOAT, 0.0, 1.0, 0.05);
        PlaceGridChild(gr, ui->roff->rc, 2, 1);
        ui->boff = CreateSpinChoice(gr, "", 4, SPIN_TYPE_FLOAT, 0.0, 1.0, 0.05);
        PlaceGridChild(gr, ui->boff->rc, 1, 2);

	fr = CreateFrame(arrange_panel, "Spacing");
        gr = CreateGrid(fr, 2, 1);
        rc = CreateHContainer(gr);
        ui->hgap = CreateSpinChoice(rc,
            "Hgap/width", 3, SPIN_TYPE_FLOAT, 0.0, 9.0, 0.1);
        ui->hpack = CreateToggleButton(rc, "Pack");
        AddToggleButtonCB(ui->hpack, hpack_cb, ui);
        PlaceGridChild(gr, rc, 0, 0);
        rc = CreateHContainer(gr);
        ui->vgap = CreateSpinChoice(rc,
            "Vgap/height", 3, SPIN_TYPE_FLOAT, 0.0, 9.0, 0.1);
        ui->vpack = CreateToggleButton(rc, "Pack");
        AddToggleButtonCB(ui->vpack, vpack_cb, ui);
        PlaceGridChild(gr, rc, 1, 0);
        
	CreateAACDialog(ui->top, arrange_panel, define_arrange_proc, ui);
        
        SetSpinChoice(ui->nrows, (double) 1);
        SetSpinChoice(ui->ncols, (double) 1);
        
        SetSpinChoice(ui->toff, GA_OFFSET_DEFAULT);
        SetSpinChoice(ui->loff, GA_OFFSET_DEFAULT);
        SetSpinChoice(ui->roff, GA_OFFSET_DEFAULT);
        SetSpinChoice(ui->boff, GA_OFFSET_DEFAULT);

        SetSpinChoice(ui->hgap, GA_GAP_DEFAULT);
        SetSpinChoice(ui->vgap, GA_GAP_DEFAULT);
        
        SetToggleButtonState(ui->add, TRUE);
    }

    RaiseWindow(GetParent(ui->top));
    
    unset_wait_cursor();
}

/*
 * Overlay graphs popup routines
 */
static int define_overlay_proc(void *data)
{
    int g1, g2;
    int type = GetOptionChoice(graph_overlaytype_item);
    
    if (GetSingleListChoice(graph_overlay1_choice_item, &g1) != RETURN_SUCCESS) {
	errmsg("Please select a single graph");
	return RETURN_FAILURE;
    }
    
    if (GetSingleListChoice(graph_overlay2_choice_item, &g2) != RETURN_SUCCESS) {
	errmsg("Please select a single graph");
	return RETURN_FAILURE;
    }

    if (g1 == g2) {
	errmsg("Can't overlay a graph onto itself");
	return RETURN_FAILURE;
    }

    overlay_graphs(g1, g2, type);

    update_all();
    xdrawgraph();
    
    return RETURN_SUCCESS;
}

void create_overlay_frame(void *data)
{
    char *label1[2];
    
    set_wait_cursor();
    if (overlay_dialog == NULL) {
        OptionItem opitems[5];
	label1[0] = "Accept";
	label1[1] = "Close";
        
	overlay_dialog = CreateDialogForm(app_shell, "Overlay graphs");
	
        graph_overlay1_choice_item = CreateGraphChoice(overlay_dialog,
            "Overlay graph:", LIST_TYPE_SINGLE);
	AddDialogFormChild(overlay_dialog, graph_overlay1_choice_item->rc);
        graph_overlay2_choice_item = CreateGraphChoice(overlay_dialog,
            "Onto graph:", LIST_TYPE_SINGLE);
	AddDialogFormChild(overlay_dialog, graph_overlay2_choice_item->rc);
	
        opitems[0].value = GOVERLAY_SMART_AXES_DISABLED;
        opitems[0].label = "Disabled";
        opitems[1].value = GOVERLAY_SMART_AXES_NONE;
        opitems[1].label = "X and Y axes different";
        opitems[2].value = GOVERLAY_SMART_AXES_X;
        opitems[2].label = "Same X axis scaling";
        opitems[3].value = GOVERLAY_SMART_AXES_Y;
        opitems[3].label = "Same Y axis scaling";
        opitems[4].value = GOVERLAY_SMART_AXES_XY;
        opitems[4].label = "Same X and Y axis scaling";
        graph_overlaytype_item = CreateOptionChoice(overlay_dialog,
            "Smart axis hints:", 0, 5, opitems);

	CreateAACDialog(overlay_dialog,
            graph_overlaytype_item->menu, define_overlay_proc, NULL);
    }

    RaiseWindow(GetParent(overlay_dialog));
    unset_wait_cursor();
}

/*
 * autoscale popup
 */
typedef struct _Auto_ui {
    Widget top;
    SetChoiceItem sel;
    OptionStructure *on_item;
    Widget *applyto_item;
} Auto_ui;

static Auto_ui aui;

static int define_autos_proc(void *data)
{
    int aon, au, ap;
    Auto_ui *ui = (Auto_ui *) data;
    
    aon = GetOptionChoice(ui->on_item);
    ap = GetChoice(ui->applyto_item);
    au = GetSelectedSet(ui->sel);
    if (au == SET_SELECT_ERROR) {
        errmsg("No set selected");
        return RETURN_FAILURE;
    }
    if (au == SET_SELECT_ALL) {
      au = -1;
    }
    
    define_autos(aon, au, ap);
    
    return RETURN_SUCCESS;
}

void create_autos_frame(void *data)
{
    set_wait_cursor();
    
    if (aui.top == NULL) {
	Widget rc;
        
        aui.top = CreateDialogForm(app_shell, "Autoscale graphs");

	rc = CreateVContainer(aui.top);
        aui.on_item = CreateASChoice(rc, "Autoscale:");
        aui.sel = CreateSetSelector(rc, "Use set:",
                                    SET_SELECT_ALL,
                                    FILTER_SELECT_NONE,
                                    GRAPH_SELECT_CURRENT,
                                    SELECTION_TYPE_SINGLE);
	aui.applyto_item = CreatePanelChoice(rc, "Apply to graph:",
					     3,
					     "Current",
					     "All",
					     NULL);

	CreateAACDialog(aui.top, rc, define_autos_proc, &aui);
    }
    
    RaiseWindow(GetParent(aui.top));
    unset_wait_cursor();
}

void define_autos(int aon, int au, int ap)
{
    int i, ming, maxg;
    int cg = get_cg();

    if (au >= 0 && !is_set_active(cg, au)) {
	errmsg("Set not active");
	return;
    }
    if (ap) {
	ming = 0;
	maxg = number_of_graphs() - 1;
    } else {
	ming = cg;
	maxg = cg;
    }
    if (ming == cg && maxg == cg) {
	if (!is_graph_active(cg)) {
	    errmsg("Current graph is not active!");
	    return;
	}
    }
    for (i = ming; i <= maxg; i++) {
	if (is_graph_active(i)) {
	    autoscale_byset(i, au, aon);
	}
    }
    update_ticks(cg);
    xdrawgraph();
}
