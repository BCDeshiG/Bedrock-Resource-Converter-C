// Include file for `convert.c`

#ifndef HEADER_INCLUDE_COPY
#define HEADER_INCLUDE_COPY
void copyTextures(char *arg1, char *arg2); // Copy over identical textures
char *getPathCSV(char *arg, char *buf); // Helper method to get file path
void copyPNG(FILE *innPTR, FILE *outPTR); // Helper method to copy PNG file
void checkMissing(void); // Outputs list of missing textures FIXME arraylist
void copyAnimations(char *arg2); // Add default animations
#endif
