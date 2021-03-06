#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "split.h"
#include "helper.h"

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

// compass_atlas.png -> compass_01.png compass_02.png et cetera
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

	// Load texture atlas
	int w, h, ch;
	unsigned char *img = stbi_load(innPath, &w, &h, &ch, 0);
	if (img == NULL){
		reportMissing("", innPath);
		free(innPath); // Failed to load so not needed
		free(outPath); // Failed to load so not needed
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

void splitPaintings(char *arg1, char *arg2){
	// Path of big painting
	char kzPath[] = "/textures/painting/kz.png";
	unsigned short newLen = strlen(arg1)+strlen(kzPath);
	char *innPath = calloc(newLen+1, sizeof(char));
	strcpy(innPath, arg1);
	strcat(innPath, kzPath);

	// Paths of output textures (split by size)
	char tempPath[] = "/assets/minecraft/textures/painting/";
	char *x16P[] = {
		"kebab.png", "aztec.png", "alban.png", "aztec2.png",
		"bomb.png", "plant.png", "wasteland.png"
	};
	char *xWide[] = {
		"pool.png", "courbet.png", "sea.png",
		"sunset.png", "creebet.png"
	};
	char *xTall[] = {"wanderer.png", "graham.png"};
	char *x32P[] = {
		"match.png", "bust.png", "stage.png",
		"void.png", "skull_and_roses.png", "wither.png"
	};
	char *x64P[] = {"pointer.png", "pigscene.png", "burning_skull.png"};

	newLen = strlen(arg2)+strlen(tempPath);
	char *outPath = calloc(newLen+1, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, tempPath);

	// Load big painting
	int w, h, ch;
	unsigned char *kz = stbi_load(innPath, &w, &h, &ch, 0);
	if (kz == NULL){
		reportMissing("", innPath);
		free(innPath); // Failed to load so not needed
		free(outPath); // Failed to load so not needed
		return;
	}
	free(innPath); // Atlas loaded
	const int q = floor(h/16); // Quantum cell size

	// Crop paintings
	paintingAux1(kz, x16P,  7, q, 0,  1, 1, ch, outPath);
	paintingAux1(kz, xWide, 5, q, 2,  2, 1, ch, outPath);
	paintingAux1(kz, xTall, 2, q, 4,  1, 2, ch, outPath);
	paintingAux1(kz, x32P,  6, q, 8,  2, 2, ch, outPath);
	paintingAux1(kz, x64P,  3, q, 12, 4, 4, ch, outPath);
	paintingAux2(kz, "fighters.png",    q, 6, 0,  4, 2, ch, outPath);
	paintingAux2(kz, "skeleton.png",    q, 4, 12, 4, 3, ch, outPath);
	paintingAux2(kz, "donkey_kong.png", q, 7, 12, 4, 3, ch, outPath);

	stbi_image_free(kz);
	free(outPath);
}

void paintingAux1(unsigned char *img, char *arr[], const int arrLen,
	const int q, int qTop, int qw, int qh, int ch, char *outPath)
{
	for (int i=0; i<arrLen; i++){
		int qStat[4] = {qTop, i*qw, qw, qh}; // (Quantum) top left width height
		unsigned char *outIMG = crop(img, q, qStat, 16*q, ch); // Store cropped image
		char *tempSTR = calloc(strlen(outPath)+20, sizeof(char)); // Store path
		strcpy(tempSTR, outPath);
		strcat(tempSTR, arr[i]);
		if (stbi_write_png(tempSTR, q*qw, q*qh, ch, outIMG, q*qw*ch) == 0){
			fprintf(stderr, "Unable to write to file\n");
		}
		// Free up resources
		free(outIMG);
		free(tempSTR);
	}
}

void paintingAux2(unsigned char *img, char *name, const int q,
	int qTop, int qLeft, int qw, int qh, int ch, char *outPath)
{
	int qStat[] = {qTop, qLeft, qw, qh};
	unsigned char *outIMG = crop(img, q, qStat, 16*q, ch); // Store cropped image
	int newLen = strlen(outPath)+strlen(name);
	char *tempSTR = calloc(newLen+1, sizeof(char)); // Store path of output image
	strcpy(tempSTR, outPath);
	strcat(tempSTR, name);
	if (stbi_write_png(tempSTR, q*qw, q*qh, ch, outIMG, q*qw*ch) == 0){
		fprintf(stderr, "Unable to write to file\n");
	}
	// Free up resources
	free(outIMG);
	free(tempSTR);
}
