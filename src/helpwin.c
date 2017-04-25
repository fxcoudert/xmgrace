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

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "files.h"
#include "protos.h"

#include "motifinc.h"

#define NO_HELP "doc/nohelp.html"

#ifdef WITH_LIBHELP
#  include <help.h>
#endif

#ifdef WITH_XMHTML
#  include <XmHTML/XmHTML.h>
void create_helper_frame(char *fname);
#endif

int force_external_viewer =
#if defined WITH_XMHTML || defined WITH_LIBHELP
    FALSE;
#else
    TRUE;
#endif

void HelpCB(void *data)
{
    char *URL, *ha;
    int remote;

    ha = (char *) data;
    
    if (ha == NULL) {
        ha = NO_HELP;
    }
    
    if (strstr(ha, "http:") || strstr(ha, "ftp:") || strstr(ha, "mailto:")) {
        URL = copy_string(NULL, ha);
        remote = TRUE;
    } else {
        char *p, *pa;
        
        if (ha == strstr(ha, "file:")) {
            p = (ha + 5);
        } else {
            p = ha;
        }

        pa = strchr(p, '#');
        if (pa) {
            char *base = copy_string(NULL, p);
            base[pa - p] = '\0';
            if (force_external_viewer) {
                URL = copy_string(NULL, "file://");
            } else {
                URL = NULL;
            }
            URL = concat_strings(URL, grace_path(base));
            URL = concat_strings(URL, pa);
            xfree(base);
        } else {
            URL = copy_string(NULL, grace_path(p));
        }

        remote = FALSE;
    }
    
    if (remote || force_external_viewer) {
        char *help_viewer, *command;
        int i, j, len, urllen, comlen;
        
        help_viewer = get_help_viewer();
        len = strlen(help_viewer);
        urllen = strlen(URL);
        for (i = 0, comlen = len; i < len - 1; i++) {
    	    if ((help_viewer[i] == '%') && (help_viewer[i + 1] == 's')){
    	        comlen += urllen - 2;
    	        i++;
    	    }
        }
        command = xmalloc((comlen + 1)*SIZEOF_CHAR);
        command[comlen] = '\0';
        for (i = 0, j = 0; i < len; i++) {
    	    if ((help_viewer[i] == '%') && (help_viewer[i + 1] == 's')){
    	        strcpy (&command[j], URL);
    	        j += urllen;
    	        i++;
    	    } else {
    	        command[j++] = help_viewer[i];
    	    }
        }
#ifdef VMS    
        system_spawn(command);
#else
        command = concat_strings(command, "&");    
        system_wrap(command);
#endif
        xfree(command);
    } else {
#ifdef WITH_XMHTML
        create_helper_frame(URL);
#endif
#ifdef WITH_LIBHELP
        get_help(app_shell, (XtPointer) URL, NULL);
#endif
    }
    
    xfree(URL);
}

/*
 * say a few things about Grace
 */
static Widget about_frame;

