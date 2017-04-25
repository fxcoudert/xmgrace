#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "grace_np.h"

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#  define EXIT_FAILURE -1
#endif

void my_error_function(const char *msg)
{
    fprintf(stderr, "library message : \"%s\"\n", msg);
}

int
main (int argc, char* argv[])
{
    int i;

    GraceRegisterErrorFunction (my_error_function);

    /* Start Grace with a buffer size of 2048 and open the pipe */
    if (GraceOpen(2048) == -1) {
        fprintf (stderr, "Can't run Grace. \n");
        exit (EXIT_FAILURE);
    }
    
    /* Send some initialization commands to Grace */
    GracePrintf ("world xmax 100");
    GracePrintf ("world ymax 10000");
    GracePrintf ("xaxis tick major 20");
    GracePrintf ("xaxis tick minor 10");
    GracePrintf ("yaxis tick major 2000");
    GracePrintf ("yaxis tick minor 1000");
    GracePrintf ("s0 on");
    GracePrintf ("s0 symbol 1");
    GracePrintf ("s0 symbol size 0.3");
    GracePrintf ("s0 symbol fill pattern 1");
    GracePrintf ("s1 on");
    GracePrintf ("s1 symbol 1");
    GracePrintf ("s1 symbol size 0.3");
    GracePrintf ("s1 symbol fill pattern 1");

    /* Display sample data */
    for (i = 1; i <= 100 && GraceIsOpen(); i++) {
        GracePrintf ("g0.s0 point %d, %d", i, i);
        GracePrintf ("g0.s1 point %d, %d", i, i * i);
        /* Update the Grace display after every ten steps */
        if (i % 10 == 0) {
            GracePrintf ("redraw");
            /* Wait a second, just to simulate some time needed for
               calculations. Your real application shouldn't wait. */
            sleep (1);
        }
    }

    if (GraceIsOpen()) {
        /* Tell Grace to save the data */
        GracePrintf ("saveall \"sample.agr\"");

        /* Flush the output buffer and close Grace */
        GraceClose();

        /* We are done */
        exit (EXIT_SUCCESS);
    } else {
        exit (EXIT_FAILURE);
    }

}

