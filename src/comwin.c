/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-99 Grace Development Team
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
 * Command Panel
 *
 */

#include <config.h>

#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Command.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/List.h>

#include "globals.h"
#include "graphs.h"
#include "utils.h"
#include "files.h"
#include "motifinc.h"
#include "parser.h"
#include "protos.h"

/*
 * Widget item declarations
 */

static Widget command;
static Widget comshell;
static Widget hl;			/* command history list */

static void comcall(Widget w, XtPointer cd, XtPointer calld);
static void clear_history(Widget w, XtPointer client_data, XtPointer call_data);
static void replay_history(Widget w, XtPointer client_data, XtPointer call_data);
static void whist_apply_notify_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void add_com(Widget w, XtPointer client_data, XtPointer call_data);
static void replace_com(Widget w, XtPointer client_data, XtPointer call_data);
static void delete_com(Widget w, XtPointer client_data, XtPointer call_data);
static void move_com(Widget w, XtPointer client_data, XtPointer call_data);
void close_rhist_popup(Widget w, XtPointer client_data, XtPointer call_data);
void comwin_add_act(Widget w, XKeyEvent *e, String *p, Cardinal *c);
void comwin_delete_act(Widget w, XKeyEvent *e, String *p, Cardinal *c);
void comwin_down_act(Widget w, XKeyEvent *e, String *p, Cardinal *c);
void comwin_replace_act(Widget w, XKeyEvent *e, String *p, Cardinal *c);
void comwin_up_act(Widget w, XKeyEvent *e, String *p, Cardinal *c);
void create_rhist_popup(Widget w, XtPointer client_data, XtPointer call_data);
void create_whist_frame(Widget w, XtPointer client_data, XtPointer call_data);
static int do_rhist_proc(char *filename, void *data);
void open_command(void *data);

extern XtAppContext app_con;

