// Include file for `fixTextures.c`

#ifndef HEADER_INCLUDE_FIX
#define HEADER_INCLUDE_FIX
void splitCompass(char *arg1, char *arg2, char *atlas); // Split compass/clock textures
void splitPaintings(char *arg1, char *arg2); // Split painting into separate textures
void splitPaintingsAux(unsigned char *img, char *arr[],
		short qtop, short qw, short qh, char *arg2); // Helper function

void fixBeds(char *arg1, char *arg2);
void fixChests(char *arg1, char *arg2, char *chest);
void fixZombies(char *arg1, char *arg2, char *zombie);
void fixDrowned(char *arg1, char *arg2);
void fixSheep(char *arg1, char *arg2);
void fixHoglins(char *arg1, char *arg2, char *hog);
void fixFoxes(char *arg1, char *arg2, char *fox);
void fixDog(char *arg1, char *arg2);
void fixCat(char *arg1, char *arg2);
void fixAzalea(char *arg1, char *arg2, char *bush);

void fixes(char *arg1, char *arg2); // Calls the other methods
#endif
