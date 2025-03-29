#include "mandelbrot.h"

#include <stdio.h>
#include <stdlib.h>

#include "My_lib/Logger/logging.h"

int main ()
{
    set_log_lvl (kDebug);

    DrawMandelbrotDefault ();
    DrawMandelbrotArray ();
    // DrawMandelbrot128 ();
    DrawMandelbrot256 ();

    return EXIT_SUCCESS;
}
