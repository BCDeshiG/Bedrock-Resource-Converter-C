#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "helper.h"
#include "stb_image/stb_image.h"

void safe_create_dir(const char *dir){
    if (mkdir(dir, 0755) < 0) {
        if (errno != EEXIST) {
            perror(dir);
            exit(1);
        }
    }
}

void stripCHAR(char *string, char toStrip){
	char *stripME = strrchr(string, toStrip);
	if (stripME != NULL){
		strcpy(stripME, "\0");
	}
}

void dirSPACES(char *string){
	char *token; // Stores chunk of text
	unsigned short numTokens = 0; // Tracks number of chunks
	const char sp[2] = " ";
	unsigned short length = strlen(string);

	// strtok() modifies so make a copy
	char *tempSTR = calloc(length+1, sizeof(char));
	strcpy(tempSTR, string);

	token = strtok(tempSTR, sp); // Get 1st chunk
	while (token != NULL){
		numTokens++;
		token = strtok(NULL, sp); // Get next chunk
	}
	if (numTokens == 1){ // No splits happened
		free(tempSTR); // Wasn't needed after all
		return;
	}

	// Resize to fit escape characters
	string = realloc(string, (length+numTokens+1)*sizeof(char));
	strcpy(tempSTR, string); // Copy string again
	token = strtok(tempSTR, sp); // Re-get 1st chunk
	strcpy(string, token);
	for (short i=1; i<numTokens; i++){
		token = strtok(NULL, sp); // Get next chunk
		strcat(string, "\ "); // Add escaped space
		strcat(string, token); // Add next chunk
	}
	free(tempSTR);
}

FILE *getFileARG(char *arg, char *path, char *mode){
	unsigned short newLen = strlen(arg)+strlen(path);
	char *fPATH = calloc(newLen+1, sizeof(char));
	strcpy(fPATH, arg);
	strcat(fPATH, path);
	FILE *fPTR = fopen(fPATH, mode);
	free(fPATH);
	return fPTR;
}

char *getImageARG(char *arg, char *path, int *w, int *h, int *ch){
	unsigned short newLen = strlen(arg)+strlen(path);
	char *fPATH = calloc(newLen+1, sizeof(char));
	strcpy(fPATH, arg);
	strcat(fPATH, path);
	unsigned char *img = stbi_load(fPATH, w, h, ch, 0);
	free(fPATH);
	return img;
}

char *intToSTR(int i){
	int tempLen = snprintf(NULL, 0, "%d", i);
	char* intSTR = malloc(tempLen+1);
	snprintf(intSTR, tempLen+1, "%d", i);
	return intSTR;
}

char *crop(unsigned char *img, const int q, int qStat[4], int w, int ch){
	// Region width and height (scaled)
	const int newW = qStat[2]*q;
	const int newH = qStat[3]*q;

	const int imgW = w*ch; // Width of source texture in bytes
	const int regionSize = newW*newH*ch; // Size of cropped region in bytes
	const int newRowSize = newW*ch; // Width of region in bytes
	int count = 0; // Number of bytes read
	int pos = 0; // Position along row
	int shift = (qStat[0]*q*imgW)+(qStat[1]*q*ch); // qTop rows down, qLeft pixels across

	unsigned char *outIMG = calloc(regionSize, sizeof(unsigned char)); // Output image
	while (count < regionSize){
		while (pos < newRowSize){ // Read a row of pixels
			outIMG[count] = img[pos+shift];
			pos++;
			count++;
		}
		shift += imgW; // Shift over to next row
		pos = 0;
	}
	return outIMG;
}

void pasteRegion(unsigned char *region, unsigned char *destIMG,
	const int q, int qStat[4], int w, int ch)
{
	// Region width and height (scaled)
	const int newW = qStat[2]*q;
	const int newH = qStat[3]*q;

	const int imgW = w*ch; // Width of source texture in bytes
	const int regionSize = newW*newH*ch; // Size of region in bytes
	const int newRowSize = newW*ch; // Width of region in bytes
	int count = 0; // Number of bytes read
	int pos = 0; // Position along row
	int shift = (qStat[0]*q*imgW)+(qStat[1]*q*ch); // qTop rows down, qLeft pixels across

	while (count < regionSize){
		while (pos < newRowSize){ // Read a row of pixels
			// Fill area with 0 if blank region, otherwise copy
			destIMG[pos+shift] = (region == NULL) ? 0 : region[count];
			pos++;
			count++;
		}
		shift += imgW; // Shift over to next row
		pos = 0;
	}
}

void rotate(unsigned char *img, const int q, int w, int h, int ch)
{
	const int imgW = q*w*ch; // Width of source texture in bytes
	const int imgSize = q*q*w*h*ch; // Size of region in bytes
	int count = 0; // Number of bytes read

	// Store copy of original
	unsigned char *tempIMG = calloc(imgSize, sizeof(char));
	memcpy(tempIMG, img, imgSize);

	// Basically read the image backwards keeping in mind channels
	while (count < imgSize){
		for (short i=0; i<ch; i++){
			int next = imgSize-count-ch+i;
			img[count+i] = tempIMG[next];
		}
		count += ch;
	}
	// Destroy old copy
	free(tempIMG);
}

void flipUP(unsigned char *img, const int q, int w, int h, int ch)
{
	const int imgW = q*w*ch; // Width of source texture in bytes
	const int imgSize = q*q*w*h*ch; // Size of region in bytes
	int count = 0; // Number of bytes read
	int pos = 0; // Position along row
	int shift = 1; // Number of rows down

	// Store copy of original
	unsigned char *tempIMG = calloc(imgSize, sizeof(char));
	memcpy(tempIMG, img, imgSize);

	while (count < imgSize){
		while (pos < imgW){ // Read a row of pixels
			int next = imgSize-(shift*imgW); // Read from bottom
			img[count] = tempIMG[pos+next];
			pos++;
			count++;
		}
		pos = 0; // Back to beginning of row
		shift++; // Move onto next row
	}
	// Destroy old copy
	free(tempIMG);
}

char *makeOpaque(unsigned char *img, int w, int h){
	int imgSize = w*h*4; // Size of image in bytes
	int count = 0; // Number of bytes read

	unsigned char *outIMG = calloc(imgSize, sizeof(char));
	while (count < imgSize){
		outIMG[count] = ((count+1)%4 == 0) ? 255 : img[count];
		count++;
	}
	return outIMG;
}
