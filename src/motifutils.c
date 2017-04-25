/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2003 Grace Development Team
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
 * utilities for Motif
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/BulletinB.h>
#include <Xm/MessageB.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/CascadeBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ArrowBG.h>

#ifdef WITH_EDITRES
#  include <X11/Xmu/Editres.h>
#endif

#if XmVersion < 2000
#  define XmStringConcatAndFree(a, b) XmStringConcat(a, b); XmStringFree(a); XmStringFree(b)
#endif

#include "Tab.h"
#include "motifinc.h"

#include "defines.h"
#include "globals.h"
#include "draw.h"
#include "patterns.h"
#include "jbitmaps.h"
#include "t1fonts.h"
#include "graphs.h"
#include "utils.h"
#include "events.h"
#include "parser.h"
#include "protos.h"

static XmStringCharSet charset = XmFONTLIST_DEFAULT_TAG;

/* lookup table to determine if character is a floating point digit 
 * only allowable char's [0-9.eE]
 */
unsigned char fpdigit[256] = {  
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
			      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


extern Display *disp;
extern Window root;
extern int depth;

extern XtAppContext app_con;

extern unsigned long xvlibcolors[];


static OptionItem *color_option_items = NULL;
static int ncolor_option_items = 0;
static OptionStructure **color_selectors = NULL;
static int ncolor_selectors = 0;

static char *label_to_resname(const char *s, const char *suffix)
{
    char *retval, *rs;
    int capitalize = FALSE;
    
    retval = copy_string(NULL, s);
    rs = retval;
    while (*s) {
        if (isalnum(*s)) {
            if (capitalize == TRUE) {
                *rs = toupper(*s);
                capitalize = FALSE;
            } else {
                *rs = tolower(*s);
            }
            rs++;
        } else {
            capitalize = TRUE;
        }
        s++;
    }
    *rs = '\0';
    if (suffix != NULL) {
        retval = concat_strings(retval, suffix);
    }
    return retval;
}


void ManageChild(Widget w)
{
    XtManageChild(w);
}

void UnmanageChild(Widget w)
{
    XtUnmanageChild(w);
}

void SetSensitive(Widget w, int onoff)
{
    XtSetSensitive(w, onoff ? True : False);
}

Widget GetParent(Widget w)
{
    if (w) {
        return (XtParent(w));
    } else {
        errmsg("Internal error: GetParent() called with NULL widget");
        return NULL;
    }
}

void RegisterEditRes(Widget shell)
{
#ifdef WITH_EDITRES    
    XtAddEventHandler(shell, (EventMask) 0, True, _XEditResCheckMessages, NULL);
#endif
}

void SetDimensions(Widget w, unsigned int width, unsigned int height)
{
    XtVaSetValues(w,
        XmNwidth, (Dimension) width,
        XmNheight, (Dimension) height,
        NULL);
}

void GetDimensions(Widget w, unsigned int *width, unsigned int *height)
{
    Dimension ww, wh;

    XtVaGetValues(w,
        XmNwidth, &ww,
        XmNheight, &wh,
        NULL);

    *width  = (unsigned int) ww;
    *height = (unsigned int) wh;
}

#define MAX_PULLDOWN_LENGTH 30

OptionStructure *CreateOptionChoice(Widget parent, char *labelstr, int ncols,
                                                int nchoices, OptionItem *items)
{
    Arg args[2];
    XmString str;
    OptionStructure *retval;

    retval = xmalloc(sizeof(OptionStructure));

    XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
    retval->pulldown = XmCreatePulldownMenu(parent, "pulldownMenu", args, 1);

    retval->ncols = ncols;
    
    retval->nchoices = 0;
    retval->options = NULL;
    UpdateOptionChoice(retval, nchoices, items);

    str = XmStringCreateLocalized(labelstr);
    XtSetArg(args[0], XmNlabelString, str);
    XtSetArg(args[1], XmNsubMenuId, retval->pulldown);

    retval->menu = XmCreateOptionMenu(parent, "optionMenu", args, 2);

    XmStringFree(str);

    XtManageChild(retval->menu);
    
    return retval;
}

void UpdateOptionChoice(OptionStructure *optp, int nchoices, OptionItem *items)
{
    int i, nold, ncols, nw;
    Widget *wlist;
    
    nold = optp->nchoices;

    if (optp->ncols == 0) {
        ncols = 1;
    } else {
        ncols = optp->ncols;
    }
    
    /* Don't create too tall pulldowns */
    if (nchoices > MAX_PULLDOWN_LENGTH*ncols) {
        ncols = (nchoices + MAX_PULLDOWN_LENGTH - 1)/MAX_PULLDOWN_LENGTH;
    }
    
    XtVaSetValues(optp->pulldown, XmNnumColumns, ncols, NULL);

    nw = nold - nchoices;
    if (nw > 0) {
        /* Unmanage extra items before destroying to speed the things up */
        wlist = xmalloc(nw*sizeof(Widget));
        for (i = nchoices; i < nold; i++) {
            wlist[i - nchoices] = optp->options[i].widget;
        }
        XtUnmanageChildren(wlist, nw);
        xfree(wlist);
        
        for (i = nchoices; i < nold; i++) {
            XtDestroyWidget(optp->options[i].widget);
        }
    }

    optp->options = xrealloc(optp->options, nchoices*sizeof(OptionWidgetItem));
    optp->nchoices = nchoices;

    for (i = nold; i < nchoices; i++) {
        optp->options[i].widget = 
                  XmCreatePushButton(optp->pulldown, "button", NULL, 0);
    }
    
    for (i = 0; i < nchoices; i++) {
	optp->options[i].value = items[i].value;
	if (items[i].label != NULL) {
            XmString str, ostr;
            XtVaGetValues(optp->options[i].widget, XmNlabelString, &ostr, NULL);
            str = XmStringCreateLocalized(items[i].label);
            if (XmStringCompare(str, ostr) != True) {
                XtVaSetValues(optp->options[i].widget, XmNlabelString, str, NULL);
            }
            XmStringFree(str);
        }
    }
    
    nw = nchoices - nold;
    if (nw > 0) {
        wlist = xmalloc(nw*sizeof(Widget));
        for (i = nold; i < nchoices; i++) {
            wlist[i - nold] = optp->options[i].widget;
        }
        XtManageChildren(wlist, nw);
        xfree(wlist);
    }
}

OptionStructure *CreateBitmapOptionChoice(Widget parent, char *labelstr, int ncols,
                int nchoices, int width, int height, BitmapOptionItem *items)
{
    int i;
    XmString str;
    OptionStructure *retval;
    Pixel fg, bg;
    Pixmap ptmp;

    retval = xmalloc(sizeof(OptionStructure));
    if (retval == NULL) {
        errmsg("Malloc error in CreateBitmapOptionChoice()");
    }
    retval->nchoices = nchoices;
    retval->options = xmalloc(nchoices*sizeof(OptionWidgetItem));
    if (retval->options == NULL) {
        errmsg("Malloc error in CreateBitmapOptionChoice()");
        XCFREE(retval);
        return retval;
    }


    retval->pulldown = XmCreatePulldownMenu(parent, "pulldownMenu", NULL, 0);
    XtVaSetValues(retval->pulldown, 
                  XmNentryAlignment, XmALIGNMENT_CENTER,
                  NULL);

    if (ncols > 0) {
        XtVaSetValues(retval->pulldown,
                      XmNpacking, XmPACK_COLUMN,
                      XmNnumColumns, ncols,
                      NULL);
    }
    
    XtVaGetValues(retval->pulldown,
                  XmNforeground, &fg,
                  XmNbackground, &bg,
                  NULL);
    
    for (i = 0; i < nchoices; i++) {
	retval->options[i].value = items[i].value;
        if (items[i].bitmap != NULL) {
            ptmp = XCreatePixmapFromBitmapData(disp, root, 
                                    (char *) items[i].bitmap, width, height,
                                    fg, bg, depth);
            retval->options[i].widget = 
                XtVaCreateWidget("pixButton", xmPushButtonWidgetClass,
                                 retval->pulldown,
	                         XmNlabelType, XmPIXMAP,
	                         XmNlabelPixmap, ptmp,
	                         NULL);
        } else {
	    retval->options[i].widget = 
                  XmCreatePushButton(retval->pulldown, "None", NULL, 0);
        }
                                
    }
    for (i = 0; i < nchoices; i++) {
        XtManageChild(retval->options[i].widget);
    }

    retval->menu = XmCreateOptionMenu(parent, "optionMenu", NULL, 0);
    str = XmStringCreateLocalized(labelstr);
    XtVaSetValues(retval->menu,
		  XmNlabelString, str,
		  XmNsubMenuId, retval->pulldown,
		  NULL);
    XmStringFree(str);
    XtManageChild(retval->menu);
    
    return retval;
}


void SetOptionChoice(OptionStructure *opt, int value)
{
    int i;
    Arg a;
    
    if (opt->options == NULL || opt->nchoices <= 0) {
        return;
    }
    
    for (i = 0; i < opt->nchoices; i++) {
        if (opt->options[i].value == value) {
            XtSetArg(a, XmNmenuHistory, opt->options[i].widget);
            XtSetValues(opt->menu, &a, 1);
            return;
        }
    }
}

int GetOptionChoice(OptionStructure *opt)
{
    Arg a;
    Widget warg;
    int i;

    if (opt->options == NULL || opt->nchoices <= 0) {
        errmsg("Internal error in GetOptionChoice()");
        return 0;
    }

    XtSetArg(a, XmNmenuHistory, &warg);
    XtGetValues(opt->menu, &a, 1);

    for (i = 0; i < opt->nchoices; i++) {
        if (opt->options[i].widget == warg) {
            return(opt->options[i].value);
        }
    }
    errmsg("Internal error in GetOptionChoice()");
    return 0;
}

typedef struct {
    OptionStructure *opt;
    void (*cbproc)();
    void *anydata;
} OC_CBdata;

static void oc_int_cb_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int value;
    
    OC_CBdata *cbdata = (OC_CBdata *) client_data;

    value = GetOptionChoice(cbdata->opt);
    cbdata->cbproc(value, cbdata->anydata);
}

void AddOptionChoiceCB(OptionStructure *opt, OC_CBProc cbproc, void *anydata)
{
    OC_CBdata *cbdata;
    int i;
    
    cbdata = xmalloc(sizeof(OC_CBdata));
    
    cbdata->opt = opt;
    cbdata->cbproc = cbproc;
    cbdata->anydata = anydata;
    for (i = 0; i < opt->nchoices; i++) {
        XtAddCallback(opt->options[i].widget, XmNactivateCallback, 
                                    oc_int_cb_proc, (XtPointer) cbdata);
    }
}


static char list_translation_table[] = "\
    Ctrl<Key>A: list_selectall_action()\n\
    Ctrl<Key>U: list_unselectall_action()\n\
    Ctrl<Key>I: list_invertselection_action()";

ListStructure *CreateListChoice(Widget parent, char *labelstr, int type,
                                int nvisible, int nchoices, OptionItem *items)
{
    Arg args[4];
    Widget lab;
    ListStructure *retval;

    retval = xmalloc(sizeof(ListStructure));
    retval->rc = XmCreateRowColumn(parent, "rcList", NULL, 0);
    AddHelpCB(retval->rc, "doc/UsersGuide.html#list-selector");

    lab = XmCreateLabel(retval->rc, labelstr, NULL, 0);
    XtManageChild(lab);
    
    XtSetArg(args[0], XmNlistSizePolicy, XmCONSTANT);
    XtSetArg(args[1], XmNscrollBarDisplayPolicy, XmSTATIC);
    if (type == LIST_TYPE_SINGLE) {
        XtSetArg(args[2], XmNselectionPolicy, XmSINGLE_SELECT);
    } else {
        XtSetArg(args[2], XmNselectionPolicy, XmEXTENDED_SELECT);
    }
    XtSetArg(args[3], XmNvisibleItemCount, nvisible);
    retval->list = XmCreateScrolledList(retval->rc, "listList", args, 4);
    retval->values = NULL;

    XtOverrideTranslations(retval->list, 
                             XtParseTranslationTable(list_translation_table));
    
    UpdateListChoice(retval, nchoices, items);

    XtManageChild(retval->list);
    
    XtManageChild(retval->rc);
    
    return retval;
}

void UpdateListChoice(ListStructure *listp, int nchoices, OptionItem *items)
{
    int i, nsel;
    int *selvalues;
    XmString str;
    
    if (listp == NULL) {
        return;
    }
    
    nsel = GetListChoices(listp, &selvalues);

    listp->nchoices = nchoices;
    listp->values = xrealloc(listp->values, nchoices*SIZEOF_INT);
    for (i = 0; i < nchoices; i++) {
        listp->values[i] = items[i].value;
    }
    
    XmListDeleteAllItems(listp->list);
    for (i = 0; i < nchoices; i++) {
	str = XmStringCreateLocalized(items[i].label);
        XmListAddItemUnselected(listp->list, str, 0);
        XmStringFree(str);
    }
    SelectListChoices(listp, nsel, selvalues);
    if (nsel > 0) {
        xfree(selvalues);
    }
}

