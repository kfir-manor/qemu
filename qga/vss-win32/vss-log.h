#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

typedef struct LogState {
    FILE *log_file;
    bool logging_enabled;
} LogState;
LogState *log_state;

void init_vss_log(void);
void cleanup_vss_log(void);
void freeze_log(LogState *log_state);
void unfreeze_log(LogState *log_state);
