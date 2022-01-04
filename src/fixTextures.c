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
	fixHoglins(arg1, arg2, "hoglin.png");
	fixHoglins(arg1, arg2, "zoglin.png");
	fixFoxes(arg1, arg2, "fox.png");
	fixFoxes(arg1, arg2, "arctic_fox.png");
	fixDog(arg1, arg2);
	fixCat(arg1, arg2);
	fixAzalea(arg1, arg2, "potted_azalea_bush_side.png");
	fixAzalea(arg1, arg2, "potted_flowering_azalea_bush_side.png");
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
			reportMissing(arg1, innPath);
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
		reportMissing(arg1, innPath);
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
		reportMissing(arg1, innPath);
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
		reportMissing(arg1, innPath);
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

void fixHoglins(char *arg1, char *arg2, char *hog){
	// Path of input texture
	char *innPath = calloc(strlen(hog)+35, sizeof(char));
	strcpy(innPath, "/textures/entity/");
	strncat(innPath, hog, 6); // Exclude file extension
	strcat(innPath, "/"); // This is a folder
	strcat(innPath, hog); // The filename itself

	// Load texture
	int w, h, ch;
	unsigned char *hoglin = getImageARG(arg1, innPath, &w, &h, &ch);
	if (hoglin == NULL){
		reportMissing(arg1, innPath);
		free(innPath); // Failed to load so not needed
		return;
	}
	free(innPath); // Texture has been loaded

	const int q = floor(w/128); // Resize scale
	int qStat[4] = {13, 1, 8, 13}; // Crop parameters
	unsigned char *tusk = crop(hoglin, q, qStat, w, ch);
	qStat[1] = 10; // Shift down
	pasteRegion(tusk, hoglin, q, qStat, w, ch);

	// Path of output texture
	short newLen = strlen(arg2)+strlen(hog);
	char *outPath = calloc(newLen+42, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/assets/minecraft/textures/entity/hoglin/");
	strcat(outPath, hog);

	// Save result
	if (stbi_write_png(outPath, w, h, ch, hoglin, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath);
	}
	// Free up resources
	stbi_image_free(hoglin);
	free(tusk);
	free(outPath);
}

void fixFoxes(char *arg1, char *arg2, char *fox){
	// Path of input texture
	char *innPath = calloc(strlen(fox)+22, sizeof(char));
	strcpy(innPath, "/textures/entity/fox/");
	strcat(innPath, fox);

	// Load texture
	int w, h, ch;
	unsigned char *img = getImageARG(arg1, innPath, &w, &h, &ch);
	if (img == NULL){
		reportMissing(arg1, innPath);
		free(innPath); // Failed to load so not needed
		return;
	}
	free(innPath); // Texture has been loaded

	// Compose awake texture
	const int q = floor(w/64); // Resize scale
	const int newW = 48*q; // Scaled width of new texture
	const int newH = 32*q; // Scaled height of new texture
	const int texSize = newW*newH*ch; // Size of texture in bytes
	unsigned char *woke = calloc(texSize, sizeof(char)); // Awake texture
	int qStat[4] = {0, 28, 18, 14}; // Crop parameters
	unsigned char *tempIMG = crop(img, q, qStat, w, ch); // Tail
	qStat[1] = 30; // Shift right
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 15, qStat[1] = 30, qStat[2] = 24, qStat[3] = 17; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Body
	qStat[1] = 24; // Shift left
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 24, qStat[1] = 22, qStat[2] = 8, qStat[3] = 8; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Foot
	qStat[1] = 13; // Shift left
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[1] = 14; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Other Foot
	qStat[1] = 4; // Shift left
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 24, qStat[1] = 0, qStat[2] = 14, qStat[3] = 5; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Nose
	qStat[0] = 18, qStat[1] = 6; // Shift down + right
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 0, qStat[1] = 0, qStat[2] = 6, qStat[3] = 3; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Ear
	qStat[0] = 1, qStat[1] = 8; // Shift down + right
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 0, qStat[1] = 22; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Other Ear
	qStat[0] = 1, qStat[1] = 15; // Shift down + left
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	// Copy to asleep texture
	unsigned char *slep = calloc(texSize, sizeof(char));
	memcpy(slep, woke, texSize);

	qStat[0] = 0, qStat[1] = 0, qStat[2] = 28, qStat[3] = 12; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Awake Head
	qStat[0] = 5, qStat[1] = 1; // Shift down + right
	pasteRegion(tempIMG, woke, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Ready for next step

	qStat[0] = 12, qStat[1] = 0; // New params
	tempIMG = crop(img, q, qStat, w, ch); // Asleep Head
	qStat[0] = 5, qStat[1] = 1; // Shift up + right
	pasteRegion(tempIMG, slep, q, qStat, newW, ch); // Paste cropped region onto image
	free(tempIMG); // Done pasting

	// Fix extra ears from pasting head
	qStat[0] = 5, qStat[1] = 1, qStat[2] = 6, qStat[3] = 3; // New params
	pasteRegion(NULL, woke, q, qStat, newW, ch); // Clear region
	pasteRegion(NULL, slep, q, qStat, newW, ch); // Clear region
	qStat[1] = 23; // New params
	pasteRegion(NULL, woke, q, qStat, newW, ch); // Clear region
	pasteRegion(NULL, slep, q, qStat, newW, ch); // Clear region

	// arctic_fox becomes snow_fox
	if (!strncmp(fox, "arctic", 6)){
		fox = "snow_fox.png";
	}

	// Paths of output textures
	unsigned short newLen = strlen(arg2)+strlen(fox);
	char *outPath1 = calloc(newLen+39, sizeof(char));
	char *outPath2 = calloc(newLen+45, sizeof(char));
	strcpy(outPath1, arg2);
	strcat(outPath1, "/assets/minecraft/textures/entity/fox/");
	strncat(outPath1, fox, strlen(fox)-4);
	strcpy(outPath2, outPath1);
	strcat(outPath1, ".png");
	strcat(outPath2, "_sleep.png");

	// Save results
	if (stbi_write_png(outPath1, newW, newH, ch, woke, newW*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath1);
	}
	if (stbi_write_png(outPath2, newW, newH, ch, slep, newW*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath2);
	}

	// Free up resources
	stbi_image_free(img);
	free(outPath1);
	free(outPath2);
	free(woke);
	free(slep);
}

void fixDog(char *arg1, char *arg2){
	// Load texture
	int w, h, ch;
	char *innPath = "/textures/entity/wolf/wolf_tame.tga";
	unsigned char *img = getImageARG(arg1, innPath, &w, &h, &ch);
	if (img == NULL){
		reportMissing(arg1, innPath);
		return;
	}
	unsigned char *tame = makeOpaque(img, w, h);

	// Path of output texture
	char *outPath = calloc(strlen(arg2)+53, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/assets/minecraft/textures/entity/wolf/wolf_tame.png");

	// Save results
	if (stbi_write_png(outPath, w, h, ch, tame, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath);
	}

	// Free up resources
	stbi_image_free(img);
	free(outPath);
	free(tame);
}

void fixCat(char *arg1, char *arg2){
	// Path of input folder
	char *innPath = calloc(strlen(arg1)+22, sizeof(char));
	strcpy(innPath, arg1);
	strcat(innPath, "/textures/entity/cat/");

	// Read folder contents
	DIR *dir = opendir(innPath); // Check for cat textures
	struct dirent *entry; // Store file name of cat
	if (dir == NULL){
		fprintf(stderr, "Could not load cat textures\n");
		free(innPath); // Failed to load so not needed
		return;
	}
	else{
		while ((entry=readdir(dir))){ // Check all files in folder
			char *fileName = entry->d_name; // Name of texture within folder
			char *tempSTR = strstr(fileName, "tame");
			if (tempSTR != NULL){ // Check if file name contains 'tame'
				// Found a texture we can use for the collar
				unsigned short newLen = strlen(innPath)+strlen(fileName);
				innPath = realloc(innPath, (newLen+1)*sizeof(char));
				strcat(innPath, fileName);
				break;
			}
		}
		closedir(dir); // Finished reading
	}

	// Load texture
	int w, h, ch;
	unsigned char *img = getImageARG("", innPath, &w, &h, &ch);
	if (img == NULL){
		reportMissing(arg1, innPath);
		free(innPath); // Failed to load so not needed
		return;
	}
	free(innPath); // Texture has been loaded

	// Path of output texture
	char *outPath = calloc(strlen(arg2)+53, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/assets/minecraft/textures/entity/cat/cat_collar.png");

	// Save results
	if (stbi_write_png(outPath, w, h, ch, img, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath);
	}

	// Free up resources
	stbi_image_free(img);
	free(outPath);
}

void fixAzalea(char *arg1, char *arg2, char *bush){
	// Path of input texture
	char *innPath = calloc(strlen(bush)+18, sizeof(char));
	strcpy(innPath, "/textures/blocks/");
	strcat(innPath, bush);

	// Load texture
	int w, h, ch;
	unsigned char *img = getImageARG(arg1, innPath, &w, &h, &ch);
	if (img == NULL){
		reportMissing(arg1, innPath);
		free(innPath); // Failed to load so not needed
		return;
	}
	free(innPath); // Texture has been loaded

	// Shift image down 1 row
	const int regionSize = w*h*ch; // Size of image in bytes
	const int rowSize = w*ch; // Size of row in bytes
	unsigned char *outIMG = calloc(regionSize, sizeof(char));
	int count = 0; // Number of bytes read
	while (count+rowSize < regionSize){
		outIMG[count+rowSize] = img[count];
		count++;
	}
	stbi_image_free(img); // Done reading image

	// Path of output texture
	short newLen = strlen(arg2)+strlen(bush);
	char *outPath = calloc(newLen+34, sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/assets/minecraft/textures/block/");
	strcat(outPath, bush);

	// Save results
	if (stbi_write_png(outPath, w, h, ch, outIMG, w*ch) == 0){
		fprintf(stderr, "Could not save to '%s'\n", outPath);
	}

	// Free up resources
	free(outIMG);
	free(outPath);
}
