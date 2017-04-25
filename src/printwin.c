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
 * Page/Device setup
 */

#include <config.h>

#include <stdio.h>

#include "cmath.h"
#include "globals.h"
#include "device.h"
#include "utils.h"
#include "graphutils.h"
#include "plotone.h"

#include "motifinc.h"
#include "protos.h"


extern char print_file[];

static int current_page_units = 0;

static Widget psetup_frame;
static Widget psetup_rc;
static Widget device_opts_item;
static Widget printto_item;
static Widget print_string_item;
static Widget rc_filesel;
static Widget printfile_item;
static Widget pdev_rc;
static OptionStructure *devices_item;
static Widget output_frame;
static OptionStructure *page_orient_item;
static OptionStructure *page_format_item;
static Widget page_x_item;
static Widget page_y_item;
static OptionStructure *page_size_unit_item;
static Widget dev_res_item;
static Widget fontaa_item;
static Widget devfont_item;
static Widget dsync_item, psync_item;

static void do_pr_toggle(int onoff, void *data);
static void do_format_toggle(int value, void *data);
static void do_orient_toggle(int value, void *data);

static int set_printer_proc(void *data);
void create_printfiles_popup(void *data);
void create_devopts_popup(void *data);

static void do_device_toggle(int value, void *data);
static void do_units_toggle(int value, void *data);
static void update_printer_setup(int device_id);
static void update_device_setup(int device_id);

static void do_print_cb(void *data);

