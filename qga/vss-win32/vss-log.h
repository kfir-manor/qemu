#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

typedef struct LogConfig LogConfig;
typedef struct LogState LogState;

LogConfig *log_config;
LogState *log_state;


void init_vss_log(void);
void cleanup_vss_log(void);
void freeze_log(void);
void unfreeze_log(void);
