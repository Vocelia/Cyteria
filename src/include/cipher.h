#pragma once

#include <stdint.h>

/*0 = alphanumeric character,
1 = lowercase alphabetic character,
2 = uppercase alphabetic character, 
3 = supported special character,
4 = foreign special character or multi-byte character*/
enum STATE {
	LW_CHAR = 1, UP_CHAR = 2,
	INT_CHAR = 0, SS_CHAR = 3, FS_CHAR = 4
};

typedef struct {
	char* text;
	char* buffer;
	uint32_t buff_len;
	enum STATE state;
	uint32_t sysi; /*sysi - SYS index*/
	uint32_t txti; /*txti - text index*/
	uint8_t system;
	uint8_t spacing;
} cipher_t;

void encrypt(char* text, char* buffer, uint8_t system, uint8_t spacing);
void decrypt(char* text, char* buffer, uint8_t system, uint8_t spacing);