void create_about_grtool(void *data)
{
    set_wait_cursor();
    
    if (about_frame == NULL) {
        Widget wbut, fr, rc, about_panel;
        char buf[1024];
        
	about_frame = CreateDialogForm(app_shell, "About");
	
        about_panel = CreateVContainer(about_frame);
        AddDialogFormChild(about_frame, about_panel);

	fr = CreateFrame(about_panel, NULL);
        rc = CreateVContainer(fr);
	CreateLabel(rc, bi_version_string());
#ifdef DEBUG
	CreateLabel(rc, "Debugging is enabled");
#endif

	fr = CreateFrame(about_panel, "Legal stuff");
        rc = CreateVContainer(fr);
	CreateLabel(rc, "Copyright (c) 1991-1995 Paul J Turner");
	CreateLabel(rc, "Copyright (c) 1996-2015 Grace Development Team");
	CreateLabel(rc, "Maintained by Evgeny Stambulchik");
	CreateLabel(rc, "All rights reserved");
	CreateLabel(rc,
            "The program is distributed under the terms of the GNU General Public License");

	fr = CreateFrame(about_panel, "Third party copyrights");
        rc = CreateVContainer(fr);
	CreateLabel(rc,
            "Tab widget, Copyright (c) 1997 Pralay Dakua");
	CreateLabel(rc, "Xbae widget,");
	CreateLabel(rc,
            "      Copyright (c) 1991, 1992 Bell Communications Research, Inc. (Bellcore)");
	CreateLabel(rc,
            "      Copyright (c) 1995-1999 Andrew Lister");
	CreateLabel(rc, "Raster driver based on the GD-1.3 library,");
	CreateLabel(rc,
            "      Portions copyright (c) 1994-1998 Cold Spring Harbor Laboratory");
	CreateLabel(rc,
            "      Portions copyright (c) 1996-1998 Boutell.Com, Inc");
#ifdef HAVE_LIBPDF
	CreateLabel(rc, "PDFlib library, Copyright (c) 1997-2012 PDFlib GmbH");
#endif

	fr = CreateFrame(about_panel, "Build info");
        rc = CreateVContainer(fr);
        sprintf(buf, "Host: %s", bi_system());
	CreateLabel(rc, buf);
	sprintf(buf, "Time: %s", bi_date());
	CreateLabel(rc, buf);
	sprintf(buf, "GUI toolkit: %s ", bi_gui());
	CreateLabel(rc, buf);
	sprintf(buf, "Xbae version: %s ", bi_gui_xbae());
	CreateLabel(rc, buf);
	sprintf(buf, "T1lib: %s ", bi_t1lib());
	CreateLabel(rc, buf);
#ifdef HAVE_LIBPNG
	sprintf(buf, "libpng: %s ", bi_pnglib());
	CreateLabel(rc, buf);
#endif
#ifdef HAVE_LIBJPEG
	sprintf(buf, "libjpeg: %s ", bi_libjpeg());
	CreateLabel(rc, buf);
#endif
#ifdef HAVE_LIBPDF
	sprintf(buf, "PDFlib: %s ", bi_libpdf());
	CreateLabel(rc, buf);
#endif
	fr = CreateFrame(about_panel, "Home page");
        rc = CreateVContainer(fr);
	CreateLabel(rc, "http://plasma-gate.weizmann.ac.il/Grace/");
	
	CreateSeparator(about_panel);

	wbut = CreateButton(about_panel, "Close");
	AlignLabel(wbut, ALIGN_CENTER);
        AddButtonCB(wbut, destroy_dialog_cb, GetParent(about_frame));
        
        ManageChild(about_frame);
    }
    
    RaiseWindow(GetParent(about_frame));
    
    unset_wait_cursor();
}


#ifdef WITH_XMHTML
/*
 * Simplistic HTML viewer
 */

typedef struct _html_ui {
    Widget top;
    Widget html;
    TextStructure *location;
    Widget track;
    
    char *url;
    char *base;
    char *anchor;
    
    TextStructure *input;
    Widget case_sensitive;
    Widget find_backwards;
    
    XmHTMLTextFinder finder;
    char *last;
} html_ui;

static char *loadFile(char *URL)
{
    FILE *file;
    int size;
    char *content;

    /* open the given file */
    if ((file = grace_openr(URL, SOURCE_DISK)) == NULL) {
        return NULL;
    }

    /* see how large this file is */
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    /* allocate a buffer large enough to contain the entire file */
    if ((content = xmalloc(size + 1)) == NULL) {
        errmsg("xmalloc failed");
        return NULL;
    }

    /* now read the contents of this file */
    if ((fread(content, 1, size, file)) != size) {
        errmsg("Warning: did not read entire file!");
    }

    grace_close(file);

    /* sanity */
    content[size] = '\0';

    return content;
}

static char *translateURL(char *url, char *base)
{
    char *fname;
    URLType type;
    
    if (url == NULL) {
        return NULL;
    }
    
    type = XmHTMLGetURLType(url);
    if (type != ANCHOR_FILE_LOCAL || url[0] == '/') {
        fname = copy_string(NULL, url);
    } else {
        char *p;
        fname = copy_string(NULL, base);
        p = strrchr(fname, '/');
        if (p) {
            p++;
            *p = '\0';
            fname = concat_strings(fname, url);
        } else {
            fname = copy_string(NULL, url);
        }
    }
    
    return fname;
}

static void anchorCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int id;
    XmHTMLAnchorPtr href_data = (XmHTMLAnchorPtr) call_data;
    html_ui *ui = (html_ui *) client_data;
    char *turl;
    
    /* see if we have been called with a valid reason */
    if (href_data->reason != XmCR_ACTIVATE) {
        return;
    }

    switch (href_data->url_type) {
    /* a named anchor */
    case ANCHOR_JUMP:
        /* see if XmHTML knows this anchor */
        if ((id = XmHTMLAnchorGetId(w, href_data->href)) != -1) {
            /* and let XmHTML jump and mark as visited */
            href_data->doit = True;
            href_data->visited = True;
            
            ui->url = copy_string(ui->url, ui->base);
            ui->url = concat_strings(ui->url, href_data->href);
            SetTextString(ui->location, ui->url);
        }
        break;
    /* let HelpCB check all other types */
    default:
        turl = translateURL(href_data->href, ui->base);
        HelpCB(turl);
        xfree(turl);
        break;
    }
}

