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

#ifndef __UTILS_H_
#define __UTILS_H_

/* for size_t */
#include <sys/types.h>

#define  PAD(bits, pad)  (((bits)+(pad)-1)&-(pad))

#define MIN2(a, b) (((a) < (b)) ? a : b)
#define MAX2(a, b) (((a) > (b)) ? a : b)
#define MIN3(a, b, c) (((a) < (b)) ? MIN2(a, c) : MIN2(b, c))
#define MAX3(a, b, c) (((a) > (b)) ? MAX2(a, c) : MAX2(b, c))

#define yes_or_no(x) ((x)?"yes":"no")
#define on_or_off(x) ((x)?"on":"off")
#define true_or_false(x) ((x)?"true":"false")
#define w_or_v(x) ((x == COORD_WORLD)?"world":"view")

#define LFORMAT_TYPE_PLAIN      0
#define LFORMAT_TYPE_EXTENDED   1

#define XCFREE(ptr) xfree(ptr); ptr = NULL

#define PSTRING(s) ((s == NULL) ? "" : escapequotes(s))

void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
void *xrealloc(void *ptr, size_t size);
void xfree(void *ptr);

void fswap(double *x, double *y);
void iswap(int *x, int *y);
int isoneof(int c, char *s);
int argmatch(char *s1, char *s2, int atleast);
void lowtoupper(char *s);
void convertchar(char *s);
int ilog2(int n);
double comp_area(int n, double *x, double *y);
double comp_perimeter(int n, double *x, double *y);

char *create_fstring(int form, int prec, double loc, int type);
char *escapequotes (char *s);

int sign(double a);
double mytrunc(double a);

void bailout(void);

void installSignal(void);

int bin_dump(char *value, int i, int pad);
unsigned char reversebits(unsigned char inword);

char *copy_string(char *dest, const char *src);
char *concat_strings(char *dest, const char *src);
int compare_strings(const char *s1, const char *s2);

char *get_grace_home(void);
void set_grace_home(const char *dir);

char *get_help_viewer(void);
void set_help_viewer(const char *dir);

char *get_print_cmd(void);
void set_print_cmd(const char *cmd);

char *get_editor(void);
void set_editor(const char *cmd);

void set_docname(const char *s);
char *get_docname(void);
char *get_docbname(void);

void errmsg(const char *msg);
void echomsg(char *msg);
void stufftext(char *msg);

int yesnoterm(char *msg);
int yesno(char *msg, char *s1, char *s2, char *help_anchor);

char *mybasename(const char *s);

void expand_tilde(char *buf);
int set_workingdir(const char *wd);
char *get_workingdir(void);

void init_username(void);
char *get_username(void);

void init_userhome(void);
char *get_userhome(void);

void update_app_title(void);

void set_dirtystate(void);
void clear_dirtystate(void);
void lock_dirtystate(int flag);
int is_dirtystate(void);

int system_wrap(const char *string);
void msleep_wrap(unsigned int msec);

int init_locale(void);
void set_locale_num(int flag);

long bi_version_id(void);
char *bi_version_string(void);
char *bi_system(void);
char *bi_date(void);
char *bi_gui(void);
#ifdef MOTIF_GUI
char *bi_gui_xbae(void);
#endif
char *bi_ccompiler(void);
char *bi_t1lib(void);
#ifdef HAVE_LIBPNG
char *bi_pnglib(void);
#endif
#ifdef HAVE_LIBJPEG
char *bi_libjpeg(void);
#endif
#ifdef HAVE_LIBPDF
char *bi_libpdf(void);
#endif


#ifdef DEBUG
void set_debuglevel(int level);
int get_debuglevel(void);
#endif

#endif /* __UTILS_H_*/
