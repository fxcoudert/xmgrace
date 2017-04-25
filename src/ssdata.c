/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2002 Grace Development Team
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
 * spreadsheet data stuff
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "globals.h"
#include "utils.h"
#include "graphs.h"
#include "graphutils.h"
#include "files.h"
#include "ssdata.h"
#include "parser.h"

#include "protos.h"

double *copy_data_column(double *src, int nrows)
{
    double *dest;
    
    dest = xmalloc(nrows*SIZEOF_DOUBLE);
    if (dest != NULL) {
        memcpy(dest, src, nrows*SIZEOF_DOUBLE);
    }
    return dest;
}

char **copy_string_column(char **src, int nrows)
{
    char **dest;
    int i;

    dest = xmalloc(nrows*sizeof(char *));
    if (dest != NULL) {
        for (i = 0; i < nrows; i++)
            dest[i] =copy_string(NULL, src[i]);
    }
    return dest;
}

/* TODO: index_shift */
double *allocate_index_data(int nrows)
{
    int i;
    double *retval;
    
    retval = xmalloc(nrows*SIZEOF_DOUBLE);
    if (retval != NULL) {
        for (i = 0; i < nrows; i++) {
            retval[i] = i;
        }
    }
    return retval;
}

double *allocate_mesh(double start, double stop, int len)
{
    int i;
    double *retval;
    
    retval = xmalloc(len*SIZEOF_DOUBLE);
    if (retval != NULL) {
        double s = (start + stop)/2, d = (stop - start)/2;
        for (i = 0; i < len; i++) {
            retval[i] = s + d*((double) (2*i + 1 - len)/(len - 1));
        }
    }
    return retval;
}

static ss_data blockdata = {0, 0, NULL, NULL};

void set_blockdata(ss_data *ssd)
{
    free_ss_data(&blockdata);
    if (ssd) {
        memcpy(&blockdata, ssd, sizeof(ss_data));
    }
}

int get_blockncols(void)
{
    return blockdata.ncols;
}

int get_blocknrows(void)
{
    return blockdata.nrows;
}

int *get_blockformats(void)
{
    return blockdata.formats;
}

int realloc_ss_data(ss_data *ssd, int nrows)
{
    int i, j;
    char  **sp;
    
    for (i = 0; i < ssd->ncols; i++) {
        if (ssd->formats[i] == FFORMAT_STRING) {
            sp = (char **) ssd->data[i];
            for (j = nrows; j < ssd->nrows; j++) {
                XCFREE(sp[j]);
            }
            ssd->data[i] = xrealloc(ssd->data[i], nrows*sizeof(char *));
            sp = (char **) ssd->data[i];
            for (j = ssd->nrows; j < nrows; j++) {
                sp[j] = NULL;
            }
        } else {
            ssd->data[i] = xrealloc(ssd->data[i], nrows*SIZEOF_DOUBLE);
        }
    }
    ssd->nrows = nrows;
    
    return RETURN_SUCCESS;
}

void free_ss_data(ss_data *ssd)
{
    if (ssd) {
        int i, j;
        char  **sp;

        for (i = 0; i < ssd->ncols; i++) {
            if (ssd->formats && ssd->formats[i] == FFORMAT_STRING) {
                sp = (char **) ssd->data[i];
                for (j = 0; j < ssd->nrows; j++) {
                    XCFREE(sp[j]);
                }
            }
            XCFREE(ssd->data[i]);
        }
        XCFREE(ssd->data);
        XCFREE(ssd->formats);
        ssd->nrows = 0;
        ssd->ncols = 0;
    }
}

int init_ss_data(ss_data *ssd, int ncols, int *formats)
{
    int i;
    
    ssd->data = xmalloc(ncols*SIZEOF_VOID_P);
    for (i = 0; i < ncols; i++) {
        ssd->data[i] = NULL;
    }
    ssd->formats = xmalloc(ncols*SIZEOF_INT);
    memcpy(ssd->formats, formats, ncols*SIZEOF_INT);
    ssd->ncols = ncols;
    ssd->nrows = 0;

    return RETURN_SUCCESS;
}

