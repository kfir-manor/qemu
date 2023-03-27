#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

void init_vss_log(void);
void cleanup_vss_log(void);
void disable_log(void);
void enable_log(void);
#define g_critical_error_pretty_fix(win32_err,fmt,...) \
    g_critical_error_pretty((win32_err), "%s",(fmt), ## __VA_ARGS__)
void g_critical_error_pretty(int win32_err,const char *fmt,...) G_GNUC_PRINTF(2, 3);