int SelectListChoice(ListStructure *listp, int choice)
{
    int top, visible;
    int i = 0;
    
    while (i < listp->nchoices && listp->values[i] != choice) {
        i++;
    }
    if (i < listp->nchoices) {
        i++;
        XmListDeselectAllItems(listp->list);
        XmListSelectPos(listp->list, i, True);
        XtVaGetValues(listp->list, XmNtopItemPosition, &top,
                                 XmNvisibleItemCount, &visible,
                                 NULL);
        if (i < top) {
            XmListSetPos(listp->list, i);
        } else if (i >= top + visible) {
            XmListSetBottomPos(listp->list, i);
        }
        
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

void SelectListChoices(ListStructure *listp, int nchoices, int *choices)
{
    int i = 0, j;
    unsigned char selection_type_save;
    int bottom, visible;
    
    XtVaGetValues(listp->list, XmNselectionPolicy, &selection_type_save, NULL);
    XtVaSetValues(listp->list, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
                             
    XmListDeselectAllItems(listp->list);
    for (j = 0; j < nchoices; j++) {
        i = 0;
        while (i < listp->nchoices && listp->values[i] != choices[j]) {
            i++;
        }
        if (i < listp->nchoices) {
            i++;
            XmListSelectPos(listp->list, i, True);
        }
    }
    
    if (nchoices > 0) {
        /* Rewind list so the last choice is always visible */
        XtVaGetValues(listp->list, XmNtopItemPosition, &bottom,
                                 XmNvisibleItemCount, &visible,
                                 NULL);
        if (i > bottom) {
            XmListSetBottomPos(listp->list, i);
        } else if (i <= bottom - visible) {
            XmListSetPos(listp->list, i);
        }
    }

    XtVaSetValues(listp->list, XmNselectionPolicy, selection_type_save, NULL);
}

int GetListChoices(ListStructure *listp, int **values)
{
    int i, n;
    
    if (XmListGetSelectedPos(listp->list, values, &n) != True) {
        return 0;
    }
    
    for (i = 0; i < n; i++) {
        (*values)[i] = listp->values[(*values)[i] - 1];
    }
    
    return n;
}

int GetSingleListChoice(ListStructure *listp, int *value)
{
    int n, *values, retval;
 
    n = GetListChoices(listp, &values);
    if (n == 1) {
        *value = values[0];
        retval = RETURN_SUCCESS;
    } else {
        retval = RETURN_FAILURE;
    }
    if (n > 0) {
        xfree(values);
    }
    return retval;
}

typedef struct {
    ListStructure *listp;
    void (*cbproc)();
    void *anydata;
} List_CBdata;

static void list_int_cb_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int n, *values;
    List_CBdata *cbdata = (List_CBdata *) client_data;
 
    n = GetListChoices(cbdata->listp, &values);
    
    cbdata->cbproc(n, values, cbdata->anydata);

    if (n > 0) {
        xfree(values);
    }
}

void AddListChoiceCB(ListStructure *listp, List_CBProc cbproc, void *anydata)
{
    List_CBdata *cbdata;
    
    cbdata = xmalloc(sizeof(List_CBdata));
    cbdata->listp = listp;
    cbdata->cbproc = (List_CBProc) cbproc;
    cbdata->anydata = anydata;
    XtAddCallback(listp->list,
        XmNsingleSelectionCallback,   list_int_cb_proc, (XtPointer) cbdata);
    XtAddCallback(listp->list,
        XmNmultipleSelectionCallback, list_int_cb_proc, (XtPointer) cbdata);
    XtAddCallback(listp->list,
        XmNextendedSelectionCallback, list_int_cb_proc, (XtPointer) cbdata);
}


static void spin_arrow_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    SpinStructure *spinp;
    double value, incr;
    
    spinp = (SpinStructure *) client_data;
    value = GetSpinChoice(spinp);
    incr = spinp->incr;
    
    if (w == spinp->arrow_up) {
        incr =  spinp->incr;
    } else if (w == spinp->arrow_down) {
        incr = -spinp->incr;
    } else {
        errmsg("Wrong call to spin_arrow_cb()");
        return;
    }
    value += incr;
    SetSpinChoice(spinp, value);
}

SpinStructure *CreateSpinChoice(Widget parent, char *s, int len,
                        int type, double min, double max, double incr)
{
    SpinStructure *retval;
    Widget fr, form;
    XmString str;
    
    if (min >= max) {
        errmsg("min >= max in CreateSpinChoice()!");
        return NULL;
    }
    
    retval = xmalloc(sizeof(SpinStructure));
    
    retval->type = type;
    retval->min = min;
    retval->max = max;
    retval->incr = incr;
    
    retval->rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, parent,
        XmNorientation, XmHORIZONTAL,
        NULL);
    str = XmStringCreateLocalized(s);
    XtVaCreateManagedWidget("label", xmLabelWidgetClass, retval->rc,
	XmNlabelString, str,
	NULL);
    XmStringFree(str);
    fr = XtVaCreateWidget("fr", xmFrameWidgetClass, retval->rc,
        XmNshadowType, XmSHADOW_ETCHED_OUT,
        NULL);
    form = XtVaCreateWidget("form", xmFormWidgetClass, fr,
        NULL);
    retval->text = XtVaCreateWidget("text", xmTextWidgetClass, form,
	XmNtraversalOn, True,
	XmNcolumns, len,
	NULL);
    retval->arrow_up = XtVaCreateWidget("form", xmArrowButtonGadgetClass, form,
        XmNarrowDirection, XmARROW_UP,
        NULL);
    XtAddCallback(retval->arrow_up, XmNactivateCallback,
        spin_arrow_cb, (XtPointer) retval);
    retval->arrow_down = XtVaCreateWidget("form", xmArrowButtonGadgetClass, form,
        XmNarrowDirection, XmARROW_DOWN,
        NULL);
    XtAddCallback(retval->arrow_down, XmNactivateCallback,
        spin_arrow_cb, (XtPointer) retval);
    XtVaSetValues(retval->text,
        XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_NONE,
        NULL);
    XtVaSetValues(retval->arrow_down,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, retval->text,
        XmNrightAttachment, XmATTACH_NONE,
        NULL);
    XtVaSetValues(retval->arrow_up,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, retval->arrow_down,
        NULL);
    
    XtManageChild(retval->text);
    XtManageChild(retval->arrow_up);
    XtManageChild(retval->arrow_down);
    XtManageChild(form);
    XtManageChild(fr);
    XtManageChild(retval->rc);
    
    return retval;
}

void SetSpinChoice(SpinStructure *spinp, double value)
{
    char buf[64];
    
    if (value < spinp->min) {
        XBell(disp, 50);
        value = spinp->min;
    } else if (value > spinp->max) {
        XBell(disp, 50);
        value = spinp->max;
    }
    
    if (spinp->type == SPIN_TYPE_FLOAT) {
        sprintf(buf, "%g", value);
    } else {
        sprintf(buf, "%d", (int) rint(value));
    }
    XmTextSetString(spinp->text, buf);
}

double GetSpinChoice(SpinStructure *spinp)
{
    double retval;
    
    xv_evalexpr(spinp->text, &retval);
    if (retval < spinp->min) {
        errmsg("Input value below min limit in GetSpinChoice()");
        retval = spinp->min;
        SetSpinChoice(spinp, retval);
    } else if (retval > spinp->max) {
        errmsg("Input value above max limit in GetSpinChoice()");
        retval = spinp->max;
        SetSpinChoice(spinp, retval);
    }
    
    if (spinp->type == SPIN_TYPE_INT) {
        return rint(retval);
    } else {
        return retval;
    }
}


TextStructure *CreateTextInput(Widget parent, char *s)
{
    TextStructure *retval;
    XmString str;
    
    retval = xmalloc(sizeof(TextStructure));
    retval->form = XtVaCreateWidget("form", xmFormWidgetClass, parent, NULL);

    str = XmStringCreateLocalized(s);
    retval->label = XtVaCreateManagedWidget("label", 
        xmLabelWidgetClass, retval->form,
        XmNlabelString, str,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_NONE,
        NULL);
    XmStringFree(str);

    retval->text = XtVaCreateManagedWidget("cstext",
        xmTextWidgetClass, retval->form,
        XmNtraversalOn, True,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, retval->label,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    XtManageChild(retval->form);

    return retval;
}

void cstext_edit_action(Widget w, XEvent *e, String *par, Cardinal *npar)
{
    create_fonttool(w);
}

static char cstext_translation_table[] = "\
    Ctrl<Key>E: cstext_edit_action()";

TextStructure *CreateCSText(Widget parent, char *s)
{
    TextStructure *retval;

    retval = CreateTextInput(parent, s);
    XtOverrideTranslations(retval->text, 
        XtParseTranslationTable(cstext_translation_table));
        
    return retval;
}

char *GetTextString(TextStructure *cst)
{
    static char *buf = NULL;
    char *s;
    
    s = XmTextGetString(cst->text);
    buf = copy_string(buf, s);
    XtFree(s);
    
    return buf;
}

void SetTextString(TextStructure *cst, char *s)
{
    XmTextSetString(cst->text, s ? s : "");
}

typedef struct {
    void (*cbproc)();
    void *anydata;
} Text_CBdata;

static void text_int_cb_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    Text_CBdata *cbdata = (Text_CBdata *) client_data;
    cbdata->cbproc(cbdata->anydata);
}

void AddTextInputCB(TextStructure *cst, Text_CBProc cbproc, void *data)
{
    Text_CBdata *cbdata;
    
    cbdata = xmalloc(sizeof(Text_CBdata));
    cbdata->anydata = data;
    cbdata->cbproc = cbproc;
    
    XtAddCallback(cst->text,
        XmNactivateCallback, text_int_cb_proc, (XtPointer) cbdata);
}

int GetTextCursorPos(TextStructure *cst)
{
    return XmTextGetInsertionPosition(cst->text);
}

void TextInsert(TextStructure *cst, int pos, char *s)
{
    XmTextInsert(cst->text, pos, s);
}


typedef struct {
    void (*cbproc)();
    void *anydata;
} Button_CBdata;

Widget CreateButton(Widget parent, char *label)
{
    Widget button;
    XmString xmstr;
    
    xmstr = XmStringCreateLocalized(label);
    button = XtVaCreateManagedWidget("button",
        xmPushButtonWidgetClass, parent, 
        XmNalignment, XmALIGNMENT_CENTER,
    	XmNlabelString, xmstr,
/*
 *         XmNmarginLeft, 5,
 *         XmNmarginRight, 5,
 *         XmNmarginTop, 3,
 *         XmNmarginBottom, 2,
 */
    	NULL);
    XmStringFree(xmstr);

    return button;
}

Widget CreateBitmapButton(Widget parent,
    int width, int height, const unsigned char *bits)
{
    Widget button;
    Pixmap pm;
    Pixel fg, bg;

    button = XtVaCreateManagedWidget("button",
        xmPushButtonWidgetClass, parent, 
	XmNlabelType, XmPIXMAP,
    	NULL);
    
/*
 * We need to get right fore- and background colors for pixmap.
 */    
    XtVaGetValues(button,
		  XmNforeground, &fg,
		  XmNbackground, &bg,
		  NULL);
    pm = XCreatePixmapFromBitmapData(disp,
        root, (char *) bits, width, height, fg, bg, depth);
    XtVaSetValues(button, XmNlabelPixmap, pm, NULL);

    return button;
}

static void button_int_cb_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    Button_CBdata *cbdata = (Button_CBdata *) client_data;
    cbdata->cbproc(cbdata->anydata);
}

void AddButtonCB(Widget button, Button_CBProc cbproc, void *data)
{
    Button_CBdata *cbdata;
    
    cbdata = xmalloc(sizeof(Button_CBdata));
    cbdata->anydata = data;
    cbdata->cbproc = cbproc;
    XtAddCallback(button,
        XmNactivateCallback, button_int_cb_proc, (XtPointer) cbdata);
}

static void fsb_setcwd_cb(void *data)
{
    char *bufp;
    XmString directory;
    Widget fsb = (Widget) data;
    
    XtVaGetValues(fsb, XmNdirectory, &directory, NULL);
    bufp = GetStringSimple(directory);
    XmStringFree(directory);
    if (bufp != NULL) {
        set_workingdir(bufp);
        XtFree(bufp);
    }
}

#define FSB_CWD     0
#define FSB_HOME    1
#define FSB_ROOT    2
#define FSB_CYGDRV  3

static void fsb_cd_cb(int value, void *data)
{
    char *bufp;
    XmString dir, pattern, dirmask;
    Widget FSB = (Widget) data;
    
    switch (value) {
    case FSB_CWD:
        bufp = get_workingdir();
        break;
    case FSB_HOME:
	bufp = get_userhome();
        break;
    case FSB_ROOT:
        bufp = "/";
        break;
    case FSB_CYGDRV:
        bufp = "/cygdrive/";
        break;
    default:
        return;
    }
    
    XtVaGetValues(FSB, XmNpattern, &pattern, NULL);
    
    dir = XmStringCreateLocalized(bufp);
    dirmask = XmStringConcatAndFree(dir, pattern);

    XmFileSelectionDoSearch(FSB, dirmask);
    XmStringFree(dirmask);
}

static OptionItem fsb_items[] = {
    {FSB_CWD,  "Cwd"},
    {FSB_HOME, "Home"},
    {FSB_ROOT, "/"}
#ifdef __CYGWIN__
    ,{FSB_CYGDRV, "My Computer"}
#endif
};

#define FSB_ITEMS_NUM   sizeof(fsb_items)/sizeof(OptionItem)

#if XmVersion >= 2000    
static void show_hidden_cb(int onoff, void *data)
{
    FSBStructure *fsb = (FSBStructure *) data;
    XtVaSetValues(fsb->FSB, XmNfileFilterStyle,
        onoff ? XmFILTER_NONE:XmFILTER_HIDDEN_FILES, NULL);
}
#endif

FSBStructure *CreateFileSelectionBox(Widget parent, char *s)
{
    FSBStructure *retval;
    OptionStructure *opt;
    Widget fr, form, button;
    XmString xmstr;
    char *bufp, *resname;
    
    retval = xmalloc(sizeof(FSBStructure));
    resname = label_to_resname(s, "FSB");
    retval->FSB = XmCreateFileSelectionDialog(parent, resname, NULL, 0);
    xfree(resname);
    retval->dialog = XtParent(retval->FSB);
    handle_close(retval->dialog);
    bufp = copy_string(NULL, "Grace: ");
    bufp = concat_strings(bufp, s);
    XtVaSetValues(retval->dialog, XmNtitle, bufp, NULL);
    xfree(bufp);
    
    xmstr = XmStringCreateLocalized(get_workingdir());
    XtVaSetValues(retval->FSB, XmNdirectory, xmstr, NULL);
    XmStringFree(xmstr);
    
    XtAddCallback(retval->FSB,
        XmNcancelCallback, destroy_dialog, retval->dialog);
    AddHelpCB(retval->FSB, "doc/UsersGuide.html#FS-dialog");
    
    retval->rc = XmCreateRowColumn(retval->FSB, "rc", NULL, 0);
#if XmVersion >= 2000    
    button = CreateToggleButton(retval->rc, "Show hidden files");
    AddToggleButtonCB(button, show_hidden_cb, retval);
    XtVaSetValues(retval->FSB, XmNfileFilterStyle, XmFILTER_HIDDEN_FILES, NULL);
#endif
    fr = CreateFrame(retval->rc, NULL);
    form = XtVaCreateWidget("form", xmFormWidgetClass, fr, NULL);
    opt = CreateOptionChoice(form, "Chdir to:", 1, FSB_ITEMS_NUM, fsb_items);
    AddOptionChoiceCB(opt, fsb_cd_cb, (void *) retval->FSB);
    button = CreateButton(form, "Set as cwd");
    AddButtonCB(button, fsb_setcwd_cb, (void *) retval->FSB);

    XtVaSetValues(opt->menu,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_NONE,
        NULL);
    XtVaSetValues(button,
        XmNleftAttachment, XmATTACH_NONE,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);
    XtManageChild(form);

    XtManageChild(retval->rc);
        
    return retval;
}

typedef struct {
    FSBStructure *fsb;
    int (*cbproc)();
    void *anydata;
} FSB_CBdata;

