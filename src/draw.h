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

#ifndef __DRAW_H_
#define __DRAW_H_

#include "defines.h"

/* bpp that Grace uses internally ( = 256 colors) */
#define GRACE_BPP	8
#define MAXCOLORS	(0x01 << GRACE_BPP)

#define MAXPATTERNS 32

#define MAXLINESTYLES 9

#define MAX_LINEWIDTH 20.0        /* max width of drawn lines */

/* polyline drawing modes */
#define POLYLINE_OPEN	    0
#define POLYLINE_CLOSED	    1

/* polygon fill type */
#define FILLRULE_WINDING    0
#define FILLRULE_EVENODD    1

/* arc fill modes */
#define ARCFILL_CHORD       0
#define ARCFILL_PIESLICE    1

/* pixmap transparency types */
#define PIXMAP_TRANSPARENT  0
#define PIXMAP_OPAQUE	    1

/* line cap parameter */
#define LINECAP_BUTT        0
#define LINECAP_ROUND       1
#define LINECAP_PROJ        2

/* line join type */
#define LINEJOIN_MITER      0
#define LINEJOIN_ROUND      1
#define LINEJOIN_BEVEL      2

/* Text string justifications */
/* Horizontal */
#define JUST_LEFT       0
#define JUST_RIGHT      1
#define JUST_CENTER     2

/* Vertical */
#define JUST_BLINE      0
#define JUST_BOTTOM     4
#define JUST_TOP        8
#define JUST_MIDDLE    12

/* Drawing properties */
typedef struct {
    Pen pen;
    int bgcolor;
    int bgfilled;
    int lines;
    double linew;
    int linecap;
    int linejoin;
    double charsize;
    int font;
    int fillrule;
} DrawProps;

typedef struct {
    int red;
    int green;
    int blue;
} RGB;

typedef struct {
    double red;
    double green;
    double blue;
} fRGB;

typedef struct {
    double y;
    double i;
    double q;
} YIQ;

#define BAD_COLOR	-1

#define COLOR_NONE      0
#define COLOR_AUX       1
#define COLOR_MAIN      2

typedef struct {
    RGB rgb;
    char *cname;
    int ctype;
    int tstamp;
} CMap_entry;

#define BBOX_TYPE_GLOB	0
#define BBOX_TYPE_TEMP	1

typedef struct {
    int active;
    view v;
    view fv;
} BBox_type;

/* The default max drawing path limit */
#define MAX_DRAWING_PATH  20000

void setpen(Pen pen);
Pen getpen(void);

void setcolor(int color);
int getcolor(void);

void setbgcolor(int bgcolor);
int getbgcolor(void);

void setbgfill(int flag);
int getbgfill(void);

void setlinestyle(int lines);
int getlinestyle(void);

void setlinewidth(double linew);
double getlinewidth(void);

void setpattern(int pattern);
int getpattern(void);

void setcharsize(double charsize);
double getcharsize(void);

void setfont(int font);
int getfont(void);

void setfillrule(int rule);
int getfillrule(void);

void setlinecap(int type);
int getlinecap(void);

void setlinejoin(int type);
int getlinejoin(void);

void symplus(VPoint vp, double s);
void symx(VPoint vp, double s);
void symsplat(VPoint vp, double s);

void leavegraphics(void);

void DrawRect(VPoint vp1, VPoint vp2);
void FillRect(VPoint vp1, VPoint vp2);
void DrawLine(VPoint vp1, VPoint vp2);
void DrawPolyline(VPoint *vps, int n, int mode);
void DrawPolygon(VPoint *vps, int n);
void DrawArc(VPoint vp1, VPoint vp2, int angle1, int angle2);
void DrawFilledArc(VPoint vp1, VPoint vp2, int angle1, int angle2, int mode);
void DrawEllipse(VPoint vp1, VPoint vp2);
void DrawFilledEllipse(VPoint vp1, VPoint vp2);
void DrawCircle(VPoint vp, double radius);
void DrawFilledCircle(VPoint vp, double radius);

void WriteString(VPoint vp, int rot, int just, char *theString);

int is_wpoint_inside(WPoint *wp, world *w);
int is_vpoint_inside(view v, VPoint vp, double epsilon);

void setclipping(int fl);
int doclipping(void);
int is_validVPoint(VPoint vp);
int is_validWPoint(WPoint wp);
VPoint *line_intersect(VPoint vp1, VPoint vp2, VPoint vp1p, VPoint vp2p, int mode);
int clip_line(VPoint vp1, VPoint vp2, VPoint *vp1c, VPoint *vp2c);
int intersect_polygon(VPoint *vps, int n, VPoint vp1p, VPoint vp2p);
int clip_polygon(VPoint *vps, int n);

int is_valid_color(RGB rgb);
int find_color(RGB rgb);
int get_color_by_name(char *cname);
int realloc_color(int n);
int store_color(int n, CMap_entry cmap);
int add_color(CMap_entry cmap);
RGB *get_rgb(unsigned int cindex);
RGB *get_srgb(unsigned int cindex);
fRGB *get_frgb(unsigned int cindex);
fRGB *get_fsrgb(unsigned int cindex);
CMap_entry *get_cmap_entry(unsigned int cindex);
char *get_colorname(unsigned int cindex);
int get_colortype(unsigned int cindex);
YIQ RGB2YIQ(RGB rgb);
double get_colorintensity(int cindex);
void initialize_cmap(void);
void reverse_video(void);
int is_video_reversed(void);

char *scale_types(int it);

int isvalid_viewport(view v);

double fscale(double wc, int scale);
double ifscale(double vc, int scale);

int polar2xy(double phi, double rho, double *x, double *y);
void xy2polar(double x, double y, double *phi, double *rho);

double xy_xconv(double wx);
double xy_yconv(double wy);
VPoint Wpoint2Vpoint(WPoint wp);
int world2view(double x, double y, double *xv, double *yv);
void view2world(double xv, double yv, double *xw, double *yw);

int definewindow(world w, view v, int gtype,
                 int xscale, int yscale,
                 int xinv, int yinv);

void reset_bbox(int type);
void reset_bboxes(void);
void freeze_bbox(int type);
view get_bbox(int type);
int is_valid_bbox(view v);
view merge_bboxes(view v1, view v2);
void update_bbox(int type, VPoint vp);
void update_bboxes(VPoint vp);
void melt_bbox(int type);
void activate_bbox(int type, int status);
int view_extend(view *v, double w);
int update_bboxes_with_view(view *v);
int update_bboxes_with_vpoints(VPoint *vps, int n, double lw);
int VPoints2bbox(VPoint *vp1, VPoint *vp2, view *bb);

void set_draw_mode(int mode);
int get_draw_mode(void);

int number_of_colors(void);
int number_of_patterns(void);
int number_of_linestyles(void);

void vpswap(VPoint *vp1, VPoint *vp2);
int points_overlap(VPoint vp1, VPoint vp2);

void set_max_path_limit(int limit);
int get_max_path_limit(void);

#endif /* __DRAW_H_ */
