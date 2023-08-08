#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

static const char* SIG = "CYT"; //signature

/* Checks the requirements by msg in reference to data
Returns a positive number if data is enough to satisfy msg */
static uint32_t check_limit(stegano_t info) {
	return (info.alpha) ? ((info.data_len/4)*3)-((info.msg_len*8)+65) : info.data_len-((info.msg_len*8)+65); //65 bits for the header
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
        if (bit) *(container) += pow(2, ((offset-1) - i)); //addition by 2^index
    } *(info_ptr) = info;
}

/* Reads from the Least Significant Bit (LSB) of data, writing to container until reaching offset
If an alpha channel exists, alpha will be removed and re-attached later on */
static void readFromLSB(stegano_t* info_ptr, uint32_t* container) {
    stegano_t info = *(info_ptr);
    if (info.alpha) {
        unsigned char* excluded_data = (unsigned char*)malloc(info.data_len*sizeof(unsigned char));
        if (excluded_data != NULL) {
            extract_alpha(info, excluded_data);
            writeBitsToContainer(info, excluded_data, container);
            attach_alpha(info, excluded_data);
        } free(excluded_data);
    } else writeBitsToContainer(info, info.data, container);
    *(info_ptr) = info;
}

/* Writes bits to the Least Significant Bit (LSB) of data until it reaches the specified offset
Given a margin, msg is used as an array split with the specified margin. */
static void writeToLSB(stegano_t* info_ptr, const char bits) {
	bool bit;
    stegano_t info = *(info_ptr);
    unsigned char* cur_data, excluded_data;
    if (info.alpha) {
        excluded_data = (unsigned char*)malloc(info.data_len*sizeof(unsigned char));
        if (excluded_data != NULL) {
            extract_alpha(info, excluded_data);
            cur_data = excluded_data;
        }
    } else cur_data = info.data;
	for (; info.cur<info.offset; info.cur++) {
		bit = bits >> ((info.offset-1) - info.cur) & 1; //gets the LSB of bits
		if ((cur_data[info.cur]%2) == 0) cur_data[i] += (bit) ? 1 : 0;
		else cur_data[info.cur] -= (bit) ? 0 : 1;
	}
    if (info.alpha) {
        attach_alpha(info, excluded_data);
        free(excluded_data);
    } *(info_ptr) = info;
}

static void writeToLSBs(stegano_t* info_ptr, uint32_t margin) {
	bool bit;
    stegano_t info = *(info_ptr);
    unsigned char* cur_data, excluded_data;
	uint32_t j = 0, bitcount = margin, k = info.cur+(margin-1);
    if (info.alpha) {
        excluded_data = (unsigned char*)malloc(info.data_len*sizeof(unsigned char));
        if (excluded_data != NULL) {
            extract_alpha(info, excluded_data);
            cur_data = excluded_data;
        }
    } else cur_data = info.data;
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
    if (check_limit(info.data_len, info.msg_len, info.alpha)>0) {
        //Header: signature, system, spacing, length
        info.msg = SIG;
        info.offset += 24;
        writeToLSBs(&info, 8);
        info.offset += 3;
        writeToLSB(info, system);
        info.offset += 6;
        writeToLSB(info, spacing);
        info.offset += 32;
        writeToLSB(info, info.msg_len);
        //Body: message
        info.msg = msg;
        info.offset += info.msg_len*8;
        writeToLSBs(&info, 8);
    } else return false; 
    return true;
}