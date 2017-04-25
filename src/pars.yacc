%{
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
 * evaluate expressions, commands, parameter files
 * 
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#if defined(HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif

/* bison not always handles it well itself */
#if defined(HAVE_ALLOCA_H)
#  include <alloca.h>
#endif

#include "defines.h"
#include "globals.h"
#include "cephes/cephes.h"
#include "device.h"
#include "utils.h"
#include "files.h"
#include "graphs.h"
#include "graphutils.h"
#include "plotone.h"
#include "dlmodule.h"
#include "t1fonts.h"
#include "ssdata.h"
#include "protos.h"
#include "parser.h"
#include "mathstuff.h"

#define MAX_PARS_STRING_LENGTH  4096

#define CAST_DBL_TO_BOOL(x) (fabs(x) < 0.5 ? 0:1)

typedef double (*ParserFnc)();

extern graph *g;

static double  s_result;    /* return value if a scalar expression is scanned*/
static grarr *v_result;    /* return value if a vector expression is scanned*/

static int expr_parsed, vexpr_parsed;

static int interr;

static grarr freelist[100]; 	/* temporary vectors */
static int fcnt = 0;		/* number of the temporary vectors allocated */

static target trgt_pool[100]; 	/* pool of temporary targets */
static int tgtn = 0;		/* number of the temporary targets used */

int naxis = 0;	/* current axis */
static int curline, curbox, curellipse, curstring;
/* these guys attempt to avoid reentrancy problems */
static int gotparams = FALSE, gotread = FALSE, gotnlfit = FALSE; 
int readxformat;
static int nlfit_gno, nlfit_setno, nlfit_nsteps;
static double *nlfit_warray = NULL;

char batchfile[GR_MAXPATHLEN] = "",
     paramfile[GR_MAXPATHLEN] = "",
     readfile[GR_MAXPATHLEN] = "";

static char f_string[MAX_PARS_STRING_LENGTH]; /* buffer for string to parse */
static int pos;

/* the graph, set, and its length of the parser's current state */
static int whichgraph;
static int whichset;

/* the graph and set of the left part of a vector assignment */
static int vasgn_gno;
static int vasgn_setno;

static int alias_force = FALSE; /* controls whether aliases can override
                                                       existing keywords */

extern char print_file[];
extern char *close_input;

static int filltype_obs;

static int index_shift = 0;     /* 0 for C, 1 for F77 index notation */

static void free_tmpvrbl(grarr *vrbl);
static void copy_vrbl(grarr *dest, grarr *src);
static int find_set_bydata(double *data, target *tgt);

static int getcharstr(void);
static void ungetchstr(void);
static int follow(int expect, int ifyes, int ifno);

static int yylex(void);
static int yyparse(void);
static void yyerror(char *s);

static int findf(symtab_entry *keytable, char *s);

/* Total (intrinsic + user-defined) list of functions and keywords */
symtab_entry *key;

%}

%union {
    int     ival;
    double  dval;
    char   *sval;
    double *dptr;
    target *trgt;
    grarr  *vrbl;
}

%token KEY_VAR       
%token KEY_VEC       

%token KEY_CONST     
%token KEY_UNIT      
%token KEY_FUNC_I    
%token KEY_FUNC_D    
%token KEY_FUNC_NN   
%token KEY_FUNC_ND   
%token KEY_FUNC_DD   
%token KEY_FUNC_NND  
%token KEY_FUNC_PPD  
%token KEY_FUNC_PPPD 
%token KEY_FUNC_PPPPD 
%token KEY_FUNC_PPPPPD 

%token <ival> INDEX
%token <ival> DATE

%token <dptr> VAR_D	 /* a (pointer to) double variable                                     */
%token <vrbl> VEC_D	 /* a (pointer to) double array variable                                     */

%token <ival> CONSTANT	 /* a (double) constant                                     */
%token <ival> UCONSTANT	 /* a (double) unit constant                                */
%token <ival> FUNC_I	 /* a function of 1 int variable                            */
%token <ival> FUNC_D	 /* a function of 1 double variable                         */
%token <ival> FUNC_NN    /* a function of 2 int parameters                          */
%token <ival> FUNC_ND    /* a function of 1 int parameter and 1 double variable     */
%token <ival> FUNC_DD    /* a function of 2 double variables                        */
%token <ival> FUNC_NND   /* a function of 2 int parameters and 1 double variable    */
%token <ival> FUNC_PPD   /* a function of 2 double parameters and 1 double variable */
%token <ival> FUNC_PPPD  /* a function of 3 double parameters and 1 double variable */
%token <ival> FUNC_PPPPD /* a function of 4 double parameters and 1 double variable */
%token <ival> FUNC_PPPPPD/* a function of 5 double parameters and 1 double variable */

%token <ival> ABOVE
%token <ival> ABSOLUTE
%token <ival> ALIAS
%token <ival> ALT
%token <ival> ALTXAXIS
%token <ival> ALTYAXIS
%token <ival> ANGLE
%token <ival> ANTIALIASING
%token <ival> APPEND
%token <ival> ARRANGE
%token <ival> ARROW
%token <ival> ASCENDING
%token <ival> ASPLINE
%token <ival> AUTO
%token <ival> AUTOSCALE
%token <ival> AUTOTICKS
%token <ival> AVALUE
%token <ival> AVG
%token <ival> BACKGROUND
%token <ival> BAR
%token <ival> BARDY
%token <ival> BARDYDY
%token <ival> BASELINE
%token <ival> BATCH
%token <ival> BEGIN
%token <ival> BELOW
%token <ival> BETWEEN
%token <ival> BLACKMAN
%token <ival> BLOCK
%token <ival> BOTH
%token <ival> BOTTOM
%token <ival> BOX
%token <ival> CD
%token <ival> CENTER
%token <ival> CHAR
%token <ival> CHART
%token <sval> CHRSTR
%token <ival> CLEAR
%token <ival> CLICK
%token <ival> CLIP
%token <ival> CLOSE
%token <ival> COEFFICIENTS
%token <ival> COLOR
%token <ival> COMMENT
%token <ival> COMPLEX
%token <ival> COMPUTING
%token <ival> CONSTRAINTS
%token <ival> COPY
%token <ival> CYCLE
%token <ival> DAYMONTH
%token <ival> DAYOFWEEKL
%token <ival> DAYOFWEEKS
%token <ival> DAYOFYEAR
%token <ival> DDMMYY
%token <ival> DECIMAL
%token <ival> DEF
%token <ival> DEFAULT
%token <ival> DEFINE
%token <ival> DEGREESLAT
%token <ival> DEGREESLON
%token <ival> DEGREESMMLAT
%token <ival> DEGREESMMLON
%token <ival> DEGREESMMSSLAT
%token <ival> DEGREESMMSSLON
%token <ival> DESCENDING
%token <ival> DESCRIPTION
%token <ival> DEVICE
%token <ival> DFT
%token <ival> DIFFERENCE
%token <ival> DISK
%token <ival> DOWN
%token <ival> DPI
%token <ival> DROP
%token <ival> DROPLINE
%token <ival> ECHO
%token <ival> ELLIPSE
%token <ival> ENGINEERING
%token <ival> ERRORBAR
%token <ival> EXIT
%token <ival> EXPONENTIAL
%token <ival> FFT
%token <ival> FILEP
%token <ival> FILL
%token <ival> FIT
%token <ival> FIXED
%token <ival> FIXEDPOINT
%token <ival> FLUSH
%token <ival> FOCUS
%token <ival> FOLLOWS
%token <ival> FONTP
%token <ival> FORCE
%token <ival> FORMAT
%token <ival> FORMULA
%token <ival> FRAMEP
%token <ival> FREE
%token <ival> FREQUENCY
%token <ival> FROM
%token <ival> GENERAL
%token <ival> GETP
%token <ival> GRAPH
%token <ival> GRAPHNO
%token <ival> GRID
%token <ival> HAMMING
%token <ival> HANNING
%token <ival> HARDCOPY
%token <ival> HBAR
%token <ival> HELP
%token <ival> HGAP
%token <ival> HIDDEN
%token <ival> HISTOGRAM
%token <ival> HMS
%token <ival> HORIZI
%token <ival> HORIZONTAL
%token <ival> HORIZO
%token <ival> ID
%token <ival> IFILTER
%token <ival> IMAX
%token <ival> IMIN
%token <ival> IN
%token <ival> INCREMENT
%token <ival> INOUT
%token <ival> INT
%token <ival> INTEGRATE
%token <ival> INTERPOLATE
%token <ival> INVDFT
%token <ival> INVERT
%token <ival> INVFFT
%token <ival> JUST
%token <ival> KILL
%token <ival> LABEL
%token <ival> LANDSCAPE
%token <ival> LAYOUT
%token <ival> LEFT
%token <ival> LEGEND
%token <ival> LENGTH
%token <ival> LINCONV
%token <ival> LINE
%token <ival> LINEAR
%token <ival> LINESTYLE
%token <ival> LINEWIDTH
%token <ival> LINK
%token <ival> LOAD
%token <ival> LOCTYPE
%token <ival> LOG
%token <ival> LOGARITHMIC
%token <ival> LOGIT
%token <ival> LOGX
%token <ival> LOGXY
%token <ival> LOGY
%token <ival> MAGIC
%token <ival> MAGNITUDE
%token <ival> MAJOR
%token <ival> MAP
%token <ival> MAXP
%token <ival> MESH
%token <ival> MINP
%token <ival> MINOR
%token <ival> MMDD
%token <ival> MMDDHMS
%token <ival> MMDDYY
%token <ival> MMDDYYHMS
%token <ival> MMSSLAT
%token <ival> MMSSLON
%token <ival> MMYY
%token <ival> MONTHDAY
%token <ival> MONTHL
%token <ival> MONTHS
%token <ival> MONTHSY
%token <ival> MOVE
%token <ival> NEGATE
%token <ival> NEW
%token <ival> NONE
%token <ival> NONLFIT
%token <ival> NORMAL
%token <ival> NXY
%token <ival> OFF
%token <ival> OFFSET
%token <ival> OFFSETX
%token <ival> OFFSETY
%token <ival> OFILTER
%token <ival> ON
%token <ival> ONREAD
%token <ival> OP
%token <ival> OPPOSITE
%token <ival> OUT
%token <ival> PAGE
%token <ival> PARA
%token <ival> PARAMETERS
%token <ival> PARZEN
%token <ival> PATTERN
%token <ival> PERIOD
%token <ival> PERP
%token <ival> PHASE
%token <ival> PIE
%token <ival> PIPE
%token <ival> PLACE
%token <ival> POINT
%token <ival> POLAR
%token <ival> POLYI
%token <ival> POLYO
%token <ival> POP
%token <ival> PORTRAIT
%token <ival> POWER
%token <ival> PREC
%token <ival> PREPEND
%token <ival> PRINT
%token <ival> PS
%token <ival> PUSH
%token <ival> PUTP
%token <ival> RAND
%token <ival> READ
%token <ival> REAL
%token <ival> RECIPROCAL
%token <ival> REDRAW
%token <ival> REFERENCE
%token <ival> REGNUM
%token <ival> REGRESS
%token <ival> RESIZE
%token <ival> RESTRICT
%token <ival> REVERSE
%token <ival> RIGHT
%token <ival> RISER
%token <ival> ROT
%token <ival> ROUNDED
%token <ival> RSUM
%token <ival> RULE
%token <ival> RUNAVG
%token <ival> RUNMAX
%token <ival> RUNMED
%token <ival> RUNMIN
%token <ival> RUNSTD
%token <ival> SAVEALL
%token <ival> SCALE
%token <ival> SCIENTIFIC
%token <ival> SCROLL
%token <ival> SD
%token <ival> SET
%token <ival> SETNUM
%token <ival> SFORMAT
%token <ival> SIGN
%token <ival> SIZE
%token <ival> SKIP
%token <ival> SLEEP
%token <ival> SMITH 
%token <ival> SORT
%token <ival> SOURCE
%token <ival> SPEC
%token <ival> SPLINE
%token <ival> SPLIT
%token <ival> STACK
%token <ival> STACKED
%token <ival> STACKEDBAR
%token <ival> STACKEDHBAR
%token <ival> STAGGER
%token <ival> START
%token <ival> STOP
%token <ival> STRING
%token <ival> SUM
%token <ival> SUBTITLE
%token <ival> SWAP
%token <ival> SYMBOL
%token <ival> TARGET
%token <ival> TICKLABEL
%token <ival> TICKP
%token <ival> TICKSP
%token <ival> TIMER
%token <ival> TIMESTAMP
%token <ival> TITLE
%token <ival> TO
%token <ival> TOP
%token <ival> TRIANGULAR
%token <ival> TYPE
%token <ival> UP
%token <ival> UPDATEALL
%token <ival> USE
%token <ival> VERSION
%token <ival> VERTI
%token <ival> VERTICAL
%token <ival> VERTO
%token <ival> VGAP
%token <ival> VIEW
%token <ival> VX1
%token <ival> VX2
%token <ival> VXMAX
%token <ival> VY1
%token <ival> VY2
%token <ival> VYMAX
%token <ival> WELCH
%token <ival> WITH
%token <ival> WORLD
%token <ival> WRAP
%token <ival> WRITE
%token <ival> WX1
%token <ival> WX2
%token <ival> WY1
%token <ival> WY2
%token <ival> X_TOK
%token <ival> X0
%token <ival> X1
%token <ival> XAXES
%token <ival> XAXIS
%token <ival> XCOR
%token <ival> XMAX
%token <ival> XMIN
%token <ival> XY
%token <ival> XYAXES
%token <ival> XYBOXPLOT
%token <ival> XYCOLOR
%token <ival> XYCOLPAT
%token <ival> XYDX
%token <ival> XYDXDX
%token <ival> XYDXDXDYDY
%token <ival> XYDXDY
%token <ival> XYDY
%token <ival> XYDYDY
%token <ival> XYHILO
%token <ival> XYR
%token <ival> XYSIZE
%token <ival> XYSTRING
%token <ival> XYVMAP
%token <ival> XYZ
%token <ival> Y_TOK
%token <ival> Y0
%token <ival> Y1
%token <ival> Y2
%token <ival> Y3
%token <ival> Y4
%token <ival> YAXES
%token <ival> YAXIS
%token <ival> YEAR
%token <ival> YMAX
%token <ival> YMIN
%token <ival> YYMMDD
%token <ival> YYMMDDHMS
%token <ival> ZERO
%token <ival> ZNORM

%token <ival> FITPARM
%token <ival> FITPMAX
%token <ival> FITPMIN
%token <dval> NUMBER

%token <sval> NEW_TOKEN

%type <ival> onoff

%type <ival> selectgraph
%type <trgt> selectset

%type <ival> pagelayout
%type <ival> pageorient

%type <ival> regiontype

%type <ival> color_select
%type <ival> pattern_select
%type <ival> font_select

%type <ival> lines_select
%type <dval> linew_select

%type <ival> graphtype
%type <ival> xytype

%type <ival> scaletype
%type <ival> signchoice

%type <ival> colpat_obs
%type <ival> direction

%type <ival> formatchoice
%type <ival> inoutchoice
%type <ival> justchoice

%type <ival> opchoice
%type <ival> opchoice_sel
%type <ival> opchoice_obs
%type <ival> opchoice_sel_obs

%type <ival> worldview

%type <ival> filtermethod
%type <ival> filtertype

%type <ival> tickspectype

%type <ival> sourcetype

%type <ival> interpmethod
%type <ival> stattype

%type <ival> datacolumn

%type <ival> runtype

%type <ival> ffttype
%type <ival> fourierdata
%type <ival> fourierloadx
%type <ival> fourierloady
%type <ival> windowtype

%type <ival> nonlfitopts

%type <ival> sortdir
%type <ival> sorton

%type <ival> proctype

%type <ival> indx
%type <ival> iexpr
%type <ival> nexpr
%type <sval> sexpr
%type <dval> jdate
%type <dval> jrawdate
%type <dval> expr

%type <vrbl> array
%type <vrbl> lside_array

%type <vrbl> vexpr

/* Precedence */
%nonassoc '?' ':'
%left OR
%left AND
%nonassoc GT LT LE GE EQ NE
%right UCONSTANT
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS NOT	/* negation--unary minus */
%right '^'		/* exponentiation        */
%left '.'


%%

full_list:
        multi_list
        | expr {
            expr_parsed = TRUE;
            s_result = $1;
        }
        | vexpr {
            vexpr_parsed = TRUE;
            v_result = $1;
        }
        ;

multi_list:
        list
        | multi_list ';' list
        ;

list:
	/* empty */
	| parmset {}
	| parmset_obs {}
	| regionset {}
	| setaxis {}
	| set_setprop {}
	| actions {}
	| options {}
	| asgn {}
	| vasgn {}
	| defines {}
	| error {
	    return 1;
	}
	;



