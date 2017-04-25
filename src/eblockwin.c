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
 * Edit block data Panel
 *
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include "graphs.h"
#include "utils.h"
#include "ssdata.h"
#include "protos.h"
#include "motifinc.h"

static int block_curtype = SET_XY;

static Widget eblock_frame;
static Widget eblock_panel;

/*
 * Panel item declarations
 */
static OptionStructure *eblock_nchoice_items[MAX_SET_COLS];
static OptionStructure *eblock_schoice_item;
static Widget eblock_ncols_item;
static OptionStructure *eblock_type_choice_item;
static GraphSetStructure *eblock_graphset_item;
static OptionStructure *auto_item;

/*
 * Event and Notify proc declarations
 */
static void eblock_type_notify_proc(int value, void *data);
static int eblock_accept_notify_proc(void *data);
static void update_eblock(int gno);

/*
 * Create the block data panel
 */
void create_eblock_frame(int gno)
{
    if (get_blockncols() == 0) {
	errmsg("Need to read block data first");
	return;
    }
    
    set_wait_cursor();
    if (eblock_frame == NULL) {
        int i;
        char buf[32];
        Widget rc, fr;
        OptionItem blockitem;

        blockitem.value = 0;
        blockitem.label = "Index";

	eblock_frame = CreateDialogForm(app_shell, "Edit block data");

	eblock_panel = CreateVContainer(eblock_frame);

	fr = CreateFrame(eblock_panel, NULL);
        eblock_ncols_item = CreateLabel(fr, "tmp");

	eblock_graphset_item =
            CreateGraphSetSelector(eblock_panel, "Load to:", LIST_TYPE_SINGLE);

        fr = CreateFrame(eblock_panel, NULL);
        rc = CreateVContainer(fr);

	eblock_type_choice_item = CreateSetTypeChoice(rc, "Set type:");
        AddOptionChoiceCB(eblock_type_choice_item, eblock_type_notify_proc, NULL);

	for (i = 0; i < MAX_SET_COLS; i++) {
            sprintf(buf, "%s from column:", dataset_colname(i));
            eblock_nchoice_items[i] = CreateOptionChoice(rc,
                buf, 3, 1, &blockitem);
        }
        eblock_schoice_item = CreateOptionChoice(rc,
            "Strings from column:", 1, 1, &blockitem);

	auto_item = CreateASChoice(eblock_panel, "Autoscale graph on load:");

	CreateAACDialog(eblock_frame,
            eblock_panel, eblock_accept_notify_proc, NULL);
    }
    update_eblock(gno);
    
    RaiseWindow(GetParent(eblock_frame));
    unset_wait_cursor();
}

/*
 * Notify and event procs
 */

static void update_eblock(int gno)
{
    int blocklen, blockncols;
    int *blockformats;
    int i, ncols, nncols, nscols;
    char buf[128];

    OptionItem *blockitems, *sblockitems;
    
    if (eblock_frame == NULL) {
	return;
    }
    blockncols = get_blockncols();
    if (blockncols == 0) {
	errmsg("Need to read block data first");
	return;
    }
    blocklen = get_blocknrows();
    blockformats = get_blockformats();
    if (is_valid_gno(gno)) {
        SelectListChoice(eblock_graphset_item->graph_sel, gno);
    }
    sprintf(buf, "Block data: %d column(s) of length %d",
        blockncols, blocklen);
    SetLabel(eblock_ncols_item, buf);
    
    /* TODO: check if new data arrived */
    if (1) {
        blockitems  = xmalloc((blockncols + 1)*sizeof(OptionItem));
        sblockitems = xmalloc((blockncols + 1)*sizeof(OptionItem));
        blockitems[0].value = -1;
        blockitems[0].label = copy_string(NULL, "Index");
        sblockitems[0].value = -1;
        sblockitems[0].label = copy_string(NULL, "None");
        nncols = 0;
        nscols = 0;
        for (i = 0; i < blockncols; i++) {
            sprintf(buf, "%d", i + 1);
            if (blockformats[i] != FFORMAT_STRING) {
                nncols++;
                blockitems[nncols].value = i;
                blockitems[nncols].label = copy_string(NULL, buf);
            } else {
                nscols++;
                sblockitems[nscols].value = i;
                sblockitems[nscols].label = copy_string(NULL, buf);
            }
        }
        for (i = 0; i < MAX_SET_COLS; i++) {
            int oldchoice = GetOptionChoice(eblock_nchoice_items[i]);
            UpdateOptionChoice(eblock_nchoice_items[i],
                nncols + 1, blockitems);
            if (oldchoice < blockncols) {
                SetOptionChoice(eblock_nchoice_items[i], oldchoice);
            } else if (i < blockncols) {
                SetOptionChoice(eblock_nchoice_items[i], i);
            }
        }
        UpdateOptionChoice(eblock_schoice_item, nscols + 1, sblockitems);

        for (i = 0; i < nncols + 1; i++) {
            xfree(blockitems[i].label);
        }
        xfree(blockitems);
        for (i = 0; i < nscols + 1; i++) {
            xfree(sblockitems[i].label);
        }
        xfree(sblockitems);
    }

    ncols = settype_cols(block_curtype);
    for (i = 0; i < MAX_SET_COLS; i++) {
        SetSensitive(eblock_nchoice_items[i]->menu, (i < ncols));
    }
}

static void eblock_type_notify_proc(int value, void *data)
{
    block_curtype = value;

    update_eblock(-1);
}

static int eblock_accept_notify_proc(void *data)
{
    int i, gno, setno;
    int cs[MAX_SET_COLS], nncols, scol, autoscale;

    if (GetSingleListChoice(eblock_graphset_item->graph_sel, &gno)
        != RETURN_SUCCESS) {
        errmsg("Please select a single graph");
        return RETURN_FAILURE;
    }
    if (GetSingleListChoice(eblock_graphset_item->set_sel, &setno) !=
        RETURN_SUCCESS) {
    	/* no set selected; allocate new one */
    	setno = NEW_SET;
    }
    
    nncols = settype_cols(block_curtype);
    for (i = 0; i < nncols; i++) {
        cs[i] = GetOptionChoice(eblock_nchoice_items[i]);
    }
    scol = GetOptionChoice(eblock_schoice_item);

    autoscale = GetOptionChoice(auto_item);

    create_set_fromblock(gno, setno,
        block_curtype, nncols, cs, scol, autoscale);

    update_all();
    xdrawgraph();
    
    return RETURN_SUCCESS;
}
