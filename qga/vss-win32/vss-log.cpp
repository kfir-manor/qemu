#include "qemu/osdep.h"
#include "vss-log.h"
#include "qemu/qga/log.h"
#include "vss-handles.h"

#define DEFAULT_LOG_LEVEL_MASK 28
#define FULL_LOG_LEVEL_MASK 252
#define LOG_FILE_NAME "qga_vss_log.log"

typedef struct LogConfig {
    char log_filepath[MAX_PATH + 15];
    GLogLevelFlags log_level_mask;
} LogConfig;


LogConfig *log_config;
FILE *log_file;

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



DWORD get_log_level(void)
{
    return get_reg_dword_value(HKEY_LOCAL_MACHINE,
                               QGA_PROVIDER_REGISTRY_ADDRESS,
                               "LogLevel", 0);
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

GLogLevelFlags get_inactive_mask(GLogLevelFlags log_mask)
{
 return static_cast<GLogLevelFlags>(FULL_LOG_LEVEL_MASK ^ log_mask);
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

void inactive_vss_log(const gchar *log_domain, GLogLevelFlags log_level,
                     const gchar *message, gpointer user_data)
{
    return;
}

void active_vss_log(const gchar *log_domain, GLogLevelFlags log_level,
                     const gchar *message, gpointer user_data)
{
    FILE *log_file = (FILE *)user_data;
    const char *level_str = log_level_str(log_level);
    file_log(log_file, level_str, message);
}

void init_vss_log(void)
{
    GLogLevelFlags inactive_mask;
    log_config = g_new0(LogConfig, 1);
    log_file = g_new0(FILE, 1);
    log_file = stderr;
    log_config->log_level_mask = get_log_level_mask();
    inactive_mask = get_inactive_mask(log_config->log_level_mask);
    g_log_set_handler(G_LOG_DOMAIN, log_config->log_level_mask,
                      active_vss_log, log_file);
    if (inactive_mask != 0) {
        g_log_set_handler(G_LOG_DOMAIN, inactive_mask,
                        inactive_vss_log, NULL);
    }

    if (set_tmp_file_path(log_config->log_filepath)) {
            printf("oppening file: %s", log_config->log_filepath);
            FILE *tmp_log_file = open_logfile(log_config->log_filepath);
            if (!tmp_log_file) {
                printf("unable to open specified log file: %s",
                           strerror(errno));
                return;
            }
            *log_file = *tmp_log_file;
    }
}

void cleanup_vss_log(void)
{
    g_free(log_config);
    g_free(log_file);
}
