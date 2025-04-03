#ifndef ANALYSE_MANDELBROT_H
#define ANALYSE_MANDELBROT_H

#include <stdlib.h>

#include "mandelbrot.h"

#include "settings.h"

const size_t kNumberOfIterations = 1000;
const size_t kFrequency          = 2'000'000'000;

enum MandelbrotError AnalyseMandelbrot (settings_of_program_t set,
                                        enum MandelbrotError (*MandelbrotFunc) (int* const iteration_stop_arr,
                                                                                const settings_of_program_t settings));

#endif // ANALYSE_MANDELBROT_H
