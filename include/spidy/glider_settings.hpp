#pragma once
#include "glider.hpp"
#include <windows.h>
#include <string>

namespace spidy {
// Read once when the native swing starts, never perform disk IO in a physics step.
// Resolve next to this DLL rather than the game's or launcher's working directory.
inline GliderConfig loadGliderSettings() {
    GliderConfig result;
    HMODULE module{};
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&loadGliderSettings), &module))
        return result;
    wchar_t path[32768]{};
    const DWORD size = GetModuleFileNameW(module, path, static_cast<DWORD>(std::size(path)));
    if (!size || size >= std::size(path))
        return result;
    std::wstring file(path, size);
    const auto slash = file.find_last_of(L"\\/");
    if (slash == std::wstring::npos)
        return result;
    file.resize(slash + 1);
    file += L"spidy-glider.ini";
    result.enabled = GetPrivateProfileIntW(L"Glider", L"Enabled", 0, file.c_str()) == 1;
    result.sinkSpeed = static_cast<float>(std::clamp(
        static_cast<int>(GetPrivateProfileIntW(L"Glider", L"SinkSpeed", 4, file.c_str())), 2, 12));
    result.liftAcceleration = static_cast<float>(std::clamp(
        static_cast<int>(GetPrivateProfileIntW(L"Glider", L"LiftAcceleration", 14, file.c_str())), 4, 24));
    return result;
}
} // namespace spidy
