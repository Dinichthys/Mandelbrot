#include "mandelbrot.h"

#include <stdlib.h>
#include <limits.h>
#include <memory.h>

#include <xmmintrin.h>
#include <immintrin.h>

#include "My_lib/Logger/logging.h"
#include "My_lib/Assert/my_assert.h"

MandelbrotError MandelbrotNaive (int* const iteration_stop_arr, const settings_of_program_t settings)
{
    ASSERT ((settings.graphic_mode) && (iteration_stop_arr != NULL), "MandelbrotNative got iteration_stop_arr as null ptr.\n");

    size_t screen_width  = settings.window_width;
    size_t screen_height = settings.window_height;

    float x_base = settings.coordinates_x;
    float y_base = settings.coordinates_y;

    float scale = settings.scale;

    for (size_t y_check = 0; y_check < screen_height; y_check++)
    {
        float y_start = (y_base - (float) y_check) / scale;

        for (size_t x_check = 0; x_check < screen_width; x_check++)
        {
            float x_start = ((float) x_check - x_base) / scale;

            float x_cur = x_start;
            float y_cur = y_start;

            volatile int iteration_stop = 0;

            for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
            {
                float square_x = x_cur * x_cur;
                float square_y = y_cur * y_cur;
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

            if (settings.graphic_mode)
            {
                size_t ver_index = y_check * screen_width + x_check;
                iteration_stop_arr [ver_index] = iteration_stop;
            }
        }
    }

    return kDoneMandelbrot;
}

#define _ARRAY_FOR_INSTRUCTION(body_for)                    \
            for (index = 0; index < kNumVertices; index++)  \
            {                                               \
                body_for;                                   \
            }

MandelbrotError MandelbrotArray (int* const iteration_stop_arr, const settings_of_program_t settings)
{
    ASSERT ((settings.graphic_mode) && (iteration_stop_arr != NULL), "MandelbrotArray got iteration_stop_arr as null ptr.\n");

    size_t screen_width  = settings.window_width;
    size_t screen_height = settings.window_height;

    float x_base = settings.coordinates_x;
    float y_base = settings.coordinates_y;

    float scale = settings.scale;

    size_t index = 0;

    for (size_t y_check = 0; y_check < screen_height; y_check++)
    {
        float y_start [kNumVertices] = {};
        _ARRAY_FOR_INSTRUCTION (y_start [index] = (y_base  - (float) y_check) / scale)

        for (size_t x_check = 0; x_check < screen_width; x_check += kNumVertices)
        {
            float x_start [kNumVertices] = {};
            _ARRAY_FOR_INSTRUCTION (x_start [index] = ((float) x_check + index - x_base) / scale)

            float x_cur [kNumVertices] = {};
            _ARRAY_FOR_INSTRUCTION (x_cur [index] = x_start [index])

            float y_cur [kNumVertices] = {};
            _ARRAY_FOR_INSTRUCTION (y_cur [index] = y_start [index])

            int iteration_stop [kNumVertices] = {};

            for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
            {
                float square_x [kNumVertices] = {};
                _ARRAY_FOR_INSTRUCTION (square_x [index] = x_cur [index] * x_cur [index])

                float square_y [kNumVertices] = {};
                _ARRAY_FOR_INSTRUCTION (square_y [index] = y_cur [index] * y_cur [index])

                float dub_x_mul_y [kNumVertices] = {};
                _ARRAY_FOR_INSTRUCTION (dub_x_mul_y  [index] = 2 * x_cur [index] * y_cur [index])

                int cmp [kNumVertices] = {};
                _ARRAY_FOR_INSTRUCTION (cmp [index] = (square_x [index] + square_y [index] < kMaxModuleComplex))

                _ARRAY_FOR_INSTRUCTION (iteration_stop [index] += cmp [index])

                int mask = 0;
                _ARRAY_FOR_INSTRUCTION (mask += cmp [index])

                _ARRAY_FOR_INSTRUCTION (x_cur [index] = square_x [index] - square_y [index] + x_start [index])
                _ARRAY_FOR_INSTRUCTION (y_cur [index] = dub_x_mul_y [index] + y_start [index])

                if (mask == 0)
                {
                    break;
                }
            }

            if (settings.graphic_mode)
            {
                size_t ver_index = y_check * screen_width + x_check;
                for (size_t point_index = 0; point_index < kNumVertices; point_index++)
                {
                    iteration_stop_arr [ver_index] = iteration_stop [point_index];
                    ver_index++;
                }
            }
        }
    }

    return kDoneMandelbrot;
}

#undef _ARRAY_FOR_INSTRUCTION

enum MandelbrotError Mandelbrot256 (int* const iteration_stop_arr, const settings_of_program_t settings)
{
    ASSERT ((settings.graphic_mode) && (iteration_stop_arr != NULL), "Mandelbrot256 got iteration_stop_arr as null ptr.\n");

    size_t screen_width  = settings.window_width;
    size_t screen_height = settings.window_height;

    float x_base = settings.coordinates_x;
    float y_base = settings.coordinates_y;

    float scale = settings.scale;

    const __m256 kArrOfTwo = _mm256_set1_ps (2);
    const unsigned int kFullBitsInt = 0xFF'FF'FF'FF;
    const __m256i kMaxInt32U256     = _mm256_set1_epi32 (kFullBitsInt);
    const __m256 kArrMaxModuleComplex256 = _mm256_set1_ps (kMaxModuleComplex);

    __m256 x_base_256 = _mm256_set1_ps (x_base);
    __m256 y_base_256 = _mm256_set1_ps (y_base);
    __m256 scale_256  = _mm256_set1_ps (scale);

    alignas (__m256i) int iteration_stop [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256 = _mm256_load_si256 ((__m256i*) iteration_stop);

    for (size_t y_check = 0; y_check < screen_height; y_check++)
    {
        __m256 y_start   = _mm256_set1_ps ((float) y_check);
        y_start = _mm256_sub_ps (y_base_256, y_start);
        y_start = _mm256_div_ps (y_start, scale_256);

        for (size_t x_check = 0; x_check < screen_width; x_check += kNumVerticesOptimize)
        {
            volatile int mask = 0;
            __m256 x_start   = _mm256_set1_ps ((float) x_check);
            __m256 temp      = _mm256_set_ps (7, 6, 5, 4, 3, 2, 1, 0);

            x_start = _mm256_add_ps (x_start, temp);
            x_start = _mm256_sub_ps (x_start, x_base_256);
            x_start = _mm256_div_ps (x_start, scale_256);

            __m256 x_cur = x_start;
            __m256 y_cur = y_start;

            iteration_stop_256 = _mm256_setzero_si256 ();

            for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
            {
                __m256 x_square = _mm256_mul_ps (x_cur, x_cur);
                __m256 y_square = _mm256_mul_ps (y_cur, y_cur);
                __m256 x_mul_y  = _mm256_mul_ps (x_cur, y_cur);

                __m256 x_square_cmp = _mm256_add_ps (x_square, y_square);

                __m256 sub_square      = _mm256_sub_ps (x_square, y_square);
                __m256 x_mul_y_mul_two = _mm256_mul_ps (x_mul_y, kArrOfTwo);

                __m256 cmp_square_mask = _mm256_cmp_ps (x_square_cmp, kArrMaxModuleComplex256, _CMP_LE_OS);

                iteration_stop_256 = _mm256_sub_epi32 (iteration_stop_256, (__m256i) cmp_square_mask);

                mask = _mm256_movemask_ps (cmp_square_mask);

                cmp_square_mask = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i) cmp_square_mask);

                if (mask == 0)
                {
                    break;
                }

                x_cur = _mm256_add_ps (sub_square, x_start);
                y_cur = _mm256_add_ps (x_mul_y_mul_two, y_start);

                x_cur = _mm256_or_ps (cmp_square_mask, x_cur);
                y_cur = _mm256_or_ps (cmp_square_mask, y_cur);
            }

            if (settings.graphic_mode)
            {
                _mm256_storeu_si256 ((__m256i*) iteration_stop, iteration_stop_256);

                size_t ver_index = y_check * screen_width + x_check;
                for (size_t point_index = 0; point_index < kNumVerticesOptimize; point_index++)
                {
                    iteration_stop_arr [ver_index] = iteration_stop [point_index];
                    ver_index++;
                }
            }
        }
    }

    return kDoneMandelbrot;
}

