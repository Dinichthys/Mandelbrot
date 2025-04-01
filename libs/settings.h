#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>
#include <stdlib.h>

enum SettingsError
{
    kDoneSettings       = 0,
    kCantOpenOutputFile = 1,
};

typedef struct settings_of_program
{
    FILE* log_file;
    FILE* out_file;

    size_t window_width;
    size_t window_height;
    float scale;

    size_t coordinates_x;
    size_t coordinates_y;

    bool graphic_mode;

    bool stop_program;
} settings_of_program_t;

const int kInvalidMode = -1;
const char* const kOutputFileName = "Output.txt";

enum SettingsError SettingsCtor (settings_of_program_t* const set);
void               SettingsDtor (settings_of_program_t* const set);
void               ParseFlags   (const int argc, char* const argv[], settings_of_program_t* const set);

#endif // SETTINGS_H
