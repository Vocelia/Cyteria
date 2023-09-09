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

/*Encodes a character by moving sysi pointer spacing times*/
static uint32_t encode(const char* SYS, const uint8_t len, uint32_t sysi, uint8_t spacing) {
  if (sysi+spacing<=(len-1)) return sysi+spacing;
  else return sysi-(spacing+(len-6));
}

/*Dumps the specified character to buffer and advances the buffer pointer */
static void charDump(char* buffer, uint32_t* buff_len, const char _char) {
  uint32_t buff_len_val = *(buff_len);
  sprintf(buffer+buff_len_val, "%c", _char);
  buff_len_val++; /*Advances the pointer to the next memory block*/
  *(buff_len) = buff_len_val;
}

/*Returns SYS index through pointer arithmetic operation*/
static uint32_t getSYSIndex(const char* ptr, const char* SYS, const char _char) {
  ptr = strchr(SYS, _char);
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

//spacing range:
//SYS_0 (0~25) | SYS_1 (0~35) | SYS_2 (0~46)
//Any spacing out of boundary may lead to issues
//The buffer allocation space should match text or greater
//The memory block of buffer will be reset in its initial execution
void encrypt(char* text, char* buffer, uint8_t system, uint8_t spacing) {
  const char* ptr;
  uint32_t sysi, txti; /*sysi - SYS index, txti - text index*/
  memset(buffer, 0, sizeof(buffer));
  switch (system) {
  case 0:
    for (txti=0; txti<strlen(text); txti++) {
      if (islower(text[txti])) ptr = strchr(SYS_0, text[txti]);
      else ptr = strchr(SYS_0, text[txti]+0x20);
      if (ptr!=NULL) {
        sysi = ptr - SYS_0; /*pointer arithmetic to get character index in SYS*/
        sysi += (sysi+spacing<=26) ? spacing : spacing-27; /*character encryption process*/
        sprintf(buffer+strlen(buffer), "%c", (islower(text[txti])) ? SYS_0[sysi] : SYS_0[sysi]-0x20);
      } else sprintf(buffer+strlen(buffer), "%c", text[txti]);
    }
  break;
  case 1:
    for (txti=0; txti<strlen(text); txti++) {
      if (isalpha(text[txti])) {
        ptr = strchr(SYS_1, (islower(text[txti])) ? text[txti] : text[txti]+0x20);
        sysi = ptr - SYS_1;
        sysi += (sysi+spacing<=36) ? spacing : spacing-37;
        if (isalpha(SYS_1[sysi])) sprintf(buffer+strlen(buffer), "%c", (islower(text[txti])) ? SYS_1[sysi] : SYS_1[sysi]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_1[sysi]);
        continue;
      } else ptr = strchr(SYS_1, text[txti]);
      if (ptr!=NULL) {
        sysi = ptr - SYS_1;
        sysi += (sysi+spacing<=36) ? spacing : spacing-37;
        sprintf(buffer+strlen(buffer), "%c", SYS_1[sysi]);
      } else sprintf(buffer+strlen(buffer), "%c", text[txti]);
    }
  break;
  case 2:
    for (txti=0; txti<strlen(text); txti++) {
      if (isalpha(text[txti])) {
        ptr = strchr(SYS_2, (islower(text[txti])) ? text[txti] : text[txti]+0x20);
        sysi = ptr - SYS_2;
        sysi += (sysi+spacing<=46) ? spacing : spacing-47;
        if (isalpha(SYS_2[sysi])) sprintf(buffer+strlen(buffer), "%c", (islower(text[txti])) ? SYS_2[sysi] : SYS_2[sysi]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_2[sysi]);
        continue;
      } else ptr = strchr(SYS_2, text[txti]);
      if (ptr!=NULL) {
        sysi = ptr - SYS_2;
        sysi += (sysi+spacing<=46) ? spacing : spacing-47;
        sprintf(buffer+strlen(buffer), "%c", SYS_2[sysi]);
      } else sprintf(buffer+strlen(buffer), "%c", text[txti]);
    }
  break;
  }
}

//spacing range:
//SYS_0 (0~25) | SYS_1 (0~35) | SYS_2 (0~46)
//Any spacing out of boundary may lead to issues
//The buffer allocation space should match text or greater
//The memory block of buffer will be reset in its initial execution
void decrypt(char* text, char* buffer, uint8_t system, uint8_t spacing) {
  int32_t sysi; 
  uint32_t txti; /*sysi - SYS index, txti - text index*/
  const char* ptr;
  memset(buffer, 0, sizeof(buffer));
  switch (system) {
  case 0:
    for (txti=0; txti<strlen(text); txti++) {
      if (islower(text[txti])) ptr = strchr(SYS_0, text[txti]);
      else ptr = strchr(SYS_0, text[txti]+0x20);
      if (ptr!=NULL) {
        sysi = ptr - SYS_0;
        sysi -= (sysi-spacing>-1) ? spacing : spacing+25;
        sprintf(buffer+strlen(buffer), "%c", (islower(text[txti])) ? SYS_0[sysi] : SYS_0[sysi]-0x20);
      } else sprintf(buffer+strlen(buffer), "%c", text[txti]);
    }
  break;
  case 1:
    for (txti=0; txti<strlen(text); txti++) {
      if (isalpha(text[txti])) {
        ptr = strchr(SYS_1, (islower(text[txti])) ? text[txti] : text[txti]+0x20);
        sysi = ptr - SYS_1;
        sysi -= (sysi-spacing>-1) ? spacing : spacing+35;
        if (isalpha(SYS_1[sysi])) sprintf(buffer+strlen(buffer), "%c", (islower(text[txti])) ? SYS_1[sysi] : SYS_1[sysi]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_1[sysi]);
        continue;
      } else ptr = strchr(SYS_1, text[txti]);
      if (ptr!=NULL) {
        sysi = ptr - SYS_1;
        sysi -= (sysi-spacing>-1) ? spacing : spacing+35;
        sprintf(buffer+strlen(buffer), "%c", SYS_1[sysi]);
      } else sprintf(buffer+strlen(buffer), "%c", text[txti]);
    }
  break;
  case 2:
    for (txti=0; txti<strlen(text); txti++) {
      if (isalpha(text[txti])) {
        ptr = strchr(SYS_2, (islower(text[txti])) ? text[txti] : text[txti]+0x20);
        sysi = ptr - SYS_2;
        sysi -= (sysi-spacing>-1) ? spacing : spacing+47;
        if (isalpha(SYS_2[sysi])) sprintf(buffer+strlen(buffer), "%c", (islower(text[txti])) ? SYS_2[sysi] : SYS_2[sysi]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_2[sysi]);
        continue;
      } else ptr = strchr(SYS_2, text[txti]);
      if (ptr!=NULL) {
        sysi = ptr - SYS_2;
        sysi -= (sysi-spacing>-1) ? spacing : spacing+47;
        sprintf(buffer+strlen(buffer), "%c", SYS_2[sysi]);
      } else sprintf(buffer+strlen(buffer), "%c", text[txti]);
    }
  break;
  }
}