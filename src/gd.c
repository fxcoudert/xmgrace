#include <config.h>

#include <cmath.h>

#include <string.h>
#include <stdlib.h>

#include "gd.h"

#define costScale 1024
static int cost[] = { 1024, 1023, 1023, 1022, 1021, 1020, 1018, 1016, 1014,
1011, 1008, 1005, 1001, 997, 993, 989, 984, 979, 973, 968, 962, 955, 949, 942,
935, 928, 920, 912, 904, 895, 886, 877, 868, 858, 848, 838, 828, 817, 806, 795,
784, 772, 760, 748, 736, 724, 711, 698, 685, 671, 658, 644, 630, 616, 601, 587,
572, 557, 542, 527, 512, 496, 480, 464, 448, 432, 416, 400, 383, 366, 350, 333,
316, 299, 282, 265, 247, 230, 212, 195, 177, 160, 142, 124, 107, 89, 71, 53,
35, 17, 0, -17, -35, -53, -71, -89, -107, -124, -142, -160, -177, -195, -212,
-230, -247, -265, -282, -299, -316, -333, -350, -366, -383, -400, -416, -432,
-448, -464, -480, -496, -512, -527, -542, -557, -572, -587, -601, -616, -630,
-644, -658, -671, -685, -698, -711, -724, -736, -748, -760, -772, -784, -795,
-806, -817, -828, -838, -848, -858, -868, -877, -886, -895, -904, -912, -920,
-928, -935, -942, -949, -955, -962, -968, -973, -979, -984, -989, -993, -997,
-1001, -1005, -1008, -1011, -1014, -1016, -1018, -1020, -1021, -1022, -1023,
-1023, -1024, -1023, -1023, -1022, -1021, -1020, -1018, -1016, -1014, -1011,
-1008, -1005, -1001, -997, -993, -989, -984, -979, -973, -968, -962, -955,
-949, -942, -935, -928, -920, -912, -904, -895, -886, -877, -868, -858, -848,
-838, -828, -817, -806, -795, -784, -772, -760, -748, -736, -724, -711, -698,
-685, -671, -658, -644, -630, -616, -601, -587, -572, -557, -542, -527, -512,
-496, -480, -464, -448, -432, -416, -400, -383, -366, -350, -333, -316, -299,
-282, -265, -247, -230, -212, -195, -177, -160, -142, -124, -107, -89, -71,
-53, -35, -17, 0, 17, 35, 53, 71, 89, 107, 124, 142, 160, 177, 195, 212, 230,
247, 265, 282, 299, 316, 333, 350, 366, 383, 400, 416, 432, 448, 464, 480, 496,
512, 527, 542, 557, 572, 587, 601, 616, 630, 644, 658, 671, 685, 698, 711, 724,
736, 748, 760, 772, 784, 795, 806, 817, 828, 838, 848, 858, 868, 877, 886, 895,
904, 912, 920, 928, 935, 942, 949, 955, 962, 968, 973, 979, 984, 989, 993, 997,
1001, 1005, 1008, 1011, 1014, 1016, 1018, 1020, 1021, 1022, 1023, 1023 };

