#include "qemu/osdep.h"
#include <glib/gstdio.h>

#ifdef _WIN32
#include <dbt.h>
#endif

const char *log_level_str(GLogLevelFlags level);
int glib_log_level_to_system(int level);
FILE *open_logfile(const char *logfile);
void file_log(FILE *log_file,const char *level_str,const gchar *msg);

#ifndef _WIN32
void system_log(GLogLevelFlags level,const char *level_str,const gchar *msg)
#else
void win_system_log(Handle event_log,GLogLevelFlags level,const gchar *msg)
#endif