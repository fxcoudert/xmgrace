/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-99 Grace Development Team
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
 * SS data
 *
 */
 
#ifndef __SSDATA_H_
#define __SSDATA_H_

#define FFORMAT_NUMBER  0
#define FFORMAT_STRING  1
#define FFORMAT_DATE    2

typedef struct _ss_data
{
    int ncols;
    int nrows;
    int *formats;
    void **data;
} ss_data;

double *copy_data_column(double *src, int nrows);
double *allocate_index_data(int nrows);
double *allocate_mesh(double start, double stop, int len);

void set_blockdata(ss_data *ssd);

int get_blockncols(void);
int get_blocknrows(void);
int *get_blockformats(void);

int realloc_ss_data(ss_data *ssd, int nrows);
void free_ss_data(ss_data *ssd);
int init_ss_data(ss_data *ssd, int ncols, int *formats);

int parse_ss_row(const char *s, int *nncols, int *nscols, int **formats);
int insert_data_row(ss_data *ssd, int row, char *s);
int store_data(ss_data *ssd, int load_type, char *label);

int create_set_fromblock(int gno, int setno,
    int type, int nc, int *coli, int scol, int autoscale);
char *cols_to_field_string(int nc, int *cols, int scol);
int field_string_to_cols(const char *fs, int *nc, int **cols, int *scol);

#endif /* __SSDATA_H_ */
