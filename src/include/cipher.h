#pragma once

#include <stdint.h>

/*0 = supported special character,
1 = lowercase alphabetic character,
2 = uppercase alphabetic character, 
3 = foreign special character or multi-byte character*/
enum STATE {
  SS_CHAR = 0, FS_CHAR = 3,
  LW_CHAR = 1, UP_CHAR = 2
};

void encrypt(char* text, char* buffer, uint8_t system, uint8_t spacing);
void decrypt(char* text, char* buffer, uint8_t system, uint8_t spacing);