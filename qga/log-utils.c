
#ifndef _WIN32
#include <syslog.h>
#endif
#include "log-utils.h"

const char *ga_log_level_str(GLogLevelFlags level)
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

int glib_log_level_to_system(int level)
{
    switch (level) {
#ifndef _WIN32
    case G_LOG_LEVEL_ERROR:
        return LOG_ERR;
    case G_LOG_LEVEL_CRITICAL:
        return LOG_CRIT;
    case G_LOG_LEVEL_WARNING:
        return LOG_WARNING;
    case G_LOG_LEVEL_MESSAGE:
        return LOG_NOTICE;
    case G_LOG_LEVEL_DEBUG:
        return LOG_DEBUG;
    case G_LOG_LEVEL_INFO:
    default:
        return LOG_INFO;
#else
    case G_LOG_LEVEL_ERROR:
    case G_LOG_LEVEL_CRITICAL:
        return EVENTLOG_ERROR_TYPE;
    case G_LOG_LEVEL_WARNING:
        return EVENTLOG_WARNING_TYPE;
    case G_LOG_LEVEL_MESSAGE:
    case G_LOG_LEVEL_INFO:
    case G_LOG_LEVEL_DEBUG:
    default:
        return EVENTLOG_INFORMATION_TYPE;
#endif
    }
}

#ifndef _WIN32
void system_log(GLogLevelFlags level,const char *level_str,const gchar *msg)
{
    syslog(glib_log_level_to_system(level), "%s: %s", level_str, msg);
}
#else
void win_system_log(Handle event_log,GLogLevelFlags level,const gchar *msg)
{
    ReportEvent(event_log, glib_log_level_to_system(level),
                0, 1, NULL, 1, 0, &msg, NULL);
}
#endif

void file_log(FILE *log_file,const char *level_str,const gchar *msg)
{
        g_autoptr(GDateTime) now = g_date_time_new_now_utc();
        g_autofree char *nowstr = g_date_time_format(now, "%s.%f");
        fprintf(log_file, "%s: %s: %s\n", nowstr, level_str, msg);
        fflush(log_file);
}

FILE *open_logfile(const char *logfile)
{
    FILE *f;

    f = fopen(logfile, "a");
    if (!f) {
        return NULL;
    }

    qemu_set_cloexec(fileno(f));
    return f;
}