static void fsb_int_cb_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *s;
    int ok;
    
    FSB_CBdata *cbdata = (FSB_CBdata *) client_data;
    XmFileSelectionBoxCallbackStruct *cbs =
        (XmFileSelectionBoxCallbackStruct *) call_data;

    s = GetStringSimple(cbs->value);
    if (s == NULL) {
	errmsg("Error converting XmString to char string");
	return;
    }

    set_wait_cursor();

    ok = cbdata->cbproc(s, cbdata->anydata);
    XtFree(s);
    if (ok) {
        XtUnmanageChild(cbdata->fsb->dialog);
    }
    unset_wait_cursor();
}

void AddFileSelectionBoxCB(FSBStructure *fsb, FSB_CBProc cbproc, void *anydata)
{
    FSB_CBdata *cbdata;
    
    cbdata = xmalloc(sizeof(FSB_CBdata));
    cbdata->fsb = fsb;
    cbdata->cbproc = (FSB_CBProc) cbproc;
    cbdata->anydata = anydata;
    XtAddCallback(fsb->FSB,
        XmNokCallback, fsb_int_cb_proc, (XtPointer) cbdata);
}

void SetFileSelectionBoxPattern(FSBStructure *fsb, char *pattern)
{
    XmString xmstr;
    
    if (pattern != NULL) {
        xmstr = XmStringCreateLocalized(pattern);
        XtVaSetValues(fsb->FSB, XmNpattern, xmstr, NULL);
        XmStringFree(xmstr);
    }
}

Widget CreateLabel(Widget parent, char *s)
{
    Widget label;
    
    label = XtVaCreateManagedWidget(s,
        xmLabelWidgetClass, parent,
        XmNalignment, XmALIGNMENT_BEGINNING,
        XmNrecomputeSize, True,
        NULL);
    return label;
}

void AlignLabel(Widget w, int alignment)
{
    unsigned char xm_alignment;
    
    switch(alignment) {
    case ALIGN_BEGINNING:
        xm_alignment = XmALIGNMENT_BEGINNING;
        break;
    case ALIGN_CENTER:
        xm_alignment = XmALIGNMENT_CENTER;
        break;
    case ALIGN_END:
        xm_alignment = XmALIGNMENT_END;
        break;
    default:
        errmsg("Internal error in AlignLabel()");
        return;
        break;
    }
    XtVaSetValues(w,
        XmNalignment, xm_alignment,
        NULL);
}

static OptionItem *font_option_items;
static OptionItem *settype_option_items;
static BitmapOptionItem *pattern_option_items;
static BitmapOptionItem *lines_option_items;

#define LINES_BM_HEIGHT 15
#define LINES_BM_WIDTH  64

int init_option_menus(void) {
    int i, j, k, l, n;
    
    n = number_of_fonts();
    font_option_items = xmalloc(n*sizeof(OptionItem));
    if (font_option_items == NULL) {
        errmsg("Malloc error in init_option_menus()");
        return RETURN_FAILURE;
    }
    for (i = 0; i < n; i++) {
        font_option_items[i].value = i;
        font_option_items[i].label = get_fontalias(i);
    }
    
    n = number_of_patterns();
    pattern_option_items = xmalloc(n*sizeof(BitmapOptionItem));
    if (pattern_option_items == NULL) {
        errmsg("Malloc error in init_option_menus()");
        xfree(font_option_items);
        return RETURN_FAILURE;
    }
    for (i = 0; i < n; i++) {
        pattern_option_items[i].value = i;
        if (i == 0) {
            pattern_option_items[i].bitmap = NULL;
        } else {
            pattern_option_items[i].bitmap = pat_bits[i];
        }
    }
    
    n = number_of_linestyles();
    lines_option_items = xmalloc(n*sizeof(BitmapOptionItem));
    if (lines_option_items == NULL) {
        errmsg("Malloc error in init_option_menus()");
        xfree(pattern_option_items);
        xfree(font_option_items);
        return RETURN_FAILURE;
    }
    for (i = 0; i < n; i++) {
        lines_option_items[i].value = i;
        if (i == 0) {
            lines_option_items[i].bitmap = NULL;
            continue;
        }
        
        lines_option_items[i].bitmap = 
              xcalloc(LINES_BM_HEIGHT*LINES_BM_WIDTH/8/SIZEOF_CHAR, SIZEOF_CHAR);
        
        k = LINES_BM_WIDTH*(LINES_BM_HEIGHT/2);
        while (k < LINES_BM_WIDTH*(LINES_BM_HEIGHT/2 + 1)) {
            for (j = 0; j < dash_array_length[i]; j++) {
                for (l = 0; l < dash_array[i][j]; l++) {
                    if (k < LINES_BM_WIDTH*(LINES_BM_HEIGHT/2 + 1)) {
                        if (j % 2 == 0) { 
                            /* black */
                            lines_option_items[i].bitmap[k/8] |= 1 << k % 8;
                        }
                        k++;
                    }
                }
            }
        }
    }

    settype_option_items = xmalloc(NUMBER_OF_SETTYPES*sizeof(OptionItem));
    if (settype_option_items == NULL) {
        errmsg("Malloc error in init_option_menus()");
        return RETURN_FAILURE;
    }
    for (i = 0; i < NUMBER_OF_SETTYPES; i++) {
        settype_option_items[i].value = i;
        settype_option_items[i].label = copy_string(NULL, set_types(i));
        lowtoupper(settype_option_items[i].label);
    }

    return RETURN_SUCCESS;
}

OptionStructure *CreateFontChoice(Widget parent, char *s)
{
    return (CreateOptionChoice(parent,
        s, 0, number_of_fonts(), font_option_items));
}

OptionStructure *CreatePatternChoice(Widget parent, char *s)
{
    return (CreateBitmapOptionChoice(parent, s, 4, number_of_patterns(), 
                                     16, 16, pattern_option_items));
}

OptionStructure *CreateLineStyleChoice(Widget parent, char *s)
{
    return (CreateBitmapOptionChoice(parent, s, 0, number_of_linestyles(), 
                        LINES_BM_WIDTH, LINES_BM_HEIGHT, lines_option_items));
}

OptionStructure *CreateSetTypeChoice(Widget parent, char *s)
{
    return (CreateOptionChoice(parent,
        s, 0, NUMBER_OF_SETTYPES, settype_option_items));
}

static BitmapOptionItem just_option_items[12] =
{
    {JUST_LEFT  |JUST_BLINE , j_lm_o_bits},
    {JUST_CENTER|JUST_BLINE , j_cm_o_bits},
    {JUST_RIGHT |JUST_BLINE , j_rm_o_bits},
    {JUST_LEFT  |JUST_BOTTOM, j_lb_b_bits},
    {JUST_CENTER|JUST_BOTTOM, j_cb_b_bits},
    {JUST_RIGHT |JUST_BOTTOM, j_rb_b_bits},
    {JUST_LEFT  |JUST_MIDDLE, j_lm_b_bits},
    {JUST_CENTER|JUST_MIDDLE, j_cm_b_bits},
    {JUST_RIGHT |JUST_MIDDLE, j_rm_b_bits},
    {JUST_LEFT  |JUST_TOP   , j_lt_b_bits},
    {JUST_CENTER|JUST_TOP   , j_ct_b_bits},
    {JUST_RIGHT |JUST_TOP   , j_rt_b_bits}
};

OptionStructure *CreateJustChoice(Widget parent, char *s)
{
    return (CreateBitmapOptionChoice(parent, s, 4,
        12, JBITMAP_WIDTH, JBITMAP_HEIGHT, just_option_items));
}

RestrictionStructure *CreateRestrictionChoice(Widget parent, char *s)
{
    RestrictionStructure *retval;
    Widget rc;
    OptionItem restr_items[7];

    restr_items[0].value = RESTRICT_NONE;
    restr_items[0].label = "None";
    restr_items[1].value = RESTRICT_REG0;
    restr_items[1].label = "Region 0";
    restr_items[2].value = RESTRICT_REG1;
    restr_items[2].label = "Region 1";
    restr_items[3].value = RESTRICT_REG2;
    restr_items[3].label = "Region 2";
    restr_items[4].value = RESTRICT_REG3;
    restr_items[4].label = "Region 3";
    restr_items[5].value = RESTRICT_REG4;
    restr_items[5].label = "Region 4";
    restr_items[6].value = RESTRICT_WORLD;
    restr_items[6].label = "Inside graph";

    retval = xmalloc(sizeof(RestrictionStructure));

    retval->frame = CreateFrame(parent, s);
    rc = XtVaCreateWidget("rc",
        xmRowColumnWidgetClass, retval->frame,
        XmNorientation, XmHORIZONTAL,
        NULL);

    retval->r_sel = CreateOptionChoice(rc,
        "Restriction:", 1, 7, restr_items);
    retval->negate = CreateToggleButton(rc, "Negated");
    XtManageChild(rc);

    return retval;
}


static OptionItem *graph_select_items = NULL;
static int ngraph_select_items = 0;
static ListStructure **graph_selectors = NULL;
static int ngraph_selectors = 0;

void graph_select_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
    ListStructure *plist = (ListStructure *) client_data;
    int gno;
    
    gno = plist->values[cbs->item_position - 1];
    switch_current_graph(gno);
}

void update_graph_selectors(void)
{
    int i, new_n = number_of_graphs();
    char buf[64];
    OptionItem *p;
    
    for (i = 0; i < ngraph_select_items; i++) {
        xfree(graph_select_items[i].label);
    }
    p = xrealloc(graph_select_items, new_n*sizeof(OptionItem));
    if (p == NULL && new_n != 0) {
        ngraph_select_items = 0;
        return;
    } else {
        graph_select_items = p;
    }

    for (i = 0; i < new_n; i++) {
        graph_select_items[i].value = i;
        sprintf(buf, "(%c) G%d (%d sets)",
            is_graph_hidden(i) ? '-':'+', i, number_of_sets(i));
        graph_select_items[i].label = copy_string(NULL, buf);
    }
    ngraph_select_items = new_n;
    
    for (i = 0; i < ngraph_selectors; i++) {
        UpdateListChoice(graph_selectors[i],
            ngraph_select_items, graph_select_items);
    }
}

typedef struct {
    Widget popup;
    Widget label_item;
    Widget focus_item;
    Widget hide_item;
    Widget show_item;
    Widget duplicate_item;
    Widget kill_item;
    Widget copy12_item;
    Widget copy21_item;
    Widget move12_item;
    Widget move21_item;
    Widget swap_item;
} GraphPopupMenu;

typedef enum {
    GraphMenuFocusCB,
    GraphMenuHideCB,
    GraphMenuShowCB,
    GraphMenuDuplicateCB,
    GraphMenuKillCB,
    GraphMenuCopy12CB,
    GraphMenuCopy21CB,
    GraphMenuMove12CB,
    GraphMenuMove21CB,
    GraphMenuSwapCB,
    GraphMenuNewCB
} GraphMenuCBtype;

void graph_menu_cb(ListStructure *listp, GraphMenuCBtype type)
{
    int err = FALSE;
    int i, n, *values;
    char buf[32];

    n = GetListChoices(listp, &values);
    
    switch (type) {
    case GraphMenuFocusCB:
        if (n == 1) {
            switch_current_graph(values[0]);
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuHideCB:
        if (n > 0) {
            for (i = 0; i < n; i++) {
                set_graph_hidden(values[i], TRUE);
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuShowCB:
        if (n > 0) {
            for (i = 0; i < n; i++) {
                set_graph_hidden(values[i], FALSE);
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuDuplicateCB:
        if (n > 0) {
            for (i = 0; i < n; i++) {
                duplicate_graph(values[i]);
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuKillCB:
        if (n > 0) {
            if (yesno("Kill selected graph(s)?", NULL, NULL, NULL)) {
                for (i = n - 1; i >= 0; i--) {
                    kill_graph(values[i]);
                }
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuCopy12CB:
        if (n == 2) {
            sprintf(buf, "Overwrite G%d?", values[1]);
            if (yesno(buf, NULL, NULL, NULL)) {
                copy_graph(values[0], values[1]);
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuCopy21CB:
        if (n == 2) {
            sprintf(buf, "Overwrite G%d?", values[0]);
            if (yesno(buf, NULL, NULL, NULL)) {
                copy_graph(values[1], values[0]);
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuMove12CB:
        if (n == 2) {
            sprintf(buf, "Replace G%d?", values[1]);
            if (yesno(buf, NULL, NULL, NULL)) {
                move_graph(values[0], values[1]);
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuMove21CB:
        if (n == 2) {
            sprintf(buf, "Replace G%d?", values[0]);
            if (yesno(buf, NULL, NULL, NULL)) {
                move_graph(values[1], values[0]);
            }
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuSwapCB:
        if (n == 2) {
            swap_graph(values[0], values[1]);
        } else {
            err = TRUE;
        }
        break;
    case GraphMenuNewCB:
        set_graph_active(number_of_graphs());
        break;
    default:
        err = TRUE;
        break;
    }

    if (n > 0) {
        xfree(values);
    }

    if (err == FALSE) {
        update_all();
        xdrawgraph();
    }
}

void switch_focus_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuFocusCB);
}

void hide_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuHideCB);
}

void show_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuShowCB);
}

void duplicate_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuDuplicateCB);
}

void kill_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuKillCB);
}

void copy12_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuCopy12CB);
}

void copy21_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuCopy21CB);
}

void move12_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuMove12CB);
}

void move21_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuMove21CB);
}

void swap_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuSwapCB);
}

void create_new_graph_proc(void *data)
{
    graph_menu_cb((ListStructure *) data, GraphMenuNewCB);
}

GraphPopupMenu *CreateGraphPopupEntries(ListStructure *listp)
{
    GraphPopupMenu *graph_popup_menu;
    Widget popup;
    
    graph_popup_menu = xmalloc(sizeof(GraphPopupMenu));

    popup = XmCreatePopupMenu(listp->list, "graphPopupMenu", NULL, 0);
#if XmVersion >= 2000    
    XtVaSetValues(popup, XmNpopupEnabled, XmPOPUP_DISABLED, NULL);
#else
    XtVaSetValues(popup, XmNpopupEnabled, False, NULL);
#endif
    graph_popup_menu->popup = popup;
    
    graph_popup_menu->label_item = CreateMenuLabel(popup, "Selection:");
    CreateMenuSeparator(popup);
    graph_popup_menu->focus_item = CreateMenuButton(popup, "Focus to", 'F',
    	switch_focus_proc, (void *) listp);
    CreateMenuSeparator(popup);
    graph_popup_menu->hide_item = CreateMenuButton(popup, "Hide", 'H',
    	hide_graph_proc, (void *) listp);
    graph_popup_menu->show_item = CreateMenuButton(popup, "Show", 'S',
    	show_graph_proc, (void *) listp);
    graph_popup_menu->duplicate_item = CreateMenuButton(popup,"Duplicate", 'D',
    	duplicate_graph_proc, (void *) listp);
    graph_popup_menu->kill_item = CreateMenuButton(popup, "Kill", 'K',
    	kill_graph_proc, (void *) listp);
    CreateMenuSeparator(popup);
    graph_popup_menu->copy12_item = CreateMenuButton(popup, "Copy 1 to 2", '\0',
    	copy12_graph_proc, (void *) listp);
    graph_popup_menu->copy21_item = CreateMenuButton(popup, "Copy 2 to 1", '\0',
    	copy21_graph_proc, (void *) listp);
    graph_popup_menu->move12_item = CreateMenuButton(popup, "Move 1 to 2", '\0',
    	move12_graph_proc, (void *) listp);
    graph_popup_menu->move21_item = CreateMenuButton(popup, "Move 2 to 1", '\0',
    	move21_graph_proc, (void *) listp);
    graph_popup_menu->swap_item = CreateMenuButton(popup, "Swap", 'w',
    	swap_graph_proc, (void *) listp);
    CreateMenuSeparator(popup);
    CreateMenuButton(popup, "Create new", 'C',
    	create_new_graph_proc, (void *) listp);

    return graph_popup_menu;
}

