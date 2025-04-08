#include "mandelbrot.h"

#include "draw.h"

#include <x86intrin.h>

#include "My_lib/Assert/my_assert.h"

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

static enum MandelbrotError AnalyseKey (const sf::Event event,
                                        settings_of_program_t* settings);

enum MandelbrotError DrawMandelbrot (settings_of_program_t set,
                                     enum MandelbrotError (*MandelbrotFunc) (int* const iteration_stop_arr,
                                                                             const settings_of_program_t settings),
                                     const char* const window_name)
{
    ASSERT (window_name    != NULL, "DrawMandelbrot got window_name as a null ptr in argument.\n");
    ASSERT (MandelbrotFunc != NULL, "DrawMandelbrot got MandelbrotFunc as a null ptr in argument.\n");

    size_t screen_height = set.window_height;
    size_t screen_width  = set.window_width +
                                          ((kNumVerticesOptimize * kNumberOfLoopUnrolling)
                                            - (set.window_width % (kNumVerticesOptimize * kNumberOfLoopUnrolling))
                                          % (kNumVerticesOptimize * kNumberOfLoopUnrolling));

    set.window_width  = screen_width;
    set.window_height = screen_height;

    int* iteration_stop_arr = (int*) aligned_alloc (kIntrinsicSize, screen_width * screen_height * sizeof (int));
    if (iteration_stop_arr == NULL)
    {
        return kCantCallocIterationArray;
    }

    sf::Event event;

    sf::Font font;
    if (!font.loadFromFile ("data/PFAgoraSlabPro Bold.ttf"))
    {
        free (iteration_stop_arr);
        return kCantLoadFont;
    }

    char fps_str [kFPSStringLength] = "";
    sf::Text text (fps_str, font, kNumberOfChars);
    text.setCharacterSize(kCharacterSize);

    sf::RenderWindow window (sf::VideoMode ({(unsigned int) screen_width, (unsigned int) screen_height}),
                             window_name);

    size_t vertex_size = screen_height * screen_width;

    sf::VertexArray vertices (sf::PrimitiveType::Points, vertex_size);

    bool window_is_open = true;

    unsigned int aux = 0;
    unsigned long long start = 0;
    unsigned long long end   = 0;

    while (window_is_open)
    {
        start = __rdtscp (&aux);
        MandelbrotFunc (iteration_stop_arr, set);
        end = __rdtscp (&aux);

        while (window.pollEvent(event))
        {
            if (AnalyseKey (event, &set) == kCloseWindow)
            {
                window.close ();
                window_is_open = false;
                break;
            }
        }

        for (size_t point_index = 0; point_index < vertex_size; point_index++)
        {
            vertices [point_index].position = {(float) (point_index % screen_width), (float) (point_index / screen_width)};

            vertices [point_index].color.r = iteration_stop_arr [point_index] << 2;
            vertices [point_index].color.g = iteration_stop_arr [point_index] << 1;
            vertices [point_index].color.b = iteration_stop_arr [point_index];
        }

        sprintf (fps_str, "%2d", (int) (kCPUFrequency / (end - start)));
        text.setString (fps_str);
        window.draw (vertices);
        window.draw (text);
        window.display ();
    }

    vertices.clear ();
    free (iteration_stop_arr);

    return kDoneMandelbrot;
}

static enum MandelbrotError AnalyseKey (const sf::Event event,
                                        settings_of_program_t* settings)
{
    if (event.type == sf::Event::Closed)
    {
        return kCloseWindow;
    }

    enum MoveMandelbrot scancode = (MoveMandelbrot) event.key.scancode;
    if (scancode == kLeft)
    {
        settings->coordinates_x += kShift;
        return kDoneMandelbrot;
    }
    if (scancode == kRight)
    {
        settings->coordinates_x -= kShift;
        return kDoneMandelbrot;
    }
    if (scancode == kDown)
    {
        settings->coordinates_y -= kShift;
        return kDoneMandelbrot;
    }
    if (scancode == kUp)
    {
        settings->coordinates_y += kShift;
        return kDoneMandelbrot;
    }

    if (scancode == kPlus)
    {
        settings->coordinates_x -= (kStartCoordinatesX - settings->coordinates_x) * kScaleShift / 100;
        settings->coordinates_y -= (kStartCoordinatesY - settings->coordinates_y) * kScaleShift / 100;
        settings->scale += kScaleShift * settings->scale / 100;
        // (kStartCoordinates - x_base) / scale = (kStartCoordinates - (x_base + a)) / (scale + kScaleShift * scale / 100)
        //  kStartCoordinates * (scale + kScaleShift * scale / 100) - x_base * (scale + kScaleShift * scale / 100)  = kStartCoordinates * (scale) - (x_base + a) * (scale)
        //  kStartCoordinates * kScaleShift * scale / 100 - x_base * kScaleShift * scale / 100 = - a * scale
        return kDoneMandelbrot;
    }
    if ((scancode == kMinus) && (settings->scale > kScaleShift))
    {
        settings->coordinates_x += (kStartCoordinatesX - settings->coordinates_x) * kScaleShift / 100;
        settings->coordinates_y += (kStartCoordinatesY - settings->coordinates_y) * kScaleShift / 100;
        settings->scale -= kScaleShift * settings->scale / 100;
        return kDoneMandelbrot;
    }

    return kDoneMandelbrot;
}
