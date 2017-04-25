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
 * ticks / tick labels / axis labels
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>

#include "globals.h"
#include "protos.h"
#include "utils.h"
#include "graphs.h"
#include "graphutils.h"
#include "motifinc.h"

#define cg get_cg()

static int curaxis;

static Widget axes_dialog = NULL;

static Widget axes_tab;

static OptionStructure *editaxis; /* which axis to edit */
static Widget axis_active;      /* active or not */
static Widget axis_zero;        /* "zero" or "plain" */
static OptionStructure *axis_scale; /* axis scale */
static Widget axis_invert;      /* invert axis */
static Widget *axis_applyto;    /* override */
static Widget offx;             /* x offset of axis in viewport coords */
static Widget offy;             /* y offset of axis in viewport coords */
static Widget tonoff;           /* toggle display of axis ticks */
static Widget tlonoff;          /* toggle display of tick labels */
static TextStructure *axislabel;        /* axis label */
static Widget *axislabellayout; /* axis label layout (perp or parallel) */
static OptionStructure *axislabelplace;  /* axis label placement, auto or specified */
static Widget axislabelspec_rc;
static Widget axislabelspec_para;    /* location of axis label if specified (viewport coords) */
static Widget axislabelspec_perp;    /* location of axis label if specified (viewport coords) */
static OptionStructure *axislabelfont;   /* axis label font */
static Widget axislabelcharsize;/* axis label charsize */
static OptionStructure *axislabelcolor;  /* axis label color */
static Widget *axislabelop;     /* tick labels normal|opposite|both sides */
static Widget tmajor;           /* major tick spacing */
static SpinStructure *nminor;   /* # of minor ticks */
static Widget *tickop;          /* ticks normal|opposite|both sides */
static Widget *ticklop;         /* tick labels normal|opposite|both sides */
static OptionStructure *tlform; /* format for labels */
static Widget *tlprec;          /* precision for labels */
static OptionStructure *tlfont;  /* tick label font */
static Widget tlcharsize;       /* tick label charsize */
static OptionStructure *tlcolor; /* tick label color */
static Widget tlappstr;         /* tick label append string */
static Widget tlprestr;         /* tick label prepend string */
static Widget *tlskip;          /* tick marks to skip */
static Widget *tlstarttype;     /* use graph min or starting value */
static Widget tlstart;          /* value to start tick labels */
static Widget *tlstoptype;      /* use graph max or stop value */
static Widget tlstop;           /* value to stop tick labels */
static OptionStructure *tlgaptype; /* tick label placement, auto or specified */
static Widget tlgap_rc;
static Widget tlgap_para;       /* location of tick label if specified (viewport coords) */
static Widget tlgap_perp;       /* location of tick label if specified (viewport coords) */
static Widget tlangle;          /* angle */
static Widget *tlstagger;       /* stagger */
static TextStructure *tlformula; /* transformation if tick labels */
static Widget *autonum;         /* number of autotick divisions */
static Widget tround;           /* place at rounded positions */
static Widget tgrid;            /* major ticks grid */
static OptionStructure *tgridcol;
static SpinStructure *tgridlinew;
static OptionStructure *tgridlines;
static Widget tmgrid;           /* minor ticks grid */
static OptionStructure *tmgridcol;
static SpinStructure *tmgridlinew;
static OptionStructure *tmgridlines;
static Widget tlen;             /* tick length */
static Widget tmlen;
static Widget *tinout;          /* ticks in out or both */
static Widget baronoff;         /* axis bar */
static OptionStructure *barcolor;
static SpinStructure *barlinew;
static OptionStructure *barlines;

static OptionStructure *specticks;      /* special ticks/labels */
static SpinStructure *nspec;
static Widget specloc[MAX_TICKS];
static Widget speclabel[MAX_TICKS];
static Widget axis_world_start;
static Widget axis_world_stop;

static void set_axis_proc(int value, void *data);
static void set_active_proc(int onoff, void *data);
static int axes_aac_cb(void *data);
static void axis_scale_cb(int value, void *data);
static void auto_spec_cb(int value, void *data);

void create_axes_dialog_cb(void *data)
{
    create_axes_dialog(-1);
}

/*
 * Create the ticks popup
 */
