#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

void init_vss_log(void);
void deinit_vss_log(void);
void disable_log(void);
void enable_log(void);
void disable_stderr_log(void);
void enable_stderr_log(void);
#define win32_error_log_critical(win32_err, fmt, ...) \
    win32_error_log((win32_err), G_LOG_LEVEL_CRITICAL, (fmt), ##__VA_ARGS__)
#define win32_error_log_warning(win32_err, fmt, ...) \
    win32_error_log((win32_err), G_LOG_LEVEL_WARNING, (fmt), ##__VA_ARGS__)
void win32_error_log(int win32_err, GLogLevelFlags log_level, const char *fmt,
                          ...) G_GNUC_PRINTF(3, 4);
