// Include file for `split.c`

#ifndef HEADER_INCLUDE_SPLIT
#define HEADER_INCLUDE_SPLIT
void splitCompass(char *arg1, char *arg2, char *atlas); // Split compass/clock textures
void splitPaintings(char *arg1, char *arg2); // Split painting into separate textures
// Atlas, Array of names, Length of array, Unit Size, No. channels, Output path
void splitPaintingAux(unsigned char *img, char *arr[], const int arrLen,
	const int q, int qTop, int qw, int qh, int ch, char *outPath);
// qStat[] is Top, Left, Width, Height
char *cropPainting(unsigned char *img, int q, int qStat[4], int ch);
#endif