#define sintScale 1024
static int sint[] = { 0, 17, 35, 53, 71, 89, 107, 124, 142, 160, 177, 195, 212,
230, 247, 265, 282, 299, 316, 333, 350, 366, 383, 400, 416, 432, 448, 464, 480,
496, 512, 527, 542, 557, 572, 587, 601, 616, 630, 644, 658, 671, 685, 698, 711,
724, 736, 748, 760, 772, 784, 795, 806, 817, 828, 838, 848, 858, 868, 877, 886,
895, 904, 912, 920, 928, 935, 942, 949, 955, 962, 968, 973, 979, 984, 989, 993,
997, 1001, 1005, 1008, 1011, 1014, 1016, 1018, 1020, 1021, 1022, 1023, 1023,
1024, 1023, 1023, 1022, 1021, 1020, 1018, 1016, 1014, 1011, 1008, 1005, 1001,
997, 993, 989, 984, 979, 973, 968, 962, 955, 949, 942, 935, 928, 920, 912, 904,
895, 886, 877, 868, 858, 848, 838, 828, 817, 806, 795, 784, 772, 760, 748, 736,
724, 711, 698, 685, 671, 658, 644, 630, 616, 601, 587, 572, 557, 542, 527, 512,
496, 480, 464, 448, 432, 416, 400, 383, 366, 350, 333, 316, 299, 282, 265, 247,
230, 212, 195, 177, 160, 142, 124, 107, 89, 71, 53, 35, 17, 0, -17, -35, -53,
-71, -89, -107, -124, -142, -160, -177, -195, -212, -230, -247, -265, -282,
-299, -316, -333, -350, -366, -383, -400, -416, -432, -448, -464, -480, -496,
-512, -527, -542, -557, -572, -587, -601, -616, -630, -644, -658, -671, -685,
-698, -711, -724, -736, -748, -760, -772, -784, -795, -806, -817, -828, -838,
-848, -858, -868, -877, -886, -895, -904, -912, -920, -928, -935, -942, -949,
-955, -962, -968, -973, -979, -984, -989, -993, -997, -1001, -1005, -1008,
-1011, -1014, -1016, -1018, -1020, -1021, -1022, -1023, -1023, -1024, -1023,
-1023, -1022, -1021, -1020, -1018, -1016, -1014, -1011, -1008, -1005, -1001,
-997, -993, -989, -984, -979, -973, -968, -962, -955, -949, -942, -935, -928,
-920, -912, -904, -895, -886, -877, -868, -858, -848, -838, -828, -817, -806,
-795, -784, -772, -760, -748, -736, -724, -711, -698, -685, -671, -658, -644,
-630, -616, -601, -587, -572, -557, -542, -527, -512, -496, -480, -464, -448,
-432, -416, -400, -383, -366, -350, -333, -316, -299, -282, -265, -247, -230,
-212, -195, -177, -160, -142, -124, -107, -89, -71, -53, -35, -17 };

static void gdImageBrushApply(gdImagePtr im, int x, int y);
static void gdImageTileApply(gdImagePtr im, int x, int y);


gdImagePtr gdImageCreate(int sx, int sy)
{
	int i;
	gdImagePtr im;
	im = (gdImage *) malloc(sizeof(gdImage));
	/* NOW ROW-MAJOR IN GD 1.3 */
	im->pixels = (unsigned char **) malloc(sizeof(unsigned char *) * sy);
	im->polyInts = 0;
	im->polyAllocated = 0;
	im->brush = 0;
	im->tile = 0;
	im->style = 0;
	for (i=0; (i<sy); i++) {
		/* NOW ROW-MAJOR IN GD 1.3 */
		im->pixels[i] = (unsigned char *) calloc(
			sx, sizeof(unsigned char));
	}	
	im->sx = sx;
	im->sy = sy;
	im->colorsTotal = 0;
	im->transparent = (-1);
	im->interlace = 0;

        for (i=0; (i < gdMaxColors); i++) {
           im->open[i] = 1;
	   im->red[i] = 0;
           im->green[i] = 0;
           im->blue[i] = 0;
	};

	return im;
}

void gdImageDestroy(gdImagePtr im)
{
	int i;
	for (i=0; (i<im->sy); i++) {
		free(im->pixels[i]);
	}	
	free(im->pixels);
	if (im->polyInts) {
			free(im->polyInts);
	}
	if (im->style) {
		free(im->style);
	}
	free(im);
}