static void trackCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmHTMLAnchorPtr href_data = (XmHTMLAnchorPtr) call_data;
    html_ui *ui = (html_ui *) client_data;

    /* see if we have been called with a valid reason */
    if (href_data->reason != XmCR_HTML_ANCHORTRACK) {
        return;
    }

    if (href_data->href) {
        /* a valid anchor, eg, moving into an anchor */
        SetLabel(ui->track, href_data->href);
    } else {
        /* a valid anchor, eg, moving away from an anchor */
        SetLabel(ui->track, "");
    }
}

static int find_cb(void *data)
{
    char *s, *ptr;
    int case_sensitive, find_backwards;
    XmHTMLTextPosition start, end;
    html_ui *ui = (html_ui *) data;
    
    ptr = GetTextString(ui->input);

    if (!ptr || ptr[0] == '\0') {
        return RETURN_FAILURE;
    }
    
    if (ui->finder == NULL) {
        ui->finder = XmHTMLTextFinderCreate(ui->html);
        if (ui->finder == NULL) {
            errmsg("XmHTMLTextFinderCreate failed!");
            return RETURN_FAILURE;
        }
    }

    s = copy_string(NULL, ptr);

    case_sensitive = GetToggleButtonState(ui->case_sensitive);
    find_backwards = GetToggleButtonState(ui->find_backwards);

    /*****
    * The second arg represent regcomp flags, the default being
    * REG_EXTENDED. Using -1 for this arg instructs the finder to
    * keep the current flags. See man regcomp on possible values for
    * your system. The third arg specifies whether or not the search
    * should be done case-insensitive (True) or not (False). The last arg
    * specifies the search direction. Currently only forward (top to
    * bottom) is supported.
    *****/
    XmHTMLTextFinderSetPatternFlags(ui->finder,
        -1,
        case_sensitive ? False : True,
        find_backwards ? XmHTML_BACKWARD : XmHTML_FORWARD);
    
    if (ui->last == NULL || strcmp(ui->last, s)) {
        if(!XmHTMLTextFinderSetPattern(ui->finder, s)) {
            /* failure dialog */
            ptr = XmHTMLTextFinderGetErrorString(ui->finder);

            errmsg(ptr ? ptr : "(unknown error)");

            /* must free this */
            xfree(ptr);
            xfree(ui->last);
            ui->last = s;
            return RETURN_FAILURE;
        }
    }

    switch (XmHTMLTextFindString(ui->html, ui->finder)) {
    case XmREG_ERROR:
        ptr = XmHTMLTextFinderGetErrorString(ui->finder);
        errmsg(ptr ? ptr : "(unknown error)");
        xfree(ptr);
        break;
    case XmREG_NOMATCH:
        if (yesno("End of document reached; continue from beginning?",
                  NULL, NULL, NULL) == TRUE) {
            xfree(s);
            XCFREE(ui->last);
            return find_cb(ui);
        } 
        break;
    case XmREG_MATCH:
        if (XmHTMLTextFindToPosition(ui->html, ui->finder, &start, &end)) {
            XmHTMLTextSetHighlight(ui->html, start, end, XmHIGHLIGHT_SELECTED);
            XmHTMLTextShowPosition(ui->html, start);
        }
        break;
    }

    xfree(ui->last);
    ui->last = s;
    
    return RETURN_SUCCESS;
}

static void create_find_dialog(void *data)
{
    static Widget dialog = NULL;
    html_ui *ui = (html_ui *) data;

    if (!dialog) {
        Widget rc, rc2;
        
        dialog = CreateDialogForm(ui->html, "Find Dialog");
        
        rc = CreateVContainer(dialog);
        ui->input = CreateTextInput(rc, "Find:");
        rc2 = CreateHContainer(rc);
        ui->case_sensitive = CreateToggleButton(rc2, "Case sensitive");
        ui->find_backwards = CreateToggleButton(rc2, "Find backwards (N/I)");

        CreateAACDialog(dialog, rc, find_cb, data);
        
        ManageChild(dialog);
    }

    RaiseWindow(GetParent(dialog));
}

static void refresh_cb(void *data)
{
    html_ui *ui = (html_ui *) data;
    XmHTMLRedisplay(ui->html);
}

static XmImageInfo *loadImage(Widget w,
    String url, Dimension width, Dimension height, XtPointer client_data)
{
    char *fname;
    XmImageInfo *image;
    html_ui *ui = (html_ui *) client_data;
    
    fname = translateURL(url, ui->base);
    if (fname == NULL) {
        return NULL;
    }
    
    image = XmHTMLImageDefaultProc(w, fname, NULL, 0);
    
    xfree(fname);
    
    return image;
}

