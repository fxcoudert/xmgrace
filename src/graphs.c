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
 * Graph stuff
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "utils.h"
#include "device.h"
#include "draw.h"
#include "graphs.h"
#include "graphutils.h"
#include "parser.h"

#include "protos.h"

/* graph definition */
graph *g = NULL;
static int maxgraph = 0;

/* the current graph */
static int cg = -1;

int is_valid_gno(int gno)
{
    if (gno >= 0 && gno < maxgraph) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int number_of_graphs(void)
{
    return maxgraph;
}

int get_cg(void)
{
    return cg;
}

char *graph_types(int it)
{
    static char s[16];

    switch (it) {
    case GRAPH_XY:
	strcpy(s, "XY");
	break;
    case GRAPH_CHART:
	strcpy(s, "Chart");
	break;
    case GRAPH_POLAR:
	strcpy(s, "Polar");
	break;
    case GRAPH_SMITH:
	strcpy(s, "Smith");
	break;
    case GRAPH_FIXED:
	strcpy(s, "Fixed");
	break;
    case GRAPH_PIE:
	strcpy(s, "Pie");
	break;
    default:
        strcpy(s, "Unknown");
	break;
   }
    return s;
}

/*
 * kill all sets in a graph
 */
int kill_all_sets(int gno)
{
    int i;
    
    if (is_valid_gno(gno) == TRUE) {
	for (i = 0; i < g[gno].maxplot; i++) {
	    killset(gno, i);
	}
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int kill_graph(int gno)
{
    int j;
    if (is_valid_gno(gno) == TRUE) {
	kill_all_sets(gno);
        XCFREE(g[gno].labs.title.s);
        XCFREE(g[gno].labs.stitle.s);
        for (j = 0; j < MAXAXES; j++) {
            free_graph_tickmarks(g[gno].t[j]);
            g[gno].t[j] = NULL;
        }
        
        if (gno == maxgraph - 1) {
            maxgraph--;
            g = xrealloc(g, maxgraph*sizeof(graph));
            if (cg == gno) {
                cg--;
            }
        } else {
            set_graph_hidden(gno, TRUE);
        }
        
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

void kill_all_graphs(void)
{
    int i;

    for (i = maxgraph - 1; i >= 0; i--) {
        kill_graph(i);
    }
}

int copy_graph(int from, int to)
{
    int i, j;

    if (is_valid_gno(from) != TRUE || is_valid_gno(to) != TRUE || from == to) {
        return RETURN_FAILURE;
    }
    
    /* kill target graph */
    kill_all_sets(to);
    XCFREE(g[to].labs.title.s);
    XCFREE(g[to].labs.stitle.s);
    for (j = 0; j < MAXAXES; j++) {
        free_graph_tickmarks(g[to].t[j]);
        g[to].t[j] = NULL;
    }

    memcpy(&g[to], &g[from], sizeof(graph));

    /* zero allocatable storage */
    g[to].p = NULL;
    g[to].maxplot = 0;
    g[to].labs.title.s = NULL;
    g[to].labs.stitle.s = NULL;

    /* duplicate allocatable storage */
    if (realloc_graph_plots(to, g[from].maxplot) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }
    for (i = 0; i < g[from].maxplot; i++) {
        do_copyset(from, i, to, i);
    }
    g[to].labs.title.s = copy_string(NULL, g[from].labs.title.s);
    g[to].labs.stitle.s = copy_string(NULL, g[from].labs.stitle.s);
    for (j = 0; j < MAXAXES; j++) {
	g[to].t[j] = copy_graph_tickmarks(g[from].t[j]);
    }

    return RETURN_SUCCESS;
}

int move_graph(int from, int to)
{
    if (is_valid_gno(from) != TRUE || is_valid_gno(to) != TRUE) {
        return RETURN_FAILURE;
    }
    
    if (copy_graph(from, to) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    } else {
        kill_graph(from);
        return RETURN_SUCCESS;
    }
}

int duplicate_graph(int gno)
{
    int new_gno = maxgraph;
    
    if (is_valid_gno(gno) != TRUE) {
        return RETURN_FAILURE;
    }

    if (set_graph_active(new_gno) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }
    
    if (copy_graph(gno, new_gno) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    } else {
        return RETURN_SUCCESS;
    }
}

int swap_graph(int from, int to)
{
    graph gtmp;

    if (is_valid_gno(from) != TRUE || is_valid_gno(to) != TRUE) {
        return RETURN_FAILURE;
    }

    memcpy(&gtmp, &g[from], sizeof(graph));
    memcpy(&g[from], &g[to], sizeof(graph));
    memcpy(&g[to], &gtmp, sizeof(graph));

    set_dirtystate();

    return RETURN_SUCCESS;
}

int get_graph_framep(int gno, framep *f)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(f, &g[gno].f, sizeof(framep));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_graph_locator(int gno, GLocator *locator)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(locator, &g[gno].locator, sizeof(GLocator));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_graph_world(int gno, world *w)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(w, &g[gno].w, sizeof(world));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_graph_viewport(int gno, view *v)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(v, &g[gno].v, sizeof(view));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_graph_labels(int gno, labels *labs)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(labs, &g[gno].labs, sizeof(labels));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_graph_plotarr(int gno, int i, plotarr *p)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(p, &g[gno].p[i], sizeof(plotarr));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

/* Tickmarks */

int is_valid_axis(int gno, int axis)
{
    if (is_valid_gno(gno) &&
        axis >= 0 && axis < MAXAXES &&
        g[gno].t[axis] != NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

tickmarks *get_graph_tickmarks(int gno, int a)
{
    if (is_valid_axis(gno, a) == TRUE) {
        return g[gno].t[a];
    } else {
        return NULL;
    }
}

tickmarks *new_graph_tickmarks(void)
{
    tickmarks *retval;
    
    retval = xmalloc(sizeof(tickmarks));
    if (retval != NULL) {
        set_default_ticks(retval);
    }
    return retval;
}

tickmarks *copy_graph_tickmarks(tickmarks *t)
{
    tickmarks *retval;
    int i;
    
    if (t == NULL) {
        return NULL;
    } else {
        retval = new_graph_tickmarks();
        if (retval != NULL) {
            memcpy(retval, t, sizeof(tickmarks));
	    retval->label.s = copy_string(NULL, t->label.s);
	    retval->tl_formula = copy_string(NULL, t->tl_formula);
            for (i = 0; i < MAX_TICKS; i++) {
                retval->tloc[i].label = copy_string(NULL, t->tloc[i].label);
            }
        }
        return retval;
    }
}

void free_graph_tickmarks(tickmarks *t)
{
    int i;
    
    if (t == NULL) {
        return;
    }
    XCFREE(t->label.s);
    XCFREE(t->tl_formula);
    for (i = 0; i < MAX_TICKS; i++) {
        XCFREE(t->tloc[i].label);
    }
    XCFREE(t);
}

int set_graph_tickmarks(int gno, int a, tickmarks *t)
{
    if (is_valid_axis(gno, a) == TRUE) {
        free_graph_tickmarks(g[gno].t[a]);
        g[gno].t[a] = copy_graph_tickmarks(t);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}


int get_graph_legend(int gno, legend *leg)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(leg, &g[gno].l, sizeof(legend));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int graph_allocate(int gno)
{
    int retval;
    
    if (is_valid_gno(gno)) {
        return RETURN_SUCCESS;
    } else
    if (gno >= maxgraph) {
        retval = realloc_graphs(gno + 1);
        if (retval != RETURN_SUCCESS) {
            return RETURN_FAILURE;
        } else {
            return set_graph_hidden(gno, TRUE);
        }
    } else {
        return RETURN_FAILURE;
    }
}


int set_graph_active(int gno)
{
    if (graph_allocate(gno) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    } else {
        return set_graph_hidden(gno, FALSE);
    }
}

void set_graph_framep(int gno, framep * f)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }
    
    memcpy(&g[gno].f, f, sizeof(framep));
    
    set_dirtystate();
}

void set_graph_locator(int gno, GLocator *locator)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    memcpy(&g[gno].locator, locator, sizeof(GLocator));

    set_dirtystate();
}

void set_graph_world(int gno, world w)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    g[gno].w = w;
    
    set_dirtystate();
}

void set_graph_viewport(int gno, view v)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    g[gno].v = v;
    
    set_dirtystate();
}

void set_graph_labels(int gno, labels *labs)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    xfree(g[gno].labs.title.s);
    xfree(g[gno].labs.stitle.s);
    memcpy(&g[gno].labs, labs, sizeof(labels));
    g[gno].labs.title.s = copy_string(NULL, labs->title.s);
    g[gno].labs.stitle.s = copy_string(NULL, labs->stitle.s);
    
    set_dirtystate();
}

void set_graph_plotarr(int gno, int i, plotarr * p)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    memcpy(&g[gno].p[i], p, sizeof(plotarr));
    
    set_dirtystate();
}

void set_graph_legend(int gno, legend *leg)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    memcpy(&g[gno].l, leg, sizeof(legend));

    set_dirtystate();
}