void comwin_add_act( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    add_com ((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
}

void comwin_replace_act( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    replace_com ((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
}

void comwin_delete_act( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    delete_com ((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
}

void comwin_up_act( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    move_com ((Widget) NULL, (XtPointer) 0, (XtPointer) NULL);
}

void comwin_down_act( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    move_com ((Widget) NULL, (XtPointer) 1, (XtPointer) NULL);
}

static XtActionsRec actions[] = {
    { "comwin_add", (XtActionProc) comwin_add_act },
    { "comwin_replace", (XtActionProc) comwin_replace_act },
    { "comwin_delete", (XtActionProc) comwin_delete_act },
    { "comwin_up", (XtActionProc) comwin_up_act },
    { "comwin_down", (XtActionProc) comwin_down_act }
};

static char comwin_table[] = "#override\n\
    Shift ~Ctrl ~Meta ~Alt<Key>Return: comwin_add()\n\
    Shift Ctrl ~Meta ~Alt<Key>Return: comwin_replace()\n\
    Shift ~Ctrl ~Meta ~Alt<Key>osfBackSpace: comwin_delete()\n\
    Shift ~Ctrl ~Meta ~Alt<Key>osfUp: comwin_up()\n\
    Shift ~Ctrl ~Meta ~Alt<Key>osfDown: comwin_down()";

static void comcall(Widget w, XtPointer cd, XtPointer calld)
{
    char *ts;
    
    XmCommandCallbackStruct *s = (XmCommandCallbackStruct *) calld;
    ts = GetStringSimple(s->value);
    scanner(ts);
    XtFree(ts);
}

static void delete_com(Widget w, XtPointer client_data, XtPointer call_data)
/* delete a entry from the history list */
{
    int npos, *pos;

    if( XmListGetSelectedPos( hl, &pos, &npos ) == True ) {
        XmListDeletePos( hl, pos[0] );
        xfree( pos );
    }
}
 
static void move_com(Widget w, XtPointer client_data, XtPointer call_data)
/* move a entry up(0) or down(1) in the history list */
{
    int npos, *pos, numit;
    XmString selit, *selitemlst;

    if( XmListGetSelectedPos( hl, &pos, &npos ) == True ) {
    	XtVaGetValues( hl, XmNselectedItems, &selitemlst, XmNitemCount,
    			&numit, NULL );
    	selit = XmStringCopy( selitemlst[0] );
    	XmListDeletePos( hl, pos[0] );
    	if( client_data == 0 ) {
    		pos[0]--;
    	} else {
    	    if( pos[0] < numit ) {
    	    	    pos[0]++;
    	    } else {
    	    	    pos[0] = 1;
    	    }
    	}
    	XmListAddItem( hl, selit, pos[0] );
    	XmListSelectPos( hl, pos[0], False );
    	XmStringFree( selit );
	xfree( pos );
    }
}

static void  add_com(Widget w, XtPointer client_data, XtPointer call_data)
/* 
 * copy the contents of the command line to thestory list without executing it
 */
{
    int npos, *pos, numit=-1, newpos;
    XmString comtxt;

    XtVaGetValues( command, XmNcommand, &comtxt, NULL );
    if (!XmStringEmpty(comtxt)) {
	if( XmListGetSelectedPos( hl, &pos, &npos ) == True ) {
	    XtVaGetValues( hl, XmNitemCount, &numit, NULL );
	    newpos = ++pos[0];
	} else {
	    newpos = 0;
	}
	XmListAddItem( hl, comtxt, newpos );
	XmListSelectPos( hl, newpos, False );
	if( numit>0 ) {
	    xfree( pos );
	}
    }
    XmStringFree( comtxt );
    
    comtxt=XmStringCreateLocalized( "" );
    XmCommandSetValue( command, comtxt );
    XmStringFree( comtxt );
}

static void replace_com(Widget w, XtPointer client_data, XtPointer call_data)
/*
 * replace a entry in the history list with the command line 
 * without executing it
 */
{
    int npos, *pos;
    XmString comtxt;

    XtVaGetValues( command, XmNcommand, &comtxt, NULL );
    if(!XmStringEmpty(comtxt) && XmListGetSelectedPos(hl, &pos, &npos)==True) {
    	XmListDeletePos( hl, pos[0] );
    	XmListAddItem( hl, comtxt, pos[0] );
    	XmListSelectPos( hl, pos[0], False );
    	xfree( pos );
	XmStringFree( comtxt );
	
	comtxt=XmStringCreateLocalized( "" );
	XmCommandSetValue( command,  comtxt);
	XmStringFree( comtxt );
    }
}

static void clear_history(Widget w, XtPointer client_data, XtPointer call_data)
{
    int i;
    int ac = 0, hc;
    Arg al[5];
    Widget h = XmCommandGetChild(command, XmDIALOG_HISTORY_LIST);
    ac = 0;
    XtSetArg(al[ac], XmNhistoryItemCount, &hc);
    ac++;
    XtGetValues(command, al, ac);
    for (i = 0; i < hc; i++) {
	XmListDeletePos(h, 0);
    }
}

#define MAXERR 5
static void replay_history(Widget w, XtPointer client_data, XtPointer call_data)
{
    int errpos;
    static int errcount;
    char *ts;
    int i;
    int ac = 0, hc;
    XmStringTable xmstrs;
    Arg al[5];
    
    ac = 0;
    XtSetArg(al[ac], XmNhistoryItems, &xmstrs);
    ac++;
    XtSetArg(al[ac], XmNhistoryItemCount, &hc);
    ac++;
    XtGetValues(command, al, ac);
    errcount = 0;
    for (i = 0; i < hc; i++) {
        ts = GetStringSimple(xmstrs[i]);
        errpos = scanner(ts);
        XtFree(ts);

        if (errpos) {
            errcount++;
        }
        if (errcount > MAXERR) {
            if (yesno("Lots of errors, cancel?", NULL, NULL, NULL)) {
            	break;
            } else {
            	errcount = 0;
            }
        }
    }
}

void open_command(void *data)
{
    Widget form, fr1, fr2, but[6];
    char *labrow1[5] = { "Add", "Delete", "Replace", "Up", "Down"};
    char *labrow2[6] = { "Read...", "Save...", "Clear", "Replay", "Close",
			 "Help"};
    set_wait_cursor();
    if (command == NULL) {
	XmString str;
        comshell = XmCreateDialogShell(app_shell, "Commands", NULL, 0);
	handle_close(comshell);
	command = XmCreateCommand(comshell, "command", NULL, 0);
	hl = XmCommandGetChild(command, XmDIALOG_HISTORY_LIST);
	str = XmStringCreateLocalized("Command");
        XtVaSetValues(command, XmNpromptString, str, NULL);
        XmStringFree(str);
        
	form = XmCreateForm(command, "commandform", NULL, 0);
	XtVaSetValues(form, 
	    XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, hl,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_WIDGET,
            XmNbottomWidget, XmCommandGetChild(command, XmDIALOG_PROMPT_LABEL),
	    NULL);

	fr1 = CreateFrame(form, NULL);
	XtVaSetValues(fr1, 
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    NULL);
	CreateCommandButtonsNoDefault(fr1, 5, but, labrow1);
	XtAddCallback(but[0], XmNactivateCallback, add_com, NULL);
	XtAddCallback(but[1], XmNactivateCallback, delete_com, NULL);
	XtAddCallback(but[2], XmNactivateCallback, replace_com, NULL);
	XtAddCallback(but[3], XmNactivateCallback, move_com, (XtPointer) 0);
	XtAddCallback(but[4], XmNactivateCallback, move_com, (XtPointer) 1);

	fr2 = CreateFrame(form, NULL);
	XtVaSetValues(fr2, 
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, fr1,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    NULL);
	CreateCommandButtonsNoDefault(fr2, 6, but, labrow2);
	XtAddCallback(but[0], XmNactivateCallback, create_rhist_popup, NULL);
	XtAddCallback(but[1], XmNactivateCallback, create_whist_frame, NULL);
	XtAddCallback(but[2], XmNactivateCallback, clear_history, NULL);
	XtAddCallback(but[3], XmNactivateCallback, replay_history, NULL);
	XtAddCallback(but[4], XmNactivateCallback, destroy_dialog, comshell);
	AddButtonCB(but[5], HelpCB, "doc/UsersGuide.html#commands");

	XtAddCallback(command, XmNcommandEnteredCallback, comcall, NULL);
	ManageChild(form);
	ManageChild(command);
	ManageChild(comshell);
	XtAppAddActions(app_con, actions, XtNumber(actions));
	XtOverrideTranslations(XmCommandGetChild(command, XmDIALOG_COMMAND_TEXT),
	    XtParseTranslationTable(comwin_table));
    }
    RaiseWindow(comshell);
    unset_wait_cursor();
}

static Widget rhist_dialog;

void close_rhist_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
    UnmanageChild(rhist_dialog);
}

static int do_rhist_proc(char *filename, void *data)
{
    char buf[512];
    int sl;
    FILE *fp;
    XmString list_item;
    Widget h = XmCommandGetChild(command, XmDIALOG_HISTORY_LIST);

    if ((fp = grace_openr(filename, SOURCE_DISK)) != NULL) {
	while (grace_fgets(buf, 255, fp) != NULL) {
	    sl = strlen(buf);
	    buf[sl - 1] = 0;
	    if (strlen(buf) == 0) {
                continue;
	    }
	    list_item = XmStringCreateLocalized(buf);
	    XmListAddItemUnselected(h, list_item, 0);
	    XmStringFree(list_item);
	}
	grace_close(fp);
        return TRUE;
    } else {
        return FALSE;
    }
}

void create_rhist_popup(Widget w, XtPointer client_data, XtPointer call_data)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        fsb = CreateFileSelectionBox(app_shell, "Read history");
	AddFileSelectionBoxCB(fsb, do_rhist_proc, NULL);
        ManageChild(fsb->FSB);
    }
    
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

/*
 * Panel item declarations
 */
static Widget whist_frame;
static Widget whist_panel;
static Widget whist_text_item;

/*
 * Create the whist Frame and the whist Panel
 */
void create_whist_frame(Widget w, XtPointer client_data, XtPointer call_data)
{
    set_wait_cursor();
    if (whist_frame == NULL) {
	Widget buts[2];
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Cancel";
	whist_frame = XmCreateDialogShell(app_shell, "Write history", NULL, 0);
	handle_close(whist_frame);
	whist_panel = XmCreateRowColumn(whist_frame, "whist_rc", NULL, 0);

	whist_text_item = CreateTextItem2(whist_panel, 30, "Write history to:");

	CreateSeparator(whist_panel);

	CreateCommandButtons(whist_panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		      (XtCallbackProc) whist_apply_notify_proc, (XtPointer) NULL);
	XtAddCallback(buts[1], XmNactivateCallback,
		      (XtCallbackProc) destroy_dialog, (XtPointer) whist_frame);
	ManageChild(whist_panel);
    }
    RaiseWindow(whist_frame);
    unset_wait_cursor();
}

static void whist_apply_notify_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int i, ac = 0, hc;
    char *ts;
    XmStringTable xmstrs;
    Arg al[5];
    FILE *pp;
    
    pp = grace_openw(xv_getstr(whist_text_item));
    if (pp != NULL) {
        ac = 0;
        XtSetArg(al[ac], XmNhistoryItems, &xmstrs);
        ac++;
        XtSetArg(al[ac], XmNhistoryItemCount, &hc);
        ac++;
        XtGetValues(command, al, ac);
        for (i = 0; i < hc; i++) {
            ts = GetStringSimple(xmstrs[i]);
            fprintf(pp, "%s\n", ts);
            XtFree(ts);
        }
        grace_close(pp);
    }
    UnmanageChild(whist_frame);
}
