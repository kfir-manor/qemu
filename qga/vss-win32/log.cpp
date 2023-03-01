#include "qemu/osdep.h"
#include "log.h"
#include "vss-handles.h"

typedef struct LogConfig{
    char log_filepath[MAX_PATH];
    GLogLevelFlags log_level_mask;
} LogConfig;

typedef struct LogState{
    FILE *log_file;
} LogState;

LogConfig *log_config;
LogState *log_state;

DWORD get_reg_dword_value(HKEY baseKey, LPCSTR subKey, LPCSTR valueName,
                          DWORD defaultData)
{
    DWORD regGetValueError;
    DWORD dwordData;
    DWORD dataSize = sizeof(DWORD);

    regGetValueError = RegGetValue(baseKey, subKey, valueName, RRF_RT_DWORD,
                                   NULL, &dwordData, &dataSize);
    if (regGetValueError  != ERROR_SUCCESS) {
        return defaultData;
    }
    return dwordData;
}

GLogLevelFlags get_log_level_mask(DWORD log_level){
    GLogLevelFlags default_mask=G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL 
                                | G_LOG_LEVEL_WARNING;
    if(log_level > 0) {
        default_mask= default_mask | G_LOG_LEVEL_MESSAGE;
    }
    if(log_level > 1) {
        default_mask= default_mask | INFO;
    }
    if(log_level > 2) {
        default_mask= default_mask | DEBUG;
    }
    return default_mask
}

bool set_tmp_filepath(char * p){
    DWORD dwRetVal = 0;
    UINT uRetVal   = 0;
    TCHAR lpTempPathBuffer[MAX_PATH];

    dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
    if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
        g_error("GetTempPath failed");
        goto failed;
    }
    uRetVal = GetTempFileName(lpTempPathBuffer, TEXT("qga_vss_log"), 0, p);
    if (uRetVal == 0) {
        g_error("GetTempFileName failed");
        goto failed;
    }
    return true;
failed:
    return false;
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

void inactive_vss_log(const gchar *log_domain, GLogLevelFlags log_level,
                     const gchar *message, gpointer user_data)
{
    return;
}

void active_vss_log(const gchar *log_domain, GLogLevelFlags log_level,
                     const gchar *message, gpointer user_data)
{
    LogState *log_state = (LogState *)user_data;
    const char *level_str = ga_log_level_str(log_level);
    file_log(log_state->log_file, level_str, message);
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

void init_vss_log(void)
{
    log_config= g_new0(LogConfig, 1);
    log_state= g_new0(LogState, 1);
    log_state->log_file = stderr;
    log_config->log_level_mask = get_log_level_mask(
                                 get_reg_dword_value(HKEY_LOCAL_MACHINE,
                                                QGA_PROVIDER_REGISTRY_ADDRESS,
                                                 "LogLevel", 0))
    g_log_set_handler(G_LOG_DOMAIN, log_config->log_level_mask,
                      active_vss_log, log_state);
    g_log_set_handler(G_LOG_DOMAIN, ~(log_config->log_level_mask),
                      inactive_vss_log, NULL);
    g_info("im alive");
    g_message("im alive");
    g_debug("im alive");
    g_warning("im alive");
    if (set_tmp_filepath(log_config->log_filepath)) {
            printf("oppening file: %s",log_config->log_filepath);
            FILE *log_file = open_logfile(log_config->log_filepath);
            if (!log_file) {
                printf("unable to open specified log file: %s",
                           strerror(errno));
                return;
            }
            log_state->log_file = log_file;
    }
}

void cleanup_vss_log(void)
{
    g_free(log_config);
    g_free(log_state);
}