enum MandelbrotError Mandelbrot256FullPipeLineTwoIter (int* const iteration_stop_arr, const settings_of_program_t settings)
{
    ASSERT ((settings.graphic_mode) && (iteration_stop_arr != NULL), "Mandelbrot256FullPipeLine got iteration_stop_arr as null ptr.\n");

    size_t screen_width  = settings.window_width;
    size_t screen_height = settings.window_height;

    float x_base = settings.coordinates_x;
    float y_base = settings.coordinates_y;

    float scale = settings.scale;

    size_t delta_x = kNumVerticesOptimize * 2;

    alignas (__m256i) int iteration_stop_1 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_1 = _mm256_load_si256 ((__m256i*) iteration_stop_1);

    alignas (__m256i) int iteration_stop_2 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_2 = _mm256_load_si256 ((__m256i*) iteration_stop_2);

    const __m256 kArrMaxModuleComplex256 = _mm256_set1_ps (kMaxModuleComplex);

    const __m256 kArrOfTwo = _mm256_set1_ps (2);
    const unsigned int kFullBitsInt = 0xFF'FF'FF'FF;
    const __m256i kMaxInt32U256     = _mm256_set1_epi32 (kFullBitsInt);

    __m256 x_base_256 = _mm256_set1_ps (x_base);
    __m256 y_base_256 = _mm256_set1_ps (y_base);
    __m256 scale_256  = _mm256_set1_ps (scale);

    __m256 temp_1 = _mm256_set_ps ( 7,  6,  5,  4,  3,  2,  1,  0);
    __m256 temp_2 = _mm256_set_ps (15, 14, 13, 12, 11, 10,  9,  8);

    for (size_t y_check = 0; y_check < screen_height; y_check++)
    {
        __m256 y_start    = _mm256_set1_ps ((float) y_check);
        y_start = _mm256_sub_ps (y_base_256, y_start);
        y_start = _mm256_div_ps (y_start, scale_256);

        for (size_t x_check = 0; x_check < screen_width; x_check += delta_x)
        {
            __m256 x_start_1  = _mm256_set1_ps ((float) x_check);
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

            if (settings.graphic_mode)
            {
                _mm256_storeu_si256 ((__m256i*) iteration_stop_1, iteration_stop_256_1);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_2, iteration_stop_256_2);

                size_t ver_index = y_check * screen_width + x_check;
                for (size_t point_index = 0; point_index < kNumVerticesOptimize; point_index++)
                {
                    iteration_stop_arr [ver_index]                        = iteration_stop_1 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize] = iteration_stop_2 [point_index];
                    ver_index++;
                }
            }
        }
    }

    return kDoneMandelbrot;
}

