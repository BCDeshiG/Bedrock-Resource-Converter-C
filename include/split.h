// Include file for `split.c`

#ifndef HEADER_INCLUDE_SPLIT
#define HEADER_INCLUDE_SPLIT
void splitCompass(char *arg1, char *arg2, char *atlas); // Split compass/clock textures
void splitPaintings(char *arg1, char *arg2); // Split painting into separate textures
// Atlas, Names, Array Length, Unit Size, Top, Width, Height, Channels, Output path
void paintingAux1(unsigned char *img, char *arr[], const int arrLen,
	const int q, int qTop, int qw, int qh, int ch, char *outPath);
// Atlas, Name of painting, Unit Size, Top, Left, Width, Height, Channels, Output path
void paintingAux2(unsigned char *img, char *name, const int q,
	int qTop, int qLeft, int qw, int qh, int ch, char *outPath);
// qStat[] is Top, Left, Width, Height
char *cropPainting(unsigned char *img, int q, int qStat[4], int ch);
#endif
