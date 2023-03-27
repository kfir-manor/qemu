#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

void init_vss_log(void);
void cleanup_vss_log(void);
void disable_log(void);
void enable_log(void);
void g_critical_error_pretty(int win32_err,const char *fmt,...);
