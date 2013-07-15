/* 
 * File:   main.c
 * Author: daniel
 *
 * Created on July 8, 2013, 8:58 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "parser.h"
#include "consts.h"
#include "symboltable.h"


/*
 * 
 */
int main(int argc, char** argv) {
    char buffer[LINE_BUFFER_LENGTH + 1] = {0};
    FILE *sourceFile;
    char *sourceFileName = "example.as";
    SourceLine line;
    ptrSourceLine linePtr = &line;
    char *bufferPos;
    char *label;
    
    int ferrorCode;
    Boolean foundSymbol;
    SymbolType symbolType;
    SymbolTable symbolTable;
    
    int dataCounter, instructionCounter;
    
    sourceFile = fopen(sourceFileName, "r");
    if (sourceFile == NULL)
    {
        fprintf(stderr, "Error opening file %s.", sourceFileName);
        exit(1);
    }
    
    dataCounter = 0;
    instructionCounter = 0;
    symbolTable = initSymbolTable();
    
    while (fgets(buffer, LINE_BUFFER_LENGTH, sourceFile) != NULL)
    {
        bufferPos = buffer;
        puts(bufferPos);
        
        
        line = initSourceLine(bufferPos, 0, sourceFileName);
        
        if (isBlankLine(linePtr))
        {
            /* blank line */
            continue;
        }
        
        skipWhitespace(linePtr);
        
        if (isCommentLine(linePtr))
        {
            /* comment line */
            continue;
        }

        label = getLabel(linePtr);
        if (label != NULL)
        {
            printf("Label: %s\n", label);
            linePtr->text += strlen(label);
            linePtr->text += 1; /* skip the ':' */
            foundSymbol = True;
        } 
        else
        {
            foundSymbol = False;    
        }
        
        skipWhitespace(linePtr);
        
        if (*linePtr->text == GUIDANCE_TOLEN)
        {            
            if (tryGetGuidanceType(linePtr, &symbolType))
            {
                insertSymbol(&symbolTable, label, symbolType, dataCounter);
                
            }
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