void create_axes_dialog(int axisno)
{
    set_wait_cursor();
    
    if (axisno >= 0 && axisno < MAXAXES) {
        curaxis = axisno;
    }
    
    if (axes_dialog == NULL) {
        int i;
        char buf[32];
        OptionItem opitems[MAXAXES];
        Widget rc_head, rc, rc2, rc3, fr, sw, axes_main, axes_label,
            axes_ticklabel, axes_tickmark, axes_special, vbar;

        axes_dialog = CreateDialogForm(app_shell, "Axes");

        rc_head = CreateVContainer(axes_dialog);
        AddDialogFormChild(axes_dialog, rc_head);
        
        rc = CreateHContainer(rc_head);
        opitems[0].value = X_AXIS;
        opitems[0].label = "X axis";
        opitems[1].value = Y_AXIS;
        opitems[1].label = "Y axis";
        opitems[2].value = ZX_AXIS;
        opitems[2].label = "Alt X axis";
        opitems[3].value = ZY_AXIS;
        opitems[3].label = "Alt Y axis";
        editaxis = CreateOptionChoice(rc, "Edit:", 0, MAXAXES, opitems);
        AddOptionChoiceCB(editaxis, set_axis_proc, NULL);
        axis_active = CreateToggleButton(rc, "Active");
        AddToggleButtonCB(axis_active, set_active_proc, NULL);
        
        rc = CreateHContainer(rc_head);
        axis_world_start = CreateTextItem2(rc, 10, "Start:");
	axis_world_stop = CreateTextItem2(rc, 10, "Stop:");

        rc = CreateHContainer(rc_head);

        opitems[0].value = SCALE_NORMAL;
        opitems[0].label = "Linear";
        opitems[1].value = SCALE_LOG;
        opitems[1].label = "Logarithmic";
        opitems[2].value = SCALE_REC;
        opitems[2].label = "Reciprocal";
	opitems[3].value = SCALE_LOGIT;
	opitems[3].label = "Logit";
        axis_scale = CreateOptionChoice(rc, "Scale:", 0, 4, opitems);
        AddOptionChoiceCB(axis_scale, axis_scale_cb, NULL);

	axis_invert = CreateToggleButton(rc, "Invert axis");
        

        /* ------------ Tabs --------------*/

        
        axes_tab = CreateTab(axes_dialog); 
        AddDialogFormChild(axes_dialog, axes_tab);       

        axes_main = CreateTabPage(axes_tab, "Main");

        fr = CreateFrame(axes_main, "Axis label");
        
        axislabel = CreateCSText(fr, "Label string:");
        
        fr = CreateFrame(axes_main, "Tick properties");
        rc = CreateVContainer(fr);

        rc2 = CreateHContainer(rc);
        tmajor = CreateTextItem2(rc2, 8, "Major spacing:");
        nminor = CreateSpinChoice(rc2, "Minor ticks:",
            2, SPIN_TYPE_INT, 0.0, (double) MAX_TICKS - 1, 1.0);

        rc2 = CreateHContainer(rc);
        tlform = CreateFormatChoice(rc2, "Format:");
        tlprec = CreatePrecisionChoice(rc2, "Precision:");

        fr = CreateFrame(axes_main, "Display options");
        rc = CreateHContainer(fr);
        
        rc2 = CreateVContainer(rc);
        tlonoff = CreateToggleButton(rc2, "Display tick labels");
        tonoff = CreateToggleButton(rc2, "Display tick marks");
        
        rc2 = CreateVContainer(rc);
        baronoff = CreateToggleButton(rc2, "Display axis bar");

        fr = CreateFrame(axes_main, "Axis placement");
        rc = CreateHContainer(fr);
	axis_zero = CreateToggleButton(rc, "Zero axis");
        offx = CreateTextItem2(rc, 6, "Offsets - Normal:");
        offy = CreateTextItem2(rc, 6, "Opposite:");

        fr = CreateFrame(axes_main, "Tick label properties");
        rc = CreateHContainer(fr);

        tlfont = CreateFontChoice(rc, "Font:");
        tlcolor = CreateColorChoice(rc, "Color:");


        axes_label = CreateTabPage(axes_tab, "Axis label & bar");

        fr = CreateFrame(axes_label, "Label properties");
        rc = CreateVContainer(fr);

        rc2 = CreateHContainer(rc);
        axislabelfont = CreateFontChoice(rc2, "Font:");
        axislabelcolor = CreateColorChoice(rc2, "Color:");
        
        rc2 = CreateHContainer(rc);
        axislabelcharsize = CreateCharSizeChoice(rc2, "Char size");
	SetScaleWidth(axislabelcharsize, 160);
        
        axislabellayout = CreatePanelChoice(rc2, "Layout:",
                                            3,
                                            "Parallel to axis",
                                            "Perpendicular to axis",
                                            NULL);

        rc2 = CreateHContainer(rc);
        axislabelop = CreatePanelChoice(rc2, "Side:",
                                             4,
                                             "Normal",
                                             "Opposite",
                                             "Both",
                                             NULL);
        opitems[0].value = TYPE_AUTO;
        opitems[0].label = "Auto";
        opitems[1].value = TYPE_SPEC;
        opitems[1].label = "Specified";
        axislabelplace = CreateOptionChoice(rc2, "Location:", 0, 2, opitems);
        axislabelspec_rc = CreateHContainer(rc);
        axislabelspec_para = CreateTextItem2(axislabelspec_rc, 7, "Parallel offset:");
        axislabelspec_perp = CreateTextItem2(axislabelspec_rc, 7, "Perpendicular offset:");
        AddOptionChoiceCB(axislabelplace, auto_spec_cb, axislabelspec_rc);


        fr = CreateFrame(axes_label, "Bar properties");
        rc = CreateVContainer(fr);

        rc2 = CreateHContainer(rc);
        barcolor = CreateColorChoice(rc2, "Color:");
        barlinew = CreateLineWidthChoice(rc2, "Width:");
        
        barlines = CreateLineStyleChoice(rc, "Line style:");


        axes_ticklabel = CreateTabPage(axes_tab, "Tick labels");

        fr = CreateFrame(axes_ticklabel, "Labels");
        rc2 = CreateHContainer(fr);
        tlcharsize = CreateCharSizeChoice(rc2, "Char size");
	SetScaleWidth(tlcharsize, 200);
        tlangle = CreateAngleChoice(rc2, "Angle");
	SetScaleWidth(tlangle, 180);

        fr = CreateFrame(axes_ticklabel, "Placement");
        rc = CreateHContainer(fr);

        rc2 = CreateVContainer(rc);
        ticklop = CreatePanelChoice(rc2, "Side:",
                                    4,
                                    "Normal",
                                    "Opposite",
                                    "Both",
                                    NULL);
        tlstagger = CreatePanelChoice(rc2, "Stagger:",
                                      11,
                        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                                      NULL);
        
        
        rc2 = CreateVContainer(rc);
        rc3 = CreateHContainer(rc2);
        tlstarttype = CreatePanelChoice(rc3, "Start at:",
                                        3,
                                        "Axis min", "Specified:",
                                        NULL);
        tlstart = CreateTextItem2(rc3, 8, "");

        rc3 = CreateHContainer(rc2);
        tlstoptype = CreatePanelChoice(rc3, "Stop at:",
                                       3,
                                       "Axis max", "Specified:",
                                       NULL);
        tlstop = CreateTextItem2(rc3, 8, "");

        fr = CreateFrame(axes_ticklabel, "Extra");
        rc = CreateVContainer(fr);

        rc2 = CreateHContainer(rc);
        tlskip = CreatePanelChoice(rc2, "Skip every:",
                                   11,
                        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                                   NULL);

        tlformula = CreateTextInput(rc2, "Axis transform:");

        rc2 = CreateHContainer(rc);
        tlprestr = CreateTextItem2(rc2, 13, "Prepend:");
        tlappstr = CreateTextItem2(rc2, 13, "Append:");

        opitems[0].value = TYPE_AUTO;
        opitems[0].label = "Auto";
        opitems[1].value = TYPE_SPEC;
        opitems[1].label = "Specified";
        tlgaptype = CreateOptionChoice(rc, "Location:", 0, 2, opitems);
        tlgap_rc = CreateHContainer(rc);
        tlgap_para = CreateTextItem2(tlgap_rc, 7, "Parallel offset:");
        tlgap_perp = CreateTextItem2(tlgap_rc, 7, "Perpendicular offset:");
        AddOptionChoiceCB(tlgaptype, auto_spec_cb, tlgap_rc);


        axes_tickmark = CreateTabPage(axes_tab, "Tick marks");

        fr = CreateFrame(axes_tickmark, "Placement");
        rc2 = CreateVContainer(fr);
        rc = CreateHContainer(rc2);
        tinout = CreatePanelChoice(rc, "Pointing:",
                                   4,
                                   "In", "Out", "Both",
                                   NULL);
        tickop = CreatePanelChoice(rc, "Draw on:",
                                   4,
                                   "Normal side",
                                   "Opposite side",
                                   "Both sides",
                                   NULL);
        rc = CreateHContainer(rc2);
        tround = CreateToggleButton(rc, "Place at rounded positions");
	autonum = CreatePanelChoice(rc, "Autotick divisions",
                                    12,
		                    "2",
                                    "3",
                                    "4",
                                    "5",
                                    "6",
                                    "7",
                                    "8",
                                    "9",
                                    "10",
                                    "11",
                                    "12",
				    NULL);
        
        rc2 = CreateHContainer(axes_tickmark);

/* major tick marks */
        fr = CreateFrame(rc2, "Major ticks");
        rc = CreateVContainer(fr);
        tgrid = CreateToggleButton(rc, "Draw grid lines");
        tlen = CreateCharSizeChoice(rc, "Tick length");
        tgridcol = CreateColorChoice(rc, "Color:");
        tgridlinew = CreateLineWidthChoice(rc, "Line width:");
        tgridlines = CreateLineStyleChoice(rc, "Line style:");

        fr = CreateFrame(rc2, "Minor ticks");
        rc = CreateVContainer(fr);
        tmgrid = CreateToggleButton(rc, "Draw grid lines");
        tmlen = CreateCharSizeChoice(rc, "Tick length");
        tmgridcol = CreateColorChoice(rc, "Color:");
        tmgridlinew = CreateLineWidthChoice(rc, "Line width:");
        tmgridlines = CreateLineStyleChoice(rc, "Line style:");


        axes_special = CreateTabPage(axes_tab, "Special");

        opitems[0].value = TICKS_SPEC_NONE;
        opitems[0].label = "None";
        opitems[1].value = TICKS_SPEC_MARKS;
        opitems[1].label = "Tick marks";
        opitems[2].value = TICKS_SPEC_BOTH;
        opitems[2].label = "Tick marks and labels";
        specticks = CreateOptionChoice(axes_special, "Special ticks:", 0, 3, opitems);

        nspec = CreateSpinChoice(axes_special, "Number of user ticks to use:",
            3, SPIN_TYPE_INT, 0.0, (double) MAX_TICKS, 1.0);
        CreateLabel(axes_special, "Tick location - Label:");

        sw = XtVaCreateManagedWidget("sw",
                                     xmScrolledWindowWidgetClass, axes_special,
				     XmNheight, 240,
                                     XmNscrollingPolicy, XmAUTOMATIC,
                                     NULL);
        rc = CreateVContainer(sw);

        for (i = 0; i < MAX_TICKS; i++) {
            rc3 = CreateHContainer(rc);
            sprintf(buf, "%2d", i);
            specloc[i]   = CreateTextItem4(rc3, 12, buf);
            speclabel[i] = CreateTextItem4(rc3, 30, "");
        }


        SelectTabPage(axes_tab, axes_main);

         
        rc = CreateVContainer(axes_dialog);
        axis_applyto = CreatePanelChoice(rc,
                                         "Apply to:",
                                         5,
                                         "Current axis",
                                         "All axes, current graph",
                                         "Current axis, all graphs",
                                         "All axes, all graphs",
                                         NULL);

        CreateAACDialog(axes_dialog, rc, axes_aac_cb, NULL);
        
        /* set reasonable scrolling */
        vbar = XtNameToWidget(sw, "VertScrollBar");
        if (vbar) {
            int maxval;
            XtVaGetValues(vbar, XmNmaximum, &maxval, NULL);
            XtVaSetValues(vbar, XmNincrement, (int) rint(maxval/MAX_TICKS), NULL);
        }
    }
    update_ticks(cg);
    
    RaiseWindow(GetParent(axes_dialog));
    unset_wait_cursor();
}

