// Include file for `parse.c`

#ifndef HEADER_INCLUDE_PARSE
#define HEADER_INCLUDE_PARSE
void parseZip(char *arg1); // Determine input format
void parseManifest(char *arg1, char *arg2); // Convert manifest into mcmeta
void parseManAUX(FILE *filePTR); // Helper method
void parseTexts(void); // Process credits and splashes
#endif
