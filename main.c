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
    char *bufferPos;
    char *label;
    int ferrorCode;
    
    sourceFile = fopen(sourceFileName, "r");
    if (sourceFile == NULL)
    {
        fprintf(stderr, "Error opening file %s.", sourceFileName);
        exit(1);
    }
    
    while (fgets(buffer, LINE_BUFFER_LENGTH, sourceFile) != NULL)
    {
        bufferPos = buffer;
        puts(bufferPos);
        
        if (isBlankLine(bufferPos))
        {
            /* blank line */
            continue;
        }
        
        bufferPos = skipWhitespace(bufferPos);
        
        if (isCommentLine(bufferPos))
        {
            /* comment line */
            continue;
        }
        
        line = initSourceLine(bufferPos, 0, sourceFileName);


        label = getLabel(&line);
        if (label != NULL)
        {
            puts(label);
        }
    }
    
    if ((ferrorCode = ferror(sourceFile)))
    {
        fprintf(stderr, "Error reading from file %s %d.", sourceFileName, ferrorCode);
        exit(EXIT_FAILURE);
    }
    
    fclose(sourceFile);
    
    return (EXIT_SUCCESS);
}
