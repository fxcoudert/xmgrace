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
 * Locator Panel
 *
 */

#include <config.h>

#include <stdio.h>

#include "graphs.h"
#include "protos.h"
#include "motifinc.h"


static Widget locator_frame;

/*
 * Panel item declarations
 */
static Widget *delta_item;
static OptionStructure *loc_formatx;
static OptionStructure *loc_formaty;
static Widget *loc_precx;
static Widget *loc_precy;
static Widget locx_item;
static Widget locy_item;
static Widget fixedp_item;

/*
 * Event and Notify proc declarations
 */

static int locator_define_notify_proc(void *data);

void update_locator_items(int gno)
{
    if (locator_frame) {
        GLocator locator;
        char buf[32];

        if (get_graph_locator(gno, &locator) != RETURN_SUCCESS) {
            return;
        }
        
	SetToggleButtonState(fixedp_item, locator.pointset);
	SetChoice(delta_item, locator.pt_type);
	SetOptionChoice(loc_formatx, locator.fx);
	SetOptionChoice(loc_formaty, locator.fy);
	SetChoice(loc_precx, locator.px);
	SetChoice(loc_precy, locator.py);
	sprintf(buf, "%g", locator.dsx);
	xv_setstr(locx_item, buf);
	sprintf(buf, "%g", locator.dsy);
	xv_setstr(locy_item, buf);
    }
}


/*
 * Create the locator Panel
 */
void create_locator_frame(void *data)
{
    set_wait_cursor();
    
    if (locator_frame == NULL) {
        Widget rc, rc2, fr, locator_panel;
	
        locator_frame = CreateDialogForm(app_shell, "Locator props");

        locator_panel = CreateVContainer(locator_frame);
	
	delta_item = CreatePanelChoice(locator_panel, "Locator display type:",
				       7,
				       "[X, Y]",
				       "[DX, DY]",
				       "[DISTANCE]",
				       "[Phi, Rho]",
				       "[VX, VY]",
				       "[SX, SY]",
				       NULL);
	
	fr = CreateFrame(locator_panel, "X properties");
	rc = CreateVContainer(fr);
	loc_formatx = CreateFormatChoice(rc, "Format:");
	loc_precx = CreatePrecisionChoice(rc, "Precision:");

	fr = CreateFrame(locator_panel, "Y properties");
	rc = CreateVContainer(fr);
	loc_formaty = CreateFormatChoice(rc, "Format:");
	loc_precy = CreatePrecisionChoice(rc, "Precision:");

        fr = CreateFrame(locator_panel, "Fixed point");
	rc = CreateVContainer(fr);
	fixedp_item = CreateToggleButton(rc, "Enable");
	rc2 = CreateHContainer(rc);
	locx_item = CreateTextItem2(rc2, 10, "X:");
	locy_item = CreateTextItem2(rc2, 10, "Y:");

	CreateAACDialog(locator_frame,
            locator_panel, locator_define_notify_proc, NULL);
    }
    
    update_locator_items(get_cg());
    RaiseWindow(GetParent(locator_frame));
    
    unset_wait_cursor();
}

/*
 * Notify and event procs
 */

static int locator_define_notify_proc(void *data)
{
    GLocator locator;
    int gno;

    gno = get_cg();
    
    if (get_graph_locator(gno, &locator) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }
    
    locator.pt_type = GetChoice(delta_item);
    locator.fx = GetOptionChoice(loc_formatx);
    locator.fy = GetOptionChoice(loc_formaty);
    locator.px = GetChoice(loc_precx);
    locator.py = GetChoice(loc_precy);
    locator.pointset = GetToggleButtonState(fixedp_item);
    xv_evalexpr(locx_item, &locator.dsx ); 
    xv_evalexpr(locy_item, &locator.dsy ); 
    set_graph_locator(gno, &locator);
    
    xdrawgraph();
    
    return RETURN_SUCCESS;
}
