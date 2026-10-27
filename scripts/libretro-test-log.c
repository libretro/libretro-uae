/* Native variadic logger for the isolated libretro test host. */
#include <stdarg.h>
#include <stdio.h>

static FILE *log_file;

__declspec(dllexport) int probe_log_open(const char *path)
{
    log_file = fopen(path, "wb");
    return log_file ? 0 : -1;
}

__declspec(dllexport) void probe_log(int level, const char *format, ...)
{
    va_list args;
    if (!log_file)
        return;
    fprintf(log_file, "[%d] ", level);
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    fflush(log_file);
}