void graph_popup(Widget parent, ListStructure *listp, XButtonPressedEvent *event)
{
    int i, n;
    int *values;
    char buf[64];
    Widget popup;
    GraphPopupMenu* graph_popup_menu;
    
    if (event->button != 3) {
        return;
    }
    
    graph_popup_menu = (GraphPopupMenu*) listp->anydata;
    popup = graph_popup_menu->popup;
    
    n = GetListChoices(listp, &values);
    if (n > 0) {
        sprintf(buf, "G%d", values[0]);
        for (i = 1; i < n; i++) {
            if (strlen(buf) > 30) {
                strcat(buf, "...");
                break;
            }
            sprintf(buf, "%s, G%d", buf, values[i]);
        }
    } else {
        strcpy(buf, "None"); 
    }
    
    SetLabel(graph_popup_menu->label_item, buf);
    
    if (n == 0) {
        XtSetSensitive(graph_popup_menu->hide_item, False);
        XtSetSensitive(graph_popup_menu->show_item, False);
        XtSetSensitive(graph_popup_menu->duplicate_item, False);
        XtSetSensitive(graph_popup_menu->kill_item, False);
    } else {
        XtSetSensitive(graph_popup_menu->hide_item, True);
        XtSetSensitive(graph_popup_menu->show_item, True);
        XtSetSensitive(graph_popup_menu->duplicate_item, True);
        XtSetSensitive(graph_popup_menu->kill_item, True);
    }
    if (n == 1) {
        XtSetSensitive(graph_popup_menu->focus_item, True);
    } else {
        XtSetSensitive(graph_popup_menu->focus_item, False);
    }
    if (n == 2) {
        sprintf(buf, "Copy G%d to G%d", values[0], values[1]);
        SetLabel(graph_popup_menu->copy12_item, buf);
        XtManageChild(graph_popup_menu->copy12_item);
        sprintf(buf, "Copy G%d to G%d", values[1], values[0]);
        SetLabel(graph_popup_menu->copy21_item, buf);
        XtManageChild(graph_popup_menu->copy21_item);
        sprintf(buf, "Move G%d to G%d", values[0], values[1]);
        SetLabel(graph_popup_menu->move12_item, buf);
        XtManageChild(graph_popup_menu->move12_item);
        sprintf(buf, "Move G%d to G%d", values[1], values[0]);
        SetLabel(graph_popup_menu->move21_item, buf);
        XtManageChild(graph_popup_menu->move21_item);
        XtSetSensitive(graph_popup_menu->swap_item, True);
    } else {
        XtUnmanageChild(graph_popup_menu->copy12_item);
        XtUnmanageChild(graph_popup_menu->copy21_item);
        XtUnmanageChild(graph_popup_menu->move12_item);
        XtUnmanageChild(graph_popup_menu->move21_item);
        XtSetSensitive(graph_popup_menu->swap_item, False);
    }
    
    if (n > 0) {
        xfree(values);
    }
    XmMenuPosition(popup, event);
    XtManageChild(popup);
}

static void list_selectall(Widget list)
{
    int i, n;
    unsigned char selection_type_save;
    
    XtVaGetValues(list,
                  XmNselectionPolicy, &selection_type_save,
                  XmNitemCount, &n,
                  NULL);
    if (selection_type_save == XmSINGLE_SELECT) {
        XBell(disp, 50);
        return;
    }
    
    XtVaSetValues(list, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
                             
    XmListDeselectAllItems(list);
    for (i = 1; i <= n; i++) {
        XmListSelectPos(list, i, False);
    }
    
    XtVaSetValues(list, XmNselectionPolicy, selection_type_save, NULL);
}

void list_selectall_action(Widget w, XEvent *e, String *par, Cardinal *npar)
{
    list_selectall(w);
}

static void list_selectall_cb(void *data)
{
    ListStructure *listp = (ListStructure *) data;
    list_selectall(listp->list);
}

static void list_unselectall(Widget list)
{
    XmListDeselectAllItems(list);
}

void list_unselectall_action(Widget w, XEvent *e, String *par, Cardinal *npar)
{
    list_unselectall(w);
}

static void list_unselectall_cb(void *data)
{
    ListStructure *listp = (ListStructure *) data;
    list_unselectall(listp->list);
}

static void list_invertselection(Widget list)
{
    int i, n;
    unsigned char selection_type_save;
    
    XtVaGetValues(list,
        XmNselectionPolicy, &selection_type_save,
        XmNitemCount, &n,
        NULL);
    if (selection_type_save == XmSINGLE_SELECT) {
        XBell(disp, 50);
        return;
    }
    
    XtVaSetValues(list, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
    for (i = 0; i < n; i++) {
        XmListSelectPos(list, i, False);
    }
    XtVaSetValues(list, XmNselectionPolicy, selection_type_save, NULL);
}

static void list_invertselection_cb(void *data)
{
    ListStructure *listp = (ListStructure *) data;
    list_invertselection(listp->list);
}

void list_invertselection_action(Widget w, XEvent *e, String *par,
				 Cardinal *npar)
{
    list_invertselection(w);
}

void set_graph_selectors(int gno)
{
    int i;
    
    for (i = 0; i < ngraph_selectors; i++) {
        SelectListChoice(graph_selectors[i], gno);
    }
}

ListStructure *CreateGraphChoice(Widget parent, char *labelstr, int type)
{
    ListStructure *retvalp;
    int nvisible;
        
    ngraph_selectors++;
    graph_selectors = xrealloc(graph_selectors, 
                                    ngraph_selectors*sizeof(ListStructure *));

    nvisible = (type == LIST_TYPE_SINGLE) ? 2 : 4; 
    retvalp = CreateListChoice(parent, labelstr, type, nvisible,
                               ngraph_select_items, graph_select_items);
    if (retvalp == NULL) {
        return NULL;
    }
    AddHelpCB(retvalp->rc, "doc/UsersGuide.html#graph-selector");
    graph_selectors[ngraph_selectors - 1] = retvalp;
    
    XtAddCallback(retvalp->list, XmNdefaultActionCallback,
                               graph_select_cb, retvalp);
    retvalp->anydata = CreateGraphPopupEntries(retvalp);
    
    XtAddEventHandler(retvalp->list, ButtonPressMask, False, 
                            (XtEventHandler) graph_popup, retvalp);

    if (ngraph_select_items == 0) {
        update_graph_selectors();
    } else {
        UpdateListChoice(retvalp, ngraph_select_items, graph_select_items);
    }
    
    SelectListChoice(retvalp, get_cg());
    
    return retvalp;
}

/* Set selectors */
static ListStructure **set_selectors = NULL;
static int nset_selectors = 0;

void UpdateSetChoice(ListStructure *listp, int gno)
{
    int i, j, n = number_of_sets(gno);
    char buf[64];
    OptionItem *set_select_items;
    SetChoiceData *sdata;
    
    sdata = (SetChoiceData *) listp->anydata;
    sdata->gno = gno;
    
    if (n <= 0) {
        UpdateListChoice(listp, 0, NULL);
        return;
    }
    
    set_select_items = xmalloc(n*sizeof(OptionItem));
    if (set_select_items == NULL) {
        return;
    }
    
    for (i = 0, j = 0; i < n; i++) {
        if ((sdata->show_nodata == TRUE || is_set_active(gno, i) == TRUE) &&
            (sdata->show_hidden == TRUE || is_set_hidden(gno, i) != TRUE )) {
            set_select_items[j].value = i;
            sprintf(buf, "(%c) G%d.S%d[%d][%d]",
                is_set_hidden(gno, i) ? '-':'+',
                gno, i, dataset_cols(gno, i), getsetlength(gno, i));
            set_select_items[j].label = copy_string(NULL, buf);
            if (sdata->view_comments == TRUE) {
                set_select_items[j].label =
                    concat_strings(set_select_items[j].label, " \"");
                set_select_items[j].label =
                    concat_strings(set_select_items[j].label,
                    getcomment(gno, i));
                set_select_items[j].label =
                    concat_strings(set_select_items[j].label, "\"");
            }
            j++;
        }
    }
    UpdateListChoice(listp, j, set_select_items);
    
    xfree(set_select_items);
}

void update_set_selectors(int gno)
{
    int i, cg;
    SetChoiceData *sdata;
    
    cg = get_cg();
    update_graph_selectors();
    for (i = 0; i < nset_selectors; i++) {
        sdata = (SetChoiceData *) set_selectors[i]->anydata;
        if (sdata->standalone == TRUE && (gno == cg || gno == ALL_GRAPHS)) {
            UpdateSetChoice(set_selectors[i], cg);
        } else if (sdata->standalone == FALSE && sdata->gno == gno) {
            UpdateSetChoice(set_selectors[i], gno);
        }
    }
}

void set_menu_cb(ListStructure *listp, SetMenuCBtype type)
{
    SetChoiceData *sdata;
    int err = FALSE;
    int gno;
    int i, n, setno, *values;
    char buf[32];

    n = GetListChoices(listp, &values);
    sdata = (SetChoiceData *) listp->anydata;
    gno = sdata->gno;
    
    switch (type) {
    case SetMenuHideCB:
        if (n > 0) {
            for (i = 0; i < n; i++) {
                set_set_hidden(gno, values[i], TRUE);
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuShowCB:
        if (n > 0) {
            for (i = 0; i < n; i++) {
                set_set_hidden(gno, values[i], FALSE);
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuBringfCB:
        if (n == 1) {
            pushset(gno, values[0], PUSH_SET_TOFRONT);
        } else {
            err = TRUE;
        }
        break;
    case SetMenuSendbCB:
        if (n == 1) {
            pushset(gno, values[0], PUSH_SET_TOBACK);
        } else {
            err = TRUE;
        }
        break;
    case SetMenuDuplicateCB:
        if (n > 0) {
            for (i = 0; i < n; i++) {
                setno = nextset(gno);
                do_copyset(gno, values[i], gno, setno);
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuKillCB:
        if (n > 0) {
            if (yesno("Kill selected set(s)?", NULL, NULL, NULL)) {
                for (i = 0; i < n; i++) {
                    killset(gno, values[i]);
                }
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuKillDCB:
        if (n > 0) {
            if (yesno("Kill data in selected set(s)?", NULL, NULL, NULL)) {
                for (i = 0; i < n; i++) {
                    killsetdata(gno, values[i]);
                    setcomment(gno, values[i], "");
                }
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuCopy12CB:
        if (n == 2) {
            sprintf(buf, "Overwrite S%d?", values[1]);
            if (yesno(buf, NULL, NULL, NULL)) {
                do_copyset(gno, values[0], gno, values[1]);
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuCopy21CB:
        if (n == 2) {
            sprintf(buf, "Overwrite S%d?", values[0]);
            if (yesno(buf, NULL, NULL, NULL)) {
                do_copyset(gno, values[1], gno, values[0]);
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuMove12CB:
        if (n == 2) {
            sprintf(buf, "Replace S%d?", values[1]);
            if (yesno(buf, NULL, NULL, NULL)) {
                moveset(gno, values[0], gno, values[1]);
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuMove21CB:
        if (n == 2) {
            sprintf(buf, "Replace S%d?", values[0]);
            if (yesno(buf, NULL, NULL, NULL)) {
                moveset(gno, values[1], gno, values[0]);
            }
        } else {
            err = TRUE;
        }
        break;
    case SetMenuSwapCB:
        if (n == 2) {
            swapset(gno, values[0], gno, values[1]);
        } else {
            err = TRUE;
        }
        break;
    case SetMenuNewFCB:
            create_leval_frame((void *) gno);
        break;
    case SetMenuNewSCB:
            if ((setno = nextset(gno)) != -1) {
                setcomment(gno, setno, "Editor");
                set_set_hidden(gno, setno, FALSE);
                create_ss_frame(gno, setno);
            } else {
                err = TRUE;
            }
        break;
    case SetMenuNewECB:
            if ((setno = nextset(gno)) != -1) {
                setcomment(gno, setno, "Editor");
                set_set_hidden(gno, setno, FALSE);
                do_ext_editor(gno, setno);
            } else {
                err = TRUE;
            }
        break;
    case SetMenuNewBCB:
            create_eblock_frame(gno);
        break;
    case SetMenuEditSCB:
        if (n == 1) {
            create_ss_frame(gno, values[0]);
        } else {
            err = TRUE;
        }
        break;
    case SetMenuEditECB:
        if (n == 1) {
            do_ext_editor(gno, values[0]);
        } else {
            err = TRUE;
        }
        break;
    case SetMenuPackCB:
        packsets(gno);
        break;
    default:
        err = TRUE;
        break;
    }

    if (n > 0) {
        xfree(values);
    }

    if (err == FALSE) {
        update_all();
        xdrawgraph();
    }
}


void hide_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuHideCB);
}

void show_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuShowCB);
}

void bringf_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuBringfCB);
}

void sendb_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuSendbCB);
}

void duplicate_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuDuplicateCB);
}

void kill_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuKillCB);
}

void killd_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuKillDCB);
}

void copy12_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuCopy12CB);
}

void copy21_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuCopy21CB);
}

void move12_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuMove12CB);
}

void move21_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuMove21CB);
}

void swap_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuSwapCB);
}

void newF_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuNewFCB);
}

void newS_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuNewSCB);
}

void newE_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuNewECB);
}

void newB_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuNewBCB);
}

void editS_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuEditSCB);
}

void editE_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuEditECB);
}

void pack_set_proc(void *data)
{
    set_menu_cb((ListStructure *) data, SetMenuPackCB);
}

void shownd_set_proc(int onoff, void *data)
{
    ListStructure *listp = (ListStructure *) data;
    SetChoiceData *sdata = (SetChoiceData *) listp->anydata;
    
    sdata->show_nodata = onoff;
    UpdateSetChoice(listp, sdata->gno);
}

void showh_set_proc(int onoff, void *data)
{
    ListStructure *listp = (ListStructure *) data;
    SetChoiceData *sdata = (SetChoiceData *) listp->anydata;
    
    sdata->show_hidden = onoff;
    UpdateSetChoice(listp, sdata->gno);
}