expr:	NUMBER {
	    $$ = $1;
	}
	|  VAR_D {
	    $$ = *($1);
	}
	|  FITPARM {
	    $$ = nonl_parms[$1].value;
	}
	|  FITPMAX {
	    $$ = nonl_parms[$1].max;
	}
	|  FITPMIN {
	    $$ = nonl_parms[$1].min;
	}
	|  array indx {
            if ($2 >= $1->length) {
                errmsg("Access beyond array bounds");
                return 1;
            }
            $$ = $1->data[$2];
	}
	| stattype '(' vexpr ')' {
	    double dummy, dummy2;
            int idummy, ind, length = $3->length;
	    if ($3->data == NULL) {
		yyerror("NULL variable, check set type");
		return 1;
	    }
	    switch ($1) {
	    case MINP:
		$$ = vmin($3->data, length);
		break;
	    case MAXP:
		$$ = vmax($3->data, length);
		break;
            case AVG:
		stasum($3->data, length, &$$, &dummy);
                break;
            case SD:
		stasum($3->data, length, &dummy, &$$);
                break;
            case SUM:
		stasum($3->data, length, &$$, &dummy);
                $$ *= length;
                break;
            case IMIN:
		minmax($3->data, length, &dummy, &dummy2, &ind, &idummy);
                $$ = (double) ind;
                break;
            case IMAX:
		minmax($3->data, length, &dummy, &dummy2, &idummy, &ind);
                $$ = (double) ind;
                break;
	    }
	}
	| INT '(' vexpr ',' vexpr ')' {
	    if ($3->length != $5->length) {
		yyerror("X and Y are of different length");
		return 1;
            } else {
                $$ = trapint($3->data, $5->data, NULL, NULL, $3->length);
            }
	}
	| array '.' LENGTH {
	    $$ = $1->length;
	}
	| selectset '.' LENGTH {
	    $$ = getsetlength($1->gno, $1->setno);
	}
	| selectset '.' ID {
	    $$ = $1->setno;
	}
	| selectgraph '.' ID {
	    $$ = $1;
	}
	| CONSTANT
	{
            $$ = ((ParserFnc) (key[$1].data)) ();
	}
	| expr UCONSTANT
	{
	    $$ = $1 * ((ParserFnc) (key[$2].data)) ();
	}
	| RAND
	{
	    $$ = drand48();
	}
	| FUNC_I '(' iexpr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3);
	}
	| FUNC_D '(' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3);
	}
	| FUNC_ND '(' iexpr ',' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5);
	}
	| FUNC_NN '(' iexpr ',' iexpr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5);
	}
	| FUNC_DD '(' expr ',' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5);
	}
	| FUNC_NND '(' iexpr ',' iexpr ',' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5, $7);
	}
	| FUNC_PPD '(' expr ',' expr ',' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5, $7);
	}
	| FUNC_PPPD '(' expr ',' expr ',' expr ',' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5, $7, $9);
	}
	| FUNC_PPPPD '(' expr ',' expr ',' expr ',' expr ',' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5, $7, $9, $11);
	}
	| FUNC_PPPPPD '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
	{
	    $$ = ((ParserFnc) (key[$1].data)) ($3, $5, $7, $9, $11, $13);
	}
	| selectgraph '.' VX1 {
	    $$ = g[$1].v.xv1;
	}
	| selectgraph '.' VX2 {
	    $$ = g[$1].v.xv2;
	}
	| selectgraph '.' VY1 {
	    $$ = g[$1].v.yv1;
	}
	| selectgraph '.' VY2 {
	    $$ = g[$1].v.yv2;
	}
	| selectgraph '.' WX1 {
	    $$ = g[$1].w.xg1;
	}
	| selectgraph '.' WX2 {
	    $$ = g[$1].w.xg2;
	}
	| selectgraph '.' WY1 {
	    $$ = g[$1].w.yg1;
	}
	| selectgraph '.' WY2 {
	    $$ = g[$1].w.yg2;
	}
	| DATE '(' jdate ')' {
            $$ = $3;
	}
	| DATE '(' iexpr ',' nexpr ',' nexpr ')' { /* yr, mo, day */
	    $$ = cal_and_time_to_jul($3, $5, $7, 12, 0, 0.0);
	}
	| DATE '(' iexpr ',' nexpr ',' nexpr ',' nexpr ',' nexpr ',' expr ')' 
	{ /* yr, mo, day, hr, min, sec */
	    $$ = cal_and_time_to_jul($3, $5, $7, $9, $11, $13);
	}
	| VX1 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            $$ = g[whichgraph].v.xv1;
	}
	| VX2 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    $$ = g[whichgraph].v.xv2;
	}
	| VY1 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    $$ = g[whichgraph].v.yv1;
	}
	| VY2 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    $$ = g[whichgraph].v.yv2;
	}
	| WX1 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    $$ = g[whichgraph].w.xg1;
	}
	| WX2 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    $$ = g[whichgraph].w.xg2;
	}
	| WY1 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    $$ = g[whichgraph].w.yg1;
	}
	| WY2 {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    $$ = g[whichgraph].w.yg2;
	}
	| VXMAX {
	    double vx, vy;
            get_page_viewport(&vx, &vy);
            $$ = vx;
	}
	| VYMAX {
	    double vx, vy;
            get_page_viewport(&vx, &vy);
            $$ = vy;
	}
	| '(' expr ')' {
	    $$ = $2;
	}
	| expr '+' expr {
	    $$ = $1 + $3;
	}
	| expr '-' expr {
	    $$ = $1 - $3;
	}
	| '-' expr %prec UMINUS {
	    $$ = -$2;
	}
	| '+' expr %prec UMINUS {
	    $$ = $2;
	}
	| expr '*' expr {
	    $$ = $1 * $3;
	}
	| expr '/' expr
	{
	    if ($3 != 0.0) {
		$$ = $1 / $3;
	    } else {
		yyerror("Divide by zero");
		return 1;
	    }
	}
	| expr '%' expr {
	    if ($3 != 0.0) {
		$$ = fmod($1, $3);
	    } else {
		yyerror("Divide by zero");
		return 1;
	    }
	}
	| expr '^' expr {
	    if ($1 < 0 && rint($3) != $3) {
		yyerror("Negative value raised to non-integer power");
		return 1;
            } else if ($1 == 0.0 && $3 <= 0.0) {
		yyerror("Zero raised to non-positive power");
		return 1;
            } else {
                $$ = pow($1, $3);
            }
	}
	| expr '?' expr ':' expr {
	    $$ = $1 ? $3 : $5;
	}
	| expr GT expr {
	   $$ = ($1 > $3);
	}
	| expr LT expr  {
	   $$ = ($1 < $3);
	}
	| expr LE expr {
	   $$ = ($1 <= $3);
	}
	| expr GE expr {
	   $$ = ($1 >= $3);
	}
	| expr EQ expr {
	   $$ = ($1 == $3);
	}
	| expr NE expr {
	    $$ = ($1 != $3);
	}
	| expr AND expr {
	    $$ = $1 && $3;
	}
	| expr OR expr {
	    $$ = $1 || $3;
	}
	| NOT expr {
	    $$ = !($2);
	}
	;

sexpr:	CHRSTR {
            $$ = $1;
	}
        | sexpr '.' sexpr {
            $$ = concat_strings($1, $3);
            xfree($3);
        }
        | sexpr '.' expr {
            char buf[32];
            set_locale_num(TRUE);
            sprintf(buf, "%g", $3);
            set_locale_num(FALSE);
            $$ = concat_strings($1, buf);
        }
        ;

iexpr:  expr {
	    int itmp = rint($1);
            if (fabs(itmp - $1) > 1.e-6) {
		yyerror("Non-integer value supplied for integer");
		return 1;
            }
            $$ = itmp;
        }
        ;

nexpr:	iexpr {
            if ($1 < 0) {
		yyerror("Negative value supplied for non-negative");
		return 1;
            }
            $$ = $1;
	}
        ;

indx:	'[' iexpr ']' {
	    int itmp = $2 - index_shift;
            if (itmp < 0) {
		yyerror("Negative index");
		return 1;
            }
            $$ = itmp;
	}
        ;

jdate:  expr {
            $$ = $1;
        }
        | sexpr {
            double jul;
            Dates_format dummy;
            if (parse_date($1, get_date_hint(), FALSE, &jul, &dummy)
                == RETURN_SUCCESS) {
                xfree($1);
                $$ = jul;
            } else {
                xfree($1);
		yyerror("Invalid date");
		return 1;
            }
        }
        ;

jrawdate:  expr {
            $$ = $1;
        }
        | sexpr {
            double jul;
            Dates_format dummy;
            if (parse_date($1, get_date_hint(), TRUE, &jul, &dummy)
                == RETURN_SUCCESS) {
                xfree($1);
                $$ = jul;
            } else {
                xfree($1);
		yyerror("Invalid date");
		return 1;
            }
        }
        ;

array:
	VEC_D
	{
            $$ = $1;
	}
        | datacolumn
	{
	    double *ptr = getcol(vasgn_gno, vasgn_setno, $1);
            $$ = &freelist[fcnt++];
            $$->type = GRARR_SET;
            $$->data = ptr;
            if (ptr == NULL) {
                errmsg("NULL variable - check set type");
                return 1;
            } else {
                $$->length = getsetlength(vasgn_gno, vasgn_setno);
            }
	}
	| selectset '.' datacolumn
	{
	    double *ptr = getcol($1->gno, $1->setno, $3);
            $$ = &freelist[fcnt++];
            $$->type = GRARR_SET;
            $$->data = ptr;
            if (ptr == NULL) {
                errmsg("NULL variable - check set type");
                return 1;
            } else {
                $$->length = getsetlength($1->gno, $1->setno);
            }
	}
        ;
        
vexpr:
	array
	{
            $$ = $1;
	}
	| array '[' iexpr ':' iexpr ']'
	{
            int start = $3 - index_shift, stop = $5 - index_shift;
            if (start < 0 || stop < start || stop >= $1->length) {
		yyerror("Invalid index range");
            } else {
                int len = stop - start + 1;
	        double *ptr = xmalloc(len*SIZEOF_DOUBLE);
                if ($$->data == NULL) {
                    yyerror("Not enough memory");
                } else {
                    int i;
                    $$ = &freelist[fcnt++];
	            $$->data = ptr;
                    $$->length = len;
                    $$->type = GRARR_TMP;
                    for (i = 0; i < len; i++) {
                        $$->data[i] = $1->data[i + $3];
                    }
                }
            }
	}
	| MESH '(' nexpr ')'
	{
            int len = $3;
            if (len < 1) {
                yyerror("npoints must be > 0");
            } else {
                double *ptr = allocate_index_data(len);
                if (ptr == NULL) {
                    errmsg("Malloc failed");
                    return 1;
                } else {
                    $$ = &freelist[fcnt++];
                    $$->type = GRARR_TMP;
                    $$->data = ptr;
                    $$->length = len;
                }
            }
	}
	| MESH '(' expr ',' expr ',' nexpr ')'
	{
            int len = $7;
            if (len < 2) {
                yyerror("npoints must be > 1");
            } else {
                double *ptr = allocate_mesh($3, $5, len);
                if (ptr == NULL) {
                    errmsg("Malloc failed");
                    return 1;
                } else {
                    $$ = &freelist[fcnt++];
                    $$->type = GRARR_TMP;
                    $$->data = ptr;
                    $$->length = len;
                }
            }
	}
	| RAND '(' nexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    $$->data = xmalloc($3*SIZEOF_DOUBLE);
            if ($$->data == NULL) {
                errmsg("Not enough memory");
                return 1;
            } else {
                $$->length = $3;
                $$->type = GRARR_TMP;
            }
            for (i = 0; i < $$->length; i++) {
		$$->data[i] = drand48();
	    }
	}
	| REGNUM '(' selectset ')'
	{
	    int rtype, i, len;
            char *rarray;
            
            rtype = RESTRICT_REG0 + $1;
            
	    if (get_restriction_array($3->gno, $3->setno,
                rtype, FALSE, &rarray) != RETURN_SUCCESS) {
                errmsg("Error in region evaluation");
                return 1;
	    }

            len = getsetlength($3->gno, $3->setno);
            $$ = &freelist[fcnt++];
	    $$->data = xmalloc(len*SIZEOF_DOUBLE);
            if ($$->data == NULL) {
                errmsg("Not enough memory");
                return 1;
            } else {
                $$->length = len;
                $$->type = GRARR_TMP;
            }
            for (i = 0; i < $$->length; i++) {
		$$->data[i] = rarray[i];
	    }
            
            xfree(rarray);
	}
	| RSUM '(' vexpr ')'
	{
            int i;
            $$ = &freelist[fcnt++];
            copy_vrbl($$, $3);
            $$->type = GRARR_TMP;
            for (i = 1; i < $$->length; i++) {
                $$->data[i] += $$->data[i - 1];
            }
	}
	| FUNC_I '(' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;
	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ((int) ($3->data[i]));
	    }
	}
	| FUNC_D '(' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;
	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) (($3->data[i]));
	    }
	}
	| FUNC_DD '(' vexpr ',' vexpr ')'
	{
	    int i;
	    if ($3->length != $5->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;
            
	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3->data[i], $5->data[i]);
	    }
	}
	| FUNC_DD '(' expr ',' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $5);
            $$->type = GRARR_TMP;
            
	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3, $5->data[i]);
	    }
	}
	| FUNC_DD '(' vexpr ',' expr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;
            
	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3->data[i], $5);
	    }
	}
	| FUNC_ND '(' iexpr ',' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $5);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3, $5->data[i]);
	    }
	}
	| FUNC_NND '(' iexpr ',' iexpr ',' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $7);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3, $5, $7->data[i]);
	    }
	}
	| FUNC_PPD '(' expr ',' expr ',' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $7);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3, $5, $7->data[i]);
	    }
	}
	| FUNC_PPPD '(' expr ',' expr ',' expr ',' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $9);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3, $5, $7, $9->data[i]);
	    }
	}
	| FUNC_PPPPD '(' expr ',' expr ',' expr ',' expr ',' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $11);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3, $5, $7, $9, $11->data[i]);
	    }
	}
	| FUNC_PPPPPD '(' expr ',' expr ',' expr ',' expr ',' expr ',' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $13);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ((ParserFnc) (key[$1].data)) ($3, $5, $7, $9, $11, $13->data[i]);
	    }
	}
	| vexpr '+' vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] + $3->data[i];
	    }
	}
	| vexpr '+' expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] + $3;
	    }
	}
	| expr '+' vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1 + $3->data[i];
	    }
	}
	| vexpr '-' vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] - $3->data[i];
	    }
	}
	| vexpr '-' expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] - $3;
	    }
	}
	| expr '-' vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1 - $3->data[i];
	    }
	}
	| vexpr '*' vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] * $3->data[i];
	    }
	}
	| vexpr '*' expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] * $3;
	    }
	}
	| expr '*' vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1 * $3->data[i];
	    }
	}
	| vexpr '/' vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		if ($3->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                }
                $$->data[i] = $1->data[i] / $3->data[i];
	    }
	}
	| vexpr '/' expr
	{
	    int i;
	    if ($3 == 0.0) {
                errmsg("Divide by zero");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] / $3;
	    }
	}
	| expr '/' vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		if ($3->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                }
		$$->data[i] = $1 / $3->data[i];
	    }
	}
	| vexpr '%' vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		if ($3->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                } else {
                    $$->data[i] = fmod($1->data[i], $3->data[i]);
                }
	    }
	}
	| vexpr '%' expr
	{
	    int i;
	    if ($3 == 0.0) {
                errmsg("Divide by zero");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = fmod($1->data[i], $3);
	    }
	}
	| expr '%' vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		if ($3->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                } else {
		    $$->data[i] = fmod($1, $3->data[i]);
                }
	    }
	}
	| vexpr '^' vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
	        if ($1->data[i] < 0 && rint($3->data[i]) != $3->data[i]) {
	            yyerror("Negative value raised to non-integer power");
	            return 1;
                } else if ($1->data[i] == 0.0 && $3->data[i] <= 0.0) {
	            yyerror("Zero raised to non-positive power");
	            return 1;
                } else {
                    $$->data[i] = pow($1->data[i], $3->data[i]);
                }
	    }
	}
	| vexpr '^' expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
	        if ($1->data[i] < 0 && rint($3) != $3) {
	            yyerror("Negative value raised to non-integer power");
	            return 1;
                } else if ($1->data[i] == 0.0 && $3 <= 0.0) {
	            yyerror("Zero raised to non-positive power");
	            return 1;
                } else {
                    $$->data[i] = pow($1->data[i], $3);
                }
	    }
	}
	| expr '^' vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
	        if ($1 < 0 && rint($3->data[i]) != $3->data[i]) {
	            yyerror("Negative value raised to non-integer power");
	            return 1;
                } else if ($1 == 0.0 && $3->data[i] <= 0.0) {
	            yyerror("Zero raised to non-positive power");
	            return 1;
                } else {
                    $$->data[i] = pow($1, $3->data[i]);
                }
	    }
	}
	| vexpr UCONSTANT
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;
	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] * ((ParserFnc) (key[$2].data)) ();
	    }
	}
	| vexpr '?' expr ':' expr {
            int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;
            for (i = 0; i < $$->length; i++) { 
                $$->data[i] = CAST_DBL_TO_BOOL($1->data[i]) ? $3 : $5;
            }
	}
	| vexpr '?' expr ':' vexpr {
            int i;
	    if ($1->length != $5->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;
            for (i = 0; i < $$->length; i++) { 
                $$->data[i] = CAST_DBL_TO_BOOL($1->data[i]) ? $3 : $5->data[i];
            }
	}
	| vexpr '?' vexpr ':' expr {
            int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;
            for (i = 0; i < $$->length; i++) { 
                $$->data[i] = CAST_DBL_TO_BOOL($1->data[i]) ? $3->data[i] : $5;
            }
	}
	| vexpr '?' vexpr ':' vexpr {
            int i;
	    if ($1->length != $5->length || $1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;
            for (i = 0; i < $$->length; i++) { 
                $$->data[i] = CAST_DBL_TO_BOOL($1->data[i]) ? $3->data[i] : $5->data[i];
            }
	}
	| vexpr OR vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] || $3->data[i];
	    }
	}
	| vexpr OR expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] || $3;
	    }
	}
	| expr OR vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1 || $3->data[i];
	    }
	}
	| vexpr AND vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] && $3->data[i];
	    }
	}
	| vexpr AND expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1->data[i] && $3;
	    }
	}
	| expr AND vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = $1 && $3->data[i];
	    }
	}
	| vexpr GT vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] > $3->data[i]);
	    }
	}
	| vexpr GT expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] > $3);
	    }
	}
	| expr GT vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1 > $3->data[i]);
	    }
	}
	| vexpr LT vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] < $3->data[i]);
	    }
	}
	| vexpr LT expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] < $3);
	    }
	}
	| expr LT vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1 < $3->data[i]);
	    }
	}
	| vexpr GE vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] >= $3->data[i]);
	    }
	}
	| vexpr GE expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] >= $3);
	    }
	}
	| expr GE vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1 >= $3->data[i]);
	    }
	}
	| vexpr LE vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] <= $3->data[i]);
	    }
	}
	| vexpr LE expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] <= $3);
	    }
	}
	| expr LE vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1 <= $3->data[i]);
	    }
	}
	| vexpr EQ vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] == $3->data[i]);
	    }
	}
	| vexpr EQ expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] == $3);
	    }
	}
	| expr EQ vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1 == $3->data[i]);
	    }
	}
	| vexpr NE vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] != $3->data[i]);
	    }
	}
	| vexpr NE expr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $1);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1->data[i] != $3);
	    }
	}
	| expr NE vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $3);
            $$->type = GRARR_TMP;

	    for (i = 0; i < $$->length; i++) {
		$$->data[i] = ($1 != $3->data[i]);
	    }
	}
	| NOT vexpr
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $2);
            $$->type = GRARR_TMP;
            for (i = 0; i < $$->length; i++) { 
                $$->data[i] = !$2->data[i];
            }
	}
	| '(' vexpr ')'
	{
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $2);
            $$->type = GRARR_TMP;
            for (i = 0; i < $$->length; i++) { 
                $$->data[i] = $2->data[i];
            }
	}
	| '-' vexpr %prec UMINUS {
	    int i;
            $$ = &freelist[fcnt++];
	    copy_vrbl($$, $2);
            $$->type = GRARR_TMP;
            for (i = 0; i < $$->length; i++) { 
                $$->data[i] = - $2->data[i];
            }
	}
	;


asgn:
	VAR_D '=' expr
	{
	    *($1) = $3;
	}
	| FITPARM '=' expr
	{
	    nonl_parms[$1].value = $3;
	}
	| FITPMAX '=' expr
	{
	    nonl_parms[$1].max = $3;
	}
	| FITPMIN '=' expr
	{
	    nonl_parms[$1].min = $3;
	}
	| array indx '=' expr
	{
	    if ($2 >= $1->length) {
		yyerror("Access beyond array bounds");
		return 1;
            }
            $1->data[$2] = $4;
	}
	;

lside_array:
        array
        {
            target tgt;
            switch ($1->type) {
            case GRARR_SET:
                if (find_set_bydata($1->data, &tgt) == RETURN_SUCCESS) {
                    vasgn_gno   = tgt.gno;
                    vasgn_setno = tgt.setno;
                } else {
                    errmsg("Internal error");
		    return 1;
                }
                break;
            case GRARR_VEC:
                vasgn_gno   = -1;
                vasgn_setno = -1;
                break;
            default:
                /* It can NOT be a tmp array on the left side! */
                errmsg("Internal error");
	        return 1;
            }
            $$ = $1;
        }
        ;

