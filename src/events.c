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
 * event handler
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "globals.h"
#include "events.h"
#include "utils.h"
#include "graphs.h"
#include "draw.h"
#include "graphutils.h"
#include "x11drv.h"
#include "plotone.h"
#include "protos.h"

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>

#include "motifinc.h"

extern Widget loclab;
extern Widget arealab;
extern Widget perimlab;

int cursortype = 0;

static VPoint anchor_vp = {0.0, 0.0};
static int x, y;                /* pointer coordinates */
static int anchor_x = 0;
static int anchor_y = 0;
static view bb;

static int move_dir;

static int action_flag = 0;

extern int regiontype;

/*
 * for region, area and perimeter computation
 */
#define MAX_POLY_POINTS 200
static int region_pts = 0;
static int iax[MAX_POLY_POINTS];
static int iay[MAX_POLY_POINTS];
static WPoint region_wps[MAX_POLY_POINTS];


void anchor_point(int curx, int cury, VPoint curvp)
{
     anchor_vp = curvp;
     anchor_x = curx;
     anchor_y = cury;
}

void my_proc(Widget parent, XtPointer data, XEvent *event)
{
    char keybuf;
    KeySym keys;
    XComposeStatus compose;
    static Time lastc = 0;  /* time of last mouse click */
    Boolean dbl_click;

    WPoint wp, wp_new;
    VPoint vp;
    VVector shift;
    view v;
    int cg, newg, loc;
    int track_setno;
    static int track_loc;
    int add_at;
    static int type, id;   /* for objects */
    int axisno;
    Datapoint dpoint;
    GLocator locator;
    
    cg = get_cg();
    get_tracking_props(&track_setno, &move_dir, &add_at);
    
    switch (event->type) {
    case MotionNotify:
	x = event->xmotion.x;
	y = event->xmotion.y;
	if (cursortype != 0) {
            crosshair_motion(x, y);
        }
	vp = xlibdev2VPoint(x, y);
        getpoints(&vp);

        if (focus_policy == FOCUS_FOLLOWS) {
            if ((newg = next_graph_containing(-1, vp)) != cg) {
                switch_current_graph(newg);
                cg = newg;
            }
        }
        switch (action_flag) {
        case DO_NOTHING:
            break;
        case VIEW_2ND:
        case ZOOM_2ND:
        case ZOOMX_2ND:
        case ZOOMY_2ND:
        case MAKE_BOX_2ND:
        case MAKE_ELLIP_2ND:
	    select_region(anchor_x, anchor_y, x, y, 1);
            break;
        case MOVE_OBJECT_2ND:
        case COPY_OBJECT2ND:
        case PLACE_LEGEND_2ND:
        case PLACE_TIMESTAMP_2ND:
            slide_region(bb, x - anchor_x, y - anchor_y, 1);
            break;
        case MAKE_LINE_2ND:
        case DEF_REGION2ND:
	    select_line(anchor_x, anchor_y, x, y, 1);
            break;
        case DEF_REGION:
	    if (region_pts > 0) {
                select_line(anchor_x, anchor_y, x, y, 1);
            }
            break;
        case MOVE_POINT2ND:
	    switch (move_dir) {
	    case MOVE_POINT_XY:
	        select_line(anchor_x, anchor_y, x, y, 1);
                break;
	    case MOVE_POINT_X:
	        select_line(x, anchor_y, anchor_x, anchor_y, 1);
	        break;
	    case MOVE_POINT_Y:
	        select_line(anchor_x, anchor_y, anchor_x, y, 1);
	        break;
	    }
            break;
        default:
            return;
            break;
        }

        break;
    case ButtonPress:
	x = event->xbutton.x;
	y = event->xbutton.y;
	vp = xlibdev2VPoint(x, y);
	getpoints(&vp);
	switch (event->xbutton.button) {
	case Button1:
            /* first, determine if it's double click */
            if (((XButtonEvent *) event)->time - lastc < CLICKINT) {
                dbl_click = True;
            } else {
                dbl_click = False;
            }
            lastc = ((XButtonEvent *) event)->time;
            
            switch (action_flag) {
            case 0:
                if (dbl_click == True && allow_dc == TRUE) {
                    track_setno = -1;
                    if (focus_clicked(cg, vp, &anchor_vp) == TRUE) {
                        xlibVPoint2dev(anchor_vp, &anchor_x, &anchor_y);
                        set_action(VIEW_2ND);
	                select_region(anchor_x, anchor_y, x, y, 0);
                    } else if (find_point(cg, vp, &track_setno, &loc) == RETURN_SUCCESS) {
                        define_symbols_popup((void *) track_setno);
                    } else if (axis_clicked(cg, vp, &axisno) == TRUE) {
                        create_axes_dialog(axisno);
                    } else if (title_clicked(cg, vp) == TRUE) {
                        create_graphapp_frame(cg);
                    } else if (legend_clicked(cg, vp, &bb) == TRUE) {
                        create_graphapp_frame(cg);
                    } else if (find_item(cg, vp, &bb, &type, &id) == RETURN_SUCCESS) {
                        object_edit_popup(type, id);
                    } else if (timestamp_clicked(vp, &bb) == TRUE) {
                        create_plot_frame();
                    } else if (graph_clicked(cg, vp) == TRUE) {
                        define_symbols_popup((void *) -1);
                    }
                } else {
                    if (focus_policy == FOCUS_CLICK) {
                        if ((newg = next_graph_containing(cg, vp)) != cg) {
                            switch_current_graph(newg);
                        }
                    }
                }
                break;
            case VIEW_2ND:
                set_action(DO_NOTHING);
		v.xv1 = MIN2(vp.x, anchor_vp.x);
		v.yv1 = MIN2(vp.y, anchor_vp.y);
		v.xv2 = MAX2(vp.x, anchor_vp.x);
		v.yv2 = MAX2(vp.y, anchor_vp.y);
                set_graph_viewport(cg, v);
                update_view(cg);
		xdrawgraph();
                break;
            case ZOOM_1ST:
                anchor_point(x, y, vp);
                set_action(ZOOM_2ND);
	        select_region(anchor_x, anchor_y, x, y, 0);
                break;
            case ZOOMX_1ST:
                anchor_point(x, y, vp);
                set_action(ZOOMX_2ND);
	        select_region(anchor_x, anchor_y, x, y, 0);
                break;
            case ZOOMY_1ST:
                anchor_point(x, y, vp);
                set_action(ZOOMY_2ND);
	        select_region(anchor_x, anchor_y, x, y, 0);
                break;
            case VIEW_1ST:
                anchor_point(x, y, vp);
                set_action(VIEW_2ND);
	        select_region(anchor_x, anchor_y, x, y, 0);
                break;
            case ZOOM_2ND:
                set_action(DO_NOTHING);
		newworld(cg, ALL_AXES, anchor_vp, vp);
                break;
            case ZOOMX_2ND:
                set_action(DO_NOTHING);
		newworld(cg, ALL_X_AXES, anchor_vp, vp);
                break;
            case ZOOMY_2ND:
                set_action(DO_NOTHING);
		newworld(cg, ALL_Y_AXES, anchor_vp, vp);
                break;
            case EDIT_OBJECT:
                if (find_item(cg, vp, &bb, &type, &id) == RETURN_SUCCESS) {
                    object_edit_popup(type, id);
                }
                break;
            case DEL_OBJECT:
                if (find_item(cg, vp, &bb, &type, &id) == RETURN_SUCCESS) {
                    char message[32];
                    sprintf(message, "Kill the %s?", object_types(type));
                    if (yesno(message, NULL, NULL, NULL) == TRUE) {
                        kill_object(type, id);
                        xdrawgraph();
                    }
                }
                break;
            case MOVE_OBJECT_1ST:
                if (find_item(cg, vp, &bb, &type, &id) == RETURN_SUCCESS) {
                    anchor_point(x, y, vp);
	            slide_region(bb, x - anchor_x, y - anchor_y, 0);
                    set_action(MOVE_OBJECT_2ND);
                }
                break;
            case MOVE_OBJECT_2ND:
                shift.x = vp.x - anchor_vp.x;
                shift.y = vp.y - anchor_vp.y;
                move_object(type, id, shift);
                xdrawgraph();
                set_action(MOVE_OBJECT_1ST);
                break;
            case COPY_OBJECT1ST:
                if (find_item(cg, vp, &bb, &type, &id) == RETURN_SUCCESS) {
                    anchor_point(x, y, vp);
	            slide_region(bb, x - anchor_x, y - anchor_y, 0);
                    set_action(COPY_OBJECT2ND);
                }
                break;
            case COPY_OBJECT2ND:
                shift.x = vp.x - anchor_vp.x;
                shift.y = vp.y - anchor_vp.y;
                id = duplicate_object(type, id);
                move_object(type, id, shift);
                xdrawgraph();
                set_action(COPY_OBJECT1ST);
                break;
            case STR_LOC:
                id = next_string();
                init_string(id, vp);
                object_edit_popup(OBJECT_STRING, id);
                break;
            case MAKE_LINE_1ST:
                anchor_point(x, y, vp);
	        select_line(anchor_x, anchor_y, x, y, 0);
                set_action(MAKE_LINE_2ND);
                break;
            case MAKE_LINE_2ND:
	        select_line(anchor_x, anchor_y, x, y, 0);
                id = next_line();
                init_line(id, anchor_vp, vp);
                xdrawgraph();
                set_action(MAKE_LINE_1ST);
                break;
            case MAKE_BOX_1ST:
                anchor_point(x, y, vp);
	        select_region(anchor_x, anchor_y, x, y, 0);
                set_action(MAKE_BOX_2ND);
                break;
            case MAKE_BOX_2ND:
	        select_region(anchor_x, anchor_y, x, y, 0);
                id = next_box();
                init_box(id, anchor_vp, vp);
                xdrawgraph();
                set_action(MAKE_BOX_1ST);
                break;
            case MAKE_ELLIP_1ST:
                anchor_point(x, y, vp);
	        select_region(anchor_x, anchor_y, x, y, 0);
                set_action(MAKE_ELLIP_2ND);
                break;
            case MAKE_ELLIP_2ND:
	        select_region(anchor_x, anchor_y, x, y, 0);
                id = next_ellipse();
                init_ellipse(id, anchor_vp, vp);
                xdrawgraph();
                set_action(MAKE_ELLIP_1ST);
                break;
            case AUTO_NEAREST:
                if (find_point(cg, vp, &track_setno, &loc) == RETURN_SUCCESS) {
                    autoscale_byset(cg, track_setno, AUTOSCALE_XY);
                    update_ticks(cg);
                    xdrawgraph();
                    set_action(DO_NOTHING);
                }
                break;
            case TRACKER:
                track_point(cg, track_setno, &track_loc, -1);
                break;
            case DEL_POINT:
                if (find_point(cg, vp, &track_setno, &loc) == RETURN_SUCCESS) {
		    del_point(cg, track_setno, loc);
		    update_set_lists(cg);
                    xdrawgraph();
                }
                break;
            case MOVE_POINT1ST:
                if (find_point(cg, vp, &track_setno, &track_loc) == RETURN_SUCCESS) {
                    anchor_point(x, y, vp);
                    get_point(cg, track_setno, track_loc, &wp);

	            select_line(anchor_x, anchor_y, x, y, 0);
		    set_action(MOVE_POINT2ND);
                }
                break;
            case MOVE_POINT2ND:
                if (is_valid_setno(cg, track_setno)) {
                    get_point(cg, track_setno, track_loc, &wp);
                    view2world(vp.x, vp.y, &wp_new.x, &wp_new.y);

		    switch (move_dir) {
		    case 0:
		        wp = wp_new;
                        break;
		    case 1:
		        wp.x = wp_new.x;
		        break;
		    case 2:
		        wp.y = wp_new.y;
		        break;
		    }

                    set_point(cg, track_setno, track_loc, wp);
                    
                    update_point_locator(cg, track_setno, track_loc);
                    xdrawgraph();
		    set_action(MOVE_POINT1ST);
                }
                break;
            case ADD_POINT:
		view2world(vp.x, vp.y, &wp.x, &wp.y);
                zero_datapoint(&dpoint);
                dpoint.ex[0] = wp.x;
                dpoint.ex[1] = wp.y;
                switch (add_at) {
		case ADD_POINT_BEGINNING: /* at the beginning */
		    loc = 0;
		    break;
		case ADD_POINT_END: /* at the end */
		    loc = getsetlength(cg, track_setno);
		    break;
		default: /* between nearest points */
		    loc = find_insert_location(cg, track_setno, vp);
		    break;
		}
		if (add_point_at(cg, track_setno, loc, &dpoint)
                    == RETURN_SUCCESS) {
		    update_set_lists(cg);
                    xdrawgraph();
                }
                break;
            case PLACE_LEGEND_1ST:
                if (legend_clicked(cg, vp, &bb) == TRUE) {
                    anchor_point(x, y, vp);
	            slide_region(bb, x - anchor_x, y - anchor_y, 0);
                    set_action(PLACE_LEGEND_2ND);
                }
                break;
            case PLACE_LEGEND_2ND:
                shift.x = vp.x - anchor_vp.x;
                shift.y = vp.y - anchor_vp.y;
                move_legend(cg, shift);
                updatelegends(cg);
                xdrawgraph();
                set_action(PLACE_LEGEND_1ST);
                break;
            case PLACE_TIMESTAMP_1ST:
                if (timestamp_clicked(vp, &bb) == TRUE) {
                    anchor_point(x, y, vp);
	            slide_region(bb, x - anchor_x, y - anchor_y, 0);
                    set_action(PLACE_TIMESTAMP_2ND);
                }
                break;
            case PLACE_TIMESTAMP_2ND:
                shift.x = vp.x - anchor_vp.x;
                shift.y = vp.y - anchor_vp.y;
                move_timestamp(shift);
                xdrawgraph();
                set_action(PLACE_TIMESTAMP_1ST);
                break;
	    case SEL_POINT:
		if (get_graph_locator(cg, &locator) == RETURN_SUCCESS) {
		    view2world(vp.x, vp.y, &locator.dsx, &locator.dsy);
                    locator.pointset = TRUE;
		    set_graph_locator(cg, &locator);
		    update_locator_items(cg);
                    xdrawgraph();
                }
		set_action(DO_NOTHING);
		break;
	    case DEF_REGION1ST:
		anchor_point(x, y, vp);
                select_line(anchor_x, anchor_y, x, y, 0);
		set_action(DEF_REGION2ND);
		break;
	    case DEF_REGION2ND:
		set_action(DO_NOTHING);
                select_line(anchor_x, anchor_y, x, y, 0);
                activate_region(nr, regiontype, cg);
		view2world(anchor_vp.x, anchor_vp.y, &rg[nr].x1, &rg[nr].y1);
		view2world(vp.x, vp.y, &rg[nr].x2, &rg[nr].y2);
                xdrawgraph();
		break;
	    case DEF_REGION:
                anchor_point(x, y, vp);
		iax[region_pts] = x;
		iay[region_pts] = y;
                view2world(vp.x, vp.y,
                    &region_wps[region_pts].x, &region_wps[region_pts].y);
		if (region_pts < MAX_POLY_POINTS) {
                    region_pts++;
                } else {
                    errmsg("Too many points in polygon!");
                }
                select_line(anchor_x, anchor_y, x, y, 0);
		break;
            default:
                break;
            }
            break;
	case Button2:
            switch (action_flag) {
            case TRACKER:
                track_setno = -1;
                if (find_point(cg, vp, &track_setno, &track_loc) == RETURN_SUCCESS) {
                    track_point(cg, track_setno, &track_loc, 0);
                } else {
                    update_point_locator(cg, track_setno, track_loc);
                }
                break;
            default:
                break;
            }
            break;
	case Button3:
            switch (action_flag) {
            case DO_NOTHING:
/*
 *                 find_item(cg, vp, &anchor_vp, &type, &id);
 *                 sprintf(buf, "type = %d, id = %d", type, id);
 *                 set_left_footer(buf);
 */
                break;
            case TRACKER:
                track_point(cg, track_setno, &track_loc, +1);
                break;
            case DEF_REGION:
		/* end region definition */
                select_line(x, y, iax[0], iay[0], 0);
		load_poly_region(nr, cg, region_pts, region_wps);
                set_action(DO_NOTHING);
                xdrawgraph();
                break;
            default:
                set_action(DO_NOTHING);
                break;
            }
            return;
	default: /* TODO: wheel mice */
            break;
        }
	break;
    case KeyPress:
	XLookupString((XKeyEvent *) event, &keybuf, 1, &keys, &compose);
        if (keybuf == 27) { /* Esc */
            set_action(DO_NOTHING);
            return;
        }
        break;
    default:
	break;
    }
}




