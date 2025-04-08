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
        switch (settings.mandelbrot_version)
        {
            case kNaive:
            {
                DrawMandelbrot (settings, MandelbrotNaive, kWindowTitleNaive);
                break;
            }
            case kArray:
            {
                DrawMandelbrot (settings, MandelbrotArray, kWindowTitleArray);
                break;
            }
            case k256Intrin:
            {
                DrawMandelbrot (settings, Mandelbrot256, kWindowTitle256);
                break;
            }
            case k256IntrinFullPipelineTwoIter:
            {
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineTwoIter, kWindowTitle256FullPipeLineTwoIter);
                break;
            }
            case k256IntrinFullPipelineThreeIter:
            {
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineThreeIter, kWindowTitle256FullPipeLineThreeIter);
                break;
            }
            case k256IntrinFullPipelineFourIter:
            {
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineFourIter, kWindowTitle256FullPipeLineFourIter);
                break;
            }
            case k256IntrinFullPipelineFiveIter:
            {
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineFiveIter, kWindowTitle256FullPipeLineFiveIter);
                break;
            }
            case kAll:
            {
                DrawMandelbrot (settings, MandelbrotNaive,                    kWindowTitleNaive                   );
                DrawMandelbrot (settings, MandelbrotArray,                    kWindowTitleArray                   );
                DrawMandelbrot (settings, Mandelbrot256,                      kWindowTitle256                     );
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineTwoIter,   kWindowTitle256FullPipeLineTwoIter  );
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineThreeIter, kWindowTitle256FullPipeLineThreeIter);
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineFourIter,  kWindowTitle256FullPipeLineFourIter );
                DrawMandelbrot (settings, Mandelbrot256FullPipeLineFiveIter,  kWindowTitle256FullPipeLineFiveIter );
                break;
            }
            default:
            {
                SettingsDtor (&settings);
                fprintf (stderr, "Invalid Mandelbrot version.\n");
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        switch (settings.mandelbrot_version)
        {
            case kNaive:
            {
                AnalyseMandelbrot (settings, MandelbrotNaive);
                break;
            }
            case kArray:
            {
                AnalyseMandelbrot (settings, MandelbrotArray);
                break;
            }
            case k256Intrin:
            {
                AnalyseMandelbrot (settings, Mandelbrot256);
                break;
            }
            case k256IntrinFullPipelineTwoIter:
            {
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineTwoIter);
                break;
            }
            case k256IntrinFullPipelineThreeIter:
            {
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineThreeIter);
                break;
            }
            case k256IntrinFullPipelineFourIter:
            {
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineFourIter);
                break;
            }
            case k256IntrinFullPipelineFiveIter:
            {
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineFiveIter);
                break;
            }
            case kAll:
            {
                AnalyseMandelbrot (settings, MandelbrotNaive                   );
                AnalyseMandelbrot (settings, MandelbrotArray                   );
                AnalyseMandelbrot (settings, Mandelbrot256                     );
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineTwoIter  );
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineThreeIter);
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineFourIter );
                AnalyseMandelbrot (settings, Mandelbrot256FullPipeLineFiveIter );
                break;
            }
            default:
            {
                SettingsDtor (&settings);
                fprintf (stderr, "Invalid Mandelbrot version.\n");
                return EXIT_FAILURE;
            }
        }
    }

    SettingsDtor (&settings);
    return EXIT_SUCCESS;
}
