// Include file for `fixTextures.c`

#ifndef HEADER_INCLUDE_FIX
#define HEADER_INCLUDE_FIX
void fixBeds(char *arg1, char *arg2);
void fixZombies(char *arg1, char *arg2, char *zombie);
void fixDrowned(char *arg1, char *arg2);
void fixSheep(char *arg1, char *arg2);
void fixHoglins(char *arg1, char *arg2, char *hog);
void fixFoxes(char *arg1, char *arg2, char *fox);
void fixDog(char *arg1, char *arg2);
void fixCat(char *arg1, char *arg2);
void fixAzalea(char *arg1, char *arg2, char *bush);

void fixes(char *arg1, char *arg2); // Calls the other methods

// Helper methods
void fixBedsAux(unsigned char *bed, const int q, int w, int h, int ch, char *outPath);
#endif
