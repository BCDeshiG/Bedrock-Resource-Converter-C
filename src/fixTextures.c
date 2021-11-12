#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fixTextures.h"
#include "helper.h"

#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#define STBI_MAX_DIMENSIONS (1<<14) // Max texture size of 16k (way overkill)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

// compass_atlas.png -> compass_01.png
void splitCompass(char *arg1, char *arg2, char *atlas){
	// Path of input texture atlas
	char tempPath1[] = "/textures/items/";
	unsigned short newLen = strlen(arg1)+strlen(tempPath1)+strlen(atlas);
	char *innPath = calloc(newLen+1, sizeof(char));
	strcpy(innPath, arg1);
	strcat(innPath, tempPath1);
	strcat(innPath, atlas);

	// Path of output textures
	char tempPath2[] = "/assets/minecraft/textures/item/";
	newLen = strlen(arg2)+strlen(tempPath2)+strlen(atlas);
	char *outPath = calloc(newLen+1, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, tempPath2);
	// strtok() modifies so make a copy
	char *tempSTR = calloc(strlen(atlas)+1, sizeof(char));
	strcpy(tempSTR, atlas);
	char *token = strtok(tempSTR, "_");
	strcat(outPath, token);
	free(tempSTR); // No longer needed
	strcat(outPath, "_");

	int w, h, ch;
	unsigned char *img = stbi_load(innPath, &w, &h, &ch, 0);
	if (img == NULL){
		fprintf(stderr, "Could not find '%s' file\n", atlas);
		return;
	}
	free(innPath); // Atlas loaded
	const int regionSize = w*w*ch; // Size of individual texture
	unsigned char *outIMG = calloc(regionSize, sizeof(unsigned char));
	for (int i=0; i<floor(h/w); i++){
		// Store copy of path to edit
		char *tempSTR = calloc(strlen(outPath)+7, sizeof(char));
		strcpy(tempSTR, outPath);

		memcpy(outIMG, img+(regionSize*i), regionSize); // Copy region of atlas
		if (i<10){ // Single digit so add 0 before it
			strcat(tempSTR, "0");
		}
		char *intSTR = intToSTR(i);
		strcat(tempSTR, intSTR);
		strcat(tempSTR, ".png");
		if (stbi_write_png(tempSTR, w, w, ch, outIMG, w*ch) == 0){
			fprintf(stderr, "Unable to write to file: %s\n", outPath);
		}
		// Free up resources
		free(tempSTR);
		free(intSTR);
	}
	// Free up resources
	stbi_image_free(img);
	free(outIMG);
	free(outPath);
}
