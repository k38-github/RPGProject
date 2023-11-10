#include <stdio.h>
#include "../headers/u8mb.h"
#include "../../parts.h"

// UTF-8文字の1バイト目を判定して文字のバイト数を返す関数
int u8mb(const char chr)
{
    int byt = 1;
    if ((chr & 0x80) == 0x00) { //1byte文字は何もしない（[byt = 1]のまま）
    } else if ((chr & 0xE0) == 0xC0) { //2byte文字
        byt = 2;
    } else if ((chr & 0xF0) == 0xE0) { //3byte文字
        byt = 3;
    } else if ((chr & 0xF8) == 0xF0) { //4byte文字
        byt = 4;
    } else if ((chr & 0xFC) == 0xF8) { //5byte文字
        byt = 5;
    } else if ((chr & 0xFE) == 0xFC) { //6byte文字
        byt = 6;
    }
    return byt;
}

