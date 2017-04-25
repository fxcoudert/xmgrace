/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2001 Grace Development Team
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

#ifndef __GRAPHS_H_
#define __GRAPHS_H_

#include "defines.h"

/* Graph type */
typedef enum {
    GRAPH_XY   ,
    GRAPH_CHART,
    GRAPH_POLAR,
    GRAPH_SMITH,
    GRAPH_FIXED,
    GRAPH_PIE
} GraphType;

/* Set types */
typedef enum {
    SET_XY        ,
    SET_XYDX      ,
    SET_XYDY      ,
    SET_XYDXDX    ,
    SET_XYDYDY    ,
    SET_XYDXDY    ,
    SET_XYDXDXDYDY,
    SET_BAR       ,
    SET_BARDY     ,
    SET_BARDYDY   ,
    SET_XYHILO    ,
    SET_XYZ       ,
    SET_XYR       ,
    SET_XYSIZE    ,
    SET_XYCOLOR   ,
    SET_XYCOLPAT  ,
    SET_XYVMAP    ,
    SET_BOXPLOT   ,
    SET_BAD
} SetType;
#define NUMBER_OF_SETTYPES  SET_BAD

/* Data column names; */
typedef enum {
    DATA_X ,
    DATA_Y ,
    DATA_Y1,
    DATA_Y2,
    DATA_Y3,
    DATA_Y4,
    DATA_BAD
} DataColumn;
#define MAX_SET_COLS    DATA_BAD


/* target graph & set*/
typedef struct {
    int gno;    /* graph # */
    int setno;  /* set # */
} target;

typedef struct {
    int len;                    /* dataset length */
    double *ex[MAX_SET_COLS];   /* arrays of x, y, z, ... depending on type */
    char **s;                   /* pointer to strings */
} Dataset;

typedef struct {
    double ex[MAX_SET_COLS];   /* x, y, dx, z, ... depending on dataset type */
    char *s;                   /* string */
} Datapoint;

typedef struct {
    Dataset data;               /* dataset */
    
    int hidden;                 /* hidden set */

    int type;                   /* dataset type */

    char comments[MAX_STRING_LENGTH];   /* how did this set originate */

    int hotlink;                /* hot linked set */
    int hotsrc;                 /* source for hot linked file (DISK|PIPE) */
    char hotfile[GR_MAXPATHLEN];   /* hot linked filename */

    int sym;                    /* set plot symbol type */
    double symsize;             /* size of symbols */
    Pen sympen;                 /* pen props of symbol line */
    Pen symfillpen;             /* pen props of symbol filling */
    int symlines;               /* symbol linestyle */
    double symlinew;            /* symbol linewidth */
    int symskip;                /* number of symbols to skip */
    unsigned char symchar;      /* char used if sym == SYM_CHAR */
    int charfont;               /* font for symchar if sym == SYM_CHAR */

    int linet;                  /* set line type */
    int lines;                  /* set line style */
    double linew;               /* line width */
    Pen linepen;                /* pen for connecting line */

    int baseline_type;          /* type of baseline */
    int baseline;               /* should the baseline be drawn */
    int dropline;               /* should the drop lines (from data points to
                                                      the baseline) be drawn */
    
    int filltype;               /* fill type */
    int fillrule;               /* fill rule (winding/even-odd) */
    Pen setfillpen;             /* pen props for set fill */

    char lstr[MAX_STRING_LENGTH];       /* legend for this set */

    AValue avalue;              /* Parameters for annotative string */
    Errbar errbar;              /* error bar properties */
} plotarr;

/* Locator props */
typedef struct {
    int pointset;               /* if (dsx, dsy) have been set */
    int pt_type;                /* type of locator display */
    double dsx, dsy;            /* locator fixed point */
    int fx, fy;                 /* locator format type */
    int px, py;                 /* locator precision */
} GLocator;

/*
 * a graph
 */
typedef struct {
    int hidden;                 /* display or not */

    int type;                   /* type of graph */

    int maxplot;                /* number of sets allocated for this graph */

    int xscale;                 /* scale mapping of X axes*/
    int yscale;                 /* scale mapping of Y axes*/
    int xinvert;                /* X axes inverted, TRUE or FALSE */
    int yinvert;                /* Y axes inverted, TRUE or FALSE */
    int xyflip;                 /* whether x and y axes should be flipped */

    int stacked;                /* TRUE if graph is stacked */
    double bargap;              /* Distance between bars (in bar charts) */
    double znorm;               /* Normalization of pseudo-3D graphs */

    plotarr *p;                 /* sets go here */

    legend l;                   /* legends */

    world w;                    /* world */
    view v;                     /* viewport */

    labels labs;                /* title and subtitle */

    tickmarks *t[MAXAXES];      /* flags etc. for tickmarks for all axes */

    framep f;                   /* type of box around plot */

    GLocator locator;           /* locator props */

    world_stack ws[MAX_ZOOM_STACK]; /* zoom stack */
    int ws_top;                 /* stack pointer */
    int curw;                   /* for cycling through the stack */
} graph;


