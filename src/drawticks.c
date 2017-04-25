/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2007 Grace Development Team
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
 * Draw axis bars, axis labels, ticks and tick labels
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "utils.h"
#include "graphs.h"
#include "graphutils.h"
#include "draw.h"
#include "parser.h"
#include "protos.h"

int is_xaxis(int axis)
{
    return ((axis % 2 == 0));
}

int is_yaxis(int axis)
{
    return ((axis % 2 == 1));
}

int is_log_axis(int gno, int axis)
{
    if ((is_xaxis(axis) && islogx(gno)) ||
        (is_yaxis(axis) && islogy(gno))) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int is_logit_axis(int gno, int axis)
{
    if ((is_xaxis(axis) && islogitx(gno)) ||
        (is_yaxis(axis) && islogity(gno))) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void drawgrid(int gno)
{
    int caxis;
    tickmarks *t;
    tickprops tprops;
    int ttype;
    world w;
    view v;
    double wtpos;
    WPoint wp_grid_start = {0.0, 0.0}, wp_grid_stop = {0.0, 0.0};
    VPoint vp_grid_start, vp_grid_stop;
    VPoint vpc, vp1, vp2;
    double phi_start, phi_stop, rho;
    double wc_start, wc_stop; /* world coordinates */
    int ittype_loop, itick;
        
    setclipping(TRUE);
    
    /* TODO: add Pen to ticks and remove the following */
    setpattern(1);
    
    get_graph_viewport(gno, &v);
    get_graph_world(gno, &w);
    
    /* graph center; for polar plots */
    vpc.x = (v.xv1 + v.xv2)/2.0;
    vpc.y = (v.yv1 + v.yv2)/2.0;
    
    for (caxis = 0; caxis < MAXAXES; caxis++) {
	t = get_graph_tickmarks(gno, caxis);
	if (!t || t->active != TRUE) {
            continue;
        }
	if (is_xaxis(caxis)) { /* an X-axis */
	    wc_start = w.xg1;
	    wc_stop = w.xg2;
            wp_grid_start.y = w.yg1;
            wp_grid_stop.y = w.yg2;
	} else {              /* a Y-axis */
	    wc_start = w.yg1;
	    wc_stop = w.yg2;
            wp_grid_start.x = w.xg1;
            wp_grid_stop.x = w.xg2;
	}
		
	for (ittype_loop = 0; ittype_loop < 2; ittype_loop++) {
	    if (ittype_loop == 0) { /* minor ticks */
                ttype = TICK_TYPE_MINOR;
                tprops = t->mprops;
	    } else {	  /* major ticks */
                ttype = TICK_TYPE_MAJOR;
                tprops = t->props;
	    }
            if (tprops.gridflag == 0) {
	        continue;
	    }

            setcolor(tprops.color);
	    setlinewidth(tprops.linew);
	    setlinestyle(tprops.lines);
	    
	    for (itick = 0; itick < t->nticks; itick++) {
	    	if (t->tloc[itick].type != ttype) {
	    	    continue;
	    	}
                
                wtpos = t->tloc[itick].wtpos;

	    	if ((wtpos < wc_start) || (wtpos > wc_stop)) {
	    	    continue;
	    	}

	        if (is_xaxis(caxis)) { /* an X-axis */
                    wp_grid_start.x = wtpos;
                    wp_grid_stop.x = wtpos;
	        } else {              /* a Y-axis */
                    wp_grid_start.y = wtpos;
                    wp_grid_stop.y = wtpos;
	        }
	    	
	    	vp_grid_start = Wpoint2Vpoint(wp_grid_start);
	    	vp_grid_stop = Wpoint2Vpoint(wp_grid_stop);


                if (!is_xaxis(caxis) && get_graph_type(gno) == GRAPH_POLAR) {
                    xy2polar(vp_grid_start.x - vpc.x, vp_grid_start.y - vpc.y,
                             &phi_start, &rho);
                    xy2polar(vp_grid_stop.x - vpc.x, vp_grid_stop.y - vpc.y,
                             &phi_stop, &rho);
                    vp1.x = vpc.x - rho;
                    vp1.y = vpc.y + rho;
                    vp2.x = vpc.x + rho;
                    vp2.y = vpc.y - rho;
                    if (is_graph_xinvert(gno) == TRUE) {
                        fswap(&phi_start, &phi_stop);
                    }
                    if (phi_stop < phi_start) {
                        phi_stop += 2*M_PI;
                    } 
                    DrawArc(vp1, vp2, (int) rint(180.0/M_PI*phi_start),
                                      (int) rint(180.0/M_PI*phi_stop));
                } else {
		    DrawLine(vp_grid_start, vp_grid_stop);
                }
	    }
	}
    }
}

void drawaxes(int gno)
{
    int caxis;
    tickmarks *t;
    tickprops tprops;
    world w;
    view v, bb;
    double vbase1, vbase2, vbase1_start, vbase1_stop, vbase2_start, vbase2_stop;
    double vbase_tlabel, vbase_tlabel1, vbase_tlabel2;
    double tsize, tlsize, wtpos, vtpos;
    double tl_offset, tl_trans;
    WPoint wp1_start, wp1_stop, wp2_start, wp2_stop;
    VPoint vp1_start, vp1_stop, vp2_start, vp2_stop;
    VPoint vp_tick1_start, vp_tick1_stop, vp_tick2_start, vp_tick2_stop;
    VPoint vp_tlabel, vp_label, vp_label_offset1, vp_label_offset2;
    VPoint vpc, vp1, vp2;
    double phi_start, phi_stop, rho;
    VVector ort_para, ort_perp;
    double wc_start, wc_stop, wc_start_labels, wc_stop_labels; /* world
                                                                coordinates */
    int ittype_loop, itick, itcur;
    int ttype;
    char tlabel[MAX_STRING_LENGTH];
    int tlabel1_just, tlabel2_just, label1_just, label2_just;
    int langle;
    
    int tick_dir_sign;
    
    double (*coord_conv) ();
    
    
    setclipping(FALSE);

    /* TODO: add Pen to ticks and remove the following */
    setpattern(1);
    
    get_graph_viewport(gno, &v);
    get_graph_world(gno, &w);

    /* graph center; for polar plots */
    vpc.x = (v.xv1 + v.xv2)/2.0;
    vpc.y = (v.yv1 + v.yv2)/2.0;
    
       
    for (caxis = 0; caxis < MAXAXES; caxis++) {
	t = get_graph_tickmarks(gno, caxis);
	if (!t || t->active != TRUE) {
            continue;
        }
        
        if (t->zero == FALSE) {
            tick_dir_sign = +1;
        } else {
            tick_dir_sign = -1;
        }
        
        if (is_xaxis(caxis)) { /* an X-axis */
	    ort_para.x = 1.0;
	    ort_para.y = 0.0;
	    ort_perp.x = 0.0;
	    ort_perp.y = 1.0;
            
	    coord_conv = xy_xconv;
	    
	    wc_start = w.xg1;
	    wc_stop = w.xg2;

            wp1_start.x = w.xg1;
	    wp1_stop.x  = w.xg2;
	    wp2_start.x = w.xg1;
	    wp2_stop.x  = w.xg2;
	    if (t->zero == TRUE) {
                if (w.yg1 <= 0.0 && w.yg2 >= 0.0) {
	            wp1_start.y = 0.0;
	            wp1_stop.y  = 0.0;
	            wp2_start.y = 0.0;
	            wp2_stop.y  = 0.0;
                } else {
                    continue;
                }
            } else {
	        wp1_start.y = w.yg1;
	        wp1_stop.y  = w.yg1;
	        wp2_start.y = w.yg2;
	        wp2_stop.y  = w.yg2;
            }
            
            vp1_start = Wpoint2Vpoint(wp1_start);
            vp1_stop  = Wpoint2Vpoint(wp1_stop);
            vp2_start = Wpoint2Vpoint(wp2_start);
            vp2_stop  = Wpoint2Vpoint(wp2_stop);
            
            if (is_graph_yinvert(gno) == TRUE) {
                vpswap(&vp1_start, &vp2_start);
                vpswap(&vp1_stop, &vp2_stop);
            }

            /* TODO axis offset for polar plots */
            if (get_graph_type(gno) != GRAPH_POLAR) {
                 vp1_start.y -= t->offsx;
                 vp1_stop.y  -= t->offsx;
                 vp2_start.y += t->offsy;
                 vp2_stop.y  += t->offsy;
            }
	    
	    vbase1 = vp1_start.y;
	    vbase2 = vp2_start.y;
	    
	    tlabel1_just = JUST_CENTER|JUST_TOP;
	    tlabel2_just = JUST_CENTER|JUST_BOTTOM;
	    
	    switch (t->label_layout) {
	    case LAYOUT_PARALLEL:
	    	langle =  0;
	    	break;
	    case LAYOUT_PERPENDICULAR:
	    	langle = 90;
	    	break;
	    default:
	    	errmsg("Internal error in drawaxes()");
	    	return;
	    }
	} else {              /* a Y-axis */
	    ort_para.x = 0.0;
	    ort_para.y = 1.0;
	    ort_perp.x = 1.0;
	    ort_perp.y = 0.0;
	    
	    coord_conv = xy_yconv;
	    
	    wc_start = w.yg1;
	    wc_stop = w.yg2;

	    wp1_start.y = w.yg1;
	    wp1_stop.y  = w.yg2;
	    wp2_start.y = w.yg1;
	    wp2_stop.y  = w.yg2;

	    if (t->zero == TRUE) {
                if (w.xg1 <= 0.0 && w.xg2 >= 0.0) {
	            wp1_start.x = 0.0;
	            wp1_stop.x  = 0.0;
	            wp2_start.x = 0.0;
	            wp2_stop.x  = 0.0;
                } else {
                    continue;
                }
            } else {
	        wp1_start.x = w.xg1;
	        wp1_stop.x  = w.xg1;
	        wp2_start.x = w.xg2;
	        wp2_stop.x  = w.xg2;
            }
            
            vp1_start = Wpoint2Vpoint(wp1_start);
            vp1_stop  = Wpoint2Vpoint(wp1_stop);
            vp2_start = Wpoint2Vpoint(wp2_start);
            vp2_stop  = Wpoint2Vpoint(wp2_stop);

            if (is_graph_xinvert(gno) == TRUE) {
                vpswap(&vp1_start, &vp2_start);
                vpswap(&vp1_stop, &vp2_stop);
            }

            if (get_graph_type(gno) != GRAPH_POLAR) {
                vp1_start.x -= t->offsx;
                vp1_stop.x  -= t->offsx;
                vp2_start.x += t->offsy;
                vp2_stop.x  += t->offsy;
            }
	    
	    vbase1 = vp1_start.x;
	    vbase2 = vp2_start.x;
	    
	    tlabel1_just = JUST_RIGHT|JUST_MIDDLE;
	    tlabel2_just = JUST_LEFT|JUST_MIDDLE;
	
	    switch (t->label_layout) {
	    case LAYOUT_PARALLEL:
	    	langle = 90;
	    	break;
	    case LAYOUT_PERPENDICULAR:
	    	langle =  0;
	    	break;
	    default:
	    	errmsg("Internal error in drawaxes()");
	    	return;
	    }
	}
	
	/* Begin axis bar stuff */
	if (t->t_drawbar) {
	    setcolor(t->t_drawbarcolor);
	    setlinewidth(t->t_drawbarlinew);
	    setlinestyle(t->t_drawbarlines);
	    if (t->t_op == PLACEMENT_NORMAL || t->t_op == PLACEMENT_BOTH) {
                if (is_xaxis(caxis) && get_graph_type(gno) == GRAPH_POLAR) {
                    xy2polar(vp1_start.x - vpc.x, vp1_start.y - vpc.y,
                             &phi_start, &rho);
                    xy2polar(vp1_stop.x - vpc.x, vp1_stop.y - vpc.y,
                             &phi_stop, &rho);
                    vp1.x = vpc.x - rho;
                    vp1.y = vpc.y + rho;
                    vp2.x = vpc.x + rho;
                    vp2.y = vpc.y - rho;
                    if (is_graph_xinvert(gno) == TRUE) {
                        fswap(&phi_start, &phi_stop);
                    }
                    if (phi_stop < phi_start) {
                        phi_stop += 2*M_PI;
                    } 
                    DrawArc(vp1, vp2, (int) rint(180.0/M_PI*phi_start),
                                      (int) rint(180.0/M_PI*phi_stop));
                } else {
	    	    DrawLine(vp1_start, vp1_stop);
                }
	    }
	    if (t->t_op == PLACEMENT_OPPOSITE || t->t_op == PLACEMENT_BOTH) {
                if (is_xaxis(caxis) && get_graph_type(gno) == GRAPH_POLAR) {
                    xy2polar(vp2_start.x - vpc.x, vp2_start.y - vpc.y,
                             &phi_start, &rho);
                    xy2polar(vp2_stop.x - vpc.x, vp2_stop.y - vpc.y,
                             &phi_stop, &rho);
                    vp1.x = vpc.x - rho;
                    vp1.y = vpc.y + rho;
                    vp2.x = vpc.x + rho;
                    vp2.y = vpc.y - rho;
                    if (is_graph_xinvert(gno) == TRUE) {
                        fswap(&phi_start, &phi_stop);
                    }
                    if (phi_stop < phi_start) {
                        phi_stop += 2*M_PI;
                    } 
                    DrawArc(vp1, vp2, (int) rint(180.0/M_PI*phi_start),
                                      (int) rint(180.0/M_PI*phi_stop));
                } else {
	    	    DrawLine(vp2_start, vp2_stop);
                }
	    }
	}
	/* End axis bar stuff*/

        
        /* TODO ticks, labels and axis labels for polar plots */
        if (get_graph_type(gno) == GRAPH_POLAR) {
            continue;
        }

        activate_bbox(BBOX_TYPE_TEMP, TRUE);
        reset_bbox(BBOX_TYPE_TEMP);

	/* Begin axis tick stuff */
	if (t->t_flag) {
	    for (ittype_loop = 0; ittype_loop < 2; ittype_loop++) {
 
	        if (ittype_loop == 0) { /* minor ticks */
                    ttype = TICK_TYPE_MINOR;
                    tprops = t->mprops;
	        } else {      /* major ticks */
                    ttype = TICK_TYPE_MAJOR;
                    tprops = t->props;
	        }
	        tsize = 0.02 * tprops.size;
 
	        switch (t->t_inout) {
	        case TICKS_IN:
	            vbase1_start = vbase1;
	            vbase1_stop  = vbase1 + tick_dir_sign*tsize;
	            vbase2_start = vbase2;
	            vbase2_stop  = vbase2 - tick_dir_sign*tsize;
	            break;
	        case TICKS_OUT:
	            vbase1_start = vbase1;
	            vbase1_stop  = vbase1 - tick_dir_sign*tsize;
	            vbase2_start = vbase2;
	            vbase2_stop  = vbase2 + tick_dir_sign*tsize;
	            break;
	        case TICKS_BOTH:
	            vbase1_start = vbase1 - tsize;
	            vbase1_stop  = vbase1 + tsize;
	            vbase2_start = vbase2 + tsize;
	            vbase2_stop  = vbase2 - tsize;
	            break;
	        default:
	            errmsg("Internal error in drawaxes()");
	            return;
	        }

                setcolor(tprops.color);
                setlinewidth(tprops.linew);
	        setlinestyle(tprops.lines);
	
	        itcur = 0;
                for (itick = 0; itick < t->nticks; itick++) {
	            if (t->tloc[itick].type != ttype) {
	                continue;
	            }
 
                    wtpos = t->tloc[itick].wtpos;
	
	            if ((wtpos < wc_start) || (wtpos > wc_stop)) {
	                continue;
	            }
	
	            vtpos = coord_conv(wtpos);
	            if (t->t_op == PLACEMENT_NORMAL ||
	                t->t_op == PLACEMENT_BOTH) {
	                vp_tick1_start.x = vtpos*ort_para.x + vbase1_start*ort_perp.x;
	                vp_tick1_start.y = vtpos*ort_para.y + vbase1_start*ort_perp.y;
	                vp_tick1_stop.x  = vtpos*ort_para.x + vbase1_stop*ort_perp.x;
	                vp_tick1_stop.y  = vtpos*ort_para.y + vbase1_stop*ort_perp.y;
	                DrawLine(vp_tick1_start, vp_tick1_stop);
	            }
	            if (t->t_op == PLACEMENT_OPPOSITE ||
	                t->t_op == PLACEMENT_BOTH) {
	                vp_tick2_start.x = vtpos*ort_para.x + vbase2_start*ort_perp.x;
	                vp_tick2_start.y = vtpos*ort_para.y + vbase2_start*ort_perp.y;
	                vp_tick2_stop.x  = vtpos*ort_para.x + vbase2_stop*ort_perp.x;
	                vp_tick2_stop.y  = vtpos*ort_para.y + vbase2_stop*ort_perp.y;
	                DrawLine(vp_tick2_start, vp_tick2_stop);
	            }
                    itcur++;
	        }
	    }
	}
	/* End axis ticks stuff */
        
        /* Make sure we don't end up with an empty BBox if no ticks have
           been drawn */
        vp1.x = v.xv1;
        vp1.y = v.yv1;
        vp2.x = v.xv2;
        vp2.y = v.yv2;
        update_bbox(BBOX_TYPE_TEMP, vp1);
        update_bbox(BBOX_TYPE_TEMP, vp2);


	/* Begin tick label stuff */

	if(t->tl_gaptype==TYPE_AUTO) {
	  /* hard coded offsets for autoplacement of tick labels */
	  tl_trans=0.0;     /* parallel */
	  tl_offset=0.01;  /* perpendicular */
	} else{
	  tl_trans  = t->tl_gap.x;
	  tl_offset = t->tl_gap.y;
	}

	if (t->tl_flag) {
	    if (t->tl_starttype == TYPE_SPEC) {
	        wc_start_labels = t->tl_start;
            } else {
	        wc_start_labels = wc_start;
	    }
	
	    if (t->tl_stoptype == TYPE_SPEC) {
	        wc_stop_labels = t->tl_stop;
            } else {
	        wc_stop_labels = wc_stop;
	    }
	
	    tlsize = 0.02 * t->tl_charsize;

	    tsize = 0.02 * t->props.size;
 
	    switch (t->t_inout) {
	    case TICKS_IN:
	        vbase_tlabel1 = vbase1 - (1 - tick_dir_sign)/2*tsize - tl_offset;
	        vbase_tlabel2 = vbase2 + (1 - tick_dir_sign)/2*tsize + tl_offset;
	        break;
	    case TICKS_OUT:
	        vbase_tlabel1 = vbase1 - (1 + tick_dir_sign)/2*tsize - tl_offset;
	        vbase_tlabel2 = vbase2 + (1 + tick_dir_sign)/2*tsize + tl_offset;
	        break;
	    case TICKS_BOTH:
	        vbase_tlabel1 = vbase1 - tsize - tl_offset;
	        vbase_tlabel2 = vbase2 + tsize + tl_offset;
	        break;
	    default:
	        errmsg("Internal error in drawaxes()");
	        return;
	    }

	    setfont(t->tl_font);
	    setcharsize(t->tl_charsize);
	
	    itcur = 0;
            for (itick = 0; itick < t->nticks; itick++) {
	        if (t->tloc[itick].type != TICK_TYPE_MAJOR) {
	            continue;
	        }
 
                wtpos = t->tloc[itick].wtpos;

	        if ((wtpos < wc_start_labels) || (wtpos > wc_stop_labels)) {
	            continue;
	        }
	
	        if (t->tl_prestr[0]) {
	            strcpy(tlabel, t->tl_prestr);
	        } else {
                    tlabel[0] = '\0';
                }
	        if (t->tloc[itick].label != NULL) {
	            strcat(tlabel, t->tloc[itick].label);
	        }
	        if (t->tl_appstr[0]) {
	            strcat(tlabel, t->tl_appstr);
	        }
	
	        vtpos = coord_conv(wtpos);

	        if (itcur % (t->tl_skip + 1) == 0) {
                    /* Set color before each tick label, since pre/app
                       strings may change it */
	            setcolor(t->tl_color);
		    
                    /* Tick labels on normal side */
	            if (t->tl_op == PLACEMENT_NORMAL ||
	                t->tl_op == PLACEMENT_BOTH) {
	                vbase_tlabel = vbase_tlabel1 - (tl_offset + tlsize)*
	                                        (itcur % (t->tl_staggered + 1));
	                vp_tlabel.x = (vtpos + tl_trans)*ort_para.x +
                                                       vbase_tlabel*ort_perp.x;
	                vp_tlabel.y = (vtpos + tl_trans)*ort_para.y +
                                                       vbase_tlabel*ort_perp.y;
	                WriteString(vp_tlabel, t->tl_angle, tlabel1_just, tlabel);
	            }
		    /* Tick labels on opposite side */
	            if (t->tl_op == PLACEMENT_OPPOSITE ||
	                t->tl_op == PLACEMENT_BOTH) {
                        vbase_tlabel = vbase_tlabel2 + (tl_offset + tlsize)*
	                                        (itcur % (t->tl_staggered + 1));
	                vp_tlabel.x = (vtpos + tl_trans)*ort_para.x +
                                                       vbase_tlabel*ort_perp.x;
	                vp_tlabel.y = (vtpos + tl_trans)*ort_para.y +
                                                       vbase_tlabel*ort_perp.y;
	                WriteString(vp_tlabel, t->tl_angle, tlabel2_just, tlabel);
	            }
	        }
                itcur++;
	    }
	}

        /* End tick label stuff */

        bb = get_bbox(BBOX_TYPE_TEMP);

	/* Begin axis label stuff */
	
	if (t->label_place == TYPE_SPEC) {
	    vp_label_offset1.x = t->label.x;
	    vp_label_offset1.y = t->label.y;
	    vp_label_offset2.x = t->label.x;
	    vp_label_offset2.y = t->label.y;
            
            /* These settings are for backward compatibility */
            label1_just = JUST_CENTER|JUST_MIDDLE;
            label2_just = JUST_CENTER|JUST_MIDDLE;
	} else {
            /* parallel is trivial ;-) */
	    vp_label_offset1.x = 0.00;
	    vp_label_offset2.x = 0.00;
	    
            /* perpendicular */
            if (is_xaxis(caxis)) {
                vp_label_offset1.y = vbase1 - bb.yv1;
                vp_label_offset2.y = bb.yv2 - vbase2;
            } else {
                vp_label_offset1.y = vbase1 - bb.xv1;
                vp_label_offset2.y = bb.xv2 - vbase2;
            }
            
            vp_label_offset1.y += tl_offset;
            vp_label_offset2.y += tl_offset;

            label1_just = tlabel1_just;
            label2_just = tlabel2_just;
	}

	if (t->label.s && t->label.s[0]) {
	    
	    setcharsize(t->label.charsize);
	    setfont(t->label.font);
	    setcolor(t->label.color);

	    /* Axis label on normal side */
	    if (t->label_op == PLACEMENT_NORMAL ||
		t->label_op == PLACEMENT_BOTH) {

	        vp_label.x = (vp1_start.x + vp1_stop.x)/2
                    + vp_label_offset1.x*ort_para.x
                    - vp_label_offset1.y*ort_perp.x;
	        vp_label.y = (vp1_start.y + vp1_stop.y)/2
                    + vp_label_offset1.x*ort_para.y
                    - vp_label_offset1.y*ort_perp.y;

	        WriteString(vp_label, langle, label1_just, t->label.s);
	    }

	    /* Axis label on opposite side */
	    if (t->label_op == PLACEMENT_OPPOSITE ||
		t->label_op == PLACEMENT_BOTH) {

	        vp_label.x = (vp2_start.x + vp2_stop.x)/2
                    + vp_label_offset2.x*ort_para.x
		    + vp_label_offset2.y*ort_perp.x ;
	        vp_label.y = (vp2_start.y + vp2_stop.y)/2
                    + vp_label_offset2.x*ort_para.y
                    + vp_label_offset2.y*ort_perp.y ;

	        WriteString(vp_label, langle, label2_just, t->label.s);
	    }
	}
        
	/* End axis label stuff */
    }
}

void calculate_tickgrid(int gno)
{
    int caxis;
    int itick, imtick, itmaj;
    int nmajor;
    double swc_start, swc_stop, stmajor;
    int scale;
    double wtmaj;
    world w;
    tickmarks *t;
    int res, len;
    grarr *tvar;
    double *tt;
    
reenter:
    get_graph_world(gno, &w);
    
    for (caxis = 0; caxis < MAXAXES; caxis++) {
	t = get_graph_tickmarks(gno, caxis);

	if (!t || t->active != TRUE) {
            continue;
        }

	if (t->t_spec == TICKS_SPEC_NONE) {
            if (is_xaxis(caxis)) {
                scale = get_graph_xscale(gno);
                if (scale == SCALE_LOG) {
                    swc_start = fscale(w.xg1, scale);
                    swc_stop  = fscale(w.xg2, scale);
                } else {
                    swc_start = w.xg1;
                    swc_stop  = w.xg2;
                }
            } else {
                scale = get_graph_yscale(gno);
                if (scale == SCALE_LOG) {
                    swc_start = fscale(w.yg1, scale);
                    swc_stop  = fscale(w.yg2, scale);
                } else {
                    swc_start = w.yg1;
                    swc_stop  = w.yg2;
                }
            }
            if (scale == SCALE_LOG) {
                stmajor = fscale(t->tmajor, scale);
            } else {
                stmajor = t->tmajor;
            }

            if (stmajor <= 0.0) {
	        errmsg("Invalid major tick spacing, autoticking");
	        autotick_axis(gno, caxis);
                goto reenter;
	    }

            if (t->t_round == TRUE) {
                swc_start = floor(swc_start/stmajor)*stmajor;
            }

            nmajor = (int) ceil((swc_stop - swc_start) / stmajor + 1);
            t->nticks = (nmajor - 1)*(t->nminor + 1) + 1;

            if (t->nticks > MAX_TICKS) {
	        errmsg("Too many ticks ( > MAX_TICKS ), autoticking");
	        autotick_axis(gno, caxis);
                goto reenter;
	    }

    /*
     *         if (t->nticks > MAX_TICKS) {
     *             t->nticks = MAX_TICKS;
     *         }
     */

            itick = 0;
            itmaj = 0;
            while (itick < t->nticks) {
                if (scale == SCALE_LOG) {
                    wtmaj = ifscale(swc_start + itmaj*stmajor, scale);
                } else {
                    wtmaj = swc_start + itmaj*stmajor;
                    if (t->t_round == TRUE && fabs(wtmaj) < 1.0e-6*stmajor) {
                        wtmaj = 0.0;
                    }
                }
                t->tloc[itick].wtpos = wtmaj;
                t->tloc[itick].type = TICK_TYPE_MAJOR;

                itick++;
                for (imtick = 0; imtick < t->nminor && itick < t->nticks; imtick++) {
                    if (scale == SCALE_LOG) {
                        t->tloc[itick].wtpos = wtmaj * (imtick + 2);
                    } else {
                        t->tloc[itick].wtpos = wtmaj + (imtick + 1)*stmajor/(t->nminor + 1);
                    }
                    t->tloc[itick].type = TICK_TYPE_MINOR;
	            XCFREE(t->tloc[itick].label);
                    itick++;
                }
                itmaj++;
            }
        }

        if (t->t_spec != TICKS_SPEC_BOTH) {
	    nmajor = 0;
            for (itick = 0; itick < t->nticks; itick++) {
                if (t->tloc[itick].type == TICK_TYPE_MAJOR) {
                    nmajor++;
                }
            }
	    if (t->tl_formula && t->tl_formula[0] != '\0') {

                tvar = get_parser_arr_by_name("$t");
                if (tvar == NULL) {
                    tvar = define_parser_arr("$t");
                    if (tvar == NULL) {
	                errmsg("Internal error");
                        return;
                    }
                }

                if (tvar->length != 0) {
                    xfree(tvar->data);
                    tvar->length = 0;
                }
                tvar->data = xmalloc(nmajor*SIZEOF_DOUBLE);
                if (tvar->data == NULL) {
                    return;
                }
                tvar->length = nmajor;
                
                itmaj = 0;
                for (itick = 0; itick < t->nticks; itick++) {
                    if (t->tloc[itick].type == TICK_TYPE_MAJOR) {
                        tvar->data[itmaj] = t->tloc[itick].wtpos;
                        itmaj++;
                    }
                }

                res = v_scanner(t->tl_formula, &len, &tt);
                XCFREE(tvar->data);
                tvar->length = 0;
                if (res != RETURN_SUCCESS || len != nmajor) {
                    errmsg("Error in tick transformation formula");
                    return;
                }

                itmaj = 0;
                for (itick = 0; itick < t->nticks; itick++) {
                    if (t->tloc[itick].type == TICK_TYPE_MAJOR) {
	                t->tloc[itick].label = copy_string(t->tloc[itick].label, 
                            create_fstring(t->tl_format, t->tl_prec,
                                tt[itmaj], LFORMAT_TYPE_EXTENDED));
                        itmaj++;
                    }
                }
                xfree(tt);
	    } else {
                for (itick = 0; itick < t->nticks; itick++) {
                    if (t->tloc[itick].type == TICK_TYPE_MAJOR) {
	                t->tloc[itick].label = copy_string(t->tloc[itick].label, 
                            create_fstring(t->tl_format, t->tl_prec,
                                t->tloc[itick].wtpos, LFORMAT_TYPE_EXTENDED));
                    }
                }
            }
	}
    }
}