void view_comments_set_proc(int onoff, void *data)
{
    ListStructure *listp = (ListStructure *) data;
    SetChoiceData *sdata = (SetChoiceData *) listp->anydata;
    
    sdata->view_comments = onoff;
    UpdateSetChoice(listp, sdata->gno);
}

void update_set_proc(void *data)
{
    ListStructure *listp = (ListStructure *) data;
    SetChoiceData *sdata = (SetChoiceData *) listp->anydata;
    
    UpdateSetChoice(listp, sdata->gno);
}

SetPopupMenu *CreateSetPopupEntries(ListStructure *listp)
{
    SetPopupMenu *set_popup_menu;
    Widget popup, submenupane;
    
    set_popup_menu = xmalloc(sizeof(SetPopupMenu));
    popup = XmCreatePopupMenu(listp->list, "setPopupMenu", NULL, 0);
#if XmVersion >= 2000    
    XtVaSetValues(popup, XmNpopupEnabled, XmPOPUP_DISABLED, NULL);
#else
    XtVaSetValues(popup, XmNpopupEnabled, False, NULL);
#endif
    set_popup_menu->popup = popup;
    
    set_popup_menu->label_item = CreateMenuLabel(popup, "Selection:");

    CreateMenuSeparator(popup);

    set_popup_menu->hide_item = CreateMenuButton(popup, "Hide", '\0',
    	hide_set_proc, (void *) listp);
    set_popup_menu->show_item = CreateMenuButton(popup, "Show", '\0',
    	show_set_proc, (void *) listp);
    set_popup_menu->bringf_item = CreateMenuButton(popup, "Bring to front", '\0',
    	bringf_set_proc, (void *) listp);
    set_popup_menu->sendb_item = CreateMenuButton(popup, "Send to back", '\0',
    	sendb_set_proc, (void *) listp);
    CreateMenuSeparator(popup);
    set_popup_menu->duplicate_item = CreateMenuButton(popup, "Duplicate", '\0',
    	duplicate_set_proc, (void *) listp);
    set_popup_menu->kill_item = CreateMenuButton(popup, "Kill", '\0',
    	kill_set_proc, (void *) listp);
    set_popup_menu->killd_item = CreateMenuButton(popup, "Kill data", '\0',
    	killd_set_proc, (void *) listp);
    CreateMenuSeparator(popup);
    set_popup_menu->copy12_item = CreateMenuButton(popup, "Copy 1 to 2", '\0',
    	copy12_set_proc, (void *) listp);
    set_popup_menu->copy21_item = CreateMenuButton(popup, "Copy 2 to 1", '\0',
    	copy21_set_proc, (void *) listp);
    set_popup_menu->move12_item = CreateMenuButton(popup, "Move 1 to 2", '\0',
    	move12_set_proc, (void *) listp);
    set_popup_menu->move21_item = CreateMenuButton(popup, "Move 2 to 1", '\0',
    	move21_set_proc, (void *) listp);
    set_popup_menu->swap_item = CreateMenuButton(popup, "Swap", '\0',
    	swap_set_proc, (void *) listp);
    CreateMenuSeparator(popup);
    set_popup_menu->edit_item = CreateMenu(popup, "Edit", 'E', FALSE);
    CreateMenuButton(set_popup_menu->edit_item, "In spreadsheet", '\0',
    	editS_set_proc, (void *) listp);
    CreateMenuButton(set_popup_menu->edit_item, "In text editor", '\0',
    	editE_set_proc, (void *) listp);
    submenupane = CreateMenu(popup, "Create new", '\0', FALSE);
    CreateMenuButton(submenupane, "By formula", '\0',
    	newF_set_proc, (void *) listp);
    CreateMenuButton(submenupane, "In spreadsheet", '\0',
    	newS_set_proc, (void *) listp);
    CreateMenuButton(submenupane, "In text editor", '\0',
    	newE_set_proc, (void *) listp);
    CreateMenuButton(submenupane, "From block data", '\0',
    	newB_set_proc, (void *) listp);

    CreateMenuSeparator(popup);

    CreateMenuButton(popup, "Pack all sets", '\0',
    	pack_set_proc, (void *) listp);

    CreateMenuSeparator(popup);

    submenupane = CreateMenu(popup, "Selector operations", 'o', FALSE);
    CreateMenuToggle(submenupane,
        "View set comments", '\0', view_comments_set_proc, (void *) listp);
    CreateMenuSeparator(submenupane);
    set_popup_menu->shownd_item = CreateMenuToggle(submenupane,
        "Show data-less", '\0', shownd_set_proc, (void *) listp);
    set_popup_menu->showh_item = CreateMenuToggle(submenupane,
        "Show hidden", '\0', showh_set_proc, (void *) listp);
    CreateMenuSeparator(submenupane);
    CreateMenuButton(submenupane, "Select all", '\0',
    	list_selectall_cb, (void *) listp);
    CreateMenuButton(submenupane, "Unselect all", '\0',
    	list_unselectall_cb, (void *) listp);
    CreateMenuButton(submenupane, "Invert selection", '\0',
    	list_invertselection_cb, (void *) listp);
    CreateMenuSeparator(submenupane);
    CreateMenuButton(submenupane, "Update", '\0',
    	update_set_proc, (void *) listp);

    return set_popup_menu;
}

void set_popup(Widget parent, ListStructure *listp, XButtonPressedEvent *event)
{
    SetChoiceData *sdata;
    int i, n;
    int *values;
    char buf[64];
    Widget popup;
    SetPopupMenu* set_popup_menu;
    
    if (event->button != 3) {
        return;
    }
    
    sdata = (SetChoiceData *) listp->anydata;
    set_popup_menu = sdata->menu;
    popup = set_popup_menu->popup;
    
    n = GetListChoices(listp, &values);
    if (n > 0) {
        sprintf(buf, "S%d", values[0]);
        for (i = 1; i < n; i++) {
            if (strlen(buf) > 30) {
                strcat(buf, "...");
                break;
            }
            sprintf(buf, "%s, S%d", buf, values[i]);
        }
    } else {
        strcpy(buf, "None"); 
    }
    
    SetLabel(set_popup_menu->label_item, buf);
    
    SetToggleButtonState(set_popup_menu->shownd_item, sdata->show_nodata);
    SetToggleButtonState(set_popup_menu->showh_item, sdata->show_hidden);
    
    if (n == 0) {
        XtSetSensitive(set_popup_menu->hide_item, False);
        XtSetSensitive(set_popup_menu->show_item, False);
        XtSetSensitive(set_popup_menu->duplicate_item, False);
        XtSetSensitive(set_popup_menu->kill_item, False);
        XtSetSensitive(set_popup_menu->killd_item, False);
    } else {
        XtSetSensitive(set_popup_menu->hide_item, True);
        XtSetSensitive(set_popup_menu->show_item, True);
        XtSetSensitive(set_popup_menu->duplicate_item, True);
        XtSetSensitive(set_popup_menu->kill_item, True);
        XtSetSensitive(set_popup_menu->killd_item, True);
    }
    if (n == 1) {
        XtSetSensitive(set_popup_menu->bringf_item, True);
        XtSetSensitive(set_popup_menu->sendb_item, True);
        XtSetSensitive(set_popup_menu->edit_item, True);
    } else {
        XtSetSensitive(set_popup_menu->bringf_item, False);
        XtSetSensitive(set_popup_menu->sendb_item, False);
        XtSetSensitive(set_popup_menu->edit_item, False);
    }
    if (n == 2) {
        sprintf(buf, "Copy S%d to S%d", values[0], values[1]);
        SetLabel(set_popup_menu->copy12_item, buf);
        XtManageChild(set_popup_menu->copy12_item);
        sprintf(buf, "Copy S%d to S%d", values[1], values[0]);
        SetLabel(set_popup_menu->copy21_item, buf);
        XtManageChild(set_popup_menu->copy21_item);
        sprintf(buf, "Move S%d to S%d", values[0], values[1]);
        SetLabel(set_popup_menu->move12_item, buf);
        XtManageChild(set_popup_menu->move12_item);
        sprintf(buf, "Move S%d to S%d", values[1], values[0]);
        SetLabel(set_popup_menu->move21_item, buf);
        XtManageChild(set_popup_menu->move21_item);
        XtSetSensitive(set_popup_menu->swap_item, True);
    } else {
        XtUnmanageChild(set_popup_menu->copy12_item);
        XtUnmanageChild(set_popup_menu->copy21_item);
        XtUnmanageChild(set_popup_menu->move12_item);
        XtUnmanageChild(set_popup_menu->move21_item);
        XtSetSensitive(set_popup_menu->swap_item, False);
    }
    
    if (n > 0) {
        xfree(values);
    }
    XmMenuPosition(popup, event);
    XtManageChild(popup);
}

static void ss_edit_cb(Widget list, XtPointer client_data, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
    ListStructure *plist = (ListStructure *) client_data;
    SetChoiceData *sdata = (SetChoiceData *) plist->anydata;
    int gno, setno;
    
    gno = sdata->gno;
    setno = plist->values[cbs->item_position - 1];
    create_ss_frame(gno, setno);
}


ListStructure *CreateSetChoice(Widget parent, char *labelstr, 
                                        int type, int standalone)
{
    ListStructure *retvalp;
    SetChoiceData *sdata;
    int nvisible;

    nvisible = (type == LIST_TYPE_SINGLE) ? 4 : 8; 
    retvalp = CreateListChoice(parent, labelstr, type, nvisible, 0, NULL);
    if (retvalp == NULL) {
        return NULL;
    }
    AddHelpCB(retvalp->rc, "doc/UsersGuide.html#set-selector");

    sdata = xmalloc(sizeof(SetChoiceData));
    if (sdata == NULL) {
        XCFREE(retvalp);
        return NULL;
    }
    
    sdata->standalone = standalone;
    sdata->view_comments = FALSE;
    sdata->show_hidden = TRUE;
    sdata->show_nodata = FALSE;
    sdata->menu = CreateSetPopupEntries(retvalp);
    XtAddEventHandler(retvalp->list, ButtonPressMask, False, 
                            (XtEventHandler) set_popup, retvalp);
    
    XtAddCallback(retvalp->list, XmNdefaultActionCallback, ss_edit_cb, retvalp);
    
    retvalp->anydata = sdata;
    
    if (standalone == TRUE) {
        UpdateSetChoice(retvalp, get_cg());
    }

    nset_selectors++;
    set_selectors = xrealloc(set_selectors, 
                                nset_selectors*sizeof(ListStructure *));
    set_selectors[nset_selectors - 1] = retvalp;
    
    return retvalp;
}

static void update_sets_cb(int n, int *values, void *data)
{
    int gno;
    ListStructure *set_listp = (ListStructure *) data;
    
    if (n == 1) {
        gno = values[0];
    } else {
        gno = -1;
    }
    UpdateSetChoice(set_listp, gno);
}

GraphSetStructure *CreateGraphSetSelector(Widget parent, char *s, int sel_type)
{
    GraphSetStructure *retval;
    Widget rc;

    retval = xmalloc(sizeof(GraphSetStructure));
    retval->frame = CreateFrame(parent, s);
    rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, retval->frame, NULL);
    retval->graph_sel = CreateGraphChoice(rc, "Graph:", LIST_TYPE_SINGLE);
    retval->set_sel = CreateSetChoice(rc, "Set:", sel_type, FALSE);
    AddListChoiceCB(retval->graph_sel,
        update_sets_cb, (void *) retval->set_sel);
    UpdateSetChoice(retval->set_sel, get_cg());
    XtManageChild(rc);

    return retval;
}

SrcDestStructure *CreateSrcDestSelector(Widget parent, int sel_type)
{
    SrcDestStructure *retval;

    retval = xmalloc(sizeof(SrcDestStructure));

    retval->form = XtVaCreateWidget("form",
        xmFormWidgetClass, parent,
        XmNfractionBase, 2,
        NULL);
    retval->src  = CreateGraphSetSelector(retval->form, "Source", sel_type);
    retval->dest = CreateGraphSetSelector(retval->form, "Destination", sel_type);
    XtVaSetValues(retval->src->frame,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_POSITION,
        XmNrightPosition, 1,
        NULL);

    XtVaSetValues(retval->dest->frame,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_POSITION,
        XmNleftPosition, 1,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);

    XtManageChild(retval->form);

    return retval;
}


void paint_color_selector(OptionStructure *optp)
{
    int i, color;
    long bg, fg;
    
    for (i = 0; i < ncolor_option_items; i++) {
        color = color_option_items[i].value;
        bg = xvlibcolors[color];
	if ((get_colorintensity(color) < 0.5 && is_video_reversed() == FALSE) ||
            (get_colorintensity(color) > 0.5 && is_video_reversed() == TRUE )) {
	    fg = xvlibcolors[0];
	} else {
	    fg = xvlibcolors[1];
	}
	XtVaSetValues(optp->options[i].widget, 
            XmNbackground, bg,
            XmNforeground, fg,
            NULL);
    }
}

void update_color_selectors(void)
{
    int i, j;
    CMap_entry *pcmap;
    
    for (i = 0, j = 0; i < number_of_colors(); i++) {
        pcmap = get_cmap_entry(i);
        if (pcmap != NULL && pcmap->ctype == COLOR_MAIN) {
            j++;
        }
    }
    ncolor_option_items = j;

    color_option_items = xrealloc(color_option_items,
                                    ncolor_option_items*sizeof(OptionItem));
    for (i = 0, j = 0; i < number_of_colors(); i++) {
        pcmap = get_cmap_entry(i);
        if (pcmap != NULL && pcmap->ctype == COLOR_MAIN) {
            color_option_items[j].value = i;
            color_option_items[j].label = get_colorname(i);
            j++;
        }
    }
    
    for (i = 0; i < ncolor_selectors; i++) {
        UpdateOptionChoice(color_selectors[i], 
                            ncolor_option_items, color_option_items);
        paint_color_selector(color_selectors[i]);
    }
    
}

OptionStructure *CreateColorChoice(Widget parent, char *s)
{
    OptionStructure *retvalp = NULL;

    ncolor_selectors++;
    color_selectors = xrealloc(color_selectors, 
                                    ncolor_selectors*sizeof(OptionStructure *));
    if (color_selectors == NULL) {
        errmsg("Malloc failed in CreateColorChoice()");
        return retvalp;
    }
    
    retvalp = CreateOptionChoice(parent, s, 4, 
                                ncolor_option_items, color_option_items);

    color_selectors[ncolor_selectors - 1] = retvalp;
    
    paint_color_selector(retvalp);
    
    return retvalp;
}

SpinStructure *CreateLineWidthChoice(Widget parent, char *s)
{
    return CreateSpinChoice(parent, s, 3, SPIN_TYPE_FLOAT, 0.0, MAX_LINEWIDTH, 0.5);
}