enum MandelbrotError Mandelbrot256FullPipeLineThreeIter (int* const iteration_stop_arr, const settings_of_program_t settings)
{
    ASSERT ((settings.graphic_mode) && (iteration_stop_arr != NULL), "Mandelbrot256FullPipeLine got iteration_stop_arr as null ptr.\n");

    size_t screen_width  = settings.window_width;
    size_t screen_height = settings.window_height;

    float x_base = settings.coordinates_x;
    float y_base = settings.coordinates_y;

    float scale = settings.scale;

    size_t delta_x = kNumVerticesOptimize * 3;

    alignas (__m256i) int iteration_stop_1 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_1 = _mm256_load_si256 ((__m256i*) iteration_stop_1);

    alignas (__m256i) int iteration_stop_2 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_2 = _mm256_load_si256 ((__m256i*) iteration_stop_2);

    alignas (__m256i) int iteration_stop_3 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_3 = _mm256_load_si256 ((__m256i*) iteration_stop_3);

    const __m256 kArrMaxModuleComplex256 = _mm256_set1_ps (kMaxModuleComplex);

    const __m256 kArrOfTwo = _mm256_set1_ps (2);
    const unsigned int kFullBitsInt = 0xFF'FF'FF'FF;
    const __m256i kMaxInt32U256     = _mm256_set1_epi32 (kFullBitsInt);

    __m256 x_base_256 = _mm256_set1_ps (x_base);
    __m256 y_base_256 = _mm256_set1_ps (y_base);
    __m256 scale_256  = _mm256_set1_ps (scale);

    __m256 temp_1 = _mm256_set_ps ( 7,  6,  5,  4,  3,  2,   1,   0);
    __m256 temp_2 = _mm256_set_ps (15, 14, 13, 12, 11, 10,   9,   8);
    __m256 temp_3 = _mm256_set_ps (23, 22, 21, 20, 19, 18,  17,  16);

    for (size_t y_check = 0; y_check < screen_height; y_check++)
    {
        __m256 y_start    = _mm256_set1_ps ((float) y_check);
        y_start = _mm256_sub_ps (y_base_256, y_start);
        y_start = _mm256_div_ps (y_start, scale_256);

        for (size_t x_check = 0; x_check < screen_width; x_check += delta_x)
        {
            __m256 x_start_1  = _mm256_set1_ps ((float) x_check);
            __m256 x_start_2  = x_start_1;
            __m256 x_start_3  = x_start_1;

            x_start_1 = _mm256_add_ps (x_start_1, temp_1);
            x_start_2 = _mm256_add_ps (x_start_2, temp_2);
            x_start_3 = _mm256_add_ps (x_start_3, temp_3);

            x_start_1 = _mm256_sub_ps (x_start_1, x_base_256);
            x_start_2 = _mm256_sub_ps (x_start_2, x_base_256);
            x_start_3 = _mm256_sub_ps (x_start_3, x_base_256);

            x_start_1 = _mm256_div_ps (x_start_1, scale_256);
            x_start_2 = _mm256_div_ps (x_start_2, scale_256);
            x_start_3 = _mm256_div_ps (x_start_3, scale_256);

            __m256 y_cur_1 = y_start;
            __m256 y_cur_2 = y_start;
            __m256 y_cur_3 = y_start;

            __m256 x_cur_1 = x_start_1;
            __m256 x_cur_2 = x_start_2;
            __m256 x_cur_3 = x_start_3;

            iteration_stop_256_1 = _mm256_setzero_si256 ();
            iteration_stop_256_2 = _mm256_setzero_si256 ();
            iteration_stop_256_3 = _mm256_setzero_si256 ();

            for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
            {
                __m256 x_square_1 = _mm256_mul_ps (x_cur_1, x_cur_1);
                __m256 y_square_1 = _mm256_mul_ps (y_cur_1, y_cur_1);
                __m256 x_mul_y_1  = _mm256_mul_ps (x_cur_1, y_cur_1);

                __m256 x_square_2 = _mm256_mul_ps (x_cur_2, x_cur_2);
                __m256 y_square_2 = _mm256_mul_ps (y_cur_2, y_cur_2);
                __m256 x_mul_y_2  = _mm256_mul_ps (x_cur_2, y_cur_2);

                __m256 x_square_3 = _mm256_mul_ps (x_cur_3, x_cur_3);
                __m256 y_square_3 = _mm256_mul_ps (y_cur_3, y_cur_3);
                __m256 x_mul_y_3  = _mm256_mul_ps (x_cur_3, y_cur_3);

                __m256 x_square_cmp_1 = _mm256_add_ps (x_square_1, y_square_1);
                __m256 x_square_cmp_2 = _mm256_add_ps (x_square_2, y_square_2);
                __m256 x_square_cmp_3 = _mm256_add_ps (x_square_3, y_square_3);

                __m256 sub_square_1 = _mm256_sub_ps (x_square_1, y_square_1);
                __m256 sub_square_2 = _mm256_sub_ps (x_square_2, y_square_2);
                __m256 sub_square_3 = _mm256_sub_ps (x_square_3, y_square_3);

                __m256 x_mul_y_mul_two_1 = _mm256_mul_ps (x_mul_y_1, kArrOfTwo);
                __m256 x_mul_y_mul_two_2 = _mm256_mul_ps (x_mul_y_2, kArrOfTwo);
                __m256 x_mul_y_mul_two_3 = _mm256_mul_ps (x_mul_y_3, kArrOfTwo);

                __m256 cmp_square_mask_1 = _mm256_cmp_ps (x_square_cmp_1, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_2 = _mm256_cmp_ps (x_square_cmp_2, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_3 = _mm256_cmp_ps (x_square_cmp_3, kArrMaxModuleComplex256, _CMP_LE_OS);

                int mask = _mm256_movemask_ps (cmp_square_mask_1)
                            + _mm256_movemask_ps (cmp_square_mask_2)
                            + _mm256_movemask_ps (cmp_square_mask_3);

                iteration_stop_256_1 = _mm256_sub_epi32 (iteration_stop_256_1, (__m256i) cmp_square_mask_1);
                iteration_stop_256_2 = _mm256_sub_epi32 (iteration_stop_256_2, (__m256i) cmp_square_mask_2);
                iteration_stop_256_3 = _mm256_sub_epi32 (iteration_stop_256_3, (__m256i) cmp_square_mask_3);

                cmp_square_mask_1 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_1);
                cmp_square_mask_2 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_2);
                cmp_square_mask_3 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_3);

                if (mask == 0)
                {
                    break;
                }

                x_cur_1 = _mm256_add_ps (sub_square_1, x_start_1);
                y_cur_1 = _mm256_add_ps (x_mul_y_mul_two_1, y_start);

                x_cur_2 = _mm256_add_ps (sub_square_2, x_start_2);
                y_cur_2 = _mm256_add_ps (x_mul_y_mul_two_2, y_start);

                x_cur_3 = _mm256_add_ps (sub_square_3, x_start_3);
                y_cur_3 = _mm256_add_ps (x_mul_y_mul_two_3, y_start);

                x_cur_1 = _mm256_or_ps (cmp_square_mask_1, x_cur_1);
                y_cur_1 = _mm256_or_ps (cmp_square_mask_1, y_cur_1);

                x_cur_2 = _mm256_or_ps (cmp_square_mask_2, x_cur_2);
                y_cur_2 = _mm256_or_ps (cmp_square_mask_2, y_cur_2);

                x_cur_3 = _mm256_or_ps (cmp_square_mask_3, x_cur_3);
                y_cur_3 = _mm256_or_ps (cmp_square_mask_3, y_cur_3);
            }

            if (settings.graphic_mode)
            {
                _mm256_storeu_si256 ((__m256i*) iteration_stop_1, iteration_stop_256_1);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_2, iteration_stop_256_2);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_3, iteration_stop_256_3);

