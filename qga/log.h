static const char *log_level_str(GLogLevelFlags level);
static int glib_log_level_to_system(int level);
static FILE *open_logfile(const char *logfile);