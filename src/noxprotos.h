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
 * Prototypes not involving X
 *
 */

#ifndef __NOXPROTOS_H_
#define __NOXPROTOS_H_

/* For FILE */
#include <stdio.h>

#include "defines.h"
#include "graphs.h"

int filter_set(int gno, int setno, char *rarray);

void do_fourier_command(int gno, int setno, int ftype, int ltype);
int do_compute(int gno, int setno, int graphto, int loadto, char *rarray, char *fstr);
double trapint(double *x, double *y, double *resx, double *resy, int n);
void do_digfilter(int set1, int set2);
void do_linearc(int gno1, int set1, int gno2, int set2);
void do_xcor(int gno1, int set1, int gno2, int set2, int lag,  int covar);
double do_int(int gno, int setno, int itype);
void do_differ(int gno, int setno, int itype);
void do_regress(int gno, int setno, int ideg, int iresid, int rno, int invr, int rset);
void do_runavg(int gno, int setno, int runlen, int runtype, int rno, int invr);
void do_fourier(int gno, int setno, int fftflag, int load, int loadx, int invflag, int type, int wind);
void do_window(int setno, int type, int wind);
void apply_window(double *xx, double *yy, int ilen, int type, int wind);
int do_histo(int fromgraph, int fromset, int tograph, int toset,
	      double *bins, int nbins, int cumulative, int normalize);
int histogram(int ndata, double *data, int nbins, double *bins, int *hist);

void do_sample(int setno, int typeno, char *exprstr, int startno, int stepno);
void do_prune(int setno, int typeno, int deltatypeno, float deltax, float deltay, int dxtype, int dytype);

void set_program_defaults(void);
void set_region_defaults(int i);
void set_default_framep(framep * f);
void set_default_world(world * w);
void set_default_view(view * v);
void set_default_string(plotstr * s);
void set_default_arrow(Arrow *arrowp);
void set_default_line(linetype * l);
void set_default_box(boxtype * b);
void set_default_ellipse(ellipsetype * b);
void set_default_legend(int gno, legend * l);
void set_default_plotarr(plotarr * p);
void set_default_graph(int gno);
int realloc_lines(int n);
int realloc_boxes(int n);
int realloc_ellipses(int n);
int realloc_strings(int n);
void set_default_ticks(tickmarks *t);

void calculate_tickgrid(int gno);
void drawgrid(int gno);
void drawaxes(int gno);

void unregister_real_time_input(const char *name);
int register_real_time_input(int fd, const char *name, int reopen);
int real_time_under_monitoring(void);
int monitor_input(Input_buffer *tbl, int tblsize, int no_wait);

void stasum(double *x, int n, double *xbar, double *sd);
double leasev(double *c, int degree, double x);
int fitcurve(double *x, double *y, int n, int ideg, double *fitted);
void runavg(double *x, double *y, double *ax, double *ay, int n, int ilen);
void runstddev(double *x, double *y, double *ax, double *ay, int n, int ilen);
void runmedian(double *x, double *y, double *ax, double *ay, int n, int ilen);
void runminmax(double *x, double *y, double *ax, double *ay, int n, int ilen, int type);
void filterser(int n, double *x, double *y, double *resx, double *resy, double *h, int len);
void linearconv(double *x, double *h, double *y, int n, int m);
int crosscorr(double *x, double *y, int n, int maxlag, int covar, double *xres);
int transfit(int type, int n, double *x, double *y, double *fitted);
int linear_regression(int n, double *x, double *y, double *fitted);

void spline(int n, double *x, double *y, double *b, double *c, double *d);
void aspline(int n, double *x, double *y, double *b, double *c, double *d);
int seval(double *u, double *v, int ulen,
    double *x, double *y, double *b, double *c, double *d, int n);

void dft(double *jr, double *ji, int n, int iflag);
void fft(double *real_data, double *imag_data, int n_pts, int nu, int inv);

float humlik(const float x, const float y);

void putparms(int gno, FILE * pp, int embed);
void put_fitparms(FILE * pp, int embed);

void get_graph_box(int i, boxtype * b);
void get_graph_line(int i, linetype * l);
void get_graph_ellipse(int i, ellipsetype * e);
void get_graph_string(int i, plotstr * s);
void set_graph_box(int i, boxtype *b);
void set_graph_line(int i, linetype *l);
void set_graph_string(int i, plotstr *s);
void set_graph_ellipse(int i, ellipsetype * e);

char *object_types(int type);

void pop_world(void);

void define_autos(int aon, int au, int ap);

int find_item(int gno, VPoint vp, view *bb, int *type, int *id);

int is_valid_line(int line);
int is_valid_box(int box);
int is_valid_ellipse(int ellipse);
int is_valid_string(int string);

int isactive_line(int lineno);
int isactive_box(int boxno);
int isactive_ellipse(int ellipno);
int isactive_string(int strno);

int next_line(void);
int next_box(void);
int next_ellipse(void);
int next_string(void);

void kill_box(int boxno);
void kill_ellipse(int ellipseno);
void kill_line(int lineno);
void kill_string(int stringno);

void copy_object(int type, int from, int to);
int kill_object(int type, int id);
int next_object(int type);
int duplicate_object(int type, int id);

int get_object_bb(int type, int id, view *bb);
void move_object(int type, int id, VVector shift);

int number_of_boxes(void);
int number_of_ellipses(void);
int number_of_lines(void);
int number_of_strings(void);

void init_string(int id, VPoint vp);
void init_line(int id, VPoint vp1, VPoint vp2);
void init_box(int id, VPoint vp1, VPoint vp2);
void init_ellipse(int id, VPoint vp1, VPoint vp2);

