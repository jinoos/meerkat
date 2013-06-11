#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#define LOG_DEBUG   1
#define LOG_INFO    2
#define LOG_WARN    3
#define LOG_ERR     4
#define LOG_MSG     5
#define LOG_DEFAULT LOG_WARN

#define LOG_FILELINE_ON     1
#define LOG_FILELINE_OFF    0

int log_level(int level);

FILE* log_fd(FILE *fd);

int log_fileline(int set);

void log_str(const char *file, const int line, const char* level, const char* format, va_list args);

void log_log(const char *file, const int line, const char* format, ...);

void log_debug(const char *file, const int line, const char* format, ...);

void log_msg(const char *file, const int line, const char* format, ...);

void log_info(const char *file, const int line, const char* format, ...);

void log_warn(const char *file, const int line, const char* format, ...);

void log_err(const char *file, const int line, const char* format, ...);

#endif // _LOGGING_H_