void set_graph_legend_active(int gno, int flag)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    g[gno].l.active = flag;

    set_dirtystate();
}


/*
 * Count the number of active sets in graph gno
 */
int nactive(int gno)
{
    int i, cnt = 0;

    for (i = 0; i < number_of_sets(gno); i++) {
	if (is_set_active(gno, i)) {
	    cnt++;
	}
    }

    return cnt;
}



int select_graph(int gno)
{
    int retval;

    if (set_parser_gno(gno) == RETURN_SUCCESS) {
        cg = gno;
        retval = definewindow(g[gno].w, g[gno].v, g[gno].type,
                              g[gno].xscale, g[gno].yscale,
                              g[gno].xinvert, g[gno].yinvert);
    } else {
        retval = RETURN_FAILURE;
    }
    
    return retval;
}

int realloc_graphs(int n)
{
    int j;
    graph *gtmp;

    if (n <= 0) {
        return RETURN_FAILURE;
    }
    gtmp = xrealloc(g, n*sizeof(graph));
    if (gtmp == NULL) {
        return RETURN_FAILURE;
    } else {
        g = gtmp;
        for (j = maxgraph; j < n; j++) {
            set_default_graph(j);
        }
        maxgraph = n;
        return RETURN_SUCCESS;
    }
}

int realloc_graph_plots(int gno, int n)
{
    int oldmaxplot, j;
    plotarr *ptmp;
    int c, bg;
    
    if (is_valid_gno(gno) != TRUE) {
        return RETURN_FAILURE;
    }
    if (n < 0) {
        return RETURN_FAILURE;
    }
    if (n == g[gno].maxplot) {
        return RETURN_SUCCESS;
    }
    ptmp = xrealloc(g[gno].p, n * sizeof(plotarr));
    if (ptmp == NULL && n != 0) {
        return RETURN_FAILURE;
    } else {
        oldmaxplot = g[gno].maxplot;
        g[gno].p = ptmp;
        g[gno].maxplot = n;
        bg = getbgcolor();
        c = oldmaxplot + 1;
        for (j = oldmaxplot; j < n; j++) {
            set_default_plotarr(&g[gno].p[j]);
            while (c == bg || get_colortype(c) != COLOR_MAIN) {
                c++;
                c %= number_of_colors();
            }
            set_set_colors(gno, j, c);
            c++;
        }
        return RETURN_SUCCESS;
    }
}


