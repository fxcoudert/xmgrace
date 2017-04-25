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
 * constants and typedefs
 *
 */
#ifndef __DEFINES_H_
#define __DEFINES_H_

#include <config.h>

/*
 * some constants
 *
 */

/* max path length */
#define GR_MAXPATHLEN 256

/* max length for strings */
#define MAX_STRING_LENGTH 512


#define MAXAXES 4               /* max number of axes per graph */
#define MAX_TICKS 256           /* max number of ticks/labels per axis */
#define MAXREGION 5             /* max number of regions */

#define MAX_ZOOM_STACK 20       /* max stack depth for world stack */

#define MAXPARM 10              /* max number of parameters for non-lin fit */

#define MAXFIT 12               /* max degree of polynomial+1 that can be
                                 * fitted */


/* number of extra objects of a given type to allocate if not enough */
#define OBJECT_BUFNUM 10


#define MAX_ARROW 3
#define MAX_PREC 10

/* symbol types */

#define SYM_NONE    0
#define SYM_CIRCLE  1
#define SYM_SQUARE  2
#define SYM_DIAMOND 3
#define SYM_TRIANG1 4
#define SYM_TRIANG2 5
#define SYM_TRIANG3 6
#define SYM_TRIANG4 7
#define SYM_PLUS    8
#define SYM_X       9
#define SYM_SPLAT  10
#define SYM_CHAR   11

/* max number of symbols defined */
#define MAXSYM  12

/* dot (obsolete) */
#define SYM_DOT_OBS     1

/*
 * types of coordinate frames
 */
#define COORDINATES_XY      0       /* Cartesian coordinates */
#define COORDINATES_POLAR   1       /* Polar coordinates */
                                
/*
 * types of axis scale mappings
 */
#define SCALE_NORMAL    0       /* normal linear scale */
#define SCALE_LOG       1       /* logarithmic  scale */
#define SCALE_REC       2       /* reciprocal, reserved */
#define SCALE_LOGIT	  3	  /* logit scale */

/*
 * coordinates
 */
#define AXIS_TYPE_ANY -1
#define AXIS_TYPE_X    0
#define AXIS_TYPE_Y    1
#define AXIS_TYPE_BAD  2

/*
 * types of axes
 */
#define ALL_AXES    -3
#define ALL_X_AXES  -2
#define ALL_Y_AXES  -1

#define X_AXIS  0
#define Y_AXIS  1
#define ZX_AXIS 2
#define ZY_AXIS 3


/* setno == all sets selected */
#define ALL_SETS    -1
/* setno == new set to be created */
#define NEW_SET     -2

/*
 * gno == all graphs selected
 */
#define ALL_GRAPHS    -1

/* type of splines */
#define INTERP_LINEAR   0
#define INTERP_SPLINE   1
#define INTERP_ASPLINE  2

/* Canvas types */
#define PAGE_FREE       0
#define PAGE_FIXED      1

/* Strings and things */
#define OBJECT_NONE    -1
#define OBJECT_LINE     0
#define OBJECT_BOX      1
#define OBJECT_ELLIPSE  2
#define OBJECT_STRING   3

/* Region definitions */
#define REGION_ABOVE    0
#define REGION_BELOW    1
#define REGION_TOLEFT   2
#define REGION_TORIGHT  3
#define REGION_POLYI    4
#define REGION_POLYO    5
#define REGION_HORIZI   6
#define REGION_VERTI    7
#define REGION_HORIZO   8
#define REGION_VERTO    9

/* Axis label layout */
#define LAYOUT_PARALLEL         0
#define LAYOUT_PERPENDICULAR    1

/* Placement (axis labels, ticks, error bars */
typedef enum {
    PLACEMENT_NORMAL,
    PLACEMENT_OPPOSITE,
    PLACEMENT_BOTH
} PlacementType;

/* Tick label placement */
#define LABEL_ONTICK    0
#define LABEL_BETWEEN   1

/* Coordinates */
#define COORD_VIEW      0
#define COORD_WORLD     1

/* Tick sign type */
#define SIGN_NORMAL     0
#define SIGN_ABSOLUTE   1
#define SIGN_NEGATE     2


