#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "include/stegano.h"

static const char* SIG = "CYT"; //signature
static void readFromLSBs(stegano_t* info_ptr, char* container, uint32_t margin);

/* Checks the requirements by msg in reference to data
Returns a positive number if data is enough to satisfy msg */
static int32_t check_limit(stegano_t info) {
	return info.data_len-((info.msg_len*8)+65); /*65 bits for the header*/
}

/* Checks for the validaty of the data stream */
static bool check_sig(stegano_t info) {
    info.cur = 0;
    info.offset = 24; /* 3*8 bits */
    char* intro = (char*)malloc(3*sizeof(char));
    readFromLSBs(&info, intro, 8);
    bool rtn = (strcmp(SIG, intro) == 0) ? true : false;
    free(intro);
    return rtn;
}

/* Reads from the Least Significant Bit (LSB) of data, writing to container until reaching offset
Given a margin, container is used as an array split with the specified margin. */
static void readFromLSB(stegano_t* info_ptr, uint32_t* container) {
    bool bit;
    stegano_t info = *(info_ptr);
    *(container) = 0; /* Fallback for unallocated container */ 
    for (; info.cur<info.offset; info.cur++) {
        bit = info.data[info.cur] >> 0 & 1; //gets the LSBs of data
        if (bit) *(container) += pow(2, ((info.offset-1) - info.cur)); //addition by 2^index
    } *(info_ptr) = info;
}

static void readFromLSBs(stegano_t* info_ptr, char* container, uint32_t margin) {
    bool bit;
    stegano_t info = *(info_ptr);
    uint32_t j = 0, bitcount = margin, k = info.cur+(margin-1);
    for (; info.cur<info.offset; info.cur++) {
        bitcount--;
        bit = info.data[info.cur] >> 0 & 1; //gets the LSBs of data
        if (bit) container[j] += pow(2, bitcount); //addition by 2^index
        if (info.cur == k) { //container shifts into the next member each margin cycle
			j++; k += margin; bitcount = margin;
		}
    } *(info_ptr) = info;
}

/* Writes bits to the Least Significant Bit (LSB) of data until it reaches the specified offset
Given a margin, msg is used as an array split with the specified margin. */
static void writeToLSB(stegano_t* info_ptr, const char bits) {
	bool bit;
    stegano_t info = *(info_ptr);
	for (; info.cur<info.offset; info.cur++) {
		bit = bits >> ((info.offset-1) - info.cur) & 1; //gets the LSB of bits
		if ((info.data[info.cur]%2) == 0) info.data[info.cur] += (bit) ? 1 : 0;
		else info.data[info.cur] -= (bit) ? 0 : 1;
	} *(info_ptr) = info;
}

static void writeToLSBs(stegano_t* info_ptr, uint32_t margin) {
	bool bit;
    stegano_t info = *(info_ptr);
	uint32_t j = 0, bitcount = margin, k = info.cur+(margin-1);
	for (; info.cur<info.offset; info.cur++) {
		bitcount--;
		bit = info.msg[j] >> bitcount & 1; //gets the LSB of bits[j]
		if ((info.data[info.cur]%2) == 0) info.data[info.cur] += (bit) ? 1 : 0;
		else info.data[info.cur] -= (bit) ? 0 : 1;
		if (info.cur == k) { //msg shifts into the next member each margin cycle
			j++; k += margin; bitcount = margin;
		}
	} *(info_ptr) = info;
}
    
/* Hides a message (msg) within data through its Least Significant Bit (LSB)
Requirements: info.data, info.msg, info.msg_len, info.data_len */
bool hide(stegano_t* info_ptr, uint8_t system, uint8_t spacing) {
    stegano_t info = *(info_ptr);
    const char* msg = info.msg; /* msg pointer as info.msg will be overwritten with SIG */
    if (check_limit(info)>0) {
        /* Header: signature, system, spacing, length */
        info.cur = 0;
        info.offset = 0;
        info.msg = SIG;
        info.offset += 24;
        writeToLSBs(&info, 8);
        info.offset += 3;
        writeToLSB(&info, system);
        info.offset += 6;
        writeToLSB(&info, spacing);
        info.offset += 32;
        writeToLSB(&info, info.msg_len);
        /* Body: message */
        info.msg = msg;
        info.offset += info.msg_len*8;
        writeToLSBs(&info, 8);
        *(info_ptr) = info;
    } else return false;
    return true;
}

/* Reveals a message (msg) within data through its Least Significant Bit (LSB)
Requirements: info.data, info.msg (buffer), info.msg_len, info.data_len */
bool reveal(stegano_header_t* header, stegano_t* info_ptr, char* buffer) {
    stegano_t info = *(info_ptr);
    stegano_header_t head = *(header);
    if (check_sig(info)) {
        /* SIG is skipped */
        info.cur = 24;
        info.offset = 27;
        /* Header: system, spacing, length */
        readFromLSB(&info, (uint32_t*)&head.system);
        info.offset += 6;
        readFromLSB(&info, (uint32_t*)&head.spacing);
        info.offset += 32;
        readFromLSB(&info, &head.len);
        /* Body: message */
        info.offset += head.len*8;
        readFromLSBs(&info, buffer, 8);
        *(header) = head;
        *(info_ptr) = info;
    } else return false;
    return true;
}
