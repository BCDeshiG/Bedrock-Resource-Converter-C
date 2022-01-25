#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>

#include "parse.h"
#include "extractArchive.h"
#include "helper.h"
#include "cJSON.h"

void parseZip(char *arg1){
	char *fileExt = strrchr(arg1, '.');	// Check file extension if present
	if (fileExt == NULL){return;} // No dots in sight so probably folder
	else if (fileExt[1] == '.' || fileExt[1] == '/'){return;} // Relative folder path
	else if (!strcmp(fileExt, ".zip") || !strcmp(fileExt, ".mcpack")){
		// Ensure containing folder
		// FIXME Hacky solution, preferably don't use chdir
		short argLen = strlen(arg1);
		short newLen = argLen-strlen(fileExt); // Length without file ext
		char *folderPath = calloc(newLen+1, sizeof(char)); // Folder to extract to
		memcpy(folderPath, arg1, newLen*sizeof(char)); // Copy bytes except file ext
		safe_create_dir(folderPath); // Create containing folder

		// Determine if relative or absolute path
		char *tempPath = calloc(argLen+5, sizeof(char));
		if (arg1[0] == '/'){
			strcpy(tempPath, arg1); // Absolute path so just use that
		}
		else{ // Relative path so correct for new dir
			char dotDOT[] = "../";
			strcpy(tempPath, dotDOT);
			strcat(tempPath, arg1);
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

char *parseManifest(char *arg1, char *arg2){
	short argLen = strlen(arg1);
	char *manPath = calloc(argLen+15, sizeof(char));
	char *rootPath = NULL; // Stores path of folder containing manifest
	strcpy(manPath, arg1);
	strcat(manPath, "/manifest.json");

	// Check if manifest in pack root folder
	FILE *filePTR = fopen(manPath, "r");
	if(filePTR == NULL){
		DIR *dir = opendir(arg1); // Not in pack root folder so start searching
		char *entry = calloc(128, sizeof(char)); // Store name of sub-directory
		if (dir == NULL){
			fprintf(stderr, "Unable to open folder '%s'\n", arg1);
			free(manPath); // Failed to load so not needed
			free(entry); // Failed to load so not needed
			exit(1);
		}
		else{
			// Lazy search for manifest
			strcpy(entry, readdir(dir)->d_name); // Try to enter 1st thing we see
			entry = realloc(entry, (strlen(entry)+1)*sizeof(char)); // Shrink to size
			dirSPACES(entry); // Escape space from path
			closedir(dir); // No longer need to look at whole folder

			// Kinda just assume what we found was a folder
			manPath = realloc(manPath, argLen+strlen(entry)+16);
			strcpy(manPath, arg1);
			free(arg1); // Gonna replace this in a sec
			strcat(manPath, "/");
			strcat(manPath, entry);
			free(entry); // No longer needed

			// Remember folder path
			rootPath = calloc(strlen(manPath)+1, sizeof(char));
			strcpy(rootPath, manPath);
			strcat(manPath, "/manifest.json");

			// Check if manifest in 'folder'
			filePTR = fopen(manPath, "r"); 
			if(filePTR == NULL){ // Weird folder structure?
				fprintf(stderr, "Unable to locate manifest\n");
				free(manPath); // Failed to load so not needed
				free(rootPath); // Failed to load so not needed
				exit(1);
			}
		}
	}
	free(manPath); // No longer needed since we have the file
	parseManAUX(filePTR, arg2); // Actually start parsing the manifest
	if (rootPath != NULL){
		return rootPath; // Manifest was in sub-folder
	}
	else{
		return arg1; // Manifest was found immediately
	}
}

void parseManAUX(FILE *filePTR, char *arg2){
	// Read contents of json file
	char *buffer = calloc(1024, sizeof(char));
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
	
	// Generate 'pack.mcmeta' file
	char *outSTR = calloc(strlen(descSTR)+45, sizeof(char));
	strcpy(outSTR, "{\"pack\": {\"description\": ");
	strcat(outSTR, descSTR);
	strcat(outSTR, ",\"pack_format\": 8}}");
	
	// Free up resources
	cJSON_Delete(json);
	free(descSTR);
	
	// Write to output file
	FILE *outPTR = getFileARG(arg2, "/pack.mcmeta", "w");
	if (outPTR == NULL){
		fprintf(stderr, "Unable to write output file\n");
		exit(1);
	}
	fputs(outSTR, outPTR);
	
	// Free up resources
	fclose(outPTR);
	free(outSTR);
}

void parseEndText(char *arg1, char *arg2, char *text){
	// Load input text file
	char *innPath = calloc(strlen(text)+10, sizeof(char));
	strcpy(innPath, "/credits/");
	strcat(innPath, text);
	FILE *filePTR = getFileARG(arg1, innPath, "r");

	// Check if file exists
	if (filePTR == NULL){
		// Couldn't find it so skip
		fprintf(stderr, "Could not find '%s' file\n", text);
		free(innPath); // Not needed
	}
	else {
		free(innPath); // File has been loaded
		// "quote.txt" becomes "postcredits.txt"
		short newLen = strlen(text)+25;
		if (!strcmp(text, "quote.txt")){
			newLen += 6;
			text = "postcredits.txt";
		}
		char *outPath = calloc(newLen, sizeof(char));
		strcpy(outPath, "/assets/minecraft/texts/");
		strcat(outPath, text);

		// Copy contents of file
		char *buffer = calloc(1024, sizeof(char));
		FILE *outPTR = getFileARG(arg2, outPath, "w");
		if (outPTR != NULL){
			while (fgets(buffer, 1024, filePTR) != NULL){
				fputs(buffer, outPTR);
			}
			fclose(outPTR);
		}
		else{
			fprintf(stderr, "Unable to copy '%s' file\n", text);
		}

		// Free up resources
		fclose(filePTR);
		free(outPath);
		free(buffer);
	}
}

void parseSplashes(char *arg1, char *arg2){
	FILE *filePTR = getFileARG(arg1, "/splashes.json", "r");

	// Check if file exists
	if (filePTR == NULL){
		// Couldn't find it so skip
		fprintf(stderr, "Could not find 'splashes.json' file\n");
	}
	else { // Read contents of json file
		char *buffer = calloc(128, sizeof(char));
		char *tempSTR = calloc(128, sizeof(char));

		// Copy buffer into temporary string
		while (fgets(buffer, 128, filePTR) != NULL){
			tempSTR = realloc(tempSTR, strlen(tempSTR)+strlen(buffer)+1);
			strcat(tempSTR, buffer);
		}
		cJSON *json = cJSON_Parse(tempSTR); // Store json
		// Free up resources
		fclose(filePTR);
		free(buffer);
		free(tempSTR);

		// Ensure json was parsed properly
		if (json == NULL){
			const char *error_ptr = cJSON_GetErrorPtr();
			if (error_ptr != NULL)
			{
				fprintf(stderr, "Error before: %s\n", error_ptr);
			}
			fprintf(stderr, "Unable to parse splash texts\n");
		}
		else{
			// Copy splash texts to output file
			cJSON *splashes = cJSON_GetObjectItemCaseSensitive(json, "splashes");
			FILE *outPTR = getFileARG(arg2, "/assets/minecraft/texts/splashes.txt", "w");
			if (outPTR != NULL){
				for (int i=0; i<cJSON_GetArraySize(splashes); i++){
					char *outSTR = cJSON_GetArrayItem(splashes, i)->valuestring;
					strcat(outSTR, "\n");
					fputs(outSTR, outPTR);
				}
				fclose(outPTR); // Done writing splashes
			}
			else{
				fprintf(stderr, "Unable to copy splash texts\n");
			}

			// Free up resources
			cJSON_Delete(json);
		}
	}
}
