#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "copyTex.h"
#include "helper.h"

void copyTextures(char *arg1, char *arg2){
	FILE *csvPTR = fopen("../textures.csv", "r"); // Table of bedrock/java file names
	char buffer[160];
	if (csvPTR == NULL){
		fprintf(stderr, "Unable to grab texture CSV file\n");
		exit(1);
	}
	while (fgets(buffer, sizeof(buffer), csvPTR) != NULL){
		stripCHAR(buffer, '\n'); // Remove newline from end if present
		char *innPath = getPathCSV(arg1, buffer); // Get path of file to be copied
		char *outPath = getPathCSV(arg2, NULL); // Get path of destination to copy to

		char *fileExt = strrchr(innPath, '.'); // Check image type
		if (!strcmp(fileExt, ".tga")){
			// TODO Convert to png (somehow)
			fprintf(stderr, "TGA support not yet implemented: %s\n", innPath);
			free(innPath);
			free(outPath);
			continue;
		}
		else{ // PNG file so just copy bytes over
			FILE *innPTR = fopen(innPath, "rb"); // File to be copied
			if (innPTR == NULL){ // TODO implement missing.txt
				fprintf(stderr, "Unable to locate file: %s\n", innPath);
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

/*
void copyAnimations(char *arg2){
	puts("yes");
}
*/