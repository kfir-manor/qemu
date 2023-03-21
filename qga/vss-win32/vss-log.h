#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

#define LOG_FILE_NAME "qga_vss_log.log"

typedef struct LogConfig {
    char log_filepath[MAX_PATH + strlen(LOG_FILE_NAME)];
    GLogLevelFlags log_level_mask;
} LogConfig;

typedef struct LogState {
    FILE *log_file;
    bool logging_enabled;
} LogState;

void init_vss_log(LogConfig *log_config, LogState *log_state);
void cleanup_vss_log(LogConfig *log_config ,LogState *log_state);
void disable_log(LogState *log_state);
void enable_log(LogState *log_state);
