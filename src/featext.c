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
 * featext.c - routines to perform feature extraction on a set of curves
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>

#include "globals.h"
#include "utils.h"
#include "graphs.h"
#include "protos.h"
#include "motifinc.h"

typedef struct _Featext_ui {
    Widget top;
    ListStructure *tograph;
    OptionStructure *feature_item;
    Widget *xval_item;
    ListStructure *absic_graph;
    SetChoiceItem absic_set;
    Widget legload_rc;
} Featext_ui;

static Featext_ui feui;
static Widget but2[3];
 
void do_fext_proc( Widget, XtPointer, XtPointer );
double linear_interp( double, double, double, double, double );
int dbl_comp( const void *, const void * );

int getmedian( int grno, int setno, int sorton, double *median );
int get_zero_crossing( int setl, double *xv, double *yv, double *crossing );
int get_rise_time( int setl, double *xv, double *yv, double min, double max,
		double *width );
int get_fall_time( int setl, double *xv, double *yv, double min, double max,
		double *width );
int mute_linear_regression(int n, double *x, double *y, double *slope, 
			double *intercept);
int get_half_max_width(int len, double *x, double *y, double *width);
int get_barycenter( int n, double *x, double *y, double *barycenter );
void fext_routine( int gto, int feature, int abs_src, int abs_set, int abs_graph );
void get_max_pos( double *a, double *b, int n, double max, double *d );

void do_fext_toggle (Widget w, XtPointer client_data, XtPointer call_data)
{
    int value = (int) client_data;
    if (value == 2 || value == 3) {
    	SetSensitive(feui.legload_rc, True);
    } else {
    	SetSensitive(feui.legload_rc, False);
    }
}

void create_featext_frame(void *data)
{
    set_wait_cursor();
    
    if (feui.top == NULL) {
        Widget dialog;
	char *label2[2];
        int i;
        OptionItem feature_option_items[] = {
            { 0, "Y minimum"         },
            { 1, "Y maximum"         },
            { 2, "Y average"         },
            { 3, "Y std. dev."       },
            { 4, "Y median"          },
            { 5, "X minimum"         },
            { 6, "X maximum"         },
            { 7, "X average"         },
            { 8, "X std. dev."       },
            { 9, "X median"          },
            {10, "Frequency"         },
            {11, "Period"            },
            {12, "Zero crossing"     },
            {13, "Rise time"         },
            {14, "Fall time"         },
            {15, "Slope"             },
            {16, "Y intercept"       },
            {17, "Set length"        },
            {18, "Half maximal width"},
            {19, "Barycenter X"      },
            {20, "Barycenter Y"      },
            {21, "X(Y max)"          },
            {22, "Y(X max)"          },
            {23, "Integral"          }
        };

	label2[0] = "Accept";
	label2[1] = "Close";
	feui.top = XmCreateDialogShell(app_shell, "Feature Extraction", NULL, 0);
	handle_close(feui.top);
	dialog = XmCreateRowColumn(feui.top, "dialog_rc", NULL, 0);

	feui.tograph = CreateGraphChoice(dialog, "Results to graph:", LIST_TYPE_SINGLE);
	feui.feature_item =
            CreateOptionChoice(dialog, "Feature:", 3, 24, feature_option_items);
	feui.xval_item = CreatePanelChoice(dialog,
						"X values from:", 5,
						"Index", "Legends", "X from Set", "Y from set",
						NULL);
	
	for (i = 0; i < 4; i++) {
	    XtAddCallback(feui.xval_item[2 + i], XmNactivateCallback,
			(XtCallbackProc) do_fext_toggle, (XtPointer) i);
	}
	
	CreateSeparator(dialog);

	feui.legload_rc= XmCreateRowColumn(dialog, "fext_legload_rc", NULL, 0);
	
	feui.absic_graph = CreateGraphChoice(feui.legload_rc,
	    "Abscissa from graph:", LIST_TYPE_SINGLE);
	
	feui.absic_set = CreateSetSelector(feui.legload_rc, "set:",
					SET_SELECT_ACTIVE,
					FILTER_SELECT_NONE,
					0,
					SELECTION_TYPE_SINGLE);
	update_save_set_list( feui.absic_set, 0 );
	
	ManageChild(feui.legload_rc);
	SetSensitive(feui.legload_rc, False);
	CreateSeparator(dialog);

	CreateCommandButtons(dialog, 2, but2, label2);
	XtAddCallback(but2[0], XmNactivateCallback, (XtCallbackProc)
	do_fext_proc,(XtPointer) & feui);
	XtAddCallback(but2[1], XmNactivateCallback,
	(XtCallbackProc)destroy_dialog,(XtPointer)feui.top);

	ManageChild(dialog);
    }
    RaiseWindow(feui.top);
    unset_wait_cursor();
}