vasgn:
	lside_array '=' vexpr
	{
	    int i;
	    if ($1->length != $3->length) {
                errmsg("Left and right vectors are of different lengths");
                return 1;
            }
	    for (i = 0; i < $1->length; i++) {
	        $1->data[i] = $3->data[i];
	    }
	}
	| lside_array '=' expr
	{
	    int i;
	    for (i = 0; i < $1->length; i++) {
	        $1->data[i] = $3;
	    }
	}
        ;

defines:
	DEFINE NEW_TOKEN
        {
	    symtab_entry tmpkey;
            double *var;
            
            var = xmalloc(SIZEOF_DOUBLE);
            *var = 0.0;
            
	    tmpkey.s = $2;
	    tmpkey.type = KEY_VAR;
	    tmpkey.data = (void *) var;
	    if (addto_symtab(tmpkey) != RETURN_SUCCESS) {
	        yyerror("Adding new symbol failed");
	    }

            xfree($2);
        }
	| DEFINE NEW_TOKEN '[' ']'
        {
	    if (define_parser_arr($2) == NULL) {
	        yyerror("Adding new symbol failed");
	    }

            xfree($2);
        }
	| DEFINE NEW_TOKEN '[' nexpr ']'
        {
	    grarr *var;
            if ((var = define_parser_arr($2)) == NULL) {
	        yyerror("Adding new symbol failed");
	    } else {
                realloc_vrbl(var, $4);
            }

            xfree($2);
        }
	| DEFINE VAR_D
        {
            yyerror("Keyword already exists");
        }
	| DEFINE VEC_D
        {
            yyerror("Keyword already exists");
        }
	| CLEAR VAR_D
        {
            undefine_parser_var((void *) $2);
            xfree($2);
        }
	| CLEAR VEC_D
        {
            realloc_vrbl($2, 0);
            undefine_parser_var((void *) $2);
            xfree($2);
        }
	| ALIAS sexpr sexpr {
	    int position;

	    lowtoupper($3);
	    if ((position = findf(key, $3)) >= 0) {
	        symtab_entry tmpkey;
		tmpkey.s = $2;
		tmpkey.type = key[position].type;
		tmpkey.data = key[position].data;
		if (addto_symtab(tmpkey) != RETURN_SUCCESS) {
		    yyerror("Keyword already exists");
		}
	    } else {
	        yyerror("Aliased keyword not found");
	    }
	    xfree($2);
	    xfree($3);
	}
	| ALIAS FORCE onoff {
	    alias_force = $3;
	}
	| USE sexpr TYPE proctype FROM sexpr {
	    if (load_module($6, $2, $2, $4) != 0) {
	        yyerror("DL module load failed");
	    }
	    xfree($2);
	    xfree($6);
	}
	| USE sexpr TYPE proctype FROM sexpr ALIAS sexpr {
	    if (load_module($6, $2, $8, $4) != 0) {
	        yyerror("DL module load failed");
	    }
	    xfree($2);
	    xfree($6);
	    xfree($8);
	}
        ;

regionset:
	REGNUM onoff {
	    rg[$1].active = $2;
	}
	| REGNUM TYPE regiontype {
	    rg[$1].type = $3;
	}
	| REGNUM color_select {
	    rg[$1].color = $2;
	}
	| REGNUM lines_select {
	    rg[$1].lines = $2;
	}
	| REGNUM linew_select {
	    rg[$1].linew = $2;
	}
	| REGNUM LINE expr ',' expr ',' expr ',' expr
	{
	    rg[$1].x1 = $3;
	    rg[$1].y1 = $5;
	    rg[$1].x2 = $7;
	    rg[$1].y2 = $9;
	}
	| REGNUM XY expr ',' expr
	{
	    rg[$1].x = xrealloc(rg[$1].x, (rg[$1].n + 1) * SIZEOF_DOUBLE);
	    rg[$1].y = xrealloc(rg[$1].y, (rg[$1].n + 1) * SIZEOF_DOUBLE);
	    rg[$1].x[rg[$1].n] = $3;
	    rg[$1].y[rg[$1].n] = $5;
	    rg[$1].n++;
	}
	| LINK REGNUM TO selectgraph {
	    rg[$2].linkto = $4;
	}
	;


parmset:
        VERSION nexpr {
            if (set_project_version($2) != RETURN_SUCCESS) {
                errmsg("Project version is newer than software!");
            }
            if (get_project_version() < 50001) {
                map_fonts(FONT_MAP_ACEGR);
            } else {
                map_fonts(FONT_MAP_DEFAULT);
            }
        }
        | PAGE RESIZE nexpr ',' nexpr {
            set_page_dimensions($3, $5, TRUE);
        }
        | PAGE SIZE nexpr ',' nexpr {
            set_page_dimensions($3, $5, FALSE);
        }
	| DEVICE sexpr PAGE SIZE nexpr ',' nexpr {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name($2);
            xfree($2);
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.pg.width =  (long) ($5*dev.pg.dpi/72);
                dev.pg.height = (long) ($7*dev.pg.dpi/72);
                set_device_props(device_id, dev);
            }
        }
        | DEVICE sexpr DPI expr {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name($2);
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.pg.dpi = $4;
                set_device_props(device_id, dev);
            }
            xfree($2);
        }
        | DEVICE sexpr FONTP ANTIALIASING onoff {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name($2);
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.fontaa = $5;
                set_device_props(device_id, dev);
            }
            xfree($2);
        }
        | DEVICE sexpr FONTP onoff {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name($2);
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.devfonts = $4;
                set_device_props(device_id, dev);
            }
            xfree($2);
        }
        | DEVICE sexpr OP sexpr {
            int device_id;
            
            device_id = get_device_by_name($2);
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                if (parse_device_options(device_id, $4) != 
                                                        RETURN_SUCCESS) {
                    yyerror("Incorrect device option string");
                }
            }
            xfree($2);
            xfree($4);
        }
        | HARDCOPY DEVICE sexpr {
            set_printer_by_name($3);
            xfree($3);
        }
        | REFERENCE DATE jrawdate {
            set_ref_date($3);
	}
        | DATE WRAP onoff {
            allow_two_digits_years($3);
	}
        | DATE WRAP YEAR iexpr {
            set_wrap_year($4);
	}
	| BACKGROUND color_select {
	    setbgcolor($2);
	}
	| PAGE BACKGROUND FILL onoff {
	    setbgfill($4);
	}
	| PAGE SCROLL expr '%' {
	    scroll_proc((int) $3);
	}
	| PAGE INOUT expr '%' {
	    scrollinout_proc((int) $3);
	}
	| LINK PAGE onoff {
	    scrolling_islinked = $3;
	}

	| STACK WORLD expr ',' expr ',' expr ',' expr
	{
	    add_world(whichgraph, $3, $5, $7, $9);
	}

	| TIMER nexpr {
            timer_delay = $2;
	}

	| TARGET selectset {
	    target_set = *($2);
	    set_parser_setno(target_set.gno, target_set.setno);
	}
	| WITH selectgraph {
	    set_parser_gno($2);
	}
	| WITH selectset {
	    set_parser_setno($2->gno, $2->setno);
	}

/* Hot links */
	| selectset LINK sourcetype sexpr {
	    set_hotlink($1->gno, $1->setno, 1, $4, $3);
	    xfree($4);
	}
	| selectset LINK onoff {
	    set_hotlink($1->gno, $1->setno, $3, NULL, 0);
	}

/* boxes */
	| WITH BOX {
	    curbox = next_box();
	}
	| WITH BOX nexpr {
            int no = $3;
            if (is_valid_box(no) ||
                realloc_boxes(no + 1) == RETURN_SUCCESS) {
                curbox = no;
            }
	}
	| BOX onoff {
	    if (!is_valid_box(curbox)) {
                yyerror("Box not active");
	    } else {
	        boxes[curbox].active = $2;
            }
	}
	| BOX selectgraph {
	    if (!is_valid_box(curbox)) {
                yyerror("Box not active");
	    } else {
	        boxes[curbox].gno = $2;
            }
	}
	| BOX expr ',' expr ',' expr ',' expr {
	    if (!is_valid_box(curbox)) {
                yyerror("Box not active");
	    } else {
		boxes[curbox].x1 = $2;
		boxes[curbox].y1 = $4;
		boxes[curbox].x2 = $6;
		boxes[curbox].y2 = $8;
	    }
	}
	| BOX LOCTYPE worldview {
	    box_loctype = $3;
	}
	| BOX lines_select {
	    box_lines = $2;
	}
	| BOX linew_select {
	    box_linew = $2;
	}
	| BOX color_select {
	    box_color = $2;
	}
	| BOX FILL color_select {
	    box_fillcolor = $3;
	}
	| BOX FILL pattern_select {
	    box_fillpat = $3;
	}
	| BOX DEF {
	    if (!is_valid_box(curbox)) {
                yyerror("Box not active");
	    } else {
		boxes[curbox].lines = box_lines;
		boxes[curbox].linew = box_linew;
		boxes[curbox].color = box_color;
		if (get_project_version() <= 40102) {
                    switch (filltype_obs) {
                    case COLOR:
                        boxes[curbox].fillcolor = box_fillcolor;
		        boxes[curbox].fillpattern = 1;
                        break;
                    case PATTERN:
                        boxes[curbox].fillcolor = 1;
		        boxes[curbox].fillpattern = box_fillpat;
                        break;
                    default: /* NONE */
                        boxes[curbox].fillcolor = box_fillcolor;
		        boxes[curbox].fillpattern = 0;
                        break;
                    }
		} else {
                    boxes[curbox].fillcolor = box_fillcolor;
		    boxes[curbox].fillpattern = box_fillpat;
                }
                boxes[curbox].loctype = box_loctype;
	    }
	}

/* ellipses */
	| WITH ELLIPSE {
		curellipse = next_ellipse();
	}
	| WITH ELLIPSE nexpr {
            int no = $3;
            if (is_valid_ellipse(no) ||
                realloc_ellipses(no + 1) == RETURN_SUCCESS) {
                curellipse = no;
            }
	}
	| ELLIPSE onoff {
	    if (!is_valid_ellipse(curellipse)) {
                yyerror("Ellipse not active");
	    } else {
	        ellip[curellipse].active = $2;
            }
	}
	| ELLIPSE selectgraph {
	    if (!is_valid_ellipse(curellipse)) {
                yyerror("Ellipse not active");
	    } else {
	        ellip[curellipse].gno = $2;
            }
	}
	| ELLIPSE expr ',' expr ',' expr ',' expr {
	    if (!is_valid_ellipse(curellipse)) {
                yyerror("Ellipse not active");
	    } else {
		ellip[curellipse].x1 = $2;
		ellip[curellipse].y1 = $4;
		ellip[curellipse].x2 = $6;
		ellip[curellipse].y2 = $8;
	    }
	}
	| ELLIPSE LOCTYPE worldview {
	    ellipse_loctype = $3;
	}
	| ELLIPSE lines_select {
	    ellipse_lines = $2;
	}
	| ELLIPSE linew_select {
	    ellipse_linew = $2;
	}
	| ELLIPSE color_select {
	    ellipse_color = $2;
	}
	| ELLIPSE FILL color_select {
	    ellipse_fillcolor = $3;
	}
	| ELLIPSE FILL pattern_select {
	    ellipse_fillpat = $3;
	}
	| ELLIPSE DEF {
	    if (!is_valid_ellipse(curellipse)) {
                yyerror("Ellipse not active");
	    } else {
		ellip[curellipse].lines = ellipse_lines;
		ellip[curellipse].linew = ellipse_linew;
		ellip[curellipse].color = ellipse_color;
		if (get_project_version() <= 40102) {
                    switch (filltype_obs) {
                    case COLOR:
                        ellip[curellipse].fillcolor = ellipse_fillcolor;
		        ellip[curellipse].fillpattern = 1;
                        break;
                    case PATTERN:
                        ellip[curellipse].fillcolor = 1;
		        ellip[curellipse].fillpattern = ellipse_fillpat;
                        break;
                    default: /* NONE */
                        ellip[curellipse].fillcolor = ellipse_fillcolor;
		        ellip[curellipse].fillpattern = 0;
                        break;
                    }
		} else {
                    ellip[curellipse].fillcolor = ellipse_fillcolor;
		    ellip[curellipse].fillpattern = ellipse_fillpat;
                }
		ellip[curellipse].loctype = ellipse_loctype;
	    }
	}

/* lines */
	| WITH LINE {
	    curline = next_line();
	}
	| WITH LINE nexpr {
            int no = $3;
            if (is_valid_line(no) ||
                realloc_lines(no + 1) == RETURN_SUCCESS) {
                curline = no;
            }
	}
	| LINE onoff {
	    if (!is_valid_line(curline)) {
                yyerror("Line not active");
	    } else {
	        lines[curline].active = $2;
            }
	}
	| LINE selectgraph {
	    if (!is_valid_line(curline)) {
                yyerror("Line not active");
	    } else {
	        lines[curline].gno = $2;
            }
	}
	| LINE expr ',' expr ',' expr ',' expr {
	    if (!is_valid_line(curline)) {
                yyerror("Line not active");
	    } else {
	        lines[curline].x1 = $2;
	        lines[curline].y1 = $4;
	        lines[curline].x2 = $6;
	        lines[curline].y2 = $8;
            }
	}
	| LINE LOCTYPE worldview {
	    line_loctype = $3;
	}
	| LINE linew_select {
	    line_linew = $2;
	}
	| LINE lines_select {
	    line_lines = $2;
	}
	| LINE color_select {
	    line_color = $2;
	}
	| LINE ARROW nexpr {
	    line_arrow_end = $3;
	}
	| LINE ARROW LENGTH expr {
	    line_asize = $4;
	}
	| LINE ARROW TYPE nexpr {
	    line_atype = $4;
	}
	| LINE ARROW LAYOUT expr ',' expr {
	    line_a_dL_ff = $4;
	    line_a_lL_ff = $6;
	}
	| LINE DEF {
	    if (!is_valid_line(curline)) {
                yyerror("Line not active");
	    } else {
	        lines[curline].lines = line_lines;
	        lines[curline].linew = line_linew;
	        lines[curline].color = line_color;
	        lines[curline].arrow_end = line_arrow_end;
	        lines[curline].arrow.length = line_asize;
	        lines[curline].arrow.type = line_atype;
	        lines[curline].arrow.dL_ff = line_a_dL_ff;
	        lines[curline].arrow.lL_ff = line_a_lL_ff;
	        lines[curline].loctype = line_loctype;
            }
	}

/* strings */
	| WITH STRING {
            curstring = next_string();
        }
	| WITH STRING nexpr {
            int no = $3;
            if (is_valid_string(no) ||
                realloc_strings(no + 1) == RETURN_SUCCESS) {
                curstring = no;
            }
        }
	| STRING onoff {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
                pstr[curstring].active = $2;
            }
        }
	| STRING selectgraph {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
                pstr[curstring].gno = $2;
            }
        }
	| STRING expr ',' expr {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
	        pstr[curstring].x = $2;
	        pstr[curstring].y = $4;
            }
	}
	| STRING LOCTYPE worldview {
            string_loctype = $3;
        }
	| STRING color_select {
            string_color = $2;
        }
	| STRING ROT nexpr {
            string_rot = $3;
        }
	| STRING font_select {
            string_font = $2;
        }
	| STRING JUST nexpr {
            string_just = $3;
        }
	| STRING CHAR SIZE expr {
            string_size = $4;
        }
	| STRING DEF sexpr {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
	        set_plotstr_string(&pstr[curstring], $3);
	        pstr[curstring].color = string_color;
	        pstr[curstring].font = string_font;
	        pstr[curstring].just = string_just;
	        pstr[curstring].loctype = string_loctype;
	        pstr[curstring].rot = string_rot;
	        pstr[curstring].charsize = string_size;
            }
	    xfree($3);
	}

/* timestamp */
	| TIMESTAMP onoff {
            timestamp.active = $2;
        }
	| TIMESTAMP font_select {
            timestamp.font = $2;
        }
	| TIMESTAMP CHAR SIZE expr {
            timestamp.charsize = $4;
        }
	| TIMESTAMP ROT nexpr {
            timestamp.rot = $3;
        }
	| TIMESTAMP color_select {
            timestamp.color = $2;
        }
	| TIMESTAMP expr ',' expr {
	    timestamp.x = $2;
	    timestamp.y = $4;
	}
	| TIMESTAMP DEF sexpr {
	  set_plotstr_string(&timestamp, $3);
	  xfree($3);
	}