void create_printer_setup(void *data)
{
    int device;
    
    set_wait_cursor();
    
    if (data == NULL) {
        device = hdevice;
    } else {
        device = *((int *) data);
    }
    
    if (psetup_frame == NULL) {
        int i, ndev;
        Widget rc, rc1, fr, wbut;
        Widget menubar, menupane;
        OptionItem *option_items;

	psetup_frame = CreateDialogForm(app_shell, "Device setup");
        SetDialogFormResizable(psetup_frame, TRUE);

        menubar = CreateMenuBar(psetup_frame);
        AddDialogFormChild(psetup_frame, menubar);
        
        menupane = CreateMenu(menubar, "File", 'F', FALSE);
        CreateMenuButton(menupane, "Print", 'P', do_print_cb, NULL);
        CreateMenuSeparator(menupane);
        CreateMenuCloseButton(menupane, psetup_frame);

        menupane = CreateMenu(menubar, "Options", 'O', FALSE);
        dsync_item = CreateMenuToggle(menupane,
            "Sync page size of all devices", 'S', NULL, NULL);
        SetToggleButtonState(dsync_item, TRUE);
        psync_item = CreateMenuToggle(menupane,
            "Rescale plot on page size change", 'R', NULL, NULL);
        SetToggleButtonState(psync_item, FALSE);

        menupane = CreateMenu(menubar, "Help", 'H', TRUE);
        CreateMenuHelpButton(menupane, "On device setup", 'd',
            psetup_frame, "doc/UsersGuide.html#print-setup");

        ManageChild(menubar);

	psetup_rc = CreateVContainer(psetup_frame);

        fr = CreateFrame(psetup_rc, "Device setup");
        rc1 = CreateVContainer(fr);
	pdev_rc = CreateHContainer(rc1);

	ndev = number_of_devices();
        option_items = xmalloc(ndev*sizeof(OptionItem));
        for (i = 0; i < ndev; i++) {
            option_items[i].value = i;
            option_items[i].label = get_device_name(i);
        }
        devices_item =
            CreateOptionChoice(pdev_rc, "Device: ", 1, ndev, option_items);
	AddOptionChoiceCB(devices_item, do_device_toggle, NULL);
        xfree(option_items);
        
        device_opts_item = CreateButton(pdev_rc, "Device options...");
	AddButtonCB(device_opts_item, create_devopts_popup, NULL);
        
        output_frame = CreateFrame(psetup_rc, "Output");
        rc1 = CreateVContainer(output_frame);
	printto_item = CreateToggleButton(rc1, "Print to file");
        AddToggleButtonCB(printto_item, do_pr_toggle, NULL);

	print_string_item = CreateTextItem2(rc1, 25, "Print command:");

	rc_filesel = CreateHContainer(rc1);
	printfile_item = CreateTextItem2(rc_filesel, 20, "File name:");
	wbut = CreateButton(rc_filesel, "Browse...");
	AddButtonCB(wbut, create_printfiles_popup, NULL);

	
        fr = CreateFrame(psetup_rc, "Page");
        rc1 = CreateVContainer(fr);
        
	rc = CreateHContainer(rc1);

        option_items = xmalloc(2*sizeof(OptionItem));
        option_items[0].value = PAGE_ORIENT_LANDSCAPE;
        option_items[0].label = "Landscape";
        option_items[1].value = PAGE_ORIENT_PORTRAIT;
        option_items[1].label = "Portrait";
        page_orient_item =
            CreateOptionChoice(rc, "Orientation: ", 1, 2, option_items);
	AddOptionChoiceCB(page_orient_item, do_orient_toggle, NULL);
        xfree(option_items);


        option_items = xmalloc(3*sizeof(OptionItem));
        option_items[0].value = PAGE_FORMAT_CUSTOM;
        option_items[0].label = "Custom";
        option_items[1].value = PAGE_FORMAT_USLETTER;
        option_items[1].label = "Letter";
        option_items[2].value = PAGE_FORMAT_A4;
        option_items[2].label = "A4";
        page_format_item =
            CreateOptionChoice(rc, "Size: ", 1, 3, option_items);
	AddOptionChoiceCB(page_format_item, do_format_toggle, NULL);
        xfree(option_items);

	rc = CreateHContainer(rc1);
        page_x_item = CreateTextItem2(rc, 7, "Dimensions:");
        page_y_item = CreateTextItem2(rc, 7, "x ");
        option_items = xmalloc(3*sizeof(OptionItem));
        option_items[0].value = 0;
        option_items[0].label = "pix";
        option_items[1].value = 1;
        option_items[1].label = "in";
        option_items[2].value = 2;
        option_items[2].label = "cm";
        page_size_unit_item =
            CreateOptionChoice(rc, " ", 1, 3, option_items);
	AddOptionChoiceCB(page_size_unit_item, do_units_toggle, NULL);
        xfree(option_items);
        SetOptionChoice(page_size_unit_item, current_page_units);

        dev_res_item = CreateTextItem2(rc1, 4, "Resolution (dpi):");


        fr = CreateFrame(psetup_rc, "Fonts");
        rc1 = CreateVContainer(fr);
	fontaa_item = CreateToggleButton(rc1, "Enable font antialiasing");
	devfont_item = CreateToggleButton(rc1, "Use device fonts");
        
	CreateAACDialog(psetup_frame, psetup_rc, set_printer_proc, NULL);
    }
    
    update_printer_setup(device);
    
    RaiseWindow(GetParent(psetup_frame));
    unset_wait_cursor();
}

static void update_printer_setup(int device_id)
{
    if (psetup_frame) {
        SetOptionChoice(devices_item, device_id);
        update_device_setup(device_id);
    }
}