                size_t ver_index = y_check * screen_width + x_check;
                for (size_t point_index = 0; point_index < kNumVerticesOptimize; point_index++)
                {
                    iteration_stop_arr [ver_index]                            = iteration_stop_1 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize    ] = iteration_stop_2 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize * 2] = iteration_stop_3 [point_index];
                    ver_index++;
                }
            }
        }
    }

    return kDoneMandelbrot;
}

enum MandelbrotError Mandelbrot256FullPipeLineFourIter (int* const iteration_stop_arr, const settings_of_program_t settings)
{
    ASSERT ((settings.graphic_mode) && (iteration_stop_arr != NULL), "Mandelbrot256FullPipeLine got iteration_stop_arr as null ptr.\n");

    size_t screen_width  = settings.window_width;
    size_t screen_height = settings.window_height;

    float x_base = settings.coordinates_x;
    float y_base = settings.coordinates_y;

    float scale = settings.scale;

    size_t delta_x = kNumVerticesOptimize * 4;

    alignas (__m256i) int iteration_stop_1 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_1 = _mm256_load_si256 ((__m256i*) iteration_stop_1);

    alignas (__m256i) int iteration_stop_2 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_2 = _mm256_load_si256 ((__m256i*) iteration_stop_2);

    alignas (__m256i) int iteration_stop_3 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_3 = _mm256_load_si256 ((__m256i*) iteration_stop_3);

    alignas (__m256i) int iteration_stop_4 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_4 = _mm256_load_si256 ((__m256i*) iteration_stop_4);

    const __m256 kArrMaxModuleComplex256 = _mm256_set1_ps (kMaxModuleComplex);

    const __m256 kArrOfTwo = _mm256_set1_ps (2);
    const unsigned int kFullBitsInt = 0xFF'FF'FF'FF;
    const __m256i kMaxInt32U256     = _mm256_set1_epi32 (kFullBitsInt);

    __m256 x_base_256 = _mm256_set1_ps (x_base);
    __m256 y_base_256 = _mm256_set1_ps (y_base);
    __m256 scale_256  = _mm256_set1_ps (scale);

    __m256 temp_1 = _mm256_set_ps ( 7,  6,  5,  4,  3,  2,   1,   0);
    __m256 temp_2 = _mm256_set_ps (15, 14, 13, 12, 11, 10,   9,   8);
    __m256 temp_3 = _mm256_set_ps (23, 22, 21, 20, 19, 18,  17,  16);
    __m256 temp_4 = _mm256_set_ps (31, 30, 29, 28, 27, 26,  25,  24);