/* defaults */
	| DEFAULT lines_select {
	    grdefaults.lines = $2;
	    box_lines = ellipse_lines = line_lines = $2;
	}
	| DEFAULT linew_select {
	    grdefaults.linew = $2;
	    box_linew = ellipse_linew = line_linew = $2;
	}
	| DEFAULT color_select {
	    grdefaults.color = $2;
	    box_color = ellipse_color = line_color = string_color = $2;
	}
	| DEFAULT pattern_select {
	    grdefaults.pattern = $2;
	}
	| DEFAULT CHAR SIZE expr {
	    grdefaults.charsize = $4;
	    string_size = $4;
	}
	| DEFAULT font_select {
	    grdefaults.font = $2;
	    string_font = $2;
	}
	| DEFAULT SYMBOL SIZE expr {
	    grdefaults.symsize = $4;
	}
	| DEFAULT SFORMAT sexpr {
	    strcpy(sformat, $3);
	    xfree($3);
	}
	| MAP FONTP nexpr TO sexpr ',' sexpr {
	    if ((map_font_by_name($5, $3) != RETURN_SUCCESS) && 
                (map_font_by_name($7, $3) != RETURN_SUCCESS)) {
                errmsg("Failed mapping a font");
            }
            xfree($5);
	    xfree($7);
	}
	| MAP COLOR nexpr TO '(' nexpr ',' nexpr ',' nexpr ')' ',' sexpr {
	    CMap_entry cmap;
            cmap.rgb.red   = $6;
            cmap.rgb.green = $8;
            cmap.rgb.blue  = $10;
            cmap.ctype = COLOR_MAIN;
            cmap.cname = $13;
            if (store_color($3, cmap) == RETURN_FAILURE) {
                errmsg("Failed mapping a color");
            }
	    xfree($13);
        }

	| WORLD expr ',' expr ',' expr ',' expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.xg1 = $2;
	    g[whichgraph].w.yg1 = $4;
	    g[whichgraph].w.xg2 = $6;
	    g[whichgraph].w.yg2 = $8;
	}
	| ZNORM expr {
	    set_graph_znorm(whichgraph, $2);
	}
	| VIEW expr ',' expr ',' expr ',' expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.xv1 = $2;
	    g[whichgraph].v.yv1 = $4;
	    g[whichgraph].v.xv2 = $6;
	    g[whichgraph].v.yv2 = $8;
	}
	| TITLE sexpr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    set_plotstr_string(&g[whichgraph].labs.title, $2);
	    xfree($2);
	}
	| TITLE font_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.title.font = $2;
	}
	| TITLE SIZE expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.title.charsize = $3;
	}
	| TITLE color_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.title.color = $2;
	}
	| SUBTITLE sexpr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    set_plotstr_string(&g[whichgraph].labs.stitle, $2);
	    xfree($2);
	}
	| SUBTITLE font_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.stitle.font = $2;
	}
	| SUBTITLE SIZE expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.stitle.charsize = $3;
	}
	| SUBTITLE color_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.stitle.color = $2;
	}

	| XAXES SCALE scaletype {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].xscale = $3;
	}
	| YAXES SCALE scaletype {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].yscale = $3;
	}
	| XAXES INVERT onoff {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].xinvert = $3;
	}
	| YAXES INVERT onoff {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].yinvert = $3;
	}
	| AUTOSCALE ONREAD NONE {
            autoscale_onread = AUTOSCALE_NONE;
        }
	| AUTOSCALE ONREAD XAXES {
            autoscale_onread = AUTOSCALE_X;
        }
	| AUTOSCALE ONREAD YAXES {
            autoscale_onread = AUTOSCALE_Y;
        }
	| AUTOSCALE ONREAD XYAXES {
            autoscale_onread = AUTOSCALE_XY;
        }

	| DESCRIPTION sexpr {
            char *s;
            s = copy_string(NULL, get_project_description());
            s = concat_strings(s, $2);
	    xfree($2);
            s = concat_strings(s, "\n");
            set_project_description(s);
            xfree(s);
	}
        | CLEAR DESCRIPTION {
            set_project_description(NULL);
        }

	| LEGEND onoff {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.active = $2;
	}
	| LEGEND LOCTYPE worldview {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.loctype = $3;
	}
	| LEGEND VGAP nexpr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].l.vgap = $3;
	}
	| LEGEND HGAP nexpr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.hgap = $3;
	}
	| LEGEND LENGTH nexpr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.len = $3;
	}
	| LEGEND INVERT onoff {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.invert = $3;
        }
	| LEGEND BOX FILL color_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxfillpen.color = $4;
        }
	| LEGEND BOX FILL pattern_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxfillpen.pattern = $4;
        }
	| LEGEND BOX color_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxpen.color = $3;
	}
	| LEGEND BOX pattern_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxpen.pattern = $3;
	}
	| LEGEND BOX lines_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxlines = $3;
	}
	| LEGEND BOX linew_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxlinew = $3;
	}
	| LEGEND expr ',' expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.legx = $2;
	    g[whichgraph].l.legy = $4;
	}
	| LEGEND CHAR SIZE expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.charsize = $4;
	}
	| LEGEND font_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.font = $2;
	}
	| LEGEND color_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.color = $2;
	}

	| FRAMEP onoff {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.pen.pattern = $2;
	}
	| FRAMEP TYPE nexpr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.type = $3;
	}
	| FRAMEP lines_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.lines = $2;
	}
	| FRAMEP linew_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.linew = $2;
	}
	| FRAMEP color_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.pen.color = $2;
	}
	| FRAMEP pattern_select {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.pen.pattern = $2;
	}
	| FRAMEP BACKGROUND color_select
        { 
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.fillpen.color = $3;
        }
	| FRAMEP BACKGROUND pattern_select
        {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.fillpen.pattern = $3;
        }

	| selectgraph onoff {
            set_graph_hidden($1, !$2);
        }
	| selectgraph HIDDEN onoff {
            set_graph_hidden($1, $3);
        }
	| selectgraph TYPE graphtype {
            set_graph_type($1, $3);
        }
	| selectgraph STACKED onoff {
            set_graph_stacked($1, $3);
        }

	| selectgraph BAR HGAP expr {
	    set_graph_bargap($1, $4);
	}
        
	| selectgraph FIXEDPOINT onoff {
            g[$1].locator.pointset = $3;
        }
	| selectgraph FIXEDPOINT FORMAT formatchoice formatchoice {
	    g[$1].locator.fx = $4;
	    g[$1].locator.fy = $5;
	}
	| selectgraph FIXEDPOINT PREC expr ',' expr {
	    g[$1].locator.px = $4;
	    g[$1].locator.py = $6;
	}
	| selectgraph FIXEDPOINT XY expr ',' expr {
	    g[$1].locator.dsx = $4;
	    g[$1].locator.dsy = $6;
	}
	| selectgraph FIXEDPOINT TYPE nexpr {
            g[$1].locator.pt_type = $4;
        }
        
	| TYPE xytype {
	    curtype = $2;
	}

/* I/O filters */
	| DEFINE filtertype sexpr filtermethod sexpr {
	    if (add_io_filter($2, $4, $5, $3) != 0) {
	        yyerror("Failed adding i/o filter");
	    }
	    xfree($3);
	    xfree($5);
	}
	| CLEAR filtertype {
	    clear_io_filters($2);
	}

	| SOURCE sourcetype {
	    cursource = $2;
	}
	| FORMAT formatchoice {
	    readxformat = $2;
	}
        | FIT nonlfitopts { }
	| FITPARM CONSTRAINTS onoff {
	    nonl_parms[$1].constr = $3;
	}
	;

actions:
	REDRAW {
	    drawgraph();
	}
	| UPDATEALL {
#ifndef NONE_GUI
            if (inwin) {
                update_all();
            }
#endif
        }
	| CD sexpr {
	    set_workingdir($2);
	    xfree($2);
	}
	| ECHO sexpr {
	    echomsg($2);
	    xfree($2);
	}
	| ECHO expr {
	    char buf[32];
            set_locale_num(TRUE);
            sprintf(buf, "%g", $2);
            set_locale_num(FALSE);
            echomsg(buf);
	}
	| CLOSE {
	    close_input = copy_string(close_input, "");
	}
	| CLOSE sexpr {
	    close_input = copy_string(close_input, $2);
	}
	| EXIT {
	    exit(0);
	}
	| EXIT '(' iexpr ')' {
	    exit($3);
	}
	| PRINT {
	    if (!safe_mode) {
                do_hardcopy();
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	}
	| PRINT TO DEVICE {
            set_ptofile(FALSE);
	}
	| PRINT TO sexpr {
            set_ptofile(TRUE);
	    strcpy(print_file, $3);
            xfree($3);
	}
	| PAGE direction {
	    switch ($2) {
	    case UP:
		graph_scroll(GSCROLL_UP);
		break;
	    case DOWN:
		graph_scroll(GSCROLL_DOWN);
		break;
	    case RIGHT:
		graph_scroll(GSCROLL_RIGHT);
		break;
	    case LEFT:
		graph_scroll(GSCROLL_LEFT);
		break;
	    case IN:
		graph_zoom(GZOOM_SHRINK);
		break;
	    case OUT:
		graph_zoom(GZOOM_EXPAND);
		break;
	    }
	}
	| SLEEP expr {
	    if ($2 > 0) {
	        msleep_wrap((unsigned int) (1000 * $2));
	    }
	}
	| HELP sexpr {
#ifndef NONE_GUI
            if (inwin) {
                HelpCB($2);
            }
            xfree($2);
#endif
	}
	| HELP {
#ifndef NONE_GUI
            if (inwin) {
                HelpCB("doc/UsersGuide.html");
            }
#endif
	}
	| GETP sexpr {
	    gotparams = TRUE;
	    strcpy(paramfile, $2);
	    xfree($2);
	}
	| PUTP sexpr {
	    if (!safe_mode) {
                FILE *pp = grace_openw($2);
	        if (pp != NULL) {
	            putparms(whichgraph, pp, 0);
	            grace_close(pp);
	        }
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree($2);
	}
	| selectset HIDDEN onoff {
	    set_set_hidden($1->gno, $1->setno, $3);
	}
	| selectset LENGTH nexpr {
	    setlength($1->gno, $1->setno, $3);
	}
	| VEC_D LENGTH nexpr {
	    realloc_vrbl($1, $3);
	}
	| selectset POINT expr ',' expr {
	    add_point($1->gno, $1->setno, $3, $5);
	}

	| selectset DROP nexpr ',' nexpr {
	    int start = $3 - index_shift;
	    int stop = $5 - index_shift;
	    droppoints($1->gno, $1->setno, start, stop);
	}
	| SORT selectset sorton sortdir {
	    if (is_set_active($2->gno, $2->setno)) {
	        sortset($2->gno, $2->setno, $3, $4 == ASCENDING ? 0 : 1);
	    }
	}
	| COPY selectset TO selectset {
	    do_copyset($2->gno, $2->setno, $4->gno, $4->setno);
	}
	| APPEND selectset TO selectset {
	    if ($2->gno != $4->gno) {
                errmsg("Can't append sets from different graphs");
            } else {
                int sets[2];
	        sets[0] = $4->setno;
	        sets[1] = $2->setno;
	        join_sets($2->gno, sets, 2);
            }
	}
	| REVERSE selectset {
            reverse_set($2->gno, $2->setno);
	}
	| SPLIT selectset nexpr {
            do_splitsets($2->gno, $2->setno, $3);
	}
	| MOVE selectset TO selectset {
	    do_moveset($2->gno, $2->setno, $4->gno, $4->setno);
	}
	| SWAP selectset AND selectset {
	    do_swapset($2->gno, $2->setno, $4->gno, $4->setno);
	}
	| KILL selectset {
	    killset($2->gno, $2->setno);
	}
	| KILL selectset SAVEALL {
            killsetdata($2->gno, $2->setno);
            setcomment($2->gno, $2->setno, "");
        }
	| KILL selectgraph {
            kill_graph($2);
        }
	| KILL REGNUM {
            kill_region($2);
        }
	| FLUSH {
            wipeout();
        }
	| ARRANGE '(' nexpr ',' nexpr ',' expr ',' expr ',' expr ')' {
            arrange_graphs_simple($3, $5, 0, FALSE, $7, $9, $11);
        }
	| ARRANGE '(' nexpr ',' nexpr ',' expr ',' expr ',' expr ',' onoff ',' onoff ',' onoff ')' {
            int order = ($13 * GA_ORDER_HV_INV) |
                        ($15 * GA_ORDER_H_INV ) |
                        ($17 * GA_ORDER_V_INV );
            arrange_graphs_simple($3, $5, order, FALSE, $7, $9, $11);
        }
	| ARRANGE '(' nexpr ',' nexpr ',' expr ',' expr ',' expr ',' onoff ',' onoff ',' onoff ',' onoff ')' {
            int order = ($13 * GA_ORDER_HV_INV) |
                        ($15 * GA_ORDER_H_INV ) |
                        ($17 * GA_ORDER_V_INV );
            arrange_graphs_simple($3, $5, order, $19, $7, $9, $11);
        }
	| NONLFIT '(' selectset ',' nexpr ')' {
	    gotnlfit = TRUE;
	    nlfit_gno = $3->gno;
	    nlfit_setno = $3->setno;
	    nlfit_nsteps = $5;
	    nlfit_warray = NULL;
	}
	| NONLFIT '(' selectset ',' vexpr ',' nexpr ')' {
	    if (getsetlength($3->gno, $3->setno) != $5->length) {
                errmsg("Data and weight arrays are of different lengths");
                return 1;
            } else {
	        gotnlfit = TRUE;
	        nlfit_gno = $3->gno;
	        nlfit_setno = $3->setno;
	        nlfit_nsteps = $7;
	        nlfit_warray = copy_data_column($5->data, $5->length);
            }
	}
	| REGRESS '(' selectset ',' nexpr ')' {
	    do_regress($3->gno, $3->setno, $5, 0, -1, 0, -1);
	}
	| runtype '(' selectset ',' nexpr ')' {
	    do_runavg($3->gno, $3->setno, $5, $1, -1, 0);
	}
	| ffttype '(' selectset ',' nexpr ')' {
	    do_fourier_command($3->gno, $3->setno, $1, $5);
	}
        | ffttype '(' selectset ',' fourierdata ',' windowtype ',' 
                      fourierloadx ','  fourierloady ')' {
	    switch ($1) {
	    case FFT_DFT:
                do_fourier($3->gno, $3->setno, 0, $11, $9, 0, $5, $7);
	        break;
	    case FFT_INVDFT    :
                do_fourier($3->gno, $3->setno, 0, $11, $9, 1, $5, $7);
	        break;
	    case FFT_FFT:
                do_fourier($3->gno, $3->setno, 1, $11, $9, 0, $5, $7);
	        break;
	    case FFT_INVFFT    :
                do_fourier($3->gno, $3->setno, 1, $11, $9, 1, $5, $7);
	        break;
	    default:
                errmsg("Internal error");
	        break;
	    }
        }
	| INTERPOLATE '(' selectset ',' vexpr ',' interpmethod ',' onoff ')' {
            do_interp($3->gno, $3->setno, get_cg(), SET_SELECT_NEXT,
                $5->data, $5->length, $7, $9);
	}
	| HISTOGRAM '(' selectset ',' vexpr ',' onoff ',' onoff ')' {
            do_histo($3->gno, $3->setno, get_cg(), SET_SELECT_NEXT,
                $5->data, $5->length - 1, $7, $9);
	}
	| DIFFERENCE '(' selectset ',' nexpr ')' {
	    do_differ($3->gno, $3->setno, $5);
	}
	| INTEGRATE '(' selectset ')' {
	    do_int($3->gno, $3->setno, 0);
	}
 	| XCOR '(' selectset ',' selectset ',' nexpr ',' onoff ')' {
	    do_xcor($3->gno, $3->setno, $5->gno, $5->setno, $7, $9);
	}
 	| LINCONV '(' selectset ',' selectset ')' {
	    do_linearc($3->gno, $3->setno, $5->gno, $5->setno);
	}
 	| RESTRICT '(' selectset ',' vexpr ')' {
            int len = getsetlength($3->gno, $3->setno);
            if (len != $5->length) {
		errmsg("Filter expression is of a wrong length");
            } else {
                char *rarray;
                rarray = xmalloc(len*SIZEOF_CHAR);
                if (rarray) {
                    int i;
                    for (i = 0; i < len; i++) {
                        rarray[i] = CAST_DBL_TO_BOOL($5->data[i]);
                    }
                    filter_set($3->gno, $3->setno, rarray);
                    xfree(rarray);
                }
            }
	}
 	| RESTRICT '(' selectset ',' REGNUM ',' onoff ')' {
            int rtype;
            char *rarray;
            
            rtype = RESTRICT_REG0 + $5;

	    if (get_restriction_array($3->gno, $3->setno,
                rtype, $7, &rarray) != RETURN_SUCCESS) {
                errmsg("Error in region evaluation");
                return 1;
	    } else {
                filter_set($3->gno, $3->setno, rarray);
                xfree(rarray);
            }
	}
	| AUTOSCALE {
	    if (autoscale_graph(whichgraph, AUTOSCALE_XY) != RETURN_SUCCESS) {
		errmsg("Can't autoscale (no active sets?)");
	    }
	}
	| AUTOSCALE XAXES {
	    if (autoscale_graph(whichgraph, AUTOSCALE_X) != RETURN_SUCCESS) {
		errmsg("Can't autoscale (no active sets?)");
	    }
	}
	| AUTOSCALE YAXES {
	    if (autoscale_graph(whichgraph, AUTOSCALE_Y) != RETURN_SUCCESS) {
		errmsg("Can't autoscale (no active sets?)");
	    }
	}
	| AUTOSCALE selectset {
	    autoscale_byset($2->gno, $2->setno, AUTOSCALE_XY);
	}
        | AUTOTICKS {
            autotick_axis(whichgraph, ALL_AXES);
        }
	| FOCUS selectgraph {
	    int gno = $2;
            if (is_graph_hidden(gno) == FALSE) {
                select_graph(gno);
            } else {
		errmsg("Graph is not active");
            }
	}
	| READ sexpr {
	    gotread = TRUE;
	    strcpy(readfile, $2);
	    xfree($2);
	}
	| READ BATCH sexpr {
	    strcpy(batchfile, $3);
	    xfree($3);
	}
	| READ BLOCK sexpr {
	    getdata(whichgraph, $3, SOURCE_DISK, LOAD_BLOCK);
	    xfree($3);
	}
	| READ BLOCK sourcetype sexpr {
	    getdata(whichgraph, $4, $3, LOAD_BLOCK);
	    xfree($4);
	}
	| BLOCK xytype sexpr {
            int nc, *cols, scol;
            if (field_string_to_cols($3, &nc, &cols, &scol) != RETURN_SUCCESS) {
                errmsg("Erroneous field specifications");
	        xfree($3);
                return 1;
            } else {
	        xfree($3);
	        create_set_fromblock(whichgraph, NEW_SET,
                    $2, nc, cols, scol, autoscale_onread);
                xfree(cols);
            }
	}
	| KILL BLOCK {
	    set_blockdata(NULL);
	}
	| READ xytype sexpr {
	    gotread = TRUE;
	    curtype = $2;
	    strcpy(readfile, $3);
	    xfree($3);
	}
	| READ xytype sourcetype sexpr {
	    gotread = TRUE;
	    strcpy(readfile, $4);
	    curtype = $2;
	    cursource = $3;
	    xfree($4);
	}
	| READ NXY sexpr {
	    getdata(whichgraph, $3, SOURCE_DISK, LOAD_NXY);
	    xfree($3);
	}
	| READ NXY sourcetype sexpr {
	    getdata(whichgraph, $4, $3, LOAD_NXY);
	    xfree($4);
	}
	| WRITE selectset {
	    if (!safe_mode) {
                outputset($2->gno, $2->setno, "stdout", NULL);
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	}
	| WRITE selectset FORMAT sexpr {
	    if (!safe_mode) {
	        outputset($2->gno, $2->setno, "stdout", $4);
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree($4);
	}
	| WRITE selectset FILEP sexpr {
	    if (!safe_mode) {
	        outputset($2->gno, $2->setno, $4, NULL);
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree($4);
	}
	| WRITE selectset FILEP sexpr FORMAT sexpr {
	    if (!safe_mode) {
	        outputset($2->gno, $2->setno, $4, $6);
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree($4);
	    xfree($6);
	}
        | SAVEALL sexpr {
            if (!safe_mode) {
                save_project($2);
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
            xfree($2);
        }
        | LOAD sexpr {
            load_project($2);
            xfree($2);
        }
        | NEW {
            new_project(NULL);
        }
        | NEW FROM sexpr {
            new_project($3);
            xfree($3);
        }
	| PUSH {
	    push_world();
	}
	| POP {
	    pop_world();
	}
	| CYCLE {
	    cycle_world_stack();
	}
	| STACK nexpr {
	    if ($2 > 0)
		show_world_stack($2 - 1);
	}
	| CLEAR STACK {
	    clear_world_stack();
	}
	| CLEAR BOX {
	    do_clear_boxes();
	}
	| CLEAR ELLIPSE {
	    do_clear_ellipses();
	}
	| CLEAR LINE {
	    do_clear_lines();
	}
	| CLEAR STRING {
	    do_clear_text();
	}
        ;


options:
        PAGE LAYOUT pagelayout {
#ifndef NONE_GUI
            set_pagelayout($3);
#endif
        }
	| AUTO REDRAW onoff {
	    auto_redraw = $3;
	}
	| FOCUS onoff {
	    draw_focus_flag = $2;
	}
	| FOCUS SET {
	    focus_policy = FOCUS_SET;
	}
	| FOCUS FOLLOWS {
	    focus_policy = FOCUS_FOLLOWS;
	}
	| FOCUS CLICK {
	    focus_policy = FOCUS_CLICK;
	}
        ;


set_setprop:
	setprop {}
	| setprop_obs {}
	;

setprop:
	selectset onoff {
	    set_set_hidden($1->gno, $1->setno, !$2);
	}
	| selectset TYPE xytype {
	    set_dataset_type($1->gno, $1->setno, $3);
	}

	| selectset SYMBOL nexpr {
	    g[$1->gno].p[$1->setno].sym = $3;
	}
	| selectset SYMBOL color_select {
	    g[$1->gno].p[$1->setno].sympen.color = $3;
	}
	| selectset SYMBOL pattern_select {
	    g[$1->gno].p[$1->setno].sympen.pattern = $3;
	}
	| selectset SYMBOL linew_select {
	    g[$1->gno].p[$1->setno].symlinew = $3;
	}
	| selectset SYMBOL lines_select {
	    g[$1->gno].p[$1->setno].symlines = $3;
	}
	| selectset SYMBOL FILL color_select {
	    g[$1->gno].p[$1->setno].symfillpen.color = $4;
	}
	| selectset SYMBOL FILL pattern_select {
	    g[$1->gno].p[$1->setno].symfillpen.pattern = $4;
	}
	| selectset SYMBOL SIZE expr {
	    g[$1->gno].p[$1->setno].symsize = $4;
	}
	| selectset SYMBOL CHAR nexpr {
	    g[$1->gno].p[$1->setno].symchar = $4;
	}
	| selectset SYMBOL CHAR font_select {
	    g[$1->gno].p[$1->setno].charfont = $4;
	}
	| selectset SYMBOL SKIP nexpr {
	    g[$1->gno].p[$1->setno].symskip = $4;
	}

	| selectset LINE TYPE nexpr
        {
	    g[$1->gno].p[$1->setno].linet = $4;
	}
	| selectset LINE lines_select
        {
	    g[$1->gno].p[$1->setno].lines = $3;
	}
	| selectset LINE linew_select
        {
	    g[$1->gno].p[$1->setno].linew = $3;
	}
	| selectset LINE color_select
        {
	    g[$1->gno].p[$1->setno].linepen.color = $3;
	}
	| selectset LINE pattern_select
        {
	    g[$1->gno].p[$1->setno].linepen.pattern = $3;
	}

	| selectset FILL TYPE nexpr
        {
	    g[$1->gno].p[$1->setno].filltype = $4;
	}
	| selectset FILL RULE nexpr
        {
	    g[$1->gno].p[$1->setno].fillrule = $4;
	}
	| selectset FILL color_select
        {
	    int prop = $3;

	    if (get_project_version() <= 40102 && get_project_version() >= 30000) {
                switch (filltype_obs) {
                case COLOR:
                    break;
                case PATTERN:
                    prop = 1;
                    break;
                default: /* NONE */
	            prop = 0;
                    break;
                }
	    }
	    g[$1->gno].p[$1->setno].setfillpen.color = prop;
	}
	| selectset FILL pattern_select
        {
	    int prop = $3;

	    if (get_project_version() <= 40102) {
                switch (filltype_obs) {
                case COLOR:
                    prop = 1;
                    break;
                case PATTERN:
                    break;
                default: /* NONE */
	            prop = 0;
                    break;
                }
	    }
	    g[$1->gno].p[$1->setno].setfillpen.pattern = prop;
	}

        
	| selectset BASELINE onoff
        {
	    g[$1->gno].p[$1->setno].baseline = $3;
	}
	| selectset BASELINE TYPE nexpr
        {
	    g[$1->gno].p[$1->setno].baseline_type = $4;
	}
        
	| selectset DROPLINE onoff
        {
	    g[$1->gno].p[$1->setno].dropline = $3;
	}

	| selectset AVALUE onoff
        {
	    g[$1->gno].p[$1->setno].avalue.active = $3;
	}
	| selectset AVALUE TYPE nexpr
        {
	    g[$1->gno].p[$1->setno].avalue.type = $4;
	}
	| selectset AVALUE CHAR SIZE expr
        {
	    g[$1->gno].p[$1->setno].avalue.size = $5;
	}
	| selectset AVALUE font_select
        {
	    g[$1->gno].p[$1->setno].avalue.font = $3;
	}
	| selectset AVALUE color_select
        {
	    g[$1->gno].p[$1->setno].avalue.color = $3;
	}
	| selectset AVALUE ROT nexpr
        {
	    g[$1->gno].p[$1->setno].avalue.angle = $4;
	}
	| selectset AVALUE FORMAT formatchoice
        {
	    g[$1->gno].p[$1->setno].avalue.format = $4;
	}
	| selectset AVALUE PREC nexpr
        {
	    g[$1->gno].p[$1->setno].avalue.prec = $4;
	}
	| selectset AVALUE OFFSET expr ',' expr {
	    g[$1->gno].p[$1->setno].avalue.offset.x = $4;
	    g[$1->gno].p[$1->setno].avalue.offset.y = $6;
	}
	| selectset AVALUE PREPEND sexpr
        {
	    strcpy(g[$1->gno].p[$1->setno].avalue.prestr, $4);
	    xfree($4);
	}
	| selectset AVALUE APPEND sexpr
        {
	    strcpy(g[$1->gno].p[$1->setno].avalue.appstr, $4);
	    xfree($4);
	}

	| selectset ERRORBAR onoff {
	    g[$1->gno].p[$1->setno].errbar.active = $3;
	}
	| selectset ERRORBAR opchoice_sel {
	    g[$1->gno].p[$1->setno].errbar.ptype = $3;
	}
	| selectset ERRORBAR color_select {
	    g[$1->gno].p[$1->setno].errbar.pen.color = $3;
	}
	| selectset ERRORBAR pattern_select {
	    g[$1->gno].p[$1->setno].errbar.pen.pattern = $3;
	}
	| selectset ERRORBAR SIZE expr {
            g[$1->gno].p[$1->setno].errbar.barsize = $4;
	}
	| selectset ERRORBAR linew_select {
            g[$1->gno].p[$1->setno].errbar.linew = $3;
	}
	| selectset ERRORBAR lines_select {
            g[$1->gno].p[$1->setno].errbar.lines = $3;
	}
	| selectset ERRORBAR RISER linew_select {
            g[$1->gno].p[$1->setno].errbar.riser_linew = $4;
	}
	| selectset ERRORBAR RISER lines_select {
            g[$1->gno].p[$1->setno].errbar.riser_lines = $4;
	}
	| selectset ERRORBAR RISER CLIP onoff {
            g[$1->gno].p[$1->setno].errbar.arrow_clip = $5;
	}
	| selectset ERRORBAR RISER CLIP LENGTH expr {
            g[$1->gno].p[$1->setno].errbar.cliplen = $6;
	}

	| selectset COMMENT sexpr {
	    strncpy(g[$1->gno].p[$1->setno].comments, $3, MAX_STRING_LENGTH - 1);
	    xfree($3);
	}
        
	| selectset LEGEND sexpr {
	    strncpy(g[$1->gno].p[$1->setno].lstr, $3, MAX_STRING_LENGTH - 1);
	    xfree($3);
	}
	;


axisfeature:
	onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->active = $1;
	}
	| TYPE ZERO onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->zero = $3;
	}
	| TICKP tickattr {}
	| TICKP tickattr_obs {}
	| TICKLABEL ticklabelattr {}
	| TICKLABEL ticklabelattr_obs {}
	| LABEL axislabeldesc {}
	| LABEL axislabeldesc_obs {}
	| BAR axisbardesc {}
	| OFFSET expr ',' expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->offsx = $2;
	    g[whichgraph].t[naxis]->offsy = $4;
	}
	;

tickattr:
	onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_flag = $1;
	}
	| MAJOR expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->tmajor = $2;
	}
	| MINOR TICKSP nexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->nminor = $3;
	}
	| PLACE ROUNDED onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_round = $3;
	}

	| OFFSETX expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->offsx = $2;
	}
	| OFFSETY expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->offsy = $2;
	}
	| DEFAULT nexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_autonum = $2;
	}
	| inoutchoice {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_inout = $1;
	}
	| MAJOR SIZE expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.size = $3;
	}
	| MINOR SIZE expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.size = $3;
	}
	| color_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.color = g[whichgraph].t[naxis]->mprops.color = $1;
	}
	| MAJOR color_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.color = $2;
	}
	| MINOR color_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.color = $2;
	}
	| linew_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.linew = g[whichgraph].t[naxis]->mprops.linew = $1;
	}
	| MAJOR linew_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.linew = $2;
	}
	| MINOR linew_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.linew = $2;
	}
	| MAJOR lines_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.lines = $2;
	}
	| MINOR lines_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.lines = $2;
	}
	| MAJOR GRID onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.gridflag = $3;
	}
	| MINOR GRID onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.gridflag = $3;
	}
	| opchoice_sel {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_op = $1;
	}
	| SPEC TYPE tickspectype {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_spec = $3;
	}
	| SPEC nexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->nticks = $2;
	}
	| MAJOR nexpr ',' expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tloc[$2].wtpos = $4;
	    g[whichgraph].t[naxis]->tloc[$2].type = TICK_TYPE_MAJOR;
	}
	| MINOR nexpr ',' expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tloc[$2].wtpos = $4;
	    g[whichgraph].t[naxis]->tloc[$2].type = TICK_TYPE_MINOR;
	}
	;

