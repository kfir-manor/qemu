#include "registry.h"
#include "vss-log.h"
DWORD get_reg_dword_value(HKEY baseKey, LPCSTR subKey, LPCSTR valueName,
                          DWORD defaultData)
{
    DWORD regGetValueError;
    DWORD dwordData;
    DWORD dataSize = sizeof(DWORD);

    regGetValueError = RegGetValue(baseKey, subKey, valueName, RRF_RT_DWORD,
                                   NULL, &dwordData, &dataSize);
    if (regGetValueError  != ERROR_SUCCESS) {
        g_win32_error_log_warning(regGetValueError,
                                  "failed to get RegValue %s form %s",
                                  valueName, subKey);
        return defaultData;
    }
    return dwordData;
}