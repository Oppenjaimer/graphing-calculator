#include <raylib.h>
#include <stdio.h>

#include "config.h"

void custom_trace_log(int msg_type, const char *text, va_list args) {
    switch (msg_type) {
        case LOG_INFO:    printf("[INFO]    "); break;
        case LOG_ERROR:   printf("[ERROR]   "); break;
        case LOG_WARNING: printf("[WARNING] "); break;
        case LOG_DEBUG:   printf("[DEBUG]   "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

bool set_resource_dir(const char* folder_name) {
    // Check current directory
    if (DirectoryExists(folder_name)) {
        ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), folder_name));
        return true;
    }

    const char* app_dir = GetApplicationDirectory();

    // Check app directory
    const char* dir = TextFormat("%s%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    // Check one level up from app directory
    dir = TextFormat("%s../%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    // Check two levels up from app directory
    dir = TextFormat("%s../../%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    return false;
}