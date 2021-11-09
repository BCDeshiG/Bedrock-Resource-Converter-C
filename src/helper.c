#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "helper.h"

void safe_create_dir(const char *dir)
{
    if (mkdir(dir, 0755) < 0) {
        if (errno != EEXIST) {
            perror(dir);
            exit(1);
        }
    }
}

void stripCHAR(char *string, char toStrip){
	char *stripME = strrchr(string, toStrip);
	if (stripME != NULL){
		strcpy(stripME, "\0");
	}
}

void dirSPACES(char *string){
	char *token; // Stores chunk of text
	unsigned short numTokens = 0; // Tracks number of chunks
	const char sp[2] = " ";
	unsigned short length = strlen(string);

	// strtok() modifies so make a copy
	char *tempSTR = calloc(length, sizeof(char));
	strcpy(tempSTR, string);

	token = strtok(tempSTR, sp); // Get 1st chunk
	while (token != NULL){
		numTokens++;
		token = strtok(NULL, sp); // Get next chunk
	}
	if (numTokens == 1){ // No splits happened
		free(tempSTR); // Wasn't needed after all
		return;
	}

	// Resize to fit escape characters
	string = realloc(string, (length+numTokens+1)*sizeof(char));
	strcpy(tempSTR, string); // Copy string again
	token = strtok(tempSTR, sp); // Re-get 1st chunk
	strcpy(string, token);
	for(short i=1; i<numTokens; i++){
		token = strtok(NULL, sp); // Get next chunk
		strcat(string, "\ "); // Add escaped space
		strcat(string, token); // Add next chunk
	}
	free(tempSTR);
}
