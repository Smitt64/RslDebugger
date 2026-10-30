#include "strconv.h"
#include <windows.h>
#include <string.h>

namespace dbgstr {

static const unsigned int CP_866  = 866;
static const unsigned int CP_1251 = 1251;

// ------------------------------------------------------------------------------
std::wstring cp_to_wide(unsigned int codepage, const char *s, size_t len)
{
    if (!s)
        return std::wstring();

    if (len == (size_t)-1)
        len = strlen(s);

    if (len == 0)
        return std::wstring();

    int n = MultiByteToWideChar(codepage, 0, s, (int)len, nullptr, 0);
    if (n <= 0)
        return std::wstring();

    std::wstring ws((size_t)n, L'\0');
    MultiByteToWideChar(codepage, 0, s, (int)len, &ws[0], n);
    return ws;
}

// ------------------------------------------------------------------------------
std::string wide_to_cp(unsigned int codepage, const wchar_t *ws, size_t len)
{
    if (!ws)
        return std::string();

    if (len == (size_t)-1)
        len = wcslen(ws);

    if (len == 0)
        return std::string();

    int n = WideCharToMultiByte(codepage, 0, ws, (int)len, nullptr, 0, nullptr, nullptr);
    if (n <= 0)
        return std::string();

    std::string s((size_t)n, '\0');
    WideCharToMultiByte(codepage, 0, ws, (int)len, &s[0], n, nullptr, nullptr);
    return s;
}

// ------------------------------------------------------------------------------
std::string cp_to_utf8(unsigned int codepage, const char *s, size_t len)
{
    if (codepage == CP_UTF8)
    {
        if (!s)
            return std::string();
        if (len == (size_t)-1)
            return std::string(s);
        return std::string(s, len);
    }

    return wide_to_cp(CP_UTF8, cp_to_wide(codepage, s, len).c_str());
}

// ------------------------------------------------------------------------------
std::string utf8_to_cp(unsigned int codepage, const std::string &utf8)
{
    if (codepage == CP_UTF8)
        return utf8;

    return wide_to_cp(codepage, cp_to_wide(CP_UTF8, utf8.c_str(), utf8.size()).c_str());
}

// ------------------------------------------------------------------------------
std::string oem_to_utf8(const char *s)
{
    return cp_to_utf8(CP_866, s);
}

// ------------------------------------------------------------------------------
std::string oem_to_utf8(const char *s, size_t len)
{
    return cp_to_utf8(CP_866, s, len);
}

// ------------------------------------------------------------------------------
std::string oem_to_utf8(const std::string &s)
{
    return cp_to_utf8(CP_866, s.c_str(), s.size());
}

// ------------------------------------------------------------------------------
std::string utf8_to_oem(const std::string &utf8)
{
    return utf8_to_cp(CP_866, utf8);
}

// ------------------------------------------------------------------------------
std::string ansi_to_utf8(const char *s)
{
    return cp_to_utf8(CP_1251, s);
}

// ------------------------------------------------------------------------------
std::string ansi_to_utf8(const std::string &s)
{
    return cp_to_utf8(CP_1251, s.c_str(), s.size());
}

// ------------------------------------------------------------------------------
std::string utf8_to_ansi(const std::string &utf8)
{
    return utf8_to_cp(CP_1251, utf8);
}

// ------------------------------------------------------------------------------
std::wstring utf8_to_wide(const std::string &utf8)
{
    return cp_to_wide(CP_UTF8, utf8.c_str(), utf8.size());
}

// ------------------------------------------------------------------------------
std::string wide_to_utf8(const std::wstring &ws)
{
    return wide_to_cp(CP_UTF8, ws.c_str(), ws.size());
}

} // namespace dbgstr