    for (size_t y_check = 0; y_check < screen_height; y_check++)
    {
        __m256 y_start    = _mm256_set1_ps ((float) y_check);
        y_start = _mm256_sub_ps (y_base_256, y_start);
        y_start = _mm256_div_ps (y_start, scale_256);

        for (size_t x_check = 0; x_check < screen_width; x_check += delta_x)
        {
            __m256 x_start_1  = _mm256_set1_ps ((float) x_check);
            __m256 x_start_2  = x_start_1;
            __m256 x_start_3  = x_start_1;
            __m256 x_start_4  = x_start_1;

            x_start_1 = _mm256_add_ps (x_start_1, temp_1);
            x_start_2 = _mm256_add_ps (x_start_2, temp_2);
            x_start_3 = _mm256_add_ps (x_start_3, temp_3);
            x_start_4 = _mm256_add_ps (x_start_4, temp_4);

            x_start_1 = _mm256_sub_ps (x_start_1, x_base_256);
            x_start_2 = _mm256_sub_ps (x_start_2, x_base_256);
            x_start_3 = _mm256_sub_ps (x_start_3, x_base_256);
            x_start_4 = _mm256_sub_ps (x_start_4, x_base_256);

            x_start_1 = _mm256_div_ps (x_start_1, scale_256);
            x_start_2 = _mm256_div_ps (x_start_2, scale_256);
            x_start_3 = _mm256_div_ps (x_start_3, scale_256);
            x_start_4 = _mm256_div_ps (x_start_4, scale_256);

            __m256 y_cur_1 = y_start;
            __m256 y_cur_2 = y_start;
            __m256 y_cur_3 = y_start;
            __m256 y_cur_4 = y_start;

            __m256 x_cur_1 = x_start_1;
            __m256 x_cur_2 = x_start_2;
            __m256 x_cur_3 = x_start_3;
            __m256 x_cur_4 = x_start_4;

            iteration_stop_256_1 = _mm256_setzero_si256 ();
            iteration_stop_256_2 = _mm256_setzero_si256 ();
            iteration_stop_256_3 = _mm256_setzero_si256 ();
            iteration_stop_256_4 = _mm256_setzero_si256 ();

            for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
            {
                __m256 x_square_1 = _mm256_mul_ps (x_cur_1, x_cur_1);
                __m256 y_square_1 = _mm256_mul_ps (y_cur_1, y_cur_1);
                __m256 x_mul_y_1  = _mm256_mul_ps (x_cur_1, y_cur_1);

                __m256 x_square_2 = _mm256_mul_ps (x_cur_2, x_cur_2);
                __m256 y_square_2 = _mm256_mul_ps (y_cur_2, y_cur_2);
                __m256 x_mul_y_2  = _mm256_mul_ps (x_cur_2, y_cur_2);

                __m256 x_square_3 = _mm256_mul_ps (x_cur_3, x_cur_3);
                __m256 y_square_3 = _mm256_mul_ps (y_cur_3, y_cur_3);
                __m256 x_mul_y_3  = _mm256_mul_ps (x_cur_3, y_cur_3);

                __m256 x_square_4 = _mm256_mul_ps (x_cur_4, x_cur_4);
                __m256 y_square_4 = _mm256_mul_ps (y_cur_4, y_cur_4);
                __m256 x_mul_y_4  = _mm256_mul_ps (x_cur_4, y_cur_4);

                __m256 x_square_cmp_1 = _mm256_add_ps (x_square_1, y_square_1);
                __m256 x_square_cmp_2 = _mm256_add_ps (x_square_2, y_square_2);
                __m256 x_square_cmp_3 = _mm256_add_ps (x_square_3, y_square_3);
                __m256 x_square_cmp_4 = _mm256_add_ps (x_square_4, y_square_4);

                __m256 sub_square_1 = _mm256_sub_ps (x_square_1, y_square_1);
                __m256 sub_square_2 = _mm256_sub_ps (x_square_2, y_square_2);
                __m256 sub_square_3 = _mm256_sub_ps (x_square_3, y_square_3);
                __m256 sub_square_4 = _mm256_sub_ps (x_square_4, y_square_4);

                __m256 x_mul_y_mul_two_1 = _mm256_mul_ps (x_mul_y_1, kArrOfTwo);
                __m256 x_mul_y_mul_two_2 = _mm256_mul_ps (x_mul_y_2, kArrOfTwo);
                __m256 x_mul_y_mul_two_3 = _mm256_mul_ps (x_mul_y_3, kArrOfTwo);
                __m256 x_mul_y_mul_two_4 = _mm256_mul_ps (x_mul_y_4, kArrOfTwo);

                __m256 cmp_square_mask_1 = _mm256_cmp_ps (x_square_cmp_1, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_2 = _mm256_cmp_ps (x_square_cmp_2, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_3 = _mm256_cmp_ps (x_square_cmp_3, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_4 = _mm256_cmp_ps (x_square_cmp_4, kArrMaxModuleComplex256, _CMP_LE_OS);

                int mask = _mm256_movemask_ps (cmp_square_mask_1)
                            + _mm256_movemask_ps (cmp_square_mask_2)
                            + _mm256_movemask_ps (cmp_square_mask_3)
                            + _mm256_movemask_ps (cmp_square_mask_4);

                iteration_stop_256_1 = _mm256_sub_epi32 (iteration_stop_256_1, (__m256i) cmp_square_mask_1);
                iteration_stop_256_2 = _mm256_sub_epi32 (iteration_stop_256_2, (__m256i) cmp_square_mask_2);
                iteration_stop_256_3 = _mm256_sub_epi32 (iteration_stop_256_3, (__m256i) cmp_square_mask_3);
                iteration_stop_256_4 = _mm256_sub_epi32 (iteration_stop_256_4, (__m256i) cmp_square_mask_4);

                cmp_square_mask_1 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_1);
                cmp_square_mask_2 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_2);
                cmp_square_mask_3 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_3);
                cmp_square_mask_4 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_4);

                if (mask == 0)
                {
                    break;
                }

                x_cur_1 = _mm256_add_ps (sub_square_1, x_start_1);
                y_cur_1 = _mm256_add_ps (x_mul_y_mul_two_1, y_start);

                x_cur_2 = _mm256_add_ps (sub_square_2, x_start_2);
                y_cur_2 = _mm256_add_ps (x_mul_y_mul_two_2, y_start);

                x_cur_3 = _mm256_add_ps (sub_square_3, x_start_3);
                y_cur_3 = _mm256_add_ps (x_mul_y_mul_two_3, y_start);

                x_cur_4 = _mm256_add_ps (sub_square_4, x_start_4);
                y_cur_4 = _mm256_add_ps (x_mul_y_mul_two_4, y_start);

                x_cur_1 = _mm256_or_ps (cmp_square_mask_1, x_cur_1);
                y_cur_1 = _mm256_or_ps (cmp_square_mask_1, y_cur_1);

                x_cur_2 = _mm256_or_ps (cmp_square_mask_2, x_cur_2);
                y_cur_2 = _mm256_or_ps (cmp_square_mask_2, y_cur_2);

                x_cur_3 = _mm256_or_ps (cmp_square_mask_3, x_cur_3);
                y_cur_3 = _mm256_or_ps (cmp_square_mask_3, y_cur_3);

                x_cur_4 = _mm256_or_ps (cmp_square_mask_4, x_cur_4);
                y_cur_4 = _mm256_or_ps (cmp_square_mask_4, y_cur_4);
            }

            if (settings.graphic_mode)
            {
                _mm256_storeu_si256 ((__m256i*) iteration_stop_1, iteration_stop_256_1);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_2, iteration_stop_256_2);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_3, iteration_stop_256_3);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_4, iteration_stop_256_4);

                size_t ver_index = y_check * screen_width + x_check;
                for (size_t point_index = 0; point_index < kNumVerticesOptimize; point_index++)
                {
                    iteration_stop_arr [ver_index]                            = iteration_stop_1 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize    ] = iteration_stop_2 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize * 2] = iteration_stop_3 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize * 3] = iteration_stop_4 [point_index];
                    ver_index++;
                }
            }
        }
    }

    return kDoneMandelbrot;
}

