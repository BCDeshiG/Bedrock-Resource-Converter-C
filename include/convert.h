// Include file for `convert.c`

#ifndef HEADER_INCLUDE_CONVERT
#define HEADER_INCLUDE_CONVERT
#define MAX_PATH_LENGTH 4096
char *arg1; // Input - Bedrock resource pack path
char *arg2; // Output - Java resource pack path

char *initArg(char *arg, char msg[]); // Interactive mode
void startConversion(void); // Runs conversion procedures
void genFolders(char *arg2); // Creates necessary folders
void wrapUp(void); // Ending messages
#endif
