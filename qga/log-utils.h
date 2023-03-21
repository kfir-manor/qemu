#include <glib/gstdio.h>
#include "qemu/osdep.h"

static const char *log_level_str(GLogLevelFlags level);
static int glib_log_level_to_system(int level);
static FILE *open_logfile(const char *logfile);
static void file_log(FILE *log_file,const char *level_str,const gchar *msg);

#ifndef _WIN32
static void system_log(GLogLevelFlags level,const char *level_str,const gchar *msg)
#else
static void win_system_log(Handle event_log,GLogLevelFlags level,const gchar *msg)
#endif