enum MandelbrotError Mandelbrot256FullPipeLineFiveIter (int* const iteration_stop_arr, const settings_of_program_t settings)
{
    ASSERT ((settings.graphic_mode) && (iteration_stop_arr != NULL), "Mandelbrot256FullPipeLine got iteration_stop_arr as null ptr.\n");

    size_t screen_width  = settings.window_width;
    size_t screen_height = settings.window_height;

    float x_base = settings.coordinates_x;
    float y_base = settings.coordinates_y;

    float scale = settings.scale;

    size_t delta_x = kNumVerticesOptimize * 5;

    alignas (__m256i) int iteration_stop_1 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_1 = _mm256_load_si256 ((__m256i*) iteration_stop_1);

    alignas (__m256i) int iteration_stop_2 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_2 = _mm256_load_si256 ((__m256i*) iteration_stop_2);

    alignas (__m256i) int iteration_stop_3 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_3 = _mm256_load_si256 ((__m256i*) iteration_stop_3);

    alignas (__m256i) int iteration_stop_4 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_4 = _mm256_load_si256 ((__m256i*) iteration_stop_4);

    alignas (__m256i) int iteration_stop_5 [kNumVerticesOptimize] = {};
    __m256i iteration_stop_256_5 = _mm256_load_si256 ((__m256i*) iteration_stop_5);

    const __m256 kArrMaxModuleComplex256 = _mm256_set1_ps (kMaxModuleComplex);

    const __m256 kArrOfTwo = _mm256_set1_ps (2);
    const unsigned int kFullBitsInt = 0xFF'FF'FF'FF;
    const __m256i kMaxInt32U256     = _mm256_set1_epi32 (kFullBitsInt);

    __m256 x_base_256 = _mm256_set1_ps (x_base);
    __m256 y_base_256 = _mm256_set1_ps (y_base);
    __m256 scale_256  = _mm256_set1_ps (scale);

    __m256 temp_1 = _mm256_set_ps ( 7,  6,  5,  4,  3,  2,   1,   0);
    __m256 temp_2 = _mm256_set_ps (15, 14, 13, 12, 11, 10,   9,   8);
    __m256 temp_3 = _mm256_set_ps (23, 22, 21, 20, 19, 18,  17,  16);
    __m256 temp_4 = _mm256_set_ps (31, 30, 29, 28, 27, 26,  25,  24);
    __m256 temp_5 = _mm256_set_ps (39, 38, 37, 36, 35, 34,  33,  32);

