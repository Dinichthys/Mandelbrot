#include "mandelbrot.h"

#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <memory.h>

#include <xmmintrin.h>
#include <immintrin.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "My_lib/Logger/logging.h"

static enum MandelbrotError AnalyseKey (const sf::Event event,
                                        float* const x_base, float* const y_base, float* const scale);

MandelbrotError DrawMandelbrotDefault (void)
{
    time_t before = time (NULL);

    sf::RenderWindow window (sf::VideoMode ({kWindowWidth, kWindowHeight}), kWindowTitleDefault);

    float x_base = kStartCoordinatesX;
    float y_base = kStartCoordinatesY;

    float scale = kScale;

    sf::VertexArray vertexes (sf::PrimitiveType::Points, kWindowHeight * kWindowWidth);

    sf::Event event;

    size_t iteration_num = 0;

    while (true)
    {
        for (size_t y_check = 0; y_check < kWindowHeight; y_check++)
        {
            float y_start = (y_base - (float) y_check) / scale;

            for (size_t x_check = 0; x_check < kWindowWidth; x_check++)
            {
                float x_start = ((float) x_check - x_base) / scale;

                float x_cur = x_start;
                float y_cur = y_start;

                int iteration_stop = 0;

                for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
                {
                    float square_x = x_cur*x_cur;
                    float square_y = y_cur*y_cur;
                    float dub_x_mul_y  = 2 * x_cur * y_cur;

                    if (square_x + square_y < kMaxModuleComplex)
                    {
                        iteration_stop++;
                        x_cur = square_x - square_y + x_start;
                        y_cur = dub_x_mul_y + y_start;
                    }
                    else
                    {
                        break;
                    }
                }

                size_t ver_index = y_check * kWindowWidth + x_check;
                vertexes [ver_index].position = {(float) x_check, (float) y_check};

                vertexes [ver_index].color.r = iteration_stop;
                vertexes [ver_index].color.g = iteration_stop;
                vertexes [ver_index].color.b = iteration_stop;
            }
        }

        iteration_num++;

        if (iteration_num == kNumberOfIterations)
        {
            time_t after = time (NULL);
            fprintf (stderr, "Done default \n%lu times for %ld seconds\n", kNumberOfIterations, after - before);
            window.draw (vertexes);
            window.display ();
            before = time (NULL);
            iteration_num = 0;
        }
        else
        {
            window.draw (vertexes);
            window.display ();
        }

        if (window.pollEvent(event))
        {
            if (AnalyseKey (event, &x_base, &y_base, &scale) == kCloseWindow)
            {
                window.close ();
                break;
            }
        }
    }

    vertexes.clear ();

    return kDoneMandelbrot;
}

