#include <winsock2.h>
#include <windows.h>
#include "qemu/osdep.h"
#include "log.h"

#define MAX_LOG_FILE_PATH 1024


typedef struct LogConfig{
    const char log_filepath[MAX_LOG_FILE_PATH];
} LogConfig;

typedef struct LogState{
    FILE *log_file;
} LogState;

LogConfig *log_config;
LogState *log_state;

const char *get_log_filepath(const char *p){
    DWORD dwRetVal = 0;
    UINT uRetVal   = 0;
    TCHAR szTempFileName[MAX_LOG_FILE_PATH];  
    TCHAR lpTempPathBuffer[MAX_LOG_FILE_PATH];

    dwRetVal = GetTempPath(MAX_LOG_FILE_PATH, lpTempPathBuffer); 
    if (dwRetVal > MAX_LOG_FILE_PATH || (dwRetVal == 0))
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

void file_log(FILE *log_file,const char *level_str,const gchar* message)
{
    g_autoptr(GDateTime) now = g_date_time_new_now_utc();
    g_autofree char *nowstr = g_date_time_format(now, "%s.%f");
    fprintf(log_file, "%s: %s: %s\n", nowstr, level_str, message);
    fflush(log_file);
}

void vss_log(const gchar* log_domain, GLogLevelFlags log_level,
                     const gchar* message, gpointer user_data)
{
    LogState *log_state = (LogState*)user_data;
    const char *level_str = ga_log_level_str(log_level);
    file_log(log_state->log_file,level_str,message);
}

static FILE *open_logfile(const char *logfilepath)
{
    FILE *f;

    f = fopen(logfilepath, "a");
    if (!f) {
        return NULL;
    }

    fileno(f);
    return f;
}

void init_vss_log(void){
    log_config= g_new0(LogConfig,1);
    log_state= g_new0(LogState,1);
    log_state->log_file = stderr;
    g_log_set_default_handler(vss_log, log_state);
    log_config->log_filepath=get_log_filepath(log_config->log_filepath);
    if(log_config->log_filepath!=nullptr){
        g_info("log file path: %s",log_config->log_filepath);
        log_state->log_file = open_logfile(log_config->log_filepath);
        g_info("opened sucess: %s",log_config->log_filepath);
    }
}

void cleanup_vss_log(void){
    g_free(log_config);
    g_free(log_state);
}

