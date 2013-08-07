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
    char *sourceBaseFileName = NULL;
    char *sourceFullFileName = NULL;
    int ferrorCode;
    int instructionCounter;

    SymbolTable symbolTable;
    DataSection *dataSection = NULL;
    CodeSection *codeSection = NULL;
    InstructionQueue instructionQueue;

    int exitCode = EXIT_SUCCESS;
    int i;
    
    if (argc == 1)
    {
        printf("Usage: %s file1 file2 file3 ...\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    for (i = 1; i < argc; i++)
    {
        sourceBaseFileName = argv[i];
        
        sourceFullFileName = getSourceFileName(sourceBaseFileName);
        
        printf("Compiling %s...\n", sourceBaseFileName);
        
        sourceFile = fopen(sourceFullFileName, "r");

        if (sourceFile == NULL) 
        {
            fprintf(stderr, "Unable to open file %s.\n", sourceBaseFileName);
            exitCode = EXIT_FAILURE;
            goto cleanup;
        }

        codeSection = initCodeSection(&symbolTable);

        dataSection = initDataSection();

        symbolTable = initSymbolTable();

        instructionQueue = initInstructionQueue();

        instructionCounter = firstPass(sourceFile, codeSection, &instructionQueue, dataSection, sourceBaseFileName);

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

        if (!secondPass(sourceFile, codeSection, &instructionQueue, sourceBaseFileName)) 
        {
            exitCode = EXIT_FAILURE;
            goto cleanup;
        }

        if ((ferrorCode = ferror(sourceFile))) 
        {
            fprintf(stderr, "Error reading from file %s %d.", sourceBaseFileName, ferrorCode);
            exitCode = EXIT_FAILURE;
            goto cleanup;
        }

        /* printSymbolTable(&symbolTable); */


    /*
        printCodeSection(codeSection);

        printDataSection(dataSection);
    */  
        
        increaseDataSectionOffset(dataSection, getCodeSectionSize(codeSection));

        writeObjectFile(getObjectFileName(sourceBaseFileName), codeSection, dataSection);

        writeEntriesToFile(getEntriesFileName(sourceBaseFileName), &symbolTable);

        writeExternalsToFile(getExternalsFileName(sourceBaseFileName), codeSection);
    /*    
        printEntries(&symbolTable);

        printExternalSymbols(codeSection);
    */    

        printf("Done.\n");
        
    cleanup:
        free(sourceFullFileName);
        
        if (codeSection != NULL)
        {
            freeCodeSection(codeSection);
        }

        if (dataSection != NULL)
        {
            freeDataSection(dataSection);
        }

        if (sourceFile != NULL)
        {
            fclose(sourceFile);
        }
    
        freeInstructionTable(&instructionQueue);
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
    
    if (getNumberOfEntries(table) > 0)
    {
        file = fopen(fileName, "w");

        if (file == NULL)
        {
            logErrorFormat("Unable to create file %s.", fileName);
            return;
        }

        writeEntries(table, file);

        fclose(file);
    }
}


void writeExternalsToFile(char *fileName, CodeSection *codeSection)
{
    FILE *file;

    if (getNumberOfExternalSymbols(codeSection) > 0)
    {
        file = fopen(fileName, "w");

        if (file == NULL)
        {
            logErrorFormat("Unable to create file %s.", fileName);
            return;
        }

        writeExternalSymbols(codeSection, file);

        fclose(file);
    }
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