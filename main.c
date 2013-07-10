/* 
 * File:   main.c
 * Author: daniel
 *
 * Created on July 8, 2013, 8:58 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

#define LINE_BUFFER_LENGTH 255

/*
 * 
 */
int main(int argc, char** argv) {
    char buffer[LINE_BUFFER_LENGTH + 1] = {0};
    FILE *sourceFile;
    char *sourceFileName = "example.as";
    sourceFile = fopen(sourceFileName, "r");
    if (sourceFile == NULL)
    {
        fprintf(stderr, "Error opening file %s.", sourceFileName);
        exit(1);
    }
    
    if (fgets(buffer, LINE_BUFFER_LENGTH, sourceFile) == NULL)
    {
        fprintf(stderr, "Error reading line from file %s.", sourceFileName);
        exit(EXIT_FAILURE);
    }
    
    puts(buffer);
    
    fclose(sourceFile);
    
    return (EXIT_SUCCESS);
}