int set_graph_type(int gno, int gtype)
{
    if (is_valid_gno(gno) == TRUE) {
        if (g[gno].type == gtype) {
            return RETURN_SUCCESS;
        }
        
        switch (gtype) {
        case GRAPH_XY:
        case GRAPH_CHART:
        case GRAPH_FIXED:
        case GRAPH_PIE:
            break;
        case GRAPH_POLAR:
	    g[gno].w.xg1 = 0.0;
	    g[gno].w.xg2 = 2*M_PI;
	    g[gno].w.yg1 = 0.0;
	    g[gno].w.yg2 = 1.0;
            break;
        case GRAPH_SMITH:
	    g[gno].w.xg1 = -1.0;
	    g[gno].w.xg2 =  1.0;
	    g[gno].w.yg1 = -1.0;
	    g[gno].w.yg2 =  1.0;
            break;
        default:
            errmsg("Internal error in set_graph_type()");
            return RETURN_FAILURE;
        }
        g[gno].type = gtype;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int is_graph_hidden(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].hidden;
    } else {
        return TRUE;
    }
}

int set_graph_hidden(int gno, int flag)
{
    if (is_valid_gno(gno) == TRUE) {
        g[gno].hidden = flag;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int set_graph_stacked(int gno, int flag)
{
    if (is_valid_gno(gno) == TRUE) {
        g[gno].stacked = flag;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int is_graph_stacked(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].stacked;
    } else {
        return FALSE;
    }
}

double get_graph_bargap(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].bargap;
    } else {
        return 0.0;
    }
}

