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
#include "logging.h"

char *getEntriesFileName(char *sourceFileName);
char *getSourceFileName(char *sourceFileName);
char *getExternalsFileName(char* sourceFileName);
char *getObjectFileName(char* sourceFileName);
void writeEntriesToFile(char *fileName, SymbolTablePtr table);
void writeExternalsToFile(char *fileName, CodeSection *codeSection);
void writeObjectFile(char *fileName, CodeSection *codeSection, DataSection *dataSection);

int main(int argc, char** argv) {

    FILE *sourceFile = NULL;
    char *sourceFileName = "ps";

    int ferrorCode;
    int instructionCounter;

    SymbolTable symbolTable;
    DataSection *dataSection;
    CodeSection *codeSection;
    InstructionQueue instructionQueue;

    int exitCode;
    /*
    printWord(-5, stdout, 8);
    return 0; */
    
    sourceFile = fopen(getSourceFileName(sourceFileName), "r");

    if (sourceFile == NULL) 
    {
        fprintf(stderr, "Unable to open file %s.", sourceFileName);
        goto cleanup;
    }

    codeSection = initCodeSection(&symbolTable);
    
    dataSection = initDataSection();

    symbolTable = initSymbolTable();

    instructionQueue = initInstructionQueue();

    instructionCounter = firstPass(sourceFile, codeSection, &instructionQueue, dataSection, sourceFileName);

    if (instructionCounter == -1) 
    {
        exitCode = EXIT_FAILURE;
        goto cleanup;
    }

    rewind(sourceFile);

#ifdef DEBUG
    printf("Shifting data by %d.\n", instructionCounter);
#endif

    fixDataOffset(codeSection, instructionCounter);

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

    /* printSymbolTable(&symbolTable); */
    
    
/*
    printCodeSection(codeSection);

    printDataSection(dataSection);
*/  
    

    writeObjectFile(getObjectFileName(sourceFileName), codeSection, dataSection);
    
    writeEntriesToFile(getEntriesFileName(sourceFileName), &symbolTable);
    
    writeExternalsToFile(getExternalsFileName(sourceFileName), codeSection);
/*    
    printEntries(&symbolTable);
    
    printExternalSymbols(codeSection);
*/    
    exitCode = EXIT_SUCCESS;

cleanup:
    freeCodeSection(codeSection);
    freeDataSection(dataSection);
    if (sourceFile != NULL)
    {
        fclose(sourceFile);
    }

    return exitCode;
}

char *appendExtensionToFileName(char *fileName, char *extension)
{
    int fileNameLength = strlen(fileName);
    int extensionLength = strlen(extension);
    char *fileNameWithExtension;

    fileNameWithExtension = (char *)malloc(sizeof (char) * (fileNameLength + extensionLength + 1));

    strncpy(fileNameWithExtension, fileName, fileNameLength);
    
    strncpy(fileNameWithExtension + fileNameLength, extension, extensionLength);
    
    fileNameWithExtension[fileNameLength + extensionLength] = EOL;

    return fileNameWithExtension;
}

char *getSourceFileName(char *sourceFileName)
{
    return appendExtensionToFileName(sourceFileName, SOURCE_FILE_EXTENSION);
}

char *getEntriesFileName(char *sourceFileName) 
{
    return appendExtensionToFileName(sourceFileName, ENTRIES_FILE_EXTENSION);
}

char *getExternalsFileName(char *sourceFileName) 
{
    return appendExtensionToFileName(sourceFileName, EXTERNALS_FILE_EXTENSION);
}

void writeEntriesToFile(char *fileName, SymbolTablePtr table) 
{
    FILE *file;
    
    file = fopen(fileName, "w");
    
    if (file == NULL)
    {
        logErrorFormat("Unable to create file %s.", fileName);
        return;
    }
    
    writeEntries(table, file);
    
    fclose(file);
}


void writeExternalsToFile(char *fileName, CodeSection *codeSection)
{
    FILE *file;
    
    file = fopen(fileName, "w");
    
    if (file == NULL)
    {
        logErrorFormat("Unable to create file %s.", fileName);
        return;
    }
    
    writeExternalSymbols(codeSection, file);
    
    fclose(file);
}

char *getObjectFileName(char* sourceFileName)
{
    return appendExtensionToFileName(sourceFileName, OBJECT_FILE_NAME);
}

void writeObjectFile(char *fileName, CodeSection *codeSection, DataSection *dataSection)
{
    FILE *file;
    
    file = fopen(fileName, "w");
    
    if (file == NULL)
    {
        logErrorFormat("Unable to create file %s.", fileName);
        return;
    }
    
    fprintf(file, "%lo\t%lo\n", getCodeSectionSize(codeSection), getDataSectionSize(dataSection));
    
    writeCodeSection(codeSection, file);
    
    writeDataSection(dataSection, file);
    
    fclose(file);
}