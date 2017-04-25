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
 * t1fonts.h
 * Type1 fonts for Grace
 */

#ifndef __T1_FONTS_H_
#define __T1_FONTS_H_

#include <config.h>
#include <cmath.h>

#include <t1lib.h>
/* A hack - until there are T1_MAJORVERSION etc defined */
#ifndef T1ERR_SCAN_ENCODING
# define T1_CheckForFontID CheckForFontID
# define T1_GetNoFonts T1_Get_no_fonts
#endif

#include "defines.h"


#if defined(DEBUG_T1LIB)
#  define T1LOGFILE LOGFILE
#else
#  define T1LOGFILE NO_LOGFILE
#endif

#define T1_DEFAULT_BITMAP_PAD  8

#define T1_DEFAULT_ENCODING_FILE  "Default.enc"
#define T1_FALLBACK_ENCODING_FILE "IsoLatin1.enc"

#define T1_AALEVELS 5


#define BAD_FONT_ID     -1

/* Font mappings */
#define FONT_MAP_DEFAULT    0
#define FONT_MAP_ACEGR      1

/* TODO */
#define MAGIC_FONT_SCALE	0.028

#define SSCRIPT_SCALE M_SQRT1_2
#define SUBSCRIPT_SHIFT 0.4
#define SUPSCRIPT_SHIFT 0.6
#define ENLARGE_SCALE sqrt(M_SQRT2)
#define OBLIQUE_FACTOR 0.25

#define TEXT_ADVANCING_LR   0
#define TEXT_ADVANCING_RL   1

#define STRING_DIRECTION_LR 0
#define STRING_DIRECTION_RL 1

#define MARK_NONE   -1
#define MAX_MARKS   32
#define MARK_CR     MAX_MARKS

#define UNIT_TM {1.0, 0.0, 0.0, 1.0}

typedef struct {
    double cxx, cxy;
    double cyx, cyy;
} TextMatrix;

typedef struct {
    char *s;
    int len;
    int font;
    int color;
    TextMatrix tm;
    double hshift;
    double vshift;
    int underline;
    int overline;
    int setmark;
    int gotomark;
    int direction;
    int advancing;
    int ligatures;
    int kerning;
    VPoint start;
    VPoint stop;
    GLYPH *glyph;
} CompositeString;

typedef struct {
    int mapped_id;
    char *alias;
    char *fallback;
} FontDB;

int init_t1(void);

int number_of_fonts(void);
char *get_fontname(int font);
char *get_fontfullname(int font);
char *get_fontfamilyname(int font);
char *get_fontweight(int font);
char *get_fontfilename(int font, int abspath);
char *get_afmfilename(int font, int abspath);
char *get_fontalias(int font);
char *get_fontfallback(int font);
char *get_encodingscheme(int font);
char **get_default_encoding(void);
double get_textline_width(int font);
double get_underline_pos(int font);
double get_overline_pos(int font);
double get_italic_angle(int font);
double *get_kerning_vector(char *str, int len, int font);

int get_font_by_name(char *fname);
int get_font_mapped_id(int font);
int get_mapped_font(int mapped_id);
int map_font(int font, int mapped_id);
int map_font_by_name(char *fname, int mapped_id);
void map_fonts(int map);

#endif /* __T1_FONTS_H_ */