ticklabelattr:
	onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_flag = $1;
	}
	| PREC nexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_prec = $2;
	}
	| FORMAT formatchoice {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_format = $2;
	}
	| FORMAT expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_format = $2;
	}
	| APPEND sexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    strcpy(g[whichgraph].t[naxis]->tl_appstr, $2);
	    xfree($2);
	}
	| PREPEND sexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    strcpy(g[whichgraph].t[naxis]->tl_prestr, $2);
	    xfree($2);
	}
	| ANGLE nexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_angle = $2;
	}
	| SKIP nexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_skip = $2;
	}
	| STAGGER nexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_staggered = $2;
	}
	| opchoice_sel {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_op = $1;
	}
	| FORMULA sexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->tl_formula =
                copy_string(g[whichgraph].t[naxis]->tl_formula, $2);
            xfree($2);
	}
	| START expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_start = $2;
	}
	| STOP expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_stop = $2;
	}
	| START TYPE SPEC {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_starttype = TYPE_SPEC;
	}
	| START TYPE AUTO {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_starttype = TYPE_AUTO;
	}
	| STOP TYPE SPEC {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_stoptype = TYPE_SPEC;
	}
	| STOP TYPE AUTO {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_stoptype = TYPE_AUTO;
	}
	| CHAR SIZE expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_charsize = $3;
	}
	| font_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_font = $1;
	}
	| color_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_color = $1;
	}
	| nexpr ',' sexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                xfree($3);
                return 1;
            }
	    if ($1 >= MAX_TICKS) {
	         yyerror("Number of ticks exceeds maximum");
	         xfree($3);
	         return 1;
	    }
	    g[whichgraph].t[naxis]->tloc[$1].label = 
                copy_string(g[whichgraph].t[naxis]->tloc[$1].label, $3);
	    xfree($3);
	}
	| OFFSET AUTO {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_gaptype = TYPE_AUTO;
	}
	| OFFSET SPEC {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_gaptype = TYPE_SPEC;
	}
	| OFFSET expr ',' expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_gap.x = $2;
	    g[whichgraph].t[naxis]->tl_gap.y = $4;
	}
	;

axislabeldesc:
	sexpr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    set_plotstr_string(&g[whichgraph].t[naxis]->label, $1);
	    xfree($1);
	}
	| LAYOUT PERP {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_layout = LAYOUT_PERPENDICULAR;
	}
	| LAYOUT PARA {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_layout = LAYOUT_PARALLEL;
	}
	| PLACE AUTO {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_place = TYPE_AUTO;
	}
	| PLACE SPEC {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_place = TYPE_SPEC;
	}
	| PLACE expr ',' expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.x = $2;
	    g[whichgraph].t[naxis]->label.y = $4;
	}
	| JUST justchoice {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.just = $2;
	}
	| CHAR SIZE expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.charsize = $3;
	}
	| font_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.font = $1;
	}
	| color_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.color = $1;
	}
	| opchoice_sel {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_op = $1;
	}
	;

axisbardesc:
	onoff {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbar = $1;
	}
	| color_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbarcolor = $1;
	}
	| lines_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbarlines = $1;
	}
	| linew_select {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbarlinew = $1;
	}
	;

nonlfitopts:
        TITLE sexpr { 
          nonl_opts.title = copy_string(nonl_opts.title, $2);
	  xfree($2);
        }
        | FORMULA sexpr { 
          nonl_opts.formula = copy_string(nonl_opts.formula, $2);
	  xfree($2);
        }
        | WITH nexpr PARAMETERS { 
            nonl_opts.parnum = $2; 
        }
        | PREC expr { 
            nonl_opts.tolerance = $2; 
        }
        ;

selectgraph:
        GRAPHNO
        {
            $$ = $1;
        }
        | GRAPH indx
        {
            $$ = $2;
        }
        ;