/*
 * action callback
 */
void set_actioncb(void *data)
{
    int func = (int) data;
    set_action(DO_NOTHING);
    set_action(func);
}


/*
 * set the action_flag to the desired action (actions are
 * defined in defines.h), if 0 then cleanup the results
 * from previous actions.
 */
void set_action(CanvasAction act)
{
    int i;
/*
 * indicate what's happening with a message in the left footer
 */
    switch (act) {
    case 0:
        switch (action_flag) {
        case ZOOM_2ND:
        case ZOOMX_2ND:
        case ZOOMY_2ND:
        case VIEW_2ND:
            select_region(anchor_x, anchor_y, x, y, 0);
            break;
        case MOVE_OBJECT_2ND:
        case COPY_OBJECT2ND:
        case PLACE_LEGEND_2ND:
        case PLACE_TIMESTAMP_2ND:
            slide_region(bb, x - anchor_x, y - anchor_y, 0);
            break;
        case MAKE_LINE_2ND:
        case DEF_REGION2ND:
	    select_line(anchor_x, anchor_y, x, y, 0);
            break;
        case DEF_REGION:
	    select_line(anchor_x, anchor_y, x, y, 0);
	    for (i = 0; i < region_pts - 1; i++) {
                select_line(iax[i], iay[i], iax[i + 1], iay[i + 1], 0); 
            }
            break;
        case MOVE_POINT2ND:
	    switch (move_dir) {
	    case 0:
	        select_line(anchor_x, anchor_y, x, y, 0);
                break;
	    case 1:
	        select_line(anchor_x, anchor_y, x, anchor_y, 0);
	        break;
	    case 2:
	        select_line(anchor_x, anchor_y, anchor_x, y, 0);
	        break;
	    }
            break;
        default:
            break;
        }

	set_cursor(-1);
	set_left_footer(NULL);
	break;
    case ZOOM_1ST:
	set_cursor(0);
	set_left_footer("Pick first corner for zoom");
	break;
    case ZOOM_2ND:
	set_left_footer("Pick second corner for zoom");
	break;
    case ZOOMX_1ST:
	set_cursor(0);
	set_left_footer("Pick first point for zoom along X-axis");
	break;
    case ZOOMX_2ND:
	set_left_footer("Pick second point for zoom along X-axis");
	break;
    case ZOOMY_1ST:
	set_cursor(0);
	set_left_footer("Pick first point for zoom along Y-axis");
	break;
    case ZOOMY_2ND:
	set_left_footer("Pick second point for zoom along Y-axis");
	break;
    case VIEW_1ST:
	set_cursor(0);
	set_left_footer("Pick first corner of viewport");
	break;
    case VIEW_2ND:
	set_left_footer("Pick second corner of viewport");
	break;
    case EDIT_OBJECT:
	set_cursor(1);
	set_left_footer("Pick object to edit");
	break;
    case DEL_OBJECT:
	set_cursor(3);
	set_left_footer("Delete object");
	break;
    case MOVE_OBJECT_1ST:
	set_cursor(1);
	set_left_footer("Pick object to move");
	break;
    case COPY_OBJECT1ST:
	set_cursor(1);
	set_left_footer("Pick object to copy");
	break;
    case MOVE_OBJECT_2ND:
    case COPY_OBJECT2ND:
	set_cursor(4);
	set_left_footer("Place object");
	break;
    case STR_LOC:
	set_cursor(2);
	set_left_footer("Pick beginning of text");
	break;
    case MAKE_LINE_1ST:
	set_cursor(0);
	set_left_footer("Pick beginning of line");
	break;
    case MAKE_LINE_2ND:
	set_left_footer("Pick end of line");
	break;
    case MAKE_BOX_1ST:
	set_cursor(0);
	set_left_footer("First corner of box");
	break;
    case MAKE_BOX_2ND:
	set_left_footer("Second corner of box");
	break;
    case MAKE_ELLIP_1ST:
	set_cursor(0);
	set_left_footer("Pick beginning of bounding box for ellipse");
	break;
    case MAKE_ELLIP_2ND:
	set_left_footer("Pick opposite corner");
	break;
    case AUTO_NEAREST:
	set_cursor(0);
	set_left_footer("Autoscale on nearest set - click near a point of the set to autoscale");
	break;
    case TRACKER:
	set_cursor(1);
	set_left_footer("Tracker");
	break;
    case DEL_POINT:
	set_cursor(3);
	set_left_footer("Delete point");
	break;
    case MOVE_POINT1ST:
	set_cursor(4);
	set_left_footer("Pick point to move");
	break;
    case MOVE_POINT2ND:
	set_left_footer("Pick final location");
	break;
    case ADD_POINT:
	set_cursor(0);
	set_left_footer("Add point");
	break;
    case PLACE_LEGEND_1ST:
	set_cursor(1);
	set_left_footer("Pick legend");
	break;
    case PLACE_LEGEND_2ND:
	set_cursor(4);
	set_left_footer("Move legend");
	break;
    case PLACE_TIMESTAMP_1ST:
	set_cursor(1);
	set_left_footer("Pick timestamp");
	break;
    case PLACE_TIMESTAMP_2ND:
	set_cursor(4);
	set_left_footer("Place timestamp");
	break;
    case SEL_POINT:
	set_cursor(0);
	set_left_footer("Pick reference point");
	break;
    case DEF_REGION1ST:
	set_cursor(0);
	set_left_footer("Pick first point for region");
	break;
    case DEF_REGION2ND:
	set_left_footer("Pick second point for region");
	break;
    case DEF_REGION:
	set_cursor(0);
	set_left_footer("Define region");
	break;

    case COMP_AREA:
	set_cursor(0);
	set_left_footer("Compute area");
	break;
    case COMP_PERIMETER:
	set_cursor(0);
	set_left_footer("Compute perimeter");
	break;
    case DISLINE1ST:
	set_cursor(0);
	set_left_footer("Pick start of line for distance computation");
	break;
    case DISLINE2ND:
	set_cursor(0);
	set_left_footer("Pick ending point");
	break;
    }

    action_flag = act;
}


