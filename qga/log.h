static const char *log_level_str(GLogLevelFlags level);
static int glib_log_level_to_system(int level);
static FILE *open_logfile(const char *logfile);
static void file_log(GAState *s,const char *level_str,const gchar *msg);
static void system_log(GLogLevelFlags level,const char *level_str,const gchar *msg)