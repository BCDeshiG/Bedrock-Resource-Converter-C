// Include file for `parse.c`

#ifndef HEADER_INCLUDE_PARSE
#define HEADER_INCLUDE_PARSE
void parseZip(char *arg); // Determine input format
void parseManifest(void); // Convert manifest into mcmeta
void parseTexts(void); // Process credits and splashes
#endif
