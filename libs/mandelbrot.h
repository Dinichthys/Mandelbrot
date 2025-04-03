#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stdlib.h>
#include <immintrin.h>

enum MandelbrotError
{
    kDoneMandelbrot = 0,

    kCloseWindow              = 1,
    kCantCallocIterationArray = 2,
    kCantLoadFont             = 3,
};

enum MandelbrotVersion
{
    kAll = 0,

    kNaive                 = 1,
    kArray                 = 2,
    k256Intrin             = 3,
    k256IntrinFullPipeline = 4,
};

const size_t kNumVertexes                     =  4;
const size_t kNumVertexesOptimize             =  8;
const size_t kNumVertexesOptimizeFullPipeLine = 16;

const size_t kMaxNumIteration = 256;
const float kMaxModuleComplex = 100;
const float kArrMaxModuleComplex [kNumVertexes] = {kMaxModuleComplex, kMaxModuleComplex, kMaxModuleComplex, kMaxModuleComplex};

#include "draw.h"
#include "analyse_mandelbrot.h"

enum MandelbrotError MandelbrotNaive           (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError MandelbrotArray           (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError Mandelbrot256             (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError Mandelbrot256FullPipeLine (int* const iteration_stop_arr, const settings_of_program_t settings);


#endif // MANDELBROT_H