void do_fext_proc( Widget w, XtPointer client_data, XtPointer call_data )
{
    int gto, feature, abs_graph = -1, abs_set = -1, abs_src;

    Featext_ui *ui = (Featext_ui *) client_data;

    feature = GetOptionChoice(ui->feature_item);
    GetSingleListChoice(ui->tograph, &gto);
    if( gto == -1 )
            gto = get_cg();

    abs_src = (int) GetChoice(ui->xval_item);
    if( abs_src ==2 || abs_src==3 ) {
        abs_set = GetSelectedSet(ui->absic_set);
        GetSingleListChoice(ui->absic_graph, &abs_graph);
    }
    fext_routine( gto, feature, abs_src, abs_set, abs_graph ); 
    update_set_lists(gto);
    xdrawgraph();
}

void fext_routine( int gto, int feature, int abs_src, int abs_set, int abs_graph )
{
	int i, cs, ns, fts, ncurves, extract_err;
	double datum, dummy, *absy;
	double y1, y2;
	int iy1, iy2;
	char tbuf[1024];
	float *abscissa;
        double xmin, xmax, ymin, ymax;
        int cg = get_cg();
        double *x;

	abscissa = xmalloc(number_of_sets(cg)*SIZEOF_FLOAT);
	
	if( !is_graph_active( gto )	){
		errwin("Graph for results must be active");
	    return;
	}
	if( (ns=nextset( gto ) )== -1 ) {
		errwin("Choose a new graph or kill sets!");
	    return;
	}
	ncurves = nactive(get_cg());
	switch( abs_src ) {
		case 0:		/* use index */
			for( i=0; i<ncurves; i++ )
				abscissa[i] = i;
			break;	
		case 1:		/* use legend label */
			cs = 0;
			for( i=0; i<ncurves; i++ ){
				while( !is_set_active( get_cg(), cs ) )
					cs++;
				if(!sscanf( get_legend_string(get_cg(), cs), "%f", &abscissa[i]))
					break;
				cs++;
			}
			if( i != ncurves ) {
				errwin("Bad legend label");
				return;
			}
			break;
		case 2:		/* use X from set */
			if( !is_set_active( abs_graph, abs_set ) ){
	    		errwin("Abscissa set not active");
	    		return;
			}
			if( getsetlength( abs_graph, abs_set ) < ncurves ) {
				errwin("Not enough points in set");
				return;
			}
			absy = getx( abs_graph, abs_set );
			for( i=0; i<ncurves; i++ )
				abscissa[i] = absy[i];
			break;			
		case 3:										/* use Y from set */
			if( !is_set_active( abs_graph, abs_set ) ){
	    		errwin("Abscissa set not active");
	    		return;
			}
			if( getsetlength( abs_graph, abs_set ) < ncurves ) {
				errwin("Not enough points in set");
				return;
			}
			absy = gety( abs_graph, abs_set );
			for( i=0; i<ncurves; i++ )
				abscissa[i] = absy[i];
			break;
	}

	cs = 0;
	tbuf[0] = '\0';
	for( i=0; i<ncurves; i++ ) {
		while( !is_set_active( get_cg(), cs ) )
			cs++;
		extract_err = 0;
			
		getsetminmax(get_cg(), cs, &xmin, &xmax, &ymin, &ymax);
                switch( feature ) {
			case 0:			/* Y minimum */
				datum = ymin;		
				break;
			case 1: 		/* Y maximum */
				datum = ymax;		
				break;
			case 2: 		/* Y mean    */
				stasum(gety(cg, cs), getsetlength(cg, cs), &datum, &dummy);
				break;
			case 3:			/* Y std dev */
				stasum(gety(cg, cs), getsetlength(cg, cs), &dummy, &datum);
				break;
			case 4: 		/* Y median  */
				getmedian( cg, cs, DATA_Y, &datum );
				break;
			case 5:			/* X minimum */
				datum = xmin;		
				break;
			case 6: 		/* X maximum */
				datum = xmax;		
				break;
			case 7: 		/* X mean    */
				stasum(getx(cg, cs), getsetlength(cg, cs), &datum, &dummy);
				break;
			case 8:			/* X std dev */
				stasum(getx(cg, cs), getsetlength(cg, cs), &dummy, &datum);
				break;
			case 9:			/* X median  */
				getmedian( cg, cs, DATA_X, &datum );
				break;
			case 10: 		/* frequency and period */
			case 11:
				if ( ilog2(getsetlength(cg, cs)) <= 0)   	 /* only DFT */
					do_fourier(cg, cs, 0, 0, 1, 0, 0, 0);
				else		                                 /* FFT      */
					do_fourier(cg, cs, 1, 0, 1, 0, 0, 0);

				sprintf( tbuf, "FT of set %d", cs );
				fts = 0;
				while( strcmp( tbuf, getcomment(cg, fts)+1 ) )
					fts++;
					
				minmax(gety(cg, fts), getsetlength(cg, fts),&y1,&y2,&iy1,&iy2);
				x = getx(cg, fts);
				if( feature == 10 )
                                        datum = x[iy2];
				else
					datum = 1./x[iy2];
				killset( cg, fts );				/* get rid of Fourier set */
				break;
			case 12:		/* first zero crossing */
				if( get_zero_crossing( getsetlength( cg, cs ), 
									getx( cg, cs ),gety( cg, cs ), &datum ) ){
					sprintf( tbuf+strlen(tbuf), 
								"Unable to find zero crossing of set %d\n", cs );
					errwin( tbuf );
					extract_err = 1;
				}
				break;
			case 13:		/* rise time   */
				getsetminmax(cg, cs, &xmin, &xmax, &ymin, &ymax);
                                if( get_rise_time( getsetlength(cg,cs), getx(cg,cs), 
					gety(cg,cs), ymin, ymax, &datum ) ){
					sprintf( tbuf+strlen(tbuf), 
							"Unable to find rise time of set %d\n", cs );
					errwin( tbuf );
					extract_err = 1;
				}
				break;
			case 14: 		/* fall time   */
				getsetminmax(cg, cs, &xmin, &xmax, &ymin, &ymax);
                                if( get_fall_time( getsetlength(cg,cs), getx(cg,cs), 
					gety(cg,cs), ymin, ymax, &datum ) ){
					sprintf( tbuf+strlen(tbuf), 
									"Unable to find fall time of set %d\n", cs );
					extract_err = 1;
					errwin( tbuf );
				}
				break;
			case 15:		/* slope       */
				if( mute_linear_regression( getsetlength( cg, cs ), 
					getx( cg, cs ),gety( cg, cs ), &datum, &dummy ) ) {
					sprintf( tbuf+strlen(tbuf), 
										"Unable to find slope of set %d\n", cs );
					errwin( tbuf );
					extract_err = 1;
				}
				break;
			case 16:		/* Y intercept */
				if( mute_linear_regression( getsetlength( cg, cs ), 
						getx( cg, cs ), gety( cg, cs ), &dummy, &datum ) ) {
					sprintf( tbuf+strlen(tbuf), 
						"Unable to find y-intercept of set %d\n", cs );
					errwin( tbuf );
					extract_err = 1;
				}
				break;
			case 17:		/* set length  */
				datum = getsetlength( cg, cs );
				break;
			case 18:		/* half maximal widths */
                                if (get_half_max_width(getsetlength(cg, cs), getx(cg,cs), 
					   gety(cg,cs), &datum) != RETURN_SUCCESS) {
					sprintf( tbuf+strlen(tbuf), 
						"Unable to find half maximal width of set %d\n", cs );
					extract_err = 1;
					errwin( tbuf );
				}
				break;
			case 19:		/* Barycenter X */
				get_barycenter( getsetlength( cg, cs ), gety(cg,cs), 
									getx(cg,cs), &datum );
				break;
			case 20:		/* Barycenter Y */
				get_barycenter( getsetlength( cg, cs ), getx(cg,cs), 
									gety(cg,cs), &datum );
				break;
			case 21:		/* X of Maximum Y */
				getsetminmax(cg, cs, &xmin, &xmax, &ymin, &ymax);
                                get_max_pos( gety(cg, cs), getx( cg, cs ),
							getsetlength( cg, cs ), ymax, &datum ); 
				break;
			case 22:		/* Y of Maximum X */
				getsetminmax(cg, cs, &xmin, &xmax, &ymin, &ymax);
                                get_max_pos( getx(cg, cs), gety( cg, cs ),
							getsetlength( cg, cs ), xmax, &datum ); 
				break;
			case 23:		/* cumulative sum */
				datum = do_int(cg, cs, 1);
				break;
		}
		if( !extract_err )
			add_point(gto, ns, abscissa[i], datum);
		cs++;
	}

	/* set comment */	
	switch( feature ) {
		case 0:			/* Y minimum */
			sprintf(tbuf,"Y minima of graph %d",cg); 
			break;
		case 1: 		/* Y maximum */
			sprintf(tbuf,"Y maxima of graph %d",cg);
			break;
		case 2: 		/* Y mean    */
			sprintf(tbuf,"Y means of graph %d",cg);
			break;
		case 3:			/* Y std dev */
			sprintf(tbuf,"Y std. dev.'s of graph %d",cg);
			break;
		case 4:			/* Y median  */
			sprintf(tbuf,"Y medians of graph %d",cg);
			break;
		case 5:			/* X minimum */
			sprintf(tbuf,"X minima of graph %d",cg); 
			break;
		case 6: 		/* X maximum */
			sprintf(tbuf,"X maxima of graph %d",cg);
			break;
		case 7: 		/* X mean    */
			sprintf(tbuf,"X means of graph %d",cg);
			break;
		case 8:			/* X std dev */
			sprintf(tbuf,"X std. dev.'s of graph %d",cg);
			break;
		case 9:			/* X median  */
			sprintf(tbuf,"X medians of graph %d",cg);
			break;
		case 10: 		/* frequency and period */
			sprintf(tbuf,"frequencies of graph %d",cg);
			break;
		case 11:
			sprintf(tbuf,"periods of graph %d",cg);
			break;
		case 12:		/* first zero crossing */
			sprintf(tbuf,"zero crossings of graph %d",cg);
			break;
		case 13:		/* rise time */
			sprintf(tbuf,"rise times of graph %d",cg);
			break;
		case 14: 		/* fall time */
			sprintf(tbuf,"fall times of graph %d",cg);
			break;
		case 15: 		/* slopes     */
			sprintf(tbuf,"slopes of graph %d",cg);
			break;
		case 16: 		/* Y intercepts */
			sprintf(tbuf,"Y intercepts of graph %d",cg);
			break;
		case 17: 		/* set lengths */
			sprintf(tbuf,"set lengths of graph %d",cg);
			break;
		case 18: 		/* 1/2 maximal widths */
			sprintf(tbuf,"half maximal widths of graph %d",cg);
			break;
		case 19: 		/* barycenter X */
			sprintf(tbuf,"X barycenters of graph %d",cg);
			break;
		case 20: 		/* barycenter Y */
			sprintf(tbuf,"Y barycenters of graph %d",cg);
			break;
		case 21:		/* X of maximum Y */
			sprintf(tbuf,"X positions of maximum Y's of graph %d",cg);
			break;
		case 22:		/* Y of maximum X */
			sprintf(tbuf,"Y positions of maximum X's of graph %d",cg);
			break;
		case 23:		/* integral */
			sprintf(tbuf,"integrals of sets of graph %d",cg);
			break;
	}
	set_set_hidden(gto, ns, FALSE);
        setcomment( gto, ns, tbuf );
	xfree( abscissa );
}


