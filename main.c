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
#include "instructionqueue.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    FILE *sourceFile;
    char *sourceFileName = "example.as";
    
    int ferrorCode;
    int instructionCounter;
    
    SymbolTable symbolTable;
    DataSection *dataSection;
    CodeSection *codeSection;
    InstructionQueue instructionQueue;
    
    int exitCode;
    
    codeSection = initCodeSection(&symbolTable);
    dataSection = initDataSection();
    
    sourceFile = fopen(sourceFileName, "r");
    
    if (sourceFile == NULL)
    {
        fprintf(stderr, "Error opening file %s.", sourceFileName);
        exit(1);
    }
    
    symbolTable = initSymbolTable();
    
    instructionQueue = initInstructionQueue();
    
    instructionCounter = firstPass(sourceFile, &symbolTable, &instructionQueue, dataSection, sourceFileName);
    
    if (instructionCounter == -1)
    {
        exitCode = EXIT_FAILURE;
        goto cleanup;
    }
  
    rewind(sourceFile);

#ifdef DEBUG
    printf("Shifting data by %d.\n", instructionCounter);
#endif
    
    fixDataOffset(&symbolTable, instructionCounter);
    
    if (!secondPass(sourceFile, codeSection, &instructionQueue, sourceFileName))
    {
        exitCode = EXIT_FAILURE;
        goto cleanup;
    }
    
    if ((ferrorCode = ferror(sourceFile)))
    {
        fprintf(stderr, "Error reading from file %s %d.", sourceFileName, ferrorCode);
        exitCode = EXIT_FAILURE;
        goto cleanup;
    }
    
    printSymbolTable(&symbolTable);
    
    printCodeSection(codeSection);
    
    exitCode = EXIT_SUCCESS;
    
cleanup:
    freeCodeSection(codeSection);
    freeDataSection(dataSection);
    fclose(sourceFile);
    
    return exitCode;
}
