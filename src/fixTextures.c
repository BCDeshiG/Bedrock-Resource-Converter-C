#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "fixTextures.h"
#include "fixChests.h"
#include "helper.h"

#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#define STBI_MAX_DIMENSIONS (1<<13) // Max texture size of 8k (way overkill)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

void fixes(char *arg1, char *arg2){
	fixBeds(arg1, arg2);
	fixDoubleChests(arg1, arg2, "double_normal.png");
	fixDoubleChests(arg1, arg2, "trapped_double.png");
	fixSingleChests(arg1, arg2, "ender.png");
	fixSingleChests(arg1, arg2, "normal.png");
	fixSingleChests(arg1, arg2, "trapped.png");
	fixZombies(arg1, arg2, "zombie.png");
	fixZombies(arg1, arg2, "husk.png");
	fixDrowned(arg1, arg2);
	fixSheep(arg1, arg2);
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
			// Store path of input texture
			char *fileName = entry->d_name; // Name of texture within folder
			unsigned short newLen = strlen(innFolder)+strlen(fileName);
			char *tempPath = calloc(newLen+1, sizeof(char)); // Used to validate the image
			strcpy(tempPath, innFolder);
			strcat(tempPath, fileName);
			// Filter out invalid texture paths
			if (stbi_info(tempPath, NULL, NULL, NULL)){
				bedNames[count] = calloc(strlen(fileName)+1, sizeof(char));
				strcpy(bedNames[count], fileName); // Add file name to array
				count++;
			}
			free(tempPath); // Image has been checked
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

void fixZombies(char *arg1, char *arg2, char *zombie){
	// Path of input texture
	char *innPath = calloc(strlen(zombie)+25, sizeof(char));
	strcpy(innPath, "/textures/entity/zombie/");
	strcat(innPath, zombie);

	// Load texture
	int w, h, ch;
	unsigned char *img = getImageARG(arg1, innPath, &w, &h, &ch);
	if (img == NULL){
		fprintf(stderr, "Could not find '%s' file\n", zombie);
		free(innPath); // Failed to load so not needed
		return;
	}
	free(innPath); // Texture has been loaded

	const int newSize = w*h*ch*2; // Double height
	int qStat[4] = {0, 0, w, h}; // Paste in place
	unsigned char *outIMG = calloc(newSize, sizeof(char));
	pasteRegion(img, outIMG, 1, qStat, w, ch);

	// Path of output texture
	short newLen = strlen(arg2)+strlen(zombie);
	char *outPath = calloc(newLen+42, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/assets/minecraft/textures/entity/zombie/");
	strcat(outPath, zombie);

	// Save result
	if (stbi_write_png(outPath, w, h*2, ch, outIMG, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath);
	}
	// Free up resources
	stbi_image_free(img);
	free(outIMG);
	free(outPath);
}

void fixDrowned(char *arg1, char *arg2){
	// Load texture
	int w, h, ch;
	char *innPath = "/textures/entity/zombie/drowned.tga";
	unsigned char *drowned = getImageARG(arg1, innPath, &w, &h, &ch);
	if (drowned == NULL){
		fprintf(stderr, "Could not find 'drowned.tga' file\n");
		return;
	}

	// Paths of output textures
	char *outPath1 = calloc(strlen(arg2)+53, sizeof(char));
	char *outPath2 = calloc(strlen(arg2)+65, sizeof(char));
	strcpy(outPath1, arg2);
	strcat(outPath1, "/assets/minecraft/textures/entity/zombie/");
	strcpy(outPath2, outPath1);
	strcat(outPath1, "drowned.png");
	strcat(outPath2, "drowned_outer_layer.png");

	// Save inner layer as png
	if (stbi_write_png(outPath1, w, h, ch, drowned, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath1);
	}
	free(outPath1); // Inner texture has been saved (probably)

	// Construct outer layer
	const int q = floor(w/64); // Resize scale
	const int texSize = w*h*ch; // Size of texture in bytes
	unsigned char *outer = calloc(texSize, sizeof(char)); // Outer layer

	int qStat[4] = {0, 32, 32, 16}; // Crop parameters
	unsigned char *tempIMG = crop(drowned, q, qStat, w, ch); // Drowned head
	qStat[1] = 0; // Shift region left
	pasteRegion(tempIMG, outer, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 32, qStat[1] = 0, qStat[2] = 56, qStat[3] = 16; // New params
	tempIMG = crop(drowned, q, qStat, w, ch); // Drowned upper body
	qStat[0] = 16; // Shift region up
	pasteRegion(tempIMG, outer, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 48, qStat[2] = 16; // New params
	tempIMG = crop(drowned, q, qStat, w, ch); // Drowned lower left
	qStat[1] = 16; // Shift region right
	pasteRegion(tempIMG, outer, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for final step

	qStat[0] = 48, qStat[1] = 48; // New params
	tempIMG = crop(drowned, q, qStat, w, ch); // Drowned lower right
	qStat[1] = 32; // Shift region left
	pasteRegion(tempIMG, outer, q, qStat, w, ch); // Paste cropped region onto image
	free(tempIMG); // Procedure complete so no longer needed

	// Save outer layer
	if (stbi_write_png(outPath2, w, h, ch, outer, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath2);
	}
	// Free up resources
	stbi_image_free(drowned);
	free(outer);
	free(outPath2);
}

void fixSheep(char *arg1, char *arg2){
	// Load texture
	int w, h, ch;
	char *innPath = "/textures/entity/sheep/sheep.tga";
	unsigned char *sheep = getImageARG(arg1, innPath, &w, &h, &ch);
	if (sheep == NULL){
		fprintf(stderr, "Could not find 'sheep.tga' file\n");
		return;
	}

	// Paths of output textures
	char *outPath1 = calloc(strlen(arg2)+50, sizeof(char));
	char *outPath2 = calloc(strlen(arg2)+54, sizeof(char));
	strcpy(outPath1, arg2);
	strcat(outPath1, "/assets/minecraft/textures/entity/sheep/");
	strcpy(outPath2, outPath1);
	strcat(outPath1, "sheep.png");
	strcat(outPath2, "sheep_fur.png");

	// Base sheep layer
	const int q = floor(w/64); // Resize scale
	unsigned char *base = makeOpaque(sheep, w, floor(h/2));
	if (stbi_write_png(outPath1, w, floor(h/2), ch, base, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath1);
	}
	free(outPath1); // Inner texture has been saved (probably)

	// Wool layer
	int qStat[4] = {32, 0, 64, 32}; // Crop parameters
	unsigned char *wool = crop(sheep, q, qStat, w, ch);
	if (stbi_write_png(outPath2, w, qStat[3]*q, ch, wool, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath2);
	}
	// Free up resources
	stbi_image_free(sheep);
	free(outPath2);
	free(base);
	free(wool);
}