/* linear regression without posting results to log */
int mute_linear_regression(int n, double *x, double *y, double *slope, 
			double *intercept)
{
    double xbar, ybar;		/* sample means */
    double sdx, sdy;		/* sample standard deviations */
    double SXX, SYY, SXY;	/* sums of squares */
    int i;

    if (n <= 3) {
		return 1;
    }
    xbar = ybar = 0.0;
    SXX = SYY = SXY = 0.0;
    for (i = 0; i < n; i++) {
		xbar = xbar + x[i];
		ybar = ybar + y[i];
    }
    xbar = xbar / n;
    ybar = ybar / n;
    for (i = 0; i < n; i++) {
		SXX = SXX + (x[i] - xbar) * (x[i] - xbar);
		SYY = SYY + (y[i] - ybar) * (y[i] - ybar);
		SXY = SXY + (x[i] - xbar) * (y[i] - ybar);
    }
    sdx = sqrt(SXX / (n - 1));
    sdy = sqrt(SYY / (n - 1));
    if (sdx == 0.0) {
		return 2;
    }
    if (sdy == 0.0) {
		return 2;
    }
    *slope = SXY / SXX;
    *intercept = ybar - *slope * xbar;
    return 0;
}

/*
 * assume graph starts off at ymin and rises to ymax 
 * Determine time to go from 10% to 90% of rise
 */
