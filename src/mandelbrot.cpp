#include "mandelbrot.h"

#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include <xmmintrin.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "My_lib/Logger/logging.h"

static enum MandelbrotError AnalyseKey (const sf::Event event,
                                        float* const x_base, float* const y_base, float* const scale);

MandelbrotError DrawMandelbrot (void)
{
    time_t before = time (NULL);

    sf::RenderWindow window (sf::VideoMode ({kWindowWidth, kWindowHeight}), kWindowTitle);

    float x_base = kStartCoordinatesX;
    float y_base = kStartCoordinatesY;

    float scale = kScale;

    sf::VertexArray vertexes (sf::PrimitiveType::Points, kWindowHeight * kWindowWidth);

    sf::Event event;

    size_t iteration_num = 0;

    while (window.isOpen())
    {
        for (size_t y_check = 0; y_check < kWindowHeight; y_check++)
        {
            for (size_t x_check = 0; x_check < kWindowWidth; x_check += kNumVertexes)
            {
                float x_start [kNumVertexes] = {((float) x_check     - x_base)  / scale,
                                                ((float) x_check + 1 - x_base)  / scale,
                                                ((float) x_check + 2 - x_base)  / scale,
                                                ((float) x_check + 3 - x_base)  / scale};
                float y_start [kNumVertexes] = {(y_base  - (float) y_check) / scale,
                                                (y_base  - (float) y_check) / scale,
                                                (y_base  - (float) y_check) / scale,
                                                (y_base  - (float) y_check) / scale};

                __m128 x_cur = *((__m128*)x_start);
                __m128 y_cur = *((__m128*)y_start);

                int iteration_stop [kNumVertexes] = {};

                for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
                {
                    unsigned int not_cmp_square_int [kNumVertexes] = {UINT32_MAX};
                    __m128 not_cmp_square = *((__m128*)not_cmp_square_int);

                    float arr_of_two [kNumVertexes] = {2, 2, 2, 2};

                    int mask = 0;

                    __m128 x_mul_y  = _mm_mul_ps (x_cur, y_cur);
                    __m128 x_square = _mm_mul_ps (x_cur, x_cur);
                    __m128 y_square = _mm_mul_ps (y_cur, y_cur);

                    y_cur = _mm_mul_ps (x_mul_y,  *((__m128*)arr_of_two));
                    x_cur = _mm_sub_ps (x_square, y_square);

                    x_square = _mm_add_ps (x_square, y_square);

                    y_cur = _mm_add_ps (y_cur, *((__m128*)y_start));
                    x_cur = _mm_add_ps (x_cur, *((__m128*)x_start));

                    __m128 cmp_square = _mm_cmple_ps (x_square, *((__m128*)kArrMaxModuleComplex));
                    mask = _mm_movemask_ps (cmp_square);

                    not_cmp_square = _mm_sub_epi32 (not_cmp_square, cmp_square);

                    *((__m128i*) iteration_stop) = _mm_sub_epi32 (*((__m128i*)iteration_stop), cmp_square);
                    x_cur = _mm_or_ps (x_cur, not_cmp_square);
                    y_cur = _mm_or_ps (y_cur, not_cmp_square);

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
            fprintf (stderr, "Done %lu times for %ld seconds\n", kNumberOfIterations, after - before);
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
            }
        }
    }

    return kDoneMandelbrot;
}

static enum MandelbrotError AnalyseKey (const sf::Event event,
                                        float* const x_base, float* const y_base, float* const scale)
{
    if (event.type == sf::Event::Closed)
    {
        return kCloseWindow;
    }

    if (event.KeyPressed)
    {
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
            *scale += kScaleShift;
            return kDoneMandelbrot;
        }
        if ((scancode == kMinus) && (*scale > kScaleShift))
        {
            *scale -= kScaleShift;
            return kDoneMandelbrot;
        }
    }

    return kDoneMandelbrot;
}