int set_graph_bargap(int gno, double bargap)
{
    if (is_valid_gno(gno) == TRUE) {
        g[gno].bargap = bargap;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_graph_type(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].type;
    } else {
        return -1;
    }
}

int is_refpoint_active(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].locator.pointset;
    } else {
        return FALSE;
    }
}

int get_graph_xscale(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].xscale;
    } else {
        return -1;
    }
}

int get_graph_yscale(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].yscale;
    } else {
        return -1;
    }
}

int set_graph_xscale(int gno, int scale)
{
    if (is_valid_gno(gno) == TRUE) {
        if (g[gno].xscale != scale) {
            int naxis;
            g[gno].xscale = scale;
            for (naxis = 0; naxis < MAXAXES; naxis++) {
                if (is_xaxis(naxis)) {
                    tickmarks *t;
                    t = get_graph_tickmarks(gno, naxis);
                    if (t) {
                        if (scale == SCALE_LOG) {
                            if (g[gno].w.xg2 <= 0.0) {
                                g[gno].w.xg2 = 10.0;
                            }
                            if (g[gno].w.xg1 <= 0.0) {
                                g[gno].w.xg1 = g[gno].w.xg2/1.0e3;
                            }
                            t->tmajor = 10.0;
                            t->nminor = 9;
                        } else {
                            t->nminor = 1;
                        }
                    }
                }
            }
            set_dirtystate();
        }
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int set_graph_yscale(int gno, int scale)
{
    if (is_valid_gno(gno) == TRUE) {
        if (g[gno].yscale != scale) {
            int naxis;
            g[gno].yscale = scale;
            for (naxis = 0; naxis < MAXAXES; naxis++) {
                if (is_yaxis(naxis)) {
                    tickmarks *t;
                    t = get_graph_tickmarks(gno, naxis);
                    if (t) {
                        if (scale == SCALE_LOG) {
                            if (g[gno].w.yg2 <= 0.0) {
                                g[gno].w.yg2 = 10.0;
                            }
                            if (g[gno].w.yg1 <= 0.0) {
                                g[gno].w.yg1 = g[gno].w.yg2/1.0e3;
                            }
                            t->tmajor = 10.0;
                            t->nminor = 9;
                        } else {
                            t->nminor = 1;
                        }
                    }
                }
            }
            set_dirtystate();
        }
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int set_graph_znorm(int gno, double norm)
{
    if (is_valid_gno(gno) == TRUE) {
        g[gno].znorm = norm;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

double get_graph_znorm(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].znorm;
    } else {
        return 0.0;
    }
}

int is_graph_xinvert(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].xinvert;
    } else {
        return FALSE;
    }
}

int is_graph_yinvert(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].yinvert;
    } else {
        return FALSE;
    }
}

