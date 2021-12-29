#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fixChests.h"
#include "helper.h"

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

void fixDoubleChests(char *arg1, char *arg2, char *chest){
	// Path of input texture
	char *innPath = calloc(strlen(chest)+24, sizeof(char));
	strcpy(innPath, "/textures/entity/chest/");
	strcat(innPath, chest);

	// Load chest texture
	int w, h, ch;
	unsigned char *img = getImageARG(arg1, innPath, &w, &h, &ch);
	if (img == NULL){
		reportMissing(arg1, innPath);
		free(innPath); // Failed to load so not needed
		return;
	}

	free(innPath); // Loaded texture so no longer needed
	const int q = floor(h/64); // Resize scale

	// Determine if normal or trapped chest
	char *tempSTR = calloc(strlen(chest)+1, sizeof(char));
	strcpy(tempSTR, chest); // Store copy since strtok() modifies
	char *chestType = strtok(tempSTR, "_");
	// If not trapped chest then it's normal
	if (strcmp(chestType, "trapped")){
		strcpy(chestType, "normal");
	}

	// Path of output texture
	unsigned short newLen = strlen(arg2)+strlen(chestType);
	char *outPath = calloc(newLen+41, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/assets/minecraft/textures/entity/chest/");
	strcat(outPath, chestType);

	// Split chest texture into left/right
	dcAuxLeft (img, chestType, outPath, q, w, h, ch);
	dcAuxRight(img, chestType, outPath, q, w, h, ch);

	// Free up resources
	free(tempSTR);
	free(outPath);
	stbi_image_free(img);
}

void dcAuxLeft(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch)
{
	int newW = floor(w/2);
	unsigned char *outIMG = calloc(newW*h*ch, sizeof(char)); // Store output image
	int qStat[4] = {0, 0, 6, 5}; // Crop parameters
	unsigned char *tempIMG = crop(img, q, qStat, w, ch); // Chest handle
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 14, qStat[1] = 29, qStat[2] = 44, qStat[3] = 5; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid front & side
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 33, qStat[1] = 29, qStat[3] = 10; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19, qStat[1] = 29, qStat[2] = 15, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest innards
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19, qStat[1] = 59; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest base
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 0, qStat[1] = 59; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid inside
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 0, qStat[1] = 29; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid top
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Done pasting so no longer needed

	qStat[0] = 0, qStat[1] = 44; // New params
	pasteRegion(NULL, outIMG, q, qStat, newW, ch); // Delete image region
	qStat[0] = 19; // Shift region down
	pasteRegion(NULL, outIMG, q, qStat, newW, ch); // Delete image region

	// Save result
	char *tempSTR = calloc(strlen(outPath)+10, sizeof(char));
	strcpy(tempSTR, outPath);
	strcat(tempSTR, "_left.png");
	if (stbi_write_png(tempSTR, newW, h, ch, outIMG, newW*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", tempSTR);
	}

	// Free up resources
	free(tempSTR);
	free(outIMG);
}

void dcAuxRight(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch)
{
	int newW = floor(w/2);
	unsigned char *outIMG = calloc(newW*h*ch, sizeof(char)); // Store output image
	int qStat[4] = {0, 0, 6, 5}; // Crop parameters
	unsigned char *tempIMG = crop(img, q, qStat, w, ch); // Chest handle
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14, qStat[2] = 15, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid top
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 29; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 44; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid inside
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 14, qStat[1] = 0, qStat[2] = 14, qStat[3] = 5; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid side
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14, qStat[2] = 15; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid front
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 43; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 73; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid back
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for final step

	qStat[0] = 19, qStat[1] = 14, qStat[2] = 15, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest innards
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 29; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19, qStat[1] = 44; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest base
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 33, qStat[1] = 0, qStat[2] = 14, qStat[3] = 10; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom side
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14, qStat[2] = 15; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom front
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 43; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for final step

	qStat[1] = 73; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom back
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Procedure complete so no longer needed

	// Save result
	char *tempSTR = calloc(strlen(outPath)+11, sizeof(char));
	strcpy(tempSTR, outPath);
	strcat(tempSTR, "_right.png");
	if (stbi_write_png(tempSTR, newW, h, ch, outIMG, newW*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", tempSTR);
	}

	// Free up resources
	free(tempSTR);
	free(outIMG);
}

void fixSingleChests(char *arg1, char *arg2, char *chest){
	// Path of input texture
	char *innPath = calloc(strlen(chest)+24, sizeof(char));
	strcpy(innPath, "/textures/entity/chest/");
	strcat(innPath, chest);

	// Load chest texture
	int w, h, ch;
	unsigned char *img = getImageARG(arg1, innPath, &w, &h, &ch);
	if (img == NULL){
		reportMissing(arg1, innPath);
		free(innPath); // Failed to load so not needed
		return;
	}

	free(innPath); // Loaded texture so no longer needed
	const int q = floor(h/64); // Resize scale

	// Path of output texture
	unsigned short newLen = strlen(arg2)+strlen(chest);
	char *outPath = calloc(newLen+41, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/assets/minecraft/textures/entity/chest/");
	strcat(outPath, chest);

	// Actually fix chest
	chestAux(img, outPath, q, w, h, ch);

	// Free up resources
	free(outPath);
	stbi_image_free(img);
}

void chestAux(unsigned char *img, char *outPath, const int q, int w, int h, int ch){
	unsigned char *outIMG = calloc(w*h*ch, sizeof(char)); // Store output image
	int qStat[4] = {0, 0, 6, 5}; // Crop parameters
	unsigned char *tempIMG = crop(img, q, qStat, w, ch); // Chest handle
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14, qStat[2] = 14, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid top
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 28; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	tempIMG = crop(img, q, qStat, w, ch); // Chest lid inside
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest base
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 28; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	tempIMG = crop(img, q, qStat, w, ch); // Chest innards
	flipUP(tempIMG, q, qStat[2], qStat[3], ch); // Flip upside-down
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 14, qStat[1] = 0, qStat[2] = 14, qStat[3] = 5; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid side 1
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 28; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	tempIMG = crop(img, q, qStat, w, ch); // Chest lid side 2
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 0; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest lid front
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 42; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	tempIMG = crop(img, q, qStat, w, ch); // Chest lid back
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 33, qStat[1] = 0, qStat[2] = 14, qStat[3] = 10; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom side 1
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 28; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom side 2
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 0; // New params
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom front
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 42; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for final step

	tempIMG = crop(img, q, qStat, w, ch); // Chest bottom back
	rotate(tempIMG, q, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Procedure complete so no longer needed

	// Save result
	if (stbi_write_png(outPath, w, h, ch, outIMG, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath);
	}

	// Free up resources
	free(outIMG);
}
