#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>

#include "parse.h"
#include "extractZIP.h"
#include "helper.h"
#include "cJSON.h"

void parseZip(char *arg1){
	char *fileExt = strrchr(arg1, '.');	// Check file extension if present
	if (fileExt == NULL){return;} // No dots in sight so probably folder
	else if (!strcmp(fileExt, ".zip") || !strcmp(fileExt, ".mcpack")){
		// Ensure containing folder
		// FIXME Hacky solution, preferably don't use chdir
		short argLen = strlen(arg1);
		short newLen = argLen-strlen(fileExt)+1; // Length without file ext
		char *folderPath = malloc(newLen*sizeof(char)); // Folder to extract to
		memcpy(folderPath, arg1, newLen*sizeof(char)); // Copy bytes except file ext
		stripCHAR(folderPath, '.'); // Ensure proper size
		safe_create_dir(folderPath); // Create containing folder

		// Determine if relative or absolute path
		char *tempPath = malloc((argLen+4)*sizeof(char));
		switch (arg1[0]){
			case '/': // Absolute path so just use that
				strcpy(tempPath, arg1);
				break;
			default: // Relative path so correct for new dir
				char dotDOT[] = "../";
				strcpy(tempPath, dotDOT);
				strcat(tempPath, arg1);
				break;
		}

		// Finally unzip archive
		chdir(folderPath); // Enter new folder
		extract(tempPath); // Unzip contents into folder
		chdir(".."); // Exit new folder

		// No longer needed strings
		free(folderPath);
		free(tempPath);

		stripCHAR(arg1, '.'); // Now working with the folder
	}
	else{ // Not a zip file
		fprintf(stderr, "Invalid file format\n");
		exit(1);
	}
}

void parseManifest(char *arg1, char *arg2){
	short argLen = strlen(arg1);
	char *manPath = malloc((strlen(arg1)+15)*sizeof(char));
	strcpy(manPath, arg1);
	strcat(manPath, "/manifest.json");

	// Check if manifest in pack root folder
	FILE *filePTR = fopen(manPath, "r");
	if(filePTR == NULL){
		DIR *dir = opendir(arg1); // Not in pack root folder so start searching
		char *entry = malloc(128*sizeof(char)); // Store name of sub-directory
		if (dir == NULL){
			fprintf(stderr, "Unable to locate manifest file\n");
			exit(1);
		}
		else{
			// Lazy search for manifest
			strcpy(entry, readdir(dir)->d_name); // Try to enter 1st thing we see
			entry = realloc(entry, strlen(entry)*sizeof(char)); // Shrink to size
			dirSPACES(entry); // Escape space from path
			closedir(dir); // No longer need to look at whole folder

			// Kinda just assume what we found was a folder
			manPath = realloc(manPath, (argLen+strlen(entry)+16));
			strcpy(manPath, arg1);
			strcat(manPath, "/");
			strcat(manPath, entry);
			strcat(manPath, "/manifest.json");

			free(entry); // No longer needed

			// Check if manifest in 'folder'
			filePTR = fopen(manPath, "r"); 
			if(filePTR == NULL){ // Weird folder structure?
				fprintf(stderr, "Unable to locate manifest file\n");
				exit(1);
			}
		}
	}
	free(manPath); // No longer needed since we have the file
	parseManAUX(filePTR, arg2); // Actually start parsing the manifest
}

void parseManAUX(FILE *filePTR, char *arg2){
	// Read contents of json file
	char *buffer = malloc(1024);
	fread(buffer, 1024, 1, filePTR);
	cJSON *json = cJSON_Parse(buffer); // Store json
	// Free resources
	fclose(filePTR);
	free(buffer);

	// Ensure json was parsed properly
	if (json == NULL){
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL)
		{
			fprintf(stderr, "Error before: %s\n", error_ptr);
		}
		fprintf(stderr, "Unable to parse manifest");
		exit(1);
	}
	// Grab pack description
	cJSON *head = cJSON_GetObjectItemCaseSensitive(json, "header");
	cJSON *desc = cJSON_GetObjectItemCaseSensitive(head, "description");
	char *descSTR = cJSON_Print(desc); // Store description as string
	if (descSTR == NULL){
		fprintf(stderr, "Manifest is invalid\n");
		exit(1);
	}
	
	// Generate `pack.mcmeta` file
	char *outSTR = malloc((strlen(descSTR)+44)*sizeof(char));
	strcpy(outSTR, "{\"pack\": {\"description\": ");
	strcat(outSTR, descSTR);
	strcat(outSTR, ",\"pack_format\": 7}}");
	
	// Free up resources
	cJSON_Delete(json);
	free(descSTR);
	
	// Write to output file
	char *outPath = malloc((strlen(arg2)+12)*sizeof(char));
	strcpy(outPath, arg2);
	strcat(outPath, "/pack.mcmeta");
	FILE *outPTR = fopen(outPath, "w");
	if (outPTR == NULL){
		fprintf(stderr, "Unable to write output file\n");
		exit(1);
	}
	fputs(outSTR, outPTR);
	
	// Free up resources
	fclose(outPTR);
	free(outPath);
	free(outSTR);
}