void track_point(int gno, int setno, int *loc, int shift)
{
    int len;
    double *xtmp, *ytmp;
    WPoint wp;
    VPoint vp;
    world w;
    
    if ((len = getsetlength(gno, setno)) > 0) {
        *loc += shift;
        if (*loc < 0) {
            *loc += len;
        } else {
            *loc = *loc % len;
        }
        xtmp = getx(gno, setno);
        ytmp = gety(gno, setno);
        wp.x = xtmp[*loc];
        wp.y = ytmp[*loc];
       
        get_graph_world(gno, &w);
        wp.x = MAX2(wp.x, w.xg1);
        wp.x = MIN2(wp.x, w.xg2);
        wp.y = MAX2(wp.y, w.yg1);
        wp.y = MIN2(wp.y, w.yg2);
        vp = Wpoint2Vpoint(wp);
        setpointer(vp);

        update_point_locator(gno, setno, *loc);
    }
}


/*
 * set format string for locator
 */
static char *typestr[6] = {"X, Y",
                           "DX, DY",
			   "DIST",
			   "Phi, Rho",
			   "VX, VY",
                           "SX, SY"};

/*
 * locator on main_panel
 */
void getpoints(VPoint *vpp)
{
    static VPoint vp = {0.0, 0.0};
    int cg = get_cg();
    double wx, wy, xtmp, ytmp;
    int x, y;
    double dsx = 0.0, dsy = 0.0;
    char buf[256], bufx[64], bufy[64], *s;
    GLocator locator;
    
    if (vpp != NULL) {
        vp = *vpp;
    }
    
    view2world(vp.x, vp.y, &wx, &wy);
    if (get_graph_locator(cg, &locator) != RETURN_SUCCESS) {
        SetLabel(loclab, "[No graphs]");
        return;
    }
    
    if (locator.pointset) {
	dsx = locator.dsx;
	dsy = locator.dsy;
    }
    
    switch (locator.pt_type) {
    case 0:
        if (get_graph_type(cg) == GRAPH_POLAR) {
            polar2xy(wx, wy, &xtmp, &ytmp);
        } else {
            xtmp = wx;
            ytmp = wy;
        }
        break;
    case 1:
        xtmp = wx - dsx;
        ytmp = wy - dsy;
        break;
    case 2:
        if (get_graph_type(cg) == GRAPH_POLAR) {
            polar2xy(wx, wy, &xtmp, &ytmp);
        } else {
            xtmp = wx;
            ytmp = wy;
        }
        xtmp = hypot(dsx - xtmp, dsy - ytmp);
        ytmp = 0.0;
        break;
    case 3:
        if (dsx - wx != 0.0 || dsy - wy != 0.0) {
            xy2polar(wx - dsx, wy - dsy, &xtmp, &ytmp);
        } else {
            xtmp = 0.0;
            ytmp = 0.0;
        }
        break;
    case 4:
        xtmp = vp.x;
        ytmp = vp.y;
        break;
    case 5:
        xlibVPoint2dev(vp, &x, &y);
        xtmp = x;
        ytmp = y;
        break;
    default:
        return;
    }
    s = create_fstring(locator.fx, locator.px, xtmp, LFORMAT_TYPE_PLAIN);
    strcpy(bufx, s);
    s = create_fstring(locator.fy, locator.py, ytmp, LFORMAT_TYPE_PLAIN);
    strcpy(bufy, s);
    sprintf(buf, "G%1d: %s = [%s, %s]", cg, typestr[locator.pt_type], bufx, bufy);

    SetLabel(loclab, buf);
}