static void update_device_setup(int device_id)
{
    char buf[GR_MAXPATHLEN], *bufptr;
    int page_units;
    double page_x, page_y;
    PageFormat pf;
    
    Page_geometry pg;
    Device_entry dev;
    
    if (psetup_frame) {
	dev = get_device_props(device_id);
        pg = dev.pg;
        	
        if (dev.setup == NULL) {
            SetSensitive(device_opts_item, False);
        } else {
            SetSensitive(device_opts_item, True);
        }

        if (print_file == NULL || print_file[0] == '\0') {
            strcpy(print_file, mybasename(get_docname())); 
        }
        
        /* Replace existing filename extension */
        bufptr = strrchr(print_file, '.');
        if (bufptr) {
            *(bufptr + 1) = '\0';
        } else {
            strcat(print_file, ".");
        }
        strcat(print_file, dev.fext);
        
        xv_setstr(printfile_item, print_file);
                
        xv_setstr(print_string_item, get_print_cmd());
        
        switch (dev.type) {
        case DEVICE_TERM:
            UnmanageChild(output_frame);
            break;
        case DEVICE_FILE:
            ManageChild(output_frame);
            SetToggleButtonState(printto_item, TRUE);
            SetSensitive(printto_item, False);
            SetSensitive(GetParent(print_string_item), False);
            SetSensitive(rc_filesel, True);
            break;
        case DEVICE_PRINT:
            ManageChild(output_frame);
            SetToggleButtonState(printto_item, get_ptofile());
            SetSensitive(printto_item, True);
            if (get_ptofile() == TRUE) {
                SetSensitive(rc_filesel, True);
                SetSensitive(GetParent(print_string_item), False);
            } else {
                SetSensitive(rc_filesel, False);
                SetSensitive(GetParent(print_string_item), True);
            }
            break;
        }
        
        SetOptionChoice(page_orient_item, pg.width < pg.height ?
            PAGE_ORIENT_PORTRAIT : PAGE_ORIENT_LANDSCAPE);
        
        pf = get_page_format(device_id);
        SetOptionChoice(page_format_item, pf); 
        if (pf == PAGE_FORMAT_CUSTOM) {
            SetSensitive(page_x_item, True);
            SetSensitive(page_y_item, True);
            SetSensitive(page_orient_item->menu, False);
        } else {
            SetSensitive(page_x_item, False);
            SetSensitive(page_y_item, False);
            SetSensitive(page_orient_item->menu, True);
        }
        
        sprintf (buf, "%.0f", pg.dpi); 
        xv_setstr(dev_res_item, buf);
        
        page_units = GetOptionChoice(page_size_unit_item);
        
        switch (page_units) {
        case 0:     /* pixels */
            page_x = (float) pg.width;
            page_y = (float) pg.height;
            break;
        case 1:      /* inches */
            page_x = (float) pg.width / pg.dpi;
            page_y = (float) pg.height / pg.dpi;
            break;
        case 2:      /* cm */ 
            page_x = (float) CM_PER_INCH * pg.width / pg.dpi;
            page_y = (float) CM_PER_INCH * pg.height / pg.dpi;
            break;
        default:
            errmsg("Internal error");
            return;
        }
        
        sprintf (buf, "%.2f", page_x); 
        xv_setstr(page_x_item, buf);
        sprintf (buf, "%.2f", page_y); 
        xv_setstr(page_y_item, buf);
        
        SetToggleButtonState(fontaa_item, dev.fontaa);
        
        SetToggleButtonState(devfont_item, dev.devfonts);
    }
}

static int set_printer_proc(void *data)
{
    int seldevice;
    double page_x, page_y;
    double dpi;
    int page_units;
    Device_entry dev;
    Page_geometry pg;
    int do_redraw = FALSE;
    
    seldevice = GetOptionChoice(devices_item);

    dev = get_device_props(seldevice);

    if (dev.type != DEVICE_TERM) {
        hdevice = seldevice;
        set_ptofile(GetToggleButtonState(printto_item));
        if (get_ptofile()) {
            strcpy(print_file, xv_getstr(printfile_item));
        } else {
            set_print_cmd(xv_getstr(print_string_item));
        }
    }
    
    dev.devfonts = GetToggleButtonState(devfont_item);
    dev.fontaa = GetToggleButtonState(fontaa_item);
    
    if (xv_evalexpr(page_x_item, &page_x) != RETURN_SUCCESS || 
        xv_evalexpr(page_y_item, &page_y) != RETURN_SUCCESS  ) {
        errmsg("Invalid page dimension(s)");
        return RETURN_FAILURE;
    }

    if (xv_evalexpr(dev_res_item, &dpi) != RETURN_SUCCESS || dpi <= 0.0) {
        errmsg("Invalid dpi");
        return RETURN_FAILURE;
    }

    page_units = GetOptionChoice(page_size_unit_item);

    switch (page_units) {
    case 0: 
        pg.width =  (long) page_x;
        pg.height = (long) page_y;
        break;
    case 1: 
        pg.width =  (long) (page_x * dpi);
        pg.height = (long) (page_y * dpi);
        break;
    case 2: 
        pg.width =  (long) (page_x * dpi / CM_PER_INCH);
        pg.height = (long) (page_y * dpi / CM_PER_INCH);
        break;
    default:
        errmsg("Internal error");
        return RETURN_FAILURE;
    }
    
    pg.dpi = dpi;
    
    if (GetToggleButtonState(dsync_item) == TRUE) {
        set_page_dimensions((int) rint(72.0*pg.width/pg.dpi),
                            (int) rint(72.0*pg.height/pg.dpi),
                            GetToggleButtonState(psync_item) == TRUE);
        do_redraw = TRUE;
    }
    
    dev.pg = pg;
    if (set_device_props(seldevice, dev) != RETURN_SUCCESS) {
        errmsg("Invalid page dimensions or DPI");
        return RETURN_FAILURE;
    }
    
    if (seldevice == tdevice) {
        do_redraw = TRUE;
    }
    
    if (do_redraw) {
        drawgraph();
    }
    
    return RETURN_SUCCESS;
}


