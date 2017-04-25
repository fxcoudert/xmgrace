/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
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
 * Font tool
 *
 */

#include <config.h>

#include <X11/X.h>

#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/DialogS.h>
#include <Xm/Text.h>
#include <Xm/XmosP.h>

#include <Xbae/Matrix.h>

#include "t1fonts.h"
#include "utils.h"
#include "motifinc.h"
#include "protos.h"


/* used globally */
extern Widget app_shell;
extern Display *disp;
extern Window root;
extern GC gc;
extern int depth;

extern unsigned long xvlibcolors[];

static Widget fonttool_frame = NULL;
static OptionStructure *font_select_item;
static TextStructure *string_item = NULL;

static Widget cstext_parent = NULL;

static int FontID;
static BBox bbox;
static float Size = 16.8;

static int enable_edit_cb;

static void DrawCB(Widget w,XtPointer cd, XbaeMatrixDrawCellCallbackStruct *cbs);
static void EnterCB(Widget w, XtPointer cd, XbaeMatrixEnterCellCallbackStruct *cbs);
static void update_fonttool_cb(int value, void *data);
static void EditStringCB(Widget w, XtPointer client_data, XmAnyCallbackStruct *cbs);
static void fonttool_aac_cb(void *data);

void create_fonttool_cb(void *data)
{
    create_fonttool((Widget) data);
}

#ifdef NEW_CODE
static void enlarge_glyph(Widget parent,
    XtPointer closure, XEvent *event, Boolean* doit)
{
    XButtonPressedEvent *e = (XButtonPressedEvent *) event;
    if (e->button == 3) {
        int row, col;
        int x0, y0, x1, y1, cwidth, cheight;
        XbaeMatrixRowColToXY(parent, 0, 0, &x0, &y0);
        XbaeMatrixRowColToXY(parent, 1, 1, &x1, &y1);
        cwidth  = x1 - x0;
        cheight = y0 - y1;
        col = (e->x - xleft)/cwidth;
        row = (yupper - e->y)/cheight;
        printf("%d %d\n", col, row);
    }
}
#endif

void create_fonttool(Widget cstext)
{
    int i;
    short widths[16];
    unsigned char column_alignments[16];
    Widget fonttool_panel, font_table, aac_buts;
    
    if (string_item != NULL && cstext == string_item->text) {
        /* avoid recursion */
        return;
    }
    
    if (cstext_parent != NULL) {
        /* unlock previous parent */
        SetSensitive(cstext_parent, True);
    }
    
    cstext_parent = cstext;
    
    if (fonttool_frame == NULL) {
	fonttool_frame = XmCreateDialogShell(app_shell, "Font tool", NULL, 0);
	handle_close(fonttool_frame);
        fonttool_panel = XtCreateWidget("fonttool_panel", xmFormWidgetClass, 
                                        fonttool_frame, NULL, 0);

        font_select_item = CreateFontChoice(fonttool_panel, "Font:");
        XtVaSetValues(font_select_item->menu,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_FORM,
            NULL);
        
        for (i = 0; i < 16; i++) {
            widths[i] = 2;
            column_alignments[i] = XmALIGNMENT_BEGINNING;
        }
        font_table = XtVaCreateManagedWidget(
            "fontTable", xbaeMatrixWidgetClass, fonttool_panel,
            XmNrows, 16,
            XmNcolumns, 16,
            XmNvisibleRows, 8,
            XmNvisibleColumns, 16,
            XmNfill, True,
            XmNcolumnWidths, widths,
            XmNcolumnAlignments, column_alignments,
	    XmNgridType, XmGRID_CELL_SHADOW,
	    XmNcellShadowType, XmSHADOW_ETCHED_OUT,
	    XmNcellShadowThickness, 2,
            XmNaltRowCount, 0,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, font_select_item->menu,
            NULL);
            
        XtAddCallback(font_table, XmNdrawCellCallback, (XtCallbackProc) DrawCB, NULL);
        XtAddCallback(font_table, XmNenterCellCallback, (XtCallbackProc) EnterCB, NULL);
#ifdef NEW_CODE
        XtAddEventHandler(font_table, ButtonPressMask, False, 
                            enlarge_glyph, NULL);
#endif
        AddOptionChoiceCB(font_select_item, update_fonttool_cb, font_table);

        string_item = CreateCSText(fonttool_panel, "CString:");
        XtVaSetValues(string_item->form,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, font_table,
            NULL);

        XtAddCallback(string_item->text,
            XmNmodifyVerifyCallback, (XtCallbackProc) EditStringCB, font_table);
        
#ifdef NEW_CODE
        scrolled_window = XtVaCreateManagedWidget("scrolled_window",
	    xmScrolledWindowWidgetClass, fonttool_panel,
	    XmNscrollingPolicy, XmAUTOMATIC,
	    XmNvisualPolicy, XmVARIABLE,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, GetParent(string_item),
            XmNbottomAttachment, XmATTACH_FORM,
	    NULL);

        glyph_item = XtVaCreateManagedWidget("glyph",
            xmDrawingAreaWidgetClass, scrolled_window,
	    XmNheight, (Dimension) 100,
	    XmNwidth, (Dimension) 600,
	    XmNresizePolicy, XmRESIZE_ANY,
            XmNbackground,
	    xvlibcolors[0],
	    NULL);
#endif

        aac_buts = CreateAACButtons(fonttool_panel,
            fonttool_panel, fonttool_aac_cb);
        XtVaSetValues(aac_buts,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, string_item->form,
            XmNbottomAttachment, XmATTACH_FORM,
            NULL);
        
        update_fonttool_cb(0, font_table);
        ManageChild(fonttool_panel);
    }

    enable_edit_cb = FALSE;
    if (cstext_parent == NULL) {
        SetTextString(string_item, "");
    } else {
        SetTextString(string_item, xv_getstr(cstext_parent));
        /* Lock editable text */
        SetSensitive(cstext_parent, False);
    }
    enable_edit_cb = TRUE;
    
    RaiseWindow(fonttool_frame);
}