void do_clear_lines(void);
void do_clear_boxes(void);
void do_clear_text(void);
void do_clear_ellipses(void);

int getsetminmax(int gno, int setno, double *x1, double *x2, double *y1, double *y2);
int getsetminmax_c(int gno, int setno, double *xmin, double *xmax, double *ymin, double *ymax, int ivec);
void minmax(double *x, int n, double *xmin, double *xmax, int *imin, int *imax);
int minmaxrange(double *bvec, double *vec, int n, double bvmin, double bvmax,
              	   double *vmin, double *vmax);
double vmin(double *x, int n);
double vmax(double *x, int n);
int set_point(int gno, int setn, int seti, WPoint wp);
int get_point(int gno, int setn, int seti, WPoint *wp);
int get_datapoint(int gno, int setn, int seti, int *ncols, Datapoint *wp);
void setcol(int gno, int setno, int col, double *x, int len);

void copycol2(int gfrom, int setfrom, int gto, int setto, int col);
#define copyx(gno, setfrom, setto)      copycol2(gno, setfrom, gno, setto, 0)
#define copyy(gno, setfrom, setto)      copycol2(gno, setfrom, gno, setto, 1)

void packsets(int gno);
int nextset(int gno);
void killset(int gno, int setno);
void killsetdata(int gno, int setno);
int number_of_active_sets(int gno);
int swapset(int gfrom, int j1, int gto, int j2);
int pushset(int gno, int setno, int push_type);
void droppoints(int gno, int setno, int startno, int endno);
int join_sets(int gno, int *sets, int nsets);
void sort_xy(double *tmp1, double *tmp2, int up, int sorton, int stype);
void reverse_set(int gno, int setno);

void del_point(int gno, int setno, int pt);
void add_point(int gno, int setno, double px, double py);
void zero_datapoint(Datapoint *dpoint);
int add_point_at(int gno, int setno, int ind, const Datapoint *dpoint);
void delete_byindex(int gno, int setno, int *ind);

int do_copyset(int gfrom, int j1, int gto, int j2);
int do_moveset(int gfrom, int j1, int gto, int j2);
int do_swapset(int gno1, int setno1, int gno2, int setno2);
void do_splitsets(int gno, int setno, int lpart);
void do_activate(int setno, int type, int len);
void do_hideset(int gno, int setno);
void do_showset(int gno, int setno);
void do_changetype(int setno, int type);
void do_copy(int j1, int gfrom, int j2, int gto);
void do_move(int j1, int gfrom, int j2, int gto);
void do_drop_points(int gno, int setno, int startno, int endno);
void do_kill(int gno, int setno, int soft);
void do_sort(int setno, int sorton, int stype);
void do_cancel_pickop(void);


void set_hotlink(int gno, int setno, int onoroff, char *fname, int src);
int is_hotlinked(int gno, int setno);
void do_update_hotlink(int gno, int setno);
char *get_hotlink_file(int gno, int setno);
int get_hotlink_src(int gno, int setno);

void sortset(int gno, int setno, int sorton, int stype);
void do_seasonal_diff(int setno, int period);
int do_nonlfit(int gno, int setno, double *warray, char *rarray, int nsteps);
int do_interp(int gno_src, int setno_src, int gno_dest, int setno_dest,
    double *mesh, int meshlen, int method, int strict);
int get_restriction_array(int gno, int setno,
    int rtype, int negate, char **rarray);

int monotonicity(double *array, int len, int strict);
int find_span_index(double *array, int len, int m, double x);

int inbounds(int gno, double x, double y);
int intersect_to_left(double x, double y, double x1, double y1, double x2, double y2);
int inbound(double x, double y, double *xlist, double *ylist, int n);
int isleft(double x, double y, double x1, double y1, double x2, double y2);
int isright(double x, double y, double x1, double y1, double x2, double y2);
int isabove(double x, double y, double x1, double y1, double x2, double y2);
int isbelow(double x, double y, double x1, double y1, double x2, double y2);
void reporton_region(int gno, int rno, int type);
int isactive_region(int regno);
char *region_types(int it, int which);
void kill_region(int r);
void kill_all_regions(void);
void activate_region(int r, int type, int gno);
void load_poly_region(int r, int gno, int n, WPoint *wps);
int inregion(int regno, double x, double y);

void set_plotstr_string(plotstr * pstr, char *buf);

void cli_loop(void);

void initialize_nonl(void);
void reset_nonl(void);

int is_xaxis(int axis);
int is_yaxis(int axis);
int is_log_axis(int gno, int axis);
int is_logit_axis(int gno, int axis);

void kill_blockdata(void);
void alloc_blockdata(int ncols);

void set_ref_date(double ref);
double get_ref_date(void);
void set_date_hint(Dates_format preferred);
Dates_format get_date_hint(void);
void allow_two_digits_years(int allowed);
int two_digits_years_allowed(void);
void set_wrap_year(int year);
int get_wrap_year(void);
long cal_to_jul(int y, int m, int d);
void jul_to_cal(long n, int *y, int *m, int *d);
double jul_and_time_to_jul(long jul, int hour, int min, double sec);
double cal_and_time_to_jul(int y, int m, int d,
                           int hour, int min, double sec);
void jul_to_cal_and_time(double jday, int rounding,
                         int *y, int *m, int *d,
                         int *hour, int *min, int *sec);
int parse_float(const char *s, double *value, const char **after);
int parse_date(const char* s, Dates_format preferred, int absolute,
               double *jul, Dates_format *recognized);
int parse_date_or_number(const char* s, int absolute, double *value);

#endif /* __NOXPROTOS_H_ */
