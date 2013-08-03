/* 
 * File:   memory.h
 * Author: daniel
 *
 * Created on July 27, 2013, 3:15 PM
 */

#ifndef MEMORY_H
#define	MEMORY_H

#include "consts.h"
#include "types.h"


#define MEMORY_OUT_OF_MEMORY -1

typedef struct tMemory
{
    Word buffer[MAX_MEMORY_SIZE];
    int position;
} Memory;

Memory *initMemory();
void freeMemory(Memory *memory);
int writeInt(Memory *memory, int value);
int writeWord(Memory *memory, Word value);
void printMemory(Memory *memory);
#endif	/* MEMORY_H */

