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
    
    FILE *sourceFile;
    char *sourceFileName = "example.as";
    
    int ferrorCode;
    
    
    SymbolTable symbolTable;
    DataSection dataSection;
    CodeSection codeSection;
    
    sourceFile = fopen(sourceFileName, "r");
    if (sourceFile == NULL)
    {
        fprintf(stderr, "Error opening file %s.", sourceFileName);
        exit(1);
    }
    
    
    symbolTable = initSymbolTable();
    
    if (!firstPass(sourceFile, &symbolTable, &dataSection, &codeSection, sourceFileName))
    {
        fclose(sourceFile);
        return EXIT_FAILURE;
    }
  
    rewind(sourceFile);
    
    if (!secondPass(sourceFile, &symbolTable, &dataSection, &codeSection, sourceFileName))
    {
        fclose(sourceFile);
        return EXIT_FAILURE;
    }
    
    if ((ferrorCode = ferror(sourceFile)))
    {
        fprintf(stderr, "Error reading from file %s %d.", sourceFileName, ferrorCode);
        exit(EXIT_FAILURE);
    }
    
    fclose(sourceFile);
    
    return (EXIT_SUCCESS);
}