/*
 * set the print options
 */
static void do_device_toggle(int value, void *data)
{ 
    update_device_setup(value);
}

static void do_pr_toggle(int onoff, void *data)
{
    if (onoff == TRUE) {
        SetSensitive(rc_filesel, True);
        SetSensitive(GetParent(print_string_item), False);
    } else {
        SetSensitive(rc_filesel, False);
        SetSensitive(GetParent(print_string_item), True);
    }
}

static void do_format_toggle(int value, void *data)
{
    int orientation;
    int x, y;
    double px, py;
    int page_units;
    double dpi;
    char buf[32];
    
    if (value == PAGE_FORMAT_CUSTOM) {
        SetSensitive(page_x_item, True);
        SetSensitive(page_y_item, True);
        SetSensitive(page_orient_item->menu, False);
    } else {
        SetSensitive(page_x_item, False);
        SetSensitive(page_y_item, False);
        SetSensitive(page_orient_item->menu, True);
    }
    
    
    switch (value) {
    case PAGE_FORMAT_USLETTER:
        x = 612;
        y = 792;
        break;
    case PAGE_FORMAT_A4:
        x = 595;
        y = 842;
        break;
    case PAGE_FORMAT_CUSTOM:
    default:
        return;
    }

    
    page_units = GetOptionChoice(page_size_unit_item);
    
    switch (page_units) {
    case 0:      /* pixels */
        if (xv_evalexpr(dev_res_item, &dpi) != RETURN_SUCCESS) {
            errmsg("Invalid dpi");
            return;
        }
        px = (float) x*dpi/72.0;
        py = (float) y*dpi/72.0;
        break;
    case 1:      /* inches */
        px = (float) x/72.0;
        py = (float) y/72.0;
        break;
    case 2:      /* cm */ 
        px = (float) x/72.0*CM_PER_INCH;
        py = (float) y/72.0*CM_PER_INCH;
        break;
    default:
        errmsg("Internal error");
        return;
    }
    
    orientation = GetOptionChoice(page_orient_item);
    
    if ((orientation == PAGE_ORIENT_LANDSCAPE && px > py) ||
        (orientation == PAGE_ORIENT_PORTRAIT  && px < py) ) {
        sprintf (buf, "%.2f", px);
        xv_setstr(page_x_item, buf);
        sprintf (buf, "%.2f", py);
        xv_setstr(page_y_item, buf);
    } else {
        sprintf (buf, "%.2f", py);
        xv_setstr(page_x_item, buf);
        sprintf (buf, "%.2f", px);
        xv_setstr(page_y_item, buf);
    }
}