/*
 * Callback function for definition of tick marks and axis labels.
 */
static int axes_aac_cb(void *data)
{
    int i, j;
    int applyto;
    int axis_start, axis_stop, graph_start, graph_stop;
    int scale, invert;
    tickmarks *t;
    double axestart, axestop;
    char *cp;
    world w;
    
    applyto = GetChoice(axis_applyto);
    
    t = new_graph_tickmarks();
    if (!t) {
        return RETURN_FAILURE;
    }

    t->active = GetToggleButtonState(axis_active);
    
    t->zero = GetToggleButtonState(axis_zero);

    if (xv_evalexpr(tmajor, &t->tmajor) != RETURN_SUCCESS) {
	errmsg( "Specify major tick spacing" );
        free_graph_tickmarks(t);
        return RETURN_FAILURE;
    }
    t->nminor = (int) GetSpinChoice(nminor);

    t->tl_flag = GetToggleButtonState(tlonoff);
    t->t_flag = GetToggleButtonState(tonoff);
    t->t_drawbar = GetToggleButtonState(baronoff);
    set_plotstr_string(&t->label, GetTextString(axislabel));

    xv_evalexpr(offx, &t->offsx);
    xv_evalexpr(offy, &t->offsy);

    t->label_layout = GetChoice(axislabellayout) ? LAYOUT_PERPENDICULAR : LAYOUT_PARALLEL;
    t->label_place = GetOptionChoice(axislabelplace);
    if (t->label_place == TYPE_SPEC) {
        xv_evalexpr(axislabelspec_para, &t->label.x);
        xv_evalexpr(axislabelspec_perp, &t->label.y);
    }
    t->label.font = GetOptionChoice(axislabelfont);
    t->label.color = GetOptionChoice(axislabelcolor);
    t->label.charsize = GetCharSizeChoice(axislabelcharsize);

    /* somehow the value of axislabelop gets automagically correctly
       applied to all selected axes without checking for the value of
       applyto directly here (strange...) */
    t->label_op = GetChoice(axislabelop);

    t->tl_font = GetOptionChoice(tlfont);
    t->tl_color = GetOptionChoice(tlcolor);
    t->tl_skip = GetChoice(tlskip);
    t->tl_prec = GetChoice(tlprec);
    t->tl_staggered = (int) GetChoice(tlstagger);
    strcpy(t->tl_appstr, xv_getstr(tlappstr));
    strcpy(t->tl_prestr, xv_getstr(tlprestr));
    t->tl_starttype = (int) GetChoice(tlstarttype) == 0 ? TYPE_AUTO : TYPE_SPEC;
    if (t->tl_starttype == TYPE_SPEC) {
        if(xv_evalexpr(tlstart, &t->tl_start) != RETURN_SUCCESS) {
	    errmsg( "Specify tick label start" );
            free_graph_tickmarks(t);
            return RETURN_FAILURE;
	}
    }
    t->tl_stoptype = (int) GetChoice(tlstoptype) == 0 ? TYPE_AUTO : TYPE_SPEC;
    if (t->tl_stoptype == TYPE_SPEC) {
        if(xv_evalexpr(tlstop, &t->tl_stop) != RETURN_SUCCESS){
	    errmsg( "Specify tick label stop" );
            free_graph_tickmarks(t);
            return RETURN_FAILURE;
	}
    }
    t->tl_format = GetOptionChoice(tlform);

    t->tl_formula = copy_string(NULL, GetTextString(tlformula));

    t->tl_gaptype = GetOptionChoice(tlgaptype);
    if (t->tl_gaptype == TYPE_SPEC) {
        xv_evalexpr(tlgap_para, &t->tl_gap.x);
        xv_evalexpr(tlgap_perp, &t->tl_gap.y);
    }
    
    t->tl_angle = GetAngleChoice(tlangle);
    
    t->tl_charsize = GetCharSizeChoice(tlcharsize);

    switch ((int) GetChoice(tinout)) {
    case 0:
        t->t_inout = TICKS_IN;
        break;
    case 1:
        t->t_inout = TICKS_OUT;
        break;
    case 2:
        t->t_inout = TICKS_BOTH;
        break;
    }
    
    t->props.color = GetOptionChoice(tgridcol);
    t->props.linew = GetSpinChoice(tgridlinew);
    t->props.lines = GetOptionChoice(tgridlines);
    t->mprops.color = GetOptionChoice(tmgridcol);
    t->mprops.linew = GetSpinChoice(tmgridlinew);
    t->mprops.lines = GetOptionChoice(tmgridlines);
    
    t->props.size = GetCharSizeChoice(tlen);
    t->mprops.size = GetCharSizeChoice(tmlen);

    t->t_autonum = GetChoice(autonum) + 2;

    t->t_round = GetToggleButtonState(tround);
    
    t->props.gridflag = GetToggleButtonState(tgrid);
    t->mprops.gridflag = GetToggleButtonState(tmgrid);

    t->t_drawbarcolor = GetOptionChoice(barcolor);
    t->t_drawbarlinew = GetSpinChoice(barlinew);
    t->t_drawbarlines = GetOptionChoice(barlines);

    t->t_spec = GetOptionChoice(specticks);
    /* only read special info if special ticks used */
    if (t->t_spec != TICKS_SPEC_NONE) {
        t->nticks = (int) GetSpinChoice(nspec);
        /* ensure that enough tick positions have been specified */
        for (i = 0; i < t->nticks; i++) {
            if (xv_evalexpr(specloc[i], &t->tloc[i].wtpos) == RETURN_SUCCESS) {
                cp = xv_getstr(speclabel[i]);
                if (cp[0] == '\0') {
                    t->tloc[i].type = TICK_TYPE_MINOR;
                } else {
                    t->tloc[i].type = TICK_TYPE_MAJOR;
                }
                if (t->t_spec == TICKS_SPEC_BOTH) {
                    t->tloc[i].label = copy_string(t->tloc[i].label, cp);
                } else {
                    t->tloc[i].label = copy_string(t->tloc[i].label, NULL);
                }
            } else {
                errmsg("Not enough tick locations specified");
                free_graph_tickmarks(t);
                return RETURN_FAILURE;
            }
        }
    }
    
    switch (applyto) {
    case 0:                     /* current axis */
        axis_start = curaxis;
        axis_stop  = curaxis;
        graph_start = cg;
        graph_stop  = cg;
        break;
    case 1:                     /* all axes, current graph */
        axis_start = 0;
        axis_stop  = MAXAXES - 1;
        graph_start = cg;
        graph_stop  = cg;
        break;
    case 2:                     /* current axis, all graphs */
        axis_start = curaxis;
        axis_stop  = curaxis;
        graph_start = 0;
        graph_stop  = number_of_graphs() - 1;
        break;
    case 3:                     /* all axes, all graphs */
        axis_start = 0;
        axis_stop  = MAXAXES - 1;
        graph_start = 0;
        graph_stop  = number_of_graphs() - 1;
        break;
    default:
        axis_start = curaxis;
        axis_stop  = curaxis;
        graph_start = cg;
        graph_stop  = cg;
        break;        
    }
        
    if (xv_evalexpr(axis_world_start, &axestart) != RETURN_SUCCESS ||
        xv_evalexpr(axis_world_stop,  &axestop)  != RETURN_SUCCESS) {
        errmsg("Axis start/stop values undefined");
        free_graph_tickmarks(t);
        return RETURN_FAILURE;
    }
		
    for (i = graph_start; i <= graph_stop; i++) {
        for (j = axis_start; j <= axis_stop; j++) {
        
            get_graph_world(i, &w);
            if (is_xaxis(j)) {
               	w.xg1 = axestart;
                w.xg2 = axestop;
            } else {
                w.yg1 = axestart; 
               	w.yg2 = axestop;
            }
            set_graph_world(i, w);
            
            scale = GetOptionChoice(axis_scale);
            if (is_xaxis(j)) {
                set_graph_xscale(i, scale);
            } else {
                set_graph_yscale(i, scale);
            }

            invert = GetToggleButtonState(axis_invert);
            if (is_xaxis(j)) {
                set_graph_xinvert(i, invert);
            } else {
                set_graph_yinvert(i, invert);
            }
            
            t->tl_op = GetChoice(ticklop);

            t->t_op = GetChoice(tickop);

            set_graph_tickmarks(i, j, t);
        }
    }
    
    free_graph_tickmarks(t);
    
    xdrawgraph();

    update_ticks(cg);
    
    return RETURN_SUCCESS;
}