static T1_TMATRIX UNITY_MATRIX = {1.0, 0.0, 0.0, 1.0};

static void DrawCB(Widget w, XtPointer cd, XbaeMatrixDrawCellCallbackStruct *cbs)
{
    unsigned char c;
    GLYPH *glyph;
    int height, width, hshift, vshift;
    Pixmap pixmap, ptmp;
    char dummy_bits[1] = {0};
    int valid_char;
    long bg, fg;
    
        
    c = 16*cbs->row + cbs->column;
        
    if (FontID == BAD_FONT_ID) {
        glyph = NULL;
    } else {
        glyph = T1_SetChar(FontID, c, Size, &UNITY_MATRIX);
    }
       
    if (glyph != NULL && glyph->bits != NULL) {
        valid_char = TRUE;
        height = glyph->metrics.ascent - glyph->metrics.descent;
        width = glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
        hshift = MAX2(glyph->metrics.leftSideBearing - bbox.llx, 0);
        vshift = MAX2(bbox.ury - glyph->metrics.ascent, 0);
        XtVaGetValues(w, XmNbackground, &bg, XmNforeground, &fg, NULL);
        XSetForeground(disp, gc, bg);
        ptmp = XCreateBitmapFromData(disp, root,
                    (char *) glyph->bits, width, height);
        XSetBackground(disp, gc, bg);
        pixmap = XCreatePixmap(disp, root, bbox.urx - bbox.llx, bbox.ury - bbox.lly, depth);
        XFillRectangle(disp, pixmap, gc, 0, 0, bbox.urx - bbox.llx, bbox.ury - bbox.lly);
        XSetForeground(disp, gc, fg);
        XCopyPlane(disp, ptmp, pixmap, gc, 0, 0, width, height, hshift, vshift, 1);
        XFreePixmap(disp, ptmp);
    } else {
        if (c == ' ') {
            valid_char = TRUE;
        } else {
            valid_char = FALSE;
        }
        pixmap = XCreateBitmapFromData(disp, root,
             dummy_bits, 1, 1);
    }
    
    /* Assign it a pixmap */
    cbs->pixmap = pixmap;
    cbs->type = XbaePixmap;
    XbaeMatrixSetCellUserData(w, cbs->row, cbs->column, (XtPointer) valid_char);  
   
    return;
}

static void insert_into_string(char *s)
{
    int pos;
    
    pos = GetTextCursorPos(string_item);
    TextInsert(string_item, pos, s);
}

