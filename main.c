/* 
 * File:   main.c
 * Author: daniel
 *
 * Created on July 8, 2013, 8:58 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "parser.h"
#include "consts.h"


/*
 * 
 */
int main(int argc, char** argv) {
    char buffer[LINE_BUFFER_LENGTH + 1] = {0};
    FILE *sourceFile;
    char *sourceFileName = "example.as";
    SourceLine line;
    char *label;
    
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
    
    line = initSourceLine(buffer, 0, sourceFileName);
    
    label = getLabel(&line);
    if (label != NULL)
    {
        puts(label);
    }
    fclose(sourceFile);
    
    return (EXIT_SUCCESS);
}