/*
 * This CB services the axis "Scale" selector 
 */
static void axis_scale_cb(int value, void *data)
{
    int scale = value;
    double major_space, axestart, axestop;
    int auton;
    char buf[32];
    
    xv_evalexpr(tmajor, &major_space);
    xv_evalexpr(axis_world_start, &axestart) ;
    xv_evalexpr(axis_world_stop,  &axestop);
    auton = GetChoice(autonum) + 2;
    
    switch (scale) {
    case SCALE_NORMAL:
        if (major_space <= 0.0) {
            sprintf(buf, "%g", (axestop - axestart)/auton);
            xv_setstr(tmajor, buf);
        }
        break;
    case SCALE_LOG:
        if (axestart <= 0.0 && axestop <= 0.0) {
            errmsg("Can't set logarithmic scale for negative coordinates");
            SetOptionChoice(axis_scale, SCALE_NORMAL);
            return;
        } else if (axestart <= 0.0) {
            axestart = axestop/1.0e3;
            sprintf(buf, "%g", axestart);
            xv_setstr(axis_world_start, buf);
        }
        xv_setstr(tmajor, "10");
        SetSpinChoice(nminor, 9);
        break;
     case SCALE_LOGIT:
        if (axestart <= 0.0 && axestop <= 0.0) {
            errmsg("Can't set logit scale for values outside 0 and 1");
            SetOptionChoice(axis_scale, SCALE_NORMAL);
            return;
        } 
	if (axestart <= 0.0) {
            axestart = 0.1;
            sprintf(buf, "%g", axestart);
            xv_setstr(axis_world_start, buf);
        }
	if (axestop >= 1.0) {
	    axestop = 0.95;
	    sprintf(buf, "%g", axestop);
            xv_setstr(axis_world_stop, buf);
	}
        if (major_space >= 1.0) {
            xv_setstr(tmajor, "0.6");
        }
        break;	
    }
}