static char *next_token(char *s, char **token, int *quoted)
{
    *quoted = FALSE;
    *token = NULL;
    
    if (s == NULL) {
        return NULL;
    }
    
    while (*s == ' ' || *s == '\t') {
        s++;
    }
    if (*s == '"') {
        s++;
        *token = s;
        while (*s != '\0' && (*s != '"' || (*s == '"' && *(s - 1) == '\\'))) {
            s++;
        }
        if (*s == '"') {
            /* successfully identified a quoted string */
            *quoted = TRUE;
        }
    } else {
        *token = s;
        if (**token == '\n') {
            /* EOL reached */
            return NULL;
        }
        while (*s != '\n' && *s != '\0' && *s != ' ' && *s != '\t') {
            s++;
        }
    }
    
    if (*s != '\0') {
        *s = '\0';
        s++;
        return s;
    } else {
        return NULL;
    }
}

int parse_ss_row(const char *s, int *nncols, int *nscols, int **formats)
{
    int ncols;
    int quoted;
    char *buf, *s1, *token;
    double value;
    Dates_format df_pref, ddummy;
    const char *sdummy;

    *nscols = 0;
    *nncols = 0;
    *formats = NULL;
    df_pref = get_date_hint();
    buf = copy_string(NULL, s);
    s1 = buf;
    while ((s1 = next_token(s1, &token, &quoted)) != NULL) {
        if (token == NULL) {
            *nscols = 0;
            *nncols = 0;
            XCFREE(*formats);
            xfree(buf);
            return RETURN_FAILURE;
        }
        
        ncols = *nncols + *nscols;
        /* reallocate the formats array */
        if (ncols % 10 == 0) {
            *formats = xrealloc(*formats, (ncols + 10)*SIZEOF_INT);
        }

        if (quoted) {
            (*formats)[ncols] = FFORMAT_STRING;
            (*nscols)++;
        } else if (parse_date(token, df_pref, FALSE, &value, &ddummy) ==
            RETURN_SUCCESS) {
            (*formats)[ncols] = FFORMAT_DATE;
            (*nncols)++;
        } else if (parse_float(token, &value, &sdummy) == RETURN_SUCCESS) {
            (*formats)[ncols] = FFORMAT_NUMBER;
            (*nncols)++;
        } else {
            /* last resort - treat the field as string, even if not quoted */
            (*formats)[ncols] = FFORMAT_STRING;
            (*nscols)++;
        }
    }
    xfree(buf);
    
    return RETURN_SUCCESS;
}


/* NOTE: the input string will be corrupted! */
int insert_data_row(ss_data *ssd, int row, char *s)
{
    int i, j;
    int ncols = ssd->ncols;
    char *token;
    int quoted;
    char  **sp;
    double *np;
    Dates_format df_pref, ddummy;
    const char *sdummy;
    int res;
    
    df_pref = get_date_hint();
    for (i = 0; i < ncols; i++) {
        s = next_token(s, &token, &quoted);
        if (s == NULL || token == NULL) {
            /* invalid line: free the already allocated string fields */
            for (j = 0; j < i; j++) {
                if (ssd->formats[j] == FFORMAT_STRING) {
                    sp = (char **) ssd->data[j];
                    XCFREE(sp[row]);
                }
            }
            return RETURN_FAILURE;
        } else {
            if (ssd->formats[i] == FFORMAT_STRING) {
                sp = (char **) ssd->data[i];
                sp[row] = copy_string(NULL, token);
                if (sp[row] != NULL) {
                    res = RETURN_SUCCESS;
                } else {
                    res = RETURN_FAILURE;
                }
            } else if (ssd->formats[i] == FFORMAT_DATE) {
                np = (double *) ssd->data[i];
                res = parse_date(token, df_pref, FALSE, &np[row], &ddummy);
            } else {
                np = (double *) ssd->data[i];
                res = parse_float(token, &np[row], &sdummy);
            }
            if (res != RETURN_SUCCESS) {
                for (j = 0; j < i; j++) {
                    if (ssd->formats[j] == FFORMAT_STRING) {
                        sp = (char **) ssd->data[j];
                        XCFREE(sp[row]);
                    }
                }
                return RETURN_FAILURE;
            }
        }
    }
    
    return RETURN_SUCCESS;
}