int get_rise_time( int setl, double *xv, double *yv, double min, double max,
		double *width )
{
	int x10=0, x90;
	double amp10, amp90;
	
	amp10 = min + (max-min)*0.1;
	amp90 = min + (max-min)*0.9;
	while( x10<setl && yv[x10]<amp10  )
		x10++;
	
	if( x10==setl || x10==0)
		return 1;
	
	x90 = x10+1;
	
	while( x90<setl && yv[x90]<amp90 )
		x90++;
	
	*width = linear_interp( yv[x90-1], xv[x90-1], yv[x90], xv[x90], amp90 ) -
			 linear_interp( yv[x10-1], xv[x10-1], yv[x10], xv[x10], amp10 );
	return 0;
}

/* assume graph starts off at ymax and drops to ymin 
   Determine time to go from 90% to 10% of drop			*/
int get_fall_time( int setl, double *xv, double *yv, double min, double max,
		double *width )
{
	int x10, x90=0;
	double amp10, amp90;
	
	amp10 = min + (max-min)*0.1;
	amp90 = min + (max-min)*0.9;
	while( x90<setl && yv[x90]>amp90 )
		x90++;
	
	if( x90==setl || x90==0)
		return 1;
	
	x10= x90+1;
	
	while( x10<setl && yv[x10]>amp10  )
		x10++;

	if( x10==setl )
		return 1;
	
	
	*width = linear_interp( yv[x10-1], xv[x10-1], yv[x10], xv[x10], amp10 )-
	         linear_interp( yv[x90-1], xv[x90-1], yv[x90], xv[x90], amp90 );
	return 0;
}