int set_graph_xinvert(int gno, int flag)
{
    if (is_valid_gno(gno) == TRUE) {
        g[gno].xinvert = flag;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int set_graph_yinvert(int gno, int flag)
{
    if (is_valid_gno(gno) == TRUE) {
        g[gno].yinvert = flag;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int is_axis_active(int gno, int axis)
{
    if (is_valid_axis(gno, axis) == TRUE) {
        return g[gno].t[axis]->active;
    } else {
        return FALSE;
    }
}

int is_zero_axis(int gno, int axis)
{
    if (is_valid_axis(gno, axis) == TRUE) {
        return g[gno].t[axis]->zero;
    } else {
        return FALSE;
    }
}

int islogx(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return (g[gno].xscale == SCALE_LOG);
    } else {
        return FALSE;
    }
}

int islogy(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return (g[gno].yscale == SCALE_LOG);
    } else {
        return FALSE;
    }
}

int islogitx(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return (g[gno].xscale == SCALE_LOGIT);
    } else {
        return FALSE;
    }
}

int islogity(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return (g[gno].yscale == SCALE_LOGIT);
    } else {
        return FALSE;
    }
}

/* 
 * Stack manipulation functions
 */
 
void clear_world_stack(void)
{
    if (is_valid_gno(cg) != TRUE) {
        return;
    }

    g[cg].ws_top = 1;
    g[cg].curw = 0;
    g[cg].ws[0].w.xg1 = 0.0;
    g[cg].ws[0].w.xg2 = 0.0;
    g[cg].ws[0].w.yg1 = 0.0;
    g[cg].ws[0].w.yg2 = 0.0;
}

static void update_world_stack()
{
    if (is_valid_gno(cg) != TRUE) {
        return;
    }

    g[cg].ws[g[cg].curw].w = g[cg].w;
}

/* Add a world window to the stack
 * If there are other windows, simply add this one to the bottom of the stack
 * Otherwise, replace the first window with the new window 
 */
void add_world(int gno, double x1, double x2, double y1, double y2)
{
    if (is_valid_gno(gno) != TRUE) {
        return;
    }

    /* see if another entry has been stacked */
    if( g[gno].ws[0].w.xg1 == 0.0 &&
	g[gno].ws[0].w.xg2 == 0.0 &&
	g[gno].ws[0].w.yg1 == 0.0 &&
 	g[gno].ws[0].w.yg2 == 0.0 ) {	    
	    g[gno].ws_top = 0;
	}
		
    if (g[gno].ws_top < MAX_ZOOM_STACK) {
	g[gno].ws[g[gno].ws_top].w.xg1 = x1;
	g[gno].ws[g[gno].ws_top].w.xg2 = x2;
	g[gno].ws[g[gno].ws_top].w.yg1 = y1;
	g[gno].ws[g[gno].ws_top].w.yg2 = y2;

	g[gno].ws_top++;
    } else {
	errmsg("World stack full");
    }
}

void cycle_world_stack(void)
{
    int neww;
    
    if (is_valid_gno(cg) != TRUE) {
        return;
    }

    if (g[cg].ws_top < 1) {
	errmsg("World stack empty");
    } else {
	update_world_stack();
	neww = (g[cg].curw + 1) % g[cg].ws_top;
 	show_world_stack(neww);
    }
}

void show_world_stack(int n)
{
    if (is_valid_gno(cg) != TRUE) {
        return;
    }

    if (g[cg].ws_top < 1) {
	errmsg("World stack empty");
    } else {
	if (n >= g[cg].ws_top) {
	    errmsg("Selected view greater than stack depth");
	} else if (n < 0) {
	    errmsg("Selected view less than zero");
	} else {
	    g[cg].curw = n;
	    g[cg].w = g[cg].ws[n].w;
	}
    }
}

void push_world(void)
{
    int i;

    if (is_valid_gno(cg) != TRUE) {
        return;
    }
    
    if (g[cg].ws_top < MAX_ZOOM_STACK) {
        update_world_stack();
        for( i=g[cg].ws_top; i>g[cg].curw; i-- ) {
               g[cg].ws[i] = g[cg].ws[i-1];
        }
	g[cg].ws_top++;
    } else {
	errmsg("World stack full");
    }
}

