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

#include <config.h>
#include <cmath.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defines.h"
#include "draw.h"

#include "utils.h"
#include "files.h"
#include "device.h"
#include "t1fonts.h"

#include "protos.h"

extern int AAGrayLevelsOK;

static int nfonts = 0;
static FontDB *FontDBtable = NULL;
static char **DefEncoding = NULL;

void (*devputpixmap) (VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type);
void (*devputtext) (VPoint vp, char *s, int len, int font,
     TextMatrix *tm, int underline, int overline, int kerning);


int init_t1(void)
{
    int i;
    char buf[GR_MAXPATHLEN], abuf[GR_MAXPATHLEN], fbuf[GR_MAXPATHLEN], *bufp;
    FILE *fd;
    
    /* Set search paths: */
    bufp = grace_path("fonts/type1");
    if (bufp == NULL) {
        return (RETURN_FAILURE);
    }
    T1_SetFileSearchPath(T1_PFAB_PATH, bufp);
    T1_SetFileSearchPath(T1_AFM_PATH, bufp);
    bufp = grace_path("fonts/enc");
    if (bufp == NULL) {
        return (RETURN_FAILURE);
    }
    T1_SetFileSearchPath(T1_ENC_PATH, bufp);
    
    /* Set font database: */
    bufp = grace_path("fonts/FontDataBase");
    if (bufp == NULL) {
        return (RETURN_FAILURE);
    }
    T1_SetFontDataBase(bufp);

    /* Set log-level: */
    T1_SetLogLevel(T1LOG_DEBUG);
    
    /* Initialize t1-library */
    if (T1_InitLib(T1LOGFILE|IGNORE_CONFIGFILE) == NULL) {
        return (RETURN_FAILURE);
    }
    
    nfonts = T1_GetNoFonts();
    if (nfonts < 1) {
        return (RETURN_FAILURE);
    }
    
    fd = grace_openr("fonts/FontDataBase", SOURCE_DISK);
    if (fd == NULL) {
        return (RETURN_FAILURE);
    }
    
    FontDBtable = xmalloc(nfonts*sizeof(FontDB));
    
    /* skip the first line */
    grace_fgets(buf, GR_MAXPATHLEN - 1, fd); 
    for (i = 0; i < nfonts; i++) {
        grace_fgets(buf, GR_MAXPATHLEN - 1, fd); 
        if (sscanf(buf, "%s %s %*s", abuf, fbuf) != 2) {
            fclose(fd);
            return (RETURN_FAILURE);
        }
        FontDBtable[i].mapped_id = i;
        FontDBtable[i].alias     = copy_string(NULL, abuf);
        FontDBtable[i].fallback  = copy_string(NULL, fbuf);
    }
    fclose(fd);
    
    T1_SetDeviceResolutions(72.0, 72.0);
    
    DefEncoding = T1_LoadEncoding(T1_DEFAULT_ENCODING_FILE);
    if (DefEncoding == NULL) {
        DefEncoding = T1_LoadEncoding(T1_FALLBACK_ENCODING_FILE);
    }
    if (DefEncoding != NULL) {
        T1_SetDefaultEncoding(DefEncoding);
    } else {
        return (RETURN_FAILURE);
    }
    
    T1_AASetBitsPerPixel(GRACE_BPP);
    
    T1_SetBitmapPad(T1_DEFAULT_BITMAP_PAD);
    
    return (RETURN_SUCCESS);
}

void map_fonts(int map)
{
    int i;
    
    if (map == FONT_MAP_ACEGR) {
        for (i = 0; i < nfonts; i++) {
            FontDBtable[i].mapped_id = BAD_FONT_ID;
        }
        map_font_by_name("Times-Roman", 0);
        map_font_by_name("Times-Bold", 1);
        map_font_by_name("Times-Italic", 2);
        map_font_by_name("Times-BoldItalic", 3);
        map_font_by_name("Helvetica", 4);
        map_font_by_name("Helvetica-Bold", 5);
        map_font_by_name("Helvetica-Oblique", 6);
        map_font_by_name("Helvetica-BoldOblique", 7);
        map_font_by_name("Symbol", 8);
        map_font_by_name("ZapfDingbats", 9);
    } else {
        for (i = 0; i < nfonts; i++) {
            FontDBtable[i].mapped_id = i;
        }
    }
}

int get_font_mapped_id(int font)
{
    if (font >= nfonts || font < 0) {
        return(BAD_FONT_ID);
    } else {
        return(FontDBtable[font].mapped_id);
    }
}

