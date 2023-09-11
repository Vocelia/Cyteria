#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "include/cipher.h"

/*Supported special characters*/
static const char* SSC = " .,!?*$#@%&";

/*All system variables the cipher depends on*/
static const char* SYS_0 = "abcdefghijklmnopqrstuvwxyz";
static const char* SYS_1 = "abcd01efgh45ijkl23mnopqr89stuv6wxyz7";
static const char* SYS_2 = "ab cd01ef?,gh45ij@#$kl23mn.%&opqr89stuv6wx!yz7*";

/*Hardcoded SYS lengths to avoid unnecessary computations*/
static const uint8_t SYS_0_len = 26;
static const uint8_t SYS_1_len = 36;
static const uint8_t SYS_2_len = 47;

/*Dumps the specified character to buffer and advances the buffer pointer */
static void charDump(char* buffer, uint32_t* buff_len, const char _char) {
  uint32_t buff_len_val = *(buff_len);
  sprintf(buffer+buff_len_val, "%c", _char);
  buff_len_val++; /*Advances the pointer to the next memory block*/
  *(buff_len) = buff_len_val;
}

/*Returns SYS index through pointer arithmetic operation*/
static uint32_t getSYSIndex(const char* SYS, const char _char) {
  const char* ptr = strchr(SYS, _char);
  return ptr - SYS;
}

/*Returns the state according to the index of the text array
Note: The if-statements are arranged in a probabilistic way*/
static enum STATE getState(char* text, uint32_t txti) {
  if (0x61<=text[txti] && text[txti]<=0x7a) return LW_CHAR;
  else if (0x41<=text[txti] && text[txti]<=0x5a) return UP_CHAR;
  else if (0x30<=text[txti] && text[txti]<=0x39) return INT_CHAR;
  else if (strchr(SSC, text[txti])!=NULL) return SS_CHAR;
  else return FS_CHAR;
}

/*Encodes a character according to the given case
All cases:
  0 = encode (normal),
  1 = encode (uppercase),
  2 = ignore*/
static void encode(cipher_t* cipher_ptr, uint8_t _case, const char* SYS, const uint8_t SYS_len) {
  cipher_t cipher = *(cipher_ptr);
  switch (_case) {
    case 0:
      cipher.sysi = getSYSIndex(SYS, cipher.text[cipher.txti]);
      cipher.sysi = (cipher.spacing+cipher.sysi)%SYS_len;
      charDump(cipher.buffer, &cipher.buff_len, SYS[cipher.sysi]);
      break;
    case 1:
      cipher.sysi = getSYSIndex(SYS, cipher.text[cipher.txti]+0x20);
      cipher.sysi = (cipher.spacing+cipher.sysi)%SYS_len;
      charDump(cipher.buffer, &cipher.buff_len, toupper(SYS[cipher.sysi]));
      break;
    case 2:
      cipher.sysi = getSYSIndex(SYS, cipher.text[cipher.txti]);
      charDump(cipher.buffer, &cipher.buff_len, cipher.text[cipher.txti]);
      break;
  } *(cipher_ptr) = cipher;
}

/*Decodes a character according to the given case
All cases:
  0 = encode (normal),
  1 = encode (uppercase),
  2 = ignore*/
static void decode(cipher_t* cipher_ptr, uint8_t _case, const char* SYS, const uint8_t SYS_len) {
  int32_t decoded_pos;
  cipher_t cipher = *(cipher_ptr);
  switch (_case) {
    case 0:
      cipher.sysi = getSYSIndex(SYS, cipher.text[cipher.txti]);
      /*Decodes character by calculating positions through spacing*/
      decoded_pos = cipher.sysi-cipher.spacing;
      if (decoded_pos<0) cipher.sysi = SYS_len+decoded_pos;
      else cipher.sysi = decoded_pos%SYS_len;
      charDump(cipher.buffer, &cipher.buff_len, SYS[cipher.sysi]);
      break;
    case 1:
      cipher.sysi = getSYSIndex(SYS, cipher.text[cipher.txti]+0x20);
      /*Decodes character by calculating positions through spacing*/
      decoded_pos = cipher.sysi-cipher.spacing;
      if (decoded_pos<0) cipher.sysi = SYS_len+decoded_pos;
      else cipher.sysi = decoded_pos%SYS_len;
      charDump(cipher.buffer, &cipher.buff_len, toupper(SYS[cipher.sysi]));
      break;
    case 2:
      cipher.sysi = getSYSIndex(SYS, cipher.text[cipher.txti]);
      charDump(cipher.buffer, &cipher.buff_len, cipher.text[cipher.txti]);
      break;
  } *(cipher_ptr) = cipher;
}

