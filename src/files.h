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
 * File I/O 
 */

#ifndef __FILES_H_
#define __FILES_H_

#include <stdio.h>

/* data load types */
#define LOAD_SINGLE 0
#define LOAD_NXY    1
#define LOAD_BLOCK  2

int add_io_filter( int type, int method, char *id, char *comm );
int add_input_filter( int method, char *id, char *comm );
int add_output_filter( int method, char *id, char *comm );
void clear_io_filters( int f );
FILE *filter_read( char *fn );
FILE *filter_write(  char *fn );

char *grace_path(char *fn);
char *grace_exe_path(char *fn);

FILE *grace_openw(char *fn);
FILE *grace_openr(char *fn, int src);
void grace_close(FILE *fp);

int getparms(char *plfile);
int getdata(int gno, char *fn, int src, int type);
int update_set_from_file(int gno, int setno, char *fn, int src);

int readblockdata(int gno, char *fn, FILE * fp);

int load_project_file(char *fn, int as_template);

int new_project(char *template);
int load_project(char *fn);
int save_project(char *fn);

int write_set(int gno, int setno, FILE *cp, char *format, int rawdata);
void outputset(int gno, int setno, char *fname, char *dformat);

int readnetcdf(int gno,
	       int setno,
	       char *netcdfname,
	       char *xvar,
	       char *yvar,
	       int nstart,
	       int nstop,
	       int nstride);
int write_netcdf(char *fname);

char *grace_fgets(char *s, int size, FILE *stream);


#endif /* __FILES_H_ */