int store_data(ss_data *ssd, int load_type, char *label)
{
    int ncols, nncols, nncols_req, nscols, nrows;
    int i, j;
    double *xdata;
    int gno, setno;
    int x_from_index;
    
    if (ssd == NULL) {
        return RETURN_FAILURE;
    }
    ncols = ssd->ncols;
    nrows = ssd->nrows;
    if (ncols <= 0 || nrows <= 0) {
        return RETURN_FAILURE;
    }

    nncols = 0;
    for (j = 0; j < ncols; j++) {
        if (ssd->formats[j] != FFORMAT_STRING) {
            nncols++;
        }
    }
    nscols = ncols - nncols;
    
    gno = get_parser_gno();
    if (is_valid_gno(gno) != TRUE) {
        return RETURN_FAILURE;
    }
    
    switch (load_type) {
    case LOAD_SINGLE:
        if (nscols > 1) {
            errmsg("Can not use more than one column of strings per set");
            free_ss_data(ssd);
            return RETURN_FAILURE;
        }

        nncols_req = settype_cols(curtype);
        x_from_index = FALSE;
        if (nncols_req == nncols + 1) {
            x_from_index = TRUE;
        } else if (nncols_req != nncols) {
	    errmsg("Column count incorrect");
	    return RETURN_FAILURE;
        }

        setno = nextset(gno);
        set_dataset_type(gno, setno, curtype);

        nncols = 0;
        if (x_from_index) {
            xdata = allocate_index_data(nrows);
            if (xdata == NULL) {
                free_ss_data(ssd);
            }
            setcol(gno, setno, nncols, xdata, nrows);
            nncols++;
        }
        for (j = 0; j < ncols; j++) {
            if (ssd->formats[j] == FFORMAT_STRING) {
                set_set_strings(gno, setno, nrows, (char **) ssd->data[j]);
            } else {
                setcol(gno, setno, nncols, (double *) ssd->data[j], nrows);
                nncols++;
            }
        }
        if (!strlen(getcomment(gno, setno))) {
            setcomment(gno, setno, label);
        }
        
        XCFREE(ssd->data);
        XCFREE(ssd->formats);
        break;
    case LOAD_NXY:
        if (nscols != 0) {
            errmsg("Can not yet use strings when reading in data as NXY");
            free_ss_data(ssd);
            return RETURN_FAILURE;
        }
        
        for (i = 0; i < ncols - 1; i++) {
            setno = nextset(gno);
            if (setno == -1) {
                free_ss_data(ssd);
                return RETURN_FAILURE;
            }
            if (i > 0) {
                xdata = copy_data_column((double *) ssd->data[0], nrows);
                if (xdata == NULL) {
                    free_ss_data(ssd);
                }
            } else {
                xdata = (double *) ssd->data[0];
            }
            set_dataset_type(gno, setno, SET_XY);
            setcol(gno, setno, DATA_X, xdata, nrows);
            setcol(gno, setno, DATA_Y, (double *) ssd->data[i + 1], nrows);
            setcomment(gno, setno, label);
        }
    
        XCFREE(ssd->data);
        XCFREE(ssd->formats);
        break;
    case LOAD_BLOCK:
        set_blockdata(ssd);
        break;
    default:
        errmsg("Internal error");
        free_ss_data(ssd);
        return RETURN_FAILURE;
    }
    
    return RETURN_SUCCESS;
}

