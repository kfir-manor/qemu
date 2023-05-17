#include "qemu/osdep.h"

const char *ga_log_level_str(GLogLevelFlags level);
FILE *ga_open_logfile(const char *logfile);
void file_log(FILE *log_file, const char *level_str, const gchar *msg);
