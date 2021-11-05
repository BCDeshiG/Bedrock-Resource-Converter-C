#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#include "parse.h"
#include "extractZIP.h"
#include "helper.h"

void parseZip(char *arg1){
	char *fileExt = strrchr(arg1, '.');	// Check file extension if present
	if (fileExt == NULL){return;} // No dots in sight so probably folder
	else if (!strcmp(fileExt, ".zip") || !strcmp(fileExt, ".mcpack")){
		// Ensure containing folder
		// FIXME Hacky solution, preferably don't use chdir
		short newLen = strlen(arg1)-strlen(fileExt)+1; // Length without file ext
		char *folderPath = malloc(newLen*sizeof(char)); // Folder to extract to
		memcpy(folderPath, arg1, newLen*sizeof(char)); // Copy bytes except file ext
		stripCHAR(folderPath, "."); // Ensure proper size
		safe_create_dir(folderPath); // Create containing folder

		// Zip archive isn't in new folder
		char *zipName = basename(arg1); // Store name of zip archive
		char *tempPath = malloc((strlen(zipName)+4)*sizeof(char));
		char dotDOT[] = "../";
		strcpy(tempPath, dotDOT);
		strcat(tempPath, zipName);

		// Finally unzip archive
		chdir(folderPath); // Enter new folder
		extract(tempPath); // Unzip contents into folder
		chdir(".."); // Exit new folder

		// No longer needed strings
		free(folderPath);
		free(zipName);
		free(tempPath);

		stripCHAR(arg1, "."); // Now working with the folder
	}
	else{return;} // Not a zip file
}

/*
void parseManifest(){
	//open file then grab description from json
	char manPath[];
	FILE *filePTR = fopen(, "r");
	if(filePTR==NULL){
        fprintf(stderr, "Where the hell?\n");
        exit(1);
    }
}*/
