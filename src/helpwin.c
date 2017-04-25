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

void HelpCB(void *data)
{
    char *URL, *ha;
    int remote;
    char *help_viewer, *command;
    int i, j, len, urllen, comlen;

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
            URL = copy_string(NULL, "file://");
            URL = concat_strings(URL, grace_path(base));
            URL = concat_strings(URL, pa);
            xfree(base);
        } else {
            URL = copy_string(NULL, grace_path(p));
        }

        remote = FALSE;
    }
    
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
    command = concat_strings(command, "&");    
    system_wrap(command);
    xfree(command);
    
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

