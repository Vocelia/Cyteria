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
static uint32_t check_limit(stegano_t info) {
	return (info.alpha) ? ((info.data_len/4)*3)-((info.msg_len*8)+65) : info.data_len-((info.msg_len*8)+65); //65 bits for the header
}

//Checks for the validaty of the data stream
static bool check_sig(stegano_t info) {
    info.cur = 0;
    info.offset = 24; //3*8 bits
    char* intro = (char*)malloc(3*sizeof(char));
    readFromLSBs(&info, intro, 8);
    return (strcmp(SIG, intro) == 0) ? true : false;
}

//Extracts alpha values from data and copies only RGB values to buffer
static void extract_alpha(stegano_t info, unsigned char* buffer) {
	uint32_t i = 0, k = 3;
	for (; i<info.data_len; i++) {
		if (i != k) strncat((char*)buffer, (char*)&info.data[i], 1);
		else k += 4;
	}
}

//Re-attaches alpha values back by copying RGB from excluded_data to data
static void attach_alpha(stegano_t info, unsigned char* excluded_data) {
	uint32_t i = 0, j = 0, k = 3;
	for (; i<info.data_len; i++) {
		if (i != k) info.data[i] = excluded_data[j];
		else { j--; k += 4; }
		j++;
	}
}

static void writeBitsToContainer(stegano_t* info_ptr, unsigned char* data, uint32_t* container) {
    bool bit;
    stegano_t info = *(info_ptr);
    for (; info.cur<info.offset; info.cur++) {
        bit = data[info.cur] >> 0 & 1; //gets the LSBs of data
        if (bit) *(container) += pow(2, ((info.offset-1) - info.cur)); //addition by 2^index
    } *(info_ptr) = info;
}

static void writeBitsToContainers(stegano_t* info_ptr, unsigned char* data, char* container, uint32_t margin) {
    bool bit;
    stegano_t info = *(info_ptr);
    uint32_t j = 0, bitcount = margin, k = info.cur+(margin-1);
    for (; info.cur<info.offset; info.cur++) {
        bitcount--;
        bit = data[info.cur] >> 0 & 1; //gets the LSBs of data
        if (bit) container[j] += pow(2, bitcount); //addition by 2^index
        if (info.cur == k) { //container shifts into the next member each margin cycle
			j++; k += margin; bitcount = margin;
		}
    } *(info_ptr) = info;
}

/* Reads from the Least Significant Bit (LSB) of data, writing to container until reaching offset
If an alpha channel exists, alpha will be removed and re-attached later on
Given a margin, container is used as an array split with the specified margin. */
static void readFromLSB(stegano_t* info_ptr, uint32_t* container) {
    stegano_t info = *(info_ptr);
    if (info.alpha) {
        unsigned char* excluded_data = (unsigned char*)malloc(info.data_len*sizeof(unsigned char));
        if (excluded_data != NULL) {
            extract_alpha(info, excluded_data);
            writeBitsToContainer(&info, excluded_data, container);
            attach_alpha(info, excluded_data);
        } free(excluded_data);
    } else writeBitsToContainer(&info, info.data, container);
    *(info_ptr) = info;
}

static void readFromLSBs(stegano_t* info_ptr, char* container, uint32_t margin) {
    stegano_t info = *(info_ptr);
    if (info.alpha) {
        unsigned char* excluded_data = (unsigned char*)malloc(info.data_len*sizeof(unsigned char));
        if (excluded_data != NULL) {
            extract_alpha(info, excluded_data);
            writeBitsToContainers(&info, excluded_data, container, margin);
            attach_alpha(info, excluded_data);
        } free(excluded_data);
    } else writeBitsToContainers(&info, info.data, container, margin);
    *(info_ptr) = info;
}

/* Writes bits to the Least Significant Bit (LSB) of data until it reaches the specified offset
Given a margin, msg is used as an array split with the specified margin. */
static void writeToLSB(stegano_t* info_ptr, const char bits) {
	bool bit;
    unsigned char* cur_data;
    unsigned char* excluded_data;
    stegano_t info = *(info_ptr);
    if (info.alpha) {
        excluded_data = (unsigned char*)malloc(info.data_len*sizeof(unsigned char));
        if (excluded_data != NULL) {
            extract_alpha(info, excluded_data);
            cur_data = excluded_data; /* current data variable */
        }
    } else cur_data = info.data; /* current data variable */
	for (; info.cur<info.offset; info.cur++) {
		bit = bits >> ((info.offset-1) - info.cur) & 1; //gets the LSB of bits
		if ((cur_data[info.cur]%2) == 0) cur_data[info.cur] += (bit) ? 1 : 0;
		else cur_data[info.cur] -= (bit) ? 0 : 1;
	}
    if (info.alpha) {
        attach_alpha(info, excluded_data);
        free(excluded_data);
    } *(info_ptr) = info;
}

static void writeToLSBs(stegano_t* info_ptr, uint32_t margin) {
	bool bit;
    unsigned char* cur_data;
    unsigned char* excluded_data;
    stegano_t info = *(info_ptr);
	uint32_t j = 0, bitcount = margin, k = info.cur+(margin-1);
    if (info.alpha) {
        excluded_data = (unsigned char*)malloc(info.data_len*sizeof(unsigned char));
        if (excluded_data != NULL) {
            extract_alpha(info, excluded_data);
            cur_data = excluded_data; /* current data variable */
        }
    } else cur_data = info.data; /* current data variable */
	for (; info.cur<info.offset; info.cur++) {
		bitcount--;
		bit = info.msg[j] >> bitcount & 1; //gets the LSB of bits[j]
		if ((cur_data[info.cur]%2) == 0) cur_data[info.cur] += (bit) ? 1 : 0;
		else cur_data[info.cur] -= (bit) ? 0 : 1;
		if (info.cur == k) { //msg shifts into the next member each margin cycle
			j++; k += margin; bitcount = margin;
		}
	}
    if (info.alpha) {
        attach_alpha(info, excluded_data);
        free(excluded_data);
    } *(info_ptr) = info;
}
    
/* Hides a message (msg) within data through its Least Significant Bit (LSB)
If an alpha channel exists, its values are extracted and re-attached later on */
bool hide(uint8_t system, uint8_t spacing, unsigned char* data, const char* msg, bool alpha) {
    stegano_t info;
    info.cur = 0;
    info.offset = 0;
    info.data = data;
    info.alpha = alpha;
    info.msg_len = strlen(msg);
    info.data_len = strlen((char*)data);
    if (check_limit(info)>0) {
        /* Header: signature, system, spacing, length */
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
    } else return false; 
    return true;
}

/* Reveals a message (msg) within data through its Least Significant Bit (LSB)
If an alpha channel exists, its values are extracted and re-attached later on */
bool reveal(stegano_header_t* header, unsigned char* data, char* buffer, bool alpha) {
    stegano_t info;
    info.data = data;
    info.alpha = alpha;
    info.data_len = strlen((char*)data);
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
    } else return false;
    return true;
}