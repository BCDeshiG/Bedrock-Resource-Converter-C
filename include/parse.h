// Include file for `parse.c`

#ifndef HEADER_INCLUDE_PARSE
#define HEADER_INCLUDE_PARSE
void parseZip(char *arg1); // Determine input format
char *parseManifest(char *arg1, char *arg2); // Convert manifest into mcmeta
void parseManAUX(FILE *filePTR, char *arg2); // Helper method
void parseEndText(char *arg1, char *arg2); // Process end poem text
void parseSplashes(char *arg1, char *arg2); // Process splash texts
#endif