/* Tick label/display formats */
#define FORMAT_INVALID         -1
#define FORMAT_DECIMAL          0
#define FORMAT_EXPONENTIAL      1
#define FORMAT_GENERAL          2
#define FORMAT_POWER            3
#define FORMAT_SCIENTIFIC       4
#define FORMAT_ENGINEERING      5
#define FORMAT_COMPUTING        6
#define FORMAT_DDMMYY           7
#define FORMAT_MMDDYY           8
#define FORMAT_YYMMDD           9
#define FORMAT_MMYY            10
#define FORMAT_MMDD            11
#define FORMAT_MONTHDAY        12
#define FORMAT_DAYMONTH        13
#define FORMAT_MONTHS          14
#define FORMAT_MONTHSY         15
#define FORMAT_MONTHL          16
#define FORMAT_DAYOFWEEKS      17
#define FORMAT_DAYOFWEEKL      18
#define FORMAT_DAYOFYEAR       19
#define FORMAT_HMS             20
#define FORMAT_MMDDHMS         21
#define FORMAT_MMDDYYHMS       22
#define FORMAT_YYMMDDHMS       23
#define FORMAT_DEGREESLON      24
#define FORMAT_DEGREESMMLON    25
#define FORMAT_DEGREESMMSSLON  26
#define FORMAT_MMSSLON         27
#define FORMAT_DEGREESLAT      28
#define FORMAT_DEGREESMMLAT    29
#define FORMAT_DEGREESMMSSLAT  30
#define FORMAT_MMSSLAT         31

/* Focus policy */
#define FOCUS_CLICK     0
#define FOCUS_SET       1
#define FOCUS_FOLLOWS   2

/* Placement of labels etc */
#define TYPE_AUTO       0
#define TYPE_SPEC       1

/* User-defined tickmarks/labels */
#define TICKS_SPEC_NONE     0
#define TICKS_SPEC_MARKS    1
#define TICKS_SPEC_BOTH     2

/* Tick direction */
#define TICKS_IN        0
#define TICKS_OUT       1
#define TICKS_BOTH      2

/* Data source type */
#define SOURCE_DISK     0
#define SOURCE_PIPE     1


/* Types of running command */
#define RUN_AVG         0
#define RUN_MED         1
#define RUN_MIN         2
#define RUN_MAX         3
#define RUN_STD         4

/* Types of Fourier transforms */
#define FFT_FFT         0
#define FFT_INVFFT      1
#define FFT_DFT         2
#define FFT_INVDFT      3

/* return codes */
#define RETURN_SUCCESS (0)
#define RETURN_FAILURE (1)

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* types of autscales */
#define AUTOSCALE_NONE    0
#define AUTOSCALE_X       1
#define AUTOSCALE_Y       2
#define AUTOSCALE_XY      3

/*
 * for set selector gadgets
 */
#define SET_SELECT_ERROR -99
#define SET_SELECT_ACTIVE 0
#define SET_SELECT_ALL -1
#define SET_SELECT_NEXT -2
#define SET_SELECT_NEAREST -3
#define GRAPH_SELECT_CURRENT -1
#define GRAPH_SELECT_ALL -2
#define FILTER_SELECT_NONE 0
#define FILTER_SELECT_ACTIVE 1
#define FILTER_SELECT_ALL 2
#define FILTER_SELECT_INACT 3
#define FILTER_SELECT_DEACT 4
#define FILTER_SELECT_SORT 5
#define SELECTION_TYPE_SINGLE 0
#define SELECTION_TYPE_MULTIPLE 1

/* Default document name */
#define NONAME "Untitled"

/* for data pruning */
#define PRUNE_INTERPOLATION     0
#define PRUNE_CIRCLE            1
#define PRUNE_ELLIPSE           2
#define PRUNE_RECTANGLE         3

#define PRUNE_LIN               0
#define PRUNE_LOG               1

#define PRUNE_VIEWPORT          0
#define PRUNE_WORLD             1


/* for io filters */
#define FILTER_INPUT    0
#define FILTER_OUTPUT   1

#define FILTER_MAGIC    0
#define FILTER_PATTERN  1

/* set line types */
#define LINE_TYPE_NONE          0
#define LINE_TYPE_STRAIGHT      1
#define LINE_TYPE_LEFTSTAIR     2
#define LINE_TYPE_RIGHTSTAIR    3
#define LINE_TYPE_SEGMENT2      4
#define LINE_TYPE_SEGMENT3      5

/* baseline types */
#define BASELINE_TYPE_0         0
#define BASELINE_TYPE_SMIN      1
#define BASELINE_TYPE_SMAX      2
#define BASELINE_TYPE_GMIN      3
#define BASELINE_TYPE_GMAX      4
#define BASELINE_TYPE_SAVG      5

/* set fill types */
#define SETFILL_NONE            0
#define SETFILL_POLYGON         1
#define SETFILL_BASELINE        2

/* types of ann. values */
#define AVALUE_TYPE_NONE        0
#define AVALUE_TYPE_X           1
#define AVALUE_TYPE_Y           2
#define AVALUE_TYPE_XY          3
#define AVALUE_TYPE_STRING      4
#define AVALUE_TYPE_Z           5

/* ticks */
#define TICK_TYPE_MAJOR     0
#define TICK_TYPE_MINOR     1

/* push set direction */
#define PUSH_SET_TOFRONT    0
#define PUSH_SET_TOBACK     1