Widget *CreatePanelChoice(Widget parent, char *labelstr, int nchoices,...)
{
    va_list var;
    int i = 0;
    XmString str;
    char *s;
    Widget *retval;

    nchoices--;

    retval = (Widget *) XtMalloc((nchoices + 2) * sizeof(Widget));

    retval[1] = XmCreatePulldownMenu(parent, "pulldown", NULL, 0);
    
    va_start(var, nchoices);
    i = 0;
    while ((s = va_arg(var, char *)) != NULL) {
	retval[i + 2] = XmCreatePushButton(retval[1], s, NULL, 0);
	i++;
    }
    if (i != nchoices) {
	errmsg("Incorrect number of selections in CreatePanelChoice()");
    }
    va_end(var);

    XtManageChildren(retval + 2, nchoices);

    retval[0] = XmCreateOptionMenu(parent, "optionmenu", NULL, 0);
    str = XmStringCreateLocalized(labelstr);
    XtVaSetValues(retval[0],
		  XmNlabelString, str,
		  XmNsubMenuId, retval[1],
		  NULL);
    XmStringFree(str);
    XtManageChild(retval[0]);

    return retval;
}


void SetChoice(Widget * w, int value)
{
    Arg a;
    Cardinal nchoices;

    if (w == (Widget *) NULL) {
	errwin("Internal error, SetChoice: Attempt to set NULL Widget");
	return;
    }
    
    XtSetArg(a, XmNnumChildren, &nchoices);
    XtGetValues(w[1], &a, 1);
    
    if (value >= nchoices) {
	errwin("Value not found in SetChoice()");
	return;
    }
    XtSetArg(a, XmNmenuHistory, w[value + 2]);
    XtSetValues(w[0], &a, 1);
}

int GetChoice(Widget * w)
{
    Arg a;
    Widget warg;
    int i;

    if (w == NULL) {
	errwin("Internal error, GetChoice called with NULL argument");
	return 0;
    }
    XtSetArg(a, XmNmenuHistory, &warg);
    XtGetValues(w[0], &a, 1);
    i = 0;
    while (w[i + 2] != warg) {
	if (w[i + 2] == NULL) {
	    errwin("Internal error, GetChoice: Found NULL in Widget list");
	    return 0;
	}
	i++;
    }
    return i;
}

static OptionItem fmt_option_items[32] =
{
    {FORMAT_DECIMAL,        "Decimal"             },
    {FORMAT_EXPONENTIAL,    "Exponential"         },
    {FORMAT_GENERAL,        "General"             },
    {FORMAT_POWER,          "Power"               },
    {FORMAT_SCIENTIFIC,     "Scientific"          },
    {FORMAT_ENGINEERING,    "Engineering"         },
    {FORMAT_COMPUTING,      "Computing (K,M,G,...)"},
    {FORMAT_DDMMYY,         "DD-MM-YY"            },
    {FORMAT_MMDDYY,         "MM-DD-YY"            },
    {FORMAT_YYMMDD,         "YY-MM-DD"            },
    {FORMAT_MMYY,           "MM-YY"               },
    {FORMAT_MMDD,           "MM-DD"               },
    {FORMAT_MONTHDAY,       "Month-DD"            },
    {FORMAT_DAYMONTH,       "DD-Month"            },
    {FORMAT_MONTHS,         "Month (abrev.)"      },
    {FORMAT_MONTHSY,        "Month (abrev.)-YY"   },
    {FORMAT_MONTHL,         "Month"               },
    {FORMAT_DAYOFWEEKS,     "Day of week (abrev.)"},
    {FORMAT_DAYOFWEEKL,     "Day of week"         },
    {FORMAT_DAYOFYEAR,      "Day of year"         },
    {FORMAT_HMS,            "HH:MM:SS"            },
    {FORMAT_MMDDHMS,        "MM-DD HH:MM:SS"      },
    {FORMAT_MMDDYYHMS,      "MM-DD-YY HH:MM:SS"   },
    {FORMAT_YYMMDDHMS,      "YY-MM-DD HH:MM:SS"   },
    {FORMAT_DEGREESLON,     "Degrees (lon)"       },
    {FORMAT_DEGREESMMLON,   "DD MM' (lon)"        },
    {FORMAT_DEGREESMMSSLON, "DD MM' SS.s\" (lon)" },
    {FORMAT_MMSSLON,        "MM' SS.s\" (lon)"    },
    {FORMAT_DEGREESLAT,     "Degrees (lat)"       },
    {FORMAT_DEGREESMMLAT,   "DD MM' (lat)"        },
    {FORMAT_DEGREESMMSSLAT, "DD MM' SS.s\" (lat)" },
    {FORMAT_MMSSLAT,        "MM' SS.s\" (lat)"    }
};

OptionStructure *CreateFormatChoice(Widget parent, char *s)
{
    OptionStructure *retval;
    
    retval = CreateOptionChoice(parent, s, 4, 31, fmt_option_items);
    
    return(retval);
}

static OptionItem as_option_items[4] = 
{
    {AUTOSCALE_NONE, "None"},
    {AUTOSCALE_X,    "X"},
    {AUTOSCALE_Y,    "Y"},
    {AUTOSCALE_XY,   "XY"}
};

OptionStructure *CreateASChoice(Widget parent, char *s)
{
    OptionStructure *retval;
    
    retval = CreateOptionChoice(parent, s, 1, 4, as_option_items);
    /* As init value, use this */
    SetOptionChoice(retval, autoscale_onread);
    
    return(retval);
}

Widget *CreatePrecisionChoice(Widget parent, char *s)
{
    Widget *w;
    
    w = CreatePanelChoice(parent, s,
                          11,
                          "0", "1", "2", "3", "4",
                          "5", "6", "7", "8", "9",
                          NULL);

    return(w);
}
    

Widget CreateScale(Widget parent, char *s, int min, int max, int delta)
{
    Widget w;
    XmString str;

    str = XmStringCreateLocalized(s);
    
    w = XtVaCreateManagedWidget("scroll",
        xmScaleWidgetClass, parent,
	XmNtitleString, str,
	XmNminimum, min,
	XmNmaximum, max,
        XmNscaleMultiple, delta,
	XmNvalue, 0,
	XmNshowValue, True,
	XmNprocessingDirection, XmMAX_ON_RIGHT,
	XmNorientation, XmHORIZONTAL,
#if XmVersion >= 2000    
	XmNsliderMark, XmROUND_MARK,
#endif
	NULL);

    XmStringFree(str);
    
    return w;
}

void SetScaleValue(Widget w, int value)
{
    XtVaSetValues(w, XmNvalue, value, NULL);
}

int GetScaleValue(Widget w)
{
    int value;
    XtVaGetValues(w, XmNvalue, &value, NULL);
    return value;
}

void SetScaleWidth(Widget w, int width)
{
    XtVaSetValues(w, XmNscaleWidth, (Dimension) width, NULL);
}

Widget CreateAngleChoice(Widget parent, char *s)
{
    return CreateScale(parent, s, 0, 360, 10);
}

int GetAngleChoice(Widget w)
{
    return GetScaleValue(w);
}

void SetAngleChoice(Widget w, int angle)
{
    SetScaleValue(w, angle);
}

Widget CreateCharSizeChoice(Widget parent, char *s)
{
    return CreateScale(parent, s, 0, 1000, 25);
}

double GetCharSizeChoice(Widget w)
{
    return ((double) GetScaleValue(w)/100);
}

void SetCharSizeChoice(Widget w, double size)
{
    int value = (int) rint(size*100);
    SetScaleValue(w, value);
}


Widget CreateToggleButton(Widget parent, char *s)
{
    return (XtVaCreateManagedWidget(s, xmToggleButtonWidgetClass, parent, NULL));
}

int GetToggleButtonState(Widget w)
{
    return (XmToggleButtonGetState(w));
}

void SetToggleButtonState(Widget w, int value)
{
    if (w == NULL) {
        return;
    }
    XmToggleButtonSetState(w, value ? True:False, False);
    
    return;
}

typedef struct {
    void (*cbproc)();
    void *anydata;
} TB_CBdata;

static void tb_int_cb_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int onoff;
    
    TB_CBdata *cbdata = (TB_CBdata *) client_data;

    onoff = GetToggleButtonState(w);
    cbdata->cbproc(onoff, cbdata->anydata);
}

void AddToggleButtonCB(Widget w, TB_CBProc cbproc, void *anydata)
{
    TB_CBdata *cbdata;
    
    cbdata = xmalloc(sizeof(TB_CBdata));
    
    cbdata->cbproc = cbproc;
    cbdata->anydata = anydata;
    XtAddCallback(w,
        XmNvalueChangedCallback, tb_int_cb_proc, (XtPointer) cbdata);
}

Widget CreateDialogForm(Widget parent, char *s)
{
    Widget dialog, w;
    char *bufp;
    int standalone;
    
    if (parent == NULL) {
        standalone = TRUE;
        parent = XtAppCreateShell("XMgrace", "XMgrace",
            topLevelShellWidgetClass, disp,
            NULL, 0);
    } else {
        standalone = FALSE;
    }
    bufp = label_to_resname(s, "Dialog");
    dialog = XmCreateDialogShell(parent, bufp, NULL, 0);
    xfree(bufp);
    
    if (standalone) {
        RegisterEditRes(dialog);
    }
    
    handle_close(dialog);

    bufp = copy_string(NULL, "Grace: ");
    bufp = concat_strings(bufp, s);
    XtVaSetValues(dialog,
        XmNtitle, bufp,
        NULL);
    xfree(bufp);

    w = XmCreateForm(dialog, "form", NULL, 0);
    
    return w;
}

void SetDialogFormResizable(Widget form, int onoff)
{
    XtVaSetValues(form,
        XmNresizePolicy, onoff ? XmRESIZE_ANY:XmRESIZE_NONE,
        NULL);
    XtVaSetValues(XtParent(form),
        XmNallowShellResize, onoff ? True:False,
        NULL);
}

void AddDialogFormChild(Widget form, Widget child)
{
    Widget last_widget;
    
    XtVaGetValues(form, XmNuserData, &last_widget, NULL);
    if (last_widget) {
        XtVaSetValues(child,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, last_widget,
            NULL);
        XtVaSetValues(last_widget,
            XmNbottomAttachment, XmATTACH_NONE,
            NULL);
    } else {
        XtVaSetValues(child,
            XmNtopAttachment, XmATTACH_FORM,
            NULL);
    }
    XtVaSetValues(child,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);
    XtVaSetValues(form,
        XmNuserData, child,
        NULL);
}

typedef struct {
    Widget form;
    int close;
    int (*cbproc)();
    void *anydata;
} AACDialog_CBdata;

void aacdialog_int_cb_proc(void *data)
{
    AACDialog_CBdata *cbdata;
    int retval;
    
    set_wait_cursor();

    cbdata = (AACDialog_CBdata *) data;
    
    retval = cbdata->cbproc(cbdata->anydata);

    if (cbdata->close && retval == RETURN_SUCCESS) {
        XtUnmanageChild(XtParent(cbdata->form));
    }
    
    unset_wait_cursor();
}

void CreateAACDialog(Widget form,
    Widget container, AACDialog_CBProc cbproc, void *data)
{
    Widget fr, aacbut[3];
    AACDialog_CBdata *cbdata_accept, *cbdata_apply;
    char *aaclab[3] = {"Apply", "Accept", "Close"};

    fr = CreateFrame(form, NULL);
    XtVaSetValues(fr,
        XmNtopAttachment, XmATTACH_NONE,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        NULL);
    CreateCommandButtons(fr, 3, aacbut, aaclab);

    AddDialogFormChild(form, container);
    XtVaSetValues(container,
        XmNbottomAttachment, XmATTACH_WIDGET,
        XmNbottomWidget, fr,
        NULL);
    
    XtVaSetValues(form, XmNcancelButton, aacbut[2], NULL);
    
    cbdata_accept = xmalloc(sizeof(AACDialog_CBdata));
    cbdata_accept->form    = form;
    cbdata_accept->anydata = data;
    cbdata_accept->cbproc  = cbproc;
    cbdata_accept->close   = TRUE;

    cbdata_apply  = xmalloc(sizeof(AACDialog_CBdata));
    cbdata_apply->form     = form;
    cbdata_apply->anydata  = data;
    cbdata_apply->cbproc   = cbproc;
    cbdata_apply->close    = FALSE;

    AddButtonCB(aacbut[0], aacdialog_int_cb_proc, cbdata_apply);
    AddButtonCB(aacbut[1], aacdialog_int_cb_proc, cbdata_accept);
    AddButtonCB(aacbut[2], destroy_dialog_cb, XtParent(form));
    
    XtManageChild(container);
    XtManageChild(form);
}

TransformStructure *CreateTransformDialogForm(Widget parent,
    char *s, int sel_type)
{
    TransformStructure *retval;
    
    retval = xmalloc(sizeof(TransformStructure));
    
    retval->form = CreateDialogForm(parent, s);
    
    retval->srcdest = CreateSrcDestSelector(retval->form, sel_type);
    AddDialogFormChild(retval->form, retval->srcdest->form);

/*
 *     retval->restr = CreateRestrictionChoice(retval->form, "Source data filtering");
 *     AddDialogFormChild(retval->form, retval->restr->frame);
 */
    
    return retval;
}

int GetTransformDialogSettings(TransformStructure *tdialog, int exclusive,
        int *gsrc, int *gdest,
        int *nssrc, int **svaluessrc, int *nsdest, int **svaluesdest)
{
    int gsrc_ok, gdest_ok;
    
    gsrc_ok = GetSingleListChoice(tdialog->srcdest->src->graph_sel, gsrc);
    gdest_ok = GetSingleListChoice(tdialog->srcdest->dest->graph_sel, gdest);
    if (gsrc_ok == RETURN_FAILURE || gdest_ok == RETURN_FAILURE) {
        errmsg("Please select single source and destination graphs");
	return RETURN_FAILURE;
    }
    
    *nssrc = GetListChoices(tdialog->srcdest->src->set_sel, svaluessrc);
    if (*nssrc == 0) {
        errmsg("No source sets selected");
	return RETURN_FAILURE;
    }    
    *nsdest = GetListChoices(tdialog->srcdest->dest->set_sel, svaluesdest);
    if (*nsdest != 0 && *nssrc != *nsdest) {
        errmsg("Different number of source and destination sets");
        xfree(*svaluessrc);
        xfree(*svaluesdest);
	return RETURN_FAILURE;
    }
    
    /* check for mutually exclusive selections */
    if (exclusive && *gsrc == *gdest && *nsdest != 0) {
        int i;
        for (i = 0; i < *nssrc; i++) {
            if ((*svaluessrc)[i] == (*svaluesdest)[i]) {
                xfree(*svaluessrc);
                xfree(*svaluesdest);
                errmsg("Source and destination set(s) are not mutually exclusive");
	        return RETURN_FAILURE;
            }
        }
    }
    
    return RETURN_SUCCESS;
}