int gdImageColorClosest(gdImagePtr im, int r, int g, int b)
{
	int i;
	long rd, gd, bd;
	int ct = (-1);
	int first = 1;
	long mindist = 0;
	for (i=0; (i<(im->colorsTotal)); i++) {
		long dist;
		if (im->open[i]) {
			continue;
		}
		rd = (im->red[i] - r);	
		gd = (im->green[i] - g);
		bd = (im->blue[i] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if (first || (dist < mindist)) {
			mindist = dist;	
			ct = i;
			first = 0;
		}
	}
	return ct;
}

int gdImageColorExact(gdImagePtr im, int r, int g, int b)
{
	int i;
	for (i=0; (i<(im->colorsTotal)); i++) {
		if (im->open[i]) {
			continue;
		}
		if ((im->red[i] == r) && 
			(im->green[i] == g) &&
			(im->blue[i] == b)) {
			return i;
		}
	}
	return -1;
}

int gdImageColorAllocate(gdImagePtr im, int r, int g, int b)
{
	int i;
	int ct = (-1);
	for (i=0; (i<(im->colorsTotal)); i++) {
		if (im->open[i]) {
			ct = i;
			break;
		}
	}	
	if (ct == (-1)) {
		ct = im->colorsTotal;
		if (ct == gdMaxColors) {
			return -1;
		}
		im->colorsTotal++;
	}
	im->red[ct] = r;
	im->green[ct] = g;
	im->blue[ct] = b;
	im->open[ct] = 0;
	return ct;
}

void gdImageColorDeallocate(gdImagePtr im, int color)
{
	/* Mark it open. */
	im->open[color] = 1;
}

void gdImageColorTransparent(gdImagePtr im, int color)
{
	im->transparent = color;
}

void gdImageSetPixel(gdImagePtr im, int x, int y, int color)
{
	int p;
	switch(color) {
		case gdStyled:
		if (!im->style) {
			/* Refuse to draw if no style is set. */
			return;
		} else {
			p = im->style[im->stylePos++];
		}
		if (p != (gdTransparent)) {
			gdImageSetPixel(im, x, y, p);
		}
		im->stylePos = im->stylePos %  im->styleLength;
		break;
		case gdStyledBrushed:
		if (!im->style) {
			/* Refuse to draw if no style is set. */
			return;
		}
		p = im->style[im->stylePos++];
		if ((p != gdTransparent) && (p != 0)) {
			gdImageSetPixel(im, x, y, gdBrushed);
		}
		im->stylePos = im->stylePos %  im->styleLength;
		break;
		case gdBrushed:
		gdImageBrushApply(im, x, y);
		break;
		case gdTiled:
		gdImageTileApply(im, x, y);
		break;
		default:
		if (gdImageBoundsSafe(im, x, y)) {
			/* NOW ROW-MAJOR IN GD 1.3 */
			im->pixels[y][x] = color;
		}
		break;
	}
}

static void gdImageBrushApply(gdImagePtr im, int x, int y)
{
	int lx, ly;
	int hy;
	int hx;
	int x1, y1, x2, y2;
	int srcx, srcy;
	if (!im->brush) {
		return;
	}
	hy = gdImageSY(im->brush)/2;
	y1 = y - hy;
	y2 = y1 + gdImageSY(im->brush);	
	hx = gdImageSX(im->brush)/2;
	x1 = x - hx;
	x2 = x1 + gdImageSX(im->brush);
	srcy = 0;
	for (ly = y1; (ly < y2); ly++) {
		srcx = 0;
		for (lx = x1; (lx < x2); lx++) {
			int p;
			p = gdImageGetPixel(im->brush, srcx, srcy);
			/* Allow for non-square brushes! */
			if (p != gdImageGetTransparent(im->brush)) {
				gdImageSetPixel(im, lx, ly,
					im->brushColorMap[p]);
			}
			srcx++;
		}
		srcy++;
	}	
}		

static void gdImageTileApply(gdImagePtr im, int x, int y)
{
	int srcx, srcy;
	int p;
	if (!im->tile) {
		return;
	}
	srcx = x % gdImageSX(im->tile);
	srcy = y % gdImageSY(im->tile);
	p = gdImageGetPixel(im->tile, srcx, srcy);
	/* Allow for transparency */
	if (p != gdImageGetTransparent(im->tile)) {
		gdImageSetPixel(im, x, y,
			im->tileColorMap[p]);
	}
}		

int gdImageGetPixel(gdImagePtr im, int x, int y)
{
	if (gdImageBoundsSafe(im, x, y)) {
		/* NOW ROW-MAJOR IN GD 1.3 */
		return im->pixels[y][x];
	} else {
		return 0;
	}
}

/* Bresenham as presented in Foley & Van Dam */

void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	dx = abs(x2-x1);
	dy = abs(y2-y1);
	if (dy <= dx) {
		d = 2*dy - dx;
		incr1 = 2*dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}
		gdImageSetPixel(im, x, y, color);
		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y++;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		} else {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y--;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		}		
	} else {
		d = 2*dx - dy;
		incr1 = 2*dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}
		gdImageSetPixel(im, x, y, color);
		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x++;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		} else {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x--;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		}
	}
}

