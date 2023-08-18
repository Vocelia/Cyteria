#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t cur; //cursor
    uint32_t offset;
    uint32_t msg_len;
    uint32_t data_len;
    const char* msg;
    unsigned char* data;
} stegano_t;

typedef struct {
    uint32_t len;
    uint8_t system;
    uint8_t spacing;
} stegano_header_t;

bool hide(stegano_t* info_ptr, uint8_t system, uint8_t spacing);
bool reveal(stegano_header_t* head, stegano_t* info_ptr, char* buffer);
