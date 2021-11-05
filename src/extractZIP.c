/* Copyright (C) 2011 rocenting#gmail.com */
/* https://gist.github.com/mobius/1759816 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
 
#include "extractZIP.h"
#include "helper.h"
 
int extract(const char *archive)
{
	struct zip *za; // Zip archive
	struct zip_file *zf; // Stores file to be extracted
	struct zip_stat sb; // Stores file info
	char buf[100]; // Buffer to write stuff
	int i, len;
	FILE * fd; // Where file is extracted to
	long long sum; // How much file has been copied so far

	// Open the zip file
	if ((za = zip_open(archive, 0, NULL)) == NULL) {
		fprintf(stderr, "Can't open zip archive '%s'\n", archive);
		exit(1);
	} 

	// Unpack zip
	for (i=0; i < zip_get_num_entries(za, 0); i++) { // Iterate through all files in zip
		if (zip_stat_index(za, i, 0, &sb) == 0) { // Tries to grab file info
			len = strlen(sb.name);
			if (sb.name[len - 1] == '/') { // Check if directory
				safe_create_dir(sb.name); // Create directory
			} else {
				zf = zip_fopen_index(za, i, 0); // Open file within zip
				if (!zf) {
					fprintf(stderr, "Unable to open file within zip\n");
					exit(100);
				}

				fd = fopen(sb.name, "wb"); // Create new file
				if (fd == NULL) {
					fprintf(stderr, "Unable to create destination file\n");
					exit(101);
				}

				sum = 0;
				while (sum != sb.size) { // Copy bytes to new file
					len = zip_fread(zf, buf, 100);
					if (len < 0) {
						fprintf(stderr, "Unable to extract file contents\n");
						exit(102);
					}
					fwrite(buf, 1, len, fd);
					sum += len;
				}
				// Finished copying file
				fclose(fd);
				zip_fclose(zf);
			}
        }
	}
 
	if (zip_close(za) == -1) {
		fprintf(stderr, "Can't close zip archive '%s'\n", archive);
		exit(1);
	}
 
	return 0;
}
