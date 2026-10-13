#include "convert.h"
#include <QTextCodec>
#include <Windows.h>

void Win2Dos(char* str)
{
    CharToOemA(str, str);
}

void Dos2Win(char* str)
{
    OemToCharA(str, str);
}
