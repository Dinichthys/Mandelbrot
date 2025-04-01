#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>

#include "mandelbrot.h"

#include "settings.h"

enum MoveMandelbrot
{
    kRight = 86,
    kLeft  = 87,
    kDown  = 88,
    kUp    = 89,

    kMinus = 41,
    kPlus  = 42,
};

const signed long kWindowHeight = 720;
const signed long kWindowWidth  = 1080;

const float kStartCoordinatesX = kWindowWidth  / 2;
const float kStartCoordinatesY = kWindowHeight / 2;

const char* const kWindowTitleNaive           = "MandelbrotNaive";
const char* const kWindowTitleArray           = "MandelbrotArray";
const char* const kWindowTitle128             = "Mandelbrot128";
const char* const kWindowTitle256             = "Mandelbrot256";
const char* const kWindowTitle256FullPipeLine = "Mandelbrot256FullPipeLine";

const float kScale               = 300;
const float kScaleShift          = 10;
const signed long kShift         = 100;

enum MandelbrotError DrawMandelbrot (settings_of_program_t set,
                                     enum MandelbrotError (*MandelbrotFunc) (int* const iteration_stop_arr,
                                                                             const settings_of_program_t settings),
                                     const char* const window_name);

#endif // DRAW_H