/* modified to actually pop the current world view off the stack */
void pop_world(void)
{
    int i, neww;

    if (is_valid_gno(cg) != TRUE) {
        return;
    }

    if (g[cg].ws_top <= 1) {
	errmsg("World stack empty");
    } else {
    	if (g[cg].curw != g[cg].ws_top - 1) {
    	    for (i = g[cg].curw; i < g[cg].ws_top; i++) {
                g[cg].ws[i] = g[cg].ws[i + 1];
            }
            neww = g[cg].curw;
    	} else {
            neww = g[cg].curw - 1;
        }
        g[cg].ws_top--;
        show_world_stack(neww);
    }
}


void set_default_graph(int gno)
{    
    int i;
    
    g[gno].hidden = TRUE;
    g[gno].type = GRAPH_XY;
    g[gno].xinvert = FALSE;
    g[gno].yinvert = FALSE;
    g[gno].xyflip = FALSE;
    g[gno].stacked = FALSE;
    g[gno].bargap = 0.0;
    g[gno].znorm  = 1.0;
    g[gno].xscale = SCALE_NORMAL;
    g[gno].yscale = SCALE_NORMAL;
    g[gno].ws_top = 1;
    g[gno].ws[0].w.xg1=g[gno].ws[0].w.xg2=g[gno].ws[0].w.yg1=g[gno].ws[0].w.yg2=0;
        g[gno].curw = 0;
    g[gno].locator.dsx = g[gno].locator.dsy = 0.0;      /* locator props */
    g[gno].locator.pointset = FALSE;
    g[gno].locator.pt_type = 0;
    g[gno].locator.fx = FORMAT_GENERAL;
    g[gno].locator.fy = FORMAT_GENERAL;
    g[gno].locator.px = 6;
    g[gno].locator.py = 6;
    for (i = 0; i < MAXAXES; i++) {
        g[gno].t[i] = new_graph_tickmarks();
        switch (i) {
        case X_AXIS:
        case Y_AXIS:
            g[gno].t[i]->active = TRUE;
            break;
        case ZX_AXIS:
        case ZY_AXIS:
            g[gno].t[i]->active = FALSE;
            break;
        }
    }
    set_default_framep(&g[gno].f);
    set_default_world(&g[gno].w);
    set_default_view(&g[gno].v);
    set_default_legend(gno, &g[gno].l);
    set_default_string(&g[gno].labs.title);
    g[gno].labs.title.charsize = 1.5;
    set_default_string(&g[gno].labs.stitle);
    g[gno].labs.stitle.charsize = 1.0;
    g[gno].maxplot = 0;
    g[gno].p = NULL;
}

int is_valid_setno(int gno, int setno)
{
    if (is_valid_gno(gno) == TRUE && setno >= 0 && setno < g[gno].maxplot) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int is_set_hidden(int gno, int setno)
{
    if (is_valid_setno(gno, setno) == TRUE) {
        return g[gno].p[setno].hidden;
    } else {
        return FALSE;
    }
}

int set_set_hidden(int gno, int setno, int flag)
{
    if (is_valid_setno(gno, setno) == TRUE) {
        g[gno].p[setno].hidden = flag;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int number_of_sets(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].maxplot;
    } else {
        return -1;
    }
}

int graph_world_stack_size(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].ws_top;
    } else {
        return -1;
    }
}

int get_world_stack_current(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        return g[gno].curw;
    } else {
        return -1;
    }
}