/*Encrypts given text to buffer according to system and spacing
Spacing's ranges are:
SYS_0 (0~25) | SYS_1 (0~35) | SYS_2 (0~46)
Any spacing out of boundary may lead to unexpected exceptions*/
void encrypt(char* text, char* buffer, uint8_t system, uint8_t spacing) {
  /*Initialising cipher_t and its variables*/
  cipher_t cipher;
  cipher.txti = 0;
  cipher.sysi = 0;
  cipher.text = text;
  cipher.buffer = buffer;
  cipher.system = system;
  cipher.spacing = spacing;
  cipher.buff_len = strlen(buffer);
  switch (system) {
    case 0:
      for (; cipher.txti<strlen(text); cipher.txti++) {
        cipher.state = getState(text, cipher.txti);
        switch (cipher.state) {
          case LW_CHAR:
            encode(&cipher, 0, SYS_0, SYS_0_len);
            break;
          case UP_CHAR:
            encode(&cipher, 1, SYS_0, SYS_0_len);
            break;
          default:
            encode(&cipher, 2, SYS_0, SYS_0_len);
            break;
        }
      }
      break;
    case 1:
      for (; cipher.txti<strlen(text); cipher.txti++) {
        cipher.state = getState(text, cipher.txti);
        switch (cipher.state) {
          case LW_CHAR:
            encode(&cipher, 0, SYS_1, SYS_1_len);
            break;
          case UP_CHAR:
            encode(&cipher, 1, SYS_1, SYS_1_len);
            break;
          case INT_CHAR:
            encode(&cipher, 0, SYS_1, SYS_1_len);
            break;
          default:
            encode(&cipher, 2, SYS_1, SYS_1_len);
            break;
        }
      }
      break;
    case 2:
      for (; cipher.txti<strlen(text); cipher.txti++) {
        cipher.state = getState(text, cipher.txti);
        switch (cipher.state) {
          case LW_CHAR:
            encode(&cipher, 0, SYS_2, SYS_2_len);
            break;
          case UP_CHAR:
            encode(&cipher, 1, SYS_2, SYS_2_len);
            break;
          case INT_CHAR:
            encode(&cipher, 0, SYS_2, SYS_2_len);
            break;
          case SS_CHAR:
            encode(&cipher, 0, SYS_2, SYS_2_len);
            break;
          default:
            encode(&cipher, 2, SYS_2, SYS_2_len);
            break;
        }
      }
      break;
  }
}

/*Decrypts given text to buffer according to system and spacing
Spacing's ranges are:
SYS_0 (0~25) | SYS_1 (0~35) | SYS_2 (0~46)
Any spacing out of boundary may lead to unexpected exceptions*/
void decrypt(char* text, char* buffer, uint8_t system, uint8_t spacing) {
  /*Initialising cipher_t and its variables*/
  cipher_t cipher;
  cipher.txti = 0;
  cipher.sysi = 0;
  cipher.text = text;
  cipher.buffer = buffer;
  cipher.system = system;
  cipher.spacing = spacing;
  cipher.buff_len = strlen(buffer);
  switch (system) {
    case 0:
      for (; cipher.txti<strlen(text); cipher.txti++) {
        cipher.state = getState(text, cipher.txti);
        switch (cipher.state) {
          case LW_CHAR:
            decode(&cipher, 0, SYS_0, SYS_0_len);
            break;
          case UP_CHAR:
            decode(&cipher, 1, SYS_0, SYS_0_len);
            break;
          default:
            decode(&cipher, 2, SYS_0, SYS_0_len);
            break;
        }
      }
      break;
    case 1:
      for (; cipher.txti<strlen(text); cipher.txti++) {
        cipher.state = getState(text, cipher.txti);
        switch (cipher.state) {
          case LW_CHAR:
            decode(&cipher, 0, SYS_1, SYS_1_len);
            break;
          case UP_CHAR:
            decode(&cipher, 1, SYS_1, SYS_1_len);
            break;
          case INT_CHAR:
            decode(&cipher, 0, SYS_1, SYS_1_len);
            break;
          default:
            decode(&cipher, 2, SYS_1, SYS_1_len);
            break;
        }
      }
      break;
    case 2:
      for (; cipher.txti<strlen(text); cipher.txti++) {
        cipher.state = getState(text, cipher.txti);
        switch (cipher.state) {
          case LW_CHAR:
            decode(&cipher, 0, SYS_2, SYS_2_len);
            break;
          case UP_CHAR:
            decode(&cipher, 1, SYS_2, SYS_2_len);
            break;
          case INT_CHAR:
            decode(&cipher, 0, SYS_2, SYS_2_len);
            break;
          case SS_CHAR:
            decode(&cipher, 0, SYS_2, SYS_2_len);
            break;
          default:
            decode(&cipher, 2, SYS_2, SYS_2_len);
            break;
        }
      }
      break;
  }
}