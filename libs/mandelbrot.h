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

    kNaive                          = 1,
    kArray                          = 2,
    k256Intrin                      = 3,
    k256IntrinFullPipelineTwoIter   = 4,
    k256IntrinFullPipelineThreeIter = 5,
    k256IntrinFullPipelineFourIter  = 6,
    k256IntrinFullPipelineFiveIter  = 7,
};

const size_t kNumVertices         =  4;
const size_t kNumVerticesOptimize =  8;

const size_t kNumberOfLoopUnrolling = 5;

const size_t kIntrinsicSize = 256;

const size_t kCPUFrequency = 2'000'000'000;

const size_t kMaxNumIteration = 256;
const float kMaxModuleComplex = 100;
const float kArrMaxModuleComplex [kNumVertices] = {kMaxModuleComplex, kMaxModuleComplex, kMaxModuleComplex, kMaxModuleComplex};

#include "draw.h"
#include "analyse_mandelbrot.h"

enum MandelbrotError MandelbrotNaive                    (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError MandelbrotArray                    (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError Mandelbrot256                      (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError Mandelbrot256FullPipeLineTwoIter   (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError Mandelbrot256FullPipeLineThreeIter (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError Mandelbrot256FullPipeLineFourIter  (int* const iteration_stop_arr, const settings_of_program_t settings);
enum MandelbrotError Mandelbrot256FullPipeLineFiveIter  (int* const iteration_stop_arr, const settings_of_program_t settings);

#endif // MANDELBROT_H