Widget CreateVContainer(Widget parent)
{
    Widget rc;
    
    rc = XmCreateRowColumn(parent, "VContainer", NULL, 0);
    XtManageChild(rc);
    
    return rc;
}

Widget CreateHContainer(Widget parent)
{
    Widget rc;
    
    rc = XmCreateRowColumn(parent, "HContainer", NULL, 0);
    XtVaSetValues(rc, XmNorientation, XmHORIZONTAL, NULL);
    XtManageChild(rc);
    
    return rc;
}


Widget CreateFrame(Widget parent, char *s)
{
    Widget fr;
    
    fr = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, parent, NULL);
    if (s != NULL) {
        XtVaCreateManagedWidget(s, xmLabelGadgetClass, fr,
				XmNchildType, XmFRAME_TITLE_CHILD,
				NULL);
    }
    
    return (fr);   
}


typedef struct {
    int ncols;
    int nrows;
} GridData;

Widget CreateGrid(Widget parent, int ncols, int nrows)
{
    Widget w;
    int nfractions;
    GridData *gd;
    
    if (ncols <= 0 || nrows <= 0) {
        errmsg("Wrong call to CreateGrid()");
        ncols = 1;
        nrows = 1;
    }
    
    nfractions = 0;
    do {
        nfractions++;
    } while (nfractions % ncols || nfractions % nrows);
    
    gd = xmalloc(sizeof(GridData));
    gd->ncols = ncols;
    gd->nrows = nrows;
    
    w = XmCreateForm(parent, "grid_form", NULL, 0);
    XtVaSetValues(w,
        XmNfractionBase, nfractions,
        XmNuserData, gd,
        NULL);

    XtManageChild(w);
    return w;
}

void PlaceGridChild(Widget grid, Widget w, int col, int row)
{
    int nfractions, w1, h1;
    GridData *gd;
    
    XtVaGetValues(grid,
        XmNfractionBase, &nfractions,
        XmNuserData, &gd,
        NULL);
    
    if (gd == NULL) {
        errmsg("PlaceGridChild() called with a non-grid widget");
        return;
    }
    if (col < 0 || col >= gd->ncols) {
        errmsg("PlaceGridChild() called with wrong `col' argument");
        return;
    }
    if (row < 0 || row >= gd->nrows) {
        errmsg("PlaceGridChild() called with wrong `row' argument");
        return;
    }
    
    w1 = nfractions/gd->ncols;
    h1 = nfractions/gd->nrows;
    
    XtVaSetValues(w,
        XmNleftAttachment  , XmATTACH_POSITION,
        XmNleftPosition    , col*w1           ,
        XmNrightAttachment , XmATTACH_POSITION,
        XmNrightPosition   , (col + 1)*w1     ,
        XmNtopAttachment   , XmATTACH_POSITION,
        XmNtopPosition     , row*h1           ,
        XmNbottomAttachment, XmATTACH_POSITION,
        XmNbottomPosition  , (row + 1)*h1     ,
        NULL);
}


Widget CreateTab(Widget parent)
{
    Widget tab;
    
    tab = XtVaCreateManagedWidget("tab", xmTabWidgetClass, parent, NULL);
    
    return (tab);
}

Widget CreateTabPage(Widget parent, char *s)
{
    Widget w;
    XmString str;
    
    w = XmCreateRowColumn(parent, "tabPage", NULL, 0);
    str = XmStringCreateLocalized(s);
    XtVaSetValues(w, XmNtabLabel, str, NULL);
    XmStringFree(str);
    XtManageChild(w);
    
    return (w);
}

void SelectTabPage(Widget tab, Widget w)
{
    XmTabSetTabWidget(tab, w, True);
}

Widget CreateTextItem2(Widget parent, int len, char *s)
{
    Widget w;
    Widget rc;
    XmString str;
    rc = XmCreateRowColumn(parent, "rc", NULL, 0);
    XtVaSetValues(rc, XmNorientation, XmHORIZONTAL, NULL);
    str = XmStringCreateLocalized(s);
    XtVaCreateManagedWidget("label", xmLabelWidgetClass, rc,
			    XmNlabelString, str,
			    NULL);
    XmStringFree(str);
    w = XtVaCreateManagedWidget("text", xmTextWidgetClass, rc,
				XmNtraversalOn, True,
				XmNcolumns, len,
				NULL);
    XtManageChild(rc);
    return w;
}

Widget CreateTextItem4(Widget parent, int len, char *label)
{
    Widget retval;
    XtVaCreateManagedWidget(label, xmLabelWidgetClass, parent, NULL);
    retval = XtVaCreateManagedWidget("text",
        xmTextWidgetClass, parent,
        XmNcolumns, len,
        NULL);
    return retval;
}


/* 
 * create a multiline editable window
 * parent = parent widget
 * hgt    = number of lines in edit window
 * s      = label for window
 * 
 * returns the edit window widget
 */
Widget CreateScrollTextItem2(Widget parent, int hgt, char *s)
{
    Widget w, form, label;
    XmString str;
    Arg args[4];
    int ac;
	
    form = XmCreateForm(parent, "form", NULL, 0);

    str = XmStringCreateLocalized(s);
    label = XtVaCreateManagedWidget("label",
        xmLabelWidgetClass, form,
	XmNlabelString, str,
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL);
    XmStringFree(str);

    ac = 0;
    if (hgt > 0) {
        XtSetArg(args[ac], XmNrows, hgt); ac++;
    }
    XtSetArg(args[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
    XtSetArg(args[ac], XmNwordWrap, True); ac++;
    XtSetArg(args[ac], XmNvisualPolicy, XmVARIABLE); ac++;
    w = XmCreateScrolledText(form, "text", args, ac);
    XtVaSetValues(XtParent(w),
	XmNtopAttachment, XmATTACH_WIDGET,
        XmNtopWidget, label,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);
    XtManageChild(w);
    
    XtManageChild(form);
    return w;
}


char *xv_getstr(Widget w)
/* 
 * return the string from a text widget
 *
 * NB - newlines are converted to spaces
 */
{
    char *s;
    int i;
    static char buf[MAX_STRING_LENGTH];

    strncpy(buf, s = XmTextGetString(w), MAX_STRING_LENGTH - 1);
    XtFree(s);
    
    i=strlen(buf);
    for (i--; i >= 0; i--) {
        if (buf[i] == '\n') {
            buf[i] = ' ';
        }
    }
    return buf;
}


/*
 * xv_evalexpr - take a text field and pass it to the parser if it needs to
 * evaluated, else use atof().
 * place the double result in answer
 * if an error, return False, else True
 */
Boolean xv_evalexpr(Widget w, double *answer )
{
    char *s;
    static char *buf = NULL;
    int i, len, ier = 0;
    double result;
	
    buf = copy_string(buf, s = XmTextGetString(w));
    XtFree(s);

    if (!(len = strlen( buf ) )) { /* check for zero length */
        *answer = 0;
        return RETURN_FAILURE;
    }
    /* first character may be a sign */
    if (!fpdigit[(int) buf[0]] && buf[0] != '-' && buf[0] != '+') {
        i = len +1;
    } else {
        i = 1;
    }

    for (; i<len; i++) {
        if (!fpdigit[(int) buf[i]]) {
            break;
        }
    }

    if (i == len) {         /* only floating point digits */
        *answer = atof( buf );
        return RETURN_SUCCESS;
    } else {                /* must evaluate an expression */
        ier = s_scanner(buf, &result);
        if( !ier ) {
            *answer = result;
            return RETURN_SUCCESS;
        } else {
            *answer = 0;
            return RETURN_FAILURE;
        }
    }
}

/*
 * xv_evalexpri - take a text field and pass it to the parser if it needs to
 * evaluated, else use atoi().
 * place the integer result in answer
 * if an error, return False, else True
 */
Boolean xv_evalexpri(Widget w, int *answer )
{
    char *s;
    static char *buf = NULL;
    int i, len, ier = 0;
    double result;
	
    buf = copy_string(buf, s = XmTextGetString(w));
    XtFree(s);

    if (!(len = strlen( buf ) )) { /* check for zero length */
        *answer = 0;
        return RETURN_FAILURE;
    }
    /* first character may be a sign */
    if (!fpdigit[(int) buf[0]] && buf[0] != '-' && buf[0] != '+') {
        i = len +1;
    } else {
        i = 1;
    }
    
    for (; i<len; i++) {
        if (!fpdigit[(int) buf[i]]) {
            break;
        }
    }

    if (i == len) {             /* only floating point digits */
        *answer = atoi(buf);
        return RETURN_SUCCESS;
    } else {                    /* must evaluate an expression */
        ier = s_scanner(buf, &result);
        if( !ier ) {
            *answer = (int)result;
            return RETURN_SUCCESS;
        } else {
            *answer = 0;
            return RETURN_FAILURE;
        }
    }
}


void xv_setstr(Widget w, char *s)
{
    if (w != NULL) {
        XmTextSetString(w, s ? s : "");
    }
}

/*
 * generic unmanage popup routine, used elswhere
 */
void destroy_dialog(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild((Widget) client_data);
}

/*
 * same for AddButtonCB
 */
void destroy_dialog_cb(void *data)
{
    XtUnmanageChild((Widget) data);
}

/* if user tried to close from WM */
static void wm_exit_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    bailout();
}

/*
 * handle the close item on the WM menu
 */
void handle_close(Widget w)
{
    Atom WM_DELETE_WINDOW;
    
    XtVaSetValues(w, XmNdeleteResponse, XmDO_NOTHING, NULL);
    WM_DELETE_WINDOW = XmInternAtom(disp, "WM_DELETE_WINDOW", False);
    XmAddProtocolCallback(w,
        XM_WM_PROTOCOL_ATOM(w), WM_DELETE_WINDOW,
        (w == app_shell) ? wm_exit_cb : destroy_dialog, w);
    
    savewidget(w);
}

/*
 * Manage and raise
 */
void RaiseWindow(Widget w)
{
    XtManageChild(w);
    XMapRaised(XtDisplay(w), XtWindow(w));
}


static Widget *savewidgets = NULL;
static int nsavedwidgets = 0;

void savewidget(Widget w)
{
    int i;
    
    for (i = 0; i < nsavedwidgets; i++) {
        if (w == savewidgets[i]) {
            return;
        }
    }
    
    savewidgets = xrealloc(savewidgets, (nsavedwidgets + 1)*sizeof(Widget));
    savewidgets[nsavedwidgets] = w;
    nsavedwidgets++;
}

void deletewidget(Widget w)
{
    int i;
    
    for (i = 0; i < nsavedwidgets; i++) {
        if (w == savewidgets[i]) {
            nsavedwidgets--;
            for (; i <  nsavedwidgets; i++) {
                savewidgets[i] = savewidgets[i + 1];
            }
            savewidgets = xrealloc(savewidgets, nsavedwidgets*sizeof(Widget));
            XtDestroyWidget(w);
            return;
        }
    }
    
}

void DefineDialogCursor(Cursor c)
{
    int i;
    
    for (i = 0; i < nsavedwidgets; i++) {
	XDefineCursor(disp, XtWindow(savewidgets[i]), c);
    }
    XFlush(disp);
}

void UndefineDialogCursor()
{
    int i;
    
    for (i = 0; i < nsavedwidgets; i++) {
	XUndefineCursor(disp, XtWindow(savewidgets[i]));
    }
    XFlush(disp);
}

Widget CreateCommandButtonsNoDefault(Widget parent, int n, Widget * buts, char **l)
{
    int i;
    Widget form;
    Dimension h;

    form = XtVaCreateWidget("form", xmFormWidgetClass, parent,
			    XmNfractionBase, n,
			    NULL);

    for (i = 0; i < n; i++) {
	buts[i] = XtVaCreateManagedWidget(l[i],
					  xmPushButtonWidgetClass, form,
					  XmNtopAttachment, XmATTACH_FORM,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNleftAttachment, XmATTACH_POSITION,
					  XmNleftPosition, i,
					  XmNrightAttachment, XmATTACH_POSITION,
					  XmNrightPosition, i + 1,
					  XmNleftOffset, (i == 0) ? 2 : 0,
					  XmNrightOffset, 3,
					  XmNtopOffset, 2,
					  XmNbottomOffset, 3,
					  NULL);
    }
    XtManageChild(form);
    XtVaGetValues(buts[0], XmNheight, &h, NULL);
    XtVaSetValues(form, XmNpaneMaximum, h, XmNpaneMinimum, h, NULL);
    
    return form;
}

Widget CreateCommandButtons(Widget parent, int n, Widget * buts, char **l)
{
    int i;
    Widget form;
    Dimension h;

    form = XtVaCreateWidget("form", xmFormWidgetClass, parent,
			    XmNfractionBase, n,
			    NULL);

    for (i = 0; i < n; i++) {
	buts[i] = XtVaCreateManagedWidget(l[i],
					  xmPushButtonWidgetClass, form,
					  XmNtopAttachment, XmATTACH_FORM,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNleftAttachment, XmATTACH_POSITION,
					  XmNleftPosition, i,
					  XmNrightAttachment, XmATTACH_POSITION,
					  XmNrightPosition, i + 1,
					  XmNdefaultButtonShadowThickness, 1,
					  XmNshowAsDefault, (i == 0) ? True : False,
					  NULL);
    }
    XtManageChild(form);
    XtVaGetValues(buts[0], XmNheight, &h, NULL);
    XtVaSetValues(form, XmNpaneMaximum, h, XmNpaneMinimum, h, NULL);
    
    return form;
}



static SetChoiceItem *plist = NULL;
static int nplist = 0;

SetChoiceItem CreateSetSelector(Widget parent,
				char *label,
				int type,
				int ff,
				int gtype,
				int stype)
{
    Arg args[3];
    Widget rc2, lab;
    SetChoiceItem sel;
    
    rc2 = XtVaCreateWidget("rc", xmRowColumnWidgetClass, parent,
			      XmNorientation, XmVERTICAL, NULL);
    lab = XmCreateLabel(rc2, label, NULL, 0);
    XtManageChild(lab);
    XtSetArg(args[0], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE);
    XtSetArg(args[1], XmNvisibleItemCount, 6);
    sel.list = XmCreateScrolledList(rc2, "list", args, 2);
    if (stype == SELECTION_TYPE_MULTIPLE) {	/* multiple select */
	XtVaSetValues(sel.list,
		      XmNselectionPolicy, XmEXTENDED_SELECT,
		      NULL);
    } else {			/* single select */
    	XtVaSetValues(sel.list,
		      XmNselectionPolicy, XmSINGLE_SELECT,
		      NULL);
    }
    sel.type = type;
    sel.gno = gtype;
    XtManageChild(sel.list);
    sel.indx = save_set_list(sel);
    update_set_list(gtype == GRAPH_SELECT_CURRENT ? get_cg():sel.gno, sel);
    
    XtManageChild(rc2);
    return sel;
}

int GetSelectedSet(SetChoiceItem l)
{
    int retval = SET_SELECT_ERROR;
    int *pos_list;
    int pos_cnt, cnt;
    char buf[256];
	
    if (XmListGetSelectedPos(l.list, &pos_list, &pos_cnt)) {
	XmString *s, cs;
	char *cstr;
	XtVaGetValues(l.list,
		      XmNselectedItemCount, &cnt,
		      XmNselectedItems, &s,
		      NULL);
	cs = XmStringCopy(*s);
	if ((cstr = GetStringSimple(cs))) {
	    strcpy(buf, cstr);
	    if (strcmp(buf, "New set") == 0) {
		retval = SET_SELECT_NEXT;
	    } else if (strcmp(buf, "All sets") == 0) {
		retval = SET_SELECT_ALL;
	    } else if (strcmp(buf, "Nearest set") == 0) {
		retval = SET_SELECT_NEAREST;
	    } else {
		sscanf(buf, "S%d", &retval);
	    }
	    XtFree(cstr);
	}
        XmStringFree(cs);
    }
    
    return retval;
}

/*
 * if the set selection type is multiple, then get a
 * list of sets, returns the number of selected sets.
 */
int GetSelectedSets(SetChoiceItem l, int **sets)
{
    int i;
    int cnt = SET_SELECT_ERROR, retval = SET_SELECT_ERROR;
    int *ptr;
    int *pos_list;
    int pos_cnt, gno;
    if (XmListGetSelectedPos(l.list, &pos_list, &pos_cnt)) {
	char buf[256];
	char *cstr;
	XmString *s, cs;

	XtVaGetValues(l.list,
		      XmNselectedItemCount, &cnt,
		      XmNselectedItems, &s,
		      NULL);
	*sets = xmalloc(cnt * SIZEOF_INT);
	ptr = *sets;
	for (i = 0; i < cnt; i++) {
	    cs = XmStringCopy(s[i]);
	    if ((cstr = GetStringSimple(cs))) {
		strcpy(buf, cstr);
		if (strcmp(buf, "New set") == 0) {
		    retval = SET_SELECT_NEXT;
		    return retval;
		} else if (strcmp(buf, "All sets") == 0) {
		    int j, nsets = 0;
		    retval = SET_SELECT_ALL;
		    if (l.gno == GRAPH_SELECT_CURRENT) {
			gno = get_cg();
		    } else {
			gno = l.gno;
		    }
		    retval = nactive(gno);
		    *sets = xrealloc(*sets, retval * SIZEOF_INT);
		    ptr = *sets;
		    for (j = 0; j < number_of_sets(gno); j++) {
			if (is_set_active(gno, j)) {
			    ptr[nsets] = j;
			    nsets++;
			}
		    }
		    if (nsets != retval) {
			errwin("Nsets != reval, can't happen!");
		    }
		    return retval;
		} else {
		    sscanf(buf, "S%d", &retval);
		}
		ptr[i] = retval;
		XtFree(cstr);
	    }
	    XmStringFree(cs);
	}
    }
    return cnt;
}

int save_set_list(SetChoiceItem l)
{
    nplist++;
    plist = xrealloc(plist, nplist*sizeof(SetChoiceItem));
    plist[nplist - 1] = l;
    return nplist - 1;
}

void update_save_set_list( SetChoiceItem l, int newgr )
{
    plist[l.indx] = l;
    update_set_list( newgr, plist[l.indx] );
}

void update_set_list(int gno, SetChoiceItem l)
{
    int i, cnt, scnt=0;
    char buf[1024];
    XmString *xms;
    
    XmListDeleteAllItems(l.list);
    for (i = 0; i < number_of_sets(gno); i++) {
	if (is_set_active(gno, i)) {
	    scnt++;
	}
    }

    switch (l.type) {		/* TODO */
    case SET_SELECT_ACTIVE:
	xms = xmalloc(sizeof(XmString) * scnt);
	cnt = 0;
	break;
    case SET_SELECT_ALL:
	xms = xmalloc(sizeof(XmString) * (scnt + 1));
	xms[0] = XmStringCreateLocalized("All sets");
	cnt = 1;
	break;
    case SET_SELECT_NEXT:
	xms = xmalloc(sizeof(XmString) * (scnt + 1));
	xms[0] = XmStringCreateLocalized("New set");
	cnt = 1;
	break;
    case SET_SELECT_NEAREST:
	xms = xmalloc(sizeof(XmString) * (scnt + 1));
	xms[0] = XmStringCreateLocalized("Nearest set");
	cnt = 1;
	break;
    default:
	xms = xmalloc(sizeof(XmString) * scnt);
	cnt = 0;
	break;
    }

    for (i = 0; i < number_of_sets(gno); i++) {
        if (is_set_active(gno, i)) {
            sprintf(buf, "S%d (N=%d, %s)", i, getsetlength(gno, i), getcomment(gno, i));
            xms[cnt] = XmStringCreateLocalized(buf);
            cnt++;
        }
    }
    XmListAddItemsUnselected(l.list, xms, cnt, 0);

    /* automatically highlight if only 1 selection */
    if (scnt == 1) {
        XmListSelectItem(l.list, xms[cnt-1], True);
    }
	
    for (i = 0; i < cnt; i++) {
        XmStringFree(xms[i]);
    }
    xfree(xms);
}


void update_set_lists(int gno)
{
    int i;

    if (gno == GRAPH_SELECT_CURRENT) {
        update_set_selectors(get_cg());
        update_ss_editors(get_cg());
    } else {
        update_set_selectors(gno);
        update_ss_editors(gno);
    }

    if (inwin) {
        for (i = 0; i < nplist; i++) {
            if (plist[i].gno == gno || 
                (gno == get_cg() && plist[i].gno == GRAPH_SELECT_CURRENT)) {
                update_set_list(gno, plist[i]);
            }
        }
    }
}


Widget CreateSeparator(Widget parent)
{
    Widget sep;
    
    sep = XmCreateSeparator(parent, "sep", NULL, 0);
    XtManageChild(sep);
    return sep;
}

Widget CreateMenuBar(Widget parent)
{
    Widget menubar;
    
    menubar = XmCreateMenuBar(parent, "menuBar", NULL, 0);
    return menubar;
}

Widget CreateMenu(Widget parent, char *label, char mnemonic, int help)
{
    Widget menupane, cascade;
    XmString str;
    char *name, ms[2];
    
    name = label_to_resname(label, "Menu");
    menupane = XmCreatePulldownMenu(parent, name, NULL, 0);
    xfree(name);

    ms[0] = mnemonic;
    ms[1] = '\0';
    
    str = XmStringCreateLocalized(label);
    cascade = XtVaCreateManagedWidget("cascade",
        xmCascadeButtonGadgetClass, parent, 
    	XmNsubMenuId, menupane, 
    	XmNlabelString, str, 
    	XmNmnemonic, XStringToKeysym(ms),
    	NULL);
    XmStringFree(str);

    if (help) {
        XtVaSetValues(parent, XmNmenuHelpWidget, cascade, NULL);
        CreateMenuButton(menupane, "On context", 'x',
            ContextHelpCB, NULL);
        CreateSeparator(menupane);
    }

    return menupane;
}


Widget CreateMenuButton(Widget parent, char *label, char mnemonic,
	Button_CBProc cb, void *data)
{
    Widget button;
    XmString str;
    char *name, ms[2];
    
    ms[0] = mnemonic;
    ms[1] = '\0';
    
    str = XmStringCreateLocalized(label);
    name = label_to_resname(label, "Button");
    button = XtVaCreateManagedWidget(name,
        xmPushButtonWidgetClass, parent, 
    	XmNlabelString, str,
    	XmNmnemonic, XStringToKeysym(ms),
    	NULL);
    xfree(name);
    XmStringFree(str);

    AddButtonCB(button, cb, data);

    return button;
}

Widget CreateMenuCloseButton(Widget parent, Widget form)
{
    Widget wbut;
    XmString str;
    
    wbut = CreateMenuButton(parent,
        "Close", 'C', destroy_dialog_cb, XtParent(form));
    str = XmStringCreateLocalized("Esc");
    XtVaSetValues(wbut, XmNacceleratorText, str, NULL);
    XmStringFree(str);
    XtVaSetValues(form, XmNcancelButton, wbut, NULL);
    
    return wbut;
}

Widget CreateMenuHelpButton(Widget parent, char *label, char mnemonic,
    Widget form, char *ha)
{
    Widget wbut;
    
    wbut = CreateMenuButton(parent, label, mnemonic, HelpCB, ha);
    AddHelpCB(form, ha);
    
    return wbut;
}

Widget CreateMenuToggle(Widget parent, char *label, char mnemonic,
	TB_CBProc cb, void *data)
{
    Widget button;
    XmString str;
    char *name, ms[2];
    
    ms[0] = mnemonic;
    ms[1] = '\0';
    
    str = XmStringCreateLocalized(label);
    name = label_to_resname(label, NULL);
    button = XtVaCreateManagedWidget(name,
        xmToggleButtonWidgetClass, parent, 
    	XmNlabelString, str,
    	XmNmnemonic, XStringToKeysym(ms),
    	XmNvisibleWhenOff, True,
    	XmNindicatorOn, True,
    	NULL);
    xfree(name);
    XmStringFree(str);

    if (cb) {
        AddToggleButtonCB(button, cb, data);
    }

    return button;
}

Widget CreateMenuLabel(Widget parent, char *name)
{
    Widget lab;
    
    lab = XmCreateLabel(parent, name, NULL, 0);
    XtManageChild(lab);
    return lab;
}

static void help_int_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    HelpCB(client_data);
}