MandelbrotError DrawMandelbrotArray (void)
{
    time_t before = time (NULL);

    sf::RenderWindow window (sf::VideoMode ({kWindowWidth, kWindowHeight}), kWindowTitleArray);

    float x_base = kStartCoordinatesX;
    float y_base = kStartCoordinatesY;

    float scale = kScale;

    sf::VertexArray vertexes (sf::PrimitiveType::Points, kWindowHeight * kWindowWidth);

    sf::Event event;

    size_t iteration_num = 0;

    while (true)
    {
        for (size_t y_check = 0; y_check < kWindowHeight; y_check++)
        {
            float y_start [kNumVertexes] = {};
            for (size_t index = 0; index < kNumVertexes; index++)
            {
                y_start [index] = (y_base  - (float) y_check) / scale;
            }

            for (size_t x_check = 0; x_check < kWindowWidth; x_check += kNumVertexes)
            {
                float x_start [kNumVertexes] = {};

                float x_cur [kNumVertexes] = {};
                float y_cur [kNumVertexes] = {};

                for (size_t index = 0; index < kNumVertexes; index++)
                {
                    x_start [index] = ((float) x_check + index - x_base) / scale;

                    x_cur [index] = x_start [index];
                    y_cur [index] = y_start [index];
                }

                int iteration_stop [kNumVertexes] = {};

                for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
                {
                    float square_x [kNumVertexes] = {};
                    float square_y [kNumVertexes] = {};
                    float dub_x_mul_y  [kNumVertexes] = {};

                    int mask = 0;

                    for (size_t index = 0; index < kNumVertexes; index++)
                    {
                        square_x [index] = x_cur [index] * x_cur [index];
                        square_y [index] = y_cur [index] * y_cur [index];
                        dub_x_mul_y  [index] = 2 * x_cur [index] * y_cur [index];

                        if (square_x [index] + square_y [index] < kMaxModuleComplex)
                        {
                            mask++;
                            (iteration_stop [index])++;
                            x_cur [index] = square_x [index] - square_y [index] + x_start [index];
                            y_cur [index] = dub_x_mul_y [index] + y_start [index];
                        }
                    }

                    if (mask == 0)
                    {
                        break;
                    }
                }

                size_t ver_index = y_check * kWindowWidth + x_check;
                for (size_t point_index = 0; point_index < kNumVertexes; point_index++)
                {
#include "colors/color_good.cpp"
                    ver_index++;
                }
            }
        }

        iteration_num++;

        if (iteration_num == kNumberOfIterations)
        {
            time_t after = time (NULL);
            fprintf (stderr, "Done for arrays\n%lu times for %ld seconds\n", kNumberOfIterations, after - before);
            window.draw (vertexes);
            window.display ();
            before = time (NULL);
            iteration_num = 0;
        }
        else
        {
            window.draw (vertexes);
            window.display ();
        }

        if (window.pollEvent(event))
        {
            if (AnalyseKey (event, &x_base, &y_base, &scale) == kCloseWindow)
            {
                window.close ();
                break;
            }
        }
    }

    vertexes.clear ();

    return kDoneMandelbrot;
}

enum MandelbrotError DrawMandelbrot256 (void)
{
    time_t before = time (NULL);

    sf::RenderWindow window (sf::VideoMode ({kWindowWidth, kWindowHeight}), kWindowTitle256);

    sf::VertexArray vertexes (sf::PrimitiveType::Points, kWindowHeight * kWindowWidth);

    sf::Event event;

    float x_base = kStartCoordinatesX;
    float y_base = kStartCoordinatesY;

    float scale = kScale;

    const unsigned int kFullBitsInt = 0xFF'FF'FF'FF;
    float kFullBitsFloat = 0;

    memcpy (&kFullBitsFloat, &kFullBitsInt, sizeof (kFullBitsInt));

    size_t iteration_num = 0;

