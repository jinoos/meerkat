#include "logging.h"

//extern int LOG_LEVEL = LOG_DEFAULT;
static int LOG_LEVEL = LOG_DEFAULT;

//extern FILE* LOG_FD = NULL;
static FILE* LOG_FD = NULL;

int log_level(int level)
{
    if(level != 0) LOG_LEVEL = level;
    return LOG_LEVEL;
}

FILE* log_fd(FILE *fd)
{
    if(fd != NULL) LOG_FD = fd;
    return LOG_FD;
}

//static int LOG_FILELINE = LOG_FILELINE_ON;
static int LOG_FILELINE = LOG_FILELINE_OFF;

int log_fileline(int set)
{
    if(set == LOG_FILELINE_ON)
        LOG_FILELINE = LOG_FILELINE_ON;
    else
        LOG_FILELINE = LOG_FILELINE_OFF;

    return LOG_FILELINE;
}


void log_str(const char *file, const int line, const char* level, const char* format, va_list args)
{
    if(LOG_FD == NULL)
    {
        LOG_FD = stdout;
    }

	struct tm* t;
	time_t ct;

	time(&ct);

	t = localtime(&ct);

    if(LOG_FILELINE == LOG_FILELINE_ON)
    {
        fprintf(LOG_FD, "%d-%02d-%02d %02d:%02d:%02d [%s] %s(%d) ", (t->tm_year) + 1900, (t->tm_mon) + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, level, file, line);
    }else
    {
        fprintf(LOG_FD, "%d-%02d-%02d %02d:%02d:%02d [%s] ", (t->tm_year) + 1900, (t->tm_mon) + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, level);
    }
	vfprintf(LOG_FD, format, args);
	fprintf(LOG_FD, "\n");
	fflush(LOG_FD);
}

void log_log(const char *file, const int line, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	log_str(file, line, "LOG", format, argptr);
	va_end(argptr);
}

void log_debug(const char *file, const int line, const char* format, ...)
{
	if (LOG_LEVEL <= LOG_DEBUG)
	{
		va_list argptr;
		va_start(argptr, format);
		log_str(file, line, "DBG", format, argptr);
		va_end(argptr);
	}
}

void log_info(const char *file, const int line, const char* format, ...)
{
	if (LOG_LEVEL <= LOG_INFO)
	{
		va_list argptr;
		va_start(argptr, format);
		log_str(file, line, "INF", format, argptr);
		va_end(argptr);
	}
}

void log_msg(const char *file, const int line, const char* format, ...)
{
	if (LOG_LEVEL <= LOG_MSG)
	{
		va_list argptr;
		va_start(argptr, format);
		log_str(file, line, "MSG", format, argptr);
		va_end(argptr);
	}
}

void log_warn(const char *file, const int line, const char* format, ...)
{
	if (LOG_LEVEL <= LOG_WARN)
	{
		va_list argptr;
		va_start(argptr, format);
		log_str(file, line, "WRN", format, argptr);
		va_end(argptr);
	}
}

void log_err(const char *file, const int line, const char* format, ...)
{
	if (LOG_LEVEL <= LOG_ERR)
	{
		va_list argptr;
		va_start(argptr, format);
		log_str(file, line, "ERR", format, argptr);
		va_end(argptr);
	}
}
