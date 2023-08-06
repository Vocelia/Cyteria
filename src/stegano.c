#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

static const char* SIG = "CYT"; //signature

/* Checks the requirements by msg in reference to data
Returns a positive number if data is enough to satisfy msg */
static uint32_t check_limit(uint32_t data_len, uint32_t msg_len, bool alpha) {
	return (alpha) ? ((data_len/4)*3)-((msg_len*8)+65) : data_len-((msg_len*8)+65); //65 bits for the header
}

//Extracts alpha values from data and copies only RGB values to buffer
static void extract_alpha(unsigned char* data, uint32_t data_len, unsigned char* buffer) {
	uint32_t i = 0, k = 3;
	for (; i<data_len; i++) {
		if (i != k) strncat((char*)buffer, (char*)&data[i], 1);
		else k += 4;
	}
}

//Re-attaches alpha values back by copying RGB from excluded_data to data
static void attach_alpha(unsigned char* data, uint32_t data_len, unsigned char* excluded_data) {
	uint32_t i = 0, j = 0, k = 3;
	for (; i<data_len; i++) {
		if (i != k) data[i] = excluded_data[j];
		else { j--; k += 4; }
		j++;
	}
}

/* Writes bits to the Least Significant Bit (LSB) of data until it reaches the specified offset
Given a margin, bits is split into an array with the specified margin. */
static void writeToLSB(unsigned char* data, const char bits, uint32_t* i, uint32_t offset) {
	bool bit;
	uint32_t il = (*i);
	for (; il<offset; il++) {
		//gets the LSB of bits
		bit = bits >> ((offset-1) - il) & 1;
		if ((data[il]%2)==0) data[il] += (bit) ? 1 : 0;
		else data[il] -= (bit) ? 0 : 1;
	}
	(*i) = il;
}

static void writeToLSBs(unsigned char* data, const char* bits, uint32_t* i, uint32_t offset, uint32_t margin) {
	bool bit;
	uint32_t j = 0, il = (*i), bitcount = margin, k = il+(margin-1);
	for (; il<offset; il++) {
		bitcount--;
		//gets the LSB of bits[j]
		bit = bits[j] >> bitcount & 1;
		if ((data[il]%2)==0) data[il] += (bit) ? 1 : 0;
		else data[il] -= (bit) ? 0 : 1;
		//bits shifts into the next member each margin cycle
		if (il==k) {
			j++; k += margin; bitcount = margin;
		}
	}
	(*i) = il;
}
    
/* Hides a message (msg) within data through its Least Significant Bit (LSB)
If an alpha channel exists, its values are extracted and re-attached later on */
bool hide(uint8_t system, uint8_t spacing, unsigned char* data, const char* msg, bool alpha) {
	uint32_t i = 0, offset = 0;
	uint32_t msg_len = strlen(msg);
	uint32_t data_len = strlen((char*)data);
    if (check_limit(data_len, msg_len, alpha)>0) {
    	if (alpha) {
			unsigned char* excluded_data = (unsigned char*)malloc(data_len*sizeof(unsigned char));
			extract_alpha(data, data_len, excluded_data);
			//Header: signature, system, spacing, length
			offset += 24;
			writeToLSBs(excluded_data, SIG, &i, offset, 8);
			offset += 3;
			writeToLSB(excluded_data, system, &i, offset);
			offset += 6;
			writeToLSB(excluded_data, spacing, &i, offset);
			offset += 32;
			writeToLSB(excluded_data, msg_len, &i, offset);
			//Body: message
			offset += msg_len*8;
			writeToLSBs(excluded_data, msg, &i, offset, 8);
			attach_alpha(data, data_len, excluded_data);
    	} else {
    		//Header: signature, system, spacing, length
			offset += 24;
			writeToLSBs(data, SIG, &i, offset, 8);
			offset += 3;
			writeToLSB(data, system, &i, offset);
			offset += 6;
			writeToLSB(data, spacing, &i, offset);
			offset += 32;
			writeToLSB(data, msg_len, &i, offset);
			//Body: message
			offset += msg_len*8;
			writeToLSBs(data, msg, &i, offset, 8);
		}
    } else return false; 
    return true;
}