    while (true)
    {
        for (size_t y_check = 0; y_check < kWindowHeight; y_check++)
        {
            __m256 scale_256 = _mm256_set1_ps (scale);

            __m256 y_base_256 = _mm256_set1_ps (y_base);
            __m256 y_start   = _mm256_set1_ps ((float) y_check);
            y_start = _mm256_sub_ps (y_base_256, y_start);
            y_start = _mm256_div_ps (y_start, scale_256);

            for (size_t x_check = 0; x_check < kWindowWidth; x_check += kNumVertexesOptimize)
            {
                __m256 x_base_256 = _mm256_set1_ps (x_base);
                __m256 x_start   = _mm256_set1_ps ((float) x_check);
                __m256 temp      = _mm256_set_ps (7, 6, 5, 4, 3, 2, 1, 0);

                x_start = _mm256_add_ps (x_start, temp);
                x_start = _mm256_sub_ps (x_start, x_base_256);
                x_start = _mm256_div_ps (x_start, scale_256);

                __m256 x_cur = x_start;
                __m256 y_cur = y_start;

                alignas (__m256i) int iteration_stop [kNumVertexesOptimize] = {};
                __m256i iteration_stop_256 = _mm256_load_si256 ((__m256i*) iteration_stop);

                const __m256 kArrMaxModuleComplex256 = _mm256_set1_ps (kMaxModuleComplex);

                __m256 x_square = _mm256_mul_ps (x_cur, x_cur);
                __m256 y_square = _mm256_mul_ps (y_cur, y_cur);
                __m256 x_mul_y  = _mm256_mul_ps (x_cur, y_cur);

                __m256 arr_of_two = _mm256_set1_ps (2);
                __m256 max_float_256 = _mm256_set1_ps (kFullBitsFloat);

                __m256 x_square_cmp = _mm256_add_ps (x_square, y_square);

                __m256 sub_square = _mm256_sub_ps (x_square, y_square);
                __m256 x_mul_y_mul_two = _mm256_mul_ps (x_mul_y, arr_of_two);

                for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
                {
                    __m256 cmp_square_mask = _mm256_cmp_ps (x_square_cmp, kArrMaxModuleComplex256, _CMP_LE_OS);

                    iteration_stop_256 = _mm256_sub_epi32 (iteration_stop_256, (__m256i) cmp_square_mask);

                   int mask = _mm256_movemask_ps (cmp_square_mask);

                    cmp_square_mask = (__m256) _mm256_sub_epi32 ((__m256i) max_float_256,(__m256i) cmp_square_mask);

                    x_cur = _mm256_or_ps (cmp_square_mask, x_cur);
                    y_cur = _mm256_or_ps (cmp_square_mask, y_cur);

                    if (mask == 0)
                    {
                        break;
                    }

                    x_square = _mm256_mul_ps (x_cur, x_cur);
                    y_square = _mm256_mul_ps (y_cur, y_cur);
                    x_mul_y  = _mm256_mul_ps (x_cur, y_cur);

                    x_square_cmp = _mm256_add_ps (x_square, y_square);

                    sub_square = _mm256_sub_ps (x_square, y_square);
                    x_mul_y_mul_two = _mm256_mul_ps (x_mul_y, arr_of_two);

                    x_cur = _mm256_add_ps (sub_square, x_start);
                    y_cur = _mm256_add_ps (x_mul_y_mul_two, y_start);
                }
                _mm256_storeu_si256 ((__m256i*) iteration_stop, iteration_stop_256);

                size_t ver_index = y_check * kWindowWidth + x_check;
                for (size_t point_index = 0; point_index < kNumVertexesOptimize; point_index++)
                {
#include "colors/color_good.cpp"
                    ver_index++;
                }
            }
        }

        iteration_num++;

        if (iteration_num == kNumberOfIterations)
        {
            time_t after = time (NULL);
            fprintf (stderr, "Done for 256 \n%lu times for %ld seconds\n", kNumberOfIterations, after - before);
            window.draw (vertexes);
            window.display ();
            before = time (NULL);
            iteration_num = 0;
        }
        else
        {
            window.draw (vertexes);
            window.display ();
        }

        if (window.pollEvent(event))
        {
            if (AnalyseKey (event, &x_base, &y_base, &scale) == kCloseWindow)
            {
                window.close ();
                break;
            }
        }
    }

    vertexes.clear ();

    return kDoneMandelbrot;
}

enum MandelbrotError DrawMandelbrot256FullPipeLine (void)
{
    size_t screen_height = kWindowHeight;
    size_t screen_width  = kWindowWidth +
                                          (kNumVertexesOptimizeFullPipeLine
                                            - (kWindowWidth % kNumVertexesOptimizeFullPipeLine))
                                          % kNumVertexesOptimizeFullPipeLine;

    sf::RenderWindow window (sf::VideoMode ({(unsigned int) screen_width, (unsigned int) screen_height}),
                             kWindowTitle256FullPipeLine);

    sf::VertexArray vertexes (sf::PrimitiveType::Points, screen_height * screen_width);

    sf::Event event;

    float x_base = kStartCoordinatesX;
    float y_base = kStartCoordinatesY;

    float scale = kScale;

    const unsigned int kFullBitsInt = 0xFF'FF'FF'FF;

    alignas (__m256i) int iteration_stop_1 [kNumVertexesOptimize] = {};
    __m256i iteration_stop_256_1 = _mm256_load_si256 ((__m256i*) iteration_stop_1);

    alignas (__m256i) int iteration_stop_2 [kNumVertexesOptimize] = {};
    __m256i iteration_stop_256_2 = _mm256_load_si256 ((__m256i*) iteration_stop_2);

    const __m256 kArrMaxModuleComplex256 = _mm256_set1_ps (kMaxModuleComplex);

    const __m256 kArrOfTwo = _mm256_set1_ps (2);
    const __m256i kMaxInt32U256 = _mm256_set1_epi32 (kFullBitsInt);

    size_t iteration_num = 0;
    time_t before = time (NULL);

