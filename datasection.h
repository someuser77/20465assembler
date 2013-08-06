/* 
 * File:   data.h
 * Author: daniel
 *
 * Created on July 27, 2013, 3:22 PM
 */

#ifndef DATA_H
#define	DATA_H

#include "types.h"
#include "memory.h"

typedef struct 
{
    Memory *memory;
    Word dataBaseAddress;
} DataSection;

#define DATA_PARSE_ERROR -2
#define DATA_WRITE_ERROR -1

DataSection *initDataSection();
void freeDataSection(DataSection *dataSection);
int writeDataArray(DataSection *dataSection, SourceLinePtr sourceLine);
int writeDataString(DataSection *dataSection, SourceLinePtr sourceLine);
void writeDataSection(DataSection *dataSection, FILE *file);
void printDataSection(DataSection *dataSection);
Word getDataSectionSize(DataSection *dataSection);
#endif	/* DATA_H */