/* restriction types */
#define RESTRICT_NONE  -1
#define RESTRICT_WORLD -2
#define RESTRICT_REG0   0
#define RESTRICT_REG1   1
#define RESTRICT_REG2   2
#define RESTRICT_REG3   3
#define RESTRICT_REG4   4


/*
 * defaults
 */
typedef struct {
    int color;
    int bgcolor;
    int pattern;
    int lines;
    double linew;
    double charsize;
    int font;
    double symsize;
} defaults;

typedef struct {
    int color;
    int pattern;
/*
 *     int transparency;
 */
} Pen;

/* A point in world coordinates */
typedef struct {
    double x;
    double y;
} WPoint;


/* A point in viewport coordinates */
typedef struct {
    double x;
    double y;
} VPoint;

typedef struct {
    double x;
    double y;
} VVector;

typedef struct {
    double xg1, xg2, yg1, yg2;  /* window into world coords */
} world;

typedef struct {
    double xv1, xv2, yv1, yv2;  /* viewport */
} view;


/*
 * typedefs for objects
 */
typedef struct {
    int active;
    int loctype;
    int gno;
    double x1;
    double y1;
    double x2;
    double y2;
    int lines;
    double linew;
    int color;
    int fillcolor;
    int fillpattern;
    view bb;
} boxtype;

typedef struct {
    int type;
    double length;  /* head length (L) */
    double dL_ff;   /* d/L form factor */
    double lL_ff;   /* l/L form factor */
} Arrow;

typedef struct {
    int active;
    int loctype;
    int gno;
    double x1;
    double y1;
    double x2;
    double y2;
    int lines;
    double linew;
    int color;
    int arrow_end;
    Arrow arrow;
    view bb;
} linetype;

typedef struct {
    int active;
    int loctype;
    int gno;
    double x1;
    double y1;
    double x2;
    double y2;
    int lines;
    double linew;
    int color;
    int fillcolor;
    int fillpattern;
    view bb;
} ellipsetype;

typedef struct {
    int active;
    int loctype;
    int gno;
    double x;
    double y;
    int color;
    int rot;
    int font;
    int just;
    double charsize;
    char *s;
    view bb;
} plotstr;


/*
 * world stack
 */
typedef struct {
    world w;                    /* current world */
} world_stack;

typedef struct {
    plotstr title;              /* graph title */
    plotstr stitle;             /* graph subtitle */
} labels;

typedef struct {
    int active;                 /* active flag */
    int type;                   /* regression type */
    double xmin;
    double xmax;
    double coef[15];
} Regression;

typedef struct {
    int active;                 /* active flag */
    int type;                   /* regression type */
    int npts;                   /* number of points */
    double xmin;
    double xmax;
    double *a;
    double *b;
    double *c;
    double *d;
} Spline;

typedef struct {
    int active;          /* on/off */
    PlacementType ptype; /* placement type */
    Pen pen;             /* pen */
    double linew;        /* error bar line width */
    int lines;           /* error bar line style */
    double riser_linew;  /* connecting line between error limits line width */
    int riser_lines;     /* connecting line between error limits line style */
    double barsize;      /* size of error bar */
    int arrow_clip;      /* draw arrows if clipped */
    double cliplen;      /* riser clipped length (v.p.) */
} Errbar;

/* Annotative strings for data values */
typedef struct {
    int active;                 /* active or not */
    int type;                   /* type */
    double size;                /* char size */
    int font;                   /* font */
    int color;                  /* color */
    int angle;                  /* angle */
    int format;                 /* format */
    int prec;                   /* precision */
    char prestr[64];            /* prepend string */
    char appstr[64];            /* append string */
    VPoint offset;              /* offset related to symbol position */
} AValue;



typedef struct {
    int type;
    double wtpos;
    char *label;
} tickloc;

typedef struct {
    double size;              /* length of tickmarks */
    int color;                /* color of tickmarks */
    double linew;             /* linewidth of tickmarks */
    int lines;                /* linestyle of tickmarks */
    int gridflag;             /* grid lines at tick marks */
} tickprops;