/*
 * switch on the area calculator
 */
void do_select_area(void)
{
    set_action(DO_NOTHING);
    set_action(COMP_AREA);
}

/*
 * switch on the perimeter calculator
 */
void do_select_peri(void)
{
    set_action(DO_NOTHING);
    set_action(COMP_PERIMETER);
}

void do_dist_proc(void)
{
    set_action(DO_NOTHING);
    set_action(DISLINE1ST);
}


/*
 * define a (polygon) region
 */
void do_select_region(void)
{
    region_pts = 0;
    set_action(DO_NOTHING);
    set_action(DEF_REGION);
}

/*
 * Given the graph gno, find the graph that contains
 * (wx, wy). Used for setting the graph focus.
 */
int next_graph_containing(int cg, VPoint vp)
{
    int i, j, ng, gno = -1;
    view v;

    ng = number_of_graphs();

    if (is_valid_gno(cg) == FALSE) {
        cg = -1;
    }

    for (i = 0; i < ng ; i++) {
	j = (i + cg + 1) % ng;
	if (is_graph_hidden(j)        == FALSE &&
            get_graph_viewport(j, &v) == RETURN_SUCCESS &&
            is_vpoint_inside(v, vp, MAXPICKDIST)   == TRUE) {
	    
            gno = j;
            break;
	}
    }

    return gno;
}