int gdImageBoundsSafe(gdImagePtr im, int x, int y)
{
	return (!(((y < 0) || (y >= im->sy)) ||
		((x < 0) || (x >= im->sx))));
}

/* s and e are integers modulo 360 (degrees), with 0 degrees
  being the rightmost extreme and degrees changing clockwise.
  cx and cy are the center in pixels; w and h are the horizontal 
  and vertical diameter in pixels. Nice interface, but slow, since
  I don't yet use Bresenham (I'm using an inefficient but
  simple solution with too much work going on in it; generalizing
  Bresenham to ellipses and partial arcs of ellipses is non-trivial,
  at least for me) and there are other inefficiencies (small circles
  do far too much work). */

void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color)
{
	int i;
	int lx = 0, ly = 0;
	int w2, h2;
	w2 = w/2;
	h2 = h/2;
	while (e < s) {
		e += 360;
	}
	while (s < 0) {
		s += 360;
		e += 360;
	}
	for (i=s; (i <= e); i++) {
		int x, y;
		x = ((long)cost[i % 360] * (long)w2 / costScale) + cx; 
		y = ((long)sint[i % 360] * (long)h2 / sintScale) + cy;
		if (i != s) {
			gdImageLine(im, lx, ly, x, y, color);	
		}
		lx = x;
		ly = y;
	}
}

void gdImageFilledArc(gdImagePtr im, int cx, int cy, int w, int h,
    int s, int e, int mode, int color)
{
	int i, n, ntot;
	int w2, h2;
	gdPointPtr p;
	w2 = w/2;
	h2 = h/2;
	while (e < s) {
		e += 360;
	}
	while (s < 0) {
		s += 360;
		e += 360;
	}
        
        n = e - s + 1;
        if (mode == gdArcFillPieSlice) {
            ntot = n + 1;
        } else {
            ntot = n;
        }
        p = malloc(ntot*sizeof(gdPoint));
        if (p == NULL) {
            return;
        }
        for (i=0; i < n; i++) {
	    int a = i + s;
	    p[i].x = ((long)cost[a % 360] * (long)w2 / costScale) + cx; 
	    p[i].y = ((long)sint[a % 360] * (long)h2 / sintScale) + cy;
	}
        if (mode == gdArcFillPieSlice) {
	    p[n].x = cx; 
	    p[n].y = cy;
        }
        gdImageFilledPolygon(im, p, ntot, color);
        free(p);
}

#if 0
	/* Bresenham octant code, which I should use eventually */
	int x, y, d;
	x = 0;
	y = w;
	d = 3-2*w;
	while (x < y) {
		gdImageSetPixel(im, cx+x, cy+y, color);
		if (d < 0) {
			d += 4 * x + 6;
		} else {
			d += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
	if (x == y) {
		gdImageSetPixel(im, cx+x, cy+y, color);
	}
#endif

void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	gdImageLine(im, x1, y1, x2, y1, color);		
	gdImageLine(im, x1, y2, x2, y2, color);		
	gdImageLine(im, x1, y1, x1, y2, color);
	gdImageLine(im, x2, y1, x2, y2, color);
}

void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int x, y;
	for (y=y1; (y<=y2); y++) {
		for (x=x1; (x<=x2); x++) {
			gdImageSetPixel(im, x, y, color);
		}
	}
}

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int lx, ly;
	if (!n) {
		return;
	}
	lx = p->x;
	ly = p->y;
	gdImageLine(im, lx, ly, p[n-1].x, p[n-1].y, c);
	for (i=1; (i < n); i++) {
		p++;
		gdImageLine(im, lx, ly, p->x, p->y, c);
		lx = p->x;
		ly = p->y;
	}
}	
	
int gdCompareInt(const void *a, const void *b);
	
