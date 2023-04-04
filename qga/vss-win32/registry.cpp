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
    if (regGetValueError != ERROR_SUCCESS) {
        if (regGetValueError == ERROR_FILE_NOT_FOUND) {
            win32_error_log_info(regGetValueError,
            "add value %s to qga vss registry to change qga vss functionality",
                                  valueName);
        } else {
            win32_error_log_warning(regGetValueError,
                                    "failed to get RegValue %s form %s",
                                    valueName, subKey);
        }
    }
    return dwordData;
}
