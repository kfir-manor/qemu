#include <winsock2.h>
#include "glib.h"
#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "QGA VSS SERVICE"

void init_vss_log(void);
void cleanup_vss_log(void);