int get_mapped_font(int mapped_id)
{
    int i;
    
    for (i = 0; i < nfonts; i++) {
        if (FontDBtable[i].mapped_id == mapped_id) {
            return(i);
        }
    }
    
    return(BAD_FONT_ID);
}

int map_font(int font, int mapped_id)
{
    int i;
    
    if (font >= nfonts || font < 0) {
        return RETURN_FAILURE;
    }
    
    /* make sure the mapping is unique */
    for (i = 0; i < nfonts; i++) {
        if (FontDBtable[i].mapped_id == mapped_id) {
            FontDBtable[i].mapped_id = BAD_FONT_ID;
        }
    }
    FontDBtable[font].mapped_id = mapped_id;

    return RETURN_SUCCESS;
}

int map_font_by_name(char *fname, int mapped_id)
{
    return(map_font(get_font_by_name(fname), mapped_id));
}

int number_of_fonts(void)
{
    return (nfonts);
}

int get_font_by_name(char *fname)
{
    int i;
    
    if (fname == NULL) {
        return(BAD_FONT_ID);
    }
    
    for (i = 0; i < nfonts; i++) {
        if (strcmp(get_fontalias(i), fname) == 0) {
            return(i);
        }
    }

    for (i = 0; i < nfonts; i++) {
        if (strcmp(get_fontfallback(i), fname) == 0) {
            return(i);
        }
    }

    return(BAD_FONT_ID);
}

char *get_fontfilename(int font, int abspath)
{
    if (abspath) {
        return (T1_GetFontFilePath(font));
    } else {
        return (T1_GetFontFileName(font));
    }
}

char *get_afmfilename(int font, int abspath)
{
    char *s;

    if (abspath) {
        s = T1_GetAfmFilePath(font);
    } else {
        s = T1_GetAfmFileName(font);
    }
    
    if (s == NULL) {
        char *s1;
        static char buf[256];
        int len;
        
        s = get_fontfilename(font, abspath);
        len = strlen(s);
        s1 = s + (len - 1);
        while(s1 && *s1 != '.') {
            len--;
            s1--;
        }
        strncpy(buf, s, len);
        buf[len] = '\0';
        strcat(buf, "afm");
        return buf;
    } else {
        return s;
    }
}

char *get_fontname(int font)
{
    return (T1_GetFontName(font));
}

char *get_fontfullname(int font)
{
    return (T1_GetFullName(font));
}

char *get_fontfamilyname(int font)
{
    return (T1_GetFamilyName(font));
}

char *get_fontweight(int font)
{
    return (T1_GetWeight(font));
}

char *get_fontalias(int font)
{
    return (FontDBtable[font].alias);
}

char *get_fontfallback(int font)
{
    return (FontDBtable[font].fallback);
}

char *get_encodingscheme(int font)
{
    return (T1_GetEncodingScheme(font));
}

char **get_default_encoding(void)
{
    return (DefEncoding);
}

double get_textline_width(int font)
{
    return (double) T1_GetUnderlineThickness(font)/1000.0;
}

double get_underline_pos(int font)
{
    return (double) T1_GetLinePosition(font, T1_UNDERLINE)/1000.0;
}

double get_overline_pos(int font)
{
    return (double) T1_GetLinePosition(font, T1_OVERLINE)/1000.0;
}

double get_italic_angle(int font)
{
    return (double) T1_GetItalicAngle(font);
}

double *get_kerning_vector(char *str, int len, int font)
{
    if (len < 2 || T1_GetNoKernPairs(font) <= 0) {
        return NULL;
    } else {
        int i, k, ktot;
        double *kvector;
        
        kvector = xmalloc(len*SIZEOF_DOUBLE);
        for (i = 0, ktot = 0; i < len - 1; i++) {
            k = T1_GetKerning(font, str[i], str[i + 1]);
            ktot += k;
            kvector[i] = (double) k/1000;
        }
        if (ktot) {
            kvector[len - 1] = (double) ktot/1000;
        } else {
            XCFREE(kvector);
        }
        
        return kvector;
    }
}

