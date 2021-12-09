// Include file for `fixChests.c`

#ifndef HEADER_INCLUDE_CHEST
#define HEADER_INCLUDE_CHEST
void fixDoubleChest(char *arg1, char *arg2, char *chest);
void dcAuxLeft(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch);
void dcAuxRight(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch);
#endif
