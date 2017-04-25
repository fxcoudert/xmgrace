/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "pars.yacc" /* yacc.c:339  */

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


#line 207 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    KEY_VAR = 258,
    KEY_VEC = 259,
    KEY_CONST = 260,
    KEY_UNIT = 261,
    KEY_FUNC_I = 262,
    KEY_FUNC_D = 263,
    KEY_FUNC_NN = 264,
    KEY_FUNC_ND = 265,
    KEY_FUNC_DD = 266,
    KEY_FUNC_NND = 267,
    KEY_FUNC_PPD = 268,
    KEY_FUNC_PPPD = 269,
    KEY_FUNC_PPPPD = 270,
    KEY_FUNC_PPPPPD = 271,
    INDEX = 272,
    DATE = 273,
    VAR_D = 274,
    VEC_D = 275,
    CONSTANT = 276,
    UCONSTANT = 277,
    FUNC_I = 278,
    FUNC_D = 279,
    FUNC_NN = 280,
    FUNC_ND = 281,
    FUNC_DD = 282,
    FUNC_NND = 283,
    FUNC_PPD = 284,
    FUNC_PPPD = 285,
    FUNC_PPPPD = 286,
    FUNC_PPPPPD = 287,
    ABOVE = 288,
    ABSOLUTE = 289,
    ALIAS = 290,
    ALT = 291,
    ALTXAXIS = 292,
    ALTYAXIS = 293,
    ANGLE = 294,
    ANTIALIASING = 295,
    APPEND = 296,
    ARRANGE = 297,
    ARROW = 298,
    ASCENDING = 299,
    ASPLINE = 300,
    AUTO = 301,
    AUTOSCALE = 302,
    AUTOTICKS = 303,
    AVALUE = 304,
    AVG = 305,
    BACKGROUND = 306,
    BAR = 307,
    BARDY = 308,
    BARDYDY = 309,
    BASELINE = 310,
    BATCH = 311,
    BEGIN = 312,
    BELOW = 313,
    BETWEEN = 314,
    BLACKMAN = 315,
    BLOCK = 316,
    BOTH = 317,
    BOTTOM = 318,
    BOX = 319,
    CD = 320,
    CENTER = 321,
    CHAR = 322,
    CHART = 323,
    CHRSTR = 324,
    CLEAR = 325,
    CLICK = 326,
    CLIP = 327,
    CLOSE = 328,
    COEFFICIENTS = 329,
    COLOR = 330,
    COMMENT = 331,
    COMPLEX = 332,
    COMPUTING = 333,
    CONSTRAINTS = 334,
    COPY = 335,
    CYCLE = 336,
    DAYMONTH = 337,
    DAYOFWEEKL = 338,
    DAYOFWEEKS = 339,
    DAYOFYEAR = 340,
    DDMMYY = 341,
    DECIMAL = 342,
    DEF = 343,
    DEFAULT = 344,
    DEFINE = 345,
    DEGREESLAT = 346,
    DEGREESLON = 347,
    DEGREESMMLAT = 348,
    DEGREESMMLON = 349,
    DEGREESMMSSLAT = 350,
    DEGREESMMSSLON = 351,
    DESCENDING = 352,
    DESCRIPTION = 353,
    DEVICE = 354,
    DFT = 355,
    DIFFERENCE = 356,
    DISK = 357,
    DOWN = 358,
    DPI = 359,
    DROP = 360,
    DROPLINE = 361,
    ECHO = 362,
    ELLIPSE = 363,
    ENGINEERING = 364,
    ERRORBAR = 365,
    EXIT = 366,
    EXPONENTIAL = 367,
    FFT = 368,
    FILEP = 369,
    FILL = 370,
    FIT = 371,
    FIXED = 372,
    FIXEDPOINT = 373,
    FLUSH = 374,
    FOCUS = 375,
    FOLLOWS = 376,
    FONTP = 377,
    FORCE = 378,
    FORMAT = 379,
    FORMULA = 380,
    FRAMEP = 381,
    FREE = 382,
    FREQUENCY = 383,
    FROM = 384,
    GENERAL = 385,
    GETP = 386,
    GRAPH = 387,
    GRAPHNO = 388,
    GRID = 389,
    HAMMING = 390,
    HANNING = 391,
    HARDCOPY = 392,
    HBAR = 393,
    HELP = 394,
    HGAP = 395,
    HIDDEN = 396,
    HISTOGRAM = 397,
    HMS = 398,
    HORIZI = 399,
    HORIZONTAL = 400,
    HORIZO = 401,
    ID = 402,
    IFILTER = 403,
    IMAX = 404,
    IMIN = 405,
    IN = 406,
    INCREMENT = 407,
    INOUT = 408,
    INT = 409,
    INTEGRATE = 410,
    INTERPOLATE = 411,
    INVDFT = 412,
    INVERT = 413,
    INVFFT = 414,
    JUST = 415,
    KILL = 416,
    LABEL = 417,
    LANDSCAPE = 418,
    LAYOUT = 419,
    LEFT = 420,
    LEGEND = 421,
    LENGTH = 422,
    LINCONV = 423,
    LINE = 424,
    LINEAR = 425,
    LINESTYLE = 426,
    LINEWIDTH = 427,
    LINK = 428,
    LOAD = 429,
    LOCTYPE = 430,
    LOG = 431,
    LOGARITHMIC = 432,
    LOGIT = 433,
    LOGX = 434,
    LOGXY = 435,
    LOGY = 436,
    MAGIC = 437,
    MAGNITUDE = 438,
    MAJOR = 439,
    MAP = 440,
    MAXP = 441,
    MESH = 442,
    MINP = 443,
    MINOR = 444,
    MMDD = 445,
    MMDDHMS = 446,
    MMDDYY = 447,
    MMDDYYHMS = 448,
    MMSSLAT = 449,
    MMSSLON = 450,
    MMYY = 451,
    MONTHDAY = 452,
    MONTHL = 453,
    MONTHS = 454,
    MONTHSY = 455,
    MOVE = 456,
    NEGATE = 457,
    NEW = 458,
    NONE = 459,
    NONLFIT = 460,
    NORMAL = 461,
    NXY = 462,
    OFF = 463,
    OFFSET = 464,
    OFFSETX = 465,
    OFFSETY = 466,
    OFILTER = 467,
    ON = 468,
    ONREAD = 469,
    OP = 470,
    OPPOSITE = 471,
    OUT = 472,
    PAGE = 473,
    PARA = 474,
    PARAMETERS = 475,
    PARZEN = 476,
    PATTERN = 477,
    PERIOD = 478,
    PERP = 479,
    PHASE = 480,
    PIE = 481,
    PIPE = 482,
    PLACE = 483,
    POINT = 484,
    POLAR = 485,
    POLYI = 486,
    POLYO = 487,
    POP = 488,
    PORTRAIT = 489,
    POWER = 490,
    PREC = 491,
    PREPEND = 492,
    PRINT = 493,
    PS = 494,
    PUSH = 495,
    PUTP = 496,
    RAND = 497,
    READ = 498,
    REAL = 499,
    RECIPROCAL = 500,
    REDRAW = 501,
    REFERENCE = 502,
    REGNUM = 503,
    REGRESS = 504,
    RESIZE = 505,
    RESTRICT = 506,
    REVERSE = 507,
    RIGHT = 508,
    RISER = 509,
    ROT = 510,
    ROUNDED = 511,
    RSUM = 512,
    RULE = 513,
    RUNAVG = 514,
    RUNMAX = 515,
    RUNMED = 516,
    RUNMIN = 517,
    RUNSTD = 518,
    SAVEALL = 519,
    SCALE = 520,
    SCIENTIFIC = 521,
    SCROLL = 522,
    SD = 523,
    SET = 524,
    SETNUM = 525,
    SFORMAT = 526,
    SIGN = 527,
    SIZE = 528,
    SKIP = 529,
    SLEEP = 530,
    SMITH = 531,
    SORT = 532,
    SOURCE = 533,
    SPEC = 534,
    SPLINE = 535,
    SPLIT = 536,
    STACK = 537,
    STACKED = 538,
    STACKEDBAR = 539,
    STACKEDHBAR = 540,
    STAGGER = 541,
    START = 542,
    STOP = 543,
    STRING = 544,
    SUM = 545,
    SUBTITLE = 546,
    SWAP = 547,
    SYMBOL = 548,
    TARGET = 549,
    TICKLABEL = 550,
    TICKP = 551,
    TICKSP = 552,
    TIMER = 553,
    TIMESTAMP = 554,
    TITLE = 555,
    TO = 556,
    TOP = 557,
    TRIANGULAR = 558,
    TYPE = 559,
    UP = 560,
    UPDATEALL = 561,
    USE = 562,
    VERSION = 563,
    VERTI = 564,
    VERTICAL = 565,
    VERTO = 566,
    VGAP = 567,
    VIEW = 568,
    VX1 = 569,
    VX2 = 570,
    VXMAX = 571,
    VY1 = 572,
    VY2 = 573,
    VYMAX = 574,
    WELCH = 575,
    WITH = 576,
    WORLD = 577,
    WRAP = 578,
    WRITE = 579,
    WX1 = 580,
    WX2 = 581,
    WY1 = 582,
    WY2 = 583,
    X_TOK = 584,
    X0 = 585,
    X1 = 586,
    XAXES = 587,
    XAXIS = 588,
    XCOR = 589,
    XMAX = 590,
    XMIN = 591,
    XY = 592,
    XYAXES = 593,
    XYBOXPLOT = 594,
    XYCOLOR = 595,
    XYCOLPAT = 596,
    XYDX = 597,
    XYDXDX = 598,
    XYDXDXDYDY = 599,
    XYDXDY = 600,
    XYDY = 601,
    XYDYDY = 602,
    XYHILO = 603,
    XYR = 604,
    XYSIZE = 605,
    XYSTRING = 606,
    XYVMAP = 607,
    XYZ = 608,
    Y_TOK = 609,
    Y0 = 610,
    Y1 = 611,
    Y2 = 612,
    Y3 = 613,
    Y4 = 614,
    YAXES = 615,
    YAXIS = 616,
    YEAR = 617,
    YMAX = 618,
    YMIN = 619,
    YYMMDD = 620,
    YYMMDDHMS = 621,
    ZERO = 622,
    ZNORM = 623,
    FITPARM = 624,
    FITPMAX = 625,
    FITPMIN = 626,
    NUMBER = 627,
    NEW_TOKEN = 628,
    OR = 629,
    AND = 630,
    GT = 631,
    LT = 632,
    LE = 633,
    GE = 634,
    EQ = 635,
    NE = 636,
    UMINUS = 637,
    NOT = 638
  };
#endif
/* Tokens.  */
#define KEY_VAR 258
#define KEY_VEC 259
#define KEY_CONST 260
#define KEY_UNIT 261
#define KEY_FUNC_I 262
#define KEY_FUNC_D 263
#define KEY_FUNC_NN 264
#define KEY_FUNC_ND 265
#define KEY_FUNC_DD 266
#define KEY_FUNC_NND 267
#define KEY_FUNC_PPD 268
#define KEY_FUNC_PPPD 269
#define KEY_FUNC_PPPPD 270
#define KEY_FUNC_PPPPPD 271
#define INDEX 272
#define DATE 273
#define VAR_D 274
#define VEC_D 275
#define CONSTANT 276
#define UCONSTANT 277
#define FUNC_I 278
#define FUNC_D 279
#define FUNC_NN 280
#define FUNC_ND 281
#define FUNC_DD 282
#define FUNC_NND 283
#define FUNC_PPD 284
#define FUNC_PPPD 285
#define FUNC_PPPPD 286
#define FUNC_PPPPPD 287
#define ABOVE 288
#define ABSOLUTE 289
#define ALIAS 290
#define ALT 291
#define ALTXAXIS 292
#define ALTYAXIS 293
#define ANGLE 294
#define ANTIALIASING 295
#define APPEND 296
#define ARRANGE 297
#define ARROW 298
#define ASCENDING 299
#define ASPLINE 300
#define AUTO 301
#define AUTOSCALE 302
#define AUTOTICKS 303
#define AVALUE 304
#define AVG 305
#define BACKGROUND 306
#define BAR 307
#define BARDY 308
#define BARDYDY 309
#define BASELINE 310
#define BATCH 311
#define BEGIN 312
#define BELOW 313
#define BETWEEN 314
#define BLACKMAN 315
#define BLOCK 316
#define BOTH 317
#define BOTTOM 318
#define BOX 319
#define CD 320
#define CENTER 321
#define CHAR 322
#define CHART 323
#define CHRSTR 324
#define CLEAR 325
#define CLICK 326
#define CLIP 327
#define CLOSE 328
#define COEFFICIENTS 329
#define COLOR 330
#define COMMENT 331
#define COMPLEX 332
#define COMPUTING 333
#define CONSTRAINTS 334
#define COPY 335
#define CYCLE 336
#define DAYMONTH 337
#define DAYOFWEEKL 338
#define DAYOFWEEKS 339
#define DAYOFYEAR 340
#define DDMMYY 341
#define DECIMAL 342
#define DEF 343
#define DEFAULT 344
#define DEFINE 345
#define DEGREESLAT 346
#define DEGREESLON 347
#define DEGREESMMLAT 348
#define DEGREESMMLON 349
#define DEGREESMMSSLAT 350
#define DEGREESMMSSLON 351
#define DESCENDING 352
#define DESCRIPTION 353
#define DEVICE 354
#define DFT 355
#define DIFFERENCE 356
#define DISK 357
#define DOWN 358
#define DPI 359
#define DROP 360
#define DROPLINE 361
#define ECHO 362
#define ELLIPSE 363
#define ENGINEERING 364
#define ERRORBAR 365
#define EXIT 366
#define EXPONENTIAL 367
#define FFT 368
#define FILEP 369
#define FILL 370
#define FIT 371
#define FIXED 372
#define FIXEDPOINT 373
#define FLUSH 374
#define FOCUS 375
#define FOLLOWS 376
#define FONTP 377
#define FORCE 378
#define FORMAT 379
#define FORMULA 380
#define FRAMEP 381
#define FREE 382
#define FREQUENCY 383
#define FROM 384
#define GENERAL 385
#define GETP 386
#define GRAPH 387
#define GRAPHNO 388
#define GRID 389
#define HAMMING 390
#define HANNING 391
#define HARDCOPY 392
#define HBAR 393
#define HELP 394
#define HGAP 395
#define HIDDEN 396
#define HISTOGRAM 397
#define HMS 398
#define HORIZI 399
#define HORIZONTAL 400
#define HORIZO 401
#define ID 402
#define IFILTER 403
#define IMAX 404
#define IMIN 405
#define IN 406
#define INCREMENT 407
#define INOUT 408
#define INT 409
#define INTEGRATE 410
#define INTERPOLATE 411
#define INVDFT 412
#define INVERT 413
#define INVFFT 414
#define JUST 415
#define KILL 416
#define LABEL 417
#define LANDSCAPE 418
#define LAYOUT 419
#define LEFT 420
#define LEGEND 421
#define LENGTH 422
#define LINCONV 423
#define LINE 424
#define LINEAR 425
#define LINESTYLE 426
#define LINEWIDTH 427
#define LINK 428
#define LOAD 429
#define LOCTYPE 430
#define LOG 431
#define LOGARITHMIC 432
#define LOGIT 433
#define LOGX 434
#define LOGXY 435
#define LOGY 436
#define MAGIC 437
#define MAGNITUDE 438
#define MAJOR 439
#define MAP 440
#define MAXP 441
#define MESH 442
#define MINP 443
#define MINOR 444
#define MMDD 445
#define MMDDHMS 446
#define MMDDYY 447
#define MMDDYYHMS 448
#define MMSSLAT 449
#define MMSSLON 450
#define MMYY 451
#define MONTHDAY 452
#define MONTHL 453
#define MONTHS 454
#define MONTHSY 455
#define MOVE 456
#define NEGATE 457
#define NEW 458
#define NONE 459
#define NONLFIT 460
#define NORMAL 461
#define NXY 462
#define OFF 463
#define OFFSET 464
#define OFFSETX 465
#define OFFSETY 466
#define OFILTER 467
#define ON 468
#define ONREAD 469
#define OP 470
#define OPPOSITE 471
#define OUT 472
#define PAGE 473
#define PARA 474
#define PARAMETERS 475
#define PARZEN 476
#define PATTERN 477
#define PERIOD 478
#define PERP 479
#define PHASE 480
#define PIE 481
#define PIPE 482
#define PLACE 483
#define POINT 484
#define POLAR 485
#define POLYI 486
#define POLYO 487
#define POP 488
#define PORTRAIT 489
#define POWER 490
#define PREC 491
#define PREPEND 492
#define PRINT 493
#define PS 494
#define PUSH 495
#define PUTP 496
#define RAND 497
#define READ 498
#define REAL 499
#define RECIPROCAL 500
#define REDRAW 501
#define REFERENCE 502
#define REGNUM 503
#define REGRESS 504
#define RESIZE 505
#define RESTRICT 506
#define REVERSE 507
#define RIGHT 508
#define RISER 509
#define ROT 510
#define ROUNDED 511
#define RSUM 512
#define RULE 513
#define RUNAVG 514
#define RUNMAX 515
#define RUNMED 516
#define RUNMIN 517
#define RUNSTD 518
#define SAVEALL 519
#define SCALE 520
#define SCIENTIFIC 521
#define SCROLL 522
#define SD 523
#define SET 524
#define SETNUM 525
#define SFORMAT 526
#define SIGN 527
#define SIZE 528
#define SKIP 529
#define SLEEP 530
#define SMITH 531
#define SORT 532
#define SOURCE 533
#define SPEC 534
#define SPLINE 535
#define SPLIT 536
#define STACK 537
#define STACKED 538
#define STACKEDBAR 539
#define STACKEDHBAR 540
#define STAGGER 541
#define START 542
#define STOP 543
#define STRING 544
#define SUM 545
#define SUBTITLE 546
#define SWAP 547
#define SYMBOL 548
#define TARGET 549
#define TICKLABEL 550
#define TICKP 551
#define TICKSP 552
#define TIMER 553
#define TIMESTAMP 554
#define TITLE 555
#define TO 556
#define TOP 557
#define TRIANGULAR 558
#define TYPE 559
#define UP 560
#define UPDATEALL 561
#define USE 562
#define VERSION 563
#define VERTI 564
#define VERTICAL 565
#define VERTO 566
#define VGAP 567
#define VIEW 568
#define VX1 569
#define VX2 570
#define VXMAX 571
#define VY1 572
#define VY2 573
#define VYMAX 574
#define WELCH 575
#define WITH 576
#define WORLD 577
#define WRAP 578
#define WRITE 579
#define WX1 580
#define WX2 581
#define WY1 582
#define WY2 583
#define X_TOK 584
#define X0 585
#define X1 586
#define XAXES 587
#define XAXIS 588
#define XCOR 589
#define XMAX 590
#define XMIN 591
#define XY 592
#define XYAXES 593
#define XYBOXPLOT 594
#define XYCOLOR 595
#define XYCOLPAT 596
#define XYDX 597
#define XYDXDX 598
#define XYDXDXDYDY 599
#define XYDXDY 600
#define XYDY 601
#define XYDYDY 602
#define XYHILO 603
#define XYR 604
#define XYSIZE 605
#define XYSTRING 606
#define XYVMAP 607
#define XYZ 608
#define Y_TOK 609
#define Y0 610
#define Y1 611
#define Y2 612
#define Y3 613
#define Y4 614
#define YAXES 615
#define YAXIS 616
#define YEAR 617
#define YMAX 618
#define YMIN 619
#define YYMMDD 620
#define YYMMDDHMS 621
#define ZERO 622
#define ZNORM 623
#define FITPARM 624
#define FITPMAX 625
#define FITPMIN 626
#define NUMBER 627
#define NEW_TOKEN 628
#define OR 629
#define AND 630
#define GT 631
#define LT 632
#define LE 633
#define GE 634
#define EQ 635
#define NE 636
#define UMINUS 637
#define NOT 638

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 142 "pars.yacc" /* yacc.c:355  */

    int     ival;
    double  dval;
    char   *sval;
    double *dptr;
    target *trgt;
    grarr  *vrbl;

#line 1019 "y.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);



/* Copy the second part of user declarations.  */

#line 1034 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  523
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   11879

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  400
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  77
/* YYNRULES -- Number of rules.  */
#define YYNRULES  839
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1673

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   638

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   388,     2,     2,
     394,   395,   386,   384,   396,   385,   392,   387,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   375,   393,
       2,   399,     2,   374,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   397,     2,   398,   391,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   376,
     377,   378,   379,   380,   381,   382,   383,   389,   390
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   623,   623,   624,   628,   635,   636,   639,   641,   642,
     643,   644,   645,   646,   647,   648,   649,   650,   651,   658,
     661,   664,   667,   670,   673,   680,   714,   722,   725,   728,
     731,   734,   738,   742,   746,   750,   754,   758,   762,   766,
     770,   774,   778,   782,   786,   789,   792,   795,   798,   801,
     804,   807,   810,   813,   816,   820,   827,   834,   841,   848,
     855,   862,   869,   876,   881,   886,   889,   892,   895,   898,
     901,   904,   913,   921,   932,   935,   938,   941,   944,   947,
     950,   953,   956,   959,   964,   967,   971,   980,   990,   999,
    1009,  1012,  1027,  1030,  1046,  1050,  1063,  1079,  1083,  1105,
    1123,  1141,  1157,  1186,  1196,  1206,  1216,  1231,  1242,  1253,
    1264,  1275,  1286,  1297,  1308,  1319,  1334,  1345,  1356,  1371,
    1382,  1393,  1408,  1419,  1430,  1449,  1464,  1479,  1499,  1514,
    1530,  1553,  1572,  1591,  1601,  1610,  1623,  1636,  1649,  1664,
    1675,  1686,  1701,  1712,  1723,  1738,  1749,  1760,  1775,  1786,
    1797,  1812,  1823,  1834,  1849,  1860,  1871,  1886,  1897,  1908,
    1923,  1934,  1945,  1955,  1965,  1978,  1982,  1986,  1990,  1994,
    2005,  2032,  2043,  2053,  2070,  2078,  2089,  2093,  2097,  2102,
    2108,  2126,  2129,  2136,  2147,  2150,  2153,  2156,  2159,  2162,
    2169,  2177,  2184,  2194,  2197,  2200,  2215,  2229,  2243,  2257,
    2272,  2276,  2279,  2282,  2285,  2288,  2291,  2294,  2297,  2301,
    2306,  2310,  2314,  2317,  2322,  2326,  2331,  2334,  2341,  2348,
    2355,  2365,  2368,  2371,  2374,  2377,  2380,  2383,  2414,  2417,
    2424,  2431,  2438,  2448,  2451,  2454,  2457,  2460,  2463,  2466,
    2497,  2500,  2507,  2514,  2521,  2531,  2534,  2537,  2540,  2543,
    2546,  2549,  2552,  2556,  2573,  2576,  2583,  2590,  2597,  2605,
    2608,  2611,  2614,  2617,  2620,  2623,  2639,  2642,  2645,  2648,
    2651,  2654,  2658,  2664,  2668,  2672,  2676,  2679,  2683,  2687,
    2690,  2694,  2702,  2715,  2725,  2728,  2738,  2746,  2753,  2760,
    2767,  2775,  2782,  2789,  2797,  2804,  2811,  2818,  2825,  2828,
    2831,  2834,  2838,  2847,  2851,  2858,  2865,  2872,  2879,  2886,
    2893,  2900,  2907,  2914,  2921,  2928,  2935,  2943,  2950,  2957,
    2965,  2972,  2979,  2986,  2993,  3000,  3007,  3015,  3024,  3027,
    3030,  3033,  3037,  3041,  3044,  3048,  3052,  3056,  3060,  3065,
    3072,  3076,  3079,  3082,  3083,  3089,  3092,  3099,  3103,  3107,
    3114,  3117,  3120,  3123,  3126,  3133,  3136,  3141,  3163,  3168,
    3176,  3183,  3188,  3200,  3203,  3206,  3209,  3213,  3218,  3223,
    3226,  3236,  3239,  3242,  3245,  3248,  3251,  3255,  3258,  3261,
    3264,  3267,  3273,  3279,  3286,  3298,  3301,  3304,  3307,  3327,
    3331,  3335,  3338,  3341,  3344,  3347,  3364,  3379,  3384,  3389,
    3394,  3397,  3400,  3408,  3413,  3417,  3421,  3425,  3438,  3441,
    3447,  3454,  3458,  3462,  3469,  3477,  3485,  3494,  3502,  3506,
    3509,  3513,  3516,  3519,  3522,  3526,  3529,  3532,  3535,  3538,
    3545,  3550,  3553,  3556,  3559,  3562,  3569,  3570,  3574,  3577,
    3581,  3584,  3587,  3590,  3593,  3596,  3599,  3602,  3605,  3608,
    3611,  3615,  3619,  3623,  3627,  3631,  3636,  3640,  3644,  3662,
    3682,  3686,  3691,  3696,  3700,  3704,  3708,  3712,  3716,  3720,
    3724,  3728,  3732,  3737,  3743,  3746,  3749,  3752,  3755,  3758,
    3761,  3764,  3767,  3770,  3773,  3777,  3782,  3790,  3797,  3804,
    3805,  3806,  3807,  3808,  3809,  3810,  3811,  3822,  3829,  3836,
    3843,  3851,  3858,  3865,  3872,  3879,  3886,  3893,  3900,  3907,
    3914,  3921,  3928,  3935,  3942,  3949,  3956,  3963,  3970,  3977,
    3984,  3992,  4003,  4010,  4017,  4024,  4031,  4039,  4047,  4054,
    4061,  4068,  4075,  4084,  4091,  4098,  4105,  4112,  4119,  4126,
    4133,  4140,  4147,  4162,  4169,  4176,  4187,  4195,  4202,  4209,
    4216,  4223,  4231,  4238,  4245,  4252,  4259,  4269,  4276,  4283,
    4290,  4300,  4304,  4308,  4311,  4317,  4321,  4328,  4341,  4354,
    4367,  4383,  4384,  4388,  4389,  4390,  4391,  4395,  4396,  4397,
    4398,  4399,  4400,  4401,  4402,  4403,  4404,  4405,  4406,  4410,
    4411,  4412,  4416,  4417,  4421,  4422,  4426,  4427,  4428,  4429,
    4430,  4431,  4432,  4433,  4434,  4435,  4436,  4437,  4438,  4439,
    4440,  4441,  4442,  4443,  4444,  4448,  4449,  4450,  4451,  4452,
    4453,  4457,  4458,  4462,  4463,  4467,  4468,  4469,  4470,  4471,
    4472,  4473,  4474,  4475,  4476,  4479,  4480,  4481,  4482,  4485,
    4486,  4489,  4490,  4491,  4492,  4493,  4497,  4498,  4508,  4509,
    4510,  4513,  4514,  4515,  4518,  4519,  4520,  4521,  4522,  4523,
    4524,  4525,  4526,  4527,  4528,  4529,  4530,  4531,  4532,  4533,
    4534,  4535,  4536,  4537,  4538,  4539,  4540,  4541,  4542,  4543,
    4544,  4545,  4546,  4547,  4548,  4549,  4552,  4553,  4554,  4557,
    4558,  4559,  4560,  4561,  4562,  4565,  4566,  4569,  4570,  4571,
    4572,  4573,  4574,  4575,  4576,  4579,  4580,  4583,  4584,  4587,
    4588,  4589,  4590,  4594,  4595,  4599,  4600,  4601,  4605,  4606,
    4607,  4611,  4612,  4613,  4614,  4615,  4616,  4617,  4621,  4622,
    4623,  4626,  4627,  4628,  4629,  4630,  4631,  4632,  4636,  4640,
    4648,  4661,  4674,  4684,  4694,  4713,  4728,  4734,  4735,  4736,
    4741,  4753,  4756,  4759,  4763,  4766,  4771,  4773,  4775,  4777,
    4779,  4780,  4782,  4791,  4798,  4805,  4813,  4814,  4815,  4816,
    4818,  4820,  4824,  4828,  4834,  4840,  4845,  4850,  4857,  4864,
    4871,  4878,  4886,  4893,  4900,  4907,  4915,  4918,  4926,  4928,
    4931,  4935,  4936,  4937,  4938,  4943,  4944,  4954,  4968,  4972,
    4995,  5003,  5004,  5007,  5010,  5013,  5014,  5015,  5018,  5023,
    5031,  5032,  5033,  5034,  5035,  5036,  5043,  5052,  5064,  5071,
    5079,  5089,  5090,  5099,  5106,  5108,  5115,  5122,  5123,  5124,
    5131,  5153,  5154,  5155,  5158,  5164,  5165,  5166,  5167,  5168
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "KEY_VAR", "KEY_VEC", "KEY_CONST",
  "KEY_UNIT", "KEY_FUNC_I", "KEY_FUNC_D", "KEY_FUNC_NN", "KEY_FUNC_ND",
  "KEY_FUNC_DD", "KEY_FUNC_NND", "KEY_FUNC_PPD", "KEY_FUNC_PPPD",
  "KEY_FUNC_PPPPD", "KEY_FUNC_PPPPPD", "INDEX", "DATE", "VAR_D", "VEC_D",
  "CONSTANT", "UCONSTANT", "FUNC_I", "FUNC_D", "FUNC_NN", "FUNC_ND",
  "FUNC_DD", "FUNC_NND", "FUNC_PPD", "FUNC_PPPD", "FUNC_PPPPD",
  "FUNC_PPPPPD", "ABOVE", "ABSOLUTE", "ALIAS", "ALT", "ALTXAXIS",
  "ALTYAXIS", "ANGLE", "ANTIALIASING", "APPEND", "ARRANGE", "ARROW",
  "ASCENDING", "ASPLINE", "AUTO", "AUTOSCALE", "AUTOTICKS", "AVALUE",
  "AVG", "BACKGROUND", "BAR", "BARDY", "BARDYDY", "BASELINE", "BATCH",
  "BEGIN", "BELOW", "BETWEEN", "BLACKMAN", "BLOCK", "BOTH", "BOTTOM",
  "BOX", "CD", "CENTER", "CHAR", "CHART", "CHRSTR", "CLEAR", "CLICK",
  "CLIP", "CLOSE", "COEFFICIENTS", "COLOR", "COMMENT", "COMPLEX",
  "COMPUTING", "CONSTRAINTS", "COPY", "CYCLE", "DAYMONTH", "DAYOFWEEKL",
  "DAYOFWEEKS", "DAYOFYEAR", "DDMMYY", "DECIMAL", "DEF", "DEFAULT",
  "DEFINE", "DEGREESLAT", "DEGREESLON", "DEGREESMMLAT", "DEGREESMMLON",
  "DEGREESMMSSLAT", "DEGREESMMSSLON", "DESCENDING", "DESCRIPTION",
  "DEVICE", "DFT", "DIFFERENCE", "DISK", "DOWN", "DPI", "DROP", "DROPLINE",
  "ECHO", "ELLIPSE", "ENGINEERING", "ERRORBAR", "EXIT", "EXPONENTIAL",
  "FFT", "FILEP", "FILL", "FIT", "FIXED", "FIXEDPOINT", "FLUSH", "FOCUS",
  "FOLLOWS", "FONTP", "FORCE", "FORMAT", "FORMULA", "FRAMEP", "FREE",
  "FREQUENCY", "FROM", "GENERAL", "GETP", "GRAPH", "GRAPHNO", "GRID",
  "HAMMING", "HANNING", "HARDCOPY", "HBAR", "HELP", "HGAP", "HIDDEN",
  "HISTOGRAM", "HMS", "HORIZI", "HORIZONTAL", "HORIZO", "ID", "IFILTER",
  "IMAX", "IMIN", "IN", "INCREMENT", "INOUT", "INT", "INTEGRATE",
  "INTERPOLATE", "INVDFT", "INVERT", "INVFFT", "JUST", "KILL", "LABEL",
  "LANDSCAPE", "LAYOUT", "LEFT", "LEGEND", "LENGTH", "LINCONV", "LINE",
  "LINEAR", "LINESTYLE", "LINEWIDTH", "LINK", "LOAD", "LOCTYPE", "LOG",
  "LOGARITHMIC", "LOGIT", "LOGX", "LOGXY", "LOGY", "MAGIC", "MAGNITUDE",
  "MAJOR", "MAP", "MAXP", "MESH", "MINP", "MINOR", "MMDD", "MMDDHMS",
  "MMDDYY", "MMDDYYHMS", "MMSSLAT", "MMSSLON", "MMYY", "MONTHDAY",
  "MONTHL", "MONTHS", "MONTHSY", "MOVE", "NEGATE", "NEW", "NONE",
  "NONLFIT", "NORMAL", "NXY", "OFF", "OFFSET", "OFFSETX", "OFFSETY",
  "OFILTER", "ON", "ONREAD", "OP", "OPPOSITE", "OUT", "PAGE", "PARA",
  "PARAMETERS", "PARZEN", "PATTERN", "PERIOD", "PERP", "PHASE", "PIE",
  "PIPE", "PLACE", "POINT", "POLAR", "POLYI", "POLYO", "POP", "PORTRAIT",
  "POWER", "PREC", "PREPEND", "PRINT", "PS", "PUSH", "PUTP", "RAND",
  "READ", "REAL", "RECIPROCAL", "REDRAW", "REFERENCE", "REGNUM", "REGRESS",
  "RESIZE", "RESTRICT", "REVERSE", "RIGHT", "RISER", "ROT", "ROUNDED",
  "RSUM", "RULE", "RUNAVG", "RUNMAX", "RUNMED", "RUNMIN", "RUNSTD",
  "SAVEALL", "SCALE", "SCIENTIFIC", "SCROLL", "SD", "SET", "SETNUM",
  "SFORMAT", "SIGN", "SIZE", "SKIP", "SLEEP", "SMITH", "SORT", "SOURCE",
  "SPEC", "SPLINE", "SPLIT", "STACK", "STACKED", "STACKEDBAR",
  "STACKEDHBAR", "STAGGER", "START", "STOP", "STRING", "SUM", "SUBTITLE",
  "SWAP", "SYMBOL", "TARGET", "TICKLABEL", "TICKP", "TICKSP", "TIMER",
  "TIMESTAMP", "TITLE", "TO", "TOP", "TRIANGULAR", "TYPE", "UP",
  "UPDATEALL", "USE", "VERSION", "VERTI", "VERTICAL", "VERTO", "VGAP",
  "VIEW", "VX1", "VX2", "VXMAX", "VY1", "VY2", "VYMAX", "WELCH", "WITH",
  "WORLD", "WRAP", "WRITE", "WX1", "WX2", "WY1", "WY2", "X_TOK", "X0",
  "X1", "XAXES", "XAXIS", "XCOR", "XMAX", "XMIN", "XY", "XYAXES",
  "XYBOXPLOT", "XYCOLOR", "XYCOLPAT", "XYDX", "XYDXDX", "XYDXDXDYDY",
  "XYDXDY", "XYDY", "XYDYDY", "XYHILO", "XYR", "XYSIZE", "XYSTRING",
  "XYVMAP", "XYZ", "Y_TOK", "Y0", "Y1", "Y2", "Y3", "Y4", "YAXES", "YAXIS",
  "YEAR", "YMAX", "YMIN", "YYMMDD", "YYMMDDHMS", "ZERO", "ZNORM",
  "FITPARM", "FITPMAX", "FITPMIN", "NUMBER", "NEW_TOKEN", "'?'", "':'",
  "OR", "AND", "GT", "LT", "LE", "GE", "EQ", "NE", "'+'", "'-'", "'*'",
  "'/'", "'%'", "UMINUS", "NOT", "'^'", "'.'", "';'", "'('", "')'", "','",
  "'['", "']'", "'='", "$accept", "full_list", "multi_list", "list",
  "expr", "sexpr", "iexpr", "nexpr", "indx", "jdate", "jrawdate", "array",
  "vexpr", "asgn", "lside_array", "vasgn", "defines", "regionset",
  "parmset", "actions", "options", "set_setprop", "setprop", "axisfeature",
  "tickattr", "ticklabelattr", "axislabeldesc", "axisbardesc",
  "nonlfitopts", "selectgraph", "selectset", "setaxis", "axis", "proctype",
  "tickspectype", "filtertype", "filtermethod", "xytype", "graphtype",
  "pagelayout", "pageorient", "regiontype", "scaletype", "onoff",
  "runtype", "sourcetype", "justchoice", "inoutchoice", "formatchoice",
  "signchoice", "direction", "worldview", "datacolumn", "sortdir",
  "sorton", "ffttype", "fourierdata", "fourierloadx", "fourierloady",
  "windowtype", "interpmethod", "stattype", "font_select", "lines_select",
  "pattern_select", "color_select", "linew_select", "opchoice_sel",
  "opchoice", "parmset_obs", "axislabeldesc_obs", "setprop_obs",
  "tickattr_obs", "ticklabelattr_obs", "colpat_obs", "opchoice_sel_obs",
  "opchoice_obs", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
     605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
     615,   616,   617,   618,   619,   620,   621,   622,   623,   624,
     625,   626,   627,   628,    63,    58,   629,   630,   631,   632,
     633,   634,   635,   636,    43,    45,    42,    47,    37,   637,
     638,    94,    46,    59,    40,    41,    44,    91,    93,    61
};
# endif

#define YYPACT_NINF -900

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-900)))

#define YYTABLE_NINF -171

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-171)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    3356,  -900,  -289,  -319,   -53,  -900,  -274,  -264,  -256,  -248,
    -241,  -234,  -197,  -160,  -124,   -87,    20,  -900,  -900,   315,
     -80,    92,    79,  -900,  -900,   271,  7045,  5886,   289,    97,
     289,   315,  -900,   295,   -13,   289,   289,  -900,   -30,  9479,
    6077,   -19,  -900,   -71,  -900,   493, 11486,   202,   289,   -16,
    -900,   270,   289,   -11,  -900,  -900,    -3,     3,    14,  -900,
    -900,   -39,  4977,    19,  6268,  -144,   289,   -32,  -900,    25,
    -900,   315,   259,    31,  5250,  -900,    93,   254,  -900,   289,
      34,  4286,  -900,   413,   -50,    39,    43,   315,    44,  -900,
    -900,  -900,  -900,  -900,   289,  -900,   -16,  -900, 10864,   315,
     -58,   315,  9711,  5506,  -900,   111,   315,   315, 10864,  6459,
     169,  7045,  -900,   289, 10864,  7521,  -900,  -900,  -900,  -900,
    -900,  -900,   679,  7802,   315,  -900,  -900,  -900,  -900,  -900,
    -900,   -93,  -900,    46,  -900,  -900,  -900,  -900,  -900,  -900,
     -89,  -900, 10864,   -64,    47,    50,  -900, 10864,  8179,  8179,
    8179,   444,    71,  -900,  3199,     4,  4936,  -900,    69,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  6947, 11479,  -900,
      -7,    78,  -900,    80,    82,  -900,  -900,  -166,  9479, 10864,
   10864,  8179,  8179, 10864, 10864,  8179, 10864, 10864, 10864, 10864,
   10864,  -900,    60,   -60,    86,   180, 10864,    60,   -48,  -900,
    -900,  -900,  9856,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,   289,    88,  -900,  -900,    89,    91,   109,
     110,   114,   120,   121,   122,   124,  -900,   194, 10864, 10864,
     -27,  -900,  -900,  -900,  -900,  -900,  -900, 10864, 10864, 10864,
     370,  -291,   149,   151,  -900,  -900,  -900,  -900,   176,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
     176,   275,   304,  8578, 10864,   289,   305,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,   183,   289,   176,   -81,   315,  5319,
     176,   149,  -900,   194,   -27,   458,   149,  -900,  -900,  -900,
    -900, 10864,   289, 10864,   289, 10864,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,    -4,    60, 10864,  -900,  -900,
    -900,  -900,  -900,   176, 10864,  -900,  9479,   176,   315,  8179,
     315,   315,  -900,  -900,    86,   322,   685,   316,  9479, 10864,
      60, 10864, 10864,   -27, 10864, 10864, 10864, 10864,   633,  -900,
    -900,  -900,  -900,  -900,   315,  7947,  -900,   -27,   742,   149,
    -900,  -900,  -900,  -900,    60,   296,   176, 10864, 10864, 10864,
     297,   289,   315,   475,  -900,  -900, 10864,   273,  -900,  -900,
   10864,  -900, 10864, 11009,  -900,  5319,  -900,  -900,  -900,     9,
    8725,  -900,   176, 10864,   289,    75,    75,   176,    75,  9479,
   10864,   134, 10864,   315,  -900,  -900,  -900,  -900,   315,   315,
    -900,  8179,   176,  -900,  5319,  -262,  -900,  -900,  -900, 10864,
   10864,  -900,   328,   289, 10864,   -27, 10864,   831,   149,  -900,
    -900,  -900,  -900, 10864,   176,  -900,  -900,  -900,   225,  -900,
    -900,   334,   289, 10864,   864,  -900,  -900,  -900,  -900, 10864,
     176,  -900,  -900,  -900,  -900,  -285,  -900, 10864, 10864, 10864,
   10864,   920, 10864, 10864, 10864, 10864,    86,  -900, 10864, 10864,
   10864, 10864,  1126,   -41,    60,   148,   315,    60,   148,  5319,
      60, 10864, 10864, 10864,   218,   219,   227,   -82,   230,   227,
     230,  5995,  6185,  -900,  3718,  -900, 10864,  8179,  8179,  8179,
    8179,  8179,  8179,  8179,  8179,  8179,  8179,  8179,  8179,  8179,
    8179,   445, 10864,   229,  -900,  8179,  8179,  8179,  8179,  8179,
    8179,  8179,  8179,  8179,  8179,  8179,  8179,  8179,  8179,  8179,
    8179,   326,   491,   -72,    60,    60,    60,    45,   556,    -7,
    -900,   828,  -129,   289, 10864,    60,  1949,  7111,    60,   289,
   10864,    36,   278, 10864, 10864,  5697,  7045, 10864,   166,  -900,
    -900,  -900,  -900,   402,   730, 10864,  4436,  4749,   265,  -900,
    -900,   315,   315,  8179, 10864,  -900,  6377,   176,   247,   245,
    5319,  -900,  3199,   250,  7669,  8010,  8407,   255,   256,  1327,
    1372,   257,  1398,  1436,  1463,  1519,  -900,  9479,   176,    54,
     315,   260,  -900,  -900,  -900,  -900,  -900, 10864,   176,  -900,
     176, 10864, 10864, 10864, 10864, 10864, 10864, 10864, 10864, 10864,
    9856,  -900, 10864,  -900,  -900,  -900,  -900,  5319,  -900,  -900,
    -900,   218,   218,  8430, 10864, 10864, 10864, 10864, 10864, 10864,
   10864, 10864, 10864, 10864, 10864, 10864, 10864, 10864, 10864,  -900,
     315, 10864, 10864,   176,  -900,  -900,   176, 10864,  4035,  -156,
   10864,     6,   289,   377,   261,  -900,  -900,  -900,  -900, 10864,
     263,   176,  5319,   176,   441,  -900,  -900,  -900,  -900,   264,
    5319,   176,   267,  3199,  1580,   284,   287,  -900,   -54,  -900,
    -900,  -900,  -900,  -900, 10864,  -900,  -900,  5319,  -900,  -900,
     289,  -900,  5319,  5319, 10864,   288, 10864, 10864, 10864, 10864,
    -900,  -900, 10864,  -900,   -33,   379,   384,  1604,   291,   315,
     176,   292,    60,  7907,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,   293,  8140,   320,   299,  -900,   176, 10864, 10864,   298,
     176,   176,   289,   176,   289,   176,   289,  5319,   176,  -900,
    1625,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  1659,   301,   302,   306,  8450,  -900,  -900,    -6,
    -900,  1694, 10864,   176,  -900,  -900,  -900, 10864,  5319,   315,
   10864,   176,  -900, 10864,  5319,   987,  5319,  5319,  5319,  5319,
   10864,  -900,  -900,  -900,  -900,  5319,  5319,  5319,  5319, 10864,
     289,   289,  -900,  -900,  -900,  -900,  -900,  -900,   309,  -900,
    -900,  -900,  5319,  5319,  5319,  -900,  -900,   374,  -319,     2,
     -64,    47,    50,  -900,   -16,  9558, 11487,  3318,    74,   600,
    1068,  1503,  2039,  2066,  2039,  2066,  2039,  2066,  2039,  2066,
    2039,  2066,  2039,  2066,   106,   205,   106,   205,   227,   230,
     227,   230,   227,   230,   227,   230,  -900,  -326, 10864,  4141,
    4874,    74,   600,  1068,  1503,  2039,  2066,  2039,  2066,  2039,
    2066,  2039,  2066,  2039,  2066,  2039,  2066,   106,   205,   106,
     205,   227,   230,   227,   230,   227,   230,   227,   230,  3199,
    4936,   -28, 10864, 11486, 10864, 10864, 10864,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,   -16,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,   289,   427, 11486,
   10864, 10864,   289, 10864, 10864,  -900,  -900,  -900, 10864,  -900,
     176,   311,  -900, 10864,   179,   398, 10864,   -26,  -900,  -900,
    -900,  -900,  -900,  -900, 10864, 10864,   280,  -900,  -900,  -900,
    -900,   176,  -900, 10864,  -900,  -900,  -900,  -900,  -900,   289,
    1814,  -900,    60, 10110,  8957, 10864, 10864,  -900,  -900,  -900,
    -900,  -900,  -900,  1890,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,   435,   -36,   118,   -26,  7425,   176,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  1925, 10864,   289,   437,  4180,
     289,    68,  9102,   359, 10864,   289,    -2, 10864, 10864, 10255,
   10487,   -17,   329,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,    60,  -900, 10864,  -900,    60,  6920,   356,   357,  6692,
   10864, 10864,  -900,    40, 10864, 10632,   -15,   336,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,    60,   337,   339,  8640,
    -900, 10864,  -900,  -900,  -900,  -900,  -900, 10864,  8179,  8179,
    8179, 10864, 10864, 10864, 10864, 10864,  5319,  -900,  -900, 10864,
    8430,   343,  8682,   344,  1998,   363,  2122,  2196,  2233,  2257,
    1018,  2453,  3526,  3526,  3526,  3526,  3526,  3526,   248,   248,
     218,   218,   218,   218,  2305,  -900,  5319,  5319,  5319,  -900,
     365,  -900,  -900,   289,  5319,    60,  -900,   176, 10864, 10864,
    2475,  -900,  -900,  -900,  8179,  8179,  -900,  8179,   280,  -900,
    -900,  -900,  5319,   176,  5319,   315,  2502,  5319,  5319,  -900,
    2540,  -900,   371,   289, 10864,  -900,  -900,  8179,  -900, 10864,
   10864, 10864,  -900, 10864,  5319,  5319,  -900,   176,   176,   176,
   10864, 10864,  -900, 10864,  8324,  -900,  -900,  -900,  -900, 10864,
    5319,  5319,  -900,  5319,  5319,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,   640,  2573,  2600,
    -107,   176,   315,   229,   562, 10864, 10864,  5319,  8179,  8179,
    -900,  -900,  5319, 11486,  2639,  -900,  2676,  -900,   176, 10864,
    -900,  2699,  -900,   176,  -900,  -900,  -900, 10864,  5319,  -900,
    -900,  -900,  -900,   -10,  -900,  -900,  -900,  5319,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
     176, 10864,  -900,  -900,  -900,  -900,  -900,  -900,  5319,  -900,
   10864, 10864,  -900,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  2753, 10864,  -900,   176, 10864,  5319,  -900,   176,  -900,
    -900,  -900,  -900,  -900,  2805,   473,   476,  -900,   176,  -900,
    -900,  -900,  -900,  -900,  -900,    15,  5319,    22,  5319,  -900,
    -900,   289,  -900,  -900,  -900,    60, 10864,  2883,   376,  -900,
    -900,  -900,  -900,  -900,  -900,    60, 10864, 10864,  2883,   378,
    -900,  -900,  -900,  -900,  5319,  5319,    60,  5319,   -34,  -900,
    -900,  -900, 10864,  -900, 10864,  9334,  -900,   381,   380,  8777,
    8827,  9154,  9176,  9206,  9550,   382,  2920,  2943,  3717,  3740,
     383, 10864, 10864, 10864, 10864, 10864, 10864, 10864, 10864, 10864,
    -900,   176,  -900,   385,   387, 10864,  3763,  9578,  3858,  -900,
     388, 10864, 10864, 10864,  -310,  4305,   389,  4463,  -900,  -900,
    4492,  5319,   390,    13,  9732,  4531,   289, 10864, 10864,   289,
     394,  8538,   395,  8917,  9063,  8917,  9063,  -900, 10864, 10864,
    5319, 10864,  -900, 10864,  -900,  5319,  5319,  5319, 10864,  5319,
    5319, 10864,  -900,  -900,  -900,  -900,  -900,  -900,   176,  -900,
    5319, 10864,  -900,  5319,  -900, 10864,  -900,  -900,  -900,  -900,
    -900,  5319,   391,  -900,  -900,   399,   405, 10864,  -900,  -900,
    -900,  -900,  -900,  -900,  -900,  8179,  8179, 10864, 10864, 10864,
   10864,   407,  9945,  9977,   408,  4565,  4706,  4790,  4830,  5073,
   10864,  -900,  5094,    60,  -900,   -25,  -900,  5319,  5358,   410,
     289, 10864,  -900, 10864, 10864,  -900,    60,  -900, 10864,   -31,
    5403,  5564,   176, 10864,  -900,  5319,  5319,  5319,  5319,  5319,
    5319,  5319,  5319,  -900,  -900,    16,  -202, 10068, 10170, 10212,
   10312,  5738,  5946,  6120,  6299, 10864, 10864, 10864, 10864, 10864,
   10864, 10864,  -900, 10864,   412,  -900,  -900,  -900,   419, 10864,
   10864,   176,   400,   403,  6416,   423,  6505,   289, 10864, 10864,
     425,  -900,  -900,  -900,  -900,  -900,  -900,  -900,   428,  -900,
   10864,  -900,  -900,  -900,  -900,  8179, 10864, 10864, 10864,   424,
   10354, 10684,  6530,  6649,  6789,  5319,  5319,    60,    60,  5319,
     431,  -900,  -900, 10864,  -900, 10864,   176,  5319,  5319,    60,
      38,   432, 10704, 10731,  6818,  6941,  6964,  -900, 10864, 10864,
   10864,   452,   455, 10864,  5319,  1348,   460,  -900,  -900,  -900,
     434, 10864,  -900,  -900,  8179, 10864, 10864, 10885,  7130,  7204,
    -900,  -900,   463, 10864,  -900,   244,   464, 10907, 11031,  7241,
     335, 10864, 10864,   465,  7340,  -900,  -900,  -900,   467, 10864,
    -900,  -900,  8179,  -900,    60, 11051,  7540,   289, 10864,  -900,
   11071, 11098, 11118,   468, 10864,   176,  7622,  -900,  -900,  -900,
      60, 11138, 10864,   469,  7643,    60, 10864,   -45,  5319,  -900,
      60,   472,  -900
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,    18,     0,    20,    94,    31,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   575,   576,     0,
       0,     0,   397,   401,   733,     0,     0,     0,     0,     0,
     350,     0,   423,     0,     0,     0,     0,   709,     0,     0,
       0,   352,   710,     0,   379,     0,     0,     0,     0,     0,
     565,     0,   360,     0,   737,   736,     0,     0,     0,   711,
     712,     0,     0,     0,     0,     0,     0,     0,   732,     0,
     731,     0,   419,     0,     0,   422,   354,     0,   421,     0,
      33,     0,   345,     0,     0,     0,     0,     0,     0,   641,
     644,   643,   645,   642,     0,   734,     0,   569,     0,     0,
       0,     0,     0,     0,   735,     0,     0,     0,     0,     0,
       0,     0,   346,     0,     0,     0,    55,    56,    63,    57,
      58,    64,     0,     0,     0,    59,    60,    61,    62,   697,
     699,     0,   573,     0,   698,   700,   701,   702,   703,   704,
       0,   574,     0,    21,    22,    23,    19,     0,     0,     0,
       0,     0,     2,     5,     3,    97,     4,    15,     0,    16,
      17,    10,     8,    13,    14,    12,   436,     0,     0,    11,
       0,     0,    95,     0,     0,     9,   437,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    84,     0,     0,     0,     0,     0,     0,     0,   398,
     399,   400,     0,   204,   597,   598,   599,   596,   613,   610,
     611,   601,   603,   606,   605,   602,   604,   607,   608,   609,
     614,   612,   600,     0,     0,    20,    94,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   227,     0,     0,     0,
       0,   640,   639,    33,    21,    22,    23,     0,     0,     0,
       0,     0,   219,     0,   218,   222,   224,   223,   347,   178,
     179,   426,   303,   427,   592,   428,   593,   425,   429,   340,
     351,     0,     0,     0,     0,     0,     0,   278,   273,   276,
     275,   274,   176,   177,   173,     0,   302,     0,     0,   349,
     348,     0,   239,     0,     0,     0,   231,   230,   234,   236,
     235,     0,     0,     0,     0,     0,   343,   435,   434,   433,
     402,   432,   659,   667,   672,   671,   673,   661,   654,   682,
     678,   683,   679,   684,   680,   658,   655,   656,   674,   665,
     675,   662,   676,   685,   681,   664,   666,   670,   668,   669,
     660,   657,   663,   677,   342,     0,     0,     0,   320,   322,
     325,   324,   323,   361,     0,   566,     0,   359,     0,     0,
       0,     0,   408,   378,   377,   375,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   701,     0,   304,
     318,   768,   319,   769,     0,     0,   253,     0,     0,   243,
     242,   247,   248,   246,     0,     0,   418,     0,     0,     0,
       0,     0,     0,     0,   690,   693,     0,     0,   692,   694,
       0,   691,     0,     0,   689,    87,    88,   752,   357,     0,
       0,   794,   362,     0,     0,     0,     0,   403,     0,     0,
       0,     0,     0,     0,   184,   187,   186,   188,     0,     0,
     371,     0,   417,   570,   358,     0,   646,   647,   341,     0,
       0,   424,     0,     0,     0,     0,     0,     0,   257,   256,
     262,   260,   758,     0,   290,   291,   293,   761,     0,   211,
     210,     0,     0,     0,     0,   266,   267,   270,   759,     0,
     286,   287,   289,   760,   338,     0,   192,     0,     0,     0,
       0,     0,   216,   228,   240,   254,   212,   213,     0,     0,
       0,     0,     0,   413,     0,     0,     0,     0,     0,   284,
       0,     0,     0,     0,    69,     0,    68,    97,   164,    83,
     162,     0,     0,     1,     0,    32,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,   133,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     328,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   438,
     802,   804,   803,     0,     0,     0,     0,     0,     0,   571,
     487,     0,     0,     0,     0,   202,    87,    91,     0,     0,
     165,   365,    87,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   181,     0,   180,     0,
       0,     0,   431,   298,   299,   301,   300,     0,   743,   742,
     407,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     832,   831,   833,   226,   225,   756,   740,   745,   696,   695,
     221,    68,    83,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,     0,     0,   739,   738,   741,   280,     0,     0,     0,
       0,     0,     0,     0,     0,   238,   237,   757,   233,     0,
       0,   562,   564,   561,     0,   327,   326,   787,   321,     0,
     791,   200,     0,     0,     0,     0,     0,   376,     0,   762,
     314,   313,   312,   315,     0,   307,   309,   786,   308,   305,
       0,   306,   763,   764,     0,     0,     0,     0,     0,     0,
     249,   245,     0,   208,     0,     0,     0,    87,     0,     0,
     420,     0,     0,    87,   753,   622,   621,   623,   624,   430,
     750,     0,     0,    19,     0,   355,   356,     0,     0,     0,
     404,   405,     0,   411,     0,   409,     0,    92,    93,   201,
       0,   625,   626,   631,   633,   627,   629,   630,   628,   632,
     634,   185,     0,     0,     0,     0,     0,   707,   708,     0,
     372,     0,     0,   265,   263,   259,   261,     0,   292,     0,
       0,   272,   269,     0,   288,     0,   783,   782,   785,   784,
       0,   217,   229,   241,   255,   779,   778,   781,   780,     0,
       0,     0,   296,   636,   638,   635,   637,   294,     0,   297,
     295,   344,   166,   167,   168,    65,   163,     0,     0,     0,
       0,     0,     0,     6,   170,     0,     0,     0,    82,   140,
      81,   143,    75,   146,    76,   149,    77,   155,    78,   152,
      79,   158,    80,   161,    66,   117,    67,   120,    70,   123,
      71,   126,    72,   129,    73,   132,    27,     0,     0,     0,
       0,   139,   138,   142,   141,   145,   144,   148,   147,   154,
     153,   151,   150,   157,   156,   160,   159,   116,   115,   119,
     118,   122,   121,   125,   124,   128,   127,   131,   130,   172,
     171,     0,     0,     0,     0,     0,     0,   333,   329,   770,
     331,   774,   616,   619,   775,   771,   773,   772,   620,   617,
     618,   776,   777,   615,   330,    30,     0,   567,    44,    45,
      46,    47,    48,    49,    50,    51,   572,     0,     0,     0,
       0,     0,     0,     0,     0,   463,   466,   467,     0,   460,
     485,     0,   462,     0,     0,     0,     0,     0,   474,   480,
     477,   476,   479,   475,     0,     0,     0,   799,   459,   458,
     363,   486,   364,     0,   452,   455,   454,   453,   215,     0,
       0,   798,     0,     0,     0,     0,     0,   440,   444,   442,
     441,   443,   439,     0,    29,    28,    96,   495,   557,   559,
     558,   560,     0,     0,     0,     0,     0,   546,   493,   554,
     555,   795,   556,   494,   796,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   491,   522,   540,   541,   821,   531,   492,
     829,     0,   653,     0,   651,     0,     0,   732,   731,     0,
       0,     0,   652,     0,     0,     0,     0,     0,   489,   497,
     504,   507,   510,   517,   490,   820,     0,     0,     0,     0,
     203,     0,    52,    34,   104,    35,   105,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    86,    85,   370,     0,
      87,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      82,    81,    75,    76,    77,    78,    79,    80,    66,    67,
      70,    71,    72,    73,     0,   369,   277,   754,   279,   174,
       0,   594,   595,     0,   196,     0,   198,   199,     0,     0,
       0,   353,   563,    89,     0,     0,   392,     0,     0,   766,
     311,   310,   317,   765,   316,     0,     0,   250,   790,   251,
       0,   191,     0,     0,     0,    99,   373,     0,   205,   207,
       0,   206,   751,     0,   792,   793,   101,   406,   412,   410,
       0,     0,   102,     0,     0,   103,   705,   706,   368,     0,
     264,   258,   374,   268,   271,   577,   578,   579,   580,   582,
     581,   583,   584,   585,   586,   587,   588,     0,     0,     0,
     415,   414,     0,     0,     0,     0,     0,   169,     0,     0,
     788,   789,   332,     0,     0,   337,     0,   568,   473,     0,
     469,     0,   470,   472,   468,   464,   461,     0,   807,   749,
     747,   748,   746,     0,   808,   482,   481,   478,   839,   836,
     837,   838,   835,   800,   457,   456,   832,   833,   805,   451,
     214,     0,   801,   448,   449,   797,   446,   445,   447,   450,
       0,     0,   650,   649,   648,   552,   548,   547,   834,   549,
     550,     0,     0,   528,   526,     0,   525,   524,   532,   825,
     824,   826,   543,   544,     0,     0,     0,   523,   527,   687,
     688,   686,   830,   529,   530,     0,   533,     0,   534,   822,
     823,     0,   811,   503,   814,     0,     0,   498,     0,   809,
     513,   508,   511,   813,   812,     0,     0,     0,   817,     0,
     810,   514,   509,   512,   501,   502,     0,   818,     0,   519,
     815,   816,     0,   488,     0,     0,    25,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,   339,   197,     0,     0,     0,     0,     0,     0,   767,
       0,     0,     0,     0,     0,     0,     0,     0,   193,   194,
       0,   190,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    74,     0,   134,   135,   136,   137,   334,     0,     0,
     465,     0,   367,     0,   483,   366,   806,   553,     0,   496,
     539,     0,   828,   827,   536,   535,   538,   537,   542,   515,
     505,     0,   516,   506,   499,     0,   500,   591,   589,   590,
     518,   819,     0,   714,   713,     0,     0,     0,    37,    36,
     109,    38,   107,   108,   106,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   391,     0,     0,    26,     0,   394,   252,     0,     0,
       0,     0,   383,     0,     0,   385,     0,   395,     0,   182,
       0,     0,   416,     0,    98,   335,   336,   471,   484,   551,
     545,   520,   521,   386,   387,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   195,     0,     0,   730,   728,   729,     0,     0,
       0,   281,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   726,   725,   723,   721,   727,   722,   724,     0,    53,
       0,    39,   110,    40,   111,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   220,   232,     0,     0,   244,
       0,   100,   384,     0,   396,     0,   183,   285,   283,     0,
       0,     0,     0,     0,     0,     0,     0,   744,     0,     0,
       0,     0,     0,     0,   189,   209,     0,   715,   716,   717,
       0,     0,    41,   112,     0,     0,     0,     0,     0,     0,
     390,   389,     0,     0,   393,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   720,   718,   719,     0,     0,
      42,   113,     0,   380,     0,     0,     0,     0,     0,   388,
       0,     0,     0,     0,     0,   282,     0,    54,    43,   114,
       0,     0,     0,     0,     0,     0,     0,     0,   755,   381,
       0,     0,   382
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -900,  -900,  -900,   351,     0,  1524,  -170,  1755,   -37,  -900,
    -900,  1140,  2000,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,  -900,   290,  -900,  -900,  -900,  -900,  -900,  2371,
     221,  -900,  -164,  -900,  -900,   829,  -900,   -76,  -900,  -900,
    -900,  -900,   360,   769,  -900,  -301,  -900,  -900,  -899,  -900,
    -900,   -20,  -586,  -900,  -900,  -900,  -900,  -900,  -900,  -900,
    -900,  -900,   -52,    48,   -14,    23,   195,  -192,  -900,  -900,
    -900,  -900,  -900,  -900,  -292,  -174,  -149
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   151,   152,   153,   415,   638,   416,   639,   679,   609,
     779,   251,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   599,  1078,  1053,  1028,  1017,   306,   291,
     253,   169,   170,  1217,  1450,   269,  1143,   223,   944,   759,
     760,   791,   837,   434,   171,   448,  1285,  1080,   344,  1312,
     418,   660,   172,  1198,   799,   173,  1456,  1610,  1638,  1558,
    1538,   174,   277,   435,   279,   436,   437,   983,  1252,   175,
    1033,   176,  1084,  1059,   655,  1034,  1263
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     154,   697,  1016,   569,  1547,   428,   282,   283,   608,   191,
     380,   613,   355,   617,   618,   510,   621,  1409,  1230,   815,
    1535,   202,   362,   690,  1233,   202,  1141,   250,  1447,  1319,
    1282,  1350,  1309,   350,   177,   484,  1258,  1259,  1196,   289,
     295,   691,   241,   397,   446,   593,  1145,   242,   203,  1226,
     256,   460,   923,   465,   302,  1607,   280,   476,   481,   443,
    1240,  1434,   378,   299,   388,   504,  1142,   797,  1436,   507,
     351,   202,  1153,   830,   394,   255,  1551,   202,   191,   241,
     179,   278,   627,   831,   242,   382,  1490,   392,   298,   191,
     398,  1197,   798,    49,    50,   349,   525,   931,   444,    49,
      50,   541,  1249,   457,   395,   178,   354,   627,   765,   474,
     381,   202,   391,   932,   180,   491,   259,   260,   543,   430,
     181,   238,   239,   502,   772,   774,   461,   776,   466,  1283,
     182,   700,   477,   482,   692,   264,   241,   693,   183,  1260,
    1297,   242,   509,   192,   191,  1536,   184,   514,   516,   519,
     521,  1552,  1553,   185,   241,   594,   633,  1423,   241,   242,
     186,   261,   933,   242,   924,   303,  1608,   781,   274,   447,
    1448,   430,   505,   238,   239,   968,   508,   446,   606,   610,
     191,   612,   615,   934,   709,   619,   202,   622,   623,   624,
     625,   591,   782,  1559,  1560,   262,   604,   187,   241,   266,
    1310,   241,   595,   242,  1311,   263,   242,   238,   239,   363,
     241,    49,    50,  1299,   241,   242,   590,  1284,   274,   242,
    1554,   168,   257,   653,   935,   936,   937,  1261,   281,   304,
      96,    97,   925,   368,   188,   300,   627,  1555,   191,   657,
     195,  1249,   352,   201,   202,   264,  1250,   661,   662,   663,
     305,  1231,   271,   345,   431,  1537,  1251,   383,   274,   393,
     654,  1609,  1320,  1449,  1351,   926,   265,  1158,   241,   650,
     189,   938,   421,   242,   698,   939,  1262,   202,   783,   695,
     784,   999,   365,   239,   634,   627,   658,   432,   596,   597,
     635,   368,   400,   198,  1435,   659,  1346,   598,   462,   785,
     467,  1437,   447,   702,   478,   483,   431,   190,   440,   266,
     541,   627,   636,  1014,   196,   542,   696,   346,  1635,  1556,
     445,   940,   449,   946,   947,   833,   834,   468,   469,   941,
     942,   705,   627,  1015,  1417,   511,  1557,  1286,   197,   432,
     993,   239,  1287,   497,   433,   503,   202,  1300,    96,    97,
    1669,  1670,   721,   729,   835,  1266,   710,   525,   191,   713,
     284,   627,   272,   592,   288,   786,   787,   741,   706,   356,
     202,   727,   887,   238,   239,   301,   732,   733,  1301,   267,
     446,   354,   943,   358,   463,  1250,   268,   788,   401,   722,
     755,   359,   525,   836,   419,  1251,   541,   360,   651,   747,
     756,   542,  1032,  -170,  1058,  1083,   753,   433,   361,  1496,
     241,   199,   762,   384,   720,   242,   652,   273,  1305,   399,
     657,  1249,  1060,  1085,   274,   402,   420,  1636,   423,   777,
     780,   429,   792,   438,  1090,   805,   757,   439,   441,   200,
     506,   713,   479,   789,   523,   790,   512,    49,    50,   513,
     801,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   808,   524,   540,   238,   239,   560,  1637,
    1253,   613,   601,  1113,   602,  1115,   603,   202,   629,   814,
     525,   630,   178,   641,   651,   642,   241,   816,   817,   818,
     819,   242,   537,   538,   539,   129,   130,   540,   825,   826,
     827,   828,  1267,   643,   644,   447,   347,   758,   645,   694,
    1012,   842,   843,   844,   646,   647,   648,   274,   649,   966,
     134,   135,   136,   137,   138,   139,   857,   858,   860,   862,
     864,   866,   868,   870,   872,   874,   876,   878,   880,   882,
     884,   568,  1029,   588,  1055,   889,   891,   893,   895,   897,
     899,   901,   903,   905,   907,   909,   911,   913,   915,   917,
     919,   723,   980,   988,   307,  1250,   275,   995,   627,   238,
     239,  1009,  1306,   238,   239,  1251,   680,   681,   687,   712,
     688,   715,   716,  1000,    96,    97,   717,  1013,   276,   724,
     752,   556,   557,   558,   967,  1035,   559,   744,   749,   981,
     989,   802,   809,   713,   996,   735,   241,   810,  1010,   677,
     241,   242,   886,   433,   308,   242,  1020,  1030,   540,  1056,
    1081,   559,   544,   751,   979,    49,    50,  1106,   888,   994,
     921,   922,  1086,  1008,   674,   675,   676,  1110,  1016,   677,
    1092,  1019,  1112,  1091,  1114,  1093,  1116,  1117,  1118,  1119,
    1148,  1097,  1098,  1101,   793,   525,  1109,  1149,  1151,   794,
     795,  1152,  1153,  1154,  1120,  1121,  1122,  1123,  1124,  1125,
    1126,  1127,  1128,  1129,  1130,  1131,  1132,  1133,  1134,  1156,
    1172,  1136,  1137,  1157,  1165,  1173,  1175,  1138,  1177,  1180,
    1144,   569,  1182,  1186,  1268,  1183,  1192,   177,  1193,  1150,
    1239,   241,  1194,   945,  1160,  1222,   242,  1247,  1281,   526,
    1295,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,  1162,  1321,   677,   838,  1333,  1334,
    1643,  1644,  1352,  1354,  1164,  1355,  1166,  1167,  1168,  1371,
    1372,  1161,  1170,   492,   526,   856,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,  1374,
     202,   677,   309,  1380,   525,  1393,   678,  1184,  1185,  1406,
    1432,   982,  1441,  1433,  1445,  1458,   997,  1457,  1465,  1470,
    1011,  1480,  1481,  1486,  1492,  1495,  1513,   493,  1021,  1031,
    1503,  1057,  1082,  1504,  1514,  1581,   254,  1022,  1582,   191,
     718,  1515,  1200,  1525,  1526,   202,  1540,  1201,  1577,   297,
    1203,    49,    50,  1204,   311,  1578,   348,  1223,  1584,  1597,
    1218,  1589,  1087,  1088,  1590,   946,   947,  1603,  1611,  1219,
    1625,   379,   526,   390,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,  1620,   494,   677,
    1621,  1108,   368,   525,   699,  1624,   238,   239,  1633,   956,
    1639,  1647,  1649,   285,  1660,  1665,  1389,  1672,   840,   957,
     948,   949,   459,   950,   951,   853,  1288,     0,   475,   591,
       0,   952,   953,   954,   955,     0,   525,     0,  1227,     0,
    1023,   129,   130,   241,  1024,   958,     0,     0,   242,     0,
       0,  1135,   239,   202,   590,     0,     0,   274,     0,  1237,
       0,     0,     0,     0,     0,     0,   134,   135,   136,   137,
     138,   139,  1232,     0,  1234,     0,  1236,  1358,     0,     0,
       0,  1365,     0,     0,     0,     0,   570,   589,     0,   600,
       0,     0,   525,     0,     0,  1025,   605,     0,    96,    97,
     368,  1274,   959,     0,     0,     0,     0,     0,  1026,     0,
    1241,   626,     0,     0,     0,     0,   632,     0,   495,     0,
    1176,     0,     0,  1248,     0,     0,  1257,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,     0,
    1276,   559,  1205,  1206,  1207,  1208,  1209,  1210,  1211,  1212,
    1213,  1214,  1215,  1216,     0,  1278,     0,   526,     0,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,  1255,   677,     0,  1291,  1277,     0,   734,
    1202,     0,     0,     0,     0,     0,   241,   960,     0,  1296,
     525,   242,  1304,     0,     0,     0,     0,     0,     0,  1316,
    1318,   592,     0,     0,     0,     0,  1412,     0,     0,     0,
       0,     0,     0,     0,   961,   962,  1327,     0,     0,  1338,
    1344,  1345,     0,     0,  1347,     0,     0,     0,     0,     0,
       0,     0,     0,   963,     0,     0,     0,     0,     0,  1331,
     525,     0,  1342,     0,     0,     0,     0,     0,  1359,  1361,
    1363,     0,  1366,  1367,  1368,  1369,     0,     0,     0,     0,
       0,     0,     0,     0,  1330,   707,   526,  1341,   664,   665,
     666,   667,   668,   669,   670,   671,   672,   673,   674,   675,
     676,     0,   964,   677,     0,   719,     0,     0,   742,   726,
     155,     0,     0,     0,     0,     0,     0,     0,   525,     0,
       0,     0,     0,     0,   713,   713,     0,   713,     0,     0,
       0,     0,     0,   743,     0,     0,     0,     0,     0,     0,
    1256,     0,     0,     0,  1395,     0,     0,   612,     0,  1132,
       0,  1132,     0,     0,     0,     0,     0,     0,     0,     0,
    1400,  1401,     0,     0,   713,     0,     0,     0,     0,  1405,
       0,     0,     0,     0,  1474,   526,     0,   664,   665,   666,
     667,   668,   669,   670,   671,   672,   673,   674,   675,   676,
       0,     0,   677,     0,     0,  1411,     0,   807,  1413,  1415,
       0,     0,     0,     0,     0,     0,     0,     0,   526,  1420,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,     0,     0,   677,     0,     0,     0,     0,
     813,  1332,     0,     0,  1343,     0,     0,     0,     0,     0,
       0,  1425,     0,   832,     0,     0,   839,     0,     0,   841,
    1426,  1427,     0,     0,     0,     0,     0,     0,   517,   517,
     517,     0,  1429,     0,   526,  1430,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,     0,   820,     0,     0,     0,
       0,   517,   517,     0,     0,   517,  1440,     0,     0,     0,
       0,     0,   927,   928,   929,   930,  1443,     0,   600,     0,
     965,   969,     0,     0,   972,   978,     0,   990,     0,   525,
       0,   998,  1451,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1018,     0,     0,  1054,  1079,     0,     0,     0,
     525,     0,  1472,  1473,     0,  1475,  1476,  1477,  1478,  1479,
       0,     0,     0,     0,     0,  1482,  1390,     0,     0,     0,
       0,  1487,  1488,     0,   544,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,  1500,  1501,   677,
       0,     0,     0,     0,     0,     0,     0,     0,  1505,  1506,
     525,  1507,     0,  1508,     0,     0,     0,     0,  1509,     0,
       0,  1510,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1511,     0,  1410,     0,  1512,   529,   530,   531,   532,
     533,   534,   535,   536,   537,   538,   539,     0,   525,   540,
    1146,     0,     0,     0,     0,  1517,  1519,  1521,  1522,  1523,
    1524,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   525,     0,  1159,     0,     0,
       0,     0,     0,     0,  1544,     0,     0,     0,  1546,   517,
     526,     0,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,     0,     0,   677,     0,     0,
       0,  1178,   829,     0,     0,   544,  1570,  1571,  1572,  1573,
    1574,  1575,     0,  1576,     0,     0,     0,     0,     0,  1579,
     193,   525,     0,     0,     0,     0,     0,     0,  1587,  1588,
       0,     0,   258,     0,   270,     0,     0,     0,     0,   286,
     287,     0,     0,   290,     0,  1592,  1594,  1595,  1596,     0,
       0,     0,   353,     0,     0,     0,   357,     0,     0,     0,
       0,   517,     0,  1604,     0,  1605,     0,     0,     0,     0,
     396,     0,     0,     0,     0,     0,     0,     0,  1617,  1618,
    1619,     0,   544,   422,     0,   427,     0,     0,     0,     0,
       0,     0,     0,     0,  1627,  1629,  1630,     0,   442,     0,
       0,     0,     0,  1634,   570,   589,   525,     0,     0,   464,
       0,  1645,  1646,     0,   480,     0,     0,   485,     0,  1650,
       0,     0,  1651,     0,  1623,     0,     0,   525,  1656,     0,
       0,     0,     0,     0,  1661,     0,     0,     0,     0,     0,
       0,     0,  1664,     0,   854,     0,  1668,   517,   517,   517,
     517,   517,   517,   517,   517,   517,   517,   517,   517,   517,
     517,   525,     0,     0,     0,   517,   517,   517,   517,   517,
     517,   517,   517,   517,   517,   517,   517,   517,   517,   517,
     517,   526,   607,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   525,   628,   540,     0,
       0,     0,   526,  1099,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,     0,     0,   677,
       0,     0,     0,   517,  1254,     0,   545,   640,   546,   547,
     548,   549,   550,   551,   552,   553,   554,   555,   556,   557,
     558,     0,     0,   559,     0,     0,     0,     0,  1100,     0,
       0,  1272,   526,     0,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,     0,     0,   677,
       0,     0,     0,     0,  1102,     0,     0,   683,     0,   686,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   689,
     526,     0,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,     0,   701,   677,   703,   417,
    1322,     0,  1103,     0,  1324,  1329,   525,   526,  1340,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,  1353,     0,   451,     0,  1104,
       0,     0,     0,   470,     0,     0,     0,     0,     0,   486,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     711,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,   683,   526,   559,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,   525,     0,  1382,  1105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   750,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   611,     0,     0,     0,     0,
       0,     0,     0,   766,     0,     0,     0,   525,   770,   771,
     773,   631,   775,   778,   545,     0,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,     0,
       0,   559,     0,     0,     0,     0,  1155,   803,   526,     0,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,   656,     0,   677,   811,     0,     0,   526,
    1174,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,     0,     0,   677,     0,     0,     0,
     525,  1190,  1424,     0,   202,     0,     0,     0,   684,   685,
       0,     0,     0,   526,     0,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,     0,     0,     0,  1191,     0,     0,     0,     0,
     704,   525,     0,     0,     0,     0,     0,     0,   526,     0,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,     0,     0,   677,     0,     0,   544,     0,
    1199,     0,     0,     0,  1439,     0,     0,   970,     0,     0,
       0,     0,   708,   991,  1442,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1446,   973,     0,  1027,     0,
     238,   239,     0,   684,   725,     0,     0,   728,     0,   730,
     731,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     740,     0,     0,     0,   525,     0,     0,     0,   518,   520,
     522,  1107,   745,   746,   748,     0,     0,   241,     0,     0,
       0,   754,   242,     0,     0,   761,     0,     0,   764,     0,
       0,   274,     0,     0,     0,     0,     0,   974,   769,     0,
       0,   614,   616,     0,     0,   620,     0,     0,   526,     0,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,   975,   800,   677,     0,     0,     0,   804,
    1271,   806,     0,     0,     0,     0,  1147,     0,   525,     0,
       0,     0,   976,     0,     0,     0,     0,     0,   812,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   517,   517,
     517,     0,     0,     0,     0,     0,     0,   821,   822,   823,
     824,     0,  1534,   977,  1163,   525,     0,     0,     0,     0,
       0,     0,     0,     0,   526,  1545,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,   525,
       0,   677,     0,     0,     0,     0,  1280,     0,     0,     0,
       0,     0,     0,     0,   517,   517,  1187,   517,  1188,   526,
    1189,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,     0,     0,   677,   517,     0,     0,
       0,  1292,     0,     0,     0,     0,     0,   525,     0,   971,
       0,     0,   987,     0,   517,   992,     0,     0,     0,  1001,
    1007,     0,     0,     0,     0,     0,  1601,  1602,     0,     0,
       0,  1052,  1077,     0,  1220,  1221,     0,     0,  1606,   714,
       0,     0,     0,     0,     0,     0,     0,     0,   517,   517,
       0,   167,   526,     0,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,     0,     0,   677,
     194,     0,  1111,   194,  1373,     0,     0,     0,   252,     0,
       0,     0,   194,     0,     0,     0,     0,   685,     0,     0,
       0,   296,     0,  1653,     0,     0,   310,  -171,  -171,  -171,
    -171,  -171,  -171,   535,   536,   537,   538,   539,     0,  1663,
     540,     0,   364,     0,  1667,   389,     0,     0,     0,  1671,
       0,   796,   194,  1140,  -171,  -171,  -171,  -171,  -171,  -171,
     554,   555,   556,   557,   558,     0,     0,   559,   194,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,     0,   194,     0,   458,   525,     0,   194,   194,     0,
       0,  1238,     0,     0,     0,     0,  1243,     0,     0,     0,
       0,     0,     0,   496,  1169,   194,   526,   525,   664,   665,
     666,   667,   668,   669,   670,   671,   672,   673,   674,   675,
     676,     0,     0,   677,     0,     0,     0,     0,  1375,     0,
       0,     0,     0,  1270,   525,     0,     0,   859,   861,   863,
     865,   867,   869,   871,   873,   875,   877,   879,   881,   883,
     885,     0,     0,     0,     0,   890,   892,   894,   896,   898,
     900,   902,   904,   906,   908,   910,   912,   914,   916,   918,
     920,  1294,   525,     0,  1298,     0,     0,     0,     0,  1308,
     526,     0,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,     0,     0,   677,     0,     0,
       0,     0,  1376,     0,     0,   525,     0,     0,     0,     0,
       0,     0,     0,  1089,     0,   517,   517,   526,     0,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,   525,     0,   677,     0,     0,     0,     0,  1377,
       0,   526,     0,   664,   665,   666,   667,   668,   669,   670,
     671,   672,   673,   674,   675,   676,     0,     0,   677,     0,
       0,     0,     0,  1378,     0,     0,     0,     0,     0,   194,
       0,   525,     0,     0,     0,     0,     0,  1381,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   526,
    1235,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,     0,     0,   677,  1394,   525,     0,
       0,  1379,     0,     0,     0,   517,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1242,     0,  1244,  1245,
       0,   525,     0,  1246,     0,     0,     0,     0,     0,   194,
       0,   194,   194,     0,     0,     0,     0,     0,     0,  1264,
    1265,     0,     0,     0,     0,     0,     0,     0,  1269,     0,
       0,     0,     0,     0,   517,   194,     0,     0,  1273,  1275,
       0,  1279,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,   525,     0,     0,     0,     0,
       0,     0,   517,     0,     0,     0,     0,     0,     0,     0,
       0,  1293,     0,     0,     0,     0,     0,     0,     0,  1307,
       0,     0,  1313,  1314,   194,     0,     0,     0,     0,   194,
     194,     0,     0,     0,     0,     0,     0,     0,  1323,     0,
       0,  1328,     0,     0,  1339,     0,     0,   525,     0,     0,
    1349,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,  1438,  1357,     0,     0,   526,
       0,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,  1370,     0,   677,     0,     0,     0,
       0,  1385,     0,     0,     0,     0,   526,   194,   664,   665,
     666,   667,   668,   669,   670,   671,   672,   673,   674,   675,
     676,     0,     0,   677,     0,   855,     0,     0,  1391,     0,
       0,     0,     0,  1383,  1384,   525,     0,     0,     0,     0,
       0,     0,     0,     0,   526,     0,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
    1499,   677,  1396,  1502,     0,  1398,  1392,     0,  1399,     0,
       0,     0,   525,     0,     0,     0,     0,   526,  1402,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,   525,     0,     0,     0,  1407,
       0,     0,   194,   194,   526,     0,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,     0,  1408,     0,     0,     0,
       0,   194,  1422,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   526,  1541,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,     0,     0,     0,  1418,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     526,   194,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,     0,     0,   677,     0,     0,
       0,  1586,  1419,   526,     0,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,  1444,     0,     0,  1421,     0,     0,  1360,  1362,
    1364,     0,     0,     0,     0,     0,     0,     0,     0,  1452,
    1455,     0,     0,     0,     0,  1171,     0,     0,     0,     0,
     194,     0,     0,     0,     0,     0,  1471,   526,     0,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,     0,     0,     0,  1489,  1428,
       0,     0,     0,     0,  1386,  1387,     0,  1388,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1655,     0,     0,     0,     0,     0,  1397,     0,   526,
     194,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,  1404,     0,   677,     0,     0,     0,
       0,  1431,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1516,     0,     0,     0,     0,     0,     0,     0,
       0,   525,     0,     0,     0,     0,     0,     0,  1414,  1416,
       0,     0,     0,     0,     0,  1532,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1542,     0,  1543,     0,
       0,     0,     0,     0,     0,     0,     0,   526,  1550,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,     0,     0,     0,     0,   -87,
    1569,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   526,  1580,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,  1591,  1466,   526,     0,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,     0,     0,     0,     0,  1467,
     525,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    -7,     1,  1622,     0,
       0,     0,     0,     0,     0,     0,  1626,     0,     0,     0,
       0,     0,     0,     0,     2,     3,     4,     5,     0,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,     0,
       0,    16,     0,    17,    18,     0,     0,    19,    20,     0,
       0,     0,    21,    22,    23,     0,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
      27,    28,     0,     0,     0,     0,    29,     0,     0,    30,
       0,     0,     0,     0,     0,     0,    31,    32,     0,     0,
       0,     0,     0,     0,     0,    33,    34,     0,     0,     0,
       0,     0,     0,     0,    35,    36,    37,    38,     0,     0,
       0,     0,     0,    39,    40,  1518,  1520,    41,     0,    42,
       0,     0,    43,     0,     0,    44,    45,     0,     0,     0,
      46,     0,    47,     0,     0,     0,     0,    48,    49,    50,
       0,     0,     0,    51,     0,    52,     0,     0,    53,     0,
       0,     0,     0,     0,     0,    54,    55,     0,     0,     0,
      56,    57,    58,    59,     0,    60,     0,    61,     0,     0,
       0,     0,    62,     0,    63,    64,     0,     0,     0,    65,
      66,     0,     0,     0,     0,     0,   194,     0,     0,     0,
       0,    67,    68,    69,    70,     0,     0,     0,   525,     0,
       0,     0,     0,     0,     0,     0,     0,    71,     0,    72,
       0,    73,     0,     0,     0,  1593,     0,     0,     0,     0,
       0,     0,     0,   526,    74,   527,   528,   529,   530,   531,
     532,   533,   534,   535,   536,   537,   538,   539,     0,    75,
     540,     0,     0,   194,    76,    77,    78,    79,    80,    81,
       0,     0,    82,    83,    84,    85,     0,    86,    87,     0,
       0,     0,     0,    88,  1628,    89,    90,    91,    92,    93,
      94,     0,     0,     0,    95,    96,    97,     0,     0,     0,
       0,    98,     0,    99,   100,     0,     0,   101,   102,     0,
       0,     0,  1652,     0,     0,   103,   104,   105,   106,     0,
     107,     0,     0,     0,   108,   109,   110,     0,     0,     0,
     111,     0,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,     0,   122,   123,     0,
     124,   125,   126,   127,   128,   129,   130,     0,   131,   132,
     133,     0,   526,  1225,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,     0,     0,   677,
     134,   135,   136,   137,   138,   139,   140,   141,    -7,     1,
       0,     0,     0,     0,   142,   143,   144,   145,   146,     0,
       0,     0,     0,     0,     0,     0,   847,   848,     4,   525,
     147,   148,     0,     0,     0,     0,   149,     0,     0,    -7,
     150,     0,     0,    16,     0,    17,    18,     0,     0,    19,
      20,     0,   525,     0,    21,    22,    23,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    26,
       0,     0,    27,    28,     0,   544,     0,     0,    29,     0,
       0,    30,     0,     0,     0,     0,     0,     0,    31,    32,
       0,     0,     0,     0,     0,     0,     0,    33,    34,     0,
       0,     0,     0,     0,     0,     0,    35,    36,    37,    38,
       0,     0,     0,     0,     0,    39,    40,     0,     0,    41,
       0,    42,     0,     0,    43,     0,     0,    44,    45,     0,
       0,     0,    46,     0,    47,     0,     0,     0,     0,    48,
      49,    50,     0,     0,     0,    51,     0,    52,     0,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    57,    58,    59,     0,    60,     0,    61,
     544,     0,     0,     0,    62,     0,    63,    64,     0,     0,
       0,    65,    66,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    67,  -171,  -171,  -171,  -171,  -171,  -171,
     672,   673,   674,   675,   676,     0,     0,   677,     0,    71,
       0,    72,     0,    73,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    75,     0,     0,     0,     0,    76,    77,    78,    79,
       0,    81,     0,     0,    82,    83,   849,    85,     0,    86,
      87,     0,     0,     0,     0,     0,     0,    89,    90,    91,
      92,    93,    94,     0,     0,     0,     0,    96,    97,     0,
       0,     0,     0,    98,     0,    99,   100,     0,     0,   101,
     102,     0,     0,     0,     0,     0,     0,   103,     0,   105,
     106,     0,   107,     0,     0,     0,   108,   109,   110,     0,
       0,     0,   111,     0,   112,   113,   114,     0,     0,     0,
       0,   115,     0,     0,     0,     0,     0,     0,     0,   122,
     123,     0,   124,     0,     0,     0,     0,   129,   130,     0,
     131,   132,   133,   224,   225,   226,     5,     0,   227,   228,
       8,   229,   230,   231,   232,   233,   234,   235,     0,     0,
       0,     0,   134,   135,   136,   137,   138,   139,   140,   141,
       0,     0,     0,     0,     0,    24,   142,   850,   851,   852,
       0,   526,     0,   664,   665,   666,   667,   668,   669,   670,
     671,   672,   673,   674,   675,   676,     0,     0,   677,     0,
       0,    -7,     0,  1468,   526,     0,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,     0,  1469,   545,     0,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,     0,     0,   559,     0,     0,     0,     0,  1483,
       0,     0,     0,   525,     0,     0,     0,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    54,    55,     0,     0,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,   224,   225,
     226,     5,     0,   227,   228,     8,   229,   230,   231,   232,
     233,   234,   235,     0,     0,     0,     0,     0,     0,     0,
       0,    68,     0,    70,     0,     0,     0,     0,     0,     0,
      24,     0,   545,     0,   546,   547,   548,   549,   550,   551,
     552,   553,   554,   555,   556,   557,   558,     0,     0,   559,
       0,     0,     0,     0,  1485,     0,     0,     0,   312,     0,
       0,     0,   313,   314,   315,   316,   317,   318,     0,     0,
       0,   319,   320,   321,   322,   323,   324,   243,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   325,
       0,     0,   326,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,    96,    97,     0,     0,     0,     0,
     327,     0,    49,    50,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   328,     0,   104,     0,   525,     0,    54,
      55,     0,     0,     0,    56,     0,     0,     0,   204,   205,
     206,     0,   424,     0,     0,     0,     0,   425,     0,   116,
     117,   118,   119,   120,   121,   191,     0,     0,     0,     0,
     125,   126,   127,   128,   129,   130,    68,     0,    70,     0,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,     0,     0,     0,     0,     0,     0,     0,     0,   134,
     135,   136,   137,   138,   139,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   244,   245,   246,   146,     0,     0,
       0,     0,     0,     0,     0,   340,     0,     0,     0,   147,
     247,     0,   243,     0,     0,   248,     0,     0,     0,   249,
       0,     0,     0,  1139,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   341,     0,    95,    96,
      97,     0,     0,     0,   224,   225,   226,     5,     0,   227,
     228,     8,   229,   230,   231,   232,   233,   234,   235,     0,
     104,     0,     0,     0,     0,  1036,     0,  1037,     0,     0,
       0,     0,     0,     0,     0,   544,    24,     0,     0,     0,
       0,     0,     0,   426,   116,   117,   118,   119,   120,   121,
       0,     0,     0,  1038,     0,   125,   126,   127,   128,   129,
     130,   202,     0,     0,   525,   526,  1228,   527,   528,   529,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
       0,     0,   540,     0,   134,   135,   136,   137,   138,   139,
       0,     0,     0,     0,     0,   342,   343,     0,     0,   244,
     245,   246,   146,   525,     0,     0,     0,     0,   368,     0,
    1039,  1040,     0,     0,   147,   247,     0,     0,    49,    50,
     248,     0,     0,     0,   249,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    54,    55,   525,     0,     0,
      56,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1041,     0,     0,     0,     0,     0,     0,     0,   239,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    68,   207,    70,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
       0,     0,     0,     0,   241,  1042,     0,     0,     0,   242,
       0,  1025,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1043,     0,     0,     0,     0,     0,
       0,     0,  1044,  1045,     0,     0,     0,     0,   243,   526,
       0,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,     0,     0,   677,     0,     0,     0,
       0,  1491,     0,     0,    95,    96,    97,     0,  1046,     0,
    1047,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1048,  1049,  1050,     0,   104,     0,   525,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1051,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     116,   117,   118,   119,   120,   121,     0,     0,     0,     0,
       0,   125,   126,   127,   128,   129,   130,   224,   225,   226,
       5,     0,   227,   228,     8,   229,   230,   231,   232,   233,
     234,   235,     0,     0,     0,  1061,     0,     0,     0,     0,
     134,   135,   136,   137,   138,   139,     0,     0,     0,    24,
       0,     0,     0,     0,     0,   244,   245,   246,   146,     0,
       0,  1062,   525,     0,     0,     0,     0,     0,     0,     0,
     147,   247,     0,     0,   202,     0,   248,     0,     0,     0,
     249,     0,     0,     0,     0,     0,     0,   545,  1063,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,   525,     0,   559,     0,     0,     0,     0,  1493,
       0,     0,     0,     0,     0,     0,   526,     0,   664,   665,
     666,   667,   668,   669,   670,   671,   672,   673,   674,   675,
     676,    49,    50,   677,     0,     0,     0,     0,  1494,     0,
       0,     0,     0,     0,     0,     0,   544,     0,    54,    55,
    1064,     0,     0,    56,     0,   526,     0,   664,   665,   666,
     667,   668,   669,   670,   671,   672,   673,   674,   675,   676,
       0,   239,   677,     0,     0,  1065,     0,  1498,     0,     0,
       0,     0,     0,  1066,     0,  1067,     0,  1068,  1069,   526,
       0,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,     0,     0,   677,   241,   544,  1070,
    1071,  1527,   242,     0,  1025,     0,  1072,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1073,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   243,     0,     0,     0,   224,   225,   226,     5,     0,
     227,   228,     8,   229,   230,   231,   232,   233,   234,   235,
       0,     0,     0,     0,     0,     0,     0,    95,    96,    97,
       0,     0,  1074,     0,     0,     0,     0,    24,  1075,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   104,
       0,   366,     0,     0,   367,     0,     0,     0,     0,     0,
       0,     0,   202,  1076,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   116,   117,   118,   119,   120,   121,     0,
       0,     0,     0,     0,   125,   126,   127,   128,   129,   130,
     526,     0,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,   525,     0,   677,     0,   368,
       0,     0,  1528,   134,   135,   136,   137,   138,   139,    49,
      50,     0,     0,     0,     0,     0,   525,   369,   244,   245,
     246,   146,     0,     0,     0,     0,    54,    55,     0,     0,
       0,    56,     0,   147,   247,   370,     0,     0,     0,   248,
       0,   371,     0,   249,   372,     0,     0,     0,   238,   239,
       0,     0,   373,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    68,   526,    70,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,   241,  1529,     0,     0,     0,
     242,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   526,     0,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,   243,
       0,   677,     0,     0,     0,     0,  1530,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    95,    96,    97,   545,  1229,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   555,
     556,   557,   558,     0,     0,   559,   374,   104,   224,   225,
     226,     5,     0,   227,   228,     8,   229,   230,   231,   232,
     233,   234,   235,     0,     0,     0,     0,     0,     0,   375,
       0,   116,   117,   118,   119,   120,   121,     0,     0,     0,
      24,   403,   125,   126,   127,   128,   129,   130,   376,     0,
     545,     0,   546,   547,   548,   549,   550,   551,   552,   553,
     554,   555,   556,   557,   558,     0,     0,   559,     0,     0,
       0,   134,   135,   377,   137,   138,   139,     0,     0,     0,
       0,   525,     0,     0,     0,     0,   244,   245,   246,   146,
       0,     0,     0,   404,     0,     0,     0,     0,     0,     0,
       0,   147,   247,     0,     0,     0,     0,   248,     0,     0,
       0,   249,     0,     0,     0,     0,     0,     0,     0,     0,
     525,     0,    49,    50,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    54,
      55,   405,     0,   406,    56,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   407,   408,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   525,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    68,     0,    70,     0,
       0,     0,     0,     0,     0,     0,     0,   526,     0,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,     0,     0,   409,   526,  1531,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,     0,     0,   677,     0,     0,     0,     0,
    1533,     0,   243,     0,     0,     0,     0,     0,     0,     0,
     410,     0,     0,   411,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   412,    95,    96,
      97,     0,     0,   413,   224,   225,   226,     5,     0,   227,
     228,     8,   229,   230,   231,   232,   233,   234,   235,     0,
     104,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   414,    24,     0,     0,     0,
       0,     0,     0,     0,   116,   117,   118,   119,   120,   121,
       0,     0,     0,   452,     0,   125,   126,   127,   128,   129,
     130,   202,     0,     0,     0,     0,   525,     0,     0,     0,
       0,     0,     0,     0,   453,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   134,   135,   136,   137,   138,   139,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   244,
     245,   246,   146,     0,     0,     0,     0,     0,   368,     0,
       0,     0,     0,     0,   147,   247,     0,     0,    49,    50,
     248,     0,     0,     0,   249,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    54,    55,     0,     0,     0,
      56,     0,     0,     0,     0,     0,   454,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   239,     0,
       0,   455,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    68,   526,    70,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,     0,     0,   241,   224,   225,   226,     5,   242,
     227,   228,     8,   229,   230,   231,   232,   233,   234,   235,
       0,     0,   526,     0,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,    24,   243,   677,
       0,     0,     0,     0,  1539,     0,     0,     0,     0,     0,
     525,   456,     0,  1002,  1003,     0,     0,     0,     0,     0,
       0,     0,   202,     0,    95,    96,    97,   526,     0,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,   677,     0,   104,     0,     0,  1548,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1004,     0,     0,     0,     0,     0,     0,     0,
     116,   117,   118,   119,   120,   121,     0,     0,     0,    49,
      50,   125,   126,   127,   128,   129,   130,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    54,    55,     0,     0,
       0,    56,     0,     0,     0,     0,     0,     0,     0,     0,
     134,   135,   136,   137,   138,   139,     0,     0,   238,   239,
       0,     0,     0,     0,     0,   244,   245,   246,   146,     0,
       0,     0,     0,    68,     0,    70,     0,     0,     0,     0,
     147,   247,     0,     0,     0,     0,   248,     0,     0,     0,
     249,     0,     0,     0,   224,   225,   226,     5,     0,   227,
     228,     8,   229,   230,   231,   232,   233,   234,   235,   274,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,   526,   243,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,     0,     0,   677,     0,     0,     0,     0,
    1549,   202,     0,     0,     0,    95,    96,    97,   525,     0,
    1005,  1006,     0,     0,   236,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   237,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   116,   117,   118,   119,   120,   121,   525,    49,    50,
       0,     0,   125,   126,   127,   128,   129,   130,     0,     0,
       0,     0,     0,     0,     0,    54,    55,     0,     0,     0,
      56,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   134,   135,   136,   137,   138,   139,   238,   239,     0,
       0,   240,     0,     0,     0,     0,   244,   245,   246,   146,
       0,     0,    68,     0,    70,     0,     0,     0,     0,     0,
       0,   147,   247,     0,     0,     0,     0,   248,     0,     0,
       0,   249,     0,     0,   241,   224,   225,   226,     5,   242,
     227,   228,     8,   229,   230,   231,   232,   233,   234,   235,
       0,     0,   526,     0,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,    24,   243,   677,
       0,     0,     0,     0,  1565,     0,     0,     0,     0,     0,
       0,     0,   525,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   202,     0,    95,    96,    97,     0,     0,     0,
       0,     0,     0,     0,     0,   292,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   104,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   293,     0,     0,     0,     0,     0,     0,     0,
     116,   117,   118,   119,   120,   121,     0,   544,     0,    49,
      50,   125,   126,   127,   128,   129,   130,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    54,    55,     0,     0,
       0,    56,     0,     0,     0,     0,     0,     0,     0,     0,
     134,   135,   136,   137,   138,   139,     0,     0,   238,   239,
       0,     0,   294,     0,     0,   244,   245,   246,   146,     0,
       0,     0,     0,    68,     0,    70,     0,     0,     0,     0,
     147,   247,     0,     0,     0,     0,   248,     0,     0,     0,
     249,     0,     0,     0,     0,   241,   224,   225,   226,     5,
     242,   227,   228,     8,   229,   230,   231,   232,   233,   234,
     235,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   385,     0,     0,     0,     0,     0,     0,    24,   243,
     526,   525,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,     0,     0,   677,     0,     0,
       0,     0,  1566,   202,     0,    95,    96,    97,     0,     0,
       0,     0,     0,     0,     0,     0,   386,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,   526,
       0,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,     0,     0,   540,     0,     0,     0,
     845,   116,   117,   118,   119,   120,   121,     0,     0,   525,
      49,    50,   125,   126,   127,   128,   129,   130,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    54,    55,     0,
       0,     0,    56,     0,     0,     0,     0,     0,     0,     0,
       0,   134,   135,   136,   137,   138,   139,     0,   525,   238,
     239,     0,     0,   387,     0,     0,   244,   245,   246,   146,
       0,     0,     0,     0,    68,     0,    70,     0,     0,     0,
       0,   147,   247,     0,     0,     0,     0,   248,     0,     0,
       0,   249,     0,     0,     0,     0,   241,   224,   225,   226,
       5,   242,   227,   228,     8,   229,   230,   231,   232,   233,
     234,   235,     0,     0,   526,     0,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,    24,
     243,   677,     0,     0,     0,     0,  1567,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   471,   525,     0,     0,
       0,     0,     0,     0,   202,     0,    95,    96,    97,     0,
       0,     0,     0,     0,     0,     0,     0,   472,     0,     0,
       0,     0,   525,     0,     0,     0,     0,     0,   104,   545,
       0,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,     0,     0,   559,     0,     0,     0,
     846,   368,   116,   117,   118,   119,   120,   121,     0,     0,
       0,    49,    50,   125,   126,   127,   128,   129,   130,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    54,    55,
       0,     0,     0,    56,     0,     0,     0,     0,     0,     0,
       0,     0,   134,   135,   136,   137,   138,   139,     0,     0,
       0,   239,     0,     0,     0,     0,     0,   244,   245,   246,
     146,     0,     0,     0,     0,    68,     0,    70,     0,     0,
       0,     0,   147,   247,     0,     0,     0,     0,   248,     0,
       0,     0,   249,     0,     0,     0,     0,   241,     0,     0,
       0,   525,   242,   526,     0,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,     0,     0,     0,  1568,     0,     0,     0,     0,
       0,   243,     0,     0,     0,     0,     0,     0,     0,     0,
     224,   225,   226,     5,   473,   227,   228,     8,   229,   230,
     231,   232,   233,   234,   235,     0,     0,    95,    96,    97,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,     0,     0,     0,     0,     0,   104,
       0,   526,     0,   664,   665,   666,   667,   668,   669,   670,
     671,   672,   673,   674,   675,   676,     0,   202,   677,     0,
       0,     0,   -90,   116,   117,   118,   119,   120,   121,     0,
       0,     0,     0,     0,   125,   126,   127,   128,   129,   130,
     526,     0,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   673,   674,   675,   676,     0,     0,   677,     0,     0,
       0,   525,  1583,   134,   135,   136,   137,   138,   139,     0,
       0,     0,     0,     0,    49,    50,  1335,     0,   244,   245,
     246,   146,     0,     0,     0,     0,     0,     0,     0,     0,
     525,    54,    55,   147,   247,     0,    56,     0,     0,   248,
       0,     0,     0,   249,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   238,   239,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    68,   526,
      70,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,     0,     0,   677,     0,     0,     0,
     241,  1585,     0,     0,   526,   242,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,     0,  1598,     0,     0,     0,
       0,     0,     0,     0,   243,     0,     0,     0,   224,   225,
     226,     5,     0,   227,   228,     8,   229,   230,   231,   232,
     233,   234,   235,     0,     0,     0,     0,     0,     0,     0,
      95,    96,    97,   525,     0,  1336,     0,     0,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,    17,    18,   525,     0,     0,  1337,
       0,     0,     0,     0,   561,   202,     0,     0,     0,   562,
       0,     0,     0,     0,     0,     0,   116,   117,   118,   119,
     120,   121,     0,     0,     0,     0,     0,   125,   126,   127,
     128,   129,   130,   526,     0,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,     0,     0,     0,  1599,   134,   135,   136,   137,
     138,   139,    49,    50,  1325,     0,     0,     0,     0,     0,
       0,   244,   245,   246,   146,   563,     0,     0,     0,    54,
      55,     0,     0,     0,    56,     0,   147,   247,     0,     0,
       0,     0,   248,     0,     0,     0,   249,     0,   564,     0,
       0,   238,   239,     0,     0,     0,     0,   204,   205,   206,
       0,     0,     0,     0,     0,     0,    68,     0,    70,   565,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   241,   224,
     225,   226,     5,   242,   227,   228,     8,   229,   230,   231,
     232,   233,   234,   235,     0,     0,     0,     0,     0,     0,
       0,     0,   525,     0,     0,   241,     0,     0,     0,     0,
     242,    24,   243,   526,     0,   664,   665,   666,   667,   668,
     669,   670,   671,   672,   673,   674,   675,   676,     0,     0,
     677,     0,     0,     0,     0,  1600,   202,     0,    95,    96,
      97,     0,   526,  1326,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,     0,     0,   677,
     104,     0,     0,     0,  1614,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   525,     0,     0,     0,
     566,     0,     0,     0,   116,   117,   118,   119,   120,   121,
       0,     0,     0,    49,    50,   125,   126,   127,   128,   129,
     130,   567,     0,     0,     0,     0,     0,     0,     0,     0,
      54,    55,     0,   525,     0,    56,     0,     0,     0,     0,
       0,     0,     0,     0,   134,   135,   136,   137,   138,   139,
     132,     0,     0,     0,     0,     0,     0,     0,     0,   244,
     245,   246,   146,     0,     0,     0,     0,    68,     0,    70,
       0,     0,     0,     0,   147,   247,     0,     0,   141,     0,
     248,     0,     0,     0,   249,   526,     0,   664,   665,   666,
     667,   668,   669,   670,   671,   672,   673,   674,   675,   676,
       0,     0,   677,   274,     0,     0,     0,  1615,   526,   568,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,   243,     0,   677,     0,     0,     0,     0,
    1616,     0,   525,     0,     0,     0,     0,     0,     0,   984,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
      96,    97,   207,     0,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,     0,
       0,   104,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   985,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   116,   117,   118,   119,   120,
     121,     0,   986,     0,     0,     0,   125,   126,   127,   128,
     129,   130,     0,   224,   225,   226,     5,     0,   227,   228,
       8,   229,   230,   231,   232,   233,   234,   235,     0,     0,
       0,     0,     0,     0,     0,   134,   135,   136,   137,   138,
     139,  1289,     0,     0,     0,    24,     0,     0,     0,     0,
     244,   245,   246,   146,     0,     0,     0,  1249,     0,     0,
       0,     0,     0,     0,     0,   147,   247,     0,     0,     0,
       0,   248,     0,     0,   526,   249,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,     0,  1631,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   224,
     225,   226,     5,     0,   227,   228,     8,   229,   230,   231,
     232,   233,   234,   235,     0,     0,     0,    49,    50,     0,
       0,     0,   525,     0,     0,     0,     0,     0,     0,     0,
       0,    24,     0,     0,    54,    55,     0,     0,   526,    56,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,     0,     0,   677,     0,     0,     0,     0,
    1632,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    68,     0,    70,     0,   526,     0,   664,   665,   666,
     667,   668,   669,   670,   671,   672,   673,   674,   675,   676,
       0,  1250,   677,     0,     0,     0,     0,  1642,     0,     0,
       0,  1251,     0,     0,   525,     0,     0,     0,     0,     0,
       0,     0,     0,    49,    50,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   525,     0,   243,     0,     0,
      54,    55,     0,     0,     0,    56,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   544,     0,    95,    96,    97,     0,     0,     0,     0,
       0,     0,     0,     0,  1290,     0,     0,    68,     0,    70,
       0,     0,     0,     0,   526,   104,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,   677,     0,     0,     0,     0,  1648,     0,     0,   116,
     117,   118,   119,   120,   121,     0,     0,     0,     0,     0,
     125,   126,   127,   128,   129,   130,     0,     0,     0,     0,
       0,     0,     0,   243,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   134,
     135,   136,   137,   138,   139,     0,     0,     0,     0,    95,
      96,    97,     0,     0,   244,   245,   246,   146,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   147,
     247,   104,     0,     0,     0,   248,     0,     0,     0,   249,
     224,   225,   226,     5,     0,   227,   228,     8,   229,   230,
     231,   232,   233,   234,   235,   116,   117,   118,   119,   120,
     121,     0,     0,     0,     0,     0,   125,   126,   127,   128,
     129,   130,    24,     0,     0,     0,   487,   488,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   134,   135,   136,   137,   138,
     139,     0,     0,     0,   489,   490,     0,     0,     0,     0,
     244,   245,   246,   146,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   147,   247,     0,     0,     0,
       0,   248,     0,     0,   526,   249,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,   525,
       0,   677,     0,     0,    49,    50,  1654,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    54,    55,     0,     0,     0,    56,     0,     0,     0,
       0,     0,     0,     0,     0,   224,   225,   226,     5,     0,
     227,   228,     8,   229,   230,   231,   232,   233,   234,   235,
       0,     0,     0,     0,     0,     0,     0,     0,    68,     0,
      70,     0,     0,     0,     0,     0,   526,    24,   664,   665,
     666,   667,   668,   669,   670,   671,   672,   673,   674,   675,
     676,     0,     0,   677,     0,     0,     0,   526,  1662,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,   525,     0,   677,     0,     0,     0,     0,  1666,
       0,     0,     0,   545,   243,   546,   547,   548,   549,   550,
     551,   552,   553,   554,   555,   556,   557,   558,     0,     0,
     559,     0,     0,     0,  1094,     0,     0,     0,     0,     0,
      95,    96,    97,     0,     0,     0,     0,     0,     0,    49,
      50,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,     0,    54,    55,     0,     0,
       0,    56,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   736,     0,     0,   737,     0,   116,   117,   118,   119,
     120,   121,     0,     0,     0,     0,     0,   125,   126,   127,
     128,   129,   130,    68,     0,    70,     0,   498,   499,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   134,   135,   136,   137,
     138,   139,   525,     0,     0,   500,   501,     0,     0,     0,
       0,   244,   245,   246,   146,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   147,   247,     0,   243,
       0,     0,   248,     0,     0,     0,   249,   224,   225,   226,
       5,     0,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,     0,     0,     0,    95,    96,    97,     0,     0,
     738,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   739,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,   125,   126,   127,   128,   129,   130,     0,     0,
       0,   526,     0,   664,   665,   666,   667,   668,   669,   670,
     671,   672,   673,   674,   675,  1179,     0,     0,   677,     0,
       0,   134,   135,   136,   137,   138,   139,     0,     0,     0,
       0,    49,    50,     0,     0,     0,   244,   245,   246,   146,
       0,     0,     0,     0,     0,     0,     0,     0,    54,    55,
       0,   147,   247,    56,     0,     0,     0,   248,     0,     0,
       0,   249,   224,   225,   226,     5,     0,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,     0,     0,     0,
       0,     0,     0,     0,     0,    68,    69,    70,     0,     0,
       0,     0,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   526,     0,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,     0,
       0,   540,     0,     0,     0,  1095,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    80,     0,     0,     0,     0,     0,   515,     0,   544,
       0,     0,     0,     0,     0,     0,    88,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    95,    96,    97,
       0,     0,   525,     0,     0,     0,    49,    50,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   104,
       0,     0,   544,    54,    55,     0,     0,     0,    56,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   116,   117,   118,   119,   120,   121,     0,
       0,     0,     0,     0,   125,   126,   127,   128,   129,   130,
      68,    69,    70,     0,   526,     0,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,  1181,     0,
       0,   677,     0,   134,   135,   136,   137,   138,   139,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   244,   245,
     246,   146,     0,     0,     0,     0,     0,     0,     0,     0,
     525,     0,     0,   147,   148,     0,    80,     0,     0,   149,
       0,     0,  1403,   150,     0,     0,     0,     0,     0,     0,
       0,    88,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,    96,    97,     0,   224,   225,   226,     5,
       0,   227,   228,     8,   229,   230,   231,   232,   233,   234,
     235,     0,     0,     0,   104,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   116,   117,
     118,   119,   120,   121,     0,     0,     0,   191,     0,   125,
     126,   127,   128,   129,   130,     0,     0,     0,     0,     0,
       0,     0,   544,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   134,   135,
     136,   137,   138,   139,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   244,   245,   246,   146,     0,     0,     0,
       0,     0,     0,     0,   525,     0,     0,     0,   147,   148,
      49,    50,     0,     0,   149,     0,     0,     0,   150,     0,
       0,     0,     0,     0,     0,     0,     0,    54,    55,     0,
       0,     0,    56,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   224,   225,   226,     5,     0,   227,   228,
       8,   229,   230,   231,   232,   233,   234,   235,     0,     0,
       0,     0,     0,     0,    68,     0,    70,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,     0,     0,     0,
       0,   545,   767,   546,   547,   548,   549,   550,   551,   552,
     553,   554,   555,   556,   557,   558,     0,     0,   559,   525,
       0,     0,  1096,     0,   526,     0,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
     243,   677,     0,     0,   545,   845,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,     0,
       0,   559,     0,     0,     0,  1195,    95,    96,    97,   544,
       0,     0,     0,     0,     0,     0,   682,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   104,     0,
       0,     0,     0,     0,    54,    55,     0,   768,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   116,   117,   118,   119,   120,   121,     0,     0,
       0,     0,     0,   125,   126,   127,   128,   129,   130,     0,
       0,    68,  -171,    70,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,     0,     0,   677,
       0,     0,   134,   135,   136,   137,   138,   139,     0,   525,
       0,     0,     0,     0,     0,     0,     0,   244,   245,   246,
     146,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   147,   247,     0,     0,     0,   243,   248,     0,
       0,     0,   249,     0,     0,   224,   225,   226,     5,     0,
     227,   228,     8,   229,   230,   231,   232,   233,   234,   235,
       0,     0,     0,    95,    96,    97,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     0,     0,
       0,     0,     0,     0,   545,   104,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,     0,
       0,   559,   202,     0,     0,  1356,     0,     0,     0,   116,
     117,   118,   119,   120,   121,     0,     0,     0,     0,     0,
     125,   126,   127,   128,   129,   130,   526,     0,   664,   665,
     666,   667,   668,   669,   670,   671,   672,   673,   674,   675,
     676,     0,     0,   677,     0,     0,     0,  1095,     0,   134,
     135,   136,   137,   138,   139,   544,     0,     0,     0,    49,
      50,     0,     0,     0,   244,   245,   246,   146,     0,     0,
       0,     0,     0,     0,     0,     0,    54,    55,     0,   147,
     247,    56,     0,     0,     0,   248,     0,     0,     0,   249,
     224,   225,   226,     5,     0,   227,   228,     8,   229,   230,
     231,   232,   233,   234,   235,     0,     0,     0,     0,     0,
       0,     0,     0,    68,     0,    70,     0,     0,  1302,     0,
       0,   526,    24,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,     0,     0,   540,     0,
       0,     0,  1459,     0,     0,     0,   525,     0,     0,   274,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   544,   243,
       0,   545,     0,   546,   547,   548,   549,   550,   551,   552,
     553,   554,   555,   556,   557,   558,     0,     0,   559,     0,
       0,     0,  1460,     0,     0,    95,    96,    97,   525,     0,
       0,     0,     0,     0,    49,    50,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
       0,    54,    55,     0,     0,     0,    56,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,   125,   126,   127,   128,   129,   130,    68,     0,
      70,  -171,     0,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,     0,     0,   540,     0,
       0,   134,   135,   136,   137,   138,   139,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   244,   245,   246,   146,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   147,   247,     0,   243,     0,     0,   248,     0,     0,
       0,   249,   224,   225,   226,     5,     0,   227,   228,     8,
     229,   230,   231,   232,   233,   234,   235,     0,     0,     0,
      95,    96,    97,     0,     0,     0,     0,     0,     0,     0,
       0,  1303,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1453,     0,     0,     0,     0,   116,   117,   118,   119,
     120,   121,     0,     0,     0,     0,     0,   125,   126,   127,
     128,   129,   130,     0,     0,     0,     0,  -171,     0,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,     0,     0,   559,     0,   134,   135,   136,   137,
     138,   139,     0,     0,     0,     0,    49,    50,     0,     0,
       0,   244,   245,   246,   146,     0,     0,     0,     0,     0,
       0,     0,     0,    54,    55,     0,   147,   247,    56,     0,
       0,     0,   248,     0,     0,     0,   249,   224,   225,   226,
       5,     0,   227,   228,     8,   229,   230,   231,   232,   233,
     234,   235,     0,     0,     0,     0,     0,     0,     0,     0,
      68,     0,    70,     0,     0,     0,     0,     0,   526,    24,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,     0,     0,   540,     0,     0,   191,  1461,
     545,     0,   546,   547,   548,   549,   550,   551,   552,   553,
     554,   555,   556,   557,   558,     0,     0,   559,     0,     0,
       0,  1462,   544,     0,     0,     0,   243,     0,  1454,     0,
     526,     0,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,    17,    18,   540,     0,     0,
     544,  1463,    95,    96,    97,   561,     0,     0,     0,     0,
     562,    49,    50,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   104,     0,     0,     0,    54,    55,
       0,     0,     0,    56,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   116,   117,
     118,   119,   120,   121,     0,     0,     0,     0,     0,   125,
     126,   127,   128,   129,   130,    68,     0,    70,     0,     0,
       0,     0,     0,     0,     0,     0,   563,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   134,   135,
     136,   137,   138,   139,     0,     0,     0,     0,     0,   564,
       0,     0,     0,   244,   245,   246,   146,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   147,   247,
     565,   243,     0,     0,   248,     0,     0,     0,   249,   224,
     225,   226,     5,     0,   227,   228,     8,   229,   230,   231,
     232,   233,   234,   235,     0,     0,     0,    95,    96,    97,
       0,     0,     0,     0,   544,     0,     0,     0,     0,     0,
       0,    24,     0,     0,     0,     0,   241,     0,     0,   104,
       0,   242,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   116,   117,   118,   119,   120,   121,     0,
       0,     0,     0,     0,   125,   126,   127,   128,   129,   130,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   134,   135,   136,   137,   138,   139,     0,
       0,   566,     0,    49,    50,     0,     0,     0,   244,   245,
     246,   146,     0,     0,     0,     0,     0,     0,     0,     0,
      54,    55,   567,   147,   247,    56,     0,     0,     0,   248,
       0,     0,     0,   249,   224,   225,   226,     5,     0,   227,
     228,     8,   229,   230,   231,   232,   233,   234,   235,     0,
       0,   132,     0,     0,     0,     0,     0,    68,     0,    70,
       0,     0,     0,     0,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   141,
       0,     0,     0,     0,   545,   191,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,     0,
       0,   559,     0,     0,     0,  1464,     0,     0,     0,     0,
     629,     0,   545,   243,   546,   547,   548,   549,   550,   551,
     552,   553,   554,   555,   556,   557,   558,   525,     0,   559,
       0,     0,     0,  1484,     0,     0,     0,     0,     0,    95,
      96,    97,     0,     0,     0,     0,     0,     0,    49,    50,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   525,
       0,   104,     0,     0,     0,    54,    55,     0,     0,     0,
      56,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   116,   117,   118,   119,   120,
     121,     0,     0,   450,     0,     0,   125,   126,   127,   128,
     129,   130,    68,     0,    70,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   134,   135,   136,   137,   138,
     139,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     244,   245,   246,   146,     0,     0,     0,     0,     0,     0,
     525,     0,     0,     0,     0,   147,   247,     0,   243,     0,
       0,   248,     0,     0,     0,   249,   545,     0,   546,   547,
     548,   549,   550,   551,   552,   553,   554,   555,   556,   557,
     558,     0,     0,   559,    95,    96,    97,  1497,   224,   225,
     226,     5,     0,   227,   228,     8,   229,   230,   231,   232,
     233,   234,   235,     0,     0,     0,   104,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     116,   117,   118,   119,   120,   121,     0,     0,     0,     0,
       0,   125,   126,   127,   128,   129,   130,     0,     0,     0,
       0,     0,   544,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     134,   135,   136,   137,   138,   139,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   244,   245,   246,   146,     0,
       0,     0,   368,     0,   525,     0,     0,     0,     0,     0,
     147,   247,    49,    50,     0,     0,   248,     0,     0,     0,
     637,     0,     0,     0,     0,     0,     0,     0,     0,    54,
      55,     0,     0,     0,    56,     0,     0,     0,     0,     0,
       0,     0,     0,   224,   225,   226,     5,     0,   227,   228,
       8,   229,   230,   231,   232,   233,   234,   235,     0,     0,
       0,     0,     0,     0,     0,     0,    68,     0,    70,     0,
       0,     0,     0,     0,     0,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   526,
       0,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,   544,     0,   677,     0,     0,     0,
    1459,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   526,   243,   664,   665,   666,   667,   668,   669,   670,
     671,   672,   673,   674,   675,   676,     0,     0,   677,     0,
       0,     0,  1461,     0,     0,     0,   525,     0,    95,    96,
      97,     0,     0,     0,     0,     0,     0,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     104,     0,     0,     0,    54,    55,     0,     0,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   116,   117,   118,   119,   120,   121,
       0,     0,     0,     0,     0,   125,   126,   127,   128,   129,
     130,    68,   526,    70,   527,   528,   529,   530,   531,   532,
     533,   534,   535,   536,   537,   538,   539,     0,     0,   540,
       0,     0,     0,  1561,   134,   135,   136,   137,   138,   139,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   244,
     245,   246,   146,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   147,   247,     0,   243,     0,     0,
     248,     0,     0,     0,   249,   224,   225,   226,     5,     0,
     227,   228,     8,   229,   230,   231,   232,   233,   234,   235,
       0,     0,     0,    95,    96,    97,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     0,     0,
       0,     0,     0,     0,   545,   104,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,  1315,
       0,   559,     0,     0,     0,  1562,     0,     0,     0,   116,
     117,   118,   119,   120,   121,     0,     0,     0,     0,     0,
     125,   126,   127,   128,   129,   130,   526,     0,   527,   528,
     529,   530,   531,   532,   533,   534,   535,   536,   537,   538,
     539,     0,     0,   540,     0,     0,     0,  1563,     0,   134,
     135,   136,   137,   138,   139,     0,     0,     0,     0,    49,
      50,     0,     0,     0,   244,   245,   246,   146,     0,     0,
       0,     0,     0,     0,     0,     0,    54,    55,     0,   147,
     247,    56,     0,     0,     0,   248,     0,     0,     0,   249,
     224,   225,   226,     5,     0,   227,   228,     8,   229,   230,
     231,   232,   233,   234,   235,     0,     0,     0,     0,     0,
       0,     0,     0,    68,     0,    70,     0,     0,     0,     0,
       0,     0,    24,     0,     0,     0,   545,     0,   546,   547,
     548,   549,   550,   551,   552,   553,   554,   555,   556,   557,
     558,     0,     0,   559,     0,     0,   525,  1564,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   525,     0,   526,   243,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,     0,     0,   677,     0,     0,     0,  1561,
       0,     0,     0,   544,     0,    95,    96,    97,     0,     0,
       0,     0,     0,     0,    49,    50,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
       0,    54,    55,     0,     0,     0,    56,     0,     0,     0,
       0,  1317,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,   125,   126,   127,   128,   129,   130,    68,     0,
      70,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   134,   135,   136,   137,   138,   139,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   244,   245,   246,   146,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   147,   247,     0,   243,     0,     0,   248,     0,     0,
       0,   249,   224,   225,   226,     5,     0,   227,   228,     8,
     229,   230,   231,   232,   233,   234,   235,     0,     0,     0,
      95,    96,    97,     0,     0,     0,     0,   525,     0,     0,
       0,     0,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,     0,     0,     0,     0,   525,
       0,     0,     0,     0,     0,     0,  1348,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   116,   117,   118,   119,
     120,   121,     0,     0,     0,     0,     0,   125,   126,   127,
     128,   129,   130,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   134,   135,   136,   137,
     138,   139,     0,     0,     0,     0,    49,    50,     0,     0,
       0,   244,   245,   246,   146,     0,     0,     0,     0,     0,
       0,     0,     0,    54,    55,     0,   147,   247,    56,     0,
       0,     0,   248,     0,     0,     0,   249,   224,   225,   226,
       5,     0,   227,   228,     8,   229,   230,   231,   232,   233,
     234,   235,     0,     0,     0,     0,     0,     0,     0,     0,
      68,     0,    70,   544,     0,     0,     0,     0,   526,    24,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,   525,     0,   677,     0,     0,   526,  1563,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   525,     0,   540,     0,     0,     0,  1612,
       0,     0,     0,     0,     0,   545,   243,   546,   547,   548,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     525,     0,   559,     0,     0,     0,  1613,     0,     0,     0,
       0,     0,    95,    96,    97,     0,     0,     0,     0,     0,
     544,    49,    50,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   104,     0,     0,     0,    54,    55,
     525,     0,     0,    56,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   116,   117,
     118,   119,   120,   121,     0,     0,     0,     0,     0,   125,
     126,   127,   128,   129,   130,    68,     0,    70,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   134,   135,
     136,   137,   138,   139,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   244,   245,   246,   146,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   147,   247,
       0,   243,     0,     0,   248,     0,     0,     0,   249,   526,
       0,   664,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,     0,     0,   677,    95,    96,    97,
    1612,   526,     0,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,     0,     0,   540,   104,
       0,     0,  1640,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   116,   117,   118,   119,   120,   121,     0,
       0,     0,     0,     0,   125,   126,   127,   128,   129,   130,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   134,   135,   136,   137,   138,   139,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   244,   245,
     246,   763,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   147,   247,     0,     0,     0,     0,   248,
       0,     0,     0,   249,     0,   545,     0,   546,   547,   548,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
       0,     0,   559,     0,     0,   526,  1641,   664,   665,   666,
     667,   668,   669,   670,   671,   672,   673,   674,   675,   676,
       0,     0,   677,     0,     0,   526,  1640,   664,   665,   666,
     667,   668,   669,   670,   671,   672,   673,   674,   675,   676,
       0,     0,   677,     0,     0,     0,  1657,     0,     0,     0,
       0,     0,   526,     0,   527,   528,   529,   530,   531,   532,
     533,   534,   535,   536,   537,   538,   539,     0,     0,   540,
       0,     0,   545,  1658,   546,   547,   548,   549,   550,   551,
     552,   553,   554,   555,   556,   557,   558,     0,     0,   559,
       0,     0,   526,  1659,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,     0,   571,   677,
       0,     0,     0,  1658,   572,     0,   571,     0,     0,     0,
       0,     0,   572,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   202,   573,     0,     0,     0,     0,
       0,     0,   202,   573,   312,     0,     0,     0,   313,   314,
     315,   316,   317,   318,     0,     0,     0,   319,   320,   321,
     322,   323,   324,     0,   574,   575,     0,     0,     0,   576,
       0,     0,   574,   575,   577,   325,     0,   576,   326,     0,
       0,     0,   577,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   327,     0,     0,     0,
     578,     0,     0,     0,     0,     0,     0,     0,   578,   328,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   579,   580,     0,   581,     0,
     238,   239,   582,   579,   580,     0,   581,     0,   238,   239,
     582,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   339,   241,     0,     0,
       0,     0,   242,     0,     0,   241,     0,     0,     0,     0,
     242,     0,     0,     0,     0,     0,     0,     0,   583,     0,
       0,     0,     0,     0,     0,     0,   583,     0,     0,     0,
       0,   340,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   341,   584,     0,     0,     0,     0,     0,     0,
       0,   584,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   585,     0,     0,     0,     0,     0,     0,     0,
     585,     0,     0,   586,     0,     0,     0,     0,     0,     0,
       0,   586,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   587,     0,     0,     0,     0,     0,     0,     0,
     587,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   342,   343,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   588,     0,     0,     0,     0,     0,     0,     0,  1224
};

static const yytype_int16 yycheck[] =
{
       0,   293,   588,   167,    35,    81,    19,    20,   178,    69,
      62,   181,    49,   183,   184,    79,   186,   124,    46,   304,
      45,    75,    61,   104,   923,    75,   182,    27,    62,    46,
      66,    46,    34,    47,   323,   111,    62,    63,    44,    39,
      40,   122,   208,    75,   102,    52,    40,   213,    25,   375,
      27,   103,   124,   105,   125,    17,    33,   109,   110,    96,
     959,    46,    62,    40,    64,   158,   222,   329,    46,   158,
      47,    75,   398,   114,   218,    27,    60,    75,    69,   208,
     399,    33,   392,   124,   213,    62,   396,    64,    40,    69,
     122,    97,   354,   132,   133,    47,    22,    52,    98,   132,
     133,   392,    62,   103,   248,   394,   397,   392,    99,   109,
      62,    75,    64,    68,   167,   115,    19,    20,   155,   169,
     394,   171,   172,   123,   425,   426,   103,   428,   105,   165,
     394,   301,   109,   110,   215,   148,   208,   218,   394,   165,
    1039,   213,   142,   123,    69,   170,   394,   147,   148,   149,
     150,   135,   136,   394,   208,   162,   204,   167,   208,   213,
     394,    64,   117,   213,   236,   236,   128,    33,   222,   227,
     204,   169,   265,   171,   172,   304,   265,   102,   178,   179,
      69,   181,   182,   138,   354,   185,    75,   187,   188,   189,
     190,   168,    58,   395,   396,    98,   362,   394,   208,   212,
     202,   208,   209,   213,   206,   108,   213,   171,   172,   248,
     208,   132,   133,   145,   208,   213,   168,   253,   222,   213,
     204,     0,    27,   237,   179,   180,   181,   253,    33,   300,
     269,   270,   304,   122,   394,    40,   392,   221,    69,   239,
      19,    62,    47,    22,    75,   148,   206,   247,   248,   249,
     321,   279,    31,    51,   304,   280,   216,    62,   222,    64,
     237,   223,   279,   297,   279,   337,   169,   321,   208,    75,
     394,   226,    77,   213,   294,   230,   302,    75,   144,   293,
     146,   582,    61,   172,   332,   392,   313,   337,   295,   296,
     338,   122,    71,   214,   279,   322,   256,   304,   103,   165,
     105,   279,   227,   303,   109,   110,   304,   394,    87,   212,
     392,   392,   360,   147,   394,   397,   293,   115,    74,   303,
      99,   276,   101,   269,   270,   177,   178,   106,   107,   284,
     285,   345,   392,   167,  1233,   399,   320,   219,   246,   337,
     304,   172,   224,   122,   394,   124,    75,   279,   269,   270,
     395,   396,   366,   373,   206,    75,   356,    22,    69,   359,
     373,   392,    67,   168,   394,   231,   232,   387,   345,    99,
      75,   371,   542,   171,   172,   394,   376,   377,   310,   282,
     102,   397,   337,   394,   273,   206,   289,   253,   129,   366,
     117,   394,    22,   245,   301,   216,   392,   394,   204,   399,
     127,   397,   594,   399,   596,   597,   406,   394,   394,   396,
     208,   332,   412,   394,   366,   213,   222,   122,    59,   394,
     420,    62,   596,   597,   222,   394,   172,   183,   394,   429,
     430,    18,   432,   394,   604,   455,   163,   394,   394,   360,
     394,   441,   273,   309,     0,   311,   399,   132,   133,   399,
     450,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   463,   393,   391,   171,   172,   399,   225,
      72,   641,   394,   643,   394,   645,   394,    75,   392,   479,
      22,   301,   394,   394,   204,   394,   208,   487,   488,   489,
     490,   213,   386,   387,   388,   329,   330,   391,   498,   499,
     500,   501,   222,   394,   394,   227,   304,   234,   394,   288,
     586,   511,   512,   513,   394,   394,   394,   222,   394,   571,
     354,   355,   356,   357,   358,   359,   526,   527,   528,   529,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,   392,   594,   392,   596,   545,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,   559,
     560,   366,   576,   577,    71,   206,   271,   581,   392,   171,
     172,   585,   213,   171,   172,   216,   301,   273,   273,   358,
     397,   360,   361,   583,   269,   270,   264,   587,   293,   273,
     115,   386,   387,   388,   571,   595,   391,   301,   301,   576,
     577,   273,   377,   603,   581,   384,   208,   273,   585,   391,
     208,   213,   167,   394,   121,   213,   593,   594,   391,   596,
     597,   391,    22,   402,   576,   132,   133,   627,   399,   581,
     304,   140,   367,   585,   386,   387,   388,   637,  1224,   391,
     395,   593,   642,   396,   644,   395,   646,   647,   648,   649,
     273,   396,   396,   396,   433,    22,   396,   396,   395,   438,
     439,   220,   398,   396,   664,   665,   666,   667,   668,   669,
     670,   671,   672,   673,   674,   675,   676,   677,   678,   395,
     301,   681,   682,   396,   396,   301,   395,   687,   396,   396,
     690,   855,   372,   395,   986,   396,   395,   323,   396,   699,
     273,   208,   396,   147,   718,   396,   213,   396,   273,   374,
     273,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   724,   396,   391,   506,   372,   372,
     395,   396,   396,   396,   734,   396,   736,   737,   738,   396,
     396,   718,   742,    64,   374,   524,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,   396,
      75,   391,   269,   398,    22,   394,   396,   767,   768,   129,
     297,   576,   396,   297,   396,   395,   581,   396,   396,   396,
     585,   396,   395,   395,   395,   395,   395,   108,   593,   594,
     396,   596,   597,   398,   395,   395,    27,    67,   395,    69,
     115,   396,   802,   396,   396,    75,   396,   807,   396,    40,
     810,   132,   133,   813,    45,   396,    47,   854,   395,   395,
     820,   396,   601,   602,   396,   269,   270,   396,   396,   829,
     396,    62,   374,    64,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   395,   169,   391,
     395,   630,   122,    22,   396,   395,   171,   172,   395,   569,
     396,   396,   395,    34,   396,   396,  1158,   395,   508,    41,
     314,   315,   103,   317,   318,   524,  1025,    -1,   109,   856,
      -1,   325,   326,   327,   328,    -1,    22,    -1,   888,    -1,
     160,   329,   330,   208,   164,    67,    -1,    -1,   213,    -1,
      -1,   680,   172,    75,   856,    -1,    -1,   222,    -1,   946,
      -1,    -1,    -1,    -1,    -1,    -1,   354,   355,   356,   357,
     358,   359,   922,    -1,   924,    -1,   926,  1097,    -1,    -1,
      -1,  1101,    -1,    -1,    -1,    -1,   167,   168,    -1,   170,
      -1,    -1,    22,    -1,    -1,   215,   177,    -1,   269,   270,
     122,  1003,   124,    -1,    -1,    -1,    -1,    -1,   228,    -1,
     960,   192,    -1,    -1,    -1,    -1,   197,    -1,   289,    -1,
     749,    -1,    -1,   973,    -1,    -1,   976,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
    1004,   391,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    -1,  1005,    -1,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,   975,   391,    -1,  1026,  1004,    -1,   396,
     809,    -1,    -1,    -1,    -1,    -1,   208,   209,    -1,  1039,
      22,   213,  1042,    -1,    -1,    -1,    -1,    -1,    -1,  1049,
    1050,   856,    -1,    -1,    -1,    -1,  1226,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   236,   237,  1066,    -1,    -1,  1069,
    1070,  1071,    -1,    -1,  1074,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   255,    -1,    -1,    -1,    -1,    -1,  1066,
      22,    -1,  1069,    -1,    -1,    -1,    -1,    -1,  1098,  1099,
    1100,    -1,  1102,  1103,  1104,  1105,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1066,   346,   374,  1069,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,   304,   391,    -1,   366,    -1,    -1,   396,   370,
       0,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,    -1,
      -1,    -1,    -1,    -1,  1154,  1155,    -1,  1157,    -1,    -1,
      -1,    -1,    -1,   394,    -1,    -1,    -1,    -1,    -1,    -1,
     975,    -1,    -1,    -1,  1174,    -1,    -1,  1177,    -1,  1179,
      -1,  1181,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1190,  1191,    -1,    -1,  1194,    -1,    -1,    -1,    -1,  1199,
      -1,    -1,    -1,    -1,  1374,   374,    -1,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,    -1,   391,    -1,    -1,  1225,    -1,   396,  1228,  1229,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   374,  1239,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,    -1,
     396,  1066,    -1,    -1,  1069,    -1,    -1,    -1,    -1,    -1,
      -1,  1271,    -1,   504,    -1,    -1,   507,    -1,    -1,   510,
    1280,  1281,    -1,    -1,    -1,    -1,    -1,    -1,   148,   149,
     150,    -1,  1292,    -1,   374,  1295,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,
      -1,   181,   182,    -1,    -1,   185,  1326,    -1,    -1,    -1,
      -1,    -1,   563,   564,   565,   566,  1336,    -1,   569,    -1,
     571,   572,    -1,    -1,   575,   576,    -1,   578,    -1,    22,
      -1,   582,  1352,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   593,    -1,    -1,   596,   597,    -1,    -1,    -1,
      22,    -1,  1372,  1373,    -1,  1375,  1376,  1377,  1378,  1379,
      -1,    -1,    -1,    -1,    -1,  1385,  1165,    -1,    -1,    -1,
      -1,  1391,  1392,    -1,    22,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,  1407,  1408,   391,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1418,  1419,
      22,  1421,    -1,  1423,    -1,    -1,    -1,    -1,  1428,    -1,
      -1,  1431,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1441,    -1,  1222,    -1,  1445,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    22,   391,
     691,    -1,    -1,    -1,    -1,  1465,  1466,  1467,  1468,  1469,
    1470,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    -1,   718,    -1,    -1,
      -1,    -1,    -1,    -1,  1494,    -1,    -1,    -1,  1498,   359,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,
      -1,   752,   396,    -1,    -1,    22,  1526,  1527,  1528,  1529,
    1530,  1531,    -1,  1533,    -1,    -1,    -1,    -1,    -1,  1539,
      16,    22,    -1,    -1,    -1,    -1,    -1,    -1,  1548,  1549,
      -1,    -1,    28,    -1,    30,    -1,    -1,    -1,    -1,    35,
      36,    -1,    -1,    39,    -1,  1565,  1566,  1567,  1568,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    52,    -1,    -1,    -1,
      -1,   441,    -1,  1583,    -1,  1585,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1598,  1599,
    1600,    -1,    22,    79,    -1,    81,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1614,  1615,  1616,    -1,    94,    -1,
      -1,    -1,    -1,  1623,   855,   856,    22,    -1,    -1,   105,
      -1,  1631,  1632,    -1,   110,    -1,    -1,   113,    -1,  1639,
      -1,    -1,  1642,    -1,   296,    -1,    -1,    22,  1648,    -1,
      -1,    -1,    -1,    -1,  1654,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1662,    -1,   524,    -1,  1666,   527,   528,   529,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,    22,    -1,    -1,    -1,   545,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,   559,
     560,   374,   178,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    22,   193,   391,    -1,
      -1,    -1,   374,   396,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      -1,    -1,    -1,   603,   975,    -1,   374,   223,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,    -1,    -1,    -1,    -1,   396,    -1,
      -1,  1002,   374,    -1,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      -1,    -1,    -1,    -1,   396,    -1,    -1,   273,    -1,   275,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   285,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,   302,   391,   304,    74,
    1061,    -1,   396,    -1,  1065,  1066,    22,   374,  1069,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,  1086,    -1,   102,    -1,   396,
      -1,    -1,    -1,   108,    -1,    -1,    -1,    -1,    -1,   114,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     356,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,   368,   374,   391,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    22,    -1,  1145,   396,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   401,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   180,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   419,    -1,    -1,    -1,    22,   424,   425,
     426,   196,   428,   429,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,    -1,   396,   453,   374,    -1,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   238,    -1,   391,   472,    -1,    -1,   374,
     396,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,
      22,   396,  1253,    -1,    75,    -1,    -1,    -1,   273,   274,
      -1,    -1,    -1,   374,    -1,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,    -1,
     305,    22,    -1,    -1,    -1,    -1,    -1,    -1,   374,    -1,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,    -1,    -1,    22,    -1,
     396,    -1,    -1,    -1,  1325,    -1,    -1,   573,    -1,    -1,
      -1,    -1,   347,   579,  1335,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1346,   167,    -1,   594,    -1,
     171,   172,    -1,   368,   369,    -1,    -1,   372,    -1,   374,
     375,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     385,    -1,    -1,    -1,    22,    -1,    -1,    -1,   148,   149,
     150,   627,   397,   398,   399,    -1,    -1,   208,    -1,    -1,
      -1,   406,   213,    -1,    -1,   410,    -1,    -1,   413,    -1,
      -1,   222,    -1,    -1,    -1,    -1,    -1,   228,   423,    -1,
      -1,   181,   182,    -1,    -1,   185,    -1,    -1,   374,    -1,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   254,   449,   391,    -1,    -1,    -1,   454,
     396,   456,    -1,    -1,    -1,    -1,   692,    -1,    22,    -1,
      -1,    -1,   273,    -1,    -1,    -1,    -1,    -1,   473,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1098,  1099,
    1100,    -1,    -1,    -1,    -1,    -1,    -1,   492,   493,   494,
     495,    -1,  1483,   304,   730,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   374,  1496,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    22,
      -1,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1154,  1155,   772,  1157,   774,   374,
     776,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,  1177,    -1,    -1,
      -1,   396,    -1,    -1,    -1,    -1,    -1,    22,    -1,   574,
      -1,    -1,   577,    -1,  1194,   580,    -1,    -1,    -1,   584,
     585,    -1,    -1,    -1,    -1,    -1,  1577,  1578,    -1,    -1,
      -1,   596,   597,    -1,   830,   831,    -1,    -1,  1589,   359,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1228,  1229,
      -1,     0,   374,    -1,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      19,    -1,   637,    22,   396,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,   652,    -1,    -1,
      -1,    40,    -1,  1644,    -1,    -1,    45,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,  1660,
     391,    -1,    61,    -1,  1665,    64,    -1,    -1,    -1,  1670,
      -1,   441,    71,   688,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      99,    -1,   101,    -1,   103,    22,    -1,   106,   107,    -1,
      -1,   957,    -1,    -1,    -1,    -1,   962,    -1,    -1,    -1,
      -1,    -1,    -1,   122,   739,   124,   374,    22,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,    -1,    -1,    -1,    -1,   396,    -1,
      -1,    -1,    -1,   999,    22,    -1,    -1,   527,   528,   529,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,    -1,    -1,    -1,    -1,   545,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,   559,
     560,  1037,    22,    -1,  1040,    -1,    -1,    -1,    -1,  1045,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,
      -1,    -1,   396,    -1,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   603,    -1,  1465,  1466,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    22,    -1,   391,    -1,    -1,    -1,    -1,   396,
      -1,   374,    -1,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    -1,
      -1,    -1,    -1,   396,    -1,    -1,    -1,    -1,    -1,   288,
      -1,    22,    -1,    -1,    -1,    -1,    -1,  1143,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   374,
     925,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,  1173,    22,    -1,
      -1,   396,    -1,    -1,    -1,  1565,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   961,    -1,   963,   964,
      -1,    22,    -1,   968,    -1,    -1,    -1,    -1,    -1,   358,
      -1,   360,   361,    -1,    -1,    -1,    -1,    -1,    -1,   984,
     985,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   993,    -1,
      -1,    -1,    -1,    -1,  1614,   384,    -1,    -1,  1003,  1004,
      -1,  1006,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   402,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,  1642,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1036,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1044,
      -1,    -1,  1047,  1048,   433,    -1,    -1,    -1,    -1,   438,
     439,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1063,    -1,
      -1,  1066,    -1,    -1,  1069,    -1,    -1,    22,    -1,    -1,
    1075,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,  1321,  1091,    -1,    -1,   374,
      -1,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,  1109,    -1,   391,    -1,    -1,    -1,
      -1,   396,    -1,    -1,    -1,    -1,   374,   506,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,    -1,   524,    -1,    -1,   396,    -1,
      -1,    -1,    -1,  1148,  1149,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
    1406,   391,  1177,  1409,    -1,  1180,   396,    -1,  1183,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,   374,  1193,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    22,    -1,    -1,    -1,   396,
      -1,    -1,   601,   602,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,
      -1,   630,  1247,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   374,  1490,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     374,   680,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,
      -1,  1547,   396,   374,    -1,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,  1337,    -1,    -1,   396,    -1,    -1,  1098,  1099,
    1100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1354,
    1355,    -1,    -1,    -1,    -1,   744,    -1,    -1,    -1,    -1,
     749,    -1,    -1,    -1,    -1,    -1,  1371,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    -1,    -1,    -1,  1393,   396,
      -1,    -1,    -1,    -1,  1154,  1155,    -1,  1157,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1647,    -1,    -1,    -1,    -1,    -1,  1177,    -1,   374,
     809,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,  1194,    -1,   391,    -1,    -1,    -1,
      -1,   396,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1457,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,  1228,  1229,
      -1,    -1,    -1,    -1,    -1,  1480,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1491,    -1,  1493,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   374,  1503,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    -1,    -1,    -1,    -1,   396,
    1525,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   374,  1540,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,  1560,   396,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    -1,    -1,    -1,    -1,   396,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     0,     1,  1603,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1611,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    19,    20,    21,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    35,    -1,    37,    38,    -1,    -1,    41,    42,    -1,
      -1,    -1,    46,    47,    48,    -1,    50,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      64,    65,    -1,    -1,    -1,    -1,    70,    -1,    -1,    73,
      -1,    -1,    -1,    -1,    -1,    -1,    80,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    89,    90,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,   107,   108,  1465,  1466,   111,    -1,   113,
      -1,    -1,   116,    -1,    -1,   119,   120,    -1,    -1,    -1,
     124,    -1,   126,    -1,    -1,    -1,    -1,   131,   132,   133,
      -1,    -1,    -1,   137,    -1,   139,    -1,    -1,   142,    -1,
      -1,    -1,    -1,    -1,    -1,   149,   150,    -1,    -1,    -1,
     154,   155,   156,   157,    -1,   159,    -1,   161,    -1,    -1,
      -1,    -1,   166,    -1,   168,   169,    -1,    -1,    -1,   173,
     174,    -1,    -1,    -1,    -1,    -1,  1165,    -1,    -1,    -1,
      -1,   185,   186,   187,   188,    -1,    -1,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   201,    -1,   203,
      -1,   205,    -1,    -1,    -1,  1565,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   374,   218,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,   233,
     391,    -1,    -1,  1222,   238,   239,   240,   241,   242,   243,
      -1,    -1,   246,   247,   248,   249,    -1,   251,   252,    -1,
      -1,    -1,    -1,   257,  1614,   259,   260,   261,   262,   263,
     264,    -1,    -1,    -1,   268,   269,   270,    -1,    -1,    -1,
      -1,   275,    -1,   277,   278,    -1,    -1,   281,   282,    -1,
      -1,    -1,  1642,    -1,    -1,   289,   290,   291,   292,    -1,
     294,    -1,    -1,    -1,   298,   299,   300,    -1,    -1,    -1,
     304,    -1,   306,   307,   308,    -1,    -1,    -1,    -1,   313,
     314,   315,   316,   317,   318,   319,    -1,   321,   322,    -1,
     324,   325,   326,   327,   328,   329,   330,    -1,   332,   333,
     334,    -1,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
     354,   355,   356,   357,   358,   359,   360,   361,     0,     1,
      -1,    -1,    -1,    -1,   368,   369,   370,   371,   372,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    20,    22,
     384,   385,    -1,    -1,    -1,    -1,   390,    -1,    -1,   393,
     394,    -1,    -1,    35,    -1,    37,    38,    -1,    -1,    41,
      42,    -1,    22,    -1,    46,    47,    48,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    64,    65,    -1,    22,    -1,    -1,    70,    -1,
      -1,    73,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    98,    99,   100,   101,
      -1,    -1,    -1,    -1,    -1,   107,   108,    -1,    -1,   111,
      -1,   113,    -1,    -1,   116,    -1,    -1,   119,   120,    -1,
      -1,    -1,   124,    -1,   126,    -1,    -1,    -1,    -1,   131,
     132,   133,    -1,    -1,    -1,   137,    -1,   139,    -1,    -1,
     142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   155,   156,   157,    -1,   159,    -1,   161,
      22,    -1,    -1,    -1,   166,    -1,   168,   169,    -1,    -1,
      -1,   173,   174,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   185,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,   201,
      -1,   203,    -1,   205,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   218,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   233,    -1,    -1,    -1,    -1,   238,   239,   240,   241,
      -1,   243,    -1,    -1,   246,   247,   248,   249,    -1,   251,
     252,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,   261,
     262,   263,   264,    -1,    -1,    -1,    -1,   269,   270,    -1,
      -1,    -1,    -1,   275,    -1,   277,   278,    -1,    -1,   281,
     282,    -1,    -1,    -1,    -1,    -1,    -1,   289,    -1,   291,
     292,    -1,   294,    -1,    -1,    -1,   298,   299,   300,    -1,
      -1,    -1,   304,    -1,   306,   307,   308,    -1,    -1,    -1,
      -1,   313,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   321,
     322,    -1,   324,    -1,    -1,    -1,    -1,   329,   330,    -1,
     332,   333,   334,    18,    19,    20,    21,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,   354,   355,   356,   357,   358,   359,   360,   361,
      -1,    -1,    -1,    -1,    -1,    50,   368,   369,   370,   371,
      -1,   374,    -1,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    -1,
      -1,   393,    -1,   396,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,    -1,   396,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    -1,    -1,    -1,    -1,   396,
      -1,    -1,    -1,    22,    -1,    -1,    -1,   132,   133,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   149,   150,    -1,    -1,    -1,   154,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,
      20,    21,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   186,    -1,   188,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    -1,   374,    -1,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,    78,    -1,
      -1,    -1,    82,    83,    84,    85,    86,    87,    -1,    -1,
      -1,    91,    92,    93,    94,    95,    96,   242,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
      -1,    -1,   112,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   268,   269,   270,    -1,    -1,    -1,    -1,
     130,    -1,   132,   133,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,    -1,   290,    -1,    22,    -1,   149,
     150,    -1,    -1,    -1,   154,    -1,    -1,    -1,    52,    53,
      54,    -1,    56,    -1,    -1,    -1,    -1,    61,    -1,   314,
     315,   316,   317,   318,   319,    69,    -1,    -1,    -1,    -1,
     325,   326,   327,   328,   329,   330,   186,    -1,   188,    -1,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   354,
     355,   356,   357,   358,   359,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   369,   370,   371,   372,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   235,    -1,    -1,    -1,   384,
     385,    -1,   242,    -1,    -1,   390,    -1,    -1,    -1,   394,
      -1,    -1,    -1,   398,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   266,    -1,   268,   269,
     270,    -1,    -1,    -1,    18,    19,    20,    21,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
     290,    -1,    -1,    -1,    -1,    39,    -1,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    50,    -1,    -1,    -1,
      -1,    -1,    -1,   207,   314,   315,   316,   317,   318,   319,
      -1,    -1,    -1,    67,    -1,   325,   326,   327,   328,   329,
     330,    75,    -1,    -1,    22,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,    -1,   391,    -1,   354,   355,   356,   357,   358,   359,
      -1,    -1,    -1,    -1,    -1,   365,   366,    -1,    -1,   369,
     370,   371,   372,    22,    -1,    -1,    -1,    -1,   122,    -1,
     124,   125,    -1,    -1,   384,   385,    -1,    -1,   132,   133,
     390,    -1,    -1,    -1,   394,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   149,   150,    22,    -1,    -1,
     154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     164,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   172,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   186,   337,   188,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,   353,
      -1,    -1,    -1,    -1,   208,   209,    -1,    -1,    -1,   213,
      -1,   215,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   228,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   236,   237,    -1,    -1,    -1,    -1,   242,   374,
      -1,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,
      -1,   396,    -1,    -1,   268,   269,   270,    -1,   272,    -1,
     274,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   286,   287,   288,    -1,   290,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     304,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     314,   315,   316,   317,   318,   319,    -1,    -1,    -1,    -1,
      -1,   325,   326,   327,   328,   329,   330,    18,    19,    20,
      21,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
     354,   355,   356,   357,   358,   359,    -1,    -1,    -1,    50,
      -1,    -1,    -1,    -1,    -1,   369,   370,   371,   372,    -1,
      -1,    62,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     384,   385,    -1,    -1,    75,    -1,   390,    -1,    -1,    -1,
     394,    -1,    -1,    -1,    -1,    -1,    -1,   374,    89,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    22,    -1,   391,    -1,    -1,    -1,    -1,   396,
      -1,    -1,    -1,    -1,    -1,    -1,   374,    -1,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,   132,   133,   391,    -1,    -1,    -1,    -1,   396,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    22,    -1,   149,   150,
     151,    -1,    -1,   154,    -1,   374,    -1,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,   172,   391,    -1,    -1,   176,    -1,   396,    -1,    -1,
      -1,    -1,    -1,   184,    -1,   186,    -1,   188,   189,   374,
      -1,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,   208,    22,   210,
     211,   396,   213,    -1,   215,    -1,   217,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   228,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   242,    -1,    -1,    -1,    18,    19,    20,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   268,   269,   270,
      -1,    -1,   273,    -1,    -1,    -1,    -1,    50,   279,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,
      -1,    64,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,   304,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   314,   315,   316,   317,   318,   319,    -1,
      -1,    -1,    -1,    -1,   325,   326,   327,   328,   329,   330,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    22,    -1,   391,    -1,   122,
      -1,    -1,   396,   354,   355,   356,   357,   358,   359,   132,
     133,    -1,    -1,    -1,    -1,    -1,    22,   140,   369,   370,
     371,   372,    -1,    -1,    -1,    -1,   149,   150,    -1,    -1,
      -1,   154,    -1,   384,   385,   158,    -1,    -1,    -1,   390,
      -1,   164,    -1,   394,   167,    -1,    -1,    -1,   171,   172,
      -1,    -1,   175,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   186,   374,   188,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,   208,   396,    -1,    -1,    -1,
     213,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,   242,
      -1,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   268,   269,   270,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,   289,   290,    18,    19,
      20,    21,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,   312,
      -1,   314,   315,   316,   317,   318,   319,    -1,    -1,    -1,
      50,    51,   325,   326,   327,   328,   329,   330,   331,    -1,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,
      -1,   354,   355,   356,   357,   358,   359,    -1,    -1,    -1,
      -1,    22,    -1,    -1,    -1,    -1,   369,   370,   371,   372,
      -1,    -1,    -1,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   384,   385,    -1,    -1,    -1,    -1,   390,    -1,    -1,
      -1,   394,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    -1,   132,   133,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,
     150,   151,    -1,   153,   154,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   164,   165,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   186,    -1,   188,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    -1,    -1,   217,   374,   396,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,    -1,
     396,    -1,   242,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     250,    -1,    -1,   253,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   267,   268,   269,
     270,    -1,    -1,   273,    18,    19,    20,    21,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
     290,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   305,    50,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   314,   315,   316,   317,   318,   319,
      -1,    -1,    -1,    67,    -1,   325,   326,   327,   328,   329,
     330,    75,    -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   354,   355,   356,   357,   358,   359,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   369,
     370,   371,   372,    -1,    -1,    -1,    -1,    -1,   122,    -1,
      -1,    -1,    -1,    -1,   384,   385,    -1,    -1,   132,   133,
     390,    -1,    -1,    -1,   394,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   149,   150,    -1,    -1,    -1,
     154,    -1,    -1,    -1,    -1,    -1,   160,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   172,    -1,
      -1,   175,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   186,   374,   188,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    -1,    -1,   208,    18,    19,    20,    21,   213,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,   374,    -1,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    50,   242,   391,
      -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,    -1,    -1,
      22,   255,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    -1,   268,   269,   270,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    -1,   290,    -1,    -1,   396,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     314,   315,   316,   317,   318,   319,    -1,    -1,    -1,   132,
     133,   325,   326,   327,   328,   329,   330,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,    -1,
      -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     354,   355,   356,   357,   358,   359,    -1,    -1,   171,   172,
      -1,    -1,    -1,    -1,    -1,   369,   370,   371,   372,    -1,
      -1,    -1,    -1,   186,    -1,   188,    -1,    -1,    -1,    -1,
     384,   385,    -1,    -1,    -1,    -1,   390,    -1,    -1,    -1,
     394,    -1,    -1,    -1,    18,    19,    20,    21,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,   222,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,   374,   242,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,    -1,
     396,    75,    -1,    -1,    -1,   268,   269,   270,    22,    -1,
     273,   274,    -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   314,   315,   316,   317,   318,   319,    22,   132,   133,
      -1,    -1,   325,   326,   327,   328,   329,   330,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   149,   150,    -1,    -1,    -1,
     154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   354,   355,   356,   357,   358,   359,   171,   172,    -1,
      -1,   175,    -1,    -1,    -1,    -1,   369,   370,   371,   372,
      -1,    -1,   186,    -1,   188,    -1,    -1,    -1,    -1,    -1,
      -1,   384,   385,    -1,    -1,    -1,    -1,   390,    -1,    -1,
      -1,   394,    -1,    -1,   208,    18,    19,    20,    21,   213,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,   374,    -1,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    50,   242,   391,
      -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    -1,   268,   269,   270,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     314,   315,   316,   317,   318,   319,    -1,    22,    -1,   132,
     133,   325,   326,   327,   328,   329,   330,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,    -1,
      -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     354,   355,   356,   357,   358,   359,    -1,    -1,   171,   172,
      -1,    -1,   175,    -1,    -1,   369,   370,   371,   372,    -1,
      -1,    -1,    -1,   186,    -1,   188,    -1,    -1,    -1,    -1,
     384,   385,    -1,    -1,    -1,    -1,   390,    -1,    -1,    -1,
     394,    -1,    -1,    -1,    -1,   208,    18,    19,    20,    21,
     213,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    -1,    -1,    -1,    -1,    -1,    50,   242,
     374,    22,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,
      -1,    -1,   396,    75,    -1,   268,   269,   270,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,   374,
      -1,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,
     395,   314,   315,   316,   317,   318,   319,    -1,    -1,    22,
     132,   133,   325,   326,   327,   328,   329,   330,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,
      -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   354,   355,   356,   357,   358,   359,    -1,    22,   171,
     172,    -1,    -1,   175,    -1,    -1,   369,   370,   371,   372,
      -1,    -1,    -1,    -1,   186,    -1,   188,    -1,    -1,    -1,
      -1,   384,   385,    -1,    -1,    -1,    -1,   390,    -1,    -1,
      -1,   394,    -1,    -1,    -1,    -1,   208,    18,    19,    20,
      21,   213,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    50,
     242,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    -1,   268,   269,   270,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,   290,   374,
      -1,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,
     395,   122,   314,   315,   316,   317,   318,   319,    -1,    -1,
      -1,   132,   133,   325,   326,   327,   328,   329,   330,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,
      -1,    -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   354,   355,   356,   357,   358,   359,    -1,    -1,
      -1,   172,    -1,    -1,    -1,    -1,    -1,   369,   370,   371,
     372,    -1,    -1,    -1,    -1,   186,    -1,   188,    -1,    -1,
      -1,    -1,   384,   385,    -1,    -1,    -1,    -1,   390,    -1,
      -1,    -1,   394,    -1,    -1,    -1,    -1,   208,    -1,    -1,
      -1,    22,   213,   374,    -1,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,    -1,
      -1,   242,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    19,    20,    21,   255,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,   268,   269,   270,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,    -1,   290,
      -1,   374,    -1,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    75,   391,    -1,
      -1,    -1,   395,   314,   315,   316,   317,   318,   319,    -1,
      -1,    -1,    -1,    -1,   325,   326,   327,   328,   329,   330,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,
      -1,    22,   396,   354,   355,   356,   357,   358,   359,    -1,
      -1,    -1,    -1,    -1,   132,   133,   134,    -1,   369,   370,
     371,   372,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      22,   149,   150,   384,   385,    -1,   154,    -1,    -1,   390,
      -1,    -1,    -1,   394,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   171,   172,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   186,   374,
     188,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,
     208,   396,    -1,    -1,   374,   213,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   242,    -1,    -1,    -1,    18,    19,
      20,    21,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     268,   269,   270,    22,    -1,   273,    -1,    -1,    -1,    -1,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   290,    -1,    37,    38,    22,    -1,    -1,   297,
      -1,    -1,    -1,    -1,    47,    75,    -1,    -1,    -1,    52,
      -1,    -1,    -1,    -1,    -1,    -1,   314,   315,   316,   317,
     318,   319,    -1,    -1,    -1,    -1,    -1,   325,   326,   327,
     328,   329,   330,   374,    -1,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    -1,    -1,    -1,   396,   354,   355,   356,   357,
     358,   359,   132,   133,   134,    -1,    -1,    -1,    -1,    -1,
      -1,   369,   370,   371,   372,   118,    -1,    -1,    -1,   149,
     150,    -1,    -1,    -1,   154,    -1,   384,   385,    -1,    -1,
      -1,    -1,   390,    -1,    -1,    -1,   394,    -1,   141,    -1,
      -1,   171,   172,    -1,    -1,    -1,    -1,    52,    53,    54,
      -1,    -1,    -1,    -1,    -1,    -1,   186,    -1,   188,   162,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   208,    18,
      19,    20,    21,   213,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    22,    -1,    -1,   208,    -1,    -1,    -1,    -1,
     213,    50,   242,   374,    -1,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    -1,    -1,    -1,   396,    75,    -1,   268,   269,
     270,    -1,   374,   273,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
     290,    -1,    -1,    -1,   396,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,
     283,    -1,    -1,    -1,   314,   315,   316,   317,   318,   319,
      -1,    -1,    -1,   132,   133,   325,   326,   327,   328,   329,
     330,   304,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     149,   150,    -1,    22,    -1,   154,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   354,   355,   356,   357,   358,   359,
     333,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   369,
     370,   371,   372,    -1,    -1,    -1,    -1,   186,    -1,   188,
      -1,    -1,    -1,    -1,   384,   385,    -1,    -1,   361,    -1,
     390,    -1,    -1,    -1,   394,   374,    -1,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,    -1,   391,   222,    -1,    -1,    -1,   396,   374,   392,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   242,    -1,   391,    -1,    -1,    -1,    -1,
     396,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,   258,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   268,
     269,   270,   337,    -1,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,    -1,
      -1,   290,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   304,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   314,   315,   316,   317,   318,
     319,    -1,   321,    -1,    -1,    -1,   325,   326,   327,   328,
     329,   330,    -1,    18,    19,    20,    21,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   354,   355,   356,   357,   358,
     359,    46,    -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,
     369,   370,   371,   372,    -1,    -1,    -1,    62,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   384,   385,    -1,    -1,    -1,
      -1,   390,    -1,    -1,   374,   394,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,
      19,    20,    21,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,   132,   133,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    -1,    -1,   149,   150,    -1,    -1,   374,   154,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,    -1,
     396,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   186,    -1,   188,    -1,   374,    -1,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,   206,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,
      -1,   216,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   132,   133,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    -1,   242,    -1,    -1,
     149,   150,    -1,    -1,    -1,   154,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    22,    -1,   268,   269,   270,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   279,    -1,    -1,   186,    -1,   188,
      -1,    -1,    -1,    -1,   374,   290,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,    -1,   396,    -1,    -1,   314,
     315,   316,   317,   318,   319,    -1,    -1,    -1,    -1,    -1,
     325,   326,   327,   328,   329,   330,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   242,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   354,
     355,   356,   357,   358,   359,    -1,    -1,    -1,    -1,   268,
     269,   270,    -1,    -1,   369,   370,   371,   372,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   384,
     385,   290,    -1,    -1,    -1,   390,    -1,    -1,    -1,   394,
      18,    19,    20,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,   314,   315,   316,   317,   318,
     319,    -1,    -1,    -1,    -1,    -1,   325,   326,   327,   328,
     329,   330,    50,    -1,    -1,    -1,   335,   336,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   354,   355,   356,   357,   358,
     359,    -1,    -1,    -1,   363,   364,    -1,    -1,    -1,    -1,
     369,   370,   371,   372,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   384,   385,    -1,    -1,    -1,
      -1,   390,    -1,    -1,   374,   394,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    22,
      -1,   391,    -1,    -1,   132,   133,   396,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   149,   150,    -1,    -1,    -1,   154,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    19,    20,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   186,    -1,
     188,    -1,    -1,    -1,    -1,    -1,   374,    50,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,    -1,    -1,    -1,   374,   396,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    22,    -1,   391,    -1,    -1,    -1,    -1,   396,
      -1,    -1,    -1,   374,   242,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,    -1,    -1,
     391,    -1,    -1,    -1,   395,    -1,    -1,    -1,    -1,    -1,
     268,   269,   270,    -1,    -1,    -1,    -1,    -1,    -1,   132,
     133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   290,    -1,    -1,    -1,   149,   150,    -1,    -1,
      -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   164,    -1,    -1,   167,    -1,   314,   315,   316,   317,
     318,   319,    -1,    -1,    -1,    -1,    -1,   325,   326,   327,
     328,   329,   330,   186,    -1,   188,    -1,   335,   336,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   354,   355,   356,   357,
     358,   359,    22,    -1,    -1,   363,   364,    -1,    -1,    -1,
      -1,   369,   370,   371,   372,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   384,   385,    -1,   242,
      -1,    -1,   390,    -1,    -1,    -1,   394,    18,    19,    20,
      21,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,   268,   269,   270,    -1,    -1,
     273,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   304,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   314,   315,   316,   317,   318,   319,    -1,    -1,    -1,
      -1,    -1,   325,   326,   327,   328,   329,   330,    -1,    -1,
      -1,   374,    -1,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    -1,
      -1,   354,   355,   356,   357,   358,   359,    -1,    -1,    -1,
      -1,   132,   133,    -1,    -1,    -1,   369,   370,   371,   372,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,
      -1,   384,   385,   154,    -1,    -1,    -1,   390,    -1,    -1,
      -1,   394,    18,    19,    20,    21,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   186,   187,   188,    -1,    -1,
      -1,    -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,   395,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   242,    -1,    -1,    -1,    -1,    -1,   248,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,   257,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   268,   269,   270,
      -1,    -1,    22,    -1,    -1,    -1,   132,   133,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,
      -1,    -1,    22,   149,   150,    -1,    -1,    -1,   154,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   314,   315,   316,   317,   318,   319,    -1,
      -1,    -1,    -1,    -1,   325,   326,   327,   328,   329,   330,
     186,   187,   188,    -1,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,   354,   355,   356,   357,   358,   359,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   369,   370,
     371,   372,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    -1,    -1,   384,   385,    -1,   242,    -1,    -1,   390,
      -1,    -1,   248,   394,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   257,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   268,   269,   270,    -1,    18,    19,    20,    21,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,   290,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   314,   315,
     316,   317,   318,   319,    -1,    -1,    -1,    69,    -1,   325,
     326,   327,   328,   329,   330,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   354,   355,
     356,   357,   358,   359,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   369,   370,   371,   372,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,   384,   385,
     132,   133,    -1,    -1,   390,    -1,    -1,    -1,   394,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,
      -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    19,    20,    21,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    -1,   186,    -1,   188,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,
      -1,   374,    57,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    22,
      -1,    -1,   395,    -1,   374,    -1,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
     242,   391,    -1,    -1,   374,   395,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,   395,   268,   269,   270,    22,
      -1,    -1,    -1,    -1,    -1,    -1,   278,   132,   133,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,
      -1,    -1,    -1,    -1,   149,   150,    -1,   152,    -1,   154,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   314,   315,   316,   317,   318,   319,    -1,    -1,
      -1,    -1,    -1,   325,   326,   327,   328,   329,   330,    -1,
      -1,   186,   374,   188,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      -1,    -1,   354,   355,   356,   357,   358,   359,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   369,   370,   371,
     372,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   384,   385,    -1,    -1,    -1,   242,   390,    -1,
      -1,    -1,   394,    -1,    -1,    18,    19,    20,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,   268,   269,   270,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,    -1,
      -1,    -1,    -1,    -1,   374,   290,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    75,    -1,    -1,   395,    -1,    -1,    -1,   314,
     315,   316,   317,   318,   319,    -1,    -1,    -1,    -1,    -1,
     325,   326,   327,   328,   329,   330,   374,    -1,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,    -1,    -1,    -1,   395,    -1,   354,
     355,   356,   357,   358,   359,    22,    -1,    -1,    -1,   132,
     133,    -1,    -1,    -1,   369,   370,   371,   372,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,   384,
     385,   154,    -1,    -1,    -1,   390,    -1,    -1,    -1,   394,
      18,    19,    20,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   186,    -1,   188,    -1,    -1,    46,    -1,
      -1,   374,    50,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    -1,
      -1,    -1,   395,    -1,    -1,    -1,    22,    -1,    -1,   222,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,   242,
      -1,   374,    -1,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    -1,
      -1,    -1,   395,    -1,    -1,   268,   269,   270,    22,    -1,
      -1,    -1,    -1,    -1,   132,   133,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,    -1,
      -1,   149,   150,    -1,    -1,    -1,   154,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   314,   315,   316,   317,   318,   319,    -1,    -1,    -1,
      -1,    -1,   325,   326,   327,   328,   329,   330,   186,    -1,
     188,   374,    -1,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    -1,
      -1,   354,   355,   356,   357,   358,   359,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   369,   370,   371,   372,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   384,   385,    -1,   242,    -1,    -1,   390,    -1,    -1,
      -1,   394,    18,    19,    20,    21,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
     268,   269,   270,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   279,    -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   290,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    -1,    -1,    -1,    -1,   314,   315,   316,   317,
     318,   319,    -1,    -1,    -1,    -1,    -1,   325,   326,   327,
     328,   329,   330,    -1,    -1,    -1,    -1,   374,    -1,   376,
     377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
     387,   388,    -1,    -1,   391,    -1,   354,   355,   356,   357,
     358,   359,    -1,    -1,    -1,    -1,   132,   133,    -1,    -1,
      -1,   369,   370,   371,   372,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   149,   150,    -1,   384,   385,   154,    -1,
      -1,    -1,   390,    -1,    -1,    -1,   394,    18,    19,    20,
      21,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     186,    -1,   188,    -1,    -1,    -1,    -1,    -1,   374,    50,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,    -1,    -1,    69,   395,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    -1,    -1,   391,    -1,    -1,
      -1,   395,    22,    -1,    -1,    -1,   242,    -1,   244,    -1,
     374,    -1,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,    37,    38,   391,    -1,    -1,
      22,   395,   268,   269,   270,    47,    -1,    -1,    -1,    -1,
      52,   132,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   290,    -1,    -1,    -1,   149,   150,
      -1,    -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   314,   315,
     316,   317,   318,   319,    -1,    -1,    -1,    -1,    -1,   325,
     326,   327,   328,   329,   330,   186,    -1,   188,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   118,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   354,   355,
     356,   357,   358,   359,    -1,    -1,    -1,    -1,    -1,   141,
      -1,    -1,    -1,   369,   370,   371,   372,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   384,   385,
     162,   242,    -1,    -1,   390,    -1,    -1,    -1,   394,    18,
      19,    20,    21,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,   268,   269,   270,
      -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    -1,    -1,    -1,    -1,   208,    -1,    -1,   290,
      -1,   213,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   314,   315,   316,   317,   318,   319,    -1,
      -1,    -1,    -1,    -1,   325,   326,   327,   328,   329,   330,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   354,   355,   356,   357,   358,   359,    -1,
      -1,   283,    -1,   132,   133,    -1,    -1,    -1,   369,   370,
     371,   372,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     149,   150,   304,   384,   385,   154,    -1,    -1,    -1,   390,
      -1,    -1,    -1,   394,    18,    19,    20,    21,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,   333,    -1,    -1,    -1,    -1,    -1,   186,    -1,   188,
      -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   361,
      -1,    -1,    -1,    -1,   374,    69,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,    -1,
      -1,   391,    -1,    -1,    -1,   395,    -1,    -1,    -1,    -1,
     392,    -1,   374,   242,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    22,    -1,   391,
      -1,    -1,    -1,   395,    -1,    -1,    -1,    -1,    -1,   268,
     269,   270,    -1,    -1,    -1,    -1,    -1,    -1,   132,   133,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,
      -1,   290,    -1,    -1,    -1,   149,   150,    -1,    -1,    -1,
     154,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   314,   315,   316,   317,   318,
     319,    -1,    -1,   322,    -1,    -1,   325,   326,   327,   328,
     329,   330,   186,    -1,   188,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   354,   355,   356,   357,   358,
     359,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     369,   370,   371,   372,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    -1,    -1,    -1,    -1,   384,   385,    -1,   242,    -1,
      -1,   390,    -1,    -1,    -1,   394,   374,    -1,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,   268,   269,   270,   395,    18,    19,
      20,    21,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,   290,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     314,   315,   316,   317,   318,   319,    -1,    -1,    -1,    -1,
      -1,   325,   326,   327,   328,   329,   330,    -1,    -1,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     354,   355,   356,   357,   358,   359,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   369,   370,   371,   372,    -1,
      -1,    -1,   122,    -1,    22,    -1,    -1,    -1,    -1,    -1,
     384,   385,   132,   133,    -1,    -1,   390,    -1,    -1,    -1,
     394,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,
     150,    -1,    -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    19,    20,    21,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   186,    -1,   188,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   374,
      -1,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    22,    -1,   391,    -1,    -1,    -1,
     395,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   374,   242,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,    -1,
      -1,    -1,   395,    -1,    -1,    -1,    22,    -1,   268,   269,
     270,    -1,    -1,    -1,    -1,    -1,    -1,   132,   133,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     290,    -1,    -1,    -1,   149,   150,    -1,    -1,    -1,   154,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   314,   315,   316,   317,   318,   319,
      -1,    -1,    -1,    -1,    -1,   325,   326,   327,   328,   329,
     330,   186,   374,   188,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      -1,    -1,    -1,   395,   354,   355,   356,   357,   358,   359,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   369,
     370,   371,   372,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   384,   385,    -1,   242,    -1,    -1,
     390,    -1,    -1,    -1,   394,    18,    19,    20,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,   268,   269,   270,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,    -1,
      -1,    -1,    -1,    -1,   374,   290,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,   304,
      -1,   391,    -1,    -1,    -1,   395,    -1,    -1,    -1,   314,
     315,   316,   317,   318,   319,    -1,    -1,    -1,    -1,    -1,
     325,   326,   327,   328,   329,   330,   374,    -1,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,    -1,    -1,    -1,   395,    -1,   354,
     355,   356,   357,   358,   359,    -1,    -1,    -1,    -1,   132,
     133,    -1,    -1,    -1,   369,   370,   371,   372,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,   384,
     385,   154,    -1,    -1,    -1,   390,    -1,    -1,    -1,   394,
      18,    19,    20,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   186,    -1,   188,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    -1,   374,    -1,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,    -1,    -1,   391,    -1,    -1,    22,   395,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    22,    -1,   374,   242,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    -1,    -1,   391,    -1,    -1,    -1,   395,
      -1,    -1,    -1,    22,    -1,   268,   269,   270,    -1,    -1,
      -1,    -1,    -1,    -1,   132,   133,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,    -1,
      -1,   149,   150,    -1,    -1,    -1,   154,    -1,    -1,    -1,
      -1,   304,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   314,   315,   316,   317,   318,   319,    -1,    -1,    -1,
      -1,    -1,   325,   326,   327,   328,   329,   330,   186,    -1,
     188,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   354,   355,   356,   357,   358,   359,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   369,   370,   371,   372,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   384,   385,    -1,   242,    -1,    -1,   390,    -1,    -1,
      -1,   394,    18,    19,    20,    21,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
     268,   269,   270,    -1,    -1,    -1,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   290,    -1,    -1,    -1,    -1,    -1,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,   304,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   314,   315,   316,   317,
     318,   319,    -1,    -1,    -1,    -1,    -1,   325,   326,   327,
     328,   329,   330,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   354,   355,   356,   357,
     358,   359,    -1,    -1,    -1,    -1,   132,   133,    -1,    -1,
      -1,   369,   370,   371,   372,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   149,   150,    -1,   384,   385,   154,    -1,
      -1,    -1,   390,    -1,    -1,    -1,   394,    18,    19,    20,
      21,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     186,    -1,   188,    22,    -1,    -1,    -1,    -1,   374,    50,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    22,    -1,   391,    -1,    -1,   374,   395,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,    22,    -1,   391,    -1,    -1,    -1,   395,
      -1,    -1,    -1,    -1,    -1,   374,   242,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      22,    -1,   391,    -1,    -1,    -1,   395,    -1,    -1,    -1,
      -1,    -1,   268,   269,   270,    -1,    -1,    -1,    -1,    -1,
      22,   132,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   290,    -1,    -1,    -1,   149,   150,
      22,    -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   314,   315,
     316,   317,   318,   319,    -1,    -1,    -1,    -1,    -1,   325,
     326,   327,   328,   329,   330,   186,    -1,   188,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   354,   355,
     356,   357,   358,   359,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   369,   370,   371,   372,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   384,   385,
      -1,   242,    -1,    -1,   390,    -1,    -1,    -1,   394,   374,
      -1,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,   268,   269,   270,
     395,   374,    -1,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,    -1,    -1,   391,   290,
      -1,    -1,   395,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   314,   315,   316,   317,   318,   319,    -1,
      -1,    -1,    -1,    -1,   325,   326,   327,   328,   329,   330,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   354,   355,   356,   357,   358,   359,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   369,   370,
     371,   372,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   384,   385,    -1,    -1,    -1,    -1,   390,
      -1,    -1,    -1,   394,    -1,   374,    -1,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,    -1,   391,    -1,    -1,   374,   395,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,    -1,   391,    -1,    -1,   374,   395,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
      -1,    -1,   391,    -1,    -1,    -1,   395,    -1,    -1,    -1,
      -1,    -1,   374,    -1,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      -1,    -1,   374,   395,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    -1,   391,
      -1,    -1,   374,   395,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,    -1,    49,   391,
      -1,    -1,    -1,   395,    55,    -1,    49,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    78,    -1,    -1,    -1,    82,    83,
      84,    85,    86,    87,    -1,    -1,    -1,    91,    92,    93,
      94,    95,    96,    -1,   105,   106,    -1,    -1,    -1,   110,
      -1,    -1,   105,   106,   115,   109,    -1,   110,   112,    -1,
      -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,
     141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,   143,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   166,   167,    -1,   169,    -1,
     171,   172,   173,   166,   167,    -1,   169,    -1,   171,   172,
     173,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   208,    -1,    -1,
      -1,    -1,   213,    -1,    -1,   208,    -1,    -1,    -1,    -1,
     213,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   229,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   229,    -1,    -1,    -1,
      -1,   235,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   266,   274,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   274,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   293,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     293,    -1,    -1,   304,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   304,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   353,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     353,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   365,   366,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   392,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   392
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,    18,    19,    20,    21,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    35,    37,    38,    41,
      42,    46,    47,    48,    50,    51,    61,    64,    65,    70,
      73,    80,    81,    89,    90,    98,    99,   100,   101,   107,
     108,   111,   113,   116,   119,   120,   124,   126,   131,   132,
     133,   137,   139,   142,   149,   150,   154,   155,   156,   157,
     159,   161,   166,   168,   169,   173,   174,   185,   186,   187,
     188,   201,   203,   205,   218,   233,   238,   239,   240,   241,
     242,   243,   246,   247,   248,   249,   251,   252,   257,   259,
     260,   261,   262,   263,   264,   268,   269,   270,   275,   277,
     278,   281,   282,   289,   290,   291,   292,   294,   298,   299,
     300,   304,   306,   307,   308,   313,   314,   315,   316,   317,
     318,   319,   321,   322,   324,   325,   326,   327,   328,   329,
     330,   332,   333,   334,   354,   355,   356,   357,   358,   359,
     360,   361,   368,   369,   370,   371,   372,   384,   385,   390,
     394,   401,   402,   403,   404,   411,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   429,   430,   431,
     432,   444,   452,   455,   461,   469,   471,   323,   394,   399,
     167,   394,   394,   394,   394,   394,   394,   394,   394,   394,
     394,    69,   123,   405,   429,   430,   394,   246,   214,   332,
     360,   430,    75,   465,    52,    53,    54,   337,   339,   340,
     341,   342,   343,   344,   345,   346,   347,   348,   349,   350,
     351,   352,   353,   437,    18,    19,    20,    23,    24,    26,
      27,    28,    29,    30,    31,    32,    88,   115,   171,   172,
     175,   208,   213,   242,   369,   370,   371,   385,   390,   394,
     404,   411,   429,   430,   443,   463,   465,   466,   405,    19,
      20,    64,    98,   108,   148,   169,   212,   282,   289,   435,
     405,   430,    67,   122,   222,   271,   293,   462,   463,   464,
     465,   466,    19,    20,   373,   435,   405,   405,   394,   404,
     405,   429,    88,   115,   175,   404,   429,   443,   463,   465,
     466,   394,   125,   236,   300,   321,   428,    71,   121,   269,
     429,   443,    78,    82,    83,    84,    85,    86,    87,    91,
      92,    93,    94,    95,    96,   109,   112,   130,   143,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     235,   266,   365,   366,   448,    51,   115,   304,   443,   463,
     464,   465,   466,   405,   397,   408,    99,   405,   394,   394,
     394,   394,    61,   248,   429,   430,    64,    67,   122,   140,
     158,   164,   167,   175,   289,   312,   331,   356,   404,   443,
     462,   463,   465,   466,   394,    43,    88,   175,   404,   429,
     443,   463,   465,   466,   218,   248,   405,    75,   122,   394,
     430,   129,   394,    51,   103,   151,   153,   164,   165,   217,
     250,   253,   267,   273,   305,   404,   406,   407,   450,   301,
     172,   466,   405,   394,    56,    61,   207,   405,   437,    18,
     169,   304,   337,   394,   443,   463,   465,   466,   394,   394,
     430,   394,   405,   408,   404,   430,   102,   227,   445,   430,
     322,   407,    67,    88,   160,   175,   255,   404,   429,   443,
     462,   465,   466,   273,   405,   462,   465,   466,   430,   430,
     407,    67,    88,   255,   404,   443,   462,   465,   466,   273,
     405,   462,   465,   466,   437,   405,   407,   335,   336,   363,
     364,   404,    64,   108,   169,   289,   429,   430,   335,   336,
     363,   364,   404,   430,   158,   265,   394,   158,   265,   404,
      79,   399,   399,   399,   404,   248,   404,   411,   412,   404,
     412,   404,   412,     0,   393,    22,   374,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
     391,   392,   397,   408,    22,   374,   376,   377,   378,   379,
     380,   381,   382,   383,   384,   385,   386,   387,   388,   391,
     399,    47,    52,   118,   141,   162,   283,   304,   392,   432,
     443,    49,    55,    76,   105,   106,   110,   115,   141,   166,
     167,   169,   173,   229,   274,   293,   304,   353,   392,   443,
     463,   465,   466,    52,   162,   209,   295,   296,   304,   423,
     443,   394,   394,   394,   362,   443,   404,   405,   406,   409,
     404,   407,   404,   406,   412,   404,   412,   406,   406,   404,
     412,   406,   404,   404,   404,   404,   443,   392,   405,   392,
     301,   407,   443,   204,   332,   338,   360,   394,   405,   407,
     405,   394,   394,   394,   394,   394,   394,   394,   394,   394,
      75,   204,   222,   464,   465,   474,   407,   404,   313,   322,
     451,   404,   404,   404,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   391,   396,   408,
     301,   273,   278,   405,   407,   407,   405,   273,   397,   405,
     104,   122,   215,   218,   430,   464,   465,   474,   451,   396,
     406,   405,   404,   405,   407,   464,   465,   443,   407,   406,
     404,   405,   430,   404,   412,   430,   430,   264,   115,   443,
     463,   464,   465,   466,   273,   407,   443,   404,   407,   451,
     407,   407,   404,   404,   396,   430,   164,   167,   273,   304,
     407,   451,   396,   443,   301,   407,   407,   404,   407,   301,
     405,   430,   115,   404,   407,   117,   127,   163,   234,   439,
     440,   407,   404,   372,   407,    99,   405,    57,   152,   407,
     405,   405,   445,   405,   445,   405,   445,   404,   405,   410,
     404,    33,    58,   144,   146,   165,   231,   232,   253,   309,
     311,   441,   404,   430,   430,   430,   412,   329,   354,   454,
     407,   404,   273,   405,   407,   451,   407,   396,   404,   377,
     273,   405,   407,   396,   404,   304,   404,   404,   404,   404,
     396,   407,   407,   407,   407,   404,   404,   404,   404,   396,
     114,   124,   443,   177,   178,   206,   245,   442,   430,   443,
     442,   443,   404,   404,   404,   395,   395,    18,    19,   248,
     369,   370,   371,   403,   411,   429,   430,   404,   404,   412,
     404,   412,   404,   412,   404,   412,   404,   412,   404,   412,
     404,   412,   404,   412,   404,   412,   404,   412,   404,   412,
     404,   412,   404,   412,   404,   412,   167,   406,   399,   404,
     412,   404,   412,   404,   412,   404,   412,   404,   412,   404,
     412,   404,   412,   404,   412,   404,   412,   404,   412,   404,
     412,   404,   412,   404,   412,   404,   412,   404,   412,   404,
     412,   304,   140,   124,   236,   304,   337,   443,   443,   443,
     443,    52,    68,   117,   138,   179,   180,   181,   226,   230,
     276,   284,   285,   337,   438,   147,   269,   270,   314,   315,
     317,   318,   325,   326,   327,   328,   423,    41,    67,   124,
     209,   236,   237,   255,   304,   443,   462,   465,   304,   443,
     405,   407,   443,   167,   228,   254,   273,   304,   443,   463,
     464,   465,   466,   467,   258,   304,   321,   407,   464,   465,
     443,   405,   407,   304,   463,   464,   465,   466,   443,   445,
     404,   407,    66,    67,   115,   273,   274,   407,   463,   464,
     465,   466,   437,   404,   147,   167,   452,   427,   443,   463,
     465,   466,    67,   160,   164,   215,   228,   405,   426,   462,
     465,   466,   467,   470,   475,   404,    39,    41,    67,   124,
     125,   164,   209,   228,   236,   237,   272,   274,   286,   287,
     288,   304,   407,   425,   443,   462,   465,   466,   467,   473,
     475,    36,    62,    89,   151,   176,   184,   186,   188,   189,
     210,   211,   217,   228,   273,   279,   304,   407,   424,   443,
     447,   465,   466,   467,   472,   475,   367,   430,   430,   412,
     406,   396,   395,   395,   395,   395,   395,   396,   396,   396,
     396,   396,   396,   396,   396,   396,   404,   405,   430,   396,
     404,   407,   404,   406,   404,   406,   404,   404,   404,   404,
     404,   404,   404,   404,   404,   404,   404,   404,   404,   404,
     404,   404,   404,   404,   404,   430,   404,   404,   404,   398,
     407,   182,   222,   436,   404,    40,   443,   405,   273,   396,
     404,   395,   220,   398,   396,   396,   395,   396,   321,   443,
     464,   465,   404,   405,   404,   396,   404,   404,   404,   407,
     404,   429,   301,   301,   396,   395,   430,   396,   443,   388,
     396,   388,   372,   396,   404,   404,   395,   405,   405,   405,
     396,   396,   395,   396,   396,   395,    44,    97,   453,   396,
     404,   404,   430,   404,   404,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,   433,   404,   404,
     405,   405,   396,   408,   392,   375,   375,   404,   375,   375,
      46,   279,   404,   448,   404,   407,   404,   408,   405,   273,
     448,   404,   407,   405,   407,   407,   407,   396,   404,    62,
     206,   216,   468,    72,   443,   463,   466,   404,    62,    63,
     165,   253,   302,   476,   407,   407,    75,   222,   474,   407,
     405,   396,   443,   407,   462,   407,   464,   465,   404,   407,
     396,   273,    66,   165,   253,   446,   219,   224,   476,    46,
     279,   404,   396,   407,   405,   273,   404,   448,   405,   145,
     279,   310,    46,   279,   404,    59,   213,   407,   405,    34,
     202,   206,   449,   407,   407,   304,   404,   304,   404,    46,
     279,   396,   443,   407,   443,   134,   273,   404,   407,   443,
     463,   465,   466,   372,   372,   134,   273,   297,   404,   407,
     443,   463,   465,   466,   404,   404,   256,   404,   304,   407,
      46,   279,   396,   443,   396,   396,   395,   407,   406,   404,
     412,   404,   412,   404,   412,   406,   404,   404,   404,   404,
     407,   396,   396,   396,   396,   396,   396,   396,   396,   396,
     398,   405,   443,   407,   407,   396,   412,   412,   412,   474,
     430,   396,   396,   394,   405,   404,   407,   412,   407,   407,
     404,   404,   407,   248,   412,   404,   129,   396,   396,   124,
     430,   404,   406,   404,   412,   404,   412,   448,   396,   396,
     404,   396,   407,   167,   443,   404,   404,   404,   396,   404,
     404,   396,   297,   297,    46,   279,    46,   279,   405,   443,
     404,   396,   443,   404,   407,   396,   443,    62,   204,   297,
     434,   404,   407,    77,   244,   407,   456,   396,   395,   395,
     395,   395,   395,   395,   395,   396,   396,   396,   396,   396,
     396,   407,   404,   404,   406,   404,   404,   404,   404,   404,
     396,   395,   404,   396,   395,   396,   395,   404,   404,   407,
     396,   396,   395,   396,   396,   395,   396,   395,   396,   405,
     404,   404,   405,   396,   398,   404,   404,   404,   404,   404,
     404,   404,   404,   395,   395,   396,   407,   404,   412,   404,
     412,   404,   404,   404,   404,   396,   396,   396,   396,   396,
     396,   396,   407,   396,   443,    45,   170,   280,   460,   396,
     396,   405,   407,   407,   404,   443,   404,    35,   396,   396,
     407,    60,   135,   136,   204,   221,   303,   320,   459,   395,
     396,   395,   395,   395,   395,   396,   396,   396,   396,   407,
     404,   404,   404,   404,   404,   404,   404,   396,   396,   404,
     407,   395,   395,   396,   395,   396,   405,   404,   404,   396,
     396,   407,   404,   412,   404,   404,   404,   395,   396,   396,
     396,   443,   443,   396,   404,   404,   443,    17,   128,   223,
     457,   396,   395,   395,   396,   396,   396,   404,   404,   404,
     395,   395,   407,   296,   395,   396,   407,   404,   412,   404,
     404,   396,   396,   395,   404,    74,   183,   225,   458,   396,
     395,   395,   396,   395,   396,   404,   404,   396,   396,   395,
     404,   404,   412,   443,   396,   405,   404,   395,   395,   395,
     396,   404,   396,   443,   404,   396,   396,   443,   404,   395,
     396,   443,   395
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   400,   401,   401,   401,   402,   402,   403,   403,   403,
     403,   403,   403,   403,   403,   403,   403,   403,   403,   404,
     404,   404,   404,   404,   404,   404,   404,   404,   404,   404,
     404,   404,   404,   404,   404,   404,   404,   404,   404,   404,
     404,   404,   404,   404,   404,   404,   404,   404,   404,   404,
     404,   404,   404,   404,   404,   404,   404,   404,   404,   404,
     404,   404,   404,   404,   404,   404,   404,   404,   404,   404,
     404,   404,   404,   404,   404,   404,   404,   404,   404,   404,
     404,   404,   404,   404,   405,   405,   405,   406,   407,   408,
     409,   409,   410,   410,   411,   411,   411,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
     412,   412,   412,   412,   412,   413,   413,   413,   413,   413,
     414,   415,   415,   416,   416,   416,   416,   416,   416,   416,
     416,   416,   416,   416,   417,   417,   417,   417,   417,   417,
     417,   417,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   418,   418,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     420,   420,   420,   420,   420,   420,   421,   421,   422,   422,
     422,   422,   422,   422,   422,   422,   422,   422,   422,   422,
     422,   422,   422,   422,   422,   422,   422,   422,   422,   422,
     422,   422,   422,   422,   422,   422,   422,   422,   422,   422,
     422,   422,   422,   422,   422,   422,   422,   422,   422,   422,
     422,   422,   422,   422,   422,   422,   422,   423,   423,   423,
     423,   423,   423,   423,   423,   423,   423,   424,   424,   424,
     424,   424,   424,   424,   424,   424,   424,   424,   424,   424,
     424,   424,   424,   424,   424,   424,   424,   424,   424,   424,
     424,   424,   425,   425,   425,   425,   425,   425,   425,   425,
     425,   425,   425,   425,   425,   425,   425,   425,   425,   425,
     425,   425,   425,   425,   425,   425,   426,   426,   426,   426,
     426,   426,   426,   426,   426,   426,   426,   427,   427,   427,
     427,   428,   428,   428,   428,   429,   429,   430,   430,   430,
     430,   431,   431,   432,   432,   432,   432,   433,   433,   433,
     433,   433,   433,   433,   433,   433,   433,   433,   433,   434,
     434,   434,   435,   435,   436,   436,   437,   437,   437,   437,
     437,   437,   437,   437,   437,   437,   437,   437,   437,   437,
     437,   437,   437,   437,   437,   438,   438,   438,   438,   438,
     438,   439,   439,   440,   440,   441,   441,   441,   441,   441,
     441,   441,   441,   441,   441,   442,   442,   442,   442,   443,
     443,   444,   444,   444,   444,   444,   445,   445,   446,   446,
     446,   447,   447,   447,   448,   448,   448,   448,   448,   448,
     448,   448,   448,   448,   448,   448,   448,   448,   448,   448,
     448,   448,   448,   448,   448,   448,   448,   448,   448,   448,
     448,   448,   448,   448,   448,   448,   449,   449,   449,   450,
     450,   450,   450,   450,   450,   451,   451,   452,   452,   452,
     452,   452,   452,   452,   452,   453,   453,   454,   454,   455,
     455,   455,   455,   456,   456,   457,   457,   457,   458,   458,
     458,   459,   459,   459,   459,   459,   459,   459,   460,   460,
     460,   461,   461,   461,   461,   461,   461,   461,   462,   462,
     463,   464,   465,   465,   465,   466,   467,   468,   468,   468,
     469,   469,   469,   469,   469,   469,   469,   469,   469,   469,
     469,   469,   469,   469,   469,   469,   469,   469,   469,   469,
     469,   469,   469,   469,   469,   469,   469,   469,   469,   469,
     469,   469,   469,   469,   469,   469,   469,   469,   469,   469,
     469,   469,   469,   469,   469,   470,   470,   471,   471,   471,
     471,   471,   471,   471,   471,   471,   471,   471,   471,   472,
     472,   472,   472,   472,   472,   472,   472,   472,   472,   472,
     472,   473,   473,   473,   473,   473,   473,   473,   473,   473,
     473,   474,   474,   474,   475,   476,   476,   476,   476,   476
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     3,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     4,     6,     3,     3,     3,
       3,     1,     2,     1,     4,     4,     6,     6,     6,     8,
       8,    10,    12,    14,     3,     3,     3,     3,     3,     3,
       3,     3,     4,     8,    14,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     2,     2,
       3,     3,     3,     3,     5,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     1,     3,     3,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     3,     1,     6,     4,
       8,     4,     4,     4,     4,     4,     6,     6,     6,     6,
       8,     8,    10,    12,    14,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     5,     5,     5,     5,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     3,     2,     3,     3,     3,     3,     4,
       1,     3,     3,     2,     4,     5,     2,     2,     2,     2,
       3,     3,     6,     8,     2,     3,     2,     2,     2,     9,
       5,     4,     2,     5,     5,     7,     4,     5,     4,     4,
       3,     3,     3,     4,     2,     4,     4,     4,     3,     9,
       2,     2,     2,     2,     4,     3,     2,     3,     2,     2,
       8,     3,     2,     2,     2,     3,     3,     2,     2,     3,
       2,     2,     8,     3,     2,     2,     2,     3,     3,     2,
       2,     3,     2,     2,     8,     3,     2,     2,     2,     3,
       4,     4,     6,     2,     2,     3,     2,     2,     4,     3,
       2,     3,     2,     3,     4,     3,     2,     2,     4,     3,
       2,     4,     3,     2,     2,     2,     2,     4,     2,     4,
       3,     7,    13,     8,     2,     8,     2,     2,     3,     2,
       2,     2,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       4,     4,     3,     3,     3,     3,     4,     4,     2,     2,
       2,     3,     2,     2,     2,     2,     3,     3,     2,     3,
       3,     3,     4,     3,     5,     6,     6,     4,     2,     5,
       2,     2,     2,     2,     3,     1,     1,     2,     2,     2,
       1,     2,     1,     4,     1,     3,     3,     2,     2,     2,
       1,     2,     2,     3,     3,     3,     5,     5,     4,     4,
       4,     2,     3,     4,     4,     2,     3,     2,     2,     1,
      12,    18,    20,     6,     8,     6,     6,     6,    12,    10,
      10,     6,     4,    10,     6,     6,     8,     1,     2,     2,
       2,     1,     2,     2,     3,     3,     4,     3,     2,     3,
       4,     3,     4,     2,     4,     4,     6,     2,     2,     1,
       3,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       3,     3,     2,     2,     2,     2,     1,     1,     2,     3,
       3,     3,     3,     3,     3,     4,     4,     4,     4,     4,
       4,     4,     3,     3,     3,     3,     4,     4,     3,     3,
       3,     4,     3,     3,     4,     5,     3,     3,     4,     4,
       4,     6,     4,     4,     3,     3,     3,     3,     4,     3,
       3,     4,     4,     5,     6,     3,     3,     1,     3,     2,
       2,     2,     2,     2,     2,     2,     4,     1,     2,     3,
       3,     2,     2,     2,     1,     3,     3,     1,     2,     2,
       1,     2,     2,     2,     2,     3,     3,     1,     3,     2,
       4,     4,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     2,     2,     2,     3,     3,     3,     3,     3,
       1,     1,     3,     2,     2,     4,     1,     2,     2,     2,
       2,     4,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     3,     2,     1,     2,     3,     4,     1,
       2,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     8,     2,     2,     1,     1,     1,
       3,     4,     2,     3,     4,    17,     3,     3,     2,     2,
       2,     2,     3,     3,     3,     4,     4,     5,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     4,     4,
       4,     3,     4,     4,     2,     1,     1,     4,     3,     3,
       4,     4,     2,     2,     2,     4,     5,     4,     4,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       1,     1,     2,     2,     2,     2,     2,     3,     3,     1,
       2,     1,     1,     1,     2,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 624 "pars.yacc" /* yacc.c:1646  */
    {
            expr_parsed = TRUE;
            s_result = (yyvsp[0].dval);
        }
#line 5388 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 628 "pars.yacc" /* yacc.c:1646  */
    {
            vexpr_parsed = TRUE;
            v_result = (yyvsp[0].vrbl);
        }
#line 5397 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 641 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5403 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 642 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5409 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 643 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5415 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 644 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5421 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 645 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5427 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 646 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5433 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 647 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5439 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 648 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5445 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 649 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5451 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 650 "pars.yacc" /* yacc.c:1646  */
    {}
#line 5457 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 651 "pars.yacc" /* yacc.c:1646  */
    {
	    return 1;
	}
#line 5465 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 658 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[0].dval);
	}
#line 5473 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 661 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = *((yyvsp[0].dptr));
	}
#line 5481 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 664 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = nonl_parms[(yyvsp[0].ival)].value;
	}
#line 5489 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 667 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = nonl_parms[(yyvsp[0].ival)].max;
	}
#line 5497 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 670 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = nonl_parms[(yyvsp[0].ival)].min;
	}
#line 5505 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 673 "pars.yacc" /* yacc.c:1646  */
    {
            if ((yyvsp[0].ival) >= (yyvsp[-1].vrbl)->length) {
                errmsg("Access beyond array bounds");
                return 1;
            }
            (yyval.dval) = (yyvsp[-1].vrbl)->data[(yyvsp[0].ival)];
	}
#line 5517 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 680 "pars.yacc" /* yacc.c:1646  */
    {
	    double dummy, dummy2;
            int idummy, ind, length = (yyvsp[-1].vrbl)->length;
	    if ((yyvsp[-1].vrbl)->data == NULL) {
		yyerror("NULL variable, check set type");
		return 1;
	    }
	    switch ((yyvsp[-3].ival)) {
	    case MINP:
		(yyval.dval) = vmin((yyvsp[-1].vrbl)->data, length);
		break;
	    case MAXP:
		(yyval.dval) = vmax((yyvsp[-1].vrbl)->data, length);
		break;
            case AVG:
		stasum((yyvsp[-1].vrbl)->data, length, &(yyval.dval), &dummy);
                break;
            case SD:
		stasum((yyvsp[-1].vrbl)->data, length, &dummy, &(yyval.dval));
                break;
            case SUM:
		stasum((yyvsp[-1].vrbl)->data, length, &(yyval.dval), &dummy);
                (yyval.dval) *= length;
                break;
            case IMIN:
		minmax((yyvsp[-1].vrbl)->data, length, &dummy, &dummy2, &ind, &idummy);
                (yyval.dval) = (double) ind;
                break;
            case IMAX:
		minmax((yyvsp[-1].vrbl)->data, length, &dummy, &dummy2, &idummy, &ind);
                (yyval.dval) = (double) ind;
                break;
	    }
	}
#line 5556 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 714 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[-3].vrbl)->length != (yyvsp[-1].vrbl)->length) {
		yyerror("X and Y are of different length");
		return 1;
            } else {
                (yyval.dval) = trapint((yyvsp[-3].vrbl)->data, (yyvsp[-1].vrbl)->data, NULL, NULL, (yyvsp[-3].vrbl)->length);
            }
	}
#line 5569 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 722 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].vrbl)->length;
	}
#line 5577 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 725 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = getsetlength((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno);
	}
#line 5585 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 728 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].trgt)->setno;
	}
#line 5593 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 731 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].ival);
	}
#line 5601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 735 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.dval) = ((ParserFnc) (key[(yyvsp[0].ival)].data)) ();
	}
#line 5609 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 739 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-1].dval) * ((ParserFnc) (key[(yyvsp[0].ival)].data)) ();
	}
#line 5617 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 743 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = drand48();
	}
#line 5625 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 747 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-3].ival)].data)) ((yyvsp[-1].ival));
	}
#line 5633 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 751 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-3].ival)].data)) ((yyvsp[-1].dval));
	}
#line 5641 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 755 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-5].ival)].data)) ((yyvsp[-3].ival), (yyvsp[-1].dval));
	}
#line 5649 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 759 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-5].ival)].data)) ((yyvsp[-3].ival), (yyvsp[-1].ival));
	}
#line 5657 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 763 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-5].ival)].data)) ((yyvsp[-3].dval), (yyvsp[-1].dval));
	}
#line 5665 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 767 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-7].ival)].data)) ((yyvsp[-5].ival), (yyvsp[-3].ival), (yyvsp[-1].dval));
	}
#line 5673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 771 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-7].ival)].data)) ((yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].dval));
	}
#line 5681 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 775 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-9].ival)].data)) ((yyvsp[-7].dval), (yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].dval));
	}
#line 5689 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 779 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-11].ival)].data)) ((yyvsp[-9].dval), (yyvsp[-7].dval), (yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].dval));
	}
#line 5697 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 783 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((ParserFnc) (key[(yyvsp[-13].ival)].data)) ((yyvsp[-11].dval), (yyvsp[-9].dval), (yyvsp[-7].dval), (yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].dval));
	}
#line 5705 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 786 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].v.xv1;
	}
#line 5713 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 789 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].v.xv2;
	}
#line 5721 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 792 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].v.yv1;
	}
#line 5729 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 795 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].v.yv2;
	}
#line 5737 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 798 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].w.xg1;
	}
#line 5745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 801 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].w.xg2;
	}
#line 5753 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 804 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].w.yg1;
	}
#line 5761 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 807 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = g[(yyvsp[-2].ival)].w.yg2;
	}
#line 5769 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 810 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.dval) = (yyvsp[-1].dval);
	}
#line 5777 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 813 "pars.yacc" /* yacc.c:1646  */
    { /* yr, mo, day */
	    (yyval.dval) = cal_and_time_to_jul((yyvsp[-5].ival), (yyvsp[-3].ival), (yyvsp[-1].ival), 12, 0, 0.0);
	}
#line 5785 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 817 "pars.yacc" /* yacc.c:1646  */
    { /* yr, mo, day, hr, min, sec */
	    (yyval.dval) = cal_and_time_to_jul((yyvsp[-11].ival), (yyvsp[-9].ival), (yyvsp[-7].ival), (yyvsp[-5].ival), (yyvsp[-3].ival), (yyvsp[-1].dval));
	}
#line 5793 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 820 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            (yyval.dval) = g[whichgraph].v.xv1;
	}
#line 5805 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 827 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    (yyval.dval) = g[whichgraph].v.xv2;
	}
#line 5817 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 834 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    (yyval.dval) = g[whichgraph].v.yv1;
	}
#line 5829 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 841 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    (yyval.dval) = g[whichgraph].v.yv2;
	}
#line 5841 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 848 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    (yyval.dval) = g[whichgraph].w.xg1;
	}
#line 5853 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 855 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    (yyval.dval) = g[whichgraph].w.xg2;
	}
#line 5865 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 862 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    (yyval.dval) = g[whichgraph].w.yg1;
	}
#line 5877 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 869 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    (yyval.dval) = g[whichgraph].w.yg2;
	}
#line 5889 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 876 "pars.yacc" /* yacc.c:1646  */
    {
	    double vx, vy;
            get_page_viewport(&vx, &vy);
            (yyval.dval) = vx;
	}
#line 5899 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 881 "pars.yacc" /* yacc.c:1646  */
    {
	    double vx, vy;
            get_page_viewport(&vx, &vy);
            (yyval.dval) = vy;
	}
#line 5909 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 886 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-1].dval);
	}
#line 5917 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 889 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].dval) + (yyvsp[0].dval);
	}
#line 5925 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 892 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].dval) - (yyvsp[0].dval);
	}
#line 5933 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 895 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = -(yyvsp[0].dval);
	}
#line 5941 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 898 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[0].dval);
	}
#line 5949 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 901 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].dval) * (yyvsp[0].dval);
	}
#line 5957 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 905 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[0].dval) != 0.0) {
		(yyval.dval) = (yyvsp[-2].dval) / (yyvsp[0].dval);
	    } else {
		yyerror("Divide by zero");
		return 1;
	    }
	}
#line 5970 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 913 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[0].dval) != 0.0) {
		(yyval.dval) = fmod((yyvsp[-2].dval), (yyvsp[0].dval));
	    } else {
		yyerror("Divide by zero");
		return 1;
	    }
	}
#line 5983 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 921 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[-2].dval) < 0 && rint((yyvsp[0].dval)) != (yyvsp[0].dval)) {
		yyerror("Negative value raised to non-integer power");
		return 1;
            } else if ((yyvsp[-2].dval) == 0.0 && (yyvsp[0].dval) <= 0.0) {
		yyerror("Zero raised to non-positive power");
		return 1;
            } else {
                (yyval.dval) = pow((yyvsp[-2].dval), (yyvsp[0].dval));
            }
	}
#line 5999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 932 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-4].dval) ? (yyvsp[-2].dval) : (yyvsp[0].dval);
	}
#line 6007 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 935 "pars.yacc" /* yacc.c:1646  */
    {
	   (yyval.dval) = ((yyvsp[-2].dval) > (yyvsp[0].dval));
	}
#line 6015 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 938 "pars.yacc" /* yacc.c:1646  */
    {
	   (yyval.dval) = ((yyvsp[-2].dval) < (yyvsp[0].dval));
	}
#line 6023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 941 "pars.yacc" /* yacc.c:1646  */
    {
	   (yyval.dval) = ((yyvsp[-2].dval) <= (yyvsp[0].dval));
	}
#line 6031 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 944 "pars.yacc" /* yacc.c:1646  */
    {
	   (yyval.dval) = ((yyvsp[-2].dval) >= (yyvsp[0].dval));
	}
#line 6039 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 947 "pars.yacc" /* yacc.c:1646  */
    {
	   (yyval.dval) = ((yyvsp[-2].dval) == (yyvsp[0].dval));
	}
#line 6047 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 950 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = ((yyvsp[-2].dval) != (yyvsp[0].dval));
	}
#line 6055 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 953 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].dval) && (yyvsp[0].dval);
	}
#line 6063 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 956 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = (yyvsp[-2].dval) || (yyvsp[0].dval);
	}
#line 6071 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 959 "pars.yacc" /* yacc.c:1646  */
    {
	    (yyval.dval) = !((yyvsp[0].dval));
	}
#line 6079 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 964 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.sval) = (yyvsp[0].sval);
	}
#line 6087 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 967 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.sval) = concat_strings((yyvsp[-2].sval), (yyvsp[0].sval));
            xfree((yyvsp[0].sval));
        }
#line 6096 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 971 "pars.yacc" /* yacc.c:1646  */
    {
            char buf[32];
            set_locale_num(TRUE);
            sprintf(buf, "%g", (yyvsp[0].dval));
            set_locale_num(FALSE);
            (yyval.sval) = concat_strings((yyvsp[-2].sval), buf);
        }
#line 6108 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 980 "pars.yacc" /* yacc.c:1646  */
    {
	    int itmp = rint((yyvsp[0].dval));
            if (fabs(itmp - (yyvsp[0].dval)) > 1.e-6) {
		yyerror("Non-integer value supplied for integer");
		return 1;
            }
            (yyval.ival) = itmp;
        }
#line 6121 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 990 "pars.yacc" /* yacc.c:1646  */
    {
            if ((yyvsp[0].ival) < 0) {
		yyerror("Negative value supplied for non-negative");
		return 1;
            }
            (yyval.ival) = (yyvsp[0].ival);
	}
#line 6133 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 999 "pars.yacc" /* yacc.c:1646  */
    {
	    int itmp = (yyvsp[-1].ival) - index_shift;
            if (itmp < 0) {
		yyerror("Negative index");
		return 1;
            }
            (yyval.ival) = itmp;
	}
#line 6146 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1009 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.dval) = (yyvsp[0].dval);
        }
#line 6154 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1012 "pars.yacc" /* yacc.c:1646  */
    {
            double jul;
            Dates_format dummy;
            if (parse_date((yyvsp[0].sval), get_date_hint(), FALSE, &jul, &dummy)
                == RETURN_SUCCESS) {
                xfree((yyvsp[0].sval));
                (yyval.dval) = jul;
            } else {
                xfree((yyvsp[0].sval));
		yyerror("Invalid date");
		return 1;
            }
        }
#line 6172 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1027 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.dval) = (yyvsp[0].dval);
        }
#line 6180 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1030 "pars.yacc" /* yacc.c:1646  */
    {
            double jul;
            Dates_format dummy;
            if (parse_date((yyvsp[0].sval), get_date_hint(), TRUE, &jul, &dummy)
                == RETURN_SUCCESS) {
                xfree((yyvsp[0].sval));
                (yyval.dval) = jul;
            } else {
                xfree((yyvsp[0].sval));
		yyerror("Invalid date");
		return 1;
            }
        }
#line 6198 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1047 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.vrbl) = (yyvsp[0].vrbl);
	}
#line 6206 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1051 "pars.yacc" /* yacc.c:1646  */
    {
	    double *ptr = getcol(vasgn_gno, vasgn_setno, (yyvsp[0].ival));
            (yyval.vrbl) = &freelist[fcnt++];
            (yyval.vrbl)->type = GRARR_SET;
            (yyval.vrbl)->data = ptr;
            if (ptr == NULL) {
                errmsg("NULL variable - check set type");
                return 1;
            } else {
                (yyval.vrbl)->length = getsetlength(vasgn_gno, vasgn_setno);
            }
	}
#line 6223 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1064 "pars.yacc" /* yacc.c:1646  */
    {
	    double *ptr = getcol((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].ival));
            (yyval.vrbl) = &freelist[fcnt++];
            (yyval.vrbl)->type = GRARR_SET;
            (yyval.vrbl)->data = ptr;
            if (ptr == NULL) {
                errmsg("NULL variable - check set type");
                return 1;
            } else {
                (yyval.vrbl)->length = getsetlength((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno);
            }
	}
#line 6240 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1080 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.vrbl) = (yyvsp[0].vrbl);
	}
#line 6248 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1084 "pars.yacc" /* yacc.c:1646  */
    {
            int start = (yyvsp[-3].ival) - index_shift, stop = (yyvsp[-1].ival) - index_shift;
            if (start < 0 || stop < start || stop >= (yyvsp[-5].vrbl)->length) {
		yyerror("Invalid index range");
            } else {
                int len = stop - start + 1;
	        double *ptr = xmalloc(len*SIZEOF_DOUBLE);
                if ((yyval.vrbl)->data == NULL) {
                    yyerror("Not enough memory");
                } else {
                    int i;
                    (yyval.vrbl) = &freelist[fcnt++];
	            (yyval.vrbl)->data = ptr;
                    (yyval.vrbl)->length = len;
                    (yyval.vrbl)->type = GRARR_TMP;
                    for (i = 0; i < len; i++) {
                        (yyval.vrbl)->data[i] = (yyvsp[-5].vrbl)->data[i + (yyvsp[-3].ival)];
                    }
                }
            }
	}
#line 6274 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 1106 "pars.yacc" /* yacc.c:1646  */
    {
            int len = (yyvsp[-1].ival);
            if (len < 1) {
                yyerror("npoints must be > 0");
            } else {
                double *ptr = allocate_index_data(len);
                if (ptr == NULL) {
                    errmsg("Malloc failed");
                    return 1;
                } else {
                    (yyval.vrbl) = &freelist[fcnt++];
                    (yyval.vrbl)->type = GRARR_TMP;
                    (yyval.vrbl)->data = ptr;
                    (yyval.vrbl)->length = len;
                }
            }
	}
#line 6296 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 1124 "pars.yacc" /* yacc.c:1646  */
    {
            int len = (yyvsp[-1].ival);
            if (len < 2) {
                yyerror("npoints must be > 1");
            } else {
                double *ptr = allocate_mesh((yyvsp[-5].dval), (yyvsp[-3].dval), len);
                if (ptr == NULL) {
                    errmsg("Malloc failed");
                    return 1;
                } else {
                    (yyval.vrbl) = &freelist[fcnt++];
                    (yyval.vrbl)->type = GRARR_TMP;
                    (yyval.vrbl)->data = ptr;
                    (yyval.vrbl)->length = len;
                }
            }
	}
#line 6318 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 1142 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    (yyval.vrbl)->data = xmalloc((yyvsp[-1].ival)*SIZEOF_DOUBLE);
            if ((yyval.vrbl)->data == NULL) {
                errmsg("Not enough memory");
                return 1;
            } else {
                (yyval.vrbl)->length = (yyvsp[-1].ival);
                (yyval.vrbl)->type = GRARR_TMP;
            }
            for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = drand48();
	    }
	}
#line 6338 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 1158 "pars.yacc" /* yacc.c:1646  */
    {
	    int rtype, i, len;
            char *rarray;
            
            rtype = RESTRICT_REG0 + (yyvsp[-3].ival);
            
	    if (get_restriction_array((yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno,
                rtype, FALSE, &rarray) != RETURN_SUCCESS) {
                errmsg("Error in region evaluation");
                return 1;
	    }

            len = getsetlength((yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno);
            (yyval.vrbl) = &freelist[fcnt++];
	    (yyval.vrbl)->data = xmalloc(len*SIZEOF_DOUBLE);
            if ((yyval.vrbl)->data == NULL) {
                errmsg("Not enough memory");
                return 1;
            } else {
                (yyval.vrbl)->length = len;
                (yyval.vrbl)->type = GRARR_TMP;
            }
            for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = rarray[i];
	    }
            
            xfree(rarray);
	}
#line 6371 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 1187 "pars.yacc" /* yacc.c:1646  */
    {
            int i;
            (yyval.vrbl) = &freelist[fcnt++];
            copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 1; i < (yyval.vrbl)->length; i++) {
                (yyval.vrbl)->data[i] += (yyval.vrbl)->data[i - 1];
            }
	}
#line 6385 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 1197 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-3].ival)].data)) ((int) ((yyvsp[-1].vrbl)->data[i]));
	    }
	}
#line 6399 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 1207 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-3].ival)].data)) (((yyvsp[-1].vrbl)->data[i]));
	    }
	}
#line 6413 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 1217 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-3].vrbl)->length != (yyvsp[-1].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-3].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            
	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-5].ival)].data)) ((yyvsp[-3].vrbl)->data[i], (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6432 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1232 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            
	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-5].ival)].data)) ((yyvsp[-3].dval), (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6447 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 1243 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-3].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            
	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-5].ival)].data)) ((yyvsp[-3].vrbl)->data[i], (yyvsp[-1].dval));
	    }
	}
#line 6462 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 1254 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-5].ival)].data)) ((yyvsp[-3].ival), (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6477 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 1265 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-7].ival)].data)) ((yyvsp[-5].ival), (yyvsp[-3].ival), (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6492 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 1276 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-7].ival)].data)) ((yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6507 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 1287 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-9].ival)].data)) ((yyvsp[-7].dval), (yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6522 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 1298 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-11].ival)].data)) ((yyvsp[-9].dval), (yyvsp[-7].dval), (yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6537 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 1309 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((ParserFnc) (key[(yyvsp[-13].ival)].data)) ((yyvsp[-11].dval), (yyvsp[-9].dval), (yyvsp[-7].dval), (yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].vrbl)->data[i]);
	    }
	}
#line 6552 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 1320 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] + (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6571 "y.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 1335 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] + (yyvsp[0].dval);
	    }
	}
#line 6586 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 1346 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].dval) + (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 1357 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] - (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6620 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 1372 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] - (yyvsp[0].dval);
	    }
	}
#line 6635 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 1383 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].dval) - (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6650 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 1394 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] * (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6669 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 1409 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] * (yyvsp[0].dval);
	    }
	}
#line 6684 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 1420 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].dval) * (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6699 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 1431 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		if ((yyvsp[0].vrbl)->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                }
                (yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] / (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6722 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 1450 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[0].dval) == 0.0) {
                errmsg("Divide by zero");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] / (yyvsp[0].dval);
	    }
	}
#line 6741 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 1465 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		if ((yyvsp[0].vrbl)->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                }
		(yyval.vrbl)->data[i] = (yyvsp[-2].dval) / (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6760 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 1480 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		if ((yyvsp[0].vrbl)->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                } else {
                    (yyval.vrbl)->data[i] = fmod((yyvsp[-2].vrbl)->data[i], (yyvsp[0].vrbl)->data[i]);
                }
	    }
	}
#line 6784 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 1500 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[0].dval) == 0.0) {
                errmsg("Divide by zero");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = fmod((yyvsp[-2].vrbl)->data[i], (yyvsp[0].dval));
	    }
	}
#line 6803 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 1515 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		if ((yyvsp[0].vrbl)->data[i] == 0.0) {
                    errmsg("Divide by zero");
                    return 1;
                } else {
		    (yyval.vrbl)->data[i] = fmod((yyvsp[-2].dval), (yyvsp[0].vrbl)->data[i]);
                }
	    }
	}
#line 6823 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 1531 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
	        if ((yyvsp[-2].vrbl)->data[i] < 0 && rint((yyvsp[0].vrbl)->data[i]) != (yyvsp[0].vrbl)->data[i]) {
	            yyerror("Negative value raised to non-integer power");
	            return 1;
                } else if ((yyvsp[-2].vrbl)->data[i] == 0.0 && (yyvsp[0].vrbl)->data[i] <= 0.0) {
	            yyerror("Zero raised to non-positive power");
	            return 1;
                } else {
                    (yyval.vrbl)->data[i] = pow((yyvsp[-2].vrbl)->data[i], (yyvsp[0].vrbl)->data[i]);
                }
	    }
	}
#line 6850 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 1554 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
	        if ((yyvsp[-2].vrbl)->data[i] < 0 && rint((yyvsp[0].dval)) != (yyvsp[0].dval)) {
	            yyerror("Negative value raised to non-integer power");
	            return 1;
                } else if ((yyvsp[-2].vrbl)->data[i] == 0.0 && (yyvsp[0].dval) <= 0.0) {
	            yyerror("Zero raised to non-positive power");
	            return 1;
                } else {
                    (yyval.vrbl)->data[i] = pow((yyvsp[-2].vrbl)->data[i], (yyvsp[0].dval));
                }
	    }
	}
#line 6873 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 1573 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
	        if ((yyvsp[-2].dval) < 0 && rint((yyvsp[0].vrbl)->data[i]) != (yyvsp[0].vrbl)->data[i]) {
	            yyerror("Negative value raised to non-integer power");
	            return 1;
                } else if ((yyvsp[-2].dval) == 0.0 && (yyvsp[0].vrbl)->data[i] <= 0.0) {
	            yyerror("Zero raised to non-positive power");
	            return 1;
                } else {
                    (yyval.vrbl)->data[i] = pow((yyvsp[-2].dval), (yyvsp[0].vrbl)->data[i]);
                }
	    }
	}
#line 6896 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 1592 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-1].vrbl)->data[i] * ((ParserFnc) (key[(yyvsp[0].ival)].data)) ();
	    }
	}
#line 6910 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 1601 "pars.yacc" /* yacc.c:1646  */
    {
            int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-4].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 0; i < (yyval.vrbl)->length; i++) { 
                (yyval.vrbl)->data[i] = CAST_DBL_TO_BOOL((yyvsp[-4].vrbl)->data[i]) ? (yyvsp[-2].dval) : (yyvsp[0].dval);
            }
	}
#line 6924 "y.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 1610 "pars.yacc" /* yacc.c:1646  */
    {
            int i;
	    if ((yyvsp[-4].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-4].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 0; i < (yyval.vrbl)->length; i++) { 
                (yyval.vrbl)->data[i] = CAST_DBL_TO_BOOL((yyvsp[-4].vrbl)->data[i]) ? (yyvsp[-2].dval) : (yyvsp[0].vrbl)->data[i];
            }
	}
#line 6942 "y.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 1623 "pars.yacc" /* yacc.c:1646  */
    {
            int i;
	    if ((yyvsp[-4].vrbl)->length != (yyvsp[-2].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-4].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 0; i < (yyval.vrbl)->length; i++) { 
                (yyval.vrbl)->data[i] = CAST_DBL_TO_BOOL((yyvsp[-4].vrbl)->data[i]) ? (yyvsp[-2].vrbl)->data[i] : (yyvsp[0].dval);
            }
	}
#line 6960 "y.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 1636 "pars.yacc" /* yacc.c:1646  */
    {
            int i;
	    if ((yyvsp[-4].vrbl)->length != (yyvsp[0].vrbl)->length || (yyvsp[-4].vrbl)->length != (yyvsp[-2].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-4].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 0; i < (yyval.vrbl)->length; i++) { 
                (yyval.vrbl)->data[i] = CAST_DBL_TO_BOOL((yyvsp[-4].vrbl)->data[i]) ? (yyvsp[-2].vrbl)->data[i] : (yyvsp[0].vrbl)->data[i];
            }
	}
#line 6978 "y.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 1650 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] || (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 6997 "y.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 1665 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] || (yyvsp[0].dval);
	    }
	}
#line 7012 "y.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 1676 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].dval) || (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 7027 "y.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 1687 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] && (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 7046 "y.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 1702 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].vrbl)->data[i] && (yyvsp[0].dval);
	    }
	}
#line 7061 "y.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 1713 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = (yyvsp[-2].dval) && (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 7076 "y.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 1724 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] > (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7095 "y.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 1739 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] > (yyvsp[0].dval));
	    }
	}
#line 7110 "y.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 1750 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].dval) > (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7125 "y.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 1761 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] < (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7144 "y.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 1776 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] < (yyvsp[0].dval));
	    }
	}
#line 7159 "y.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 1787 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].dval) < (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7174 "y.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 1798 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] >= (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7193 "y.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 1813 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] >= (yyvsp[0].dval));
	    }
	}
#line 7208 "y.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 1824 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].dval) >= (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7223 "y.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 1835 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] <= (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7242 "y.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 1850 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] <= (yyvsp[0].dval));
	    }
	}
#line 7257 "y.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 1861 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].dval) <= (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7272 "y.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 1872 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] == (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7291 "y.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 1887 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] == (yyvsp[0].dval));
	    }
	}
#line 7306 "y.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 1898 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].dval) == (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7321 "y.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 1909 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Can't operate on vectors of different lengths");
                return 1;
            }
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] != (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7340 "y.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 1924 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-2].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].vrbl)->data[i] != (yyvsp[0].dval));
	    }
	}
#line 7355 "y.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 1935 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;

	    for (i = 0; i < (yyval.vrbl)->length; i++) {
		(yyval.vrbl)->data[i] = ((yyvsp[-2].dval) != (yyvsp[0].vrbl)->data[i]);
	    }
	}
#line 7370 "y.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 1946 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 0; i < (yyval.vrbl)->length; i++) { 
                (yyval.vrbl)->data[i] = !(yyvsp[0].vrbl)->data[i];
            }
	}
#line 7384 "y.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 1956 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[-1].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 0; i < (yyval.vrbl)->length; i++) { 
                (yyval.vrbl)->data[i] = (yyvsp[-1].vrbl)->data[i];
            }
	}
#line 7398 "y.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 1965 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
            (yyval.vrbl) = &freelist[fcnt++];
	    copy_vrbl((yyval.vrbl), (yyvsp[0].vrbl));
            (yyval.vrbl)->type = GRARR_TMP;
            for (i = 0; i < (yyval.vrbl)->length; i++) { 
                (yyval.vrbl)->data[i] = - (yyvsp[0].vrbl)->data[i];
            }
	}
#line 7412 "y.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 1979 "pars.yacc" /* yacc.c:1646  */
    {
	    *((yyvsp[-2].dptr)) = (yyvsp[0].dval);
	}
#line 7420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 1983 "pars.yacc" /* yacc.c:1646  */
    {
	    nonl_parms[(yyvsp[-2].ival)].value = (yyvsp[0].dval);
	}
#line 7428 "y.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 1987 "pars.yacc" /* yacc.c:1646  */
    {
	    nonl_parms[(yyvsp[-2].ival)].max = (yyvsp[0].dval);
	}
#line 7436 "y.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 1991 "pars.yacc" /* yacc.c:1646  */
    {
	    nonl_parms[(yyvsp[-2].ival)].min = (yyvsp[0].dval);
	}
#line 7444 "y.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 1995 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[-2].ival) >= (yyvsp[-3].vrbl)->length) {
		yyerror("Access beyond array bounds");
		return 1;
            }
            (yyvsp[-3].vrbl)->data[(yyvsp[-2].ival)] = (yyvsp[0].dval);
	}
#line 7456 "y.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 2006 "pars.yacc" /* yacc.c:1646  */
    {
            target tgt;
            switch ((yyvsp[0].vrbl)->type) {
            case GRARR_SET:
                if (find_set_bydata((yyvsp[0].vrbl)->data, &tgt) == RETURN_SUCCESS) {
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
            (yyval.vrbl) = (yyvsp[0].vrbl);
        }
#line 7484 "y.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 2033 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    if ((yyvsp[-2].vrbl)->length != (yyvsp[0].vrbl)->length) {
                errmsg("Left and right vectors are of different lengths");
                return 1;
            }
	    for (i = 0; i < (yyvsp[-2].vrbl)->length; i++) {
	        (yyvsp[-2].vrbl)->data[i] = (yyvsp[0].vrbl)->data[i];
	    }
	}
#line 7499 "y.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 2044 "pars.yacc" /* yacc.c:1646  */
    {
	    int i;
	    for (i = 0; i < (yyvsp[-2].vrbl)->length; i++) {
	        (yyvsp[-2].vrbl)->data[i] = (yyvsp[0].dval);
	    }
	}
#line 7510 "y.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 2054 "pars.yacc" /* yacc.c:1646  */
    {
	    symtab_entry tmpkey;
            double *var;
            
            var = xmalloc(SIZEOF_DOUBLE);
            *var = 0.0;
            
	    tmpkey.s = (yyvsp[0].sval);
	    tmpkey.type = KEY_VAR;
	    tmpkey.data = (void *) var;
	    if (addto_symtab(tmpkey) != RETURN_SUCCESS) {
	        yyerror("Adding new symbol failed");
	    }

            xfree((yyvsp[0].sval));
        }
#line 7531 "y.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 2071 "pars.yacc" /* yacc.c:1646  */
    {
	    if (define_parser_arr((yyvsp[-2].sval)) == NULL) {
	        yyerror("Adding new symbol failed");
	    }

            xfree((yyvsp[-2].sval));
        }
#line 7543 "y.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 2079 "pars.yacc" /* yacc.c:1646  */
    {
	    grarr *var;
            if ((var = define_parser_arr((yyvsp[-3].sval))) == NULL) {
	        yyerror("Adding new symbol failed");
	    } else {
                realloc_vrbl(var, (yyvsp[-1].ival));
            }

            xfree((yyvsp[-3].sval));
        }
#line 7558 "y.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 2090 "pars.yacc" /* yacc.c:1646  */
    {
            yyerror("Keyword already exists");
        }
#line 7566 "y.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 2094 "pars.yacc" /* yacc.c:1646  */
    {
            yyerror("Keyword already exists");
        }
#line 7574 "y.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 2098 "pars.yacc" /* yacc.c:1646  */
    {
            undefine_parser_var((void *) (yyvsp[0].dptr));
            xfree((yyvsp[0].dptr));
        }
#line 7583 "y.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 2103 "pars.yacc" /* yacc.c:1646  */
    {
            realloc_vrbl((yyvsp[0].vrbl), 0);
            undefine_parser_var((void *) (yyvsp[0].vrbl));
            xfree((yyvsp[0].vrbl));
        }
#line 7593 "y.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 2108 "pars.yacc" /* yacc.c:1646  */
    {
	    int position;

	    lowtoupper((yyvsp[0].sval));
	    if ((position = findf(key, (yyvsp[0].sval))) >= 0) {
	        symtab_entry tmpkey;
		tmpkey.s = (yyvsp[-1].sval);
		tmpkey.type = key[position].type;
		tmpkey.data = key[position].data;
		if (addto_symtab(tmpkey) != RETURN_SUCCESS) {
		    yyerror("Keyword already exists");
		}
	    } else {
	        yyerror("Aliased keyword not found");
	    }
	    xfree((yyvsp[-1].sval));
	    xfree((yyvsp[0].sval));
	}
#line 7616 "y.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 2126 "pars.yacc" /* yacc.c:1646  */
    {
	    alias_force = (yyvsp[0].ival);
	}
#line 7624 "y.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 2129 "pars.yacc" /* yacc.c:1646  */
    {
	    if (load_module((yyvsp[0].sval), (yyvsp[-4].sval), (yyvsp[-4].sval), (yyvsp[-2].ival)) != 0) {
	        yyerror("DL module load failed");
	    }
	    xfree((yyvsp[-4].sval));
	    xfree((yyvsp[0].sval));
	}
#line 7636 "y.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 2136 "pars.yacc" /* yacc.c:1646  */
    {
	    if (load_module((yyvsp[-2].sval), (yyvsp[-6].sval), (yyvsp[0].sval), (yyvsp[-4].ival)) != 0) {
	        yyerror("DL module load failed");
	    }
	    xfree((yyvsp[-6].sval));
	    xfree((yyvsp[-2].sval));
	    xfree((yyvsp[0].sval));
	}
#line 7649 "y.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 2147 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-1].ival)].active = (yyvsp[0].ival);
	}
#line 7657 "y.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 2150 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-2].ival)].type = (yyvsp[0].ival);
	}
#line 7665 "y.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 2153 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-1].ival)].color = (yyvsp[0].ival);
	}
#line 7673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 2156 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-1].ival)].lines = (yyvsp[0].ival);
	}
#line 7681 "y.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 2159 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-1].ival)].linew = (yyvsp[0].dval);
	}
#line 7689 "y.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 2163 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-8].ival)].x1 = (yyvsp[-6].dval);
	    rg[(yyvsp[-8].ival)].y1 = (yyvsp[-4].dval);
	    rg[(yyvsp[-8].ival)].x2 = (yyvsp[-2].dval);
	    rg[(yyvsp[-8].ival)].y2 = (yyvsp[0].dval);
	}
#line 7700 "y.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 2170 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-4].ival)].x = xrealloc(rg[(yyvsp[-4].ival)].x, (rg[(yyvsp[-4].ival)].n + 1) * SIZEOF_DOUBLE);
	    rg[(yyvsp[-4].ival)].y = xrealloc(rg[(yyvsp[-4].ival)].y, (rg[(yyvsp[-4].ival)].n + 1) * SIZEOF_DOUBLE);
	    rg[(yyvsp[-4].ival)].x[rg[(yyvsp[-4].ival)].n] = (yyvsp[-2].dval);
	    rg[(yyvsp[-4].ival)].y[rg[(yyvsp[-4].ival)].n] = (yyvsp[0].dval);
	    rg[(yyvsp[-4].ival)].n++;
	}
#line 7712 "y.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 2177 "pars.yacc" /* yacc.c:1646  */
    {
	    rg[(yyvsp[-2].ival)].linkto = (yyvsp[0].ival);
	}
#line 7720 "y.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 2184 "pars.yacc" /* yacc.c:1646  */
    {
            if (set_project_version((yyvsp[0].ival)) != RETURN_SUCCESS) {
                errmsg("Project version is newer than software!");
            }
            if (get_project_version() < 50001) {
                map_fonts(FONT_MAP_ACEGR);
            } else {
                map_fonts(FONT_MAP_DEFAULT);
            }
        }
#line 7735 "y.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 2194 "pars.yacc" /* yacc.c:1646  */
    {
            set_page_dimensions((yyvsp[-2].ival), (yyvsp[0].ival), TRUE);
        }
#line 7743 "y.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 2197 "pars.yacc" /* yacc.c:1646  */
    {
            set_page_dimensions((yyvsp[-2].ival), (yyvsp[0].ival), FALSE);
        }
#line 7751 "y.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 2200 "pars.yacc" /* yacc.c:1646  */
    {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name((yyvsp[-5].sval));
            xfree((yyvsp[-5].sval));
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.pg.width =  (long) ((yyvsp[-2].ival)*dev.pg.dpi/72);
                dev.pg.height = (long) ((yyvsp[0].ival)*dev.pg.dpi/72);
                set_device_props(device_id, dev);
            }
        }
#line 7771 "y.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 2215 "pars.yacc" /* yacc.c:1646  */
    {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name((yyvsp[-2].sval));
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.pg.dpi = (yyvsp[0].dval);
                set_device_props(device_id, dev);
            }
            xfree((yyvsp[-2].sval));
        }
#line 7790 "y.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 2229 "pars.yacc" /* yacc.c:1646  */
    {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name((yyvsp[-3].sval));
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.fontaa = (yyvsp[0].ival);
                set_device_props(device_id, dev);
            }
            xfree((yyvsp[-3].sval));
        }
#line 7809 "y.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 2243 "pars.yacc" /* yacc.c:1646  */
    {
            int device_id;
            Device_entry dev;
            
            device_id = get_device_by_name((yyvsp[-2].sval));
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                dev = get_device_props(device_id);
                dev.devfonts = (yyvsp[0].ival);
                set_device_props(device_id, dev);
            }
            xfree((yyvsp[-2].sval));
        }
#line 7828 "y.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 2257 "pars.yacc" /* yacc.c:1646  */
    {
            int device_id;
            
            device_id = get_device_by_name((yyvsp[-2].sval));
            if (device_id < 0) {
                yyerror("Unknown device");
            } else {
                if (parse_device_options(device_id, (yyvsp[0].sval)) != 
                                                        RETURN_SUCCESS) {
                    yyerror("Incorrect device option string");
                }
            }
            xfree((yyvsp[-2].sval));
            xfree((yyvsp[0].sval));
        }
#line 7848 "y.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 2272 "pars.yacc" /* yacc.c:1646  */
    {
            set_printer_by_name((yyvsp[0].sval));
            xfree((yyvsp[0].sval));
        }
#line 7857 "y.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 2276 "pars.yacc" /* yacc.c:1646  */
    {
            set_ref_date((yyvsp[0].dval));
	}
#line 7865 "y.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 2279 "pars.yacc" /* yacc.c:1646  */
    {
            allow_two_digits_years((yyvsp[0].ival));
	}
#line 7873 "y.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 2282 "pars.yacc" /* yacc.c:1646  */
    {
            set_wrap_year((yyvsp[0].ival));
	}
#line 7881 "y.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 2285 "pars.yacc" /* yacc.c:1646  */
    {
	    setbgcolor((yyvsp[0].ival));
	}
#line 7889 "y.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 2288 "pars.yacc" /* yacc.c:1646  */
    {
	    setbgfill((yyvsp[0].ival));
	}
#line 7897 "y.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 2291 "pars.yacc" /* yacc.c:1646  */
    {
	    scroll_proc((int) (yyvsp[-1].dval));
	}
#line 7905 "y.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 2294 "pars.yacc" /* yacc.c:1646  */
    {
	    scrollinout_proc((int) (yyvsp[-1].dval));
	}
#line 7913 "y.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 2297 "pars.yacc" /* yacc.c:1646  */
    {
	    scrolling_islinked = (yyvsp[0].ival);
	}
#line 7921 "y.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 2302 "pars.yacc" /* yacc.c:1646  */
    {
	    add_world(whichgraph, (yyvsp[-6].dval), (yyvsp[-4].dval), (yyvsp[-2].dval), (yyvsp[0].dval));
	}
#line 7929 "y.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 2306 "pars.yacc" /* yacc.c:1646  */
    {
            timer_delay = (yyvsp[0].ival);
	}
#line 7937 "y.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 2310 "pars.yacc" /* yacc.c:1646  */
    {
	    target_set = *((yyvsp[0].trgt));
	    set_parser_setno(target_set.gno, target_set.setno);
	}
#line 7946 "y.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 2314 "pars.yacc" /* yacc.c:1646  */
    {
	    set_parser_gno((yyvsp[0].ival));
	}
#line 7954 "y.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 2317 "pars.yacc" /* yacc.c:1646  */
    {
	    set_parser_setno((yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno);
	}
#line 7962 "y.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 2322 "pars.yacc" /* yacc.c:1646  */
    {
	    set_hotlink((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno, 1, (yyvsp[0].sval), (yyvsp[-1].ival));
	    xfree((yyvsp[0].sval));
	}
#line 7971 "y.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 2326 "pars.yacc" /* yacc.c:1646  */
    {
	    set_hotlink((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].ival), NULL, 0);
	}
#line 7979 "y.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 2331 "pars.yacc" /* yacc.c:1646  */
    {
	    curbox = next_box();
	}
#line 7987 "y.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 2334 "pars.yacc" /* yacc.c:1646  */
    {
            int no = (yyvsp[0].ival);
            if (is_valid_box(no) ||
                realloc_boxes(no + 1) == RETURN_SUCCESS) {
                curbox = no;
            }
	}
#line 7999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 2341 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_box(curbox)) {
                yyerror("Box not active");
	    } else {
	        boxes[curbox].active = (yyvsp[0].ival);
            }
	}
#line 8011 "y.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 2348 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_box(curbox)) {
                yyerror("Box not active");
	    } else {
	        boxes[curbox].gno = (yyvsp[0].ival);
            }
	}
#line 8023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 2355 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_box(curbox)) {
                yyerror("Box not active");
	    } else {
		boxes[curbox].x1 = (yyvsp[-6].dval);
		boxes[curbox].y1 = (yyvsp[-4].dval);
		boxes[curbox].x2 = (yyvsp[-2].dval);
		boxes[curbox].y2 = (yyvsp[0].dval);
	    }
	}
#line 8038 "y.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 2365 "pars.yacc" /* yacc.c:1646  */
    {
	    box_loctype = (yyvsp[0].ival);
	}
#line 8046 "y.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 2368 "pars.yacc" /* yacc.c:1646  */
    {
	    box_lines = (yyvsp[0].ival);
	}
#line 8054 "y.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 2371 "pars.yacc" /* yacc.c:1646  */
    {
	    box_linew = (yyvsp[0].dval);
	}
#line 8062 "y.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 2374 "pars.yacc" /* yacc.c:1646  */
    {
	    box_color = (yyvsp[0].ival);
	}
#line 8070 "y.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 2377 "pars.yacc" /* yacc.c:1646  */
    {
	    box_fillcolor = (yyvsp[0].ival);
	}
#line 8078 "y.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 2380 "pars.yacc" /* yacc.c:1646  */
    {
	    box_fillpat = (yyvsp[0].ival);
	}
#line 8086 "y.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 2383 "pars.yacc" /* yacc.c:1646  */
    {
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
#line 8120 "y.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 2414 "pars.yacc" /* yacc.c:1646  */
    {
		curellipse = next_ellipse();
	}
#line 8128 "y.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 2417 "pars.yacc" /* yacc.c:1646  */
    {
            int no = (yyvsp[0].ival);
            if (is_valid_ellipse(no) ||
                realloc_ellipses(no + 1) == RETURN_SUCCESS) {
                curellipse = no;
            }
	}
#line 8140 "y.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 2424 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_ellipse(curellipse)) {
                yyerror("Ellipse not active");
	    } else {
	        ellip[curellipse].active = (yyvsp[0].ival);
            }
	}
#line 8152 "y.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 2431 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_ellipse(curellipse)) {
                yyerror("Ellipse not active");
	    } else {
	        ellip[curellipse].gno = (yyvsp[0].ival);
            }
	}
#line 8164 "y.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 2438 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_ellipse(curellipse)) {
                yyerror("Ellipse not active");
	    } else {
		ellip[curellipse].x1 = (yyvsp[-6].dval);
		ellip[curellipse].y1 = (yyvsp[-4].dval);
		ellip[curellipse].x2 = (yyvsp[-2].dval);
		ellip[curellipse].y2 = (yyvsp[0].dval);
	    }
	}
#line 8179 "y.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 2448 "pars.yacc" /* yacc.c:1646  */
    {
	    ellipse_loctype = (yyvsp[0].ival);
	}
#line 8187 "y.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 2451 "pars.yacc" /* yacc.c:1646  */
    {
	    ellipse_lines = (yyvsp[0].ival);
	}
#line 8195 "y.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 2454 "pars.yacc" /* yacc.c:1646  */
    {
	    ellipse_linew = (yyvsp[0].dval);
	}
#line 8203 "y.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 2457 "pars.yacc" /* yacc.c:1646  */
    {
	    ellipse_color = (yyvsp[0].ival);
	}
#line 8211 "y.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 2460 "pars.yacc" /* yacc.c:1646  */
    {
	    ellipse_fillcolor = (yyvsp[0].ival);
	}
#line 8219 "y.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 2463 "pars.yacc" /* yacc.c:1646  */
    {
	    ellipse_fillpat = (yyvsp[0].ival);
	}
#line 8227 "y.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 2466 "pars.yacc" /* yacc.c:1646  */
    {
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
#line 8261 "y.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 2497 "pars.yacc" /* yacc.c:1646  */
    {
	    curline = next_line();
	}
#line 8269 "y.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 2500 "pars.yacc" /* yacc.c:1646  */
    {
            int no = (yyvsp[0].ival);
            if (is_valid_line(no) ||
                realloc_lines(no + 1) == RETURN_SUCCESS) {
                curline = no;
            }
	}
#line 8281 "y.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 2507 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_line(curline)) {
                yyerror("Line not active");
	    } else {
	        lines[curline].active = (yyvsp[0].ival);
            }
	}
#line 8293 "y.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 2514 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_line(curline)) {
                yyerror("Line not active");
	    } else {
	        lines[curline].gno = (yyvsp[0].ival);
            }
	}
#line 8305 "y.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 2521 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_line(curline)) {
                yyerror("Line not active");
	    } else {
	        lines[curline].x1 = (yyvsp[-6].dval);
	        lines[curline].y1 = (yyvsp[-4].dval);
	        lines[curline].x2 = (yyvsp[-2].dval);
	        lines[curline].y2 = (yyvsp[0].dval);
            }
	}
#line 8320 "y.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 2531 "pars.yacc" /* yacc.c:1646  */
    {
	    line_loctype = (yyvsp[0].ival);
	}
#line 8328 "y.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 2534 "pars.yacc" /* yacc.c:1646  */
    {
	    line_linew = (yyvsp[0].dval);
	}
#line 8336 "y.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 2537 "pars.yacc" /* yacc.c:1646  */
    {
	    line_lines = (yyvsp[0].ival);
	}
#line 8344 "y.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 2540 "pars.yacc" /* yacc.c:1646  */
    {
	    line_color = (yyvsp[0].ival);
	}
#line 8352 "y.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 2543 "pars.yacc" /* yacc.c:1646  */
    {
	    line_arrow_end = (yyvsp[0].ival);
	}
#line 8360 "y.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 2546 "pars.yacc" /* yacc.c:1646  */
    {
	    line_asize = (yyvsp[0].dval);
	}
#line 8368 "y.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 2549 "pars.yacc" /* yacc.c:1646  */
    {
	    line_atype = (yyvsp[0].ival);
	}
#line 8376 "y.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 2552 "pars.yacc" /* yacc.c:1646  */
    {
	    line_a_dL_ff = (yyvsp[-2].dval);
	    line_a_lL_ff = (yyvsp[0].dval);
	}
#line 8385 "y.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 2556 "pars.yacc" /* yacc.c:1646  */
    {
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
#line 8405 "y.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 2573 "pars.yacc" /* yacc.c:1646  */
    {
            curstring = next_string();
        }
#line 8413 "y.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 2576 "pars.yacc" /* yacc.c:1646  */
    {
            int no = (yyvsp[0].ival);
            if (is_valid_string(no) ||
                realloc_strings(no + 1) == RETURN_SUCCESS) {
                curstring = no;
            }
        }
#line 8425 "y.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 2583 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
                pstr[curstring].active = (yyvsp[0].ival);
            }
        }
#line 8437 "y.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 2590 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
                pstr[curstring].gno = (yyvsp[0].ival);
            }
        }
#line 8449 "y.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 2597 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
	        pstr[curstring].x = (yyvsp[-2].dval);
	        pstr[curstring].y = (yyvsp[0].dval);
            }
	}
#line 8462 "y.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 2605 "pars.yacc" /* yacc.c:1646  */
    {
            string_loctype = (yyvsp[0].ival);
        }
#line 8470 "y.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 2608 "pars.yacc" /* yacc.c:1646  */
    {
            string_color = (yyvsp[0].ival);
        }
#line 8478 "y.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 2611 "pars.yacc" /* yacc.c:1646  */
    {
            string_rot = (yyvsp[0].ival);
        }
#line 8486 "y.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 2614 "pars.yacc" /* yacc.c:1646  */
    {
            string_font = (yyvsp[0].ival);
        }
#line 8494 "y.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 2617 "pars.yacc" /* yacc.c:1646  */
    {
            string_just = (yyvsp[0].ival);
        }
#line 8502 "y.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 2620 "pars.yacc" /* yacc.c:1646  */
    {
            string_size = (yyvsp[0].dval);
        }
#line 8510 "y.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 2623 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_string(curstring)) {
                yyerror("String not active");
	    } else {
	        set_plotstr_string(&pstr[curstring], (yyvsp[0].sval));
	        pstr[curstring].color = string_color;
	        pstr[curstring].font = string_font;
	        pstr[curstring].just = string_just;
	        pstr[curstring].loctype = string_loctype;
	        pstr[curstring].rot = string_rot;
	        pstr[curstring].charsize = string_size;
            }
	    xfree((yyvsp[0].sval));
	}
#line 8529 "y.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 2639 "pars.yacc" /* yacc.c:1646  */
    {
            timestamp.active = (yyvsp[0].ival);
        }
#line 8537 "y.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 2642 "pars.yacc" /* yacc.c:1646  */
    {
            timestamp.font = (yyvsp[0].ival);
        }
#line 8545 "y.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 2645 "pars.yacc" /* yacc.c:1646  */
    {
            timestamp.charsize = (yyvsp[0].dval);
        }
#line 8553 "y.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 2648 "pars.yacc" /* yacc.c:1646  */
    {
            timestamp.rot = (yyvsp[0].ival);
        }
#line 8561 "y.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 2651 "pars.yacc" /* yacc.c:1646  */
    {
            timestamp.color = (yyvsp[0].ival);
        }
#line 8569 "y.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 2654 "pars.yacc" /* yacc.c:1646  */
    {
	    timestamp.x = (yyvsp[-2].dval);
	    timestamp.y = (yyvsp[0].dval);
	}
#line 8578 "y.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 2658 "pars.yacc" /* yacc.c:1646  */
    {
	  set_plotstr_string(&timestamp, (yyvsp[0].sval));
	  xfree((yyvsp[0].sval));
	}
#line 8587 "y.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 2664 "pars.yacc" /* yacc.c:1646  */
    {
	    grdefaults.lines = (yyvsp[0].ival);
	    box_lines = ellipse_lines = line_lines = (yyvsp[0].ival);
	}
#line 8596 "y.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 2668 "pars.yacc" /* yacc.c:1646  */
    {
	    grdefaults.linew = (yyvsp[0].dval);
	    box_linew = ellipse_linew = line_linew = (yyvsp[0].dval);
	}
#line 8605 "y.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 2672 "pars.yacc" /* yacc.c:1646  */
    {
	    grdefaults.color = (yyvsp[0].ival);
	    box_color = ellipse_color = line_color = string_color = (yyvsp[0].ival);
	}
#line 8614 "y.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 2676 "pars.yacc" /* yacc.c:1646  */
    {
	    grdefaults.pattern = (yyvsp[0].ival);
	}
#line 8622 "y.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 2679 "pars.yacc" /* yacc.c:1646  */
    {
	    grdefaults.charsize = (yyvsp[0].dval);
	    string_size = (yyvsp[0].dval);
	}
#line 8631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 2683 "pars.yacc" /* yacc.c:1646  */
    {
	    grdefaults.font = (yyvsp[0].ival);
	    string_font = (yyvsp[0].ival);
	}
#line 8640 "y.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 2687 "pars.yacc" /* yacc.c:1646  */
    {
	    grdefaults.symsize = (yyvsp[0].dval);
	}
#line 8648 "y.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 2690 "pars.yacc" /* yacc.c:1646  */
    {
	    strcpy(sformat, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 8657 "y.tab.c" /* yacc.c:1646  */
    break;

  case 281:
#line 2694 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((map_font_by_name((yyvsp[-2].sval), (yyvsp[-4].ival)) != RETURN_SUCCESS) && 
                (map_font_by_name((yyvsp[0].sval), (yyvsp[-4].ival)) != RETURN_SUCCESS)) {
                errmsg("Failed mapping a font");
            }
            xfree((yyvsp[-2].sval));
	    xfree((yyvsp[0].sval));
	}
#line 8670 "y.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 2702 "pars.yacc" /* yacc.c:1646  */
    {
	    CMap_entry cmap;
            cmap.rgb.red   = (yyvsp[-7].ival);
            cmap.rgb.green = (yyvsp[-5].ival);
            cmap.rgb.blue  = (yyvsp[-3].ival);
            cmap.ctype = COLOR_MAIN;
            cmap.cname = (yyvsp[0].sval);
            if (store_color((yyvsp[-10].ival), cmap) == RETURN_FAILURE) {
                errmsg("Failed mapping a color");
            }
	    xfree((yyvsp[0].sval));
        }
#line 8687 "y.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 2715 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.xg1 = (yyvsp[-6].dval);
	    g[whichgraph].w.yg1 = (yyvsp[-4].dval);
	    g[whichgraph].w.xg2 = (yyvsp[-2].dval);
	    g[whichgraph].w.yg2 = (yyvsp[0].dval);
	}
#line 8702 "y.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 2725 "pars.yacc" /* yacc.c:1646  */
    {
	    set_graph_znorm(whichgraph, (yyvsp[0].dval));
	}
#line 8710 "y.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 2728 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.xv1 = (yyvsp[-6].dval);
	    g[whichgraph].v.yv1 = (yyvsp[-4].dval);
	    g[whichgraph].v.xv2 = (yyvsp[-2].dval);
	    g[whichgraph].v.yv2 = (yyvsp[0].dval);
	}
#line 8725 "y.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 2738 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    set_plotstr_string(&g[whichgraph].labs.title, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 8738 "y.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 2746 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.title.font = (yyvsp[0].ival);
	}
#line 8750 "y.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 2753 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.title.charsize = (yyvsp[0].dval);
	}
#line 8762 "y.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 2760 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.title.color = (yyvsp[0].ival);
	}
#line 8774 "y.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 2767 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    set_plotstr_string(&g[whichgraph].labs.stitle, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 8787 "y.tab.c" /* yacc.c:1646  */
    break;

  case 291:
#line 2775 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.stitle.font = (yyvsp[0].ival);
	}
#line 8799 "y.tab.c" /* yacc.c:1646  */
    break;

  case 292:
#line 2782 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.stitle.charsize = (yyvsp[0].dval);
	}
#line 8811 "y.tab.c" /* yacc.c:1646  */
    break;

  case 293:
#line 2789 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].labs.stitle.color = (yyvsp[0].ival);
	}
#line 8823 "y.tab.c" /* yacc.c:1646  */
    break;

  case 294:
#line 2797 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].xscale = (yyvsp[0].ival);
	}
#line 8835 "y.tab.c" /* yacc.c:1646  */
    break;

  case 295:
#line 2804 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].yscale = (yyvsp[0].ival);
	}
#line 8847 "y.tab.c" /* yacc.c:1646  */
    break;

  case 296:
#line 2811 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].xinvert = (yyvsp[0].ival);
	}
#line 8859 "y.tab.c" /* yacc.c:1646  */
    break;

  case 297:
#line 2818 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].yinvert = (yyvsp[0].ival);
	}
#line 8871 "y.tab.c" /* yacc.c:1646  */
    break;

  case 298:
#line 2825 "pars.yacc" /* yacc.c:1646  */
    {
            autoscale_onread = AUTOSCALE_NONE;
        }
#line 8879 "y.tab.c" /* yacc.c:1646  */
    break;

  case 299:
#line 2828 "pars.yacc" /* yacc.c:1646  */
    {
            autoscale_onread = AUTOSCALE_X;
        }
#line 8887 "y.tab.c" /* yacc.c:1646  */
    break;

  case 300:
#line 2831 "pars.yacc" /* yacc.c:1646  */
    {
            autoscale_onread = AUTOSCALE_Y;
        }
#line 8895 "y.tab.c" /* yacc.c:1646  */
    break;

  case 301:
#line 2834 "pars.yacc" /* yacc.c:1646  */
    {
            autoscale_onread = AUTOSCALE_XY;
        }
#line 8903 "y.tab.c" /* yacc.c:1646  */
    break;

  case 302:
#line 2838 "pars.yacc" /* yacc.c:1646  */
    {
            char *s;
            s = copy_string(NULL, get_project_description());
            s = concat_strings(s, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
            s = concat_strings(s, "\n");
            set_project_description(s);
            xfree(s);
	}
#line 8917 "y.tab.c" /* yacc.c:1646  */
    break;

  case 303:
#line 2847 "pars.yacc" /* yacc.c:1646  */
    {
            set_project_description(NULL);
        }
#line 8925 "y.tab.c" /* yacc.c:1646  */
    break;

  case 304:
#line 2851 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.active = (yyvsp[0].ival);
	}
#line 8937 "y.tab.c" /* yacc.c:1646  */
    break;

  case 305:
#line 2858 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.loctype = (yyvsp[0].ival);
	}
#line 8949 "y.tab.c" /* yacc.c:1646  */
    break;

  case 306:
#line 2865 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].l.vgap = (yyvsp[0].ival);
	}
#line 8961 "y.tab.c" /* yacc.c:1646  */
    break;

  case 307:
#line 2872 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.hgap = (yyvsp[0].ival);
	}
#line 8973 "y.tab.c" /* yacc.c:1646  */
    break;

  case 308:
#line 2879 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.len = (yyvsp[0].ival);
	}
#line 8985 "y.tab.c" /* yacc.c:1646  */
    break;

  case 309:
#line 2886 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.invert = (yyvsp[0].ival);
        }
#line 8997 "y.tab.c" /* yacc.c:1646  */
    break;

  case 310:
#line 2893 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxfillpen.color = (yyvsp[0].ival);
        }
#line 9009 "y.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 2900 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxfillpen.pattern = (yyvsp[0].ival);
        }
#line 9021 "y.tab.c" /* yacc.c:1646  */
    break;

  case 312:
#line 2907 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxpen.color = (yyvsp[0].ival);
	}
#line 9033 "y.tab.c" /* yacc.c:1646  */
    break;

  case 313:
#line 2914 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxpen.pattern = (yyvsp[0].ival);
	}
#line 9045 "y.tab.c" /* yacc.c:1646  */
    break;

  case 314:
#line 2921 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxlines = (yyvsp[0].ival);
	}
#line 9057 "y.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 2928 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.boxlinew = (yyvsp[0].dval);
	}
#line 9069 "y.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 2935 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.legx = (yyvsp[-2].dval);
	    g[whichgraph].l.legy = (yyvsp[0].dval);
	}
#line 9082 "y.tab.c" /* yacc.c:1646  */
    break;

  case 317:
#line 2943 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.charsize = (yyvsp[0].dval);
	}
#line 9094 "y.tab.c" /* yacc.c:1646  */
    break;

  case 318:
#line 2950 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.font = (yyvsp[0].ival);
	}
#line 9106 "y.tab.c" /* yacc.c:1646  */
    break;

  case 319:
#line 2957 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.color = (yyvsp[0].ival);
	}
#line 9118 "y.tab.c" /* yacc.c:1646  */
    break;

  case 320:
#line 2965 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.pen.pattern = (yyvsp[0].ival);
	}
#line 9130 "y.tab.c" /* yacc.c:1646  */
    break;

  case 321:
#line 2972 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.type = (yyvsp[0].ival);
	}
#line 9142 "y.tab.c" /* yacc.c:1646  */
    break;

  case 322:
#line 2979 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.lines = (yyvsp[0].ival);
	}
#line 9154 "y.tab.c" /* yacc.c:1646  */
    break;

  case 323:
#line 2986 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.linew = (yyvsp[0].dval);
	}
#line 9166 "y.tab.c" /* yacc.c:1646  */
    break;

  case 324:
#line 2993 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.pen.color = (yyvsp[0].ival);
	}
#line 9178 "y.tab.c" /* yacc.c:1646  */
    break;

  case 325:
#line 3000 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].f.pen.pattern = (yyvsp[0].ival);
	}
#line 9190 "y.tab.c" /* yacc.c:1646  */
    break;

  case 326:
#line 3008 "pars.yacc" /* yacc.c:1646  */
    { 
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.fillpen.color = (yyvsp[0].ival);
        }
#line 9202 "y.tab.c" /* yacc.c:1646  */
    break;

  case 327:
#line 3016 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.fillpen.pattern = (yyvsp[0].ival);
        }
#line 9214 "y.tab.c" /* yacc.c:1646  */
    break;

  case 328:
#line 3024 "pars.yacc" /* yacc.c:1646  */
    {
            set_graph_hidden((yyvsp[-1].ival), !(yyvsp[0].ival));
        }
#line 9222 "y.tab.c" /* yacc.c:1646  */
    break;

  case 329:
#line 3027 "pars.yacc" /* yacc.c:1646  */
    {
            set_graph_hidden((yyvsp[-2].ival), (yyvsp[0].ival));
        }
#line 9230 "y.tab.c" /* yacc.c:1646  */
    break;

  case 330:
#line 3030 "pars.yacc" /* yacc.c:1646  */
    {
            set_graph_type((yyvsp[-2].ival), (yyvsp[0].ival));
        }
#line 9238 "y.tab.c" /* yacc.c:1646  */
    break;

  case 331:
#line 3033 "pars.yacc" /* yacc.c:1646  */
    {
            set_graph_stacked((yyvsp[-2].ival), (yyvsp[0].ival));
        }
#line 9246 "y.tab.c" /* yacc.c:1646  */
    break;

  case 332:
#line 3037 "pars.yacc" /* yacc.c:1646  */
    {
	    set_graph_bargap((yyvsp[-3].ival), (yyvsp[0].dval));
	}
#line 9254 "y.tab.c" /* yacc.c:1646  */
    break;

  case 333:
#line 3041 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-2].ival)].locator.pointset = (yyvsp[0].ival);
        }
#line 9262 "y.tab.c" /* yacc.c:1646  */
    break;

  case 334:
#line 3044 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-4].ival)].locator.fx = (yyvsp[-1].ival);
	    g[(yyvsp[-4].ival)].locator.fy = (yyvsp[0].ival);
	}
#line 9271 "y.tab.c" /* yacc.c:1646  */
    break;

  case 335:
#line 3048 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-5].ival)].locator.px = (yyvsp[-2].dval);
	    g[(yyvsp[-5].ival)].locator.py = (yyvsp[0].dval);
	}
#line 9280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 336:
#line 3052 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-5].ival)].locator.dsx = (yyvsp[-2].dval);
	    g[(yyvsp[-5].ival)].locator.dsy = (yyvsp[0].dval);
	}
#line 9289 "y.tab.c" /* yacc.c:1646  */
    break;

  case 337:
#line 3056 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-3].ival)].locator.pt_type = (yyvsp[0].ival);
        }
#line 9297 "y.tab.c" /* yacc.c:1646  */
    break;

  case 338:
#line 3060 "pars.yacc" /* yacc.c:1646  */
    {
	    curtype = (yyvsp[0].ival);
	}
#line 9305 "y.tab.c" /* yacc.c:1646  */
    break;

  case 339:
#line 3065 "pars.yacc" /* yacc.c:1646  */
    {
	    if (add_io_filter((yyvsp[-3].ival), (yyvsp[-1].ival), (yyvsp[0].sval), (yyvsp[-2].sval)) != 0) {
	        yyerror("Failed adding i/o filter");
	    }
	    xfree((yyvsp[-2].sval));
	    xfree((yyvsp[0].sval));
	}
#line 9317 "y.tab.c" /* yacc.c:1646  */
    break;

  case 340:
#line 3072 "pars.yacc" /* yacc.c:1646  */
    {
	    clear_io_filters((yyvsp[0].ival));
	}
#line 9325 "y.tab.c" /* yacc.c:1646  */
    break;

  case 341:
#line 3076 "pars.yacc" /* yacc.c:1646  */
    {
	    cursource = (yyvsp[0].ival);
	}
#line 9333 "y.tab.c" /* yacc.c:1646  */
    break;

  case 342:
#line 3079 "pars.yacc" /* yacc.c:1646  */
    {
	    readxformat = (yyvsp[0].ival);
	}
#line 9341 "y.tab.c" /* yacc.c:1646  */
    break;

  case 343:
#line 3082 "pars.yacc" /* yacc.c:1646  */
    { }
#line 9347 "y.tab.c" /* yacc.c:1646  */
    break;

  case 344:
#line 3083 "pars.yacc" /* yacc.c:1646  */
    {
	    nonl_parms[(yyvsp[-2].ival)].constr = (yyvsp[0].ival);
	}
#line 9355 "y.tab.c" /* yacc.c:1646  */
    break;

  case 345:
#line 3089 "pars.yacc" /* yacc.c:1646  */
    {
	    drawgraph();
	}
#line 9363 "y.tab.c" /* yacc.c:1646  */
    break;

  case 346:
#line 3092 "pars.yacc" /* yacc.c:1646  */
    {
#ifndef NONE_GUI
            if (inwin) {
                update_all();
            }
#endif
        }
#line 9375 "y.tab.c" /* yacc.c:1646  */
    break;

  case 347:
#line 3099 "pars.yacc" /* yacc.c:1646  */
    {
	    set_workingdir((yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 9384 "y.tab.c" /* yacc.c:1646  */
    break;

  case 348:
#line 3103 "pars.yacc" /* yacc.c:1646  */
    {
	    echomsg((yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 9393 "y.tab.c" /* yacc.c:1646  */
    break;

  case 349:
#line 3107 "pars.yacc" /* yacc.c:1646  */
    {
	    char buf[32];
            set_locale_num(TRUE);
            sprintf(buf, "%g", (yyvsp[0].dval));
            set_locale_num(FALSE);
            echomsg(buf);
	}
#line 9405 "y.tab.c" /* yacc.c:1646  */
    break;

  case 350:
#line 3114 "pars.yacc" /* yacc.c:1646  */
    {
	    close_input = copy_string(close_input, "");
	}
#line 9413 "y.tab.c" /* yacc.c:1646  */
    break;

  case 351:
#line 3117 "pars.yacc" /* yacc.c:1646  */
    {
	    close_input = copy_string(close_input, (yyvsp[0].sval));
	}
#line 9421 "y.tab.c" /* yacc.c:1646  */
    break;

  case 352:
#line 3120 "pars.yacc" /* yacc.c:1646  */
    {
	    exit(0);
	}
#line 9429 "y.tab.c" /* yacc.c:1646  */
    break;

  case 353:
#line 3123 "pars.yacc" /* yacc.c:1646  */
    {
	    exit((yyvsp[-1].ival));
	}
#line 9437 "y.tab.c" /* yacc.c:1646  */
    break;

  case 354:
#line 3126 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!safe_mode) {
                do_hardcopy();
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	}
#line 9449 "y.tab.c" /* yacc.c:1646  */
    break;

  case 355:
#line 3133 "pars.yacc" /* yacc.c:1646  */
    {
            set_ptofile(FALSE);
	}
#line 9457 "y.tab.c" /* yacc.c:1646  */
    break;

  case 356:
#line 3136 "pars.yacc" /* yacc.c:1646  */
    {
            set_ptofile(TRUE);
	    strcpy(print_file, (yyvsp[0].sval));
            xfree((yyvsp[0].sval));
	}
#line 9467 "y.tab.c" /* yacc.c:1646  */
    break;

  case 357:
#line 3141 "pars.yacc" /* yacc.c:1646  */
    {
	    switch ((yyvsp[0].ival)) {
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
#line 9494 "y.tab.c" /* yacc.c:1646  */
    break;

  case 358:
#line 3163 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[0].dval) > 0) {
	        msleep_wrap((unsigned int) (1000 * (yyvsp[0].dval)));
	    }
	}
#line 9504 "y.tab.c" /* yacc.c:1646  */
    break;

  case 359:
#line 3168 "pars.yacc" /* yacc.c:1646  */
    {
#ifndef NONE_GUI
            if (inwin) {
                HelpCB((yyvsp[0].sval));
            }
            xfree((yyvsp[0].sval));
#endif
	}
#line 9517 "y.tab.c" /* yacc.c:1646  */
    break;

  case 360:
#line 3176 "pars.yacc" /* yacc.c:1646  */
    {
#ifndef NONE_GUI
            if (inwin) {
                HelpCB("doc/UsersGuide.html");
            }
#endif
	}
#line 9529 "y.tab.c" /* yacc.c:1646  */
    break;

  case 361:
#line 3183 "pars.yacc" /* yacc.c:1646  */
    {
	    gotparams = TRUE;
	    strcpy(paramfile, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 9539 "y.tab.c" /* yacc.c:1646  */
    break;

  case 362:
#line 3188 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!safe_mode) {
                FILE *pp = grace_openw((yyvsp[0].sval));
	        if (pp != NULL) {
	            putparms(whichgraph, pp, 0);
	            grace_close(pp);
	        }
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree((yyvsp[0].sval));
	}
#line 9556 "y.tab.c" /* yacc.c:1646  */
    break;

  case 363:
#line 3200 "pars.yacc" /* yacc.c:1646  */
    {
	    set_set_hidden((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].ival));
	}
#line 9564 "y.tab.c" /* yacc.c:1646  */
    break;

  case 364:
#line 3203 "pars.yacc" /* yacc.c:1646  */
    {
	    setlength((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].ival));
	}
#line 9572 "y.tab.c" /* yacc.c:1646  */
    break;

  case 365:
#line 3206 "pars.yacc" /* yacc.c:1646  */
    {
	    realloc_vrbl((yyvsp[-2].vrbl), (yyvsp[0].ival));
	}
#line 9580 "y.tab.c" /* yacc.c:1646  */
    break;

  case 366:
#line 3209 "pars.yacc" /* yacc.c:1646  */
    {
	    add_point((yyvsp[-4].trgt)->gno, (yyvsp[-4].trgt)->setno, (yyvsp[-2].dval), (yyvsp[0].dval));
	}
#line 9588 "y.tab.c" /* yacc.c:1646  */
    break;

  case 367:
#line 3213 "pars.yacc" /* yacc.c:1646  */
    {
	    int start = (yyvsp[-2].ival) - index_shift;
	    int stop = (yyvsp[0].ival) - index_shift;
	    droppoints((yyvsp[-4].trgt)->gno, (yyvsp[-4].trgt)->setno, start, stop);
	}
#line 9598 "y.tab.c" /* yacc.c:1646  */
    break;

  case 368:
#line 3218 "pars.yacc" /* yacc.c:1646  */
    {
	    if (is_set_active((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno)) {
	        sortset((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[-1].ival), (yyvsp[0].ival) == ASCENDING ? 0 : 1);
	    }
	}
#line 9608 "y.tab.c" /* yacc.c:1646  */
    break;

  case 369:
#line 3223 "pars.yacc" /* yacc.c:1646  */
    {
	    do_copyset((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno);
	}
#line 9616 "y.tab.c" /* yacc.c:1646  */
    break;

  case 370:
#line 3226 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[-2].trgt)->gno != (yyvsp[0].trgt)->gno) {
                errmsg("Can't append sets from different graphs");
            } else {
                int sets[2];
	        sets[0] = (yyvsp[0].trgt)->setno;
	        sets[1] = (yyvsp[-2].trgt)->setno;
	        join_sets((yyvsp[-2].trgt)->gno, sets, 2);
            }
	}
#line 9631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 371:
#line 3236 "pars.yacc" /* yacc.c:1646  */
    {
            reverse_set((yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno);
	}
#line 9639 "y.tab.c" /* yacc.c:1646  */
    break;

  case 372:
#line 3239 "pars.yacc" /* yacc.c:1646  */
    {
            do_splitsets((yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno, (yyvsp[0].ival));
	}
#line 9647 "y.tab.c" /* yacc.c:1646  */
    break;

  case 373:
#line 3242 "pars.yacc" /* yacc.c:1646  */
    {
	    do_moveset((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno);
	}
#line 9655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 374:
#line 3245 "pars.yacc" /* yacc.c:1646  */
    {
	    do_swapset((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno);
	}
#line 9663 "y.tab.c" /* yacc.c:1646  */
    break;

  case 375:
#line 3248 "pars.yacc" /* yacc.c:1646  */
    {
	    killset((yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno);
	}
#line 9671 "y.tab.c" /* yacc.c:1646  */
    break;

  case 376:
#line 3251 "pars.yacc" /* yacc.c:1646  */
    {
            killsetdata((yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno);
            setcomment((yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno, "");
        }
#line 9680 "y.tab.c" /* yacc.c:1646  */
    break;

  case 377:
#line 3255 "pars.yacc" /* yacc.c:1646  */
    {
            kill_graph((yyvsp[0].ival));
        }
#line 9688 "y.tab.c" /* yacc.c:1646  */
    break;

  case 378:
#line 3258 "pars.yacc" /* yacc.c:1646  */
    {
            kill_region((yyvsp[0].ival));
        }
#line 9696 "y.tab.c" /* yacc.c:1646  */
    break;

  case 379:
#line 3261 "pars.yacc" /* yacc.c:1646  */
    {
            wipeout();
        }
#line 9704 "y.tab.c" /* yacc.c:1646  */
    break;

  case 380:
#line 3264 "pars.yacc" /* yacc.c:1646  */
    {
            arrange_graphs_simple((yyvsp[-9].ival), (yyvsp[-7].ival), 0, FALSE, (yyvsp[-5].dval), (yyvsp[-3].dval), (yyvsp[-1].dval));
        }
#line 9712 "y.tab.c" /* yacc.c:1646  */
    break;

  case 381:
#line 3267 "pars.yacc" /* yacc.c:1646  */
    {
            int order = ((yyvsp[-5].ival) * GA_ORDER_HV_INV) |
                        ((yyvsp[-3].ival) * GA_ORDER_H_INV ) |
                        ((yyvsp[-1].ival) * GA_ORDER_V_INV );
            arrange_graphs_simple((yyvsp[-15].ival), (yyvsp[-13].ival), order, FALSE, (yyvsp[-11].dval), (yyvsp[-9].dval), (yyvsp[-7].dval));
        }
#line 9723 "y.tab.c" /* yacc.c:1646  */
    break;

  case 382:
#line 3273 "pars.yacc" /* yacc.c:1646  */
    {
            int order = ((yyvsp[-7].ival) * GA_ORDER_HV_INV) |
                        ((yyvsp[-5].ival) * GA_ORDER_H_INV ) |
                        ((yyvsp[-3].ival) * GA_ORDER_V_INV );
            arrange_graphs_simple((yyvsp[-17].ival), (yyvsp[-15].ival), order, (yyvsp[-1].ival), (yyvsp[-13].dval), (yyvsp[-11].dval), (yyvsp[-9].dval));
        }
#line 9734 "y.tab.c" /* yacc.c:1646  */
    break;

  case 383:
#line 3279 "pars.yacc" /* yacc.c:1646  */
    {
	    gotnlfit = TRUE;
	    nlfit_gno = (yyvsp[-3].trgt)->gno;
	    nlfit_setno = (yyvsp[-3].trgt)->setno;
	    nlfit_nsteps = (yyvsp[-1].ival);
	    nlfit_warray = NULL;
	}
#line 9746 "y.tab.c" /* yacc.c:1646  */
    break;

  case 384:
#line 3286 "pars.yacc" /* yacc.c:1646  */
    {
	    if (getsetlength((yyvsp[-5].trgt)->gno, (yyvsp[-5].trgt)->setno) != (yyvsp[-3].vrbl)->length) {
                errmsg("Data and weight arrays are of different lengths");
                return 1;
            } else {
	        gotnlfit = TRUE;
	        nlfit_gno = (yyvsp[-5].trgt)->gno;
	        nlfit_setno = (yyvsp[-5].trgt)->setno;
	        nlfit_nsteps = (yyvsp[-1].ival);
	        nlfit_warray = copy_data_column((yyvsp[-3].vrbl)->data, (yyvsp[-3].vrbl)->length);
            }
	}
#line 9763 "y.tab.c" /* yacc.c:1646  */
    break;

  case 385:
#line 3298 "pars.yacc" /* yacc.c:1646  */
    {
	    do_regress((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno, (yyvsp[-1].ival), 0, -1, 0, -1);
	}
#line 9771 "y.tab.c" /* yacc.c:1646  */
    break;

  case 386:
#line 3301 "pars.yacc" /* yacc.c:1646  */
    {
	    do_runavg((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno, (yyvsp[-1].ival), (yyvsp[-5].ival), -1, 0);
	}
#line 9779 "y.tab.c" /* yacc.c:1646  */
    break;

  case 387:
#line 3304 "pars.yacc" /* yacc.c:1646  */
    {
	    do_fourier_command((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno, (yyvsp[-5].ival), (yyvsp[-1].ival));
	}
#line 9787 "y.tab.c" /* yacc.c:1646  */
    break;

  case 388:
#line 3308 "pars.yacc" /* yacc.c:1646  */
    {
	    switch ((yyvsp[-11].ival)) {
	    case FFT_DFT:
                do_fourier((yyvsp[-9].trgt)->gno, (yyvsp[-9].trgt)->setno, 0, (yyvsp[-1].ival), (yyvsp[-3].ival), 0, (yyvsp[-7].ival), (yyvsp[-5].ival));
	        break;
	    case FFT_INVDFT    :
                do_fourier((yyvsp[-9].trgt)->gno, (yyvsp[-9].trgt)->setno, 0, (yyvsp[-1].ival), (yyvsp[-3].ival), 1, (yyvsp[-7].ival), (yyvsp[-5].ival));
	        break;
	    case FFT_FFT:
                do_fourier((yyvsp[-9].trgt)->gno, (yyvsp[-9].trgt)->setno, 1, (yyvsp[-1].ival), (yyvsp[-3].ival), 0, (yyvsp[-7].ival), (yyvsp[-5].ival));
	        break;
	    case FFT_INVFFT    :
                do_fourier((yyvsp[-9].trgt)->gno, (yyvsp[-9].trgt)->setno, 1, (yyvsp[-1].ival), (yyvsp[-3].ival), 1, (yyvsp[-7].ival), (yyvsp[-5].ival));
	        break;
	    default:
                errmsg("Internal error");
	        break;
	    }
        }
#line 9811 "y.tab.c" /* yacc.c:1646  */
    break;

  case 389:
#line 3327 "pars.yacc" /* yacc.c:1646  */
    {
            do_interp((yyvsp[-7].trgt)->gno, (yyvsp[-7].trgt)->setno, get_cg(), SET_SELECT_NEXT,
                (yyvsp[-5].vrbl)->data, (yyvsp[-5].vrbl)->length, (yyvsp[-3].ival), (yyvsp[-1].ival));
	}
#line 9820 "y.tab.c" /* yacc.c:1646  */
    break;

  case 390:
#line 3331 "pars.yacc" /* yacc.c:1646  */
    {
            do_histo((yyvsp[-7].trgt)->gno, (yyvsp[-7].trgt)->setno, get_cg(), SET_SELECT_NEXT,
                (yyvsp[-5].vrbl)->data, (yyvsp[-5].vrbl)->length - 1, (yyvsp[-3].ival), (yyvsp[-1].ival));
	}
#line 9829 "y.tab.c" /* yacc.c:1646  */
    break;

  case 391:
#line 3335 "pars.yacc" /* yacc.c:1646  */
    {
	    do_differ((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno, (yyvsp[-1].ival));
	}
#line 9837 "y.tab.c" /* yacc.c:1646  */
    break;

  case 392:
#line 3338 "pars.yacc" /* yacc.c:1646  */
    {
	    do_int((yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno, 0);
	}
#line 9845 "y.tab.c" /* yacc.c:1646  */
    break;

  case 393:
#line 3341 "pars.yacc" /* yacc.c:1646  */
    {
	    do_xcor((yyvsp[-7].trgt)->gno, (yyvsp[-7].trgt)->setno, (yyvsp[-5].trgt)->gno, (yyvsp[-5].trgt)->setno, (yyvsp[-3].ival), (yyvsp[-1].ival));
	}
#line 9853 "y.tab.c" /* yacc.c:1646  */
    break;

  case 394:
#line 3344 "pars.yacc" /* yacc.c:1646  */
    {
	    do_linearc((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno, (yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno);
	}
#line 9861 "y.tab.c" /* yacc.c:1646  */
    break;

  case 395:
#line 3347 "pars.yacc" /* yacc.c:1646  */
    {
            int len = getsetlength((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno);
            if (len != (yyvsp[-1].vrbl)->length) {
		errmsg("Filter expression is of a wrong length");
            } else {
                char *rarray;
                rarray = xmalloc(len*SIZEOF_CHAR);
                if (rarray) {
                    int i;
                    for (i = 0; i < len; i++) {
                        rarray[i] = CAST_DBL_TO_BOOL((yyvsp[-1].vrbl)->data[i]);
                    }
                    filter_set((yyvsp[-3].trgt)->gno, (yyvsp[-3].trgt)->setno, rarray);
                    xfree(rarray);
                }
            }
	}
#line 9883 "y.tab.c" /* yacc.c:1646  */
    break;

  case 396:
#line 3364 "pars.yacc" /* yacc.c:1646  */
    {
            int rtype;
            char *rarray;
            
            rtype = RESTRICT_REG0 + (yyvsp[-3].ival);

	    if (get_restriction_array((yyvsp[-5].trgt)->gno, (yyvsp[-5].trgt)->setno,
                rtype, (yyvsp[-1].ival), &rarray) != RETURN_SUCCESS) {
                errmsg("Error in region evaluation");
                return 1;
	    } else {
                filter_set((yyvsp[-5].trgt)->gno, (yyvsp[-5].trgt)->setno, rarray);
                xfree(rarray);
            }
	}
#line 9903 "y.tab.c" /* yacc.c:1646  */
    break;

  case 397:
#line 3379 "pars.yacc" /* yacc.c:1646  */
    {
	    if (autoscale_graph(whichgraph, AUTOSCALE_XY) != RETURN_SUCCESS) {
		errmsg("Can't autoscale (no active sets?)");
	    }
	}
#line 9913 "y.tab.c" /* yacc.c:1646  */
    break;

  case 398:
#line 3384 "pars.yacc" /* yacc.c:1646  */
    {
	    if (autoscale_graph(whichgraph, AUTOSCALE_X) != RETURN_SUCCESS) {
		errmsg("Can't autoscale (no active sets?)");
	    }
	}
#line 9923 "y.tab.c" /* yacc.c:1646  */
    break;

  case 399:
#line 3389 "pars.yacc" /* yacc.c:1646  */
    {
	    if (autoscale_graph(whichgraph, AUTOSCALE_Y) != RETURN_SUCCESS) {
		errmsg("Can't autoscale (no active sets?)");
	    }
	}
#line 9933 "y.tab.c" /* yacc.c:1646  */
    break;

  case 400:
#line 3394 "pars.yacc" /* yacc.c:1646  */
    {
	    autoscale_byset((yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno, AUTOSCALE_XY);
	}
#line 9941 "y.tab.c" /* yacc.c:1646  */
    break;

  case 401:
#line 3397 "pars.yacc" /* yacc.c:1646  */
    {
            autotick_axis(whichgraph, ALL_AXES);
        }
#line 9949 "y.tab.c" /* yacc.c:1646  */
    break;

  case 402:
#line 3400 "pars.yacc" /* yacc.c:1646  */
    {
	    int gno = (yyvsp[0].ival);
            if (is_graph_hidden(gno) == FALSE) {
                select_graph(gno);
            } else {
		errmsg("Graph is not active");
            }
	}
#line 9962 "y.tab.c" /* yacc.c:1646  */
    break;

  case 403:
#line 3408 "pars.yacc" /* yacc.c:1646  */
    {
	    gotread = TRUE;
	    strcpy(readfile, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 9972 "y.tab.c" /* yacc.c:1646  */
    break;

  case 404:
#line 3413 "pars.yacc" /* yacc.c:1646  */
    {
	    strcpy(batchfile, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 9981 "y.tab.c" /* yacc.c:1646  */
    break;

  case 405:
#line 3417 "pars.yacc" /* yacc.c:1646  */
    {
	    getdata(whichgraph, (yyvsp[0].sval), SOURCE_DISK, LOAD_BLOCK);
	    xfree((yyvsp[0].sval));
	}
#line 9990 "y.tab.c" /* yacc.c:1646  */
    break;

  case 406:
#line 3421 "pars.yacc" /* yacc.c:1646  */
    {
	    getdata(whichgraph, (yyvsp[0].sval), (yyvsp[-1].ival), LOAD_BLOCK);
	    xfree((yyvsp[0].sval));
	}
#line 9999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 407:
#line 3425 "pars.yacc" /* yacc.c:1646  */
    {
            int nc, *cols, scol;
            if (field_string_to_cols((yyvsp[0].sval), &nc, &cols, &scol) != RETURN_SUCCESS) {
                errmsg("Erroneous field specifications");
	        xfree((yyvsp[0].sval));
                return 1;
            } else {
	        xfree((yyvsp[0].sval));
	        create_set_fromblock(whichgraph, NEW_SET,
                    (yyvsp[-1].ival), nc, cols, scol, autoscale_onread);
                xfree(cols);
            }
	}
#line 10017 "y.tab.c" /* yacc.c:1646  */
    break;

  case 408:
#line 3438 "pars.yacc" /* yacc.c:1646  */
    {
	    set_blockdata(NULL);
	}
#line 10025 "y.tab.c" /* yacc.c:1646  */
    break;

  case 409:
#line 3441 "pars.yacc" /* yacc.c:1646  */
    {
	    gotread = TRUE;
	    curtype = (yyvsp[-1].ival);
	    strcpy(readfile, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 10036 "y.tab.c" /* yacc.c:1646  */
    break;

  case 410:
#line 3447 "pars.yacc" /* yacc.c:1646  */
    {
	    gotread = TRUE;
	    strcpy(readfile, (yyvsp[0].sval));
	    curtype = (yyvsp[-2].ival);
	    cursource = (yyvsp[-1].ival);
	    xfree((yyvsp[0].sval));
	}
#line 10048 "y.tab.c" /* yacc.c:1646  */
    break;

  case 411:
#line 3454 "pars.yacc" /* yacc.c:1646  */
    {
	    getdata(whichgraph, (yyvsp[0].sval), SOURCE_DISK, LOAD_NXY);
	    xfree((yyvsp[0].sval));
	}
#line 10057 "y.tab.c" /* yacc.c:1646  */
    break;

  case 412:
#line 3458 "pars.yacc" /* yacc.c:1646  */
    {
	    getdata(whichgraph, (yyvsp[0].sval), (yyvsp[-1].ival), LOAD_NXY);
	    xfree((yyvsp[0].sval));
	}
#line 10066 "y.tab.c" /* yacc.c:1646  */
    break;

  case 413:
#line 3462 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!safe_mode) {
                outputset((yyvsp[0].trgt)->gno, (yyvsp[0].trgt)->setno, "stdout", NULL);
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	}
#line 10078 "y.tab.c" /* yacc.c:1646  */
    break;

  case 414:
#line 3469 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!safe_mode) {
	        outputset((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, "stdout", (yyvsp[0].sval));
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree((yyvsp[0].sval));
	}
#line 10091 "y.tab.c" /* yacc.c:1646  */
    break;

  case 415:
#line 3477 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!safe_mode) {
	        outputset((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].sval), NULL);
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree((yyvsp[0].sval));
	}
#line 10104 "y.tab.c" /* yacc.c:1646  */
    break;

  case 416:
#line 3485 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!safe_mode) {
	        outputset((yyvsp[-4].trgt)->gno, (yyvsp[-4].trgt)->setno, (yyvsp[-2].sval), (yyvsp[0].sval));
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
	    xfree((yyvsp[-2].sval));
	    xfree((yyvsp[0].sval));
	}
#line 10118 "y.tab.c" /* yacc.c:1646  */
    break;

  case 417:
#line 3494 "pars.yacc" /* yacc.c:1646  */
    {
            if (!safe_mode) {
                save_project((yyvsp[0].sval));
            } else {
                yyerror("File modifications are disabled in safe mode");
            }
            xfree((yyvsp[0].sval));
        }
#line 10131 "y.tab.c" /* yacc.c:1646  */
    break;

  case 418:
#line 3502 "pars.yacc" /* yacc.c:1646  */
    {
            load_project((yyvsp[0].sval));
            xfree((yyvsp[0].sval));
        }
#line 10140 "y.tab.c" /* yacc.c:1646  */
    break;

  case 419:
#line 3506 "pars.yacc" /* yacc.c:1646  */
    {
            new_project(NULL);
        }
#line 10148 "y.tab.c" /* yacc.c:1646  */
    break;

  case 420:
#line 3509 "pars.yacc" /* yacc.c:1646  */
    {
            new_project((yyvsp[0].sval));
            xfree((yyvsp[0].sval));
        }
#line 10157 "y.tab.c" /* yacc.c:1646  */
    break;

  case 421:
#line 3513 "pars.yacc" /* yacc.c:1646  */
    {
	    push_world();
	}
#line 10165 "y.tab.c" /* yacc.c:1646  */
    break;

  case 422:
#line 3516 "pars.yacc" /* yacc.c:1646  */
    {
	    pop_world();
	}
#line 10173 "y.tab.c" /* yacc.c:1646  */
    break;

  case 423:
#line 3519 "pars.yacc" /* yacc.c:1646  */
    {
	    cycle_world_stack();
	}
#line 10181 "y.tab.c" /* yacc.c:1646  */
    break;

  case 424:
#line 3522 "pars.yacc" /* yacc.c:1646  */
    {
	    if ((yyvsp[0].ival) > 0)
		show_world_stack((yyvsp[0].ival) - 1);
	}
#line 10190 "y.tab.c" /* yacc.c:1646  */
    break;

  case 425:
#line 3526 "pars.yacc" /* yacc.c:1646  */
    {
	    clear_world_stack();
	}
#line 10198 "y.tab.c" /* yacc.c:1646  */
    break;

  case 426:
#line 3529 "pars.yacc" /* yacc.c:1646  */
    {
	    do_clear_boxes();
	}
#line 10206 "y.tab.c" /* yacc.c:1646  */
    break;

  case 427:
#line 3532 "pars.yacc" /* yacc.c:1646  */
    {
	    do_clear_ellipses();
	}
#line 10214 "y.tab.c" /* yacc.c:1646  */
    break;

  case 428:
#line 3535 "pars.yacc" /* yacc.c:1646  */
    {
	    do_clear_lines();
	}
#line 10222 "y.tab.c" /* yacc.c:1646  */
    break;

  case 429:
#line 3538 "pars.yacc" /* yacc.c:1646  */
    {
	    do_clear_text();
	}
#line 10230 "y.tab.c" /* yacc.c:1646  */
    break;

  case 430:
#line 3545 "pars.yacc" /* yacc.c:1646  */
    {
#ifndef NONE_GUI
            set_pagelayout((yyvsp[0].ival));
#endif
        }
#line 10240 "y.tab.c" /* yacc.c:1646  */
    break;

  case 431:
#line 3550 "pars.yacc" /* yacc.c:1646  */
    {
	    auto_redraw = (yyvsp[0].ival);
	}
#line 10248 "y.tab.c" /* yacc.c:1646  */
    break;

  case 432:
#line 3553 "pars.yacc" /* yacc.c:1646  */
    {
	    draw_focus_flag = (yyvsp[0].ival);
	}
#line 10256 "y.tab.c" /* yacc.c:1646  */
    break;

  case 433:
#line 3556 "pars.yacc" /* yacc.c:1646  */
    {
	    focus_policy = FOCUS_SET;
	}
#line 10264 "y.tab.c" /* yacc.c:1646  */
    break;

  case 434:
#line 3559 "pars.yacc" /* yacc.c:1646  */
    {
	    focus_policy = FOCUS_FOLLOWS;
	}
#line 10272 "y.tab.c" /* yacc.c:1646  */
    break;

  case 435:
#line 3562 "pars.yacc" /* yacc.c:1646  */
    {
	    focus_policy = FOCUS_CLICK;
	}
#line 10280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 436:
#line 3569 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10286 "y.tab.c" /* yacc.c:1646  */
    break;

  case 437:
#line 3570 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10292 "y.tab.c" /* yacc.c:1646  */
    break;

  case 438:
#line 3574 "pars.yacc" /* yacc.c:1646  */
    {
	    set_set_hidden((yyvsp[-1].trgt)->gno, (yyvsp[-1].trgt)->setno, !(yyvsp[0].ival));
	}
#line 10300 "y.tab.c" /* yacc.c:1646  */
    break;

  case 439:
#line 3577 "pars.yacc" /* yacc.c:1646  */
    {
	    set_dataset_type((yyvsp[-2].trgt)->gno, (yyvsp[-2].trgt)->setno, (yyvsp[0].ival));
	}
#line 10308 "y.tab.c" /* yacc.c:1646  */
    break;

  case 440:
#line 3581 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].sym = (yyvsp[0].ival);
	}
#line 10316 "y.tab.c" /* yacc.c:1646  */
    break;

  case 441:
#line 3584 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].sympen.color = (yyvsp[0].ival);
	}
#line 10324 "y.tab.c" /* yacc.c:1646  */
    break;

  case 442:
#line 3587 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].sympen.pattern = (yyvsp[0].ival);
	}
#line 10332 "y.tab.c" /* yacc.c:1646  */
    break;

  case 443:
#line 3590 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].symlinew = (yyvsp[0].dval);
	}
#line 10340 "y.tab.c" /* yacc.c:1646  */
    break;

  case 444:
#line 3593 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].symlines = (yyvsp[0].ival);
	}
#line 10348 "y.tab.c" /* yacc.c:1646  */
    break;

  case 445:
#line 3596 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symfillpen.color = (yyvsp[0].ival);
	}
#line 10356 "y.tab.c" /* yacc.c:1646  */
    break;

  case 446:
#line 3599 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symfillpen.pattern = (yyvsp[0].ival);
	}
#line 10364 "y.tab.c" /* yacc.c:1646  */
    break;

  case 447:
#line 3602 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symsize = (yyvsp[0].dval);
	}
#line 10372 "y.tab.c" /* yacc.c:1646  */
    break;

  case 448:
#line 3605 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symchar = (yyvsp[0].ival);
	}
#line 10380 "y.tab.c" /* yacc.c:1646  */
    break;

  case 449:
#line 3608 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].charfont = (yyvsp[0].ival);
	}
#line 10388 "y.tab.c" /* yacc.c:1646  */
    break;

  case 450:
#line 3611 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symskip = (yyvsp[0].ival);
	}
#line 10396 "y.tab.c" /* yacc.c:1646  */
    break;

  case 451:
#line 3616 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].linet = (yyvsp[0].ival);
	}
#line 10404 "y.tab.c" /* yacc.c:1646  */
    break;

  case 452:
#line 3620 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].lines = (yyvsp[0].ival);
	}
#line 10412 "y.tab.c" /* yacc.c:1646  */
    break;

  case 453:
#line 3624 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].linew = (yyvsp[0].dval);
	}
#line 10420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 454:
#line 3628 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].linepen.color = (yyvsp[0].ival);
	}
#line 10428 "y.tab.c" /* yacc.c:1646  */
    break;

  case 455:
#line 3632 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].linepen.pattern = (yyvsp[0].ival);
	}
#line 10436 "y.tab.c" /* yacc.c:1646  */
    break;

  case 456:
#line 3637 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].filltype = (yyvsp[0].ival);
	}
#line 10444 "y.tab.c" /* yacc.c:1646  */
    break;

  case 457:
#line 3641 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].fillrule = (yyvsp[0].ival);
	}
#line 10452 "y.tab.c" /* yacc.c:1646  */
    break;

  case 458:
#line 3645 "pars.yacc" /* yacc.c:1646  */
    {
	    int prop = (yyvsp[0].ival);

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
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].setfillpen.color = prop;
	}
#line 10474 "y.tab.c" /* yacc.c:1646  */
    break;

  case 459:
#line 3663 "pars.yacc" /* yacc.c:1646  */
    {
	    int prop = (yyvsp[0].ival);

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
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].setfillpen.pattern = prop;
	}
#line 10496 "y.tab.c" /* yacc.c:1646  */
    break;

  case 460:
#line 3683 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].baseline = (yyvsp[0].ival);
	}
#line 10504 "y.tab.c" /* yacc.c:1646  */
    break;

  case 461:
#line 3687 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].baseline_type = (yyvsp[0].ival);
	}
#line 10512 "y.tab.c" /* yacc.c:1646  */
    break;

  case 462:
#line 3692 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].dropline = (yyvsp[0].ival);
	}
#line 10520 "y.tab.c" /* yacc.c:1646  */
    break;

  case 463:
#line 3697 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].avalue.active = (yyvsp[0].ival);
	}
#line 10528 "y.tab.c" /* yacc.c:1646  */
    break;

  case 464:
#line 3701 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].avalue.type = (yyvsp[0].ival);
	}
#line 10536 "y.tab.c" /* yacc.c:1646  */
    break;

  case 465:
#line 3705 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-4].trgt)->gno].p[(yyvsp[-4].trgt)->setno].avalue.size = (yyvsp[0].dval);
	}
#line 10544 "y.tab.c" /* yacc.c:1646  */
    break;

  case 466:
#line 3709 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].avalue.font = (yyvsp[0].ival);
	}
#line 10552 "y.tab.c" /* yacc.c:1646  */
    break;

  case 467:
#line 3713 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].avalue.color = (yyvsp[0].ival);
	}
#line 10560 "y.tab.c" /* yacc.c:1646  */
    break;

  case 468:
#line 3717 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].avalue.angle = (yyvsp[0].ival);
	}
#line 10568 "y.tab.c" /* yacc.c:1646  */
    break;

  case 469:
#line 3721 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].avalue.format = (yyvsp[0].ival);
	}
#line 10576 "y.tab.c" /* yacc.c:1646  */
    break;

  case 470:
#line 3725 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].avalue.prec = (yyvsp[0].ival);
	}
#line 10584 "y.tab.c" /* yacc.c:1646  */
    break;

  case 471:
#line 3728 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-5].trgt)->gno].p[(yyvsp[-5].trgt)->setno].avalue.offset.x = (yyvsp[-2].dval);
	    g[(yyvsp[-5].trgt)->gno].p[(yyvsp[-5].trgt)->setno].avalue.offset.y = (yyvsp[0].dval);
	}
#line 10593 "y.tab.c" /* yacc.c:1646  */
    break;

  case 472:
#line 3733 "pars.yacc" /* yacc.c:1646  */
    {
	    strcpy(g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].avalue.prestr, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 10602 "y.tab.c" /* yacc.c:1646  */
    break;

  case 473:
#line 3738 "pars.yacc" /* yacc.c:1646  */
    {
	    strcpy(g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].avalue.appstr, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 10611 "y.tab.c" /* yacc.c:1646  */
    break;

  case 474:
#line 3743 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].errbar.active = (yyvsp[0].ival);
	}
#line 10619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 475:
#line 3746 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].errbar.ptype = (yyvsp[0].ival);
	}
#line 10627 "y.tab.c" /* yacc.c:1646  */
    break;

  case 476:
#line 3749 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].errbar.pen.color = (yyvsp[0].ival);
	}
#line 10635 "y.tab.c" /* yacc.c:1646  */
    break;

  case 477:
#line 3752 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].errbar.pen.pattern = (yyvsp[0].ival);
	}
#line 10643 "y.tab.c" /* yacc.c:1646  */
    break;

  case 478:
#line 3755 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].errbar.barsize = (yyvsp[0].dval);
	}
#line 10651 "y.tab.c" /* yacc.c:1646  */
    break;

  case 479:
#line 3758 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].errbar.linew = (yyvsp[0].dval);
	}
#line 10659 "y.tab.c" /* yacc.c:1646  */
    break;

  case 480:
#line 3761 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].errbar.lines = (yyvsp[0].ival);
	}
#line 10667 "y.tab.c" /* yacc.c:1646  */
    break;

  case 481:
#line 3764 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].errbar.riser_linew = (yyvsp[0].dval);
	}
#line 10675 "y.tab.c" /* yacc.c:1646  */
    break;

  case 482:
#line 3767 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].errbar.riser_lines = (yyvsp[0].ival);
	}
#line 10683 "y.tab.c" /* yacc.c:1646  */
    break;

  case 483:
#line 3770 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-4].trgt)->gno].p[(yyvsp[-4].trgt)->setno].errbar.arrow_clip = (yyvsp[0].ival);
	}
#line 10691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 484:
#line 3773 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-5].trgt)->gno].p[(yyvsp[-5].trgt)->setno].errbar.cliplen = (yyvsp[0].dval);
	}
#line 10699 "y.tab.c" /* yacc.c:1646  */
    break;

  case 485:
#line 3777 "pars.yacc" /* yacc.c:1646  */
    {
	    strncpy(g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].comments, (yyvsp[0].sval), MAX_STRING_LENGTH - 1);
	    xfree((yyvsp[0].sval));
	}
#line 10708 "y.tab.c" /* yacc.c:1646  */
    break;

  case 486:
#line 3782 "pars.yacc" /* yacc.c:1646  */
    {
	    strncpy(g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].lstr, (yyvsp[0].sval), MAX_STRING_LENGTH - 1);
	    xfree((yyvsp[0].sval));
	}
#line 10717 "y.tab.c" /* yacc.c:1646  */
    break;

  case 487:
#line 3790 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->active = (yyvsp[0].ival);
	}
#line 10729 "y.tab.c" /* yacc.c:1646  */
    break;

  case 488:
#line 3797 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->zero = (yyvsp[0].ival);
	}
#line 10741 "y.tab.c" /* yacc.c:1646  */
    break;

  case 489:
#line 3804 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10747 "y.tab.c" /* yacc.c:1646  */
    break;

  case 490:
#line 3805 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10753 "y.tab.c" /* yacc.c:1646  */
    break;

  case 491:
#line 3806 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10759 "y.tab.c" /* yacc.c:1646  */
    break;

  case 492:
#line 3807 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10765 "y.tab.c" /* yacc.c:1646  */
    break;

  case 493:
#line 3808 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10771 "y.tab.c" /* yacc.c:1646  */
    break;

  case 494:
#line 3809 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10777 "y.tab.c" /* yacc.c:1646  */
    break;

  case 495:
#line 3810 "pars.yacc" /* yacc.c:1646  */
    {}
#line 10783 "y.tab.c" /* yacc.c:1646  */
    break;

  case 496:
#line 3811 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->offsx = (yyvsp[-2].dval);
	    g[whichgraph].t[naxis]->offsy = (yyvsp[0].dval);
	}
#line 10796 "y.tab.c" /* yacc.c:1646  */
    break;

  case 497:
#line 3822 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_flag = (yyvsp[0].ival);
	}
#line 10808 "y.tab.c" /* yacc.c:1646  */
    break;

  case 498:
#line 3829 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->tmajor = (yyvsp[0].dval);
	}
#line 10820 "y.tab.c" /* yacc.c:1646  */
    break;

  case 499:
#line 3836 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->nminor = (yyvsp[0].ival);
	}
#line 10832 "y.tab.c" /* yacc.c:1646  */
    break;

  case 500:
#line 3843 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_round = (yyvsp[0].ival);
	}
#line 10844 "y.tab.c" /* yacc.c:1646  */
    break;

  case 501:
#line 3851 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->offsx = (yyvsp[0].dval);
	}
#line 10856 "y.tab.c" /* yacc.c:1646  */
    break;

  case 502:
#line 3858 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->offsy = (yyvsp[0].dval);
	}
#line 10868 "y.tab.c" /* yacc.c:1646  */
    break;

  case 503:
#line 3865 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_autonum = (yyvsp[0].ival);
	}
#line 10880 "y.tab.c" /* yacc.c:1646  */
    break;

  case 504:
#line 3872 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_inout = (yyvsp[0].ival);
	}
#line 10892 "y.tab.c" /* yacc.c:1646  */
    break;

  case 505:
#line 3879 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.size = (yyvsp[0].dval);
	}
#line 10904 "y.tab.c" /* yacc.c:1646  */
    break;

  case 506:
#line 3886 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.size = (yyvsp[0].dval);
	}
#line 10916 "y.tab.c" /* yacc.c:1646  */
    break;

  case 507:
#line 3893 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.color = g[whichgraph].t[naxis]->mprops.color = (yyvsp[0].ival);
	}
#line 10928 "y.tab.c" /* yacc.c:1646  */
    break;

  case 508:
#line 3900 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.color = (yyvsp[0].ival);
	}
#line 10940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 509:
#line 3907 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.color = (yyvsp[0].ival);
	}
#line 10952 "y.tab.c" /* yacc.c:1646  */
    break;

  case 510:
#line 3914 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.linew = g[whichgraph].t[naxis]->mprops.linew = (yyvsp[0].dval);
	}
#line 10964 "y.tab.c" /* yacc.c:1646  */
    break;

  case 511:
#line 3921 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.linew = (yyvsp[0].dval);
	}
#line 10976 "y.tab.c" /* yacc.c:1646  */
    break;

  case 512:
#line 3928 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.linew = (yyvsp[0].dval);
	}
#line 10988 "y.tab.c" /* yacc.c:1646  */
    break;

  case 513:
#line 3935 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.lines = (yyvsp[0].ival);
	}
#line 11000 "y.tab.c" /* yacc.c:1646  */
    break;

  case 514:
#line 3942 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.lines = (yyvsp[0].ival);
	}
#line 11012 "y.tab.c" /* yacc.c:1646  */
    break;

  case 515:
#line 3949 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.gridflag = (yyvsp[0].ival);
	}
#line 11024 "y.tab.c" /* yacc.c:1646  */
    break;

  case 516:
#line 3956 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->mprops.gridflag = (yyvsp[0].ival);
	}
#line 11036 "y.tab.c" /* yacc.c:1646  */
    break;

  case 517:
#line 3963 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_op = (yyvsp[0].ival);
	}
#line 11048 "y.tab.c" /* yacc.c:1646  */
    break;

  case 518:
#line 3970 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_spec = (yyvsp[0].ival);
	}
#line 11060 "y.tab.c" /* yacc.c:1646  */
    break;

  case 519:
#line 3977 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->nticks = (yyvsp[0].ival);
	}
#line 11072 "y.tab.c" /* yacc.c:1646  */
    break;

  case 520:
#line 3984 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].wtpos = (yyvsp[0].dval);
	    g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].type = TICK_TYPE_MAJOR;
	}
#line 11085 "y.tab.c" /* yacc.c:1646  */
    break;

  case 521:
#line 3992 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].wtpos = (yyvsp[0].dval);
	    g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].type = TICK_TYPE_MINOR;
	}
#line 11098 "y.tab.c" /* yacc.c:1646  */
    break;

  case 522:
#line 4003 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_flag = (yyvsp[0].ival);
	}
#line 11110 "y.tab.c" /* yacc.c:1646  */
    break;

  case 523:
#line 4010 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_prec = (yyvsp[0].ival);
	}
#line 11122 "y.tab.c" /* yacc.c:1646  */
    break;

  case 524:
#line 4017 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_format = (yyvsp[0].ival);
	}
#line 11134 "y.tab.c" /* yacc.c:1646  */
    break;

  case 525:
#line 4024 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_format = (yyvsp[0].dval);
	}
#line 11146 "y.tab.c" /* yacc.c:1646  */
    break;

  case 526:
#line 4031 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    strcpy(g[whichgraph].t[naxis]->tl_appstr, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 11159 "y.tab.c" /* yacc.c:1646  */
    break;

  case 527:
#line 4039 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    strcpy(g[whichgraph].t[naxis]->tl_prestr, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 11172 "y.tab.c" /* yacc.c:1646  */
    break;

  case 528:
#line 4047 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_angle = (yyvsp[0].ival);
	}
#line 11184 "y.tab.c" /* yacc.c:1646  */
    break;

  case 529:
#line 4054 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_skip = (yyvsp[0].ival);
	}
#line 11196 "y.tab.c" /* yacc.c:1646  */
    break;

  case 530:
#line 4061 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_staggered = (yyvsp[0].ival);
	}
#line 11208 "y.tab.c" /* yacc.c:1646  */
    break;

  case 531:
#line 4068 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_op = (yyvsp[0].ival);
	}
#line 11220 "y.tab.c" /* yacc.c:1646  */
    break;

  case 532:
#line 4075 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
            g[whichgraph].t[naxis]->tl_formula =
                copy_string(g[whichgraph].t[naxis]->tl_formula, (yyvsp[0].sval));
            xfree((yyvsp[0].sval));
	}
#line 11234 "y.tab.c" /* yacc.c:1646  */
    break;

  case 533:
#line 4084 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_start = (yyvsp[0].dval);
	}
#line 11246 "y.tab.c" /* yacc.c:1646  */
    break;

  case 534:
#line 4091 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_stop = (yyvsp[0].dval);
	}
#line 11258 "y.tab.c" /* yacc.c:1646  */
    break;

  case 535:
#line 4098 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_starttype = TYPE_SPEC;
	}
#line 11270 "y.tab.c" /* yacc.c:1646  */
    break;

  case 536:
#line 4105 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_starttype = TYPE_AUTO;
	}
#line 11282 "y.tab.c" /* yacc.c:1646  */
    break;

  case 537:
#line 4112 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_stoptype = TYPE_SPEC;
	}
#line 11294 "y.tab.c" /* yacc.c:1646  */
    break;

  case 538:
#line 4119 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_stoptype = TYPE_AUTO;
	}
#line 11306 "y.tab.c" /* yacc.c:1646  */
    break;

  case 539:
#line 4126 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_charsize = (yyvsp[0].dval);
	}
#line 11318 "y.tab.c" /* yacc.c:1646  */
    break;

  case 540:
#line 4133 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_font = (yyvsp[0].ival);
	}
#line 11330 "y.tab.c" /* yacc.c:1646  */
    break;

  case 541:
#line 4140 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_color = (yyvsp[0].ival);
	}
#line 11342 "y.tab.c" /* yacc.c:1646  */
    break;

  case 542:
#line 4147 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                xfree((yyvsp[0].sval));
                return 1;
            }
	    if ((yyvsp[-2].ival) >= MAX_TICKS) {
	         yyerror("Number of ticks exceeds maximum");
	         xfree((yyvsp[0].sval));
	         return 1;
	    }
	    g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].label = 
                copy_string(g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].label, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 11362 "y.tab.c" /* yacc.c:1646  */
    break;

  case 543:
#line 4162 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_gaptype = TYPE_AUTO;
	}
#line 11374 "y.tab.c" /* yacc.c:1646  */
    break;

  case 544:
#line 4169 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_gaptype = TYPE_SPEC;
	}
#line 11386 "y.tab.c" /* yacc.c:1646  */
    break;

  case 545:
#line 4176 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_gap.x = (yyvsp[-2].dval);
	    g[whichgraph].t[naxis]->tl_gap.y = (yyvsp[0].dval);
	}
#line 11399 "y.tab.c" /* yacc.c:1646  */
    break;

  case 546:
#line 4187 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    set_plotstr_string(&g[whichgraph].t[naxis]->label, (yyvsp[0].sval));
	    xfree((yyvsp[0].sval));
	}
#line 11412 "y.tab.c" /* yacc.c:1646  */
    break;

  case 547:
#line 4195 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_layout = LAYOUT_PERPENDICULAR;
	}
#line 11424 "y.tab.c" /* yacc.c:1646  */
    break;

  case 548:
#line 4202 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_layout = LAYOUT_PARALLEL;
	}
#line 11436 "y.tab.c" /* yacc.c:1646  */
    break;

  case 549:
#line 4209 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_place = TYPE_AUTO;
	}
#line 11448 "y.tab.c" /* yacc.c:1646  */
    break;

  case 550:
#line 4216 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_place = TYPE_SPEC;
	}
#line 11460 "y.tab.c" /* yacc.c:1646  */
    break;

  case 551:
#line 4223 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.x = (yyvsp[-2].dval);
	    g[whichgraph].t[naxis]->label.y = (yyvsp[0].dval);
	}
#line 11473 "y.tab.c" /* yacc.c:1646  */
    break;

  case 552:
#line 4231 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.just = (yyvsp[0].ival);
	}
#line 11485 "y.tab.c" /* yacc.c:1646  */
    break;

  case 553:
#line 4238 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.charsize = (yyvsp[0].dval);
	}
#line 11497 "y.tab.c" /* yacc.c:1646  */
    break;

  case 554:
#line 4245 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.font = (yyvsp[0].ival);
	}
#line 11509 "y.tab.c" /* yacc.c:1646  */
    break;

  case 555:
#line 4252 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label.color = (yyvsp[0].ival);
	}
#line 11521 "y.tab.c" /* yacc.c:1646  */
    break;

  case 556:
#line 4259 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_op = (yyvsp[0].ival);
	}
#line 11533 "y.tab.c" /* yacc.c:1646  */
    break;

  case 557:
#line 4269 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbar = (yyvsp[0].ival);
	}
#line 11545 "y.tab.c" /* yacc.c:1646  */
    break;

  case 558:
#line 4276 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbarcolor = (yyvsp[0].ival);
	}
#line 11557 "y.tab.c" /* yacc.c:1646  */
    break;

  case 559:
#line 4283 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbarlines = (yyvsp[0].ival);
	}
#line 11569 "y.tab.c" /* yacc.c:1646  */
    break;

  case 560:
#line 4290 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_drawbarlinew = (yyvsp[0].dval);
	}
#line 11581 "y.tab.c" /* yacc.c:1646  */
    break;

  case 561:
#line 4300 "pars.yacc" /* yacc.c:1646  */
    { 
          nonl_opts.title = copy_string(nonl_opts.title, (yyvsp[0].sval));
	  xfree((yyvsp[0].sval));
        }
#line 11590 "y.tab.c" /* yacc.c:1646  */
    break;

  case 562:
#line 4304 "pars.yacc" /* yacc.c:1646  */
    { 
          nonl_opts.formula = copy_string(nonl_opts.formula, (yyvsp[0].sval));
	  xfree((yyvsp[0].sval));
        }
#line 11599 "y.tab.c" /* yacc.c:1646  */
    break;

  case 563:
#line 4308 "pars.yacc" /* yacc.c:1646  */
    { 
            nonl_opts.parnum = (yyvsp[-1].ival); 
        }
#line 11607 "y.tab.c" /* yacc.c:1646  */
    break;

  case 564:
#line 4311 "pars.yacc" /* yacc.c:1646  */
    { 
            nonl_opts.tolerance = (yyvsp[0].dval); 
        }
#line 11615 "y.tab.c" /* yacc.c:1646  */
    break;

  case 565:
#line 4318 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.ival) = (yyvsp[0].ival);
        }
#line 11623 "y.tab.c" /* yacc.c:1646  */
    break;

  case 566:
#line 4322 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.ival) = (yyvsp[0].ival);
        }
#line 11631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 567:
#line 4329 "pars.yacc" /* yacc.c:1646  */
    {
	    int gno = (yyvsp[-2].ival), setno = (yyvsp[0].ival);
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                (yyval.trgt) = &trgt_pool[tgtn];
                (yyval.trgt)->gno   = gno;
                (yyval.trgt)->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
#line 11648 "y.tab.c" /* yacc.c:1646  */
    break;

  case 568:
#line 4342 "pars.yacc" /* yacc.c:1646  */
    {
	    int gno = (yyvsp[-3].ival), setno = (yyvsp[0].ival);
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                (yyval.trgt) = &trgt_pool[tgtn];
                (yyval.trgt)->gno   = gno;
                (yyval.trgt)->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
#line 11665 "y.tab.c" /* yacc.c:1646  */
    break;

  case 569:
#line 4355 "pars.yacc" /* yacc.c:1646  */
    {
	    int gno = whichgraph, setno = (yyvsp[0].ival);
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                (yyval.trgt) = &trgt_pool[tgtn];
                (yyval.trgt)->gno   = gno;
                (yyval.trgt)->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
#line 11682 "y.tab.c" /* yacc.c:1646  */
    break;

  case 570:
#line 4368 "pars.yacc" /* yacc.c:1646  */
    {
	    int gno = whichgraph, setno = (yyvsp[0].ival);
            if (allocate_set(gno, setno) == RETURN_SUCCESS) {
                (yyval.trgt) = &trgt_pool[tgtn];
                (yyval.trgt)->gno   = gno;
                (yyval.trgt)->setno = setno;
                tgtn++;
            } else {
                errmsg("Can't allocate referred set");
                return 1;
            }
	}
#line 11699 "y.tab.c" /* yacc.c:1646  */
    break;

  case 571:
#line 4383 "pars.yacc" /* yacc.c:1646  */
    {}
#line 11705 "y.tab.c" /* yacc.c:1646  */
    break;

  case 572:
#line 4384 "pars.yacc" /* yacc.c:1646  */
    {}
#line 11711 "y.tab.c" /* yacc.c:1646  */
    break;

  case 573:
#line 4388 "pars.yacc" /* yacc.c:1646  */
    { naxis =  X_AXIS; }
#line 11717 "y.tab.c" /* yacc.c:1646  */
    break;

  case 574:
#line 4389 "pars.yacc" /* yacc.c:1646  */
    { naxis = Y_AXIS; }
#line 11723 "y.tab.c" /* yacc.c:1646  */
    break;

  case 575:
#line 4390 "pars.yacc" /* yacc.c:1646  */
    { naxis = ZX_AXIS; }
#line 11729 "y.tab.c" /* yacc.c:1646  */
    break;

  case 576:
#line 4391 "pars.yacc" /* yacc.c:1646  */
    { naxis = ZY_AXIS; }
#line 11735 "y.tab.c" /* yacc.c:1646  */
    break;

  case 577:
#line 4395 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = CONSTANT;  }
#line 11741 "y.tab.c" /* yacc.c:1646  */
    break;

  case 578:
#line 4396 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = UCONSTANT; }
#line 11747 "y.tab.c" /* yacc.c:1646  */
    break;

  case 579:
#line 4397 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_I;    }
#line 11753 "y.tab.c" /* yacc.c:1646  */
    break;

  case 580:
#line 4398 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_D;    }
#line 11759 "y.tab.c" /* yacc.c:1646  */
    break;

  case 581:
#line 4399 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_ND;   }
#line 11765 "y.tab.c" /* yacc.c:1646  */
    break;

  case 582:
#line 4400 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_NN;   }
#line 11771 "y.tab.c" /* yacc.c:1646  */
    break;

  case 583:
#line 4401 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_DD;   }
#line 11777 "y.tab.c" /* yacc.c:1646  */
    break;

  case 584:
#line 4402 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_NND;  }
#line 11783 "y.tab.c" /* yacc.c:1646  */
    break;

  case 585:
#line 4403 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_PPD;  }
#line 11789 "y.tab.c" /* yacc.c:1646  */
    break;

  case 586:
#line 4404 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_PPPD; }
#line 11795 "y.tab.c" /* yacc.c:1646  */
    break;

  case 587:
#line 4405 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_PPPPD; }
#line 11801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 588:
#line 4406 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FUNC_PPPPPD; }
#line 11807 "y.tab.c" /* yacc.c:1646  */
    break;

  case 589:
#line 4410 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) =  TICKS_SPEC_NONE; }
#line 11813 "y.tab.c" /* yacc.c:1646  */
    break;

  case 590:
#line 4411 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = TICKS_SPEC_MARKS; }
#line 11819 "y.tab.c" /* yacc.c:1646  */
    break;

  case 591:
#line 4412 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = TICKS_SPEC_BOTH; }
#line 11825 "y.tab.c" /* yacc.c:1646  */
    break;

  case 592:
#line 4416 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FILTER_INPUT; }
#line 11831 "y.tab.c" /* yacc.c:1646  */
    break;

  case 593:
#line 4417 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FILTER_OUTPUT; }
#line 11837 "y.tab.c" /* yacc.c:1646  */
    break;

  case 594:
#line 4421 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FILTER_MAGIC; }
#line 11843 "y.tab.c" /* yacc.c:1646  */
    break;

  case 595:
#line 4422 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FILTER_PATTERN; }
#line 11849 "y.tab.c" /* yacc.c:1646  */
    break;

  case 596:
#line 4426 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XY; }
#line 11855 "y.tab.c" /* yacc.c:1646  */
    break;

  case 597:
#line 4427 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_BAR; }
#line 11861 "y.tab.c" /* yacc.c:1646  */
    break;

  case 598:
#line 4428 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_BARDY; }
#line 11867 "y.tab.c" /* yacc.c:1646  */
    break;

  case 599:
#line 4429 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_BARDYDY; }
#line 11873 "y.tab.c" /* yacc.c:1646  */
    break;

  case 600:
#line 4430 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYZ; }
#line 11879 "y.tab.c" /* yacc.c:1646  */
    break;

  case 601:
#line 4431 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYDX; }
#line 11885 "y.tab.c" /* yacc.c:1646  */
    break;

  case 602:
#line 4432 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYDY; }
#line 11891 "y.tab.c" /* yacc.c:1646  */
    break;

  case 603:
#line 4433 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYDXDX; }
#line 11897 "y.tab.c" /* yacc.c:1646  */
    break;

  case 604:
#line 4434 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYDYDY; }
#line 11903 "y.tab.c" /* yacc.c:1646  */
    break;

  case 605:
#line 4435 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYDXDY; }
#line 11909 "y.tab.c" /* yacc.c:1646  */
    break;

  case 606:
#line 4436 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYDXDXDYDY; }
#line 11915 "y.tab.c" /* yacc.c:1646  */
    break;

  case 607:
#line 4437 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYHILO; }
#line 11921 "y.tab.c" /* yacc.c:1646  */
    break;

  case 608:
#line 4438 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYR; }
#line 11927 "y.tab.c" /* yacc.c:1646  */
    break;

  case 609:
#line 4439 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYSIZE; }
#line 11933 "y.tab.c" /* yacc.c:1646  */
    break;

  case 610:
#line 4440 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYCOLOR; }
#line 11939 "y.tab.c" /* yacc.c:1646  */
    break;

  case 611:
#line 4441 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYCOLPAT; }
#line 11945 "y.tab.c" /* yacc.c:1646  */
    break;

  case 612:
#line 4442 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XYVMAP; }
#line 11951 "y.tab.c" /* yacc.c:1646  */
    break;

  case 613:
#line 4443 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_BOXPLOT; }
#line 11957 "y.tab.c" /* yacc.c:1646  */
    break;

  case 614:
#line 4444 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SET_XY; }
#line 11963 "y.tab.c" /* yacc.c:1646  */
    break;

  case 615:
#line 4448 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = GRAPH_XY; }
#line 11969 "y.tab.c" /* yacc.c:1646  */
    break;

  case 616:
#line 4449 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = GRAPH_CHART; }
#line 11975 "y.tab.c" /* yacc.c:1646  */
    break;

  case 617:
#line 4450 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = GRAPH_POLAR; }
#line 11981 "y.tab.c" /* yacc.c:1646  */
    break;

  case 618:
#line 4451 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = GRAPH_SMITH; }
#line 11987 "y.tab.c" /* yacc.c:1646  */
    break;

  case 619:
#line 4452 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = GRAPH_FIXED; }
#line 11993 "y.tab.c" /* yacc.c:1646  */
    break;

  case 620:
#line 4453 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = GRAPH_PIE;   }
#line 11999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 621:
#line 4457 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PAGE_FREE; }
#line 12005 "y.tab.c" /* yacc.c:1646  */
    break;

  case 622:
#line 4458 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PAGE_FIXED; }
#line 12011 "y.tab.c" /* yacc.c:1646  */
    break;

  case 623:
#line 4462 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PAGE_ORIENT_LANDSCAPE; }
#line 12017 "y.tab.c" /* yacc.c:1646  */
    break;

  case 624:
#line 4463 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PAGE_ORIENT_PORTRAIT;  }
#line 12023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 625:
#line 4467 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_ABOVE; }
#line 12029 "y.tab.c" /* yacc.c:1646  */
    break;

  case 626:
#line 4468 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_BELOW; }
#line 12035 "y.tab.c" /* yacc.c:1646  */
    break;

  case 627:
#line 4469 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_TOLEFT; }
#line 12041 "y.tab.c" /* yacc.c:1646  */
    break;

  case 628:
#line 4470 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_TORIGHT; }
#line 12047 "y.tab.c" /* yacc.c:1646  */
    break;

  case 629:
#line 4471 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_POLYI; }
#line 12053 "y.tab.c" /* yacc.c:1646  */
    break;

  case 630:
#line 4472 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_POLYO; }
#line 12059 "y.tab.c" /* yacc.c:1646  */
    break;

  case 631:
#line 4473 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_HORIZI; }
#line 12065 "y.tab.c" /* yacc.c:1646  */
    break;

  case 632:
#line 4474 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_VERTI; }
#line 12071 "y.tab.c" /* yacc.c:1646  */
    break;

  case 633:
#line 4475 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_HORIZO; }
#line 12077 "y.tab.c" /* yacc.c:1646  */
    break;

  case 634:
#line 4476 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = REGION_VERTO; }
#line 12083 "y.tab.c" /* yacc.c:1646  */
    break;

  case 635:
#line 4479 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SCALE_NORMAL; }
#line 12089 "y.tab.c" /* yacc.c:1646  */
    break;

  case 636:
#line 4480 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SCALE_LOG; }
#line 12095 "y.tab.c" /* yacc.c:1646  */
    break;

  case 637:
#line 4481 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SCALE_REC; }
#line 12101 "y.tab.c" /* yacc.c:1646  */
    break;

  case 638:
#line 4482 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SCALE_LOGIT; }
#line 12107 "y.tab.c" /* yacc.c:1646  */
    break;

  case 639:
#line 4485 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = TRUE; }
#line 12113 "y.tab.c" /* yacc.c:1646  */
    break;

  case 640:
#line 4486 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FALSE; }
#line 12119 "y.tab.c" /* yacc.c:1646  */
    break;

  case 641:
#line 4489 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = RUN_AVG; }
#line 12125 "y.tab.c" /* yacc.c:1646  */
    break;

  case 642:
#line 4490 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = RUN_STD; }
#line 12131 "y.tab.c" /* yacc.c:1646  */
    break;

  case 643:
#line 4491 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = RUN_MED; }
#line 12137 "y.tab.c" /* yacc.c:1646  */
    break;

  case 644:
#line 4492 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = RUN_MAX; }
#line 12143 "y.tab.c" /* yacc.c:1646  */
    break;

  case 645:
#line 4493 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = RUN_MIN; }
#line 12149 "y.tab.c" /* yacc.c:1646  */
    break;

  case 646:
#line 4497 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SOURCE_DISK; }
#line 12155 "y.tab.c" /* yacc.c:1646  */
    break;

  case 647:
#line 4498 "pars.yacc" /* yacc.c:1646  */
    {
            if (!safe_mode) {
                (yyval.ival) = SOURCE_PIPE;
            } else {
                yyerror("Pipe inputs are disabled in safe mode");
                (yyval.ival) = SOURCE_DISK;
            }
        }
#line 12168 "y.tab.c" /* yacc.c:1646  */
    break;

  case 648:
#line 4508 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = JUST_RIGHT; }
#line 12174 "y.tab.c" /* yacc.c:1646  */
    break;

  case 649:
#line 4509 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = JUST_LEFT; }
#line 12180 "y.tab.c" /* yacc.c:1646  */
    break;

  case 650:
#line 4510 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = JUST_CENTER; }
#line 12186 "y.tab.c" /* yacc.c:1646  */
    break;

  case 651:
#line 4513 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = TICKS_IN; }
#line 12192 "y.tab.c" /* yacc.c:1646  */
    break;

  case 652:
#line 4514 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = TICKS_OUT; }
#line 12198 "y.tab.c" /* yacc.c:1646  */
    break;

  case 653:
#line 4515 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = TICKS_BOTH; }
#line 12204 "y.tab.c" /* yacc.c:1646  */
    break;

  case 654:
#line 4518 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DECIMAL; }
#line 12210 "y.tab.c" /* yacc.c:1646  */
    break;

  case 655:
#line 4519 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_EXPONENTIAL; }
#line 12216 "y.tab.c" /* yacc.c:1646  */
    break;

  case 656:
#line 4520 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_GENERAL; }
#line 12222 "y.tab.c" /* yacc.c:1646  */
    break;

  case 657:
#line 4521 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_SCIENTIFIC; }
#line 12228 "y.tab.c" /* yacc.c:1646  */
    break;

  case 658:
#line 4522 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_ENGINEERING; }
#line 12234 "y.tab.c" /* yacc.c:1646  */
    break;

  case 659:
#line 4523 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_COMPUTING; }
#line 12240 "y.tab.c" /* yacc.c:1646  */
    break;

  case 660:
#line 4524 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_POWER; }
#line 12246 "y.tab.c" /* yacc.c:1646  */
    break;

  case 661:
#line 4525 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DDMMYY; }
#line 12252 "y.tab.c" /* yacc.c:1646  */
    break;

  case 662:
#line 4526 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MMDDYY; }
#line 12258 "y.tab.c" /* yacc.c:1646  */
    break;

  case 663:
#line 4527 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_YYMMDD; }
#line 12264 "y.tab.c" /* yacc.c:1646  */
    break;

  case 664:
#line 4528 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MMYY; }
#line 12270 "y.tab.c" /* yacc.c:1646  */
    break;

  case 665:
#line 4529 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MMDD; }
#line 12276 "y.tab.c" /* yacc.c:1646  */
    break;

  case 666:
#line 4530 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MONTHDAY; }
#line 12282 "y.tab.c" /* yacc.c:1646  */
    break;

  case 667:
#line 4531 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DAYMONTH; }
#line 12288 "y.tab.c" /* yacc.c:1646  */
    break;

  case 668:
#line 4532 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MONTHS; }
#line 12294 "y.tab.c" /* yacc.c:1646  */
    break;

  case 669:
#line 4533 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MONTHSY; }
#line 12300 "y.tab.c" /* yacc.c:1646  */
    break;

  case 670:
#line 4534 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MONTHL; }
#line 12306 "y.tab.c" /* yacc.c:1646  */
    break;

  case 671:
#line 4535 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DAYOFWEEKS; }
#line 12312 "y.tab.c" /* yacc.c:1646  */
    break;

  case 672:
#line 4536 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DAYOFWEEKL; }
#line 12318 "y.tab.c" /* yacc.c:1646  */
    break;

  case 673:
#line 4537 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DAYOFYEAR; }
#line 12324 "y.tab.c" /* yacc.c:1646  */
    break;

  case 674:
#line 4538 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_HMS; }
#line 12330 "y.tab.c" /* yacc.c:1646  */
    break;

  case 675:
#line 4539 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MMDDHMS; }
#line 12336 "y.tab.c" /* yacc.c:1646  */
    break;

  case 676:
#line 4540 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MMDDYYHMS; }
#line 12342 "y.tab.c" /* yacc.c:1646  */
    break;

  case 677:
#line 4541 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_YYMMDDHMS; }
#line 12348 "y.tab.c" /* yacc.c:1646  */
    break;

  case 678:
#line 4542 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DEGREESLON; }
#line 12354 "y.tab.c" /* yacc.c:1646  */
    break;

  case 679:
#line 4543 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DEGREESMMLON; }
#line 12360 "y.tab.c" /* yacc.c:1646  */
    break;

  case 680:
#line 4544 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DEGREESMMSSLON; }
#line 12366 "y.tab.c" /* yacc.c:1646  */
    break;

  case 681:
#line 4545 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MMSSLON; }
#line 12372 "y.tab.c" /* yacc.c:1646  */
    break;

  case 682:
#line 4546 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DEGREESLAT; }
#line 12378 "y.tab.c" /* yacc.c:1646  */
    break;

  case 683:
#line 4547 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DEGREESMMLAT; }
#line 12384 "y.tab.c" /* yacc.c:1646  */
    break;

  case 684:
#line 4548 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_DEGREESMMSSLAT; }
#line 12390 "y.tab.c" /* yacc.c:1646  */
    break;

  case 685:
#line 4549 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FORMAT_MMSSLAT; }
#line 12396 "y.tab.c" /* yacc.c:1646  */
    break;

  case 686:
#line 4552 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SIGN_NORMAL; }
#line 12402 "y.tab.c" /* yacc.c:1646  */
    break;

  case 687:
#line 4553 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SIGN_ABSOLUTE; }
#line 12408 "y.tab.c" /* yacc.c:1646  */
    break;

  case 688:
#line 4554 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SIGN_NEGATE; }
#line 12414 "y.tab.c" /* yacc.c:1646  */
    break;

  case 689:
#line 4557 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = UP; }
#line 12420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 690:
#line 4558 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DOWN; }
#line 12426 "y.tab.c" /* yacc.c:1646  */
    break;

  case 691:
#line 4559 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = RIGHT; }
#line 12432 "y.tab.c" /* yacc.c:1646  */
    break;

  case 692:
#line 4560 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = LEFT; }
#line 12438 "y.tab.c" /* yacc.c:1646  */
    break;

  case 693:
#line 4561 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = IN; }
#line 12444 "y.tab.c" /* yacc.c:1646  */
    break;

  case 694:
#line 4562 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = OUT; }
#line 12450 "y.tab.c" /* yacc.c:1646  */
    break;

  case 695:
#line 4565 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = COORD_WORLD; }
#line 12456 "y.tab.c" /* yacc.c:1646  */
    break;

  case 696:
#line 4566 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = COORD_VIEW; }
#line 12462 "y.tab.c" /* yacc.c:1646  */
    break;

  case 697:
#line 4569 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_X; }
#line 12468 "y.tab.c" /* yacc.c:1646  */
    break;

  case 698:
#line 4570 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_Y; }
#line 12474 "y.tab.c" /* yacc.c:1646  */
    break;

  case 699:
#line 4571 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_X; }
#line 12480 "y.tab.c" /* yacc.c:1646  */
    break;

  case 700:
#line 4572 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_Y; }
#line 12486 "y.tab.c" /* yacc.c:1646  */
    break;

  case 701:
#line 4573 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_Y1; }
#line 12492 "y.tab.c" /* yacc.c:1646  */
    break;

  case 702:
#line 4574 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_Y2; }
#line 12498 "y.tab.c" /* yacc.c:1646  */
    break;

  case 703:
#line 4575 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_Y3; }
#line 12504 "y.tab.c" /* yacc.c:1646  */
    break;

  case 704:
#line 4576 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_Y4; }
#line 12510 "y.tab.c" /* yacc.c:1646  */
    break;

  case 705:
#line 4579 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = ASCENDING; }
#line 12516 "y.tab.c" /* yacc.c:1646  */
    break;

  case 706:
#line 4580 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DESCENDING; }
#line 12522 "y.tab.c" /* yacc.c:1646  */
    break;

  case 707:
#line 4583 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_X; }
#line 12528 "y.tab.c" /* yacc.c:1646  */
    break;

  case 708:
#line 4584 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = DATA_Y; }
#line 12534 "y.tab.c" /* yacc.c:1646  */
    break;

  case 709:
#line 4587 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FFT_DFT; }
#line 12540 "y.tab.c" /* yacc.c:1646  */
    break;

  case 710:
#line 4588 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FFT_FFT; }
#line 12546 "y.tab.c" /* yacc.c:1646  */
    break;

  case 711:
#line 4589 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FFT_INVDFT; }
#line 12552 "y.tab.c" /* yacc.c:1646  */
    break;

  case 712:
#line 4590 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = FFT_INVFFT; }
#line 12558 "y.tab.c" /* yacc.c:1646  */
    break;

  case 713:
#line 4594 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=0;}
#line 12564 "y.tab.c" /* yacc.c:1646  */
    break;

  case 714:
#line 4595 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=1;}
#line 12570 "y.tab.c" /* yacc.c:1646  */
    break;

  case 715:
#line 4599 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=0;}
#line 12576 "y.tab.c" /* yacc.c:1646  */
    break;

  case 716:
#line 4600 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=1;}
#line 12582 "y.tab.c" /* yacc.c:1646  */
    break;

  case 717:
#line 4601 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=2;}
#line 12588 "y.tab.c" /* yacc.c:1646  */
    break;

  case 718:
#line 4605 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=0;}
#line 12594 "y.tab.c" /* yacc.c:1646  */
    break;

  case 719:
#line 4606 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=1;}
#line 12600 "y.tab.c" /* yacc.c:1646  */
    break;

  case 720:
#line 4607 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=2;}
#line 12606 "y.tab.c" /* yacc.c:1646  */
    break;

  case 721:
#line 4611 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=0;}
#line 12612 "y.tab.c" /* yacc.c:1646  */
    break;

  case 722:
#line 4612 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=1;}
#line 12618 "y.tab.c" /* yacc.c:1646  */
    break;

  case 723:
#line 4613 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=2;}
#line 12624 "y.tab.c" /* yacc.c:1646  */
    break;

  case 724:
#line 4614 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=3;}
#line 12630 "y.tab.c" /* yacc.c:1646  */
    break;

  case 725:
#line 4615 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=4;}
#line 12636 "y.tab.c" /* yacc.c:1646  */
    break;

  case 726:
#line 4616 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=5;}
#line 12642 "y.tab.c" /* yacc.c:1646  */
    break;

  case 727:
#line 4617 "pars.yacc" /* yacc.c:1646  */
    {(yyval.ival)=6;}
#line 12648 "y.tab.c" /* yacc.c:1646  */
    break;

  case 728:
#line 4621 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = INTERP_LINEAR; }
#line 12654 "y.tab.c" /* yacc.c:1646  */
    break;

  case 729:
#line 4622 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = INTERP_SPLINE; }
#line 12660 "y.tab.c" /* yacc.c:1646  */
    break;

  case 730:
#line 4623 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = INTERP_ASPLINE; }
#line 12666 "y.tab.c" /* yacc.c:1646  */
    break;

  case 731:
#line 4626 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = MINP; }
#line 12672 "y.tab.c" /* yacc.c:1646  */
    break;

  case 732:
#line 4627 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = MAXP; }
#line 12678 "y.tab.c" /* yacc.c:1646  */
    break;

  case 733:
#line 4628 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = AVG; }
#line 12684 "y.tab.c" /* yacc.c:1646  */
    break;

  case 734:
#line 4629 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SD; }
#line 12690 "y.tab.c" /* yacc.c:1646  */
    break;

  case 735:
#line 4630 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = SUM; }
#line 12696 "y.tab.c" /* yacc.c:1646  */
    break;

  case 736:
#line 4631 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = IMIN; }
#line 12702 "y.tab.c" /* yacc.c:1646  */
    break;

  case 737:
#line 4632 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = IMAX; }
#line 12708 "y.tab.c" /* yacc.c:1646  */
    break;

  case 738:
#line 4637 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.ival) = get_mapped_font((yyvsp[0].ival));
        }
#line 12716 "y.tab.c" /* yacc.c:1646  */
    break;

  case 739:
#line 4641 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.ival) = get_font_by_name((yyvsp[0].sval));
            xfree((yyvsp[0].sval));
        }
#line 12725 "y.tab.c" /* yacc.c:1646  */
    break;

  case 740:
#line 4649 "pars.yacc" /* yacc.c:1646  */
    {
	    int lines = (yyvsp[0].ival);
            if (lines >= 0 && lines < number_of_linestyles()) {
	        (yyval.ival) = lines;
	    } else {
	        errmsg("invalid linestyle");
	        (yyval.ival) = 1;
	    }
        }
#line 12739 "y.tab.c" /* yacc.c:1646  */
    break;

  case 741:
#line 4662 "pars.yacc" /* yacc.c:1646  */
    {
	    int patno = (yyvsp[0].ival);
            if (patno >= 0 && patno < number_of_patterns()) {
	        (yyval.ival) = patno;
	    } else {
	        errmsg("invalid pattern number");
	        (yyval.ival) = 1;
	    }
        }
#line 12753 "y.tab.c" /* yacc.c:1646  */
    break;

  case 742:
#line 4675 "pars.yacc" /* yacc.c:1646  */
    {
            int c = (yyvsp[0].ival);
            if (c >= 0 && c < number_of_colors()) {
                (yyval.ival) = c;
            } else {
                errmsg("Invalid color ID");
                (yyval.ival) = 1;
            }
        }
#line 12767 "y.tab.c" /* yacc.c:1646  */
    break;

  case 743:
#line 4685 "pars.yacc" /* yacc.c:1646  */
    {
            int c = get_color_by_name((yyvsp[0].sval));
            if (c == BAD_COLOR) {
                errmsg("Invalid color name");
                c = 1;
            }
            xfree((yyvsp[0].sval));
            (yyval.ival) = c;
        }
#line 12781 "y.tab.c" /* yacc.c:1646  */
    break;

  case 744:
#line 4695 "pars.yacc" /* yacc.c:1646  */
    {
            int c;
            CMap_entry cmap;
            cmap.rgb.red = (yyvsp[-5].ival);
            cmap.rgb.green = (yyvsp[-3].ival);
            cmap.rgb.blue = (yyvsp[-1].ival);
            cmap.ctype = COLOR_MAIN;
            cmap.cname = NULL;
            c = add_color(cmap);
            if (c == BAD_COLOR) {
                errmsg("Can't allocate requested color");
                c = 1;
            }
            (yyval.ival) = c;
        }
#line 12801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 745:
#line 4714 "pars.yacc" /* yacc.c:1646  */
    {
            double linew;
            linew = (yyvsp[0].dval);
            if (linew < 0.0) {
                yyerror("Negative linewidth");
                linew = 0.0;
            } else if (linew > MAX_LINEWIDTH) {
                yyerror("Linewidth too large");
                linew = MAX_LINEWIDTH;
            }
            (yyval.dval) = linew;
        }
#line 12818 "y.tab.c" /* yacc.c:1646  */
    break;

  case 746:
#line 4729 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.ival) = (yyvsp[0].ival);
        }
#line 12826 "y.tab.c" /* yacc.c:1646  */
    break;

  case 747:
#line 4734 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_NORMAL; }
#line 12832 "y.tab.c" /* yacc.c:1646  */
    break;

  case 748:
#line 4735 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_OPPOSITE; }
#line 12838 "y.tab.c" /* yacc.c:1646  */
    break;

  case 749:
#line 4736 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_BOTH; }
#line 12844 "y.tab.c" /* yacc.c:1646  */
    break;

  case 750:
#line 4742 "pars.yacc" /* yacc.c:1646  */
    {
            int wpp, hpp;
            if ((yyvsp[0].ival) == PAGE_ORIENT_LANDSCAPE) {
                wpp = 792;
                hpp = 612;
            } else {
                wpp = 612;
                hpp = 792;
            }
            set_page_dimensions(wpp, hpp, FALSE);
        }
#line 12860 "y.tab.c" /* yacc.c:1646  */
    break;

  case 751:
#line 4753 "pars.yacc" /* yacc.c:1646  */
    {
            set_page_dimensions((int) (yyvsp[-1].dval), (int) (yyvsp[0].dval), FALSE);
        }
#line 12868 "y.tab.c" /* yacc.c:1646  */
    break;

  case 752:
#line 4756 "pars.yacc" /* yacc.c:1646  */
    {
	    scroll_proc((yyvsp[0].ival));
	}
#line 12876 "y.tab.c" /* yacc.c:1646  */
    break;

  case 753:
#line 4759 "pars.yacc" /* yacc.c:1646  */
    {
	    scrollinout_proc((yyvsp[0].ival));
	}
#line 12884 "y.tab.c" /* yacc.c:1646  */
    break;

  case 754:
#line 4763 "pars.yacc" /* yacc.c:1646  */
    {
	}
#line 12891 "y.tab.c" /* yacc.c:1646  */
    break;

  case 755:
#line 4767 "pars.yacc" /* yacc.c:1646  */
    {
	    add_world(whichgraph, (yyvsp[-14].dval), (yyvsp[-12].dval), (yyvsp[-10].dval), (yyvsp[-8].dval));
	}
#line 12899 "y.tab.c" /* yacc.c:1646  */
    break;

  case 756:
#line 4771 "pars.yacc" /* yacc.c:1646  */
    {filltype_obs = (yyvsp[0].ival);}
#line 12905 "y.tab.c" /* yacc.c:1646  */
    break;

  case 757:
#line 4773 "pars.yacc" /* yacc.c:1646  */
    {filltype_obs = (yyvsp[0].ival);}
#line 12911 "y.tab.c" /* yacc.c:1646  */
    break;

  case 758:
#line 4775 "pars.yacc" /* yacc.c:1646  */
    { }
#line 12917 "y.tab.c" /* yacc.c:1646  */
    break;

  case 759:
#line 4777 "pars.yacc" /* yacc.c:1646  */
    { }
#line 12923 "y.tab.c" /* yacc.c:1646  */
    break;

  case 760:
#line 4779 "pars.yacc" /* yacc.c:1646  */
    { }
#line 12929 "y.tab.c" /* yacc.c:1646  */
    break;

  case 761:
#line 4780 "pars.yacc" /* yacc.c:1646  */
    { }
#line 12935 "y.tab.c" /* yacc.c:1646  */
    break;

  case 762:
#line 4782 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    if ((yyvsp[0].ival) == FALSE && get_project_version() <= 40102) {
                g[whichgraph].l.boxpen.pattern = 0;
            }
	}
#line 12949 "y.tab.c" /* yacc.c:1646  */
    break;

  case 763:
#line 4791 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.legx = (yyvsp[0].dval);
	}
#line 12961 "y.tab.c" /* yacc.c:1646  */
    break;

  case 764:
#line 4798 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].l.legy = (yyvsp[0].dval);
	}
#line 12973 "y.tab.c" /* yacc.c:1646  */
    break;

  case 765:
#line 4805 "pars.yacc" /* yacc.c:1646  */
    {
	    if (is_valid_setno(whichgraph, (yyvsp[-1].ival))) {
                strncpy(g[whichgraph].p[(yyvsp[-1].ival)].lstr, (yyvsp[0].sval), MAX_STRING_LENGTH - 1);
	    } else {
                yyerror("Unallocated set");
            }
            xfree((yyvsp[0].sval));
	}
#line 12986 "y.tab.c" /* yacc.c:1646  */
    break;

  case 766:
#line 4813 "pars.yacc" /* yacc.c:1646  */
    { }
#line 12992 "y.tab.c" /* yacc.c:1646  */
    break;

  case 767:
#line 4814 "pars.yacc" /* yacc.c:1646  */
    {filltype_obs = (yyvsp[0].ival);}
#line 12998 "y.tab.c" /* yacc.c:1646  */
    break;

  case 768:
#line 4815 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13004 "y.tab.c" /* yacc.c:1646  */
    break;

  case 769:
#line 4816 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13010 "y.tab.c" /* yacc.c:1646  */
    break;

  case 770:
#line 4818 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13016 "y.tab.c" /* yacc.c:1646  */
    break;

  case 771:
#line 4820 "pars.yacc" /* yacc.c:1646  */
    { 
	    g[(yyvsp[-2].ival)].type = GRAPH_XY;
	    g[(yyvsp[-2].ival)].xscale = SCALE_LOG;
	}
#line 13025 "y.tab.c" /* yacc.c:1646  */
    break;

  case 772:
#line 4824 "pars.yacc" /* yacc.c:1646  */
    { 
	    g[(yyvsp[-2].ival)].type = GRAPH_XY;
	    g[(yyvsp[-2].ival)].yscale = SCALE_LOG;
	}
#line 13034 "y.tab.c" /* yacc.c:1646  */
    break;

  case 773:
#line 4829 "pars.yacc" /* yacc.c:1646  */
    { 
	    g[(yyvsp[-2].ival)].type = GRAPH_XY;
	    g[(yyvsp[-2].ival)].xscale = SCALE_LOG;
	    g[(yyvsp[-2].ival)].yscale = SCALE_LOG;
	}
#line 13044 "y.tab.c" /* yacc.c:1646  */
    break;

  case 774:
#line 4835 "pars.yacc" /* yacc.c:1646  */
    { 
	    g[(yyvsp[-2].ival)].type = GRAPH_CHART;
	    g[(yyvsp[-2].ival)].xyflip = FALSE;
	    g[(yyvsp[-2].ival)].stacked = FALSE;
	}
#line 13054 "y.tab.c" /* yacc.c:1646  */
    break;

  case 775:
#line 4841 "pars.yacc" /* yacc.c:1646  */
    { 
	    g[(yyvsp[-2].ival)].type = GRAPH_CHART;
	    g[(yyvsp[-2].ival)].xyflip = TRUE;
	}
#line 13063 "y.tab.c" /* yacc.c:1646  */
    break;

  case 776:
#line 4846 "pars.yacc" /* yacc.c:1646  */
    { 
	    g[(yyvsp[-2].ival)].type = GRAPH_CHART;
	    g[(yyvsp[-2].ival)].stacked = TRUE;
	}
#line 13072 "y.tab.c" /* yacc.c:1646  */
    break;

  case 777:
#line 4851 "pars.yacc" /* yacc.c:1646  */
    { 
	    g[(yyvsp[-2].ival)].type = GRAPH_CHART;
	    g[(yyvsp[-2].ival)].stacked = TRUE;
	    g[(yyvsp[-2].ival)].xyflip = TRUE;
	}
#line 13082 "y.tab.c" /* yacc.c:1646  */
    break;

  case 778:
#line 4857 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.xg1 = (yyvsp[0].dval);
	}
#line 13094 "y.tab.c" /* yacc.c:1646  */
    break;

  case 779:
#line 4864 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.xg2 = (yyvsp[0].dval);
	}
#line 13106 "y.tab.c" /* yacc.c:1646  */
    break;

  case 780:
#line 4871 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.yg1 = (yyvsp[0].dval);
	}
#line 13118 "y.tab.c" /* yacc.c:1646  */
    break;

  case 781:
#line 4878 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].w.yg2 = (yyvsp[0].dval);
	}
#line 13130 "y.tab.c" /* yacc.c:1646  */
    break;

  case 782:
#line 4886 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.xv1 = (yyvsp[0].dval);
	}
#line 13142 "y.tab.c" /* yacc.c:1646  */
    break;

  case 783:
#line 4893 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.xv2 = (yyvsp[0].dval);
	}
#line 13154 "y.tab.c" /* yacc.c:1646  */
    break;

  case 784:
#line 4900 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.yv1 = (yyvsp[0].dval);
	}
#line 13166 "y.tab.c" /* yacc.c:1646  */
    break;

  case 785:
#line 4907 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
	    g[whichgraph].v.yv2 = (yyvsp[0].dval);
	}
#line 13178 "y.tab.c" /* yacc.c:1646  */
    break;

  case 786:
#line 4915 "pars.yacc" /* yacc.c:1646  */
    {
	}
#line 13185 "y.tab.c" /* yacc.c:1646  */
    break;

  case 787:
#line 4918 "pars.yacc" /* yacc.c:1646  */
    { 
	    if (!is_valid_gno(whichgraph)) {
                yyerror("No valid graph selected");
                return 1;
            }
            g[whichgraph].f.fillpen.pattern = (yyvsp[0].ival);
        }
#line 13197 "y.tab.c" /* yacc.c:1646  */
    break;

  case 788:
#line 4926 "pars.yacc" /* yacc.c:1646  */
    {
        }
#line 13204 "y.tab.c" /* yacc.c:1646  */
    break;

  case 789:
#line 4928 "pars.yacc" /* yacc.c:1646  */
    {
        }
#line 13211 "y.tab.c" /* yacc.c:1646  */
    break;

  case 790:
#line 4931 "pars.yacc" /* yacc.c:1646  */
    {
	    line_asize = 2.0*(yyvsp[0].dval);
	}
#line 13219 "y.tab.c" /* yacc.c:1646  */
    break;

  case 791:
#line 4935 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13225 "y.tab.c" /* yacc.c:1646  */
    break;

  case 792:
#line 4936 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13231 "y.tab.c" /* yacc.c:1646  */
    break;

  case 793:
#line 4937 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13237 "y.tab.c" /* yacc.c:1646  */
    break;

  case 794:
#line 4938 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13243 "y.tab.c" /* yacc.c:1646  */
    break;

  case 795:
#line 4943 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13249 "y.tab.c" /* yacc.c:1646  */
    break;

  case 796:
#line 4944 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->label_op = (yyvsp[0].ival);
	}
#line 13261 "y.tab.c" /* yacc.c:1646  */
    break;

  case 797:
#line 4954 "pars.yacc" /* yacc.c:1646  */
    {
	    switch ((yyvsp[0].ival)){
	    case 0:
	        g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symfillpen.pattern = 0;
	        break;
	    case 1:
	        g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symfillpen.pattern = 1;
	        break;
	    case 2:
	        g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symfillpen.pattern = 1;
	        g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].symfillpen.color = getbgcolor();
	        break;
	    }
	}
#line 13280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 798:
#line 4969 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].symskip = (yyvsp[0].ival);
	}
#line 13288 "y.tab.c" /* yacc.c:1646  */
    break;

  case 799:
#line 4973 "pars.yacc" /* yacc.c:1646  */
    {
	    switch ((yyvsp[0].ival)) {
            case 0:
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].filltype = SETFILL_NONE;
                break;
            case 1:
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].filltype = SETFILL_POLYGON;
                break;
            case 2:
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].filltype = SETFILL_BASELINE;
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].baseline_type = BASELINE_TYPE_0;
                break;
            case 6:
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].filltype = SETFILL_BASELINE;
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].baseline_type = BASELINE_TYPE_GMIN;
                break;
            case 7:
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].filltype = SETFILL_BASELINE;
                g[(yyvsp[-2].trgt)->gno].p[(yyvsp[-2].trgt)->setno].baseline_type = BASELINE_TYPE_GMAX;
                break;
            }
	}
#line 13315 "y.tab.c" /* yacc.c:1646  */
    break;

  case 800:
#line 4995 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].errbar.ptype = (yyvsp[0].ival);
	}
#line 13323 "y.tab.c" /* yacc.c:1646  */
    break;

  case 801:
#line 5003 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13329 "y.tab.c" /* yacc.c:1646  */
    break;

  case 802:
#line 5004 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-1].trgt)->gno].p[(yyvsp[-1].trgt)->setno].lines = (yyvsp[0].ival);
	}
#line 13337 "y.tab.c" /* yacc.c:1646  */
    break;

  case 803:
#line 5007 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-1].trgt)->gno].p[(yyvsp[-1].trgt)->setno].linew = (yyvsp[0].dval);
	}
#line 13345 "y.tab.c" /* yacc.c:1646  */
    break;

  case 804:
#line 5010 "pars.yacc" /* yacc.c:1646  */
    {
	    g[(yyvsp[-1].trgt)->gno].p[(yyvsp[-1].trgt)->setno].linepen.color = (yyvsp[0].ival);
	}
#line 13353 "y.tab.c" /* yacc.c:1646  */
    break;

  case 805:
#line 5013 "pars.yacc" /* yacc.c:1646  */
    {filltype_obs = (yyvsp[0].ival);}
#line 13359 "y.tab.c" /* yacc.c:1646  */
    break;

  case 806:
#line 5014 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13365 "y.tab.c" /* yacc.c:1646  */
    break;

  case 807:
#line 5015 "pars.yacc" /* yacc.c:1646  */
    {
            g[(yyvsp[-3].trgt)->gno].p[(yyvsp[-3].trgt)->setno].errbar.barsize = (yyvsp[0].dval);
	}
#line 13373 "y.tab.c" /* yacc.c:1646  */
    break;

  case 808:
#line 5018 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13379 "y.tab.c" /* yacc.c:1646  */
    break;

  case 809:
#line 5023 "pars.yacc" /* yacc.c:1646  */
    {
	    /* <= xmgr-4.1 */
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->active = (yyvsp[0].ival);
	}
#line 13392 "y.tab.c" /* yacc.c:1646  */
    break;

  case 810:
#line 5031 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13398 "y.tab.c" /* yacc.c:1646  */
    break;

  case 811:
#line 5032 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13404 "y.tab.c" /* yacc.c:1646  */
    break;

  case 812:
#line 5033 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13410 "y.tab.c" /* yacc.c:1646  */
    break;

  case 813:
#line 5034 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13416 "y.tab.c" /* yacc.c:1646  */
    break;

  case 814:
#line 5035 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13422 "y.tab.c" /* yacc.c:1646  */
    break;

  case 815:
#line 5036 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_NONE;
	}
#line 13434 "y.tab.c" /* yacc.c:1646  */
    break;

  case 816:
#line 5043 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    if (g[whichgraph].t[naxis]->t_spec != TICKS_SPEC_BOTH) {
                g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_MARKS;
            }
	}
#line 13448 "y.tab.c" /* yacc.c:1646  */
    break;

  case 817:
#line 5052 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    if ((yyvsp[0].dval) != 0.0) {
                g[whichgraph].t[naxis]->nminor = 
                            (int) rint(g[whichgraph].t[naxis]->tmajor / (yyvsp[0].dval) - 1);
            } else {
                g[whichgraph].t[naxis]->nminor = 0;
            }
	}
#line 13465 "y.tab.c" /* yacc.c:1646  */
    break;

  case 818:
#line 5064 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->props.size = (yyvsp[0].dval);
	}
#line 13477 "y.tab.c" /* yacc.c:1646  */
    break;

  case 819:
#line 5071 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].wtpos = (yyvsp[0].dval);
	    g[whichgraph].t[naxis]->tloc[(yyvsp[-2].ival)].type = TICK_TYPE_MAJOR;
	}
#line 13490 "y.tab.c" /* yacc.c:1646  */
    break;

  case 820:
#line 5079 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_op = (yyvsp[0].ival);
	}
#line 13502 "y.tab.c" /* yacc.c:1646  */
    break;

  case 821:
#line 5089 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13508 "y.tab.c" /* yacc.c:1646  */
    break;

  case 822:
#line 5090 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    if (g[whichgraph].t[naxis]->t_spec == TICKS_SPEC_BOTH) {
                g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_MARKS;
            }
	}
#line 13522 "y.tab.c" /* yacc.c:1646  */
    break;

  case 823:
#line 5099 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->t_spec = TICKS_SPEC_BOTH;
	}
#line 13534 "y.tab.c" /* yacc.c:1646  */
    break;

  case 824:
#line 5106 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13540 "y.tab.c" /* yacc.c:1646  */
    break;

  case 825:
#line 5108 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_angle = 0;
	}
#line 13552 "y.tab.c" /* yacc.c:1646  */
    break;

  case 826:
#line 5115 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_angle = 90;
	}
#line 13564 "y.tab.c" /* yacc.c:1646  */
    break;

  case 827:
#line 5122 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13570 "y.tab.c" /* yacc.c:1646  */
    break;

  case 828:
#line 5123 "pars.yacc" /* yacc.c:1646  */
    { }
#line 13576 "y.tab.c" /* yacc.c:1646  */
    break;

  case 829:
#line 5124 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    g[whichgraph].t[naxis]->tl_op = (yyvsp[0].ival);
	}
#line 13588 "y.tab.c" /* yacc.c:1646  */
    break;

  case 830:
#line 5131 "pars.yacc" /* yacc.c:1646  */
    {
	    if (!is_valid_axis(whichgraph, naxis)) {
                yyerror("No valid axis selected");
                return 1;
            }
	    switch((yyvsp[0].ival)) {
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
#line 13613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 834:
#line 5159 "pars.yacc" /* yacc.c:1646  */
    {
            (yyval.ival) = (yyvsp[0].ival);
        }
#line 13621 "y.tab.c" /* yacc.c:1646  */
    break;

  case 835:
#line 5164 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_OPPOSITE; }
#line 13627 "y.tab.c" /* yacc.c:1646  */
    break;

  case 836:
#line 5165 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_NORMAL; }
#line 13633 "y.tab.c" /* yacc.c:1646  */
    break;

  case 837:
#line 5166 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_NORMAL; }
#line 13639 "y.tab.c" /* yacc.c:1646  */
    break;

  case 838:
#line 5167 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_OPPOSITE; }
#line 13645 "y.tab.c" /* yacc.c:1646  */
    break;

  case 839:
#line 5168 "pars.yacc" /* yacc.c:1646  */
    { (yyval.ival) = PLACEMENT_BOTH; }
#line 13651 "y.tab.c" /* yacc.c:1646  */
    break;


#line 13655 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 5171 "pars.yacc" /* yacc.c:1906  */


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
