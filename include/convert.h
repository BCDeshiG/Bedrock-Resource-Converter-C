// Include file for `convert.c`

#define MAX_PATH_LENGTH 4096
char *arg1; // Input - Bedrock resource pack path
char *arg2; // Output - Java resource pack path

void checkMissing(void); // Outputs list of missing textures FIXME arraylist
void startConversion(void); // Runs conversion procedures
void parseZip(void); // Determine input format
void parseManifest(void); // Convert manifest into mcmeta
void genFolders(void); // Creates necessary folders
void parseTexts(void); // Process credits and splashes
void copyTextures(void); // Copy over identical textures
void splitCompass(void); // Split compass and clock textures
void splitPaintings(void);
void splitPaintingsAux(void); // Helper function
void copyAnimations(void); // Add default animations

char *initArg(char *arg, char msg[]); // Interactive mode
void resBuffer(unsigned short *buff, int len); // Helper to resize buffer
