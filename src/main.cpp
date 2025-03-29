#include "mandelbrot.h"

#include <stdio.h>
#include <stdlib.h>

#include "My_lib/Logger/logging.h"

int main ()
{
    set_log_lvl (kDebug);

    DrawMandelbrot ();

    return EXIT_SUCCESS;
}