int field_string_to_cols(const char *fs, int *nc, int **cols, int *scol)
{
    int col;
    char *s, *buf;

    buf = copy_string(NULL, fs);
    if (buf == NULL) {
        return RETURN_FAILURE;
    }

    s = buf;
    *nc = 0;
    while ((s = strtok(s, ":")) != NULL) {
	(*nc)++;
	s = NULL;
    }
    *cols = xmalloc((*nc)*SIZEOF_INT);
    if (*cols == NULL) {
        xfree(buf);
        return RETURN_FAILURE;
    }

    strcpy(buf, fs);
    s = buf;
    *nc = 0;
    *scol = -1;
    while ((s = strtok(s, ":")) != NULL) {
        int strcol;
        if (*s == '{') {
            char *s1;
            strcol = TRUE;
            s++;
            if ((s1 = strchr(s, '}')) != NULL) {
                *s1 = '\0';
            }
        } else {
            strcol = FALSE;
        }
        col = atoi(s);
        col--;
        if (strcol) {
            *scol = col;
        } else {
            (*cols)[*nc] = col;
	    (*nc)++;
        }
	s = NULL;
    }
    
    xfree(buf);
    
    return RETURN_SUCCESS;
}

char *cols_to_field_string(int nc, int *cols, int scol)
{
    int i;
    char *s, buf[32];
    
    s = NULL;
    for (i = 0; i < nc; i++) {
        sprintf(buf, "%d", cols[i] + 1);
        if (i != 0) {
            s = concat_strings(s, ":");
        }
        s = concat_strings(s, buf);
    }
    if (scol >= 0) {
        sprintf(buf, ":{%d}", scol + 1);
        s = concat_strings(s, buf);
    }
    
    return s;
}

int create_set_fromblock(int gno, int setno,
    int type, int nc, int *coli, int scol, int autoscale)
{
    int i, ncols, blockncols, blocklen, column;
    double *cdata;
    char buf[256], *s;

    blockncols = get_blockncols();
    if (blockncols <= 0) {
        errmsg("No block data read");
        return RETURN_FAILURE;
    }

    blocklen = get_blocknrows();
    
    ncols = settype_cols(type);
    if (nc > ncols) {
        errmsg("Too many columns scanned in column string");
        return RETURN_FAILURE;
    }
    if (nc < ncols) {
	errmsg("Too few columns scanned in column string");
	return RETURN_FAILURE;
    }
    
    for (i = 0; i < nc; i++) {
	if (coli[i] < -1 || coli[i] >= blockncols) {
	    errmsg("Column index out of range");
	    return RETURN_FAILURE;
	}
    }
    
    if (scol >= blockncols) {
	errmsg("String column index out of range");
	return RETURN_FAILURE;
    }
    
    if (setno == NEW_SET) {
        setno = nextset(gno);
        if (setno == -1) {
            return RETURN_FAILURE;
        }
    }
    
    /* clear data stored in the set, if any */
    killsetdata(gno, setno);
    
    if (activateset(gno, setno) != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }
    
    set_dataset_type(gno, setno, type);

    for (i = 0; i < nc; i++) {
        column = coli[i];
        if (column == -1) {
            cdata = allocate_index_data(blocklen);
        } else {
            if (blockdata.formats[column] != FFORMAT_STRING) {
                cdata = copy_data_column((double *) blockdata.data[column], blocklen);
            } else {
                errmsg("Tried to read doubles from strings!");
                killsetdata(gno, setno);
                return RETURN_FAILURE;
            }
        }
        if (cdata == NULL) {
            killsetdata(gno, setno);
            return RETURN_FAILURE;
        }
        setcol(gno, setno, i, cdata, blocklen);
    }

    /* strings, if any */
    if (scol >= 0) {
        if (blockdata.formats[scol] != FFORMAT_STRING) {
            errmsg("Tried to read strings from doubles!");
            killsetdata(gno, setno);
            return RETURN_FAILURE;
        } else {
            set_set_strings(gno, setno, blocklen,
                copy_string_column((char **) blockdata.data[scol], blocklen));
        }
    }

    s = cols_to_field_string(nc, coli, scol);
    sprintf(buf, "Cols %s", s);
    xfree(s);
    setcomment(gno, setno, buf);

    autoscale_graph(gno, autoscale);

    return RETURN_SUCCESS;
}