int get_zero_crossing( int setl, double *xv, double *yv, double *crossing )
{
	int i=0;
	
	while( i<setl && yv[i] != 0. && yv[i]*yv[i+1]>0. )
		i++;
	
	if( i==setl )
		return 1;
	
	if( yv[i] == 0 )
		*crossing = xv[i];
	else
		*crossing = linear_interp( yv[i], xv[i], yv[i+1], xv[i+1], 0 );

	return 0;
}


/* Get FWHM of the highest peak */
int get_half_max_width(int len, double *x, double *y, double *width)
{
    int i, imin, imax;
    double ymin, ymax, yavg;
    double x_u, x_d;

    minmax(y, len, &ymin, &ymax, &imin, &imax);
    yavg = (ymin + ymax)/2.0;
	
    i = imax;
    while (i >= 0 && y[i] > yavg) {
        i--;
    }
    if (i < 0) {
        return RETURN_FAILURE;
    }
    x_d = linear_interp(y[i], x[i], y[i + 1], x[i + 1], yavg);

    i = imax;
    while (i < len && y[i] > yavg) {
        i++;
    }
    if (i == len) {
        return RETURN_FAILURE;
    }
    x_u = linear_interp(y[i - 1], x[i - 1], y[i], x[i], yavg);

    *width = fabs(x_u - x_d);
    
    return RETURN_SUCCESS;
}

/* linear interpolate between two points, return a y value given an x */
double linear_interp( double x1, double y1, double x2, double y2, double x )
{
	return y1 + ( x-x1 )*(y2-y1)/(x2-x1);
}

/* get the median of the X or Y portion of a set */
int getmedian( int grno, int setno, int sorton, double *median )
{
	int setlen;
	double *setdata;
	
	setlen = getsetlength( get_cg(), setno );
	setdata = (double *)xmalloc( setlen*sizeof(double) );
	if( sorton == DATA_X )
		memcpy( setdata, getx( grno, setno ), setlen*sizeof(double) );
	else
		memcpy( setdata, gety( grno, setno ), setlen*sizeof(double) );
	
	qsort( setdata, setlen, sizeof(double), dbl_comp );
	
	if( setlen%2 )		/* odd set */
		*median = setdata[(setlen+1)/2-1];
	else
		*median = ( setdata[setlen/2-1] + setdata[setlen/2] )/2.;

	xfree( setdata );
	return 0;
}

int dbl_comp( const void *a, const void *b )
{
	if( *(double *)a < *(double *)b )
		return -1;
	else if( *(double *)a > *(double *)b )
		return 1;
	else
		return 0;
}

int get_barycenter( int n, double *x, double *y, double *barycenter )
{
	double wsum=0;
	
	*barycenter = 0;
	for( n--; n>=0; n-- ) {
		wsum += x[n]*y[n];
		*barycenter += x[n];
	}
	*barycenter = wsum/(*barycenter);
	return 0;
}

/* given maximum of set a, find the corresponding entry in set b */
void get_max_pos( double *a, double *b, int n, double max, double *d )
{
	int i=-1;
	
	while( ++i<n && a[i] != max  );
	
	if( i==n )
		return;
	else
		*d = b[i];
}
