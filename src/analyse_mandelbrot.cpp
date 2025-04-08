#include "mandelbrot.h"
#include "analyse_mandelbrot.h"

#include <stdlib.h>
#include <x86intrin.h>

#include "My_lib/Assert/my_assert.h"

enum MandelbrotError AnalyseMandelbrot (settings_of_program_t set,
                                          enum MandelbrotError (*MandelbrotFunc) (int* const iteration_stop_arr,
                                                                                  const settings_of_program_t settings))
{
    ASSERT (MandelbrotFunc != NULL, "AnalyseMandelbrot got MandelbrotFunc as a null ptr in argument.\n");

    size_t screen_height = set.window_height;
    size_t screen_width  = set.window_width +
                                          ((kNumVertexesOptimize * kNumberOfLoopUnrolling)
                                            - (set.window_width % (kNumVertexesOptimize * kNumberOfLoopUnrolling))
                                          % (kNumVertexesOptimize * kNumberOfLoopUnrolling));

    set.window_width  = screen_width;
    set.window_height = screen_height;

    unsigned int aux = 0;
    unsigned long long start = 0;
    unsigned long long end   = 0;

    for (size_t iteration_index = 0; iteration_index < kNumberOfIterations; iteration_index++)
    {
        start = __rdtscp (&aux);
        MandelbrotFunc (NULL, set);
        end = __rdtscp (&aux);
        fprintf (set.out_file, "%llu\n", end - start);
    }

    return kDoneMandelbrot;
}