static void EnterCB(Widget w, XtPointer cd, XbaeMatrixEnterCellCallbackStruct *cbs)
{
    int valid_char;
    char s[7];
    unsigned char c;
    
    valid_char = (int) XbaeMatrixGetCellUserData(w, cbs->row, cbs->column);
    if (valid_char == TRUE) {
        c = 16*cbs->row + cbs->column;
        /* TODO: check for c being displayable in the _X_ font */
        if (c > 31) {
            s[0] = (char) c;
            s[1] = '\0';
        } else {
            sprintf(s, "\\#{%02x}", c);
        }
        insert_into_string(s);
    } else {
        XBell(disp, 25);
    }
}


static void update_fonttool_cb(int value, void *data)
{
    char *buf;
    int x0, y0, x1, y1, cwidth, cheight;
    int csize, bsize;
    Widget font_table = (Widget) data;
    
    FontID = value;
    switch (T1_CheckForFontID(FontID)) {
    case 0:
        T1_LoadFont(FontID);
        break;
    case -1:
        errmsg("Couldn't load font");
        FontID = BAD_FONT_ID;
        return;
        break;
    default:
        break;
    }

    bbox = T1_GetFontBBox(FontID);
    /* check if bbox is zero or invalid and then calculate it ourselves */
    if (bbox.llx >= bbox.urx || bbox.lly >= bbox.ury) {
        int c;
        memset(&bbox, 0, sizeof(bbox));
        for (c = 0; c < 256; c++) {
            BBox bbox_tmp = T1_GetCharBBox(FontID, c);
            bbox.llx = MIN2(bbox.llx, bbox_tmp.llx);
            bbox.lly = MIN2(bbox.lly, bbox_tmp.lly);
            bbox.urx = MAX2(bbox.urx, bbox_tmp.urx);
            bbox.ury = MAX2(bbox.ury, bbox_tmp.ury);
        }
    }

    XbaeMatrixRowColToXY(font_table, 0, 0, &x0, &y0);
    XbaeMatrixRowColToXY(font_table, 1, 1, &x1, &y1);
    cwidth  = x1 - x0;
    cheight = y1 - y0;
    
    /* 6 = 2*cellShadowThickness + 2 */
    csize = MIN2(cwidth, cheight) - 6;
    bsize = MAX2(bbox.urx - bbox.llx, bbox.ury - bbox.lly);
    Size  = floor(1000.0*csize/bsize);

    bbox.llx = bbox.llx*Size/1000;
    bbox.lly = bbox.lly*Size/1000;
    bbox.urx = bbox.urx*Size/1000;
    bbox.ury = bbox.ury*Size/1000;
            
    XbaeMatrixRefresh(font_table);
    buf = copy_string(NULL, "\\f{");
    buf = concat_strings(buf, get_fontalias(FontID));
    buf = concat_strings(buf, "}");
    insert_into_string(buf);
    xfree(buf);
}


static void EditStringCB(Widget w, XtPointer client_data, XmAnyCallbackStruct *cbs)
{
    unsigned char c;
    int valid_char;
    static int column = 0, row = 0;
    XmTextVerifyCallbackStruct *tcbs;
    XmTextBlock text;
    Widget ftable = (Widget) client_data;
    
    if (enable_edit_cb != TRUE) {
        return;
    }
    
    XbaeMatrixDeselectCell(ftable, row, column);
    
    tcbs = (XmTextVerifyCallbackStruct *) cbs;
    
    text = tcbs->text;
    
    if (text->length == 1) {
        /* */
        c = text->ptr[0];
        row = c/16;
        column = c % 16;

        valid_char = (int) XbaeMatrixGetCellUserData(ftable, row, column);
        if (valid_char == TRUE) {
            XbaeMatrixSelectCell(ftable, row, column);
        } else {
            tcbs->doit = False;
        }
    }
}

static void fonttool_aac_cb(void *data)
{
    int aac_mode;
    
    aac_mode = (int) data;
    
    if (aac_mode == AAC_CLOSE) {
        UnmanageChild(fonttool_frame);
        if (cstext_parent != NULL) {
            SetSensitive(cstext_parent, True);
        }
        return;
    }

    if (cstext_parent != NULL) {
        xv_setstr(cstext_parent, GetTextString(string_item));
    }
    
    if (aac_mode == AAC_ACCEPT) {
        UnmanageChild(fonttool_frame);
        if (cstext_parent != NULL) {
            SetSensitive(cstext_parent, True);
        }
    }
}
