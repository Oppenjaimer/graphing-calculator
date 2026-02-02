#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdarg.h>

/**
 * Trace log callback for raylib.
 */
void custom_trace_log(int msg_type, const char *text, va_list args);

/**
 * Look for resource directory and change to it.
 * Returns whether the directory is found.
 */
bool set_resource_dir(const char* folder_name);

#endif