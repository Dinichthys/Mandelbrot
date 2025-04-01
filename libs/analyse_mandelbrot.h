#ifndef ANALYSE_MANDELBROT_H
#define ANALYSE_MANDELBROT_H

#include <stdlib.h>

#include "mandelbrot.h"

#include "settings.h"

const size_t kNumberOfIterations = 100;

enum MandelbrotError AnalyseMandelbrot (settings_of_program_t set,
                                        enum MandelbrotError (*MandelbrotFunc) (int* const iteration_stop_arr,
                                                                                const settings_of_program_t settings));

#endif // ANALYSE_MANDELBROT_H
