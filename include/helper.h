// Include file for `helper.c`

#ifndef HEADER_INCLUDE_HELPER
#define HEADER_INCLUDE_HELPER
void safe_create_dir(const char *dir); // Create directory
void stripCHAR(char *string, char toStrip); // Strip characters from end of string
void dirSPACES(char *string); // Escape spaces in path name
FILE *getFileARG(char *arg, char *path, char *mode); // Helper to open file
char *intToSTR(int i); // Convert int to string
// qStat[] is Top, Left, Width, Height (scaled)
char *crop(unsigned char *img, const int q, int qStat[4], int w, int ch);
void debugIMG(unsigned char *outIMG); // Prints RGBA values of each pixel
// Paste a region onto an image (deletes area if region is NULL)
void pasteRegion(unsigned char *region, unsigned char *destIMG,
	const int q, int qStat[4], int w, int ch);
#endif
