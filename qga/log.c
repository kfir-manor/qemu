#include <glib/gstdio.h>

static const char *ga_log_level_str(GLogLevelFlags level)
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

static int glib_log_level_to_system(int level)
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

static void system_log(GLogLevelFlags level,const char *level_str,const gchar *msg)
{
#ifndef _WIN32
    syslog(glib_log_level_to_system(level), "%s: %s", level_str, msg);
#else
    ReportEvent(s->event_log, glib_log_level_to_system(level),
                0, 1, NULL, 1, 0, &msg, NULL);
#endif
}

static void file_log(GAState *s,const char *level_str,const gchar *msg)
{
        g_autoptr(GDateTime) now = g_date_time_new_now_utc();
        g_autofree char *nowstr = g_date_time_format(now, "%s.%f");
        fprintf(s->log_file, "%s: %s: %s\n", nowstr, level_str, msg);
        fflush(s->log_file);
}

static void ga_log(const gchar *domain, GLogLevelFlags level,
                   const gchar *msg, gpointer opaque)
{
    GAState *s = opaque;
    const char *level_str = ga_log_level_str(level);

    if (!ga_logging_enabled(s)) {
        return;
    }

    level &= G_LOG_LEVEL_MASK;
    if (g_strcmp0(domain, "syslog") == 0) {
        system_log(level,level_str,msg);
    } else if (level & s->log_level) {
        file_log(s,level_str,msg);
    }
}

static FILE *open_logfile(const char *logfile)
{
    FILE *f;

    f = fopen(logfile, "a");
    if (!f) {
        return NULL;
    }

    qemu_set_cloexec(fileno(f));
    return f;
}