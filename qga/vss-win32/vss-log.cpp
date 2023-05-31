#include "registry.h"
#include "vss-handles.h"
extern "C" {
#include "qga/log-utils.h"
}
#include "vss-log.h"

#define DEFAULT_LOG_LEVEL_MASK \
    (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING)
#define LOG_FILE_NAME "qga_vss.log"

typedef struct LogConfig {
    char log_filepath[MAX_PATH + strlen(LOG_FILE_NAME)];
    GLogLevelFlags log_level_mask;
} LogConfig;

typedef struct LogState {
    FILE *log_file;
    bool logging_enabled;
    bool stderr_log_enabled;
} LogState;

bool is_log_init;
static LogConfig *log_config;
static LogState *log_state;

void disable_log(void)
{
    log_state->logging_enabled = false;
}

void enable_log(void)
{
    log_state->logging_enabled = true;
}

void disable_stderr_log(void)
{
    log_state->stderr_log_enabled = false;
}

void enable_stderr_log(void)
{
    log_state->stderr_log_enabled = true;
}

DWORD get_log_level(void)
{
    return get_reg_dword_value(HKEY_LOCAL_MACHINE,
                               QGA_PROVIDER_REGISTRY_ADDRESS, "LogLevel", 0);
}

GLogLevelFlags convert_log_level_to_mask(DWORD log_level)
{
    int mask = DEFAULT_LOG_LEVEL_MASK;
    if (log_level > 0) {
        mask |= G_LOG_LEVEL_MESSAGE;
    }
    if (log_level > 1) {
        mask |= G_LOG_LEVEL_INFO;
    }
    if (log_level > 2) {
        mask |= G_LOG_LEVEL_DEBUG;
    }
    return static_cast<GLogLevelFlags>(mask);
}

GLogLevelFlags get_log_level_mask(void)
{
    return convert_log_level_to_mask(get_log_level());
}

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
    if (!(log_config->log_level_mask & log_level)) {
        return;
    }
    LogState *s = (LogState *)user_data;
    if (!s->logging_enabled) {
        return;
    }
    const char *level_str = ga_log_level_str(log_level);
    if (s->stderr_log_enabled) {
        file_log(stderr, level_str, message);
    }
    file_log(s->log_file, level_str, message);
}

void init_vss_log(void)
{
    if (is_log_init == true)
    {
        return;
    }
    log_config = g_new0(LogConfig, 1);
    log_state = g_new0(LogState, 1);
    log_state->logging_enabled = true;
    log_state->log_file = stderr;
    log_config->log_level_mask = get_log_level_mask();
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
    is_log_init = true;
}

void deinit_vss_log(void)
{
    if (is_log_init == false) {
        return;
    }
    g_free(log_config);
    g_free(log_state);
    is_log_init = false;
}

void win32_error_log(int win32_err, GLogLevelFlags log_level, const char *fmt,
                     ...)
{
    va_list ap;
    va_start(ap, fmt);
    char *msg = g_strdup_vprintf(fmt, ap);
    va_end(ap);
    char *suffix = g_win32_error_message(win32_err);
    g_log(G_LOG_DOMAIN, log_level, "%s: %s", msg, suffix);
    g_free(suffix);
    g_free(msg);
}
