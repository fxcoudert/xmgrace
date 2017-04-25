/*
 * test of -pipe option
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#  define M_PI 3.14
#endif

int main(int argc, char **argv)
{
    int i, j;
    double t;

    printf("@focus off\n"); 		    /* turn of the focus markers (annoying) */
    printf("@g0 on\n");                     /* Activate graph 0 */
    printf("@with g0\n");                   /* reset the current graph to graph 0 */
    printf("@view 0.1, 0.1, 0.9, 0.4\n");   /* set the viewport for this graph */
    printf("@g1 on\n");                     /* Activate graph 1 */
    printf("@with g1\n");		    /* reset the current graph to graph 1 */
    printf("@view 0.1, 0.5, 0.9, 0.9\n");   /* set the viewport for graph 1 */
    printf("@subtitle \"Spectrum\"\n");     /* set the subtitle */
    for (j = 20; j > 0; j--) {
	printf("@with g0\n");               /* reset the current graph to graph 0 */
	printf("@kill s0\n");               /* make sure s0 is available
                                             * (data will be read into this set)
                                             */

	/*
	 * write out a set
	 */
	for (i = 0; i < 101; i++) {
	    t = 8.0 * i / (1.0 * j) * M_PI;
	    printf("%d %f\n", i, cos(t) + sin(2.0 * t) + cos(t / 2.0) + sin(4 * t) + cos(t / 4.0));
	}

	printf("&\n");		/* end of set marker */

	printf("@subtitle \"Data %d\"\n", j);   /* set the subtitle for graph 0 */
	if (j == 20) {
	    printf("@autoscale\n"); 		/* autoscale the first time through */
	}
        printf("@dft(s0, 2)\n");                /* compute the spectrum of the first set */
	printf("@move g0.s1 to g1.s0\n");       /* move the computed spectrum to graph 1 */
	if (j == 20) {
	    printf("@with g1\n");               /* set the focus on graph 1 */
	    printf("@autoscale\n");             /* autoscale graph 1 */
	}
	printf("@redraw\n");
	printf("@sleep 1\n");                   /* let the graph sit for a sec */
    }	/* end for */
    
    exit(0);
}