int get_world_stack_entry(int gno, int n, world_stack *ws)
{
    if (is_valid_gno(gno) == TRUE) {
        memcpy(ws, &g[gno].ws[n], sizeof(world_stack));
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int activate_tick_labels(int gno, int axis, int flag)
{
    if (is_valid_axis(gno, axis) == TRUE) {
        g[gno].t[axis]->tl_flag = flag;
        set_dirtystate();
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int set_set_colors(int gno, int setno, int color)
{
    if (is_valid_setno(gno, setno) != TRUE) {
        return RETURN_FAILURE;
    }
    if (color >= number_of_colors() || color < 0) {
        return RETURN_FAILURE;
    }
    
    g[gno].p[setno].linepen.color = color;
    g[gno].p[setno].sympen.color = color;
    g[gno].p[setno].symfillpen.color = color;
    g[gno].p[setno].errbar.pen.color = color;
    
    set_dirtystate();
    return RETURN_SUCCESS;
}

static int project_version;

int get_project_version(void)
{
    return project_version;
}

int set_project_version(int version)
{
    if (version  > bi_version_id()) {
        project_version = bi_version_id();
        return RETURN_FAILURE;
    } else {
        project_version = version;
        return RETURN_SUCCESS;
    }
}

void reset_project_version(void)
{
    project_version = bi_version_id();
}

static char *project_description = NULL;

void set_project_description(char *descr)
{
    project_description = copy_string(project_description, descr);
    set_dirtystate();
}

char *get_project_description(void)
{
    return project_description;
}

extern plotstr *pstr;

void postprocess_project(int version)
{
    int gno, setno, naxis, strno;
    double ext_x, ext_y;
    
    if (version >= bi_version_id()) {
        return;
    }

    if (version < 40005) {
        set_page_dimensions(792, 612, FALSE);
    }

    if (get_project_version() < 50002) {
        setbgfill(TRUE);
    }

    if (get_project_version() < 50003) {
        allow_two_digits_years(TRUE);
        set_wrap_year(1900);
    }
    
    if (version <= 40102) {
#ifndef NONE_GUI
        set_pagelayout(PAGE_FIXED);
#endif
        get_page_viewport(&ext_x, &ext_y);
        rescale_viewport(ext_x, ext_y);
    }

    for (gno = 0; gno < number_of_graphs(); gno++) {
	if (version <= 40102) {
            g[gno].l.vgap -= 1;
        }
	for (setno = 0; setno < number_of_sets(gno); setno++) {
            if (version < 50000) {
                switch (g[gno].p[setno].sym) {
                case SYM_NONE:
                    break;
                case SYM_DOT_OBS:
                    g[gno].p[setno].sym = SYM_CIRCLE;
                    g[gno].p[setno].symsize = 0.0;
                    g[gno].p[setno].symlines = 0;
                    g[gno].p[setno].symfillpen.pattern = 1;
                    break;
                default:
                    g[gno].p[setno].sym--;
                    break;
                }
            }
            if ((version < 40004 && g[gno].type != GRAPH_CHART) ||
                g[gno].p[setno].sympen.color == -1) {
                g[gno].p[setno].sympen.color = g[gno].p[setno].linepen.color;
            }
            if (version < 40200 || g[gno].p[setno].symfillpen.color == -1) {
                g[gno].p[setno].symfillpen.color = g[gno].p[setno].sympen.color;
            }
            
	    if (version <= 40102 && g[gno].type == GRAPH_CHART) {
                set_dataset_type(gno, setno, SET_BAR);
                g[gno].p[setno].sympen = g[gno].p[setno].linepen;
                g[gno].p[setno].symlines = g[gno].p[setno].lines;
                g[gno].p[setno].symlinew = g[gno].p[setno].linew;
                g[gno].p[setno].lines = 0;
                
                g[gno].p[setno].symfillpen = g[gno].p[setno].setfillpen;
                g[gno].p[setno].setfillpen.pattern = 0;
            }
	    if (version <= 40102 && g[gno].p[setno].type == SET_XYHILO) {
                g[gno].p[setno].symlinew = g[gno].p[setno].linew;
            }
	    if (version <= 50112 && g[gno].p[setno].type == SET_XYHILO) {
                g[gno].p[setno].avalue.active = FALSE;
            }
	    if (version < 50100 && g[gno].p[setno].type == SET_BOXPLOT) {
                g[gno].p[setno].symlinew = g[gno].p[setno].linew;
                g[gno].p[setno].symlines = g[gno].p[setno].lines;
                g[gno].p[setno].symsize = 2.0;
                g[gno].p[setno].errbar.riser_linew = g[gno].p[setno].linew;
                g[gno].p[setno].errbar.riser_lines = g[gno].p[setno].lines;
                g[gno].p[setno].lines = 0;
                g[gno].p[setno].errbar.barsize = 0.0;
            }
            if (version < 50003) {
                g[gno].p[setno].errbar.active = TRUE;
                g[gno].p[setno].errbar.pen.color = g[gno].p[setno].sympen.color;
                g[gno].p[setno].errbar.pen.pattern = 1;
                switch (g[gno].p[setno].errbar.ptype) {
                case PLACEMENT_NORMAL:
                    g[gno].p[setno].errbar.ptype = PLACEMENT_OPPOSITE;
                    break;
                case PLACEMENT_OPPOSITE:
                    g[gno].p[setno].errbar.ptype = PLACEMENT_NORMAL;
                    break;
                case PLACEMENT_BOTH:
                    switch (g[gno].p[setno].type) {
                    case SET_XYDXDX:
                    case SET_XYDYDY:
                    case SET_BARDYDY:
                        g[gno].p[setno].errbar.ptype = PLACEMENT_NORMAL;
                        break;
                    }
                    break;
                }
            }
            if (version < 50002) {
                g[gno].p[setno].errbar.barsize *= 2;
            }
            if (version < 50105) {
                /* Starting with 5.1.5, X axis min & inverting is honored
                   in pie charts */
                if (get_graph_type(gno) == GRAPH_PIE) {
                    world w;
                    get_graph_world(gno, &w);
                    w.xg1 = 0.0;
                    w.xg2 = 2*M_PI;
                    set_graph_world(gno, w);
                    set_graph_xinvert(gno, FALSE);
                }
            }
            if (version < 50107) {
                /* Starting with 5.1.7, symskip is honored for all set types */
                switch (g[gno].p[setno].type) {
                case SET_BAR:
                case SET_BARDY:
                case SET_BARDYDY:
                case SET_XYHILO:
                case SET_XYR:
                case SET_XYVMAP:
                case SET_BOXPLOT:
                    g[gno].p[setno].symskip = 0;
                    break;
                }
            }
        }
        for (naxis = 0; naxis < MAXAXES; naxis++) {
	    tickmarks *t = get_graph_tickmarks(gno, naxis);
            if (!t) {
                continue;
            }
            
            if (version <= 40102) {
                if ( (is_xaxis(naxis) && g[gno].xscale == SCALE_LOG) ||
                     (!is_xaxis(naxis) && g[gno].yscale == SCALE_LOG) ) {
                    t->tmajor = pow(10.0, t->tmajor);
                }
                
                /* TODO : world/view translation */
                t->offsx = 0.0;
                t->offsy = 0.0;
            }
	    if (version < 50000) {
	        /* There was no label_op in Xmgr */
                t->label_op = t->tl_op;
	        
                /* in xmgr, axis label placement was in x,y coordinates */
	        /* in Grace, it's parallel/perpendicular */
	        if(!is_xaxis(naxis)) {
	            fswap(&t->label.x, &t->label.y);
	        }
	        t->label.y *= -1;
	    }
	    if (version >= 50000 && version < 50103) {
	        /* Autoplacement of axis labels wasn't implemented 
                   in early versions of Grace */
                if (t->label_place == TYPE_AUTO) {
                    t->label.x = 0.0;
                    t->label.y = 0.08;
                    t->label_place = TYPE_SPEC;
                }
            }
        }
    }
    
    if (version >= 40200 && version <= 50005) {
        /* BBox type justification was erroneously set */
        for (strno = 0; strno < number_of_strings(); strno++) {
            pstr[strno].just |= JUST_MIDDLE;
        }
    }
}
