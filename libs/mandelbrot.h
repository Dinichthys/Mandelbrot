#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stdlib.h>
#include <immintrin.h>

enum MandelbrotError
{
    kDoneMandelbrot = 0,

    kCloseWindow = 1,
};

enum MoveMandelbrot
{
    kRight = 86,
    kLeft  = 87,
    kDown  = 88,
    kUp    = 89,

    kMinus = 41,
    kPlus  = 42,
};

const size_t kNumVertexes         = 4;
const size_t kNumVertexesOptimize = 8;

const signed long kWindowHeight = 720;
const signed long kWindowWidth  = 1080;

const float kStartCoordinatesX = kWindowWidth  / 2;
const float kStartCoordinatesY = kWindowHeight / 2;

const char* const kWindowTitleDefault = "MandelbrotDefault";
const char* const kWindowTitleArray   = "MandelbrotArray";
const char* const kWindowTitle128     = "Mandelbrot128";
const char* const kWindowTitle256     = "Mandelbrot256";

const size_t kMaxNumIteration    = 256;
const float kMaxModuleComplex   = 100;
const float kArrMaxModuleComplex [kNumVertexes] = {kMaxModuleComplex, kMaxModuleComplex, kMaxModuleComplex, kMaxModuleComplex};

const float kScale               = 300;
const float kScaleShift          = 10;
const size_t kNumberOfIterations = 100;
const signed long kShift         = 100;


enum MandelbrotError DrawMandelbrotDefault (void);
enum MandelbrotError DrawMandelbrotArray   (void);
enum MandelbrotError DrawMandelbrot128     (void);
enum MandelbrotError DrawMandelbrot256     (void);

#endif // MANDELBROT_H
