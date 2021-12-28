#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

#include "copyTex.h"
#include "helper.h"

void copyTextures(char *arg1, char *arg2){
	FILE *missingPTR = fopen("../missing.txt", "w"); // Keep track of missing files
	FILE *csvPTR = fopen("../textures.csv", "r"); // Table of bedrock/java file names
	char buffer[160];
	if (csvPTR == NULL){
		fprintf(stderr, "Unable to grab texture CSV file\n");
		free(arg1);
		free(arg2);
		exit(1);
	}
	while (fgets(buffer, sizeof(buffer), csvPTR) != NULL){
		stripCHAR(buffer, '\n'); // Remove newline from end if present
		char *innPath = getPathCSV(arg1, buffer); // Get path of file to be copied
		char *outPath = getPathCSV(arg2, NULL); // Get path of destination to copy to

		char *fileExt = strrchr(innPath, '.'); // Check image type
		if (!strcmp(fileExt, ".tga")){
			int w, h, ch;
			unsigned char *img = stbi_load(innPath, &w, &h, &ch, 0);
			if (img == NULL){
				//fprintf(stderr, "Unable to locate file: %s\n", innPath);
				fprintf(missingPTR, "%s\n", innPath); // Note down missing texture
				free(innPath);
				free(outPath);
				continue;
			}
			else{
				free(innPath); // File has been opened
				if (stbi_write_png(outPath, w, h, ch, img, w*ch) == 0){
					fprintf(stderr, "Unable to write to file: %s\n", outPath);
				}
				// Free up resources
				stbi_image_free(img);
				free(outPath);
			}
		}
		else{ // PNG file so just copy bytes over
			FILE *innPTR = fopen(innPath, "rb"); // File to be copied
			if (innPTR == NULL){
				fprintf(missingPTR, "%s\n", innPath); // Note down missing texture
				free(innPath);
				free(outPath);
				continue;
			}
			else{
				free(innPath); // File has been opened
				FILE *outPTR = fopen(outPath, "wb"); // Destination
				if (outPTR == NULL){
					fprintf(stderr, "Unable to write to file: %s\n", outPath);
					fclose(innPTR);
					free(outPath);
					continue;
				}
				free(outPath); // File has been opened
				copyPNG(innPTR, outPTR); // Actually copy the file
			}
		}
	}
	// Free up resources
	fclose(csvPTR);
	fclose(missingPTR);
}

char *getPathCSV(char *arg, char *buf){
	char *tempSTR = strtok(buf, ","); // Store path from CSV
	unsigned short newLen = strlen(arg) + strlen(tempSTR);
	char *fPath = calloc(newLen+1, sizeof(char));
	// Prepend pack root folder
	strcpy(fPath, arg);
	strcat(fPath, tempSTR);
	return fPath;
}

void copyPNG(FILE *innPTR, FILE *outPTR){
	size_t n, m;
	unsigned char buff[1024];
	do {
		n = fread(buff, 1, sizeof(buff), innPTR);
		if (n){m = fwrite(buff, 1, n, outPTR);}
		else {m = 0;}
	} while ((n > 0) && (n == m));
	// Free up resources
	fclose(innPTR);
	fclose(outPTR);
}
