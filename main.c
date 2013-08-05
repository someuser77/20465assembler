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
void writeEntriesToFile(char *fileName, SymbolTablePtr table);
void writeExternalsToFile(char *fileName, CodeSection *codeSection);

int main(int argc, char** argv) {

    FILE *sourceFile;
    char *sourceFileName = "ps";

    int ferrorCode;
    int instructionCounter;

    SymbolTable symbolTable;
    DataSection *dataSection;
    CodeSection *codeSection;
    InstructionQueue instructionQueue;

    int exitCode;

    codeSection = initCodeSection(&symbolTable);
    dataSection = initDataSection();

    sourceFile = fopen(getSourceFileName(sourceFileName), "r");

    if (sourceFile == NULL) 
    {
        fprintf(stderr, "Error opening file %s.", sourceFileName);
        exit(1);
    }

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

    printSymbolTable(&symbolTable);
/*
    printCodeSection(codeSection);

    printDataSection(dataSection);
*/
    writeEntriesToFile(getEntriesFileName(sourceFileName), &symbolTable);
    
    writeExternalsToFile(getExternalsFileName(sourceFileName), codeSection);
    
    exitCode = EXIT_SUCCESS;

cleanup:
    freeCodeSection(codeSection);
    freeDataSection(dataSection);
    fclose(sourceFile);

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
    
    writeEntries(file, table);
    
    fclose(file);
}

void writeExternalSymbol(ListNodeDataPtr nodeData, void *context)
{
    printf("%s\t%o\n", nodeData->symbolLocation->symbol, nodeData->symbolLocation->location.value);
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
    
    actOnList(&codeSection->externalSymbols, writeExternalSymbol, NULL);
    
    fclose(file);
}