    while (true)
    {
        for (size_t y_check = 0; y_check < screen_height; y_check++)
        {
            __m256 scale_256 = _mm256_set1_ps (scale);

            __m256 y_base_256 = _mm256_set1_ps (y_base);
            __m256 y_start    = _mm256_set1_ps ((float) y_check);
            y_start = _mm256_sub_ps (y_base_256, y_start);
            y_start = _mm256_div_ps (y_start, scale_256);

            for (size_t x_check = 0; x_check < screen_width; x_check += kNumVertexesOptimizeFullPipeLine)
            {
                __m256 x_start_1  = _mm256_set1_ps ((float) x_check);
                __m256 x_base_256 = _mm256_set1_ps (x_base);
                __m256 temp_1     = _mm256_set_ps ( 7,  6,  5,  4,  3,  2,  1,  0);
                __m256 temp_2     = _mm256_set_ps (15, 14, 13, 12, 11, 10,  9,  8);
                __m256 x_start_2  = x_start_1;

                x_start_1 = _mm256_add_ps (x_start_1, temp_1);
                x_start_2 = _mm256_add_ps (x_start_2, temp_2);

                x_start_1 = _mm256_sub_ps (x_start_1, x_base_256);
                x_start_2 = _mm256_sub_ps (x_start_2, x_base_256);

                x_start_1 = _mm256_div_ps (x_start_1, scale_256);
                x_start_2 = _mm256_div_ps (x_start_2, scale_256);

                __m256 y_cur_1 = y_start;
                __m256 y_cur_2 = y_start;

                __m256 x_cur_1 = x_start_1;
                __m256 x_cur_2 = x_start_2;

                iteration_stop_256_1 = _mm256_setzero_si256 ();
                iteration_stop_256_2 = _mm256_setzero_si256 ();

                for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
                {
                    __m256 x_square_1 = _mm256_mul_ps (x_cur_1, x_cur_1);
                    __m256 y_square_1 = _mm256_mul_ps (y_cur_1, y_cur_1);
                    __m256 x_mul_y_1  = _mm256_mul_ps (x_cur_1, y_cur_1);

                    __m256 x_square_2 = _mm256_mul_ps (x_cur_2, x_cur_2);
                    __m256 y_square_2 = _mm256_mul_ps (y_cur_2, y_cur_2);
                    __m256 x_mul_y_2  = _mm256_mul_ps (x_cur_2, y_cur_2);

                    __m256 x_square_cmp_1 = _mm256_add_ps (x_square_1, y_square_1);
                    __m256 x_square_cmp_2 = _mm256_add_ps (x_square_2, y_square_2);

                    __m256 sub_square_1 = _mm256_sub_ps (x_square_1, y_square_1);
                    __m256 sub_square_2 = _mm256_sub_ps (x_square_2, y_square_2);
                    __m256 x_mul_y_mul_two_1 = _mm256_mul_ps (x_mul_y_1, kArrOfTwo);
                    __m256 x_mul_y_mul_two_2 = _mm256_mul_ps (x_mul_y_2, kArrOfTwo);

                    __m256 cmp_square_mask_1 = _mm256_cmp_ps (x_square_cmp_1, kArrMaxModuleComplex256, _CMP_LE_OS);
                    __m256 cmp_square_mask_2 = _mm256_cmp_ps (x_square_cmp_2, kArrMaxModuleComplex256, _CMP_LE_OS);

                   int mask = _mm256_movemask_ps (cmp_square_mask_1)
                              + _mm256_movemask_ps (cmp_square_mask_2);

                    iteration_stop_256_1 = _mm256_sub_epi32 (iteration_stop_256_1, (__m256i) cmp_square_mask_1);
                    iteration_stop_256_2 = _mm256_sub_epi32 (iteration_stop_256_2, (__m256i) cmp_square_mask_2);

                    cmp_square_mask_1 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_1);
                    cmp_square_mask_2 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_2);

                    if (mask == 0)
                    {
                        break;
                    }

                    x_cur_1 = _mm256_add_ps (sub_square_1, x_start_1);
                    y_cur_1 = _mm256_add_ps (x_mul_y_mul_two_1, y_start);

                    x_cur_2 = _mm256_add_ps (sub_square_2, x_start_2);
                    y_cur_2 = _mm256_add_ps (x_mul_y_mul_two_2, y_start);

                    x_cur_1 = _mm256_or_ps (cmp_square_mask_1, x_cur_1);
                    y_cur_1 = _mm256_or_ps (cmp_square_mask_1, y_cur_1);

                    x_cur_2 = _mm256_or_ps (cmp_square_mask_2, x_cur_2);
                    y_cur_2 = _mm256_or_ps (cmp_square_mask_2, y_cur_2);
                }
                _mm256_storeu_si256 ((__m256i*) iteration_stop_1, iteration_stop_256_1);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_2, iteration_stop_256_2);