void AddHelpCB(Widget w, char *ha)
{
    if (XtHasCallbacks(w, XmNhelpCallback) == XtCallbackHasSome) {
        /* allow only one help callback */
        XtRemoveAllCallbacks(w, XmNhelpCallback);
    }
    
    XtAddCallback(w, XmNhelpCallback, help_int_cb, (XtPointer) ha);
}

void ContextHelpCB(void *data)
{
    Widget whelp;
    Cursor cursor;
    int ok = FALSE;
    
    cursor = XCreateFontCursor(disp, XC_question_arrow);
    whelp = XmTrackingLocate(app_shell, cursor, False);
    while (whelp != NULL) {
        if (XtHasCallbacks(whelp, XmNhelpCallback) == XtCallbackHasSome) {
            XtCallCallbacks(whelp, XmNhelpCallback, NULL);
            ok = TRUE;
            break;
        } else {
            whelp = GetParent(whelp);
        }
    }
    if (!ok) {
        HelpCB(NULL);
    }
    XFreeCursor(disp, cursor);
}


static int yesno_retval = 0;
static Boolean keep_grab = True;

void yesnoCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;
    int why = cbs->reason;

    keep_grab = False;
    
    XtRemoveGrab(XtParent(w));
    XtUnmanageChild(w);
    switch (why) {
    case XmCR_OK:
	yesno_retval = 1;
	/* process ok action */
	break;
    case XmCR_CANCEL:
	yesno_retval = 0;
	/* process cancel action */
	break;
    }
}

static void update_yesno(Widget w, char *msg, char *ha)
{
    Widget hb;
    XmString str;
	
    if (msg != NULL) {
        str = XmStringCreateLocalized(msg);
    } else {
        str = XmStringCreateLocalized("Warning");
    }
    XtVaSetValues(w, XmNmessageString, str, NULL);
    XmStringFree(str);
    
    hb = XtNameToWidget(w, "Help");
    if (ha) {
        AddButtonCB(hb, HelpCB, ha);
        XtSetSensitive(hb, True);
    } else {    
        XtSetSensitive(hb, False);
    }
}

int yesnowin(char *msg, char *s1, char *s2, char *help_anchor)
{
    static Widget yesno_popup = NULL;
    XEvent event;

    keep_grab = True;

    if (yesno_popup == NULL) {
	yesno_popup = XmCreateErrorDialog(app_shell, "warndlg", NULL, 0);

	XtAddCallback(yesno_popup, XmNokCallback, yesnoCB, NULL);
	XtAddCallback(yesno_popup, XmNcancelCallback, yesnoCB, NULL);
    }
    update_yesno(yesno_popup, msg, help_anchor);
    RaiseWindow(yesno_popup);
    
    XtAddGrab(XtParent(yesno_popup), True, False);
    while (keep_grab || XtAppPending(app_con)) {
	XtAppNextEvent(app_con, &event);
	XtDispatchEvent(&event);
    }
    return yesno_retval;
}


Widget CreateAACButtons(Widget parent, Widget form, Button_CBProc aac_cb)
{
    Widget w;
    Widget aacbut[3];
    static char *aaclab[3] = {"Apply", "Accept", "Close"};
    
    w = CreateCommandButtons(parent, 3, aacbut, aaclab);
    AddButtonCB(aacbut[0], aac_cb, (void *) AAC_APPLY);
    AddButtonCB(aacbut[1], aac_cb, (void *) AAC_ACCEPT);
    AddButtonCB(aacbut[2], aac_cb, (void *) AAC_CLOSE);
    
    if (form != NULL) {
        XtVaSetValues(form, XmNcancelButton, aacbut[2], NULL);
    }
    
    return w;
}

void SetLabel(Widget w, char *s)
{
    XmString str;

    str = XmStringCreateLocalized(s);
    XtVaSetValues(w, XmNlabelString, str, NULL);
    XmStringFree(str);
}

void SetFixedFont(Widget w)
{
    XFontStruct *f;
    XmFontList xmf;

    f = XLoadQueryFont(disp, "fixed");
    xmf = XmFontListCreate(f, charset);
    if (xmf == NULL) {
        errmsg("Can't load font \"fixed\"");
        return;
    } else {
        XtVaSetValues(w, XmNfontList, xmf, NULL);
        XmFontListFree(xmf);
    }
}

char *GetStringSimple(XmString xms)
{
    char *s;

    if (XmStringGetLtoR(xms, charset, &s)) {
        return s;
    } else {
        return NULL;
    }
}

extern int ReqUpdateColorSel;

void update_all(void)
{
    int gno = get_cg();
    
    if (!inwin) {
        return;
    }
    
    update_set_lists(gno);

    update_set_selectors(ALL_GRAPHS);
    update_ss_editors(ALL_GRAPHS);

    if (ReqUpdateColorSel == TRUE) {
        update_color_selectors();
        ReqUpdateColorSel = FALSE;
    }

    update_ticks(gno);
    update_props_items();
    update_hotlinks();
    update_prune_frame();
    update_locator_items(gno);
    set_stack_message();
    set_left_footer(NULL);
}

void update_all_cb(void *data)
{
    update_all();
}