int legend_clicked(int gno, VPoint vp, view *bb)
{
    legend l;

    if (is_graph_hidden(gno) == FALSE) {
        get_graph_legend(gno, &l);
	if (l.active && is_vpoint_inside(l.bb, vp, MAXPICKDIST)) {
	    *bb = l.bb;
            return TRUE;
	} else {
            return FALSE;
        }
    } else {
        return FALSE;
    }
}

int graph_clicked(int gno, VPoint vp)
{
    view v;

    if (is_graph_hidden(gno) == FALSE) {
        get_graph_viewport(gno, &v);
	if (is_vpoint_inside(v, vp, MAXPICKDIST)) {
            return TRUE;
	} else {
            return FALSE;
        }
    } else {
        return FALSE;
    }
}

int timestamp_clicked(VPoint vp, view *bb)
{
    if (timestamp.active && is_vpoint_inside(timestamp.bb, vp, MAXPICKDIST)) {
        *bb = timestamp.bb;
        return TRUE;
    } else {
        return FALSE;
    }
}

int focus_clicked(int cg, VPoint vp, VPoint *avp)
{
    view v;
    
    if (is_graph_hidden(cg) == TRUE) {
        return FALSE;
    }
    if (get_graph_viewport(cg, &v) != RETURN_SUCCESS) {
        return FALSE;
    }

    if (fabs(vp.x - v.xv1) < MAXPICKDIST && fabs(vp.y - v.yv1) < MAXPICKDIST) {
        avp->x = v.xv2;
        avp->y = v.yv2;
        return TRUE;
    } else if (fabs(vp.x - v.xv1) < MAXPICKDIST && fabs(vp.y - v.yv2) < MAXPICKDIST) {
        avp->x = v.xv2;
        avp->y = v.yv1;
        return TRUE;
    } else if (fabs(vp.x - v.xv2) < MAXPICKDIST && fabs(vp.y - v.yv1) < MAXPICKDIST) {
        avp->x = v.xv1;
        avp->y = v.yv2;
        return TRUE;
    } else if (fabs(vp.x - v.xv2) < MAXPICKDIST && fabs(vp.y - v.yv2) < MAXPICKDIST) {
        avp->x = v.xv1;
        avp->y = v.yv1;
        return TRUE;
    } else {
        return FALSE;
    }
}