selectset:
	selectgraph '.' SETNUM
	{
	    int gno = $1, setno = $3;
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                $$ = &trgt_pool[tgtn];
                $$->gno   = gno;
                $$->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
	| selectgraph '.' SET indx
	{
	    int gno = $1, setno = $4;
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                $$ = &trgt_pool[tgtn];
                $$->gno   = gno;
                $$->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
	| SETNUM
	{
	    int gno = whichgraph, setno = $1;
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                $$ = &trgt_pool[tgtn];
                $$->gno   = gno;
                $$->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
	| SET indx
	{
	    int gno = whichgraph, setno = $2;
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                $$ = &trgt_pool[tgtn];
                $$->gno   = gno;
                $$->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
	;

setaxis:
	axis axisfeature {}
	| selectgraph axis axisfeature {}
	;

axis:
	XAXIS { naxis =  X_AXIS; }
	| YAXIS { naxis = Y_AXIS; }
	| ALTXAXIS { naxis = ZX_AXIS; }
	| ALTYAXIS { naxis = ZY_AXIS; }
	;

proctype:
        KEY_CONST         { $$ = CONSTANT;  }
        | KEY_UNIT        { $$ = UCONSTANT; }
        | KEY_FUNC_I      { $$ = FUNC_I;    }
	| KEY_FUNC_D      { $$ = FUNC_D;    }
	| KEY_FUNC_ND     { $$ = FUNC_ND;   }
	| KEY_FUNC_NN     { $$ = FUNC_NN;   }
	| KEY_FUNC_DD     { $$ = FUNC_DD;   }
	| KEY_FUNC_NND    { $$ = FUNC_NND;  }
	| KEY_FUNC_PPD    { $$ = FUNC_PPD;  }
	| KEY_FUNC_PPPD   { $$ = FUNC_PPPD; }
	| KEY_FUNC_PPPPD  { $$ = FUNC_PPPPD; }
	| KEY_FUNC_PPPPPD { $$ = FUNC_PPPPPD; }
	;

tickspectype:
	NONE { $$ =  TICKS_SPEC_NONE; }
	| TICKSP { $$ = TICKS_SPEC_MARKS; }
	| BOTH { $$ = TICKS_SPEC_BOTH; }
	;

filtertype:
        IFILTER       { $$ = FILTER_INPUT; }
	| OFILTER    { $$ = FILTER_OUTPUT; }
	;
	
filtermethod:
        MAGIC         { $$ = FILTER_MAGIC; }
	| PATTERN   { $$ = FILTER_PATTERN; }
	;
	
xytype:
	XY { $$ = SET_XY; }
	| BAR { $$ = SET_BAR; }
	| BARDY { $$ = SET_BARDY; }
	| BARDYDY { $$ = SET_BARDYDY; }
	| XYZ { $$ = SET_XYZ; }
	| XYDX { $$ = SET_XYDX; }
	| XYDY { $$ = SET_XYDY; }
	| XYDXDX { $$ = SET_XYDXDX; }
	| XYDYDY { $$ = SET_XYDYDY; }
	| XYDXDY { $$ = SET_XYDXDY; }
	| XYDXDXDYDY { $$ = SET_XYDXDXDYDY; }
	| XYHILO { $$ = SET_XYHILO; }
	| XYR { $$ = SET_XYR; }
	| XYSIZE { $$ = SET_XYSIZE; }
	| XYCOLOR { $$ = SET_XYCOLOR; }
	| XYCOLPAT { $$ = SET_XYCOLPAT; }
	| XYVMAP { $$ = SET_XYVMAP; }
	| XYBOXPLOT { $$ = SET_BOXPLOT; }
	| XYSTRING { $$ = SET_XY; }
	;

graphtype:
	XY { $$ = GRAPH_XY; }
	| CHART { $$ = GRAPH_CHART; }
	| POLAR { $$ = GRAPH_POLAR; }
	| SMITH { $$ = GRAPH_SMITH; }
	| FIXED { $$ = GRAPH_FIXED; }
	| PIE   { $$ = GRAPH_PIE;   }
	;
        
pagelayout:
        FREE { $$ = PAGE_FREE; }
        | FIXED { $$ = PAGE_FIXED; }
        ;

pageorient:
        LANDSCAPE  { $$ = PAGE_ORIENT_LANDSCAPE; }
        | PORTRAIT { $$ = PAGE_ORIENT_PORTRAIT;  }
        ;

regiontype:
	ABOVE { $$ = REGION_ABOVE; }
	|  BELOW { $$ = REGION_BELOW; }
	|  LEFT { $$ = REGION_TOLEFT; }
	|  RIGHT { $$ = REGION_TORIGHT; }
	|  POLYI { $$ = REGION_POLYI; }
	|  POLYO { $$ = REGION_POLYO; }
	|  HORIZI { $$ = REGION_HORIZI; }
	|  VERTI { $$ = REGION_VERTI; }
	|  HORIZO { $$ = REGION_HORIZO; }
	|  VERTO { $$ = REGION_VERTO; }
	;

scaletype: NORMAL { $$ = SCALE_NORMAL; }
	| LOGARITHMIC { $$ = SCALE_LOG; }
	| RECIPROCAL { $$ = SCALE_REC; }
	| LOGIT { $$ = SCALE_LOGIT; }
	;

onoff: ON { $$ = TRUE; }
	| OFF { $$ = FALSE; }
	;

runtype: RUNAVG { $$ = RUN_AVG; }
	| RUNSTD { $$ = RUN_STD; }
	| RUNMED { $$ = RUN_MED; }
	| RUNMAX { $$ = RUN_MAX; }
	| RUNMIN { $$ = RUN_MIN; }
	;

sourcetype: 
        DISK { $$ = SOURCE_DISK; }
	| PIPE {
            if (!safe_mode) {
                $$ = SOURCE_PIPE;
            } else {
                yyerror("Pipe inputs are disabled in safe mode");
                $$ = SOURCE_DISK;
            }
        }
	;

justchoice: RIGHT { $$ = JUST_RIGHT; }
	| LEFT { $$ = JUST_LEFT; }
	| CENTER { $$ = JUST_CENTER; }
	;

inoutchoice: IN { $$ = TICKS_IN; }
	| OUT { $$ = TICKS_OUT; }
	| BOTH { $$ = TICKS_BOTH; }
	;

formatchoice: DECIMAL { $$ = FORMAT_DECIMAL; }
	| EXPONENTIAL { $$ = FORMAT_EXPONENTIAL; }
	| GENERAL { $$ = FORMAT_GENERAL; }
	| SCIENTIFIC { $$ = FORMAT_SCIENTIFIC; }
	| ENGINEERING { $$ = FORMAT_ENGINEERING; }
	| COMPUTING { $$ = FORMAT_COMPUTING; }
	| POWER { $$ = FORMAT_POWER; }
	| DDMMYY { $$ = FORMAT_DDMMYY; }
	| MMDDYY { $$ = FORMAT_MMDDYY; }
	| YYMMDD { $$ = FORMAT_YYMMDD; }
	| MMYY { $$ = FORMAT_MMYY; }
	| MMDD { $$ = FORMAT_MMDD; }
	| MONTHDAY { $$ = FORMAT_MONTHDAY; }
	| DAYMONTH { $$ = FORMAT_DAYMONTH; }
	| MONTHS { $$ = FORMAT_MONTHS; }
	| MONTHSY { $$ = FORMAT_MONTHSY; }
	| MONTHL { $$ = FORMAT_MONTHL; }
	| DAYOFWEEKS { $$ = FORMAT_DAYOFWEEKS; }
	| DAYOFWEEKL { $$ = FORMAT_DAYOFWEEKL; }
	| DAYOFYEAR { $$ = FORMAT_DAYOFYEAR; }
	| HMS { $$ = FORMAT_HMS; }
	| MMDDHMS { $$ = FORMAT_MMDDHMS; }
	| MMDDYYHMS { $$ = FORMAT_MMDDYYHMS; }
	| YYMMDDHMS { $$ = FORMAT_YYMMDDHMS; }
	| DEGREESLON { $$ = FORMAT_DEGREESLON; }
	| DEGREESMMLON { $$ = FORMAT_DEGREESMMLON; }
	| DEGREESMMSSLON { $$ = FORMAT_DEGREESMMSSLON; }
	| MMSSLON { $$ = FORMAT_MMSSLON; }
	| DEGREESLAT { $$ = FORMAT_DEGREESLAT; }
	| DEGREESMMLAT { $$ = FORMAT_DEGREESMMLAT; }
	| DEGREESMMSSLAT { $$ = FORMAT_DEGREESMMSSLAT; }
	| MMSSLAT { $$ = FORMAT_MMSSLAT; }
	;

signchoice: NORMAL { $$ = SIGN_NORMAL; }
	| ABSOLUTE { $$ = SIGN_ABSOLUTE; }
	| NEGATE { $$ = SIGN_NEGATE; }
	;

direction: UP { $$ = UP; }
	| DOWN { $$ = DOWN; }
	| RIGHT { $$ = RIGHT; }
	| LEFT { $$ = LEFT; }
	| IN { $$ = IN; }
	| OUT { $$ = OUT; }
	;

worldview: WORLD { $$ = COORD_WORLD; }
	| VIEW { $$ = COORD_VIEW; }
	;

datacolumn: X_TOK { $$ = DATA_X; }
	| Y_TOK { $$ = DATA_Y; }
	| X0 { $$ = DATA_X; }
	| Y0 { $$ = DATA_Y; }
	| Y1 { $$ = DATA_Y1; }
	| Y2 { $$ = DATA_Y2; }
	| Y3 { $$ = DATA_Y3; }
	| Y4 { $$ = DATA_Y4; }
	;

sortdir: ASCENDING { $$ = ASCENDING; }
	| DESCENDING { $$ = DESCENDING; }
	;

sorton: X_TOK { $$ = DATA_X; }
	| Y_TOK { $$ = DATA_Y; }
	;

ffttype: DFT { $$ = FFT_DFT; }
	| FFT { $$ = FFT_FFT; }
	| INVDFT { $$ = FFT_INVDFT; }
	| INVFFT { $$ = FFT_INVFFT; }
	;

fourierdata:
	REAL {$$=0;}
	| COMPLEX {$$=1;}
	;

fourierloadx:
	INDEX {$$=0;}
	| FREQUENCY {$$=1;}
	| PERIOD {$$=2;}
	;

fourierloady:
	MAGNITUDE {$$=0;}
	| PHASE {$$=1;}
	| COEFFICIENTS {$$=2;}
	;

windowtype:
	NONE {$$=0;}
	| TRIANGULAR {$$=1;}
	| HANNING {$$=2;}
	| WELCH {$$=3;}
	| HAMMING {$$=4;}
	| BLACKMAN {$$=5;}
	| PARZEN {$$=6;}
	;

interpmethod:
        LINEAR    { $$ = INTERP_LINEAR; }
        | SPLINE  { $$ = INTERP_SPLINE; }
        | ASPLINE { $$ = INTERP_ASPLINE; }
	;
	
stattype: MINP { $$ = MINP; }
	| MAXP { $$ = MAXP; }
        | AVG { $$ = AVG; }
	| SD { $$ = SD; }
	| SUM { $$ = SUM; }
	| IMIN { $$ = IMIN; }
	| IMAX { $$ = IMAX; }
	;

font_select:
        FONTP nexpr
        {
            $$ = get_mapped_font($2);
        }
        | FONTP sexpr
        {
            $$ = get_font_by_name($2);
            xfree($2);
        }
        ;

lines_select:
        LINESTYLE nexpr
        {
	    int lines = $2;
            if (lines >= 0 && lines < number_of_linestyles()) {
	        $$ = lines;
	    } else {
	        errmsg("invalid linestyle");
	        $$ = 1;
	    }
        }
        ;

pattern_select:
        PATTERN nexpr
        {
	    int patno = $2;
            if (patno >= 0 && patno < number_of_patterns()) {
	        $$ = patno;
	    } else {
	        errmsg("invalid pattern number");
	        $$ = 1;
	    }
        }
        ;

color_select:
        COLOR nexpr
        {
            int c = $2;
            if (c >= 0 && c < number_of_colors()) {
                $$ = c;
            } else {
                errmsg("Invalid color ID");
                $$ = 1;
            }
        }
        | COLOR sexpr
        {
            int c = get_color_by_name($2);
            if (c == BAD_COLOR) {
                errmsg("Invalid color name");
                c = 1;
            }
            xfree($2);
            $$ = c;
        }
        | COLOR '(' nexpr ',' nexpr ',' nexpr ')'
        {
            int c;
            CMap_entry cmap;
            cmap.rgb.red = $3;
            cmap.rgb.green = $5;
            cmap.rgb.blue = $7;
            cmap.ctype = COLOR_MAIN;
            cmap.cname = NULL;
            c = add_color(cmap);
            if (c == BAD_COLOR) {
                errmsg("Can't allocate requested color");
                c = 1;
            }
            $$ = c;
        }
        ;

linew_select:
        LINEWIDTH expr
        {
            double linew;
            linew = $2;
            if (linew < 0.0) {
                yyerror("Negative linewidth");
                linew = 0.0;
            } else if (linew > MAX_LINEWIDTH) {
                yyerror("Linewidth too large");
                linew = MAX_LINEWIDTH;
            }
            $$ = linew;
        }
        ;

opchoice_sel: PLACE opchoice
        {
            $$ = $2;
        }
        ;

opchoice: NORMAL { $$ = PLACEMENT_NORMAL; }
	| OPPOSITE { $$ = PLACEMENT_OPPOSITE; }
	| BOTH { $$ = PLACEMENT_BOTH; }
	;


parmset_obs:
        PAGE LAYOUT pageorient
        {
            int wpp, hpp;
            if ($3 == PAGE_ORIENT_LANDSCAPE) {
                wpp = 792;
                hpp = 612;
            } else {
                wpp = 612;
                hpp = 792;
            }
            set_page_dimensions(wpp, hpp, FALSE);
        }
        | PAGE SIZE NUMBER NUMBER {
            set_page_dimensions((int) $3, (int) $4, FALSE);
        }
	| PAGE nexpr {
	    scroll_proc($2);
	}
	| PAGE INOUT nexpr {
	    scrollinout_proc($3);
	}

	| DEFAULT FONTP SOURCE expr {
	}

	| STACK WORLD expr ',' expr ',' expr ',' expr TICKP expr ',' expr ',' expr ',' expr
	{
	    add_world(whichgraph, $3, $5, $7, $9);
	}

	| BOX FILL colpat_obs {filltype_obs = $3;}

	| ELLIPSE FILL colpat_obs {filltype_obs = $3;}

	| STRING linew_select { }

	| TIMESTAMP linew_select { }

	| TITLE linew_select { }
	| SUBTITLE linew_select { }

	| LEGEND BOX onoff {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    if ($3 == FALSE && get_project_version() <= 40102) {
                g[whichgraph].l.boxpen.pattern = 0;
            }
	}
	| LEGEND X1 expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.legx = $3;
	}
	| LEGEND Y1 expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.legy = $3;
	}
	| LEGEND STRING nexpr sexpr {
	    if (is_valid_setno(whichgraph, $3)) {
                strncpy(g[whichgraph].p[$3].lstr, $4, MAX_STRING_LENGTH - 1);
	    } else {
                yyerror("Unallocated set");
            }
            xfree($4);
	}
	| LEGEND BOX FILL onoff { }
	| LEGEND BOX FILL WITH colpat_obs {filltype_obs = $5;}
	| LEGEND lines_select { }
	| LEGEND linew_select { }

	| selectgraph LABEL onoff { }

	| selectgraph TYPE LOGX { 
	    g[$1].type = GRAPH_XY;
	    g[$1].xscale = SCALE_LOG;
	}
	| selectgraph TYPE LOGY { 
	    g[$1].type = GRAPH_XY;
	    g[$1].yscale = SCALE_LOG;
	}
	| selectgraph TYPE LOGXY
	{ 
	    g[$1].type = GRAPH_XY;
	    g[$1].xscale = SCALE_LOG;
	    g[$1].yscale = SCALE_LOG;
	}
	| selectgraph TYPE BAR
	{ 
	    g[$1].type = GRAPH_CHART;
	    g[$1].xyflip = FALSE;
	    g[$1].stacked = FALSE;
	}
	| selectgraph TYPE HBAR
	{ 
	    g[$1].type = GRAPH_CHART;
	    g[$1].xyflip = TRUE;
	}
	| selectgraph TYPE STACKEDBAR
	{ 
	    g[$1].type = GRAPH_CHART;
	    g[$1].stacked = TRUE;
	}
	| selectgraph TYPE STACKEDHBAR
	{ 
	    g[$1].type = GRAPH_CHART;
	    g[$1].stacked = TRUE;
	    g[$1].xyflip = TRUE;
	}

	| WORLD XMIN expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.xg1 = $3;
	}
	| WORLD XMAX expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.xg2 = $3;
	}
	| WORLD YMIN expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.yg1 = $3;
	}
	| WORLD YMAX expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.yg2 = $3;
	}

	| VIEW XMIN expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.xv1 = $3;
	}
	| VIEW XMAX expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.xv2 = $3;
	}
	| VIEW YMIN expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.yv1 = $3;
	}
	| VIEW YMAX expr {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.yv2 = $3;
	}

	| LEGEND LAYOUT expr {
	}

	| FRAMEP FILL onoff { 
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.fillpen.pattern = $3;
        }

	| selectgraph AUTOSCALE TYPE AUTO {
        }
	| selectgraph AUTOSCALE TYPE SPEC {
        }

	| LINE ARROW SIZE expr {
	    line_asize = 2.0*$4;
	}

        | HARDCOPY DEVICE expr { }
        | PS LINEWIDTH BEGIN expr { }
        | PS LINEWIDTH INCREMENT expr { }
        | PS linew_select { }
        ;


axislabeldesc_obs:
	linew_select { }
	| opchoice_sel_obs {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_op = $1;
	}
        ;

setprop_obs:
	selectset SYMBOL FILL nexpr {
	    switch ($4){
	    case 0:
	        g[$1->gno].p[$1->setno].symfillpen.pattern = 0;
	        break;
	    case 1:
	        g[$1->gno].p[$1->setno].symfillpen.pattern = 1;
	        break;
	    case 2:
	        g[$1->gno].p[$1->setno].symfillpen.pattern = 1;
	        g[$1->gno].p[$1->setno].symfillpen.color = getbgcolor();
	        break;
	    }
	}
	| selectset SKIP nexpr
        {
	    g[$1->gno].p[$1->setno].symskip = $3;
	}
	| selectset FILL nexpr
        {
	    switch ($3) {
            case 0:
                g[$1->gno].p[$1->setno].filltype = SETFILL_NONE;
                break;
            case 1:
                g[$1->gno].p[$1->setno].filltype = SETFILL_POLYGON;
                break;
            case 2:
                g[$1->gno].p[$1->setno].filltype = SETFILL_BASELINE;
                g[$1->gno].p[$1->setno].baseline_type = BASELINE_TYPE_0;
                break;
            case 6:
                g[$1->gno].p[$1->setno].filltype = SETFILL_BASELINE;
                g[$1->gno].p[$1->setno].baseline_type = BASELINE_TYPE_GMIN;
                break;
            case 7:
                g[$1->gno].p[$1->setno].filltype = SETFILL_BASELINE;
                g[$1->gno].p[$1->setno].baseline_type = BASELINE_TYPE_GMAX;
                break;
            }
	}
	| selectset ERRORBAR TYPE opchoice_obs {
	    g[$1->gno].p[$1->setno].errbar.ptype = $4;
	}
/*
 * 	| selectset SYMBOL COLOR '-' N_NUMBER {
 * 	    g[$1->gno].p[$1->setno].sympen.color = -1;
 * 	}
 */
	| selectset SYMBOL CENTER onoff { }
	| selectset lines_select {
	    g[$1->gno].p[$1->setno].lines = $2;
	}
	| selectset linew_select {
	    g[$1->gno].p[$1->setno].linew = $2;
	}
	| selectset color_select {
	    g[$1->gno].p[$1->setno].linepen.color = $2;
	}
	| selectset FILL WITH colpat_obs {filltype_obs = $4;}
	| selectset XYZ expr ',' expr { }
	| selectset ERRORBAR LENGTH expr {
            g[$1->gno].p[$1->setno].errbar.barsize = $4;
	}
	| selectset ERRORBAR RISER onoff { }
        ;
        

tickattr_obs:
	MAJOR onoff {
	    /* <= xmgr-4.1 */
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->active = $2;
	}
	| MINOR onoff { }
	| ALT onoff   { }
	| MINP NUMBER   { }
	| MAXP NUMBER   { }
	| LOG onoff   { }
	| TYPE AUTO {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_NONE;
	}
	| TYPE SPEC {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    if (g[whichgraph].t[naxis]->t_spec != TICKS_SPEC_BOTH) {
                g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_MARKS;
            }
	}
	| MINOR expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    if ($2 != 0.0) {
                g[whichgraph].t[naxis]->nminor = 
                            (int) rint(g[whichgraph].t[naxis]->tmajor / $2 - 1);
            } else {
                g[whichgraph].t[naxis]->nminor = 0;
            }
	}
	| SIZE expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.size = $2;
	}
	| nexpr ',' expr {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tloc[$1].wtpos = $3;
	    g[whichgraph].t[naxis]->tloc[$1].type = TICK_TYPE_MAJOR;
	}
	| opchoice_sel_obs {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_op = $1;
	}
        ;

ticklabelattr_obs:
	linew_select { }
	| TYPE AUTO {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    if (g[whichgraph].t[naxis]->t_spec == TICKS_SPEC_BOTH) {
                g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_MARKS;
            }
	}
	| TYPE SPEC {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_BOTH;
	}
	| LAYOUT SPEC { }

	| LAYOUT HORIZONTAL {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_angle = 0;
	}
	| LAYOUT VERTICAL {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_angle = 90;
	}
	| PLACE ON TICKSP { }
	| PLACE BETWEEN TICKSP { }
	| opchoice_sel_obs {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_op = $1;
	}
	| SIGN signchoice {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    switch($2) {
            case SIGN_NEGATE:
                g[whichgraph].t[naxis]->tl_formula =
                    copy_string(g[whichgraph].t[naxis]->tl_formula, "-$t");
                break;
            case SIGN_ABSOLUTE:
                g[whichgraph].t[naxis]->tl_formula =
                    copy_string(g[whichgraph].t[naxis]->tl_formula, "abs($t)");
                break;
            default:
                g[whichgraph].t[naxis]->tl_formula =
                    copy_string(g[whichgraph].t[naxis]->tl_formula, NULL);
                break;
            }
	}
        ;

colpat_obs: NONE
	| COLOR
	| PATTERN
	;

opchoice_sel_obs: OP opchoice_obs
        {
            $$ = $2;
        }
        ;

opchoice_obs: TOP { $$ = PLACEMENT_OPPOSITE; }
	| BOTTOM { $$ = PLACEMENT_NORMAL; }
	| LEFT { $$ = PLACEMENT_NORMAL; }
	| RIGHT { $$ = PLACEMENT_OPPOSITE; }
	| BOTH { $$ = PLACEMENT_BOTH; }
	;

%%