    for (size_t y_check = 0; y_check < screen_height; y_check++)
    {
        __m256 y_start    = _mm256_set1_ps ((float) y_check);
        y_start = _mm256_sub_ps (y_base_256, y_start);
        y_start = _mm256_div_ps (y_start, scale_256);

        for (size_t x_check = 0; x_check < screen_width; x_check += delta_x)
        {
            __m256 x_start_1  = _mm256_set1_ps ((float) x_check);
            __m256 x_start_2  = x_start_1;
            __m256 x_start_3  = x_start_1;
            __m256 x_start_4  = x_start_1;
            __m256 x_start_5  = x_start_1;

            x_start_1 = _mm256_add_ps (x_start_1, temp_1);
            x_start_2 = _mm256_add_ps (x_start_2, temp_2);
            x_start_3 = _mm256_add_ps (x_start_3, temp_3);
            x_start_4 = _mm256_add_ps (x_start_4, temp_4);
            x_start_5 = _mm256_add_ps (x_start_5, temp_5);

            x_start_1 = _mm256_sub_ps (x_start_1, x_base_256);
            x_start_2 = _mm256_sub_ps (x_start_2, x_base_256);
            x_start_3 = _mm256_sub_ps (x_start_3, x_base_256);
            x_start_4 = _mm256_sub_ps (x_start_4, x_base_256);
            x_start_5 = _mm256_sub_ps (x_start_5, x_base_256);

            x_start_1 = _mm256_div_ps (x_start_1, scale_256);
            x_start_2 = _mm256_div_ps (x_start_2, scale_256);
            x_start_3 = _mm256_div_ps (x_start_3, scale_256);
            x_start_4 = _mm256_div_ps (x_start_4, scale_256);
            x_start_5 = _mm256_div_ps (x_start_5, scale_256);

            __m256 y_cur_1 = y_start;
            __m256 y_cur_2 = y_start;
            __m256 y_cur_3 = y_start;
            __m256 y_cur_4 = y_start;
            __m256 y_cur_5 = y_start;

            __m256 x_cur_1 = x_start_1;
            __m256 x_cur_2 = x_start_2;
            __m256 x_cur_3 = x_start_3;
            __m256 x_cur_4 = x_start_4;
            __m256 x_cur_5 = x_start_5;

            iteration_stop_256_1 = _mm256_setzero_si256 ();
            iteration_stop_256_2 = _mm256_setzero_si256 ();
            iteration_stop_256_3 = _mm256_setzero_si256 ();
            iteration_stop_256_4 = _mm256_setzero_si256 ();
            iteration_stop_256_5 = _mm256_setzero_si256 ();

            for (size_t num_point = 0; num_point < kMaxNumIteration; num_point++)
            {
                __m256 x_square_1 = _mm256_mul_ps (x_cur_1, x_cur_1);
                __m256 y_square_1 = _mm256_mul_ps (y_cur_1, y_cur_1);
                __m256 x_mul_y_1  = _mm256_mul_ps (x_cur_1, y_cur_1);

                __m256 x_square_2 = _mm256_mul_ps (x_cur_2, x_cur_2);
                __m256 y_square_2 = _mm256_mul_ps (y_cur_2, y_cur_2);
                __m256 x_mul_y_2  = _mm256_mul_ps (x_cur_2, y_cur_2);

                __m256 x_square_3 = _mm256_mul_ps (x_cur_3, x_cur_3);
                __m256 y_square_3 = _mm256_mul_ps (y_cur_3, y_cur_3);
                __m256 x_mul_y_3  = _mm256_mul_ps (x_cur_3, y_cur_3);

                __m256 x_square_4 = _mm256_mul_ps (x_cur_4, x_cur_4);
                __m256 y_square_4 = _mm256_mul_ps (y_cur_4, y_cur_4);
                __m256 x_mul_y_4  = _mm256_mul_ps (x_cur_4, y_cur_4);

                __m256 x_square_5 = _mm256_mul_ps (x_cur_5, x_cur_5);
                __m256 y_square_5 = _mm256_mul_ps (y_cur_5, y_cur_5);
                __m256 x_mul_y_5  = _mm256_mul_ps (x_cur_5, y_cur_5);

                __m256 x_square_cmp_1 = _mm256_add_ps (x_square_1, y_square_1);
                __m256 x_square_cmp_2 = _mm256_add_ps (x_square_2, y_square_2);
                __m256 x_square_cmp_3 = _mm256_add_ps (x_square_3, y_square_3);
                __m256 x_square_cmp_4 = _mm256_add_ps (x_square_4, y_square_4);
                __m256 x_square_cmp_5 = _mm256_add_ps (x_square_5, y_square_5);

                __m256 sub_square_1 = _mm256_sub_ps (x_square_1, y_square_1);
                __m256 sub_square_2 = _mm256_sub_ps (x_square_2, y_square_2);
                __m256 sub_square_3 = _mm256_sub_ps (x_square_3, y_square_3);
                __m256 sub_square_4 = _mm256_sub_ps (x_square_4, y_square_4);
                __m256 sub_square_5 = _mm256_sub_ps (x_square_5, y_square_5);

                __m256 x_mul_y_mul_two_1 = _mm256_mul_ps (x_mul_y_1, kArrOfTwo);
                __m256 x_mul_y_mul_two_2 = _mm256_mul_ps (x_mul_y_2, kArrOfTwo);
                __m256 x_mul_y_mul_two_3 = _mm256_mul_ps (x_mul_y_3, kArrOfTwo);
                __m256 x_mul_y_mul_two_4 = _mm256_mul_ps (x_mul_y_4, kArrOfTwo);
                __m256 x_mul_y_mul_two_5 = _mm256_mul_ps (x_mul_y_5, kArrOfTwo);

                __m256 cmp_square_mask_1 = _mm256_cmp_ps (x_square_cmp_1, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_2 = _mm256_cmp_ps (x_square_cmp_2, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_3 = _mm256_cmp_ps (x_square_cmp_3, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_4 = _mm256_cmp_ps (x_square_cmp_4, kArrMaxModuleComplex256, _CMP_LE_OS);
                __m256 cmp_square_mask_5 = _mm256_cmp_ps (x_square_cmp_5, kArrMaxModuleComplex256, _CMP_LE_OS);

                int mask = _mm256_movemask_ps (cmp_square_mask_1)
                            + _mm256_movemask_ps (cmp_square_mask_2)
                            + _mm256_movemask_ps (cmp_square_mask_3)
                            + _mm256_movemask_ps (cmp_square_mask_4)
                            + _mm256_movemask_ps (cmp_square_mask_5);

                iteration_stop_256_1 = _mm256_sub_epi32 (iteration_stop_256_1, (__m256i) cmp_square_mask_1);
                iteration_stop_256_2 = _mm256_sub_epi32 (iteration_stop_256_2, (__m256i) cmp_square_mask_2);
                iteration_stop_256_3 = _mm256_sub_epi32 (iteration_stop_256_3, (__m256i) cmp_square_mask_3);
                iteration_stop_256_4 = _mm256_sub_epi32 (iteration_stop_256_4, (__m256i) cmp_square_mask_4);
                iteration_stop_256_5 = _mm256_sub_epi32 (iteration_stop_256_5, (__m256i) cmp_square_mask_5);

                cmp_square_mask_1 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_1);
                cmp_square_mask_2 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_2);
                cmp_square_mask_3 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_3);
                cmp_square_mask_4 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_4);
                cmp_square_mask_5 = (__m256) _mm256_sub_epi32 (kMaxInt32U256,(__m256i)  cmp_square_mask_5);

                if (mask == 0)
                {
                    break;
                }

                x_cur_1 = _mm256_add_ps (sub_square_1, x_start_1);
                y_cur_1 = _mm256_add_ps (x_mul_y_mul_two_1, y_start);

                x_cur_2 = _mm256_add_ps (sub_square_2, x_start_2);
                y_cur_2 = _mm256_add_ps (x_mul_y_mul_two_2, y_start);

                x_cur_3 = _mm256_add_ps (sub_square_3, x_start_3);
                y_cur_3 = _mm256_add_ps (x_mul_y_mul_two_3, y_start);

                x_cur_4 = _mm256_add_ps (sub_square_4, x_start_4);
                y_cur_4 = _mm256_add_ps (x_mul_y_mul_two_4, y_start);

                x_cur_5 = _mm256_add_ps (sub_square_5, x_start_5);
                y_cur_5 = _mm256_add_ps (x_mul_y_mul_two_5, y_start);

                x_cur_1 = _mm256_or_ps (cmp_square_mask_1, x_cur_1);
                y_cur_1 = _mm256_or_ps (cmp_square_mask_1, y_cur_1);

                x_cur_2 = _mm256_or_ps (cmp_square_mask_2, x_cur_2);
                y_cur_2 = _mm256_or_ps (cmp_square_mask_2, y_cur_2);

                x_cur_3 = _mm256_or_ps (cmp_square_mask_3, x_cur_3);
                y_cur_3 = _mm256_or_ps (cmp_square_mask_3, y_cur_3);

                x_cur_4 = _mm256_or_ps (cmp_square_mask_4, x_cur_4);
                y_cur_4 = _mm256_or_ps (cmp_square_mask_4, y_cur_4);

                x_cur_5 = _mm256_or_ps (cmp_square_mask_5, x_cur_5);
                y_cur_5 = _mm256_or_ps (cmp_square_mask_5, y_cur_5);
            }

            if (settings.graphic_mode)
            {
                _mm256_storeu_si256 ((__m256i*) iteration_stop_1, iteration_stop_256_1);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_2, iteration_stop_256_2);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_3, iteration_stop_256_3);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_4, iteration_stop_256_4);
                _mm256_storeu_si256 ((__m256i*) iteration_stop_5, iteration_stop_256_5);

                size_t ver_index = y_check * screen_width + x_check;
                for (size_t point_index = 0; point_index < kNumVerticesOptimize; point_index++)
                {
                    iteration_stop_arr [ver_index]                            = iteration_stop_1 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize    ] = iteration_stop_2 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize * 2] = iteration_stop_3 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize * 3] = iteration_stop_4 [point_index];
                    iteration_stop_arr [ver_index + kNumVerticesOptimize * 4] = iteration_stop_5 [point_index];
                    ver_index++;
                }
            }
        }
    }

    return kDoneMandelbrot;
}
