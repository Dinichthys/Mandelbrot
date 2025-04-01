#include "mandelbrot.h"

#include <stdio.h>
#include <stdlib.h>

#include "settings.h"

#include "My_lib/Logger/logging.h"

int main (const int argc, char* const argv[])
{
    settings_of_program_t settings = {};
    if (SettingsCtor (&settings) != kDoneSettings)
    {
        fprintf (stderr, "Can't create the structure");
        return EXIT_FAILURE;
    }

    ParseFlags (argc, argv, &settings);
    if (settings.stop_program)
    {
        SettingsDtor (&settings);
        return EXIT_FAILURE;
    }

    set_log_lvl (kDebug);
    set_log_file (settings.log_file);

    if (settings.graphic_mode)
    {
        DrawMandelbrot (settings, MandelbrotNaive,           kWindowTitleNaive          );
        DrawMandelbrot (settings, MandelbrotArray,           kWindowTitleArray          );
        DrawMandelbrot (settings, Mandelbrot256,             kWindowTitle256            );
        DrawMandelbrot (settings, Mandelbrot256FullPipeLine, kWindowTitle256FullPipeLine);
    }
    else
    {
        AnalyseMandelbrot (settings, MandelbrotNaive          );
        AnalyseMandelbrot (settings, MandelbrotArray          );
        AnalyseMandelbrot (settings, Mandelbrot256            );
        AnalyseMandelbrot (settings, Mandelbrot256FullPipeLine);
    }

    SettingsDtor (&settings);
    return EXIT_SUCCESS;
}
