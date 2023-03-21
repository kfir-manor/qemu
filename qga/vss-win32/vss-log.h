#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

typedef struct LogConfig LogConfig;
typedef struct LogState LogState;

LogConfig *log_config;
LogState  *log_state;

void init_vss_log(void);
void cleanup_vss_log(void);
void disable_log(void);
void enable_log(void);