static void do_orient_toggle(int value, void *data)
{
    int orientation = value;
    double px, py;
    char buf[32];

    if (xv_evalexpr(page_x_item, &px) != RETURN_SUCCESS || 
        xv_evalexpr(page_y_item, &py) != RETURN_SUCCESS ) {
        errmsg("Invalid page dimension(s)");
        return;
    }
    
    if ((orientation == PAGE_ORIENT_LANDSCAPE && px > py) ||
        (orientation == PAGE_ORIENT_PORTRAIT  && px < py) ) {
        sprintf (buf, "%.2f", px);
        xv_setstr(page_x_item, buf);
        sprintf (buf, "%.2f", py);
        xv_setstr(page_y_item, buf);
    } else {
        sprintf (buf, "%.2f", py);
        xv_setstr(page_x_item, buf);
        sprintf (buf, "%.2f", px);
        xv_setstr(page_y_item, buf);
    }
}

static int do_prfilesel_proc(char *filename, void *data)
{
    xv_setstr(printfile_item, filename);
    strcpy(print_file, filename);
    XtVaSetValues(printfile_item, XmNcursorPosition, strlen(filename), NULL);
    return TRUE;
}

void create_printfiles_popup(void *data)
{
    static FSBStructure *fsb = NULL;
    int device;
    Device_entry dev;
    char buf[16];

    set_wait_cursor();

    if (fsb == NULL) {
        fsb = CreateFileSelectionBox(app_shell, "Select print file");
	AddFileSelectionBoxCB(fsb, do_prfilesel_proc, NULL);
        ManageChild(fsb->FSB);
    }

    device = GetOptionChoice(devices_item);
    dev = get_device_props(device);

    sprintf(buf, "*.%s", dev.fext);
    SetFileSelectionBoxPattern(fsb, buf);
    
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

void create_devopts_popup(void *data)
{
    int device_id;
    Device_entry dev;
    
    device_id = GetOptionChoice(devices_item);
    dev = get_device_props(device_id);
    if (dev.setup == NULL) {
        /* Should never come to here */
        errmsg("No options can be set for this device");
    } else {
        (dev.setup)();
    }
}

static void do_units_toggle(int value, void *data)
{
    char buf[32];
    double page_x, page_y;
    double dev_res;
    int page_units = value;
    
    if (xv_evalexpr(page_x_item, &page_x) != RETURN_SUCCESS || 
        xv_evalexpr(page_y_item, &page_y) != RETURN_SUCCESS ) {
        errmsg("Invalid page dimension(s)");
        return;
    }
    
    if (xv_evalexpr(dev_res_item, &dev_res) != RETURN_SUCCESS) {
        errmsg("Invalid device resolution(s)");
        return;
    }
    
    if (dev_res <= 0.0) {
        errmsg("Device resolution(s) <= 0");
        return;
    }
    
    if (current_page_units == page_units) {
        ;
    } else if (current_page_units == 0 && page_units == 1) {
        page_x /= dev_res;
        page_y /= dev_res;
    } else if (current_page_units == 0 && page_units == 2) {
        page_x /= (dev_res/CM_PER_INCH);
        page_y /= (dev_res/CM_PER_INCH);
    } else if (current_page_units == 1 && page_units == 0) {
        page_x *= dev_res;
        page_y *= dev_res;
    } else if (current_page_units == 1 && page_units == 2) {
        page_x *= CM_PER_INCH;
        page_y *= CM_PER_INCH;
    } else if (current_page_units == 2 && page_units == 0) {
        page_x *= (dev_res/CM_PER_INCH);
        page_y *= (dev_res/CM_PER_INCH);
    } else if (current_page_units == 2 && page_units == 1) {
        page_x /= CM_PER_INCH;
        page_y /= CM_PER_INCH;
    } else {
        errmsg("Internal error");
        return;
    }
        
    current_page_units = page_units;
    
    sprintf (buf, "%.2f", page_x); 
    xv_setstr(page_x_item, buf);
    sprintf (buf, "%.2f", page_y); 
    xv_setstr(page_y_item, buf);
}

static void do_print_cb(void *data)
{
    set_wait_cursor();
    do_hardcopy();
    unset_wait_cursor();
}