int get_cg(void);

char *graph_types(int it);
char *set_types(int it);
int get_settype_by_name(char *s);

int kill_graph(int gno);
void kill_all_graphs(void);
int copy_graph(int from, int to);
int move_graph(int from, int to);
int swap_graph(int from, int to);
int duplicate_graph(int gno);

tickmarks *new_graph_tickmarks(void);
tickmarks *copy_graph_tickmarks(tickmarks *);
tickmarks *get_graph_tickmarks(int gno, int a);
void free_graph_tickmarks(tickmarks *t);
int set_graph_tickmarks(int gno, int a, tickmarks *t);

int get_graph_framep(int gno, framep *f);
int get_graph_world(int gno, world *w);
int get_graph_viewport(int gno, view *v);
int get_graph_labels(int gno, labels *labs);
int get_graph_plotarr(int gno, int i, plotarr *p);
int get_graph_legend(int gno, legend *leg);

int graph_allocate(int gno);
int set_graph_active(int gno);

void set_graph_framep(int gno, framep *f);
void set_graph_world(int gno, world w);
void set_graph_viewport(int gno, view v);
void set_graph_labels(int gno, labels *labs);
void set_graph_plotarr(int gno, int i, plotarr *p);
void set_graph_legend(int gno, legend *leg);
void set_graph_legend_active(int gno, int flag);


int nactive(int gno);

#define is_graph_active(gno) is_valid_gno(gno)

int is_graph_hidden(int gno);
int set_graph_hidden(int gno, int flag);

int get_graph_type(int gno);

int is_graph_stacked(int gno);
int set_graph_stacked(int gno, int flag);

double get_graph_bargap(int gno);
int set_graph_bargap(int gno, double bargap);

int islogx(int gno);
int islogy(int gno);

int islogitx(int gno);
int islogity(int gno);

int number_of_graphs(void);
int select_graph(int gno);

int realloc_graphs(int n);
int realloc_graph_plots(int gno, int n);

int set_graph_xscale(int gno, int scale);
int set_graph_yscale(int gno, int scale);

int get_graph_xscale(int gno);
int get_graph_yscale(int gno);

int set_graph_znorm(int gno, double norm);
double get_graph_znorm(int gno);

int is_valid_gno(int gno);

int set_graph_type(int gno, int gtype);

int allocate_set(int gno, int setno);
int activateset(int gno, int setno);

int is_valid_setno(int gno, int setno);
int is_set_active(int gno, int setno);
int is_set_hidden(int gno, int setno);
int set_set_hidden(int gno, int setno, int flag);

#define is_set_drawable(gno, setno) (is_set_active(gno, setno) && !is_set_hidden(gno, setno))

int number_of_sets(int gno);

int load_comments_to_legend(int gno, int setno);

int settype_cols(int type);
int dataset_type(int gno, int setno);
int dataset_cols(int gno, int setno);
char *dataset_colname(int col);

int is_refpoint_active(int gno);

int set_refpoint(int gno, WPoint wp);

WPoint get_refpoint(int gno);

double *getcol(int gno, int setno, int col);
#define getx(gno, setno) getcol(gno, setno, 0)
#define gety(gno, setno) getcol(gno, setno, 1)

char *get_legend_string(int gno, int setno);
int set_legend_string(int gno, int setno, char *s);

int set_dataset_type(int gno, int set, int stype);

char *getcomment(int gno, int setno);
int setcomment(int gno, int setno, char *s);

int set_set_strings(int gno, int setno, int len, char **s);
char **get_set_strings(int gno, int setno);

int setlength(int gno, int setno, int length);
int getsetlength(int gno, int setno);

double setybase(int gno, int setno);

int is_graph_xinvert(int gno);
int is_graph_yinvert(int gno);

int set_graph_xinvert(int gno, int flag);
int set_graph_yinvert(int gno, int flag);

int is_valid_axis(int gno, int axis);
int is_axis_active(int gno, int axis);
int is_zero_axis(int gno, int axis);

void cycle_world_stack(void);
void clear_world_stack(void);
void show_world_stack(int n);
void add_world(int gno, double x1, double x2, double y1, double y2);
void push_world(void);

int activate_tick_labels(int gno, int axis, int flag);

int get_graph_locator(int gno, GLocator *locator);
void set_graph_locator(int gno, GLocator *locator);

int graph_world_stack_size(int gno);
int get_world_stack_current(int gno);
int get_world_stack_entry(int gno, int n, world_stack *ws);

int set_set_colors(int gno, int setno, int color);

int moveset(int gnofrom, int setfrom, int gnoto, int setto);
int copyset(int gnofrom, int setfrom, int gnoto, int setto);
int copysetdata(int gnofrom, int setfrom, int gnoto, int setto);

int get_recent_setno(void);
int get_recent_gno(void);

int get_project_version(void);
int set_project_version(int version);
void reset_project_version(void);

void set_project_description(char *descr);
char *get_project_description(void);

void postprocess_project(int version);

#endif /* __GRAPHS_H_ */
