#include "registry.h"
#include "vss-handles.h"
extern "C" {
#include "qga/log-utils.h"
}
#include "vss-log.h"

#define LOG_FILE_NAME "qga_vss.log"

typedef struct LogConfig {
    char log_filepath[MAX_PATH + strlen(LOG_FILE_NAME)];
} LogConfig;

typedef struct LogState {
    FILE *log_file;
} LogState;

static LogConfig *log_config;
static LogState *log_state;

bool set_tmp_file_path(char *p)
{
    DWORD dwRetVal = 0;
    dwRetVal = GetTempPath(MAX_PATH, p);
    if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
        g_critical("GetTempPath failed");
        return false;
    }
    strcat(p, LOG_FILE_NAME);
    return true;
}

void vss_log(const gchar *log_domain, GLogLevelFlags log_level,
                    const gchar *message, gpointer user_data)
{
    LogState *s = (LogState *)user_data;
    const char *level_str = ga_log_level_str(log_level);
    file_log(s->log_file, level_str, message);
}

void init_vss_log(void)
{
    log_config = g_new0(LogConfig, 1);
    log_state = g_new0(LogState, 1);
    log_state->log_file = stderr;
    g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_MASK, vss_log,
                      log_state);
    if (set_tmp_file_path(log_config->log_filepath)) {
        printf("oppening file: %s\n", log_config->log_filepath);
        FILE *tmp_log_file = ga_open_logfile(log_config->log_filepath);
        if (!tmp_log_file) {
            printf("unable to open specified log file: %s\n", strerror(errno));
            return;
        }
        log_state->log_file = tmp_log_file;
    }
}

void deinit_vss_log(void)
{
    g_free(log_config);
    g_free(log_state);
}
