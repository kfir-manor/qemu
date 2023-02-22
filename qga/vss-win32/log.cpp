
//  This application opens a file specified by the user and uses
//  a temporary file to convert the file to upper case letters.
//  Note that the given source file is assumed to be an ASCII text file
//  and the new file created is overwritten each time the application is
//  run.
//

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "main.c"

#define G_LOG_USE_STRUCTURED 
#include <glib.h>

#include <iostream>

#define BUFSIZE 1024

typedef struct LogConfig{
    char *log_filepath;
} LogConfig;

typedef struct LogState{
    FILE *log_file;
/*
#ifdef _WIN32
    HANDLE event_log;
#endif
} LogState;
*/
static LogConfig log_config= g_new0(LogConfig, 1);
static GAState *log_state = g_new0(LogState, 1);

void get_log_file_path(){
    DWORD dwRetVal = 0;
    UINT uRetVal   = 0;
    TCHAR szTempFileName[MAX_PATH];  
    TCHAR lpTempPathBuffer[MAX_PATH];

    dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer); 
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
    {
        g_error("GetTempPath failed");
    }
    uRetVal = GetTempFileName(lpTempPathBuffer, TEXT("qga_vss_log"), 0, szTempFileName);
    if (uRetVal == 0)
    {
        g_error("GetTempFileName failed");
    }
    g_info("log file location: %s",szTempFileName);

    return szTempFileName;
}

static void vss_log_init()
{
    log_state->log_file = stderr;
    g_log_set_writer_func(vss_log,log_state);
/*
#ifdef _WIN32
    log_state->event_log = RegisterEventSource(NULL, "qemu-ga-vss");
    if (!log_state->event_log) {
        g_autofree gchar *errmsg = g_win32_error_message(GetLastError());
        g_critical("unable to register event source: %s", errmsg);
        return NULL;
    }
#endif
*/
    log_config->log_filepath=get_log_file_path();
    FILE *log_file = ga_open_logfile(config->log_filepath);
    if (!log_file) {
        g_critical("unable to open specified log file: %s",
                    strerror(errno));
        return NULL;
    }
    log_state->log_file = log_file;
    return log_state;
}
static void vss_log_cleanup(){
    g_free(log_config);
    g_free(log_state);
}

static void system_log(HANDLE event_log, GLogLevelFlags level,
                       const char *level_str, const gchar *msg)
{
#ifndef _WIN32
    syslog(glib_log_level_to_system(level), "%s: %s", level_str, msg);
#else
    ReportEvent(event_log, glib_log_level_to_system(level),
                0, 1, NULL, 1, 0, &msg, NULL);
#endif
}

static void file_log(FILE log_file, const char *level_str, const gchar *msg)
{
    g_autoptr(GDateTime) now = g_date_time_new_now_utc();
    g_autofree char *nowstr = g_date_time_format(now, "%s.%f");
    fprintf(log_file, "%s: %s: %s\n", nowstr, level_str, msg);
    fflush(log_file);   
}


GLogWriterOutput vss_log(GLogLevelFlags log_level, const GLogField* fields,
                         gsize n_fields, gpointer user_data)
{
    const char *log_level_str = ga_log_level_str(log_level);
    LogConfig *log_config = user_data;
    level &= G_LOG_LEVEL_MASK;
/*
//LEVEL_ERROR = 4
    if (log_level == LEVEL_ERROR) {
        system_log(log_config->event_log,log_level,log_level_str,msg);
        goto out;
    }
*/
    file_log(log_config->log_file,log_level_str,msg);
out:
    return G_LOG_WRITER_HANDLED;
}