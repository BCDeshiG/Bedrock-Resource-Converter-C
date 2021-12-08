#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "fixTextures.h"
#include "helper.h"

#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#define STBI_MAX_DIMENSIONS (1<<14) // Max texture size of 16k (way overkill)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

void fixes(char *arg1, char *arg2){
	fixBeds(arg1, arg2);
	fixChests(arg1, arg2, "double_normal.png");
	fixChests(arg1, arg2, "trapped_double.png");
}

void fixBeds(char *arg1, char *arg2){
	// Path of input folder
	char *innFolder = calloc(strlen(arg1)+22, sizeof(char));
	strcpy(innFolder, arg1);
	strcat(innFolder, "/textures/entity/bed/");
	// Path of output folder
	char *outFolder = calloc(strlen(arg2)+39, sizeof(char));
	strcpy(outFolder, arg2);
	strcat(outFolder, "/assets/minecraft/textures/entity/bed/");

	// Read folder contents
	DIR *dir = opendir(innFolder); // Check for bed textures
	struct dirent *entry; // Store file name of bed
	char *bedNames[16]; // Array of file names
	unsigned short count = 0; // Track how many textures found
	if (dir == NULL){
		fprintf(stderr, "Could not load bed textures\n");
		free(innFolder); // Failed to load so not needed
		free(outFolder); // Failed to load so not needed
		return;
	}
	else{
		while ((entry=readdir(dir))){ // Check all files in folder
			if (count>=16){break;} // 16 bed textures
			char *fileName = entry->d_name; // Store name of texture
			if (fileName[0] != '.'){ // Filter out "." and ".." paths
				bedNames[count] = calloc(strlen(fileName)+1, sizeof(char));
				strcpy(bedNames[count], fileName); // Add file name to array
				count++;
			}
		}
		closedir(dir); // Finished reading
	}

	// Iterate through textures
	for (int i=0; i<count; i++){
		// Store path of input texture
		unsigned short newLen = strlen(innFolder)+strlen(bedNames[i]);
		char *innPath = calloc(newLen+1, sizeof(char));
		strcpy(innPath, innFolder);
		strcat(innPath, bedNames[i]);

		// Load input texture
		int w, h, ch;
		unsigned char *img = stbi_load(innPath, &w, &h, &ch, 0);
		if (img == NULL){
			fprintf(stderr, "Could not find '%s' file\n", bedNames[i]);
			free(bedNames[i]); // Failed to load so not needed
			free(innPath); // Failed to load so not needed
			continue;
		}

		// Store output path
		char *outPath = calloc(strlen(outFolder)+15, sizeof(char));
		strcpy(outPath, outFolder);
		// Silver bed becomes Light Gray bed
		if (!strncmp(bedNames[i], "silver", 6)){
			strtok(bedNames[i], "."); // Throw away old name
			char ext[4] = "."; // Stores file type of image
			strcat(ext, strtok(NULL, "."));
			bedNames[i] = realloc(bedNames[i], 15*sizeof(char));
			strcpy(bedNames[i], "light_gray"); // Copy new name
			strcat(bedNames[i], ext); // Append file extension
		}
		strcat(outPath, bedNames[i]);

		// Actually fix texture
		const int q = floor(h/64); // Resize scale
		fixBedsAux(img, q, w, h, ch, outPath);

		// Free up resources
		free(outPath);
		stbi_image_free(img);
		free(bedNames[i]);
		free(innPath);
	}

	// Free up resources
	free(innFolder);
	free(outFolder);
}

// TODO Bed Feet
void fixBedsAux(unsigned char *bed, const int q, int w, int h, int ch, char *outPath){
	int qStat[4] = {22, 0, 44, 28}; // Crop parameters
	unsigned char *tempIMG = crop(bed, q, qStat, w, ch); // Store cropped image
	pasteRegion(NULL, bed, q, qStat, w, ch); // Erase section of image
	qStat[0] += 6; // Shift down 6 pixels
	pasteRegion(tempIMG, bed, q, qStat, w, ch); // Paste cropped region back onto image
	// Clear intermediate for next step
	free(tempIMG);

	qStat[0] = 0, qStat[1] = 22, qStat[2] = 16, qStat[3] = 6; // New params
	tempIMG = crop(bed, q, qStat, w, ch); // Store cropped image
	pasteRegion(NULL, bed, q, qStat, w, ch); // Erase section of image
	qStat[0] += 22; // Shift down into gap
	pasteRegion(tempIMG, bed, q, qStat, w, ch); // Paste cropped region back onto image
	// Clear intermediate
	free(tempIMG);

	// Save result
	if (stbi_write_png(outPath, w, h, ch, bed, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath);
	}
}

void fixChests(char *arg1, char *arg2, char *chest){
	// Path of input texture
	char *innPath = calloc(strlen(chest)+24, sizeof(char));
	strcpy(innPath, "/textures/entity/chest/");
	strcat(innPath, chest);

	// Load chest texture
	int w, h, ch;
	unsigned char *img = getImageARG(arg1, innPath, &w, &h, &ch);
	if (img == NULL){
		fprintf(stderr, "Could not find '%s' file\n", chest);
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
	chestAuxLeft (img, chestType, outPath, q, w, h, ch);
	chestAuxRight(img, chestType, outPath, q, w, h, ch);

	// Free up resources
	free(tempSTR);
	free(outPath);
	stbi_image_free(img);
}

void chestAuxLeft(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch)
{
	int newW = floor(w/2);
	unsigned char *outIMG = calloc(newW*h*ch, sizeof(char)); // Store output image
	int qStat[4] = {0, 0, 6, 5}; // Crop parameters
	unsigned char *tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 14, qStat[1] = 29, qStat[2] = 44, qStat[3] = 5; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	rotate(tempIMG, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 33, qStat[1] = 29, qStat[3] = 10; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	rotate(tempIMG, qStat[2], qStat[3], ch); // Rotate 180 degrees
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19, qStat[1] = 29, qStat[2] = 15, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19, qStat[1] = 59; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 0, qStat[1] = 59; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 0, qStat[1] = 29; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
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

void chestAuxRight(unsigned char *img, char *chestType, char *outPath,
	const int q, int w, int h, int ch)
{
	int newW = floor(w/2);
	unsigned char *outIMG = calloc(newW*h*ch, sizeof(char)); // Store output image
	int qStat[4] = {0, 0, 29, 43}; // Crop parameters
	unsigned char *tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14, qStat[2] = 15, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 29; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19, qStat[1] = 14, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 29; // Shift region right
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 0, qStat[1] = 44, qStat[2] = 15, qStat[3] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 19, qStat[1] = 44; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 14; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 14, qStat[1] = 73, qStat[2] = 15, qStat[3] = 5; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 43; // Shift region left
	pasteRegion(tempIMG, outIMG, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for final step

	qStat[0] = 33, qStat[1] = 73, qStat[3] = 10; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Store cropped region
	qStat[1] = 43; // Shift region left
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