void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int y;
	int ymin, ymax;
	int ints;
	if (n < 1) {
		return;
	}
	if (!im->polyAllocated) {
		im->polyInts = (int *) malloc(sizeof(int) * n);
		im->polyAllocated = n;
	}		
	if (im->polyAllocated < n) {
		while (im->polyAllocated < n) {
			im->polyAllocated *= 2;
		}	
		im->polyInts = (int *) realloc(im->polyInts,
			sizeof(int) * im->polyAllocated);
	}
	ymin = p[0].y;
	ymax = p[0].y;
	for (i=1; (i < n); i++) {
		if (p[i].y < ymin) {
			ymin = p[i].y;
		}
		if (p[i].y > ymax) {
			ymax = p[i].y;
		}
	}
	for (y=ymin; (y < ymax); y++) {
		ints = 0;
		for (i=0; (i < n); i++) {
			int x1, x2;
			int y1, y2;
			int ind1, ind2;
			ind1 = i;
			ind2 = (i + 1) % n;
			y1 = p[ind1].y;
			y2 = p[ind2].y;
			x1 = p[ind1].x;
			x2 = p[ind2].x;
			/* intersection exists only if y is between y1 and y2 */
			if (((y >= y1) && (y <= y2)) || ((y >= y2) && (y <= y1))) {
				if (y1 == y2) {
					/* horizontal edge - just draw it */
					gdImageLine(im, x1, y, x2, y, c);
				} else {
					if ((y == y1 && y1 < y2) || (y == y2 && y2 < y1)) {
						/* intersecting at min of an edge, ignore 
						   to avoid double counting! */
					} else {
						/* OK, this one we do want to count :) */
						int inter = (y-y1) * (x2-x1) / (y2-y1) + x1;
						im->polyInts[ints++] = inter;
					}
				}
			}
		}
		qsort(im->polyInts, ints, sizeof(int), gdCompareInt);
		for (i=0; (i < (ints-1)); i+=2) {
			gdImageLine(im, im->polyInts[i], y,
				im->polyInts[i+1], y, c);
		}
	}
}
	
int gdCompareInt(const void *a, const void *b)
{
	return (*(const int *)a) - (*(const int *)b);
}

void gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels)
{
	if (im->style) {
		free(im->style);
	}
	im->style = (int *) 
		malloc(sizeof(int) * noOfPixels);
	memcpy(im->style, style, sizeof(int) * noOfPixels);
	im->styleLength = noOfPixels;
	im->stylePos = 0;
}

void gdImageSetBrush(gdImagePtr im, gdImagePtr brush)
{
	int i;
	im->brush = brush;
	for (i=0; (i < gdImageColorsTotal(brush)); i++) {
		int index;
		index = gdImageColorExact(im, 
			gdImageRed(brush, i),
			gdImageGreen(brush, i),
			gdImageBlue(brush, i));
		if (index == (-1)) {
			index = gdImageColorAllocate(im,
				gdImageRed(brush, i),
				gdImageGreen(brush, i),
				gdImageBlue(brush, i));
			if (index == (-1)) {
				index = gdImageColorClosest(im,
					gdImageRed(brush, i),
					gdImageGreen(brush, i),
					gdImageBlue(brush, i));
			}
		}
		im->brushColorMap[i] = index;
	}
}
	
void gdImageSetTile(gdImagePtr im, gdImagePtr tile)
{
	int i;
	im->tile = tile;
	for (i=0; (i < gdImageColorsTotal(tile)); i++) {
		int index;
		index = gdImageColorExact(im, 
			gdImageRed(tile, i),
			gdImageGreen(tile, i),
			gdImageBlue(tile, i));
		if (index == (-1)) {
			index = gdImageColorAllocate(im,
				gdImageRed(tile, i),
				gdImageGreen(tile, i),
				gdImageBlue(tile, i));
			if (index == (-1)) {
				index = gdImageColorClosest(im,
					gdImageRed(tile, i),
					gdImageGreen(tile, i),
					gdImageBlue(tile, i));
			}
		}
		im->tileColorMap[i] = index;
	}
}

void gdImageInterlace(gdImagePtr im, int interlaceArg)
{
	im->interlace = interlaceArg;
}
