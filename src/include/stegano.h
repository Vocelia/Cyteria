#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool alpha;
    uint32_t offset;
    uint32_t* cursor;
    uint32_t msg_len;
    uint32_t data_len;
    const char* msg;
    unsigned char* data;
} stegano_t;

bool hide(uint8_t system, uint8_t spacing, unsigned char* data, const char* msg, bool alpha);