/* list of intrinsic functions and keywords */
symtab_entry ikey[] = {
	{"A0", FITPARM, NULL},
	{"A0MAX", FITPMAX, NULL},
	{"A0MIN", FITPMIN, NULL},
	{"A1", FITPARM, NULL},
	{"A1MAX", FITPMAX, NULL},
	{"A1MIN", FITPMIN, NULL},
	{"A2", FITPARM, NULL},
	{"A2MAX", FITPMAX, NULL},
	{"A2MIN", FITPMIN, NULL},
	{"A3", FITPARM, NULL},
	{"A3MAX", FITPMAX, NULL},
	{"A3MIN", FITPMIN, NULL},
	{"A4", FITPARM, NULL},
	{"A4MAX", FITPMAX, NULL},
	{"A4MIN", FITPMIN, NULL},
	{"A5", FITPARM, NULL},
	{"A5MAX", FITPMAX, NULL},
	{"A5MIN", FITPMIN, NULL},
	{"A6", FITPARM, NULL},
	{"A6MAX", FITPMAX, NULL},
	{"A6MIN", FITPMIN, NULL},
	{"A7", FITPARM, NULL},
	{"A7MAX", FITPMAX, NULL},
	{"A7MIN", FITPMIN, NULL},
	{"A8", FITPARM, NULL},
	{"A8MAX", FITPMAX, NULL},
	{"A8MIN", FITPMIN, NULL},
	{"A9", FITPARM, NULL},
	{"A9MAX", FITPMAX, NULL},
	{"A9MIN", FITPMIN, NULL},
	{"ABOVE", ABOVE, NULL},
	{"ABS", FUNC_D, (void *) fabs},
	{"ABSOLUTE", ABSOLUTE, NULL},
	{"ACOS", FUNC_D, (void *) acos},
	{"ACOSH", FUNC_D, (void *) acosh},
	{"AI", FUNC_D, (void *) ai_wrap},
	{"ALIAS", ALIAS, NULL},
	{"ALT", ALT, NULL},
	{"ALTXAXIS", ALTXAXIS, NULL},
	{"ALTYAXIS", ALTYAXIS, NULL},
	{"AND", AND, NULL},
	{"ANGLE", ANGLE, NULL},
	{"ANTIALIASING", ANTIALIASING, NULL},
	{"APPEND", APPEND, NULL},
	{"ARRANGE", ARRANGE, NULL},
	{"ARROW", ARROW, NULL},
	{"ASCENDING", ASCENDING, NULL},
	{"ASIN", FUNC_D, (void *) asin},
	{"ASINH", FUNC_D, (void *) asinh},
	{"ASPLINE", ASPLINE, NULL},
	{"ATAN", FUNC_D, (void *) atan},
	{"ATAN2", FUNC_DD, (void *) atan2},
	{"ATANH", FUNC_D, (void *) atanh},
	{"AUTO", AUTO, NULL},
	{"AUTOSCALE", AUTOSCALE, NULL},
	{"AUTOTICKS", AUTOTICKS, NULL},
	{"AVALUE", AVALUE, NULL},
	{"AVG", AVG, NULL},
	{"BACKGROUND", BACKGROUND, NULL},
	{"BAR", BAR, NULL},
	{"BARDY", BARDY, NULL},
	{"BARDYDY", BARDYDY, NULL},
	{"BASELINE", BASELINE, NULL},
	{"BATCH", BATCH, NULL},
        {"BEGIN", BEGIN, NULL},
	{"BELOW", BELOW, NULL},
	{"BETA", FUNC_DD, (void *) beta},
	{"BETWEEN", BETWEEN, NULL},
	{"BI", FUNC_D, (void *) bi_wrap},
	{"BLACKMAN", BLACKMAN, NULL},
	{"BLOCK", BLOCK, NULL},
	{"BOTH", BOTH, NULL},
	{"BOTTOM", BOTTOM, NULL},
	{"BOX", BOX, NULL},
	{"CD", CD, NULL},
	{"CEIL", FUNC_D, (void *) ceil},
	{"CENTER", CENTER, NULL},
	{"CHAR", CHAR, NULL},
	{"CHART", CHART, NULL},
	{"CHDTR", FUNC_DD, (void *) chdtr},
	{"CHDTRC", FUNC_DD, (void *) chdtrc},
	{"CHDTRI", FUNC_DD, (void *) chdtri},
	{"CHI", FUNC_D, (void *) chi_wrap},
	{"CI", FUNC_D, (void *) ci_wrap},
	{"CLEAR", CLEAR, NULL},
	{"CLICK", CLICK, NULL},
	{"CLIP", CLIP, NULL},
	{"CLOSE", CLOSE, NULL},
	{"COEFFICIENTS", COEFFICIENTS, NULL},
	{"COLOR", COLOR, NULL},
	{"COMMENT", COMMENT, NULL},
	{"COMPLEX", COMPLEX, NULL},
	{"COMPUTING", COMPUTING, NULL},
	{"CONST", KEY_CONST, NULL},
	{"CONSTRAINTS", CONSTRAINTS, NULL},
	{"COPY", COPY, NULL},
	{"COS", FUNC_D, (void *) cos},
	{"COSH", FUNC_D, (void *) cosh},
	{"CYCLE", CYCLE, NULL},
	{"DATE", DATE, NULL},
	{"DAWSN", FUNC_D, (void *) dawsn},
	{"DAYMONTH", DAYMONTH, NULL},
	{"DAYOFWEEKL", DAYOFWEEKL, NULL},
	{"DAYOFWEEKS", DAYOFWEEKS, NULL},
	{"DAYOFYEAR", DAYOFYEAR, NULL},
	{"DDMMYY", DDMMYY, NULL},
	{"DECIMAL", DECIMAL, NULL},
	{"DEF", DEF, NULL},
	{"DEFAULT", DEFAULT, NULL},
	{"DEFINE", DEFINE, NULL},
	{"DEG", UCONSTANT, (void *) deg_uconst},
	{"DEGREESLAT", DEGREESLAT, NULL},
	{"DEGREESLON", DEGREESLON, NULL},
	{"DEGREESMMLAT", DEGREESMMLAT, NULL},
	{"DEGREESMMLON", DEGREESMMLON, NULL},
	{"DEGREESMMSSLAT", DEGREESMMSSLAT, NULL},
	{"DEGREESMMSSLON", DEGREESMMSSLON, NULL},
	{"DESCENDING", DESCENDING, NULL},
	{"DESCRIPTION", DESCRIPTION, NULL},
	{"DEVICE", DEVICE, NULL},
	{"DFT", DFT, NULL},
	{"DIFF", DIFFERENCE, NULL},
	{"DIFFERENCE", DIFFERENCE, NULL},
	{"DISK", DISK, NULL},
	{"DOWN", DOWN, NULL},
	{"DPI", DPI, NULL},
	{"DROP", DROP, NULL},
	{"DROPLINE", DROPLINE, NULL},
	{"ECHO", ECHO, NULL},
	{"ELLIE", FUNC_DD, (void *) ellie},
	{"ELLIK", FUNC_DD, (void *) ellik},
	{"ELLIPSE", ELLIPSE, NULL},
	{"ELLPE", FUNC_D, (void *) ellpe_wrap},
	{"ELLPK", FUNC_D, (void *) ellpk_wrap},
	{"ENGINEERING", ENGINEERING, NULL},
	{"EQ", EQ, NULL},
	{"ER", ERRORBAR, NULL},
	{"ERF", FUNC_D, (void *) erf},
	{"ERFC", FUNC_D, (void *) erfc},
	{"ERRORBAR", ERRORBAR, NULL},
	{"EXIT", EXIT, NULL},
	{"EXP", FUNC_D, (void *) exp},
	{"EXPN", FUNC_ND, (void *) expn},
	{"EXPONENTIAL", EXPONENTIAL, NULL},
	{"FAC", FUNC_I, (void *) fac},
	{"FALSE", OFF, NULL},
	{"FDTR", FUNC_NND, (void *) fdtr},
	{"FDTRC", FUNC_NND, (void *) fdtrc},
	{"FDTRI", FUNC_NND, (void *) fdtri},
	{"FFT", FFT, NULL},
	{"FILE", FILEP, NULL},
	{"FILL", FILL, NULL},
	{"FIT", FIT, NULL},
	{"FIXED", FIXED, NULL},
	{"FIXEDPOINT", FIXEDPOINT, NULL},
	{"FLOOR", FUNC_D, (void *) floor},
	{"FLUSH", FLUSH, NULL},
	{"FOCUS", FOCUS, NULL},
	{"FOLLOWS", FOLLOWS, NULL},
	{"FONT", FONTP, NULL},
	{"FORCE", FORCE, NULL},
	{"FORMAT", FORMAT, NULL},
	{"FORMULA", FORMULA, NULL},
	{"FRAME", FRAMEP, NULL},
	{"FREE", FREE, NULL},
	{"FREQUENCY", FREQUENCY, NULL},
	{"FRESNLC", FUNC_D, (void *) fresnlc_wrap},
	{"FRESNLS", FUNC_D, (void *) fresnls_wrap},
	{"FROM", FROM, NULL},
	{"F_OF_D", KEY_FUNC_D, NULL},
	{"F_OF_DD", KEY_FUNC_DD, NULL},
        {"F_OF_I", KEY_FUNC_I, NULL},
	{"F_OF_ND", KEY_FUNC_ND, NULL},
	{"F_OF_NN", KEY_FUNC_NN, NULL},
	{"F_OF_NND", KEY_FUNC_NND, NULL},
	{"F_OF_PPD", KEY_FUNC_PPD, NULL},
	{"F_OF_PPPD", KEY_FUNC_PPPD, NULL},
	{"F_OF_PPPPD", KEY_FUNC_PPPPD, NULL},
	{"F_OF_PPPPPD", KEY_FUNC_PPPPPD, NULL},
	{"GAMMA", FUNC_D, (void *) true_gamma},
	{"GDTR", FUNC_PPD, (void *) gdtr},
	{"GDTRC", FUNC_PPD, (void *) gdtrc},
	{"GE", GE, NULL},
	{"GENERAL", GENERAL, NULL},
	{"GETP", GETP, NULL},
	{"GRAPH", GRAPH, NULL},
	{"GRID", GRID, NULL},
	{"GT", GT, NULL},
	{"HAMMING", HAMMING, NULL},
	{"HANNING", HANNING, NULL},
	{"HARDCOPY", HARDCOPY, NULL},
	{"HBAR", HBAR, NULL},
	{"HELP", HELP, NULL},
	{"HGAP", HGAP, NULL},
	{"HIDDEN", HIDDEN, NULL},
	{"HISTOGRAM", HISTOGRAM, NULL},
	{"HMS", HMS, NULL},
	{"HORIZI", HORIZI, NULL},
	{"HORIZO", HORIZO, NULL},
	{"HORIZONTAL", HORIZONTAL, NULL},
	{"HYP2F1", FUNC_PPPD, (void *) hyp2f1},
	{"HYPERG", FUNC_PPD, (void *) hyperg},
	{"HYPOT", FUNC_DD, (void *) hypot},
	{"I0E", FUNC_D, (void *) i0e},
	{"I1E", FUNC_D, (void *) i1e},
	{"ID", ID, NULL},
	{"IFILTER", IFILTER, NULL},
	{"IGAM", FUNC_DD, (void *) igam},
	{"IGAMC", FUNC_DD, (void *) igamc},
	{"IGAMI", FUNC_DD, (void *) igami},
	{"IMAX", IMAX, NULL},
	{"IMIN", IMIN, NULL},
	{"IN", IN, NULL},
	{"INCBET", FUNC_PPD, (void *) incbet},
	{"INCBI", FUNC_PPD, (void *) incbi},
	{"INCREMENT", INCREMENT, NULL},
	{"INDEX", INDEX, NULL},
	{"INOUT", INOUT, NULL},
	{"INT", INT, NULL},
	{"INTEGRATE", INTEGRATE, NULL},
	{"INTERPOLATE", INTERPOLATE, NULL},
	{"INVDFT", INVDFT, NULL},
	{"INVERT", INVERT, NULL},
	{"INVFFT", INVFFT, NULL},
	{"IRAND", FUNC_I, (void *) irand_wrap},
	{"IV", FUNC_DD, (void *) iv_wrap},
	{"JUST", JUST, NULL},
	{"JV", FUNC_DD, (void *) jv_wrap},
	{"K0E", FUNC_D, (void *) k0e},
	{"K1E", FUNC_D, (void *) k1e},
	{"KILL", KILL, NULL},
	{"KN", FUNC_ND, (void *) kn_wrap},
	{"LABEL", LABEL, NULL},
	{"LANDSCAPE", LANDSCAPE, NULL},
	{"LAYOUT", LAYOUT, NULL},
	{"LBETA", FUNC_DD, (void *) lbeta},
	{"LE", LE, NULL},
	{"LEFT", LEFT, NULL},
	{"LEGEND", LEGEND, NULL},
	{"LENGTH", LENGTH, NULL},
	{"LGAMMA", FUNC_D, (void *) lgamma},
	{"LINCONV", LINCONV, NULL},
	{"LINE", LINE, NULL},
	{"LINEAR", LINEAR, NULL},
	{"LINESTYLE", LINESTYLE, NULL},
	{"LINEWIDTH", LINEWIDTH, NULL},
	{"LINK", LINK, NULL},
	{"LN", FUNC_D, (void *) log},
	{"LOAD", LOAD, NULL},
	{"LOCTYPE", LOCTYPE, NULL},
	{"LOG", LOG, NULL},
	{"LOG10", FUNC_D, (void *) log10},
	{"LOG2", FUNC_D, (void *) log2},
	{"LOGARITHMIC", LOGARITHMIC, NULL},
	{"LOGX", LOGX, NULL},
	{"LOGXY", LOGXY, NULL},
	{"LOGY", LOGY, NULL},
	{"LOGIT", LOGIT, NULL},
	{"LT", LT, NULL},
	{"MAGIC", MAGIC, NULL},
	{"MAGNITUDE", MAGNITUDE, NULL},
	{"MAJOR", MAJOR, NULL},
	{"MAP", MAP, NULL},
	{"MAX", MAXP, NULL},
	{"MAXOF", FUNC_DD, (void *) max_wrap},
	{"MESH", MESH, NULL},
	{"MIN", MINP, NULL},
	{"MINOF", FUNC_DD, (void *) min_wrap},
	{"MINOR", MINOR, NULL},
	{"MMDD", MMDD, NULL},
	{"MMDDHMS", MMDDHMS, NULL},
	{"MMDDYY", MMDDYY, NULL},
	{"MMDDYYHMS", MMDDYYHMS, NULL},
	{"MMSSLAT", MMSSLAT, NULL},
	{"MMSSLON", MMSSLON, NULL},
	{"MMYY", MMYY, NULL},
	{"MOD", FUNC_DD, (void *) fmod},
	{"MONTHDAY", MONTHDAY, NULL},
	{"MONTHL", MONTHL, NULL},
	{"MONTHS", MONTHS, NULL},
	{"MONTHSY", MONTHSY, NULL},
	{"MOVE", MOVE, NULL},
	{"NDTR", FUNC_D, (void *) ndtr},
	{"NDTRI", FUNC_D, (void *) ndtri},
	{"NE", NE, NULL},
	{"NEGATE", NEGATE, NULL},
	{"NEW", NEW, NULL},
	{"NONE", NONE, NULL},
	{"NONLFIT", NONLFIT, NULL},
	{"NORM", FUNC_D, (void *) fx},
	{"NORMAL", NORMAL, NULL},
	{"NOT", NOT, NULL},
	{"NXY", NXY, NULL},
	{"OFF", OFF, NULL},
	{"OFFSET", OFFSET, NULL},
	{"OFFSETX", OFFSETX, NULL},
	{"OFFSETY", OFFSETY, NULL},
	{"OFILTER", OFILTER, NULL},
	{"ON", ON, NULL},
	{"ONREAD", ONREAD, NULL},
	{"OP", OP, NULL},
	{"OPPOSITE", OPPOSITE, NULL},
	{"OR", OR, NULL},
	{"OUT", OUT, NULL},
	{"PAGE", PAGE, NULL},
	{"PARA", PARA, NULL},
	{"PARAMETERS", PARAMETERS, NULL},
	{"PARZEN", PARZEN, NULL},
	{"PATTERN", PATTERN, NULL},
	{"PDTR", FUNC_ND, (void *) pdtr},
	{"PDTRC", FUNC_ND, (void *) pdtrc},
	{"PDTRI", FUNC_ND, (void *) pdtri},
	{"PERIOD", PERIOD, NULL},
	{"PERP", PERP, NULL},
	{"PHASE", PHASE, NULL},
	{"PI", CONSTANT, (void *) pi_const},
	{"PIE", PIE, NULL},
	{"PIPE", PIPE, NULL},
	{"PLACE", PLACE, NULL},
	{"POINT", POINT, NULL},
	{"POLAR", POLAR, NULL},
	{"POLYI", POLYI, NULL},
	{"POLYO", POLYO, NULL},
	{"POP", POP, NULL},
	{"PORTRAIT", PORTRAIT, NULL},
	{"POWER", POWER, NULL},
	{"PREC", PREC, NULL},
	{"PREPEND", PREPEND, NULL},
	{"PRINT", PRINT, NULL},
	{"PS", PS, NULL},
	{"PSI", FUNC_D, (void *) psi},
	{"PUSH", PUSH, NULL},
	{"PUTP", PUTP, NULL},
	{"RAD", UCONSTANT, (void *) rad_uconst},
	{"RAND", RAND, NULL},
	{"READ", READ, NULL},
	{"REAL", REAL, NULL},
	{"RECIPROCAL", RECIPROCAL, NULL},
	{"REDRAW", REDRAW, NULL},
	{"REFERENCE", REFERENCE, NULL},
	{"REGRESS", REGRESS, NULL},
	{"RESIZE", RESIZE, NULL},
	{"RESTRICT", RESTRICT, NULL},
	{"REVERSE", REVERSE, NULL},
	{"RGAMMA", FUNC_D, (void *) rgamma},
	{"RIGHT", RIGHT, NULL},
	{"RINT", FUNC_D, (void *) rint},
	{"RISER", RISER, NULL},
	{"RNORM", FUNC_DD, (void *) rnorm},
	{"ROT", ROT, NULL},
	{"ROUNDED", ROUNDED, NULL},
	{"RSUM", RSUM, NULL},
	{"RULE", RULE, NULL},
	{"RUNAVG", RUNAVG, NULL},
	{"RUNMAX", RUNMAX, NULL},
	{"RUNMED", RUNMED, NULL},
	{"RUNMIN", RUNMIN, NULL},
	{"RUNSTD", RUNSTD, NULL},
	{"SAVEALL", SAVEALL, NULL},
	{"SCALE", SCALE, NULL},
	{"SCIENTIFIC", SCIENTIFIC, NULL},
	{"SCROLL", SCROLL, NULL},
	{"SD", SD, NULL},
	{"SET", SET, NULL},
	{"SFORMAT", SFORMAT, NULL},
	{"SGN", FUNC_D, (void *) sign_wrap},
	{"SHI", FUNC_D, (void *) shi_wrap},
	{"SI", FUNC_D, (void *) si_wrap},
	{"SIGN", SIGN, NULL},
	{"SIN", FUNC_D, (void *) sin},
	{"SINH", FUNC_D, (void *) sinh},
	{"SIZE", SIZE, NULL},
	{"SKIP", SKIP, NULL},
	{"SLEEP", SLEEP, NULL},
	{"SMITH", SMITH, NULL},
	{"SORT", SORT, NULL},
	{"SOURCE", SOURCE, NULL},
	{"SPEC", SPEC, NULL},
	{"SPENCE", FUNC_D, (void *) spence},
	{"SPLINE", SPLINE, NULL},
	{"SPLIT", SPLIT, NULL},
	{"SQR", FUNC_D, (void *) sqr_wrap},
	{"SQRT", FUNC_D, (void *) sqrt},
	{"STACK", STACK, NULL},
	{"STACKED", STACKED, NULL},
	{"STACKEDBAR", STACKEDBAR, NULL},
	{"STACKEDHBAR", STACKEDHBAR, NULL},
	{"STAGGER", STAGGER, NULL},
	{"START", START, NULL},
	{"STDTR", FUNC_ND, (void *) stdtr},
	{"STDTRI", FUNC_ND, (void *) stdtri},
	{"STOP", STOP, NULL},
	{"STRING", STRING, NULL},
	{"STRUVE", FUNC_DD, (void *) struve},
	{"SUBTITLE", SUBTITLE, NULL},
	{"SUM", SUM, NULL},
	{"SWAP", SWAP, NULL},
	{"SYMBOL", SYMBOL, NULL},
	{"TAN", FUNC_D, (void *) tan},
	{"TANH", FUNC_D, (void *) tanh},
	{"TARGET", TARGET, NULL},
	{"TICK", TICKP, NULL},
	{"TICKLABEL", TICKLABEL, NULL},
	{"TICKS", TICKSP, NULL},
	{"TIMER", TIMER, NULL},
	{"TIMESTAMP", TIMESTAMP, NULL},
	{"TITLE", TITLE, NULL},
	{"TO", TO, NULL},
	{"TOP", TOP, NULL},
	{"TRIANGULAR", TRIANGULAR, NULL},
	{"TRUE", ON, NULL},
	{"TYPE", TYPE, NULL},
	{"UNIT", KEY_UNIT, NULL},
	{"UP", UP, NULL},
	{"UPDATEALL", UPDATEALL, NULL},
	{"USE", USE, NULL},
	{"VERSION", VERSION, NULL},
	{"VERTI", VERTI, NULL},
	{"VERTICAL", VERTICAL, NULL},
	{"VERTO", VERTO, NULL},
	{"VGAP", VGAP, NULL},
	{"VIEW", VIEW, NULL},
	{"VOIGT", FUNC_PPD, (void *) voigt},
	{"VX1", VX1, NULL},
	{"VX2", VX2, NULL},
	{"VXMAX", VXMAX, NULL},
	{"VY1", VY1, NULL},
	{"VY2", VY2, NULL},
	{"VYMAX", VYMAX, NULL},
	{"WELCH", WELCH, NULL},
	{"WITH", WITH, NULL},
	{"WORLD", WORLD, NULL},
	{"WRAP", WRAP, NULL},
	{"WRITE", WRITE, NULL},
	{"WX1", WX1, NULL},
	{"WX2", WX2, NULL},
	{"WY1", WY1, NULL},
	{"WY2", WY2, NULL},
	{"X", X_TOK, NULL},
	{"X0", X0, NULL},
	{"X1", X1, NULL},
	{"XAXES", XAXES, NULL},
	{"XAXIS", XAXIS, NULL},
	{"XCOR", XCOR, NULL},
	{"XMAX", XMAX, NULL},
	{"XMIN", XMIN, NULL},
	{"XY", XY, NULL},
	{"XYAXES", XYAXES, NULL},
	{"XYBOXPLOT", XYBOXPLOT, NULL},
	{"XYCOLOR", XYCOLOR, NULL},
	{"XYCOLPAT", XYCOLPAT, NULL},
	{"XYDX", XYDX, NULL},
	{"XYDXDX", XYDXDX, NULL},
	{"XYDXDXDYDY", XYDXDXDYDY, NULL},
	{"XYDXDY", XYDXDY, NULL},
	{"XYDY", XYDY, NULL},
	{"XYDYDY", XYDYDY, NULL},
	{"XYHILO", XYHILO, NULL},
	{"XYR", XYR, NULL},
	{"XYSIZE", XYSIZE, NULL},
	{"XYSTRING", XYSTRING, NULL},
	{"XYVMAP", XYVMAP, NULL},
	{"XYZ", XYZ, NULL},
	{"Y", Y_TOK, NULL},
	{"Y0", Y0, NULL},
	{"Y1", Y1, NULL},
	{"Y2", Y2, NULL},
	{"Y3", Y3, NULL},
	{"Y4", Y4, NULL},
	{"YAXES", YAXES, NULL},
	{"YAXIS", YAXIS, NULL},
	{"YEAR", YEAR, NULL},
	{"YMAX", YMAX, NULL},
	{"YMIN", YMIN, NULL},
	{"YV", FUNC_DD, (void *) yv_wrap},
	{"YYMMDD", YYMMDD, NULL},
	{"YYMMDDHMS", YYMMDDHMS, NULL},
	{"ZERO", ZERO, NULL},
	{"ZEROXAXIS", ALTXAXIS, NULL},
	{"ZEROYAXIS", ALTYAXIS, NULL},
	{"ZETA", FUNC_DD, (void *) zeta},
	{"ZETAC", FUNC_D, (void *) zetac},
	{"ZNORM", ZNORM, NULL}
};