/*
 * Fill 'Axes' dialog with values
 */

void update_ticks(int gno)
{
    tickmarks *t;
    world w;
    char buf[128];
    int i;

    if (axes_dialog && XtIsManaged(axes_dialog)) {
        t = get_graph_tickmarks(gno, curaxis);
        if (!t) {
            return;
        }

        SetToggleButtonState(axis_active, is_axis_active(gno, curaxis));
        if (is_axis_active(gno, curaxis) == FALSE) {
            SetSensitive(axes_tab, False);
        } else {
            SetSensitive(axes_tab, True);
        }

        SetOptionChoice(editaxis, curaxis);

        SetToggleButtonState(axis_zero, is_zero_axis(gno, curaxis));

        get_graph_world(gno, &w);
        if (is_xaxis(curaxis)) {
            sprintf(buf, "%.9g", w.xg1);
            xv_setstr(axis_world_start, buf);
            sprintf(buf, "%.9g", w.xg2);
            xv_setstr(axis_world_stop, buf);
            SetOptionChoice(axis_scale, get_graph_xscale(gno));
            SetToggleButtonState(axis_invert, is_graph_xinvert(gno));
        } else {
            sprintf(buf, "%.9g", w.yg1);
            xv_setstr(axis_world_start, buf);
            sprintf(buf, "%.9g", w.yg2);
            xv_setstr(axis_world_stop, buf);
            SetOptionChoice(axis_scale, get_graph_yscale(gno));
            SetToggleButtonState(axis_invert, is_graph_yinvert(gno));
        }

        sprintf(buf, "%.4f", t->offsx);
        xv_setstr(offx, buf);
        sprintf(buf, "%.4f", t->offsy);
        xv_setstr(offy, buf);

        SetChoice(axislabellayout, t->label_layout == LAYOUT_PERPENDICULAR ? 1 : 0);
        SetOptionChoice(axislabelplace, t->label_place);
        sprintf(buf, "%.4f", t->label.x);
        xv_setstr(axislabelspec_para, buf);
        sprintf(buf, "%.4f", t->label.y);
        xv_setstr(axislabelspec_perp, buf);
        SetSensitive(axislabelspec_rc, t->label_place == TYPE_SPEC);
        SetOptionChoice(axislabelfont, t->label.font);
        SetOptionChoice(axislabelcolor, t->label.color);
        SetCharSizeChoice(axislabelcharsize, t->label.charsize);
        SetChoice(axislabelop, t->label_op);

        SetToggleButtonState(tlonoff, t->tl_flag);
        SetToggleButtonState(tonoff, t->t_flag);
        SetToggleButtonState(baronoff, t->t_drawbar);
        SetTextString(axislabel, t->label.s);

        if (is_log_axis(gno, curaxis)) {
            if (t->tmajor <= 1.0) {
                t->tmajor = 10.0;
            }
            sprintf(buf, "%g", t->tmajor);	    
        } else if (is_logit_axis(gno, curaxis)) {
	    if (t->tmajor <= 0.0) {
                t->tmajor = 0.1;
            }
	    else if (t->tmajor >= 0.5) {
                t->tmajor = 0.4;
	    }
            sprintf(buf, "%g", t->tmajor);
        } else if (t->tmajor > 0) {
            sprintf(buf, "%g", t->tmajor);
        } else {
            strcpy(buf, "UNDEFINED");
        }
        xv_setstr(tmajor, buf);
 
        SetSpinChoice(nminor, t->nminor);

        SetOptionChoice(tlfont, t->tl_font);
        SetOptionChoice(tlcolor, t->tl_color);
        SetChoice(tlskip, t->tl_skip);
        SetChoice(tlstagger, t->tl_staggered);
        xv_setstr(tlappstr, t->tl_appstr);
        xv_setstr(tlprestr, t->tl_prestr);
        SetChoice(tlstarttype, t->tl_starttype == TYPE_SPEC);
        if (t->tl_starttype == TYPE_SPEC) {
            sprintf(buf, "%f", t->tl_start);
            xv_setstr(tlstart, buf);
            sprintf(buf, "%f", t->tl_stop);
            xv_setstr(tlstop, buf);
        }
        SetChoice(tlstoptype, t->tl_stoptype == TYPE_SPEC);
        if (t->tl_stoptype == TYPE_SPEC) {
            sprintf(buf, "%f", t->tl_stop);
            xv_setstr(tlstop, buf);
        }
        SetOptionChoice(tlform, t->tl_format);
        SetChoice(ticklop, t->tl_op);
        SetTextString(tlformula, t->tl_formula);
        SetChoice(tlprec, t->tl_prec);

        SetOptionChoice(tlgaptype, t->tl_gaptype);
        sprintf(buf, "%.4f", t->tl_gap.x);
        xv_setstr(tlgap_para, buf);
        sprintf(buf, "%.4f", t->tl_gap.y);
        xv_setstr(tlgap_perp, buf);
        SetSensitive(tlgap_rc, t->tl_gaptype == TYPE_SPEC);

        SetCharSizeChoice(tlcharsize, t->tl_charsize);
        SetAngleChoice(tlangle, t->tl_angle);

        switch (t->t_inout) {
        case TICKS_IN:
            SetChoice(tinout, 0);
            break;
        case TICKS_OUT:
            SetChoice(tinout, 1);
            break;
        case TICKS_BOTH:
            SetChoice(tinout, 2);
            break;
        }
        
        SetChoice(tickop, t->t_op);
        
        SetOptionChoice(tgridcol, t->props.color);
        SetSpinChoice(tgridlinew, t->props.linew);
        SetOptionChoice(tgridlines, t->props.lines);
        SetOptionChoice(tmgridcol, t->mprops.color);
        SetSpinChoice(tmgridlinew, t->mprops.linew);
        SetOptionChoice(tmgridlines, t->mprops.lines);
        SetCharSizeChoice(tlen, t->props.size);
        SetCharSizeChoice(tmlen, t->mprops.size);

        SetChoice(autonum, t->t_autonum - 2);

        SetToggleButtonState(tround, t->t_round);
        SetToggleButtonState(tgrid, t->props.gridflag);
        SetToggleButtonState(tmgrid, t->mprops.gridflag);

        SetOptionChoice(barcolor, t->t_drawbarcolor);
        SetSpinChoice(barlinew, t->t_drawbarlinew);
        SetOptionChoice(barlines, t->t_drawbarlines);

        SetOptionChoice(specticks, t->t_spec);
        SetSpinChoice(nspec, t->nticks);
        for (i = 0; i < t->nticks; i++) {
            sprintf(buf, "%.9g", t->tloc[i].wtpos);
            xv_setstr(specloc[i], buf);
            if (t->tloc[i].type == TICK_TYPE_MAJOR) {
                xv_setstr(speclabel[i], t->tloc[i].label);
            } else {
                xv_setstr(speclabel[i], "");
            }
        }
    }
}



static void set_active_proc(int onoff, void *data)
{
    SetSensitive(axes_tab, onoff);
}

static void set_axis_proc(int value, void *data)
{
    curaxis = value;
    update_ticks(cg);
}

static void auto_spec_cb(int value, void *data)
{
    Widget rc = (Widget) data;
    SetSensitive(rc, value);
}