typedef struct {
    int active;                 /* active or not */

    int zero;                   /* "zero" axis or plain */

    plotstr label;              /* graph axis label */
    int label_layout;           /* axis label orientation (h or v) */
    int label_place;            /* axis label placement (specfied or auto) */
    PlacementType label_op;     /* tick labels on opposite side or both */

    int t_drawbar;              /* draw a bar connecting tick marks */
    int t_drawbarcolor;         /* color of bar */
    int t_drawbarlines;         /* linestyle of bar */
    double t_drawbarlinew;      /* line width of bar */

    double offsx, offsy;        /* offset of axes in viewport coords
                                   (attention: these
				   are not x and y coordinates but
				   perpendicular and parallel offsets */

    int t_flag;                 /* toggle tickmark display */
    int t_autonum;              /* approximate default number of major ticks */

    int t_spec;                 /* special (user-defined) tickmarks/ticklabels, */
                                /* can be none/marks/both marks and labels */

    int t_round;                /* place major ticks at rounded positions */

    double tmajor;              /* major tick divisions */
    int nminor;                 /* number of minor ticks per one major division */

    int nticks;                 /* total number of ticks */
    tickloc tloc[MAX_TICKS];    /* locations of ticks */

    int t_inout;                /* ticks inward, outward or both */
    PlacementType t_op;         /* ticks on opposite side */
    
    tickprops props;
    tickprops mprops;

    int tl_flag;                /* toggle ticmark labels on or off */
    int tl_angle;               /* angle to draw labels */

    int tl_format;              /* tickmark label format */
    int tl_prec;                /* places to right of decimal point */

    char *tl_formula;           /* transformation formula */

    int tl_skip;                /* tick labels to skip */
    int tl_staggered;           /* tick labels staggered */
    int tl_starttype;           /* start at graphmin or use tl_start/stop */
    int tl_stoptype;            /* start at graphmax or use tl_start/stop */
    double tl_start;            /* value of x to begin tick labels and major ticks */
    double tl_stop;             /* value of x to end tick labels and major ticks */

    PlacementType tl_op;        /* tick labels on opposite side or both */

    int tl_gaptype;             /* tick label placement auto or specified */
    VVector tl_gap;             /* tick label to tickmark distance
				   (parallel and perpendicular to axis) */

    int tl_font;                /* font to use for tick labels */
    double tl_charsize;         /* character size for tick labels */
    int tl_color;               /* color of tick labels */

    char tl_appstr[64];         /* append string to tick label */
    char tl_prestr[64];         /* prepend string to tick label */

} tickmarks;

typedef struct {
    int active;                 /* legend on or off */
    int loctype;                /* locate in world or viewport coords */
    int vgap;                   /* verticle gap between entries */
    int hgap;                   /* horizontal gap(s) between legend string
                                                                  elements */
    int len;                    /* length of line to draw */
    int invert;                 /* switch between ascending and descending
                                   order of set legends */
    double legx;                /* location on graph */
    double legy;
    int font;
    double charsize;
    int color;
    Pen boxpen;
    Pen boxfillpen;
    double boxlinew;            /* legend frame line width */
    int boxlines;               /* legend frame line style */
    view bb;
} legend;

typedef struct {
    int active;                 /* region on or off */
    int type;                   /* region type */
    int color;                  /* region color */
    int lines;                  /* region linestyle */
    double linew;               /* region line width */
    int linkto;                 /* associated with graph linkto */
    int n;                      /* number of points if type is POLY */
    double *x, *y;              /* coordinates if type is POLY */
    double x1, y1, x2, y2;      /* starting and ending points if type is not POLY */
} region;

typedef struct {
    int type;                   /* frame type */
    Pen pen;                    /* frame pen */
    int lines;                  /* frame linestyle */
    double linew;                  /* frame line width */
    Pen fillpen;                /* fill pen */
} framep;


/* parameters for non-linear fit */
typedef struct {
    double value;       /* parameter itself */
    int constr;         /* whether or not to use constraints */
    double min;         /* low bound constraint */
    double max;         /* upper bound constraint */
} nonlparms;

/* options for non-linear fit */
typedef struct {
    char *title;        /* fit title */
    char *formula;      /* fit function */
    int parnum;         /* # of fit parameters */
    double tolerance;   /* tolerance */
} nonlopts;

/* real time inputs */
typedef struct _Input_buffer {
    int           fd;     /* file descriptor */
    int           errors; /* number of successive parse errors */
    int           lineno; /* line number */
    int           zeros;  /* number of successive reads of zero byte */
    int           reopen; /* non-zero if we should close and reopen */
                          /* when other side is closed (mainly for fifos) */
    char         *name;   /* name of the input (filename or symbolic name) */
    int           size;   /* size of the buffer for already read lines */
    int           used;   /* number of bytes used in the buffer */
    char         *buf;    /* buffer for already read lines */
    unsigned long id;     /* id for X library */
} Input_buffer;

/* dates formats */
typedef enum   { FMT_iso,
                 FMT_european,
                 FMT_us,
                 FMT_nohint
               } Dates_format;

/* rounding types for dates */
#define ROUND_SECOND 1
#define ROUND_MINUTE 2
#define ROUND_HOUR   3
#define ROUND_DAY    4
#define ROUND_MONTH  5

/* tokens for the calendar dates parser */
typedef struct { int value;
                 int digits;
               } Int_token;

#endif /* __DEFINES_H_ */