int axis_clicked(int gno, VPoint vp, int *axisno)
{
    view v;
    
    /* TODO: check for offsets, zero axes, polar graphs */
    if (is_graph_hidden(gno) == TRUE) {
        return FALSE;
    } else {
        get_graph_viewport(gno, &v);
        if (vp.x >= v.xv1 && vp.x <= v.xv2 &&
            (fabs(vp.y - v.yv1) < MAXPICKDIST ||
             fabs(vp.y - v.yv2) < MAXPICKDIST)) {
            *axisno = X_AXIS;
            return TRUE;
        } else if (vp.y >= v.yv1 && vp.y <= v.yv2 &&
            (fabs(vp.x - v.xv1) < MAXPICKDIST ||
             fabs(vp.x - v.xv2) < MAXPICKDIST)) {
            *axisno = Y_AXIS;
            return TRUE;
        } else {
            return FALSE;
        }
    }
}

int title_clicked(int gno, VPoint vp)
{
    view v;
    
    /* a rude check; TODO: use right offsets */
    if (is_graph_hidden(gno) == TRUE) {
        return FALSE;
    } else {
        get_graph_viewport(gno, &v);
        if (vp.x >= v.xv1 && vp.x <= v.xv2 &&
            vp.y > v.yv2 && vp.y < v.yv2 + 0.1) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
}

/*
 * locate a point and the set the point is in
 */
int find_point(int gno, VPoint vp, int *setno, int *loc)
{
    int i, start, stop, j, found;
    double *xtmp, *ytmp;
    WPoint wptmp;
    VPoint vptmp;
    double dist, mindist = MAXPICKDIST;

    if (is_valid_gno(gno) != TRUE) {
        return RETURN_FAILURE;
    }
        
    if (is_valid_setno(gno, *setno)) {
        start = *setno;
        stop = *setno;
    } else {
        start = 0;
        stop = number_of_sets(gno) - 1;
    }
    found = FALSE;
    for (i = start; i <= stop; i++) {
	if (is_set_hidden(gno, i) == FALSE) {
	    xtmp = getx(gno, i);
	    ytmp = gety(gno, i);
	    for (j = 0; j < getsetlength(gno, i); j++) {
		wptmp.x = xtmp[j];
		wptmp.y = ytmp[j];
                vptmp = Wpoint2Vpoint(wptmp);
                
                dist = MAX2(fabs(vp.x - vptmp.x), fabs(vp.y - vptmp.y));
                if (dist < mindist) {
		    found = TRUE;
                    *setno = i;
		    *loc = j;
                    mindist = dist;
		}
	    }
	}
    }
    
    if (found == FALSE) {
        return RETURN_FAILURE;
    } else {
        update_point_locator(gno, *setno, *loc);
        return RETURN_SUCCESS;
    }
}

int find_insert_location(int gno, int setno, VPoint vp)
{
    int j, loc = -1;
    double *xtmp, *ytmp;
    WPoint wptmp;
    VPoint vp1, vp2;
    double dist, mindist = 1.0;
    
    if (is_valid_setno(gno, setno) == TRUE) {
        if (is_set_hidden(gno, setno) == FALSE) {
            xtmp = getx(gno, setno);
            ytmp = gety(gno, setno);
            for (j = 0; j < getsetlength(gno, setno) - 1; j++) {
                wptmp.x = xtmp[j];
                wptmp.y = ytmp[j];
                vp1 = Wpoint2Vpoint(wptmp);
                wptmp.x = xtmp[j + 1];
                wptmp.y = ytmp[j + 1];
                vp2 = Wpoint2Vpoint(wptmp);
 
                dist = hypot(vp.x - vp1.x, vp.y - vp1.y) +
                       hypot(vp.x - vp2.x, vp.y - vp2.y);
                if (dist < mindist) {
                    loc = j + 1;
                    mindist = dist;
                }
            }
        }
    }
    
    return loc;
}


/*
 * find object containing vp inside its bb
 */
int find_item(int gno, VPoint vp, view *bb, int *type, int *id)
{
    int i;

    *type = OBJECT_NONE;
    for (i = 0; i < number_of_boxes(); i++) {
	if (isactive_box(i)) {
            get_object_bb(OBJECT_BOX, i, bb);
	    if (is_vpoint_inside(*bb, vp, MAXPICKDIST)) {
		*type = OBJECT_BOX;
		*id = i;
	    }
	}
    }
    for (i = 0; i < number_of_ellipses(); i++) {
	if (isactive_ellipse(i)) {
            get_object_bb(OBJECT_ELLIPSE, i, bb);
	    if (is_vpoint_inside(*bb, vp, MAXPICKDIST)) {
		*type = OBJECT_ELLIPSE;
		*id = i;
	    }
	}
    }
    for (i = 0; i < number_of_lines(); i++) {
	if (isactive_line(i)) {
            get_object_bb(OBJECT_LINE, i, bb);
	    if (is_vpoint_inside(*bb, vp, MAXPICKDIST)) {
		*type = OBJECT_LINE;
		*id = i;
	    }
	}
    }
    for (i = 0; i < number_of_strings(); i++) {
	if (isactive_string(i)) {
            get_object_bb(OBJECT_STRING, i, bb);
	    if (is_vpoint_inside(*bb, vp, MAXPICKDIST)) {
		*type = OBJECT_STRING;
		*id = i;
	    }
	}
    }
    
    if (*type == OBJECT_NONE) {
        return RETURN_FAILURE;
    } else {
        get_object_bb(*type, *id, bb);
        return RETURN_SUCCESS;
    }
}


/*
 * for zooms
 *
 */
void newworld(int gno, int axes, VPoint vp1, VPoint vp2)
{
    world w, wtmp;

    if (vp1.x == vp2.x && (axes == ALL_AXES || axes == ALL_X_AXES)) {
        errmsg("Zoomed rectangle is zero along X, zoom cancelled");
        return;
    }

    if (vp1.y == vp2.y && (axes == ALL_AXES || axes == ALL_Y_AXES)) {
        errmsg("Zoomed rectangle is zero along Y, zoom cancelled");
        return;
    }

    view2world(vp1.x, vp1.y, &w.xg1, &w.yg1);
    view2world(vp2.x, vp2.y, &w.xg2, &w.yg2);
    if (w.xg1 > w.xg2) {
        fswap(&w.xg1, &w.xg2);
    }
    if (w.yg1 > w.yg2) {
        fswap(&w.yg1, &w.yg2);
    }

    if (is_graph_active(gno)) {
        get_graph_world(gno, &wtmp);
        switch (axes) {
        case ALL_AXES:
            wtmp.xg1 = w.xg1;
            wtmp.xg2 = w.xg2;
            wtmp.yg1 = w.yg1;
            wtmp.yg2 = w.yg2;
            break;
        case ALL_X_AXES:
            wtmp.xg1 = w.xg1;
            wtmp.xg2 = w.xg2;
            break;
        case ALL_Y_AXES:
            wtmp.yg1 = w.yg1;
            wtmp.yg2 = w.yg2;
            break;
        default:
            return;
            break;
        }
        set_graph_world(gno, wtmp);
        autotick_axis(gno, axes);
        xdrawgraph();
    }
}

void switch_current_graph(int gno)
{
    int saveg = get_cg();
    
    if (is_graph_hidden(gno) == FALSE) {
        select_graph(gno);
        draw_focus(saveg);
        draw_focus(gno);
        update_all();
        set_graph_selectors(gno);
        getpoints(NULL);
    }
}


/* -------------------------------------------------------------- */
/* canvas_actions */
void autoscale_action(Widget w, XKeyEvent *e, String *p, Cardinal *c)
{
    int cg = get_cg();
    
    autoscale_graph(cg, AUTOSCALE_XY);
    update_ticks(cg);
    xdrawgraph();
}

void autoscale_on_near_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(AUTO_NEAREST);       
}

void draw_line_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(MAKE_LINE_1ST);
}

void draw_box_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(MAKE_BOX_1ST);       
}

void draw_ellipse_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(MAKE_ELLIP_1ST);
}

void write_string_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(STR_LOC);
}


void delete_object_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(DEL_OBJECT); 
}

void place_legend_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(PLACE_LEGEND_1ST);
}

void place_timestamp_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(PLACE_TIMESTAMP_1ST);
}

void move_object_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(MOVE_OBJECT_1ST);
}

void refresh_hotlink_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    do_hotupdate_proc(NULL);
}

void set_viewport_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(VIEW_1ST);
}

void enable_zoom_action( Widget w, XKeyEvent *e, String *p, Cardinal *c )
{
    set_action(DO_NOTHING);
    set_action(ZOOM_1ST);
}


/*
 * world stack operations
 */
void push_and_zoom(void)
{
    push_world();
    set_action(DO_NOTHING);
    set_action(ZOOM_1ST);
}