static int tm_scale(TextMatrix *tm, double s)
{
    if (s != 0.0) {
        tm->cxx *= s; tm->cxy *= s; tm->cyx *= s; tm->cyy *= s;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

/* determinant */
static double tm_det(TextMatrix *tm)
{
    return tm->cxx*tm->cyy - tm->cxy*tm->cyx;
}

/* vertical size */
static double tm_size(TextMatrix *tm)
{
    return tm_det(tm)/sqrt(tm->cxx*tm->cxx + tm->cyx*tm->cyx);
}

static int tm_product(TextMatrix *tm, TextMatrix *p)
{
    TextMatrix tmp;
    
    if (tm_det(p) != 0.0) {
        tmp.cxx = p->cxx*tm->cxx + p->cxy*tm->cyx;
        tmp.cxy = p->cxx*tm->cxy + p->cxy*tm->cyy;
        tmp.cyx = p->cyx*tm->cxx + p->cyy*tm->cyx;
        tmp.cyy = p->cyx*tm->cxy + p->cyy*tm->cyy;
        *tm = tmp;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

static void tm_rotate(TextMatrix *tm, double angle)
{
    if (angle != 0.0) {
        double co, si;
        TextMatrix tmp;

        si = sin(M_PI/180.0*angle);
        co = cos(M_PI/180.0*angle);
        tmp.cxx = co; tmp.cyy = co; tmp.cxy = -si; tmp.cyx = si;
        tm_product(tm, &tmp);
    }
}

static void tm_slant(TextMatrix *tm, double slant)
{
    if (slant != 0.0) {
        TextMatrix tmp;

        tmp.cxx = 1.0; tmp.cyy = 1.0; tmp.cxy = slant; tmp.cyx = 0.0;
        tm_product(tm, &tmp);
    }
}

GLYPH *GetGlyphString(CompositeString *cs, double dpv, int fontaa)
{
    int i;
    
    int len = cs->len;
    int FontID = cs->font;
    float Size;
    
    long Space = 0;
    
    GLYPH *glyph;
    
    static int aacolors[T1_AALEVELS];
    unsigned int fg, bg;
    static unsigned long last_bg = 0, last_fg = 0;

    int modflag;
    T1_TMATRIX matrix, *matrixP;

    RGB fg_rgb, bg_rgb, delta_rgb, *prgb;
    CMap_entry cmap;
    
    if (cs->len == 0) {
        return NULL;
    }

    /* T1lib doesn't like negative sizes */
    Size = (float) fabs(tm_size(&cs->tm));
    if (Size == 0.0) {
        return NULL;
    }

    /* NB: T1lib uses counter-intuitive names for off-diagonal terms */
    matrix.cxx = (float) cs->tm.cxx/Size;
    matrix.cxy = (float) cs->tm.cyx/Size;
    matrix.cyx = (float) cs->tm.cxy/Size;
    matrix.cyy = (float) cs->tm.cyy/Size;

    Size *= dpv;

    modflag = T1_UNDERLINE * cs->underline |
              T1_OVERLINE  * cs->overline  |
              T1_KERNING   * cs->kerning;

    if (fabs(matrix.cxx - 1) < 0.01 && fabs(matrix.cyy - 1) < 0.01 &&
        fabs(matrix.cxy) < 0.01 && fabs(matrix.cyx) < 0.01) {
        matrixP = NULL;
    } else {
        matrixP = &matrix;
    }

    if (fontaa == TRUE) {
    	fg = cs->color;
    	bg = getbgcolor();

    	aacolors[0] = bg;
    	aacolors[T1_AALEVELS - 1] = fg;

    	if (!AAGrayLevelsOK || (fg != last_fg) || (bg != last_bg)) {
    	    /* Get RGB values for fore- and background */
    	    prgb = get_rgb(fg);
    	    if (prgb == NULL) {
    		return NULL;
    	    }
    	    fg_rgb = *prgb;
 
    	    prgb = get_rgb(bg);
    	    if (prgb == NULL) {
    		return NULL;
    	    }
    	    bg_rgb = *prgb;
 
    	    delta_rgb.red   = (fg_rgb.red   - bg_rgb.red)   / (T1_AALEVELS - 1);
    	    delta_rgb.green = (fg_rgb.green - bg_rgb.green) / (T1_AALEVELS - 1);
    	    delta_rgb.blue  = (fg_rgb.blue  - bg_rgb.blue)  / (T1_AALEVELS - 1);
 
    	    for (i = 1; i < T1_AALEVELS - 1; i++) {
    		cmap.rgb.red   = bg_rgb.red + i*delta_rgb.red;
    		cmap.rgb.green = bg_rgb.green + i*delta_rgb.green;
    		cmap.rgb.blue  = bg_rgb.blue + i*delta_rgb.blue;
    		cmap.cname = "";
    		cmap.ctype = COLOR_AUX;
    		aacolors[i] = add_color(cmap);
    	    }
 
    	    last_fg = fg;
    	    last_bg = bg;
    	    AAGrayLevelsOK = TRUE;
    	}
 
    	/* Set the colors for Anti-Aliasing */
    	T1_AASetGrayValues(aacolors[0],
    			   aacolors[1],
    			   aacolors[2],
    			   aacolors[3],
    			   aacolors[4]);

    	glyph = T1_AASetString(FontID, cs->s, len,
    				   Space, modflag, Size, matrixP);
    } else {
    	glyph = T1_SetString(FontID, cs->s, len,
    				   Space, modflag, Size, matrixP);
    }
 
    return glyph;
}

static void FreeCompositeString(CompositeString *cs, int nss)
{
    int i = 0;
    
    for (i = 0; i < nss; i++) {
	xfree(cs[i].s);
	if (cs[i].glyph != NULL) {
            T1_FreeGlyph(cs[i].glyph);
        }
    }
    xfree(cs);
}

static int get_escape_args(char *s, char *buf)
{
    int i = 0;
    
    if (*s == '{') {
        s++;
        while (*s != '\0') {
            if (*s == '}') {
                *buf = '\0';
                return i;
            } else {
                *buf = *s;
                buf++; s++; i++;
            }
        }
    }
    
    return -1;
}

static const TextMatrix unit_tm = UNIT_TM;

static CompositeString *String2Composite(char *string, int *nss)
{
    CompositeString *csbuf;

    char *ss, *buf, *acc_buf;
    int inside_escape = FALSE;
    int i, isub, j;
    int acc_len;
    int slen;
    char ccode;
    int upperset = FALSE;
    double scale;
    TextMatrix tm_buf;
    
    int font = BAD_FONT_ID, new_font = font;
    int color = BAD_COLOR, new_color = color;
    TextMatrix tm = unit_tm, tm_new = tm;
    double hshift = 0.0, new_hshift = hshift;
    double baseline = 0.0, baseline_old;
    double vshift = baseline, new_vshift = vshift;
    int underline = FALSE, overline = FALSE;
    int new_underline = underline, new_overline = overline;
    int kerning = FALSE, new_kerning = kerning;
    int direction = STRING_DIRECTION_LR, new_direction = direction;
    int advancing = TEXT_ADVANCING_LR, new_advancing = advancing;
    int ligatures = FALSE, new_ligatures = ligatures;

    int setmark = MARK_NONE;
    int gotomark = MARK_NONE, new_gotomark = gotomark;
    
    double val;

    csbuf = NULL;
    *nss = 0;
    
    if (string == NULL) {
        return NULL;
    }

    slen = strlen(string);
    
    if (slen == 0) {
        return NULL;
    }
    
    ss = xmalloc(slen + 1);
    buf = xmalloc(slen + 1);
    acc_buf = xmalloc(slen + 1);
    if (ss == NULL || buf == NULL || acc_buf == NULL) {
        xfree(acc_buf);
        xfree(buf);
        xfree(ss);
        return NULL;
    }
     
    isub = 0;
    ss[isub] = 0;
    
    for (i = 0; i <= slen; i++) {
	ccode = string[i];
	acc_len = 0;
        if (ccode < 32 && ccode > 0) {
	    /* skip control codes */
            continue;
	}
        if (inside_escape) {
            inside_escape = FALSE;
            
            if (isdigit(ccode)) {
	        new_font = get_mapped_font(ccode - '0');
	        continue;
	    } else if (ccode == 'd') {
                i++;
                switch (string[i]) {
                case 'l':
		    new_direction = STRING_DIRECTION_LR;
		    break;
	        case 'r':
		    new_direction = STRING_DIRECTION_RL;
		    break;
	        case 'L':
		    new_advancing = TEXT_ADVANCING_LR;
		    break;
	        case 'R':
		    new_advancing = TEXT_ADVANCING_RL;
		    break;
                default:
                    /* undo advancing */
                    i--;
		    break;
                }
                continue;
	    } else if (ccode == 'F') {
                i++;
                switch (string[i]) {
                case 'k':
		    new_kerning = TRUE;
		    break;
	        case 'K':
		    new_kerning = FALSE;
		    break;
	        case 'l':
		    new_ligatures = TRUE;
		    break;
	        case 'L':
		    new_ligatures = FALSE;
		    break;
                default:
                    /* undo advancing */
                    i--;
		    break;
                }
                continue;
            } else if (isoneof(ccode, "cCsSNBxuUoO+-qQn")) {
                switch (ccode) {
	        case 's':
                    new_vshift -= tm_size(&tm_new)*SUBSCRIPT_SHIFT;
                    tm_scale(&tm_new, SSCRIPT_SCALE);
		    break;
	        case 'S':
                    new_vshift += tm_size(&tm_new)*SUPSCRIPT_SHIFT;
                    tm_scale(&tm_new, SSCRIPT_SCALE);
		    break;
	        case 'N':
                    scale = 1.0/tm_size(&tm_new);
                    tm_scale(&tm_new, scale);
		    new_vshift = baseline;
		    break;
	        case 'B':
		    new_font = BAD_FONT_ID;
		    break;
	        case 'x':
		    new_font = get_font_by_name("Symbol");
		    break;
	        case 'c':
	            upperset = TRUE;
		    break;
	        case 'C':
	            upperset = FALSE;
		    break;
	        case 'u':
		    new_underline = TRUE;
		    break;
	        case 'U':
		    new_underline = FALSE;
		    break;
	        case 'o':
		    new_overline = TRUE;
		    break;
	        case 'O':
		    new_overline = FALSE;
		    break;
	        case '-':
                    tm_scale(&tm_new, 1.0/ENLARGE_SCALE);
		    break;
	        case '+':
                    tm_scale(&tm_new, ENLARGE_SCALE);
		    break;
	        case 'q':
                    tm_slant(&tm_new, OBLIQUE_FACTOR);
		    break;
	        case 'Q':
                    tm_slant(&tm_new, -OBLIQUE_FACTOR);
		    break;
	        case 'n':
                    new_gotomark = MARK_CR;
		    baseline -= 1.0;
                    new_vshift = baseline;
		    new_hshift = 0.0;
		    break;
                }
                continue;
            } else if (isoneof(ccode, "fhvVzZmM#rltTR") &&
                       (j = get_escape_args(&(string[i + 1]), buf)) >= 0) {
                i += (j + 2);
                switch (ccode) {
	        case 'f':
                    if (j == 0) {
                        new_font = BAD_FONT_ID;
                    } else if (isdigit(buf[0])) {
                        new_font = get_mapped_font(atoi(buf));
                    } else {
                        new_font = get_font_by_name(buf);
                    }
                    break;
	        case 'v':
                    if (j == 0) {
                        new_vshift = baseline;
                    } else {
                        val = atof(buf);
                        new_vshift += tm_size(&tm_new)*val;
                    }
                    break;
	        case 'V':
                    baseline_old = baseline;
                    if (j == 0) {
                        baseline = 0.0;
                    } else {
                        val = atof(buf);
                        baseline += tm_size(&tm_new)*val;
                    }
                    new_vshift = baseline;
                    break;
	        case 'h':
                    val = atof(buf);
                    new_hshift = tm_size(&tm_new)*val;
                    break;
	        case 'z':
                    if (j == 0) {
                        scale = 1.0/tm_size(&tm_new);
                        tm_scale(&tm_new, scale);
                    } else {
                        scale = atof(buf);
                        tm_scale(&tm_new, scale);
                    }
                    break;
	        case 'Z':
                    scale = atof(buf)/tm_size(&tm_new);
                    tm_scale(&tm_new, scale);
                    break;
	        case 'r':
                    tm_rotate(&tm_new, atof(buf));
                    break;
	        case 'l':
                    tm_slant(&tm_new, atof(buf));
                    break;
	        case 't':
                    if (j == 0) {
                        tm_new = unit_tm;
                    } else {
                        if (sscanf(buf, "%lf %lf %lf %lf",
                                        &tm_buf.cxx, &tm_buf.cxy,
                                        &tm_buf.cyx, &tm_buf.cyy) == 4) {
                            tm_product(&tm_new, &tm_buf);
                        }
                    }
                    break;
	        case 'T':
                    if (sscanf(buf, "%lf %lf %lf %lf",
                                    &tm_buf.cxx, &tm_buf.cxy,
                                    &tm_buf.cyx, &tm_buf.cyy) == 4) {
                        tm_new = tm_buf;
                    }
                    break;
	        case 'm':
                    setmark = atoi(buf);
                    break;
	        case 'M':
                    new_gotomark = atoi(buf);
		    new_vshift = baseline;
		    new_hshift = 0.0;
                    break;
	        case 'R':
                    if (j == 0) {
                        new_color = BAD_COLOR;
                    } else if (isdigit(buf[0])) {
                            new_color = atof(buf);
                    } else {
                        new_color = get_color_by_name(buf);
                    }
                    break;
	        case '#':
                    if (j % 2 == 0) {
                        int k;
                        char hex[3];
                        hex[2] = '\0';
                        for (k = 0; k < j; k += 2) {
                            hex[0] = buf[k];
                            hex[1] = buf[k + 1];
                            acc_buf[acc_len] = strtol(hex, NULL, 16);
	                    acc_len++;
                        }
                    }
                    break;
                }

                if (ccode != '#') {
                    continue;
                }
	    } else {
                /* store the char */
                acc_buf[0] = (ccode + (upperset*0x80)) & 0xff;
                acc_len = 1;
            }
        } else {
            if (ccode == '\\') {
                inside_escape = TRUE;
                continue;
            } else {
                /* store the char */
                acc_buf[0] = (ccode + (upperset*0x80)) & 0xff;
                acc_len = 1;
            }
        }
	
        if ((new_font      != font      ) ||
	    (new_color     != color     ) ||
	    (tm_new.cxx    != tm.cxx    ) ||
	    (tm_new.cxy    != tm.cxy    ) ||
	    (tm_new.cyx    != tm.cyx    ) ||
	    (tm_new.cyy    != tm.cyy    ) ||
	    (new_hshift    != 0.0       ) ||
	    (new_vshift    != vshift    ) ||
	    (new_underline != underline ) ||
	    (new_overline  != overline  ) ||
	    (new_kerning   != kerning   ) ||
	    (new_direction != direction ) ||
	    (new_advancing != advancing ) ||
	    (new_ligatures != ligatures ) ||
	    (setmark       >= 0         ) ||
	    (new_gotomark  >= 0         ) ||
	    (ccode         == 0         )) {
	    
            if (isub != 0 || setmark >= 0) {	/* non-empty substring */
	
	        csbuf = xrealloc(csbuf, (*nss + 1)*sizeof(CompositeString));
	        csbuf[*nss].font = font;
	        csbuf[*nss].color = color;
	        csbuf[*nss].tm = tm;
	        csbuf[*nss].hshift = hshift;
	        csbuf[*nss].vshift = vshift;
	        csbuf[*nss].underline = underline;
	        csbuf[*nss].overline = overline;
	        csbuf[*nss].kerning = kerning;
	        csbuf[*nss].direction = direction;
	        csbuf[*nss].advancing = advancing;
	        csbuf[*nss].ligatures = ligatures;
	        csbuf[*nss].setmark = setmark;
                setmark = MARK_NONE;
	        csbuf[*nss].gotomark = gotomark;

	        csbuf[*nss].s = xmalloc(isub*SIZEOF_CHAR);
	        memcpy(csbuf[*nss].s, ss, isub);
	        csbuf[*nss].len = isub;
	        isub = 0;
	
                (*nss)++;
            }
	    
	    font = new_font;
	    color = new_color;
	    tm = tm_new;
	    hshift = new_hshift;
            if (hshift != 0.0) {
                /* once a substring is manually advanced, all the following
                 * substrings will be advanced as well!
                 */
                new_hshift = 0.0;
            }
	    vshift = new_vshift;
	    underline = new_underline;
	    overline = new_overline;
	    kerning = new_kerning;
	    direction = new_direction;
	    advancing = new_advancing;
	    ligatures = new_ligatures;
            gotomark = new_gotomark;
            if (gotomark >= 0) {
                /* once a substring is manually advanced, all the following
                 * substrings will be advanced as well!
                 */
                new_gotomark = MARK_NONE;
            }
	} 
	memcpy(&ss[isub], acc_buf, acc_len*SIZEOF_CHAR);
	isub += acc_len;
    }
    
    xfree(acc_buf);
    xfree(buf);
    xfree(ss);

    return (csbuf);
}

static void reverse_string(char *s, int len)
{
    char cbuf;
    int i;
    
    if (s == NULL) {
        return;
    }
    
    for (i = 0; i < len/2; i++) {
        cbuf = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = cbuf;
    }
}

static void process_ligatures(CompositeString *cs)
{
    int j, k, l, m, none_found;
    char *ligtheString;
    char *succs, *ligs;
    char buf_char;

    ligtheString = xmalloc((cs->len + 1)*SIZEOF_CHAR);
    /* Loop through the characters */
    for (j = 0, m = 0; j < cs->len; j++, m++) {
        if ((k = T1_QueryLigs(cs->font, cs->s[j], &succs, &ligs)) > 0) {
            buf_char = cs->s[j];
            while (k > 0){
                none_found = 1;
                for (l = 0; l < k; l++) { /* Loop through the ligatures */
                    if (succs[l] == cs->s[j + 1]) {
                        buf_char = ligs[l];
                        j++;
                        none_found = 0;
                        break;
                    }
                }
                if (none_found) {
                    break;
                }
                k = T1_QueryLigs(cs->font, buf_char, &succs, &ligs);
            }
            ligtheString[m] = buf_char;
        } else { /* There are no ligatures */
            ligtheString[m] = cs->s[j];
        }
    }
    ligtheString[m] = 0;
    
    xfree(cs->s);
    cs->s = ligtheString;
    cs->len = m;
}

void WriteString(VPoint vp, int rot, int just, char *theString)
{    
    VPoint vptmp;
 
    double page_ipv, page_dpv;
    
    int def_font = getfont();
    int def_color = getcolor();
 
    double Angle = (double) rot;

    /* charsize (in VP units) */
    double charsize = MAGIC_FONT_SCALE*getcharsize();

    int text_advancing;

    int iss, nss;
    GLYPH *glyph;
 
    CompositeString *cstring;
 
    int pheight, pwidth;
    int hjust, vjust;
    double hfudge, vfudge;
    
    int setmark, gotomark;
    VPoint cs_marks[MAX_MARKS];
    
    VPoint rpoint, baseline_start, baseline_stop, bbox_ll, bbox_ur, offset;
    
    Device_entry dev;
 
    if (theString == NULL || strlen(theString) == 0) {
	return;
    }
    
    if (charsize <= 0.0) {
        return;
    }

    dev = get_curdevice_props();
    
    /* inches per 1 unit of viewport */
    page_ipv = MIN2(page_width_in, page_height_in);

    /* dots per 1 unit of viewport */
    page_dpv = page_ipv*page_dpi;

    hjust = just & 03;
    switch (hjust) {
    case JUST_LEFT:
        hfudge = 0.0;
        break;
    case JUST_RIGHT:
        hfudge = 1.0;
        break;
    case JUST_CENTER:
        hfudge = 0.5;
        break;
    default:
        errmsg("Wrong justification type of string");
        return;
    }

    vjust = just & 014;
    switch (vjust) {
    case JUST_BOTTOM:
        vfudge = 0.0;
        break;
    case JUST_TOP:
        vfudge = 1.0;
        break;
    case JUST_MIDDLE:
        vfudge = 0.5;
        break;
    case JUST_BLINE:
        /* Not used; to make compiler happy */
        vfudge = 0.0;
        break;
    default:
        /* This can't happen; to make compiler happy */
        errmsg("Internal error");
        return;
    }

    cstring = String2Composite(theString, &nss);
    if (cstring == NULL) {
        return;
    }
    
    /* zero marks */
    for (gotomark = 0; gotomark < MAX_MARKS; gotomark++) {
        cs_marks[gotomark] = vp;
    }
    
    rpoint = vp;
    baseline_start = rpoint;
    bbox_ll = rpoint;
    bbox_ur = rpoint;
    for (iss = 0; iss < nss; iss++) {
	CompositeString *cs = &cstring[iss];
        
        /* Post-process the CS */
        if (cs->font == BAD_FONT_ID) {
            cs->font = def_font;
        }
        if (cs->color == BAD_COLOR) {
            cs->color = def_color;
        }
        if (cs->ligatures == TRUE) {
            process_ligatures(cs);
        }
        if (cs->direction == STRING_DIRECTION_RL) {
            reverse_string(cs->s, cs->len);
        }
        tm_rotate(&cs->tm, Angle);
        
        tm_scale(&cs->tm, charsize);
        cs->vshift *= charsize;
        cs->hshift *= charsize;
        
        text_advancing = cs->advancing;
        gotomark = cs->gotomark;
        setmark = cs->setmark;

        glyph = GetGlyphString(cs, page_dpv, dev.fontaa);
        if (glyph != NULL) {
            VPoint hvpshift, vvpshift;

            if (text_advancing == TEXT_ADVANCING_RL) {
                glyph->metrics.leftSideBearing -= glyph->metrics.advanceX;
                glyph->metrics.rightSideBearing -= glyph->metrics.advanceX;
                glyph->metrics.advanceX *= -1;
                glyph->metrics.ascent  -= glyph->metrics.advanceY;
                glyph->metrics.descent -= glyph->metrics.advanceY;
                glyph->metrics.advanceY *= -1;
            }

            vvpshift.x = cs->tm.cxy*cs->vshift/tm_size(&cs->tm);
            vvpshift.y = cs->tm.cyy*cs->vshift/tm_size(&cs->tm);
            
            hvpshift.x = cs->tm.cxx*cs->hshift/tm_size(&cs->tm);
            hvpshift.y = cs->tm.cyx*cs->hshift/tm_size(&cs->tm);

            if (gotomark >= 0 && gotomark < MAX_MARKS) {
                rpoint = cs_marks[gotomark];
            } else if (gotomark == MARK_CR) {
                /* carriage return */
                rpoint = vp;
            }

            rpoint.x += hvpshift.x;
            rpoint.y += hvpshift.y;
            
            cs->start = rpoint;
            cs->start.x += vvpshift.x;
            cs->start.y += vvpshift.y;

            /* update bbox */
            vptmp.x = cs->start.x + (double) glyph->metrics.leftSideBearing/page_dpv;
            vptmp.y = cs->start.y + (double) glyph->metrics.descent/page_dpv;
            bbox_ll.x = MIN2(bbox_ll.x, vptmp.x);
            bbox_ll.y = MIN2(bbox_ll.y, vptmp.y);

            vptmp.x = cs->start.x + (double) glyph->metrics.rightSideBearing/page_dpv;
            vptmp.y = cs->start.y + (double) glyph->metrics.ascent/page_dpv;
            bbox_ur.x = MAX2(bbox_ur.x, vptmp.x);
            bbox_ur.y = MAX2(bbox_ur.y, vptmp.y);
            
            rpoint.x += (double) glyph->metrics.advanceX/page_dpv;
            rpoint.y += (double) glyph->metrics.advanceY/page_dpv;

            if (setmark >= 0 && setmark < MAX_MARKS) {
                cs_marks[setmark].x = rpoint.x;
                cs_marks[setmark].y = rpoint.y;
            }
            
            cs->stop = rpoint;
            cs->stop.x += vvpshift.x;
            cs->stop.y += vvpshift.y;

            cs->glyph = T1_CopyGlyph(glyph);
        } else {
            cs->glyph = NULL;
        }
    }

    baseline_stop = rpoint;
    
    if (vjust == JUST_BLINE) {
        offset.x = baseline_start.x + 
            hfudge*(baseline_stop.x - baseline_start.x) - vp.x;
        offset.y = baseline_start.y + 
            hfudge*(baseline_stop.y - baseline_start.y) - vp.y;
    } else {
        offset.x = bbox_ll.x + 
            hfudge*(bbox_ur.x - bbox_ll.x) - vp.x;
        offset.y = bbox_ll.y + 
            vfudge*(bbox_ur.y - bbox_ll.y) - vp.y;
    }
    
    /* justification corrections */
    for (iss = 0; iss < nss; iss++) {
        glyph = cstring[iss].glyph;
        if (glyph == NULL) {
            continue;
        }
        cstring[iss].start.x -= offset.x;
        cstring[iss].start.y -= offset.y;
        cstring[iss].stop.x  -= offset.x;
        cstring[iss].stop.y  -= offset.y;
    }
        
    /* update BB */
    bbox_ll.x -= offset.x;
    bbox_ll.y -= offset.y;
    bbox_ur.x -= offset.x;
    bbox_ur.y -= offset.y;
    update_bboxes(bbox_ll);
    update_bboxes(bbox_ur);
        
    for (iss = 0; iss < nss; iss++) {
        CompositeString *cs = &cstring[iss];
        glyph = cs->glyph;
        if (glyph == NULL) {
            continue;
        }
        
        pheight = glyph->metrics.ascent - glyph->metrics.descent;
        pwidth  = glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
        if (pheight <= 0 || pwidth <= 0) {
            continue;
        }
        
        if (get_draw_mode() == TRUE) {
            /* No patterned texts yet */
            setpattern(1);
            setcolor(cs->color);

            if (dev.devfonts == TRUE) {
                if (cs->advancing == TEXT_ADVANCING_RL) {
                    vptmp = cs->stop;
                } else {
                    vptmp = cs->start;
                }
                if (devputtext == NULL) {
                    errmsg("Device has no built-in fonts");
                } else {
                    (*devputtext) (vptmp, cs->s, cs->len, cs->font,
                        &cs->tm, cs->underline, cs->overline, cs->kerning);
                }
            } else {
                /* upper left corner of bitmap */
                vptmp = cs->start;
                vptmp.x += (double) glyph->metrics.leftSideBearing/page_dpv;
                vptmp.y += (double) glyph->metrics.ascent/page_dpv;

                (*devputpixmap) (vptmp, pwidth, pheight, glyph->bits, 
                    glyph->bpp, T1_DEFAULT_BITMAP_PAD, PIXMAP_TRANSPARENT);
            }
        }
    }

    FreeCompositeString(cstring, nss);
}
