#include "qemu/osdep.h"
#include "vss-common.h"
DWORD get_reg_dword_value(HKEY baseKey, LPCSTR subKey, LPCSTR valueName,
                          DWORD defaultData);