                size_t ver_index = y_check * kWindowWidth + x_check;
                for (size_t point_index = 0; point_index < kNumVertexesOptimize; point_index++)
                {
                    vertexes [ver_index].position = {(float) x_check + (float) point_index, (float) y_check};

                    vertexes [ver_index].color.r = iteration_stop_1 [point_index] << 2;
                    vertexes [ver_index].color.g = iteration_stop_1 [point_index] << 1;
                    vertexes [ver_index].color.b = iteration_stop_1 [point_index];

                    vertexes [ver_index + kNumVertexesOptimize].position =
                                {(float) x_check + kNumVertexesOptimize + (float) point_index, (float) y_check};

                    vertexes [ver_index + kNumVertexesOptimize].color.r = iteration_stop_2 [point_index] << 2;
                    vertexes [ver_index + kNumVertexesOptimize].color.g = iteration_stop_2 [point_index] << 1;
                    vertexes [ver_index + kNumVertexesOptimize].color.b = iteration_stop_2 [point_index];

                    ver_index++;
                }
            }
        }

        iteration_num++;

        if (iteration_num == kNumberOfIterations)
        {
            time_t after = time (NULL);
            fprintf (stderr, "Done for 256 in full pipeline \n%lu times for %ld seconds\n", kNumberOfIterations, after - before);
            window.draw (vertexes);
            window.display ();
            before = time (NULL);
            iteration_num = 0;
        }
        else
        {
            window.draw (vertexes);
            window.display ();
        }

        if (window.pollEvent(event))
        {
            if (AnalyseKey (event, &x_base, &y_base, &scale) == kCloseWindow)
            {
                window.close ();
                break;
            }
        }
    }

    vertexes.clear ();

    return kDoneMandelbrot;
}

static enum MandelbrotError AnalyseKey (const sf::Event event,
                                        float* const x_base, float* const y_base, float* const scale)
{
    if (event.type == sf::Event::Closed)
    {
        return kCloseWindow;
    }

    enum MoveMandelbrot scancode = (MoveMandelbrot) event.key.scancode;
    if (scancode == kLeft)
    {
        *x_base += kShift;
        return kDoneMandelbrot;
    }
    if (scancode == kRight)
    {
        *x_base -= kShift;
        return kDoneMandelbrot;
    }
    if (scancode == kDown)
    {
        *y_base -= kShift;
        return kDoneMandelbrot;
    }
    if (scancode == kUp)
    {
        *y_base += kShift;
        return kDoneMandelbrot;
    }

    if (scancode == kPlus)
    {
        *x_base -= (kStartCoordinatesX - *x_base) * kScaleShift / 100;
        *y_base -= (kStartCoordinatesY - *y_base) * kScaleShift / 100;
        *scale += kScaleShift * *scale / 100;
        // (kStartCoordinates - x_base) / scale = (kStartCoordinates - (x_base + a)) / (scale + kScaleShift * scale / 100)
        //  kStartCoordinates * (scale + kScaleShift * scale / 100) - x_base * (scale + kScaleShift * scale / 100)  = kStartCoordinates * (scale) - (x_base + a) * (scale)
        //  kStartCoordinates * kScaleShift * scale / 100 - x_base * kScaleShift * scale / 100 = - a * scale
        return kDoneMandelbrot;
    }
    if ((scancode == kMinus) && (*scale > kScaleShift))
    {
        *x_base += (kStartCoordinatesX - *x_base) * kScaleShift / 100;
        *y_base += (kStartCoordinatesY - *y_base) * kScaleShift / 100;
        *scale -= kScaleShift * *scale / 100;
        return kDoneMandelbrot;
    }

    return kDoneMandelbrot;
}