static int maxfunc = sizeof(ikey) / sizeof(symtab_entry);

int get_parser_gno(void)
{
    return whichgraph;
}

int set_parser_gno(int gno)
{
    if (is_valid_gno(gno) == TRUE) {
        whichgraph = gno;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int get_parser_setno(void)
{
    return whichset;
}

int set_parser_setno(int gno, int setno)
{
    if (is_valid_setno(gno, setno) == TRUE) {
        whichgraph = gno;
        whichset = setno;
        /* those will usually be overridden except when evaluating
           a _standalone_ vexpr */
        vasgn_gno = gno;
        vasgn_setno = setno;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

void realloc_vrbl(grarr *vrbl, int len)
{
    double *a;
    int i, oldlen;
    
    if (vrbl->type != GRARR_VEC) {
        errmsg("Internal error");
        return;
    }
    oldlen = vrbl->length;
    if (oldlen == len) {
        return;
    } else {
        a = xrealloc(vrbl->data, len*SIZEOF_DOUBLE);
        if (a != NULL || len == 0) {
            vrbl->data = a;
            vrbl->length = len;
            for (i = oldlen; i < len; i++) {
                vrbl->data[i] = 0.0;
            }
        } else {
            errmsg("Malloc failed in realloc_vrbl()");
        }
    }
}


#define PARSER_TYPE_VOID    0
#define PARSER_TYPE_EXPR    1
#define PARSER_TYPE_VEXPR   2

static int parser(char *s, int type)
{
    char *seekpos;
    int i;
    
    if (s == NULL || s[0] == '\0') {
        if (type == PARSER_TYPE_VOID) {
            /* don't consider an empty string as error for generic parser */
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    }
    
    strncpy(f_string, s, MAX_PARS_STRING_LENGTH - 2);
    f_string[MAX_PARS_STRING_LENGTH - 2] = '\0';
    strcat(f_string, " ");
    
    seekpos = f_string;

    while ((seekpos - f_string < MAX_PARS_STRING_LENGTH - 1) && (*seekpos == ' ' || *seekpos == '\t')) {
        seekpos++;
    }
    if (*seekpos == '\n' || *seekpos == '#') {
        if (type == PARSER_TYPE_VOID) {
            /* don't consider an empty string as error for generic parser */
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    }
    
    lowtoupper(f_string);
        
    pos = 0;
    interr = 0;
    expr_parsed  = FALSE;
    vexpr_parsed = FALSE;
    
    yyparse();

    /* free temp. arrays; for a vector expression keep the last one
     * (which is none but v_result), given there have been no errors
     * and it's what we've been asked for
     */
    if (vexpr_parsed && !interr && type == PARSER_TYPE_VEXPR) {
        for (i = 0; i < fcnt - 1; i++) {
            free_tmpvrbl(&(freelist[i]));
        }
    } else {
        for (i = 0; i < fcnt; i++) {
            free_tmpvrbl(&(freelist[i]));
        }
    }
    fcnt = 0;
    
    tgtn = 0;
    
    if ((type == PARSER_TYPE_VEXPR && !vexpr_parsed) ||
        (type == PARSER_TYPE_EXPR  && !expr_parsed)) {
        return RETURN_FAILURE;
    } else {
        return (interr ? RETURN_FAILURE:RETURN_SUCCESS);
    }
}

int s_scanner(char *s, double *res)
{
    int retval = parser(s, PARSER_TYPE_EXPR);
    *res = s_result;
    return retval;
}

int v_scanner(char *s, int *reslen, double **vres)
{
    int retval = parser(s, PARSER_TYPE_VEXPR);
    if (retval != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    } else {
        *reslen = v_result->length;
        if (v_result->type == GRARR_TMP) {
            *vres = v_result->data;
            v_result->length = 0;
            v_result->data = NULL;
        } else {
            *vres = copy_data_column(v_result->data, v_result->length);
        }
        return RETURN_SUCCESS;
    }
}

int scanner(char *s)
{
    int retval = parser(s, PARSER_TYPE_VOID);
    if (retval != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }
    
    if (gotparams) {
	gotparams = FALSE;
        getparms(paramfile);
    }
    
    if (gotread) {
	gotread = FALSE;
        getdata(whichgraph, readfile, cursource, LOAD_SINGLE);
    }
    
    if (gotnlfit) {
	gotnlfit = FALSE;
        do_nonlfit(nlfit_gno, nlfit_setno, nlfit_warray, NULL, nlfit_nsteps);
        XCFREE(nlfit_warray);
    }
    return retval;
}

static void free_tmpvrbl(grarr *vrbl)
{
    if (vrbl->type == GRARR_TMP) {
        vrbl->length = 0;
        XCFREE(vrbl->data);
    }
}

static void copy_vrbl(grarr *dest, grarr *src)
{
    dest->type = src->type;
    dest->data = xmalloc(src->length*SIZEOF_DOUBLE);
    if (dest->data == NULL) {
        errmsg("Malloc failed in copy_vrbl()");
    } else {
        memcpy(dest->data, src->data, src->length*SIZEOF_DOUBLE);
        dest->length = src->length;
    }
}

grarr *get_parser_arr_by_name(char * const name)
{
     int position;
     char *s;
     
     s = copy_string(NULL, name);
     lowtoupper(s);
     
     position = findf(key, s);
     xfree(s);
     
     if (position >= 0) {
         if (key[position].type == KEY_VEC) {
            return (grarr *) key[position].data;
         }
     }
     
     return NULL;
}

grarr *define_parser_arr(char * const name)
{
     if (get_parser_arr_by_name(name) == NULL) {
	symtab_entry tmpkey;
        grarr *var;
        
        var = xmalloc(sizeof(grarr));
        var->type = GRARR_VEC;
        var->length = 0;
        var->data = NULL;
        
	tmpkey.s = name;
	tmpkey.type = KEY_VEC;
	tmpkey.data = (void *) var;
	if (addto_symtab(tmpkey) == RETURN_SUCCESS) {
	    return var;
	} else {
            return NULL;
        }
     } else {
        return NULL;
     }
}

int undefine_parser_var(void *ptr)
{
    int i;
    
    for (i = 0; i < maxfunc; i++) {
	if (key[i].data == ptr) {
            xfree(key[i].s);
            maxfunc--;
            if (i != maxfunc) {
                memmove(&(key[i]), &(key[i + 1]), (maxfunc - i)*sizeof(symtab_entry));
            }
            key = xrealloc(key, maxfunc*sizeof(symtab_entry));
            return RETURN_SUCCESS;
        }
    }
    return RETURN_FAILURE;
}

static int find_set_bydata(double *data, target *tgt)
{
    int gno, setno, ncol;
    
    if (data == NULL) {
        return RETURN_FAILURE;
    } else {
        for (gno = 0; gno < number_of_graphs(); gno++) {
            for (setno = 0; setno < number_of_sets(gno); setno++) {
                for (ncol = 0; ncol < MAX_SET_COLS; ncol++) {
                    if (getcol(gno, setno, ncol) == data) {
                        tgt->gno   = gno;
                        tgt->setno = setno;
                        return RETURN_SUCCESS;
                    }
                }
            }
        }
    }
    return RETURN_FAILURE;
}

static int findf(symtab_entry *keytable, char *s)
{

    int low, high, mid;

    low = 0;
    high = maxfunc - 1;
    while (low <= high) {
	mid = (low + high) / 2;
	if (strcmp(s, keytable[mid].s) < 0) {
	    high = mid - 1;
	} else {
	    if (strcmp(s, keytable[mid].s) > 0) {
		low = mid + 1;
	    } else {
		return (mid);
	    }
	}
    }
    return (-1);
}

static int compare_keys (const void *a, const void *b)
{
    return (int) strcmp (((const symtab_entry*)a)->s,
                         ((const symtab_entry*)b)->s);
}

/* add new entry to the symbol table */
int addto_symtab(symtab_entry newkey)
{
    int position;
    char *s;
    
    s = copy_string(NULL, newkey.s);
    lowtoupper(s);
    if ((position = findf(key, s)) < 0) {
        if ((key = (symtab_entry *) xrealloc(key, (maxfunc + 1)*sizeof(symtab_entry))) != NULL) {
	    key[maxfunc].type = newkey.type;
	    key[maxfunc].data = newkey.data;
	    key[maxfunc].s = s;
	    maxfunc++;
	    qsort(key, maxfunc, sizeof(symtab_entry), compare_keys);
	    return RETURN_SUCCESS;
	} else {
	    xfree(s);
	    return RETURN_FAILURE;
	}
    } else if (alias_force == TRUE) { /* already exists but alias_force enabled */
        key[position].type = newkey.type;
	key[position].data = newkey.data;
	return RETURN_SUCCESS;
    } else {
	xfree(s);
        return RETURN_FAILURE;
    }
}

/* initialize symbol table */
void init_symtab(void)
{
    int i;
    
    if ((key = (symtab_entry *) xmalloc(maxfunc*sizeof(symtab_entry))) != NULL) {
    	memcpy (key, ikey, maxfunc*sizeof(symtab_entry));
	for (i = 0; i < maxfunc; i++) {
	    key[i].s = xmalloc(strlen(ikey[i].s) + 1);
	    strcpy(key[i].s, ikey[i].s);
	}
	qsort(key, maxfunc, sizeof(symtab_entry), compare_keys);
	return;
    } else {
	key = ikey;
	return;
    }
}

static int getcharstr(void)
{
    if (pos >= strlen(f_string))
	 return EOF;
    return (f_string[pos++]);
}

static void ungetchstr(void)
{
    if (pos > 0)
	pos--;
}

static int yylex(void)
{
    int c, i;
    int found;
    char sbuf[MAX_PARS_STRING_LENGTH + 40];

    while ((c = getcharstr()) == ' ' || c == '\t');
    if (c == EOF) {
	return (0);
    }
    if (c == '"') {
	i = 0;
	while ((c = getcharstr()) != '"' && c != EOF) {
	    if (c == '\\') {
		int ctmp;
		ctmp = getcharstr();
		if (ctmp != '"') {
		    ungetchstr();
		}
		else {
		    c = ctmp;
		}
	    }
	    sbuf[i] = c;
	    i++;
	}
	if (c == EOF) {
	    yyerror("Nonterminating string");
	    return 0;
	}
	sbuf[i] = '\0';
	yylval.sval = copy_string(NULL, sbuf);
	return CHRSTR;
    }
    if (c == '.' || isdigit(c)) {
	double d;
	int i, gotdot = 0;

	i = 0;
	while (c == '.' || isdigit(c)) {
	    if (c == '.') {
		if (gotdot) {
		    yyerror("Reading number, too many dots");
	    	    return 0;
		} else {
		    gotdot = 1;
		}
	    }
	    sbuf[i++] = c;
	    c = getcharstr();
	}
	if (c == 'E' || c == 'e') {
	    sbuf[i++] = c;
	    c = getcharstr();
	    if (c == '+' || c == '-') {
		sbuf[i++] = c;
		c = getcharstr();
	    }
	    while (isdigit(c)) {
		sbuf[i++] = c;
		c = getcharstr();
	    }
	}
	if (gotdot && i == 1) {
	    ungetchstr();
	    return '.';
	}
	sbuf[i] = '\0';
	ungetchstr();
	sscanf(sbuf, "%lf", &d);
	yylval.dval = d;
	return NUMBER;
    }
/* graphs, sets, regions resp. */
    if (c == 'G' || c == 'S' || c == 'R') {
	int i = 0, ctmp = c, gn, sn, rn;
	c = getcharstr();
	while (isdigit(c) || c == '$' || c == '_') {
	    sbuf[i++] = c;
	    c = getcharstr();
	}
	if (i == 0) {
	    c = ctmp;
	    ungetchstr();
	} else {
	    ungetchstr();
	    if (ctmp == 'G') {
	        sbuf[i] = '\0';
		if (i == 1 && sbuf[0] == '_') {
                    gn = get_recent_gno();
                } else if (i == 1 && sbuf[0] == '$') {
                    gn = whichgraph;
                } else {
                    gn = atoi(sbuf);
                }
		if (is_valid_gno(gn) || graph_allocate(gn) == RETURN_SUCCESS) {
		    yylval.ival = gn;
		    return GRAPHNO;
		}
	    } else if (ctmp == 'S') {
	        sbuf[i] = '\0';
		if (i == 1 && sbuf[0] == '_') {
                    sn = get_recent_setno();
                } else if (i == 1 && sbuf[0] == '$') {
                    sn = whichset;
                } else {
		    sn = atoi(sbuf);
                }
		yylval.ival = sn;
		return SETNUM;
	    } else if (ctmp == 'R') {
	        sbuf[i] = '\0';
		rn = atoi(sbuf);
		if (rn >= 0 && rn < MAXREGION) {
		    yylval.ival = rn;
		    return REGNUM;
		} else {
                    errmsg("Invalid region number");
                }
	    }
	}
    }
    if (isalpha(c) || c == '$') {
	char *p = sbuf;

	do {
	    *p++ = c;
	} while ((c = getcharstr()) != EOF && (isalpha(c) || isdigit(c) ||
                  c == '_' || c == '$'));
	ungetchstr();
	*p = '\0';
#ifdef DEBUG
        if (get_debuglevel() == 2) {
	    printf("->%s<-\n", sbuf);
	}
#endif
	found = -1;
	if ((found = findf(key, sbuf)) >= 0) {
	    if (key[found].type == FITPARM) {
		int index = sbuf[1] - '0';
		yylval.ival = index;
		return FITPARM;
	    }
	    else if (key[found].type == FITPMAX) {
		int index = sbuf[1] - '0';
		yylval.ival = index;
		return FITPMAX;
	    }
	    else if (key[found].type == FITPMIN) {
		int index = sbuf[1] - '0';
		yylval.ival = index;
		return FITPMIN;
	    }

	    else if (key[found].type == KEY_VAR) {
		yylval.dptr = (double *) key[found].data;
		return VAR_D;
	    }
	    else if (key[found].type == KEY_VEC) {
		yylval.vrbl = (grarr *) key[found].data;
		return VEC_D;
	    }

	    else if (key[found].type == FUNC_I) {
		yylval.ival = found;
		return FUNC_I;
	    }
	    else if (key[found].type == CONSTANT) {
		yylval.ival = found;
		return CONSTANT;
	    }
	    else if (key[found].type == UCONSTANT) {
		yylval.ival = found;
		return UCONSTANT;
	    }
	    else if (key[found].type == FUNC_D) {
		yylval.ival = found;
		return FUNC_D;
	    }
	    else if (key[found].type == FUNC_ND) {
		yylval.ival = found;
		return FUNC_ND;
	    }
	    else if (key[found].type == FUNC_DD) {
		yylval.ival = found;
		return FUNC_DD;
	    }
	    else if (key[found].type == FUNC_NND) {
		yylval.ival = found;
		return FUNC_NND;
	    }
	    else if (key[found].type == FUNC_PPD) {
		yylval.ival = found;
		return FUNC_PPD;
	    }
	    else if (key[found].type == FUNC_PPPD) {
		yylval.ival = found;
		return FUNC_PPPD;
	    }
	    else if (key[found].type == FUNC_PPPPD) {
		yylval.ival = found;
		return FUNC_PPPPD;
	    }
	    else if (key[found].type == FUNC_PPPPPD) {
		yylval.ival = found;
		return FUNC_PPPPPD;
	    }
	    else {
	        yylval.ival = key[found].type;
	        return key[found].type;
	    }
	} else {
	    yylval.sval = copy_string(NULL, sbuf);
	    return NEW_TOKEN;
	}
    }
    switch (c) {
    case '>':
	return follow('=', GE, GT);
    case '<':
	return follow('=', LE, LT);
    case '=':
	return follow('=', EQ, '=');
    case '!':
	return follow('=', NE, NOT);
    case '|':
	return follow('|', OR, '|');
    case '&':
	return follow('&', AND, '&');
    case '\n':
	return '\n';
    default:
	return c;
    }
}

static int follow(int expect, int ifyes, int ifno)
{
    int c = getcharstr();

    if (c == expect) {
	return ifyes;
    }
    ungetchstr();
    return ifno;
}

static void yyerror(char *s)
{
    char *buf;
    
    buf = copy_string(NULL, s);
    buf = concat_strings(buf, ": ");
    buf = concat_strings(buf, f_string);
    errmsg(buf);
    xfree(buf);
    interr = 1;
}
