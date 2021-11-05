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

void stripCHAR(char *string, char *toStrip){
	char *stripME = strstr(string, toStrip);
	if (stripME != NULL){
		strcpy(stripME, "\0");
	}
}
