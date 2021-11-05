// Include file for `convert.c`

#ifndef HEADER_INCLUDE_CONVERT
#define HEADER_INCLUDE_CONVERT
#define MAX_PATH_LENGTH 4096
char *arg1; // Input - Bedrock resource pack path
char *arg2; // Output - Java resource pack path

void checkMissing(void); // Outputs list of missing textures FIXME arraylist
void startConversion(void); // Runs conversion procedures
void genFolders(void); // Creates necessary folders
void copyTextures(void); // Copy over identical textures
void splitCompass(void); // Split compass and clock textures
void splitPaintings(void);
void splitPaintingsAux(void); // Helper function
void copyAnimations(void); // Add default animations

char *initArg(char *arg, char msg[]); // Interactive mode
#endif
