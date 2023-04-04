#include "log-utils.h"
#ifndef _WIN32
#include <syslog.h>
#endif
const char *log_level_str(GLogLevelFlags level)
{
    switch (level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
        return "error";
    case G_LOG_LEVEL_CRITICAL:
        return "critical";
    case G_LOG_LEVEL_WARNING:
        return "warning";
    case G_LOG_LEVEL_MESSAGE:
        return "message";
    case G_LOG_LEVEL_INFO:
        return "info";
    case G_LOG_LEVEL_DEBUG:
        return "debug";
    default:
        return "user";
    }
}

void file_log(FILE *log_file, const char *level_str, const gchar *msg)
{
    g_autoptr(GDateTime) now = g_date_time_new_now_utc();
    g_autofree char *nowstr = g_date_time_format(now, "%s.%f");
    fprintf(log_file, "%s: %s: %s\n", nowstr, level_str, msg);
    fflush(log_file);
}

FILE *ga_open_logfile(const char *logfile)
{
    FILE *f;

    f = fopen(logfile, "a");
    if (!f) {
        return NULL;
    }

    qemu_set_cloexec(fileno(f));
    return f;
}
