#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "convert.h"
#include "helper.h"
#include "parse.h"

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
		arg1 = malloc(strlen(argv[1]));
		arg2 = malloc(strlen(argv[2]));
		strcpy(arg1, argv[1]);
		strcpy(arg2, argv[2]);
	}
	// Inputs have been processed now so start converting
	startConversion();
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
	parseZip(arg1);
	parseManifest(arg1, arg2);
}