void location_cb(void *data)
{
    TextStructure *location = (TextStructure *) data;
    char *url = GetTextString(location);
    HelpCB(url);
}

void create_helper_frame(char *URL)
{
    static html_ui *ui = NULL;
    char *content;
    
    set_wait_cursor();
    
    if (ui == NULL) {
        Widget fr1, fr2, menubar, menupane, rc;
        
	ui = xmalloc(sizeof(html_ui));
        
        ui->url = NULL;
        ui->base = NULL;
        ui->anchor = NULL;
        
        ui->finder = NULL;
        ui->last = NULL;
        
        ui->top = CreateDialogForm(NULL, "Gracilla");
	
        menubar = CreateMenuBar(ui->top);
        ManageChild(menubar);
        AddDialogFormChild(ui->top, menubar);
        
        menupane = CreateMenu(menubar, "File", 'F', FALSE);
        CreateMenuButton(menupane, "Close", 'C', destroy_dialog_cb, GetParent(ui->top));
        
        menupane = CreateMenu(menubar, "Edit", 'E', FALSE);
        CreateMenuButton(menupane, "Find", 'F', create_find_dialog, ui);

        menupane = CreateMenu(menubar, "View", 'V', FALSE);
        CreateMenuButton(menupane, "Refresh", 'R', refresh_cb, ui);

        menupane = CreateMenu(menubar, "Help", 'H', TRUE);
        CreateMenuButton(menupane, "User's Guide", 'G', HelpCB, "doc/UsersGuide.html");
        CreateMenuButton(menupane, "Tutorial", 'T', HelpCB, "doc/Tutorial.html");
        CreateMenuButton(menupane, "FAQ", 'Q', HelpCB, "doc/FAQ.html");
        CreateMenuButton(menupane, "Changes", 'C', HelpCB, "doc/CHANGES.html");
        CreateMenuSeparator(menupane);
        CreateMenuButton(menupane, "License terms", 'L', HelpCB, "doc/GPL.html");

        ui->location = CreateTextInput(ui->top, "Location:");
        AddTextInputCB(ui->location, location_cb, ui->location);
        AddDialogFormChild(ui->top, ui->location->form);
        
	fr1 = CreateFrame(ui->top, NULL);
        AddDialogFormChild(ui->top, fr1);
        ui->html = XtVaCreateManagedWidget("html",
            xmHTMLWidgetClass, fr1,
            XmNimageProc, loadImage,
            XmNclientData, (XtPointer) ui,
            XmNenableBadHTMLWarnings, XmHTML_NONE,
            XmNanchorButtons, False,
            XmNmarginWidth, 20,
            XmNmarginHeight, 20,
            NULL);

	XtAddCallback(ui->html, XmNactivateCallback, anchorCB, ui);
        XtAddCallback(ui->html, XmNanchorTrackCallback, trackCB, ui);

	fr2 = CreateFrame(ui->top, NULL);
        AddDialogFormChild(ui->top, fr2);
        rc = CreateVContainer(fr2);
        ui->track = CreateLabel(rc, "Welcome to Gracilla!");
        
        XtVaSetValues(fr1,
            XmNbottomAttachment, XmATTACH_WIDGET,
            XmNbottomWidget, fr2,
            NULL);
        XtVaSetValues(fr2,
            XmNtopAttachment, XmATTACH_NONE,
            NULL);
	
        ManageChild(ui->top);
        
        XtVaSetValues(rc, XmNresizeHeight, False, NULL);
        
        ManageChild(GetParent(ui->top));
    }
    
    ui->url  = copy_string(ui->url, URL);
    ui->base = copy_string(ui->base, URL);
    if (ui->url) {
        char *p;
        
        p = strchr(ui->url, '#');
        if (p) {
            ui->base[p - ui->url] = '\0';
            ui->anchor = copy_string(ui->anchor, p);
        } else {
            XCFREE(ui->anchor);
        }
    }
    
    SetTextString(ui->location, ui->url);

    if (ui->finder) {
        XmHTMLTextFinderDestroy(ui->finder);
        ui->finder = NULL;
        ui->last = NULL;
    }
    
    content = loadFile(ui->base);
    if (content != NULL) {
        XmHTMLTextSetString(ui->html, content);
        if (ui->anchor) {
            int id = XmHTMLAnchorGetId(ui->html, ui->anchor);
            if (id != -1) {
                XmHTMLAnchorScrollToId(ui->html, id);
            }
        } else {
            XmHTMLTextScrollToLine(ui->html, 0);
        }
        xfree(content);

        RaiseWindow(GetParent(ui->top));
    }
    
    unset_wait_cursor();
}
#endif /* WITH_XMHTML */
