#pragma once

#include <stdint.h>

void encrypt(char* text, char* buffer, uint8_t system, uint8_t spacing);
void decrypt(char* text, char* buffer, uint8_t system, uint8_t spacing);