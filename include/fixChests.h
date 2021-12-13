// Include file for `fixChests.c`

#ifndef HEADER_INCLUDE_CHEST
#define HEADER_INCLUDE_CHEST
void fixDoubleChests(char *arg1, char *arg2, char *chest);
void dcAuxLeft(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch);
void dcAuxRight(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch);

void fixSingleChests(char *arg1, char *arg2, char *chest);
void chestAux(unsigned char *img, char *outPath, const int q, int w, int h, int ch);
#endif
