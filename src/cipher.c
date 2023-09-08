#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "include/cipher.h"

/*Supported special characters*/
static const char* SSC = " .,!?0123456789*$#@%&";

/*All system variables the cipher depends on*/
static const char* SYS_0 = "abcdefghijklmnopqrstuvwxyz";
static const char* SYS_1 = "abcd01efgh45ijkl23mnopqr89stuv6wxyz7";
static const char* SYS_2 = "ab cd01ef?,gh45ij@#$kl23mn.%&opqr89stuv6wx!yz7*";

/*Gets SYS index through pointer arithmetic operation*/
static uint32_t getSYSIndex(const char* ptr, const char* SYS, const char c) {
  ptr = strchr(SYS, c);
  return ptr - SYS;
}

/*Returns the state according to the index of the text array
Note: The if-statements are arranged in a probabilistic way*/
static enum STATE getState(char* text, uint32_t txti) {
  if (0x61<=text[txti] && text[txti]<=0x7a) return LW_CHAR;
  else if (0x41<=text[txti] && text[txti]<=0x5a) return UP_CHAR;
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