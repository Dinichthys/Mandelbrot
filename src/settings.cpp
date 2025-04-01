#include "settings.h"

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "mandelbrot.h"

#include "My_lib/Assert/my_assert.h"
#include "My_lib/My_stdio/my_stdio.h"
#include "My_lib/helpful.h"

static void PrintHelp (void);

void ParseFlags (const int argc, char* const argv[], settings_of_program_t* const set)
{
    ASSERT (argv != NULL, "Invalid argument: argv = %p\n", argv);
    ASSERT (set  != NULL, "Invalid argument: set = %p\n",  set);

    const struct option modifications [] =
    {
        {"log-file",     required_argument, 0, 'l'},
        {"out-file",     required_argument, 0, 'o'},
        {"window-size",  required_argument, 0, 'w'},
        {"scale",        required_argument, 0, 's'},
        {"coordinates",  required_argument, 0, 'c'},
        {"graphic-mode",       no_argument, 0, 'g'},
        {"help",               no_argument, 0, 'h'},
        {0,                              0, 0,  0 }
    };

    int mode = 0;
    int long_index = 0;
    int count_iterate = 0;

    while (mode != kInvalidMode)
    {
        count_iterate++;
        mode = getopt_long (argc, argv, "+l:o:w:s:c:gh", modifications, &long_index);
        switch (mode)
        {
            case 'l':
            {
                set->log_file = fopen (optarg, "w");
                if (!(set->log_file))
                {
                    mode = kInvalidMode;
                    set->stop_program = true;
                    fprintf (stderr, "Can't open file \"%s\" for logging.\n", optarg);
                }
                count_iterate++;
                break;
            }
            case 'o':
            {
                set->out_file = fopen (optarg, "w");
                if (!(set->out_file))
                {
                    mode = kInvalidMode;
                    set->stop_program = true;
                    fprintf (stderr, "Can't open file \"%s\" for output.\n", optarg);
                }
                count_iterate++;
                break;
            }
            case 'w':
            {
                set->window_width  = atoll (strtok (optarg, ","));
                if (errno == ERANGE)
                {
                    fprintf (stderr, "Can't convert window width to long.\n");
                    mode = kInvalidMode;
                    set->stop_program = true;
                    break;
                }
                set->window_height = atoll (strtok (NULL, ","));
                if (errno == ERANGE)
                {
                    fprintf (stderr, "Can't convert window height to long.\n");
                    mode = kInvalidMode;
                    set->stop_program = true;
                }
                count_iterate++;
                break;
            }
            case 's':
            {
                set->scale = atof (optarg);
                if (errno == ERANGE)
                {
                    fprintf (stderr, "Can't convert scale to float.\n");
                    mode = kInvalidMode;
                    set->stop_program = true;
                }
                count_iterate++;
                break;
            }
            case 'c':
            {
                set->coordinates_x = atoll (strtok (optarg, ","));
                if (errno == ERANGE)
                {
                    fprintf (stderr, "Can't convert coordinate X to long.\n");
                    mode = kInvalidMode;
                    set->stop_program = true;
                    break;
                }
                set->coordinates_y = atoll (strtok (NULL, ","));
                if (errno == ERANGE)
                {
                    fprintf (stderr, "Can't convert coordinate Y to long.\n");
                    mode = kInvalidMode;
                    set->stop_program = true;
                }
                count_iterate++;
                break;
            }
            case 'g':
            {
                set->graphic_mode = true;
                break;
            }
            case 'h':
            {
                PrintHelp ();
                mode = kInvalidMode;
                set->stop_program = true;
                break;
            }
            case kInvalidMode:
            {
                if (argc != count_iterate)
                {
                    fprintf (stderr, "There are odd symbols in the terminal.\n");
                    mode = kInvalidMode;
                    set->stop_program = true;
                }
                break;
            }
            default:
            {
                fprintf (stderr, "Invalid name of command in the terminal.\n");
                mode = -1;
                set->stop_program = true;
                break;
            }
        }
    }
}

enum SettingsError SettingsCtor (settings_of_program_t* const set)
{
    ASSERT (set != NULL, "Invalid argument: set = %p\n", set);

    set->stop_program = false;

    set->log_file  = stderr;
    set->out_file  = fopen (kOutputFileName, "w");
    if (set->out_file == NULL)
    {
        return kCantOpenOutputFile;
    }

    set->window_height = kWindowHeight;
    set->window_width  = kWindowWidth;
    set->scale = kScale;

    set->coordinates_x = kWindowWidth  / 2;
    set->coordinates_y = kWindowHeight / 2;

    set->graphic_mode = false;

    return kDoneSettings;
}

void SettingsDtor (settings_of_program_t* const set)
{
    ASSERT (set != NULL, "Invalid argument: set = %p\n", set);

    if (set->log_file != stderr)
    {
        CLOSE_AND_NULL (set->log_file);
    }

    if (set->log_file != stdout)
    {
        CLOSE_AND_NULL (set->log_file);
    }

    set->window_width  = 0;
    set->window_height = 0;

    set->scale = 0;

    set->coordinates_x = 0;
    set->coordinates_y = 0;

    set->graphic_mode = false;

    set->stop_program = true;
}

static void PrintHelp (void)
{
    fprintf (stdout, "------------------------------------------------------------------------------------------\n"
                     "| \"--log-file\" or \"-l\"             : You can change the stream of logging information    |\n"
                     "|                                    by writing the name of file for output after that.  |\n"
                     "| \"--window-size\" or \"-w\"          : You can change the size of the opening window       |\n"
                     "|                                    by writing the size of it.                          |\n"
                     "| \"--scale\" or \"-s\"                : You can change the scale of the picture             |\n"
                     "|                                    by writing the value of it.                         |\n"
                     "| \"--coordinates\" or \"-c\"          : You can change the coordinates of the point (0,0)   |\n"
                     "|                                    from left top corner by writing the value of x and y|\n"
                     "| \"--graphic-mode\" or \"-g\"         : You can turn on the graphic mode to see             |\n"
                     "|                                    the Mandelbrot's Set.                               |\n"
                     "| \"--help\" or \"-h\"                 : Write information about flags of that program.      |\n"
                     "------------------------------------------------------------------------------------------\n");
}
