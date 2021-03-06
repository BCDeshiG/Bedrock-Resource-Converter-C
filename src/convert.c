#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "convert.h"
#include "copyTex.h"
#include "fixTextures.h"
#include "helper.h"
#include "parse.h"
#include "split.h"

int main(int argc, char *argv[]){
	if (argc == 1){ // No arguments supplied
		arg1 = initArg(arg1, "Enter location of bedrock texture pack:");
		arg2 = initArg(arg2, "Enter location of destination folder:");
	}
	else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help") || argc != 3){
		printf("\nUsage: %s [-h] PACK DEST\n", argv[0]);
		printf("Convert MC Bedrock pack to Java\n");
		printf("\nArguments:\n");
		printf("PACK         Location of Texture folder/zip/mcpack file\n");
		printf("DEST         Destination folder of converted pack\n");
		printf("-h, --help   Displays this message (ofc)\n");
		printf("\nExample: %s foo.mcpack 'C:/oofPack'\n", argv[0]);
		exit(1);
	}
	else{
		arg1 = calloc(strlen(argv[1])+1, sizeof(char));
		arg2 = calloc(strlen(argv[2])+1, sizeof(char));
		strcpy(arg1, argv[1]);
		strcpy(arg2, argv[2]);
	}
	// Inputs have been processed now so start converting
	startConversion();
	// Finished
	free(arg1);
	free(arg2);
}

char *initArg(char *arg, char msg[]){
	arg = calloc(MAX_PATH_LENGTH, sizeof(char)); // Allocate memory
	puts(msg); // Text prompt
	fgets(arg, MAX_PATH_LENGTH, stdin); // Get input
	arg = realloc(arg, (strlen(arg)+1)*sizeof(char)); // Shrink
	stripCHAR(arg, '\n'); // Remove newline from end if present
	return arg;
}

void startConversion(){
	safe_create_dir(arg2);
	genFolders(arg2);
	parseZip(arg1);
	arg1 = parseManifest(arg1, arg2);
	parseEndText(arg1, arg2, "end.txt");
	parseEndText(arg1, arg2, "quote.txt");
	parseEndText(arg1, arg2, "credits.json");
	parseSplashes(arg1, arg2);
	copyTextures(arg1, arg2);
	splitCompass(arg1, arg2, "watch_atlas.png");
	splitCompass(arg1, arg2, "compass_atlas.png");
	splitPaintings(arg1, arg2);
	fixes(arg1, arg2);
	wrapUp();
}

void genFolders(char *arg2){
	FILE *filePTR = fopen("../paths.txt", "r");
	char *buffer = calloc(MAX_PATH_LENGTH, sizeof(char));
	if (filePTR == NULL){
		fprintf(stderr, "Unable to grab file paths\n");
		exit(1);
	}
	while (fgets(buffer, MAX_PATH_LENGTH, filePTR) != NULL){
		stripCHAR(buffer, '\n'); // Remove newline from end if present
		unsigned short newLen = strlen(arg2)+strlen(buffer);
		char *tempPath = calloc(newLen+1, sizeof(char)); // Store new folder path
		strcpy(tempPath, arg2);
		strcat(tempPath, buffer);

		safe_create_dir(tempPath); // Create new folder
		free(tempPath); // This folder is done, on to next
	}
	fclose(filePTR);
	free(buffer);
}

void wrapUp(){
	puts("\nConversion Complete");
	puts("Please see 'fixme.txt' for textures that need attention");
	FILE *checkPTR = fopen("missing.txt", "r");
	if (checkPTR != NULL){ // Check if missing.txt exists
		fclose(checkPTR); // No need to read it
		puts("\nSome files were missing from your texture pack and were not included.");
		puts("Please see the generated 'missing.txt' file for details.");
	}
}
