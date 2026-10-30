#ifndef STRCONV_H
#define STRCONV_H

#include <string>

// Преобразования кодировок для отладочного сервера (без Qt).
// Строки RSL приходят в OEM (cp866) или ANSI (cp1251), в протоколе — UTF-8 JSON.
// Реализация поверх MultiByteToWideChar/WideCharToMultiByte.

namespace dbgstr {

// Базовые преобразования через UTF-16. len == (size_t)-1 — строка с NUL-терминатором.
std::wstring cp_to_wide(unsigned int codepage, const char *s, size_t len = (size_t)-1);
std::string  wide_to_cp(unsigned int codepage, const wchar_t *ws, size_t len = (size_t)-1);

std::string  cp_to_utf8(unsigned int codepage, const char *s, size_t len = (size_t)-1);
std::string  utf8_to_cp(unsigned int codepage, const std::string &utf8);

// Частые случаи: RSL OEM (cp866), ANSI (cp1251), UTF-16.
std::string  oem_to_utf8(const char *s);          // cp866 -> utf8
std::string  oem_to_utf8(const char *s, size_t len);
std::string  oem_to_utf8(const std::string &s);    // cp866 -> utf8 (inline ниже)
std::string  utf8_to_oem(const std::string &utf8); // utf8 -> cp866
std::string  ansi_to_utf8(const char *s);          // cp1251 -> utf8
std::string  ansi_to_utf8(const std::string &s);   // cp1251 -> utf8 (inline ниже)
std::string  utf8_to_ansi(const std::string &utf8);// utf8 -> cp1251
std::wstring utf8_to_wide(const std::string &utf8);
std::string  wide_to_utf8(const std::wstring &ws);

} // namespace dbgstr

#endif // STRCONV_H
