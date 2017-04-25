#ifndef GD_H
#define GD_H 1

/* gd.h: declarations file for the graphic-draw module.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "AS IS." Thomas Boutell and
 * Boutell.Com, Inc. disclaim all warranties, either express or implied, 
 * including but not limited to implied warranties of merchantability and 
 * fitness for a particular purpose, with respect to this code and accompanying
 * documentation. */

/* This can't be changed in the current palette-only version of gd. */

#define gdMaxColors 256

/* Image type. See functions below; you will not need to change
	the elements directly. Use the provided macros to
	access sx, sy, the color table, and colorsTotal for 
	read-only purposes. */

typedef struct gdImageStruct {
	unsigned char ** pixels;
	int sx;
	int sy;
	int colorsTotal;
	int red[gdMaxColors];
	int green[gdMaxColors];
	int blue[gdMaxColors]; 
	int open[gdMaxColors];
	int transparent;
	int *polyInts;
	int polyAllocated;
	struct gdImageStruct *brush;
	struct gdImageStruct *tile;	
	int brushColorMap[gdMaxColors];
	int tileColorMap[gdMaxColors];
	int styleLength;
	int stylePos;
	int *style;
	int interlace;
} gdImage;

typedef gdImage * gdImagePtr;

/* Point type for use in polygon drawing. */

typedef struct {
	int x, y;
} gdPoint, *gdPointPtr;


/* Special colors. */

#define gdStyled (-2)
#define gdBrushed (-3)
#define gdStyledBrushed (-4)
#define gdTiled (-5)

/* NOT the same as the transparent color index.
	This is used in line styles only. */
#define gdTransparent (-6)

#define gdArcFillChord      0
#define gdArcFillPieSlice   1

/* Functions to manipulate images. */

gdImagePtr gdImageCreate(int sx, int sy);
void gdImageDestroy(gdImagePtr im);

void gdImageSetPixel(gdImagePtr im, int x, int y, int color);

void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Corners specified (not width and height). Upper left first, lower right
 	second. */
void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Solid bar. Upper left corner first, lower right corner second. */
void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
int gdImageBoundsSafe(gdImagePtr im, int x, int y);

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c);
void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c);

void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color);
void gdImageFilledArc(gdImagePtr im, int cx, int cy, int w, int h,
    int s, int e, int mode, int color);

int gdImageGetPixel(gdImagePtr im, int x, int y);

int gdImageColorAllocate(gdImagePtr im, int r, int g, int b);
int gdImageColorClosest(gdImagePtr im, int r, int g, int b);
int gdImageColorExact(gdImagePtr im, int r, int g, int b);
void gdImageColorDeallocate(gdImagePtr im, int color);
void gdImageColorTransparent(gdImagePtr im, int color);

void gdImageSetBrush(gdImagePtr im, gdImagePtr brush);
void gdImageSetTile(gdImagePtr im, gdImagePtr tile);
void gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels);

/* On or off (1 or 0) */
void gdImageInterlace(gdImagePtr im, int interlaceArg);

/* Macros to access information about images. READ ONLY. Changing
	these values will NOT have the desired result. */
#define gdImageSX(im) ((im)->sx)
#define gdImageSY(im) ((im)->sy)
#define gdImageColorsTotal(im) ((im)->colorsTotal)
#define gdImageRed(im, c) ((im)->red[(c)])
#define gdImageGreen(im, c) ((im)->green[(c)])
#define gdImageBlue(im, c) ((im)->blue[(c)])
#define gdImageGetTransparent(im) ((im)->transparent)
#define gdImageGetInterlaced(im) ((im)->interlace)

#endif
