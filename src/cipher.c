#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

//All system variables the cipher depends on
static const char* SYS_0 = "abcdefghijklmnopqrstuvwxyz";
static const char* SYS_1 = "abcd01efgh45ijkl23mnopqr89stuv6wxyz7";
static const char* SYS_2 = "ab cd01ef?,gh45ij@#$kl23mn.%&opqr89stuv6wx!yz7*";

//Spacing range:
//SYS_0 (0~25) | SYS_1 (0~35) | SYS_2 (0~46)
//Any spacing out of boundary may lead to issues
//The buffer allocation space should match text or greater
//The memory block of buffer will be reset in its initial execution
void encrypt(char* text, char* buffer, uint8_t system, uint8_t spacing) {
	const char* ptr;
	unsigned int i, loop;
	memset(buffer, 0, sizeof(buffer));
	switch (system) {
	case 0:
    for (loop=0; loop<strlen(text); loop++) {
      if (0x61<=text[loop] && text[loop]<=0x7a) ptr = strchr(SYS_0, text[loop]);
      else ptr = strchr(SYS_0, text[loop]+0x20);
      if (ptr!=NULL) {
        i = ptr - SYS_0;
        i += (i+spacing<=26) ? spacing : spacing-27;
        sprintf(buffer+strlen(buffer), "%c", (0x61<=text[loop] && text[loop]<=0x7a) ? SYS_0[i] : SYS_0[i]-0x20);
      } else sprintf(buffer+strlen(buffer), "%c", text[loop]);
    }
	break;
	case 1:
    for (loop=0; loop<strlen(text); loop++) {
      if (isalpha(text[loop])) {
        ptr = strchr(SYS_1, (0x61<=text[loop] && text[loop]<=0x7a) ? text[loop] : text[loop]+0x20);
        i = ptr - SYS_1;
        i += (i+spacing<=36) ? spacing : spacing-37;
        if (isalpha(SYS_1[i])) sprintf(buffer+strlen(buffer), "%c", (0x61<=text[loop] && text[loop]<=0x7a) ? SYS_1[i] : SYS_1[i]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_1[i]);
        continue;
      } else ptr = strchr(SYS_1, text[loop]);
      if (ptr!=NULL) {
        i = ptr - SYS_1;
        i += (i+spacing<=36) ? spacing : spacing-37;
        sprintf(buffer+strlen(buffer), "%c", SYS_1[i]);
      } else sprintf(buffer+strlen(buffer), "%c", text[loop]);
    }
	break;
	case 2:
    for (loop=0; loop<strlen(text); loop++) {
      if (isalpha(text[loop])) {
        ptr = strchr(SYS_2, (0x61<=text[loop] && text[loop]<=0x7a) ? text[loop] : text[loop]+0x20);
        i = ptr - SYS_2;
        i += (i+spacing<=46) ? spacing : spacing-47;
        if (isalpha(SYS_2[i])) sprintf(buffer+strlen(buffer), "%c", (0x61<=text[loop] && text[loop]<=0x7a) ? SYS_2[i] : SYS_2[i]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_2[i]);
        continue;
      } else ptr = strchr(SYS_2, text[loop]);
      if (ptr!=NULL) {
        i = ptr - SYS_2;
        i += (i+spacing<=46) ? spacing : spacing-47;
        sprintf(buffer+strlen(buffer), "%c", SYS_2[i]);
      } else sprintf(buffer+strlen(buffer), "%c", text[loop]);
    }
	break;
	}
}

//Spacing range:
//SYS_0 (0~25) | SYS_1 (0~35) | SYS_2 (0~46)
//Any spacing out of boundary may lead to issues
//The buffer allocation space should match text or greater
//The memory block of buffer will be reset in its initial execution
void decrypt(char* text, char* buffer, uint8_t system, uint8_t spacing) {
	const char* ptr;
	int i; unsigned int loop;
	memset(buffer, 0, sizeof(buffer));
	switch (system) {
	case 0:
		for (loop=0; loop<strlen(text); loop++) {
      if (0x61<=text[loop] && text[loop]<=0x7a) ptr = strchr(SYS_0, text[loop]);
      else ptr = strchr(SYS_0, text[loop]+0x20);
      if (ptr!=NULL) {
      	i = ptr - SYS_0;
      	i -= (i-spacing>-1) ? spacing : spacing+25;
        sprintf(buffer+strlen(buffer), "%c", (0x61<=text[loop] && text[loop]<=0x7a) ? SYS_0[i] : SYS_0[i]-0x20);
      } else sprintf(buffer+strlen(buffer), "%c", text[loop]);
    }
	break;
	case 1:
		for (loop=0; loop<strlen(text); loop++) {
      if (isalpha(text[loop])) {
      	ptr = strchr(SYS_1, (0x61<=text[loop] && text[loop]<=0x7a) ? text[loop] : text[loop]+0x20);
      	i = ptr - SYS_1;
      	i -= (i-spacing>-1) ? spacing : spacing+35;
        if (isalpha(SYS_1[i])) sprintf(buffer+strlen(buffer), "%c", (0x61<=text[loop] && text[loop]<=0x7a) ? SYS_1[i] : SYS_1[i]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_1[i]);
        continue;
      } else ptr = strchr(SYS_1, text[loop]);
      if (ptr!=NULL) {
      	i = ptr - SYS_1;
      	i -= (i-spacing>-1) ? spacing : spacing+35;
        sprintf(buffer+strlen(buffer), "%c", SYS_1[i]);
      } else sprintf(buffer+strlen(buffer), "%c", text[loop]);
    }
	break;
	case 2:
		for (loop=0; loop<strlen(text); loop++) {
      if (isalpha(text[loop])) {
      	ptr = strchr(SYS_2, (0x61<=text[loop] && text[loop]<=0x7a) ? text[loop] : text[loop]+0x20);
      	i = ptr - SYS_2;
      	i -= (i-spacing>-1) ? spacing : spacing+47;
        if (isalpha(SYS_2[i])) sprintf(buffer+strlen(buffer), "%c", (0x61<=text[loop] && text[loop]<=0x7a) ? SYS_2[i] : SYS_2[i]-0x20);
        else sprintf(buffer+strlen(buffer), "%c", SYS_2[i]);
        continue;
      } else ptr = strchr(SYS_2, text[loop]);
      if (ptr!=NULL) {
      	i = ptr - SYS_2;
      	i -= (i-spacing>-1) ? spacing : spacing+47;
        sprintf(buffer+strlen(buffer), "%c", SYS_2[i]);
      } else sprintf(buffer+strlen(buffer), "%c", text[loop]);
    }
	break;
	}
}