/* 
 * File:   memory.h
 * Author: daniel
 *
 * Created on July 27, 2013, 3:15 PM
 */

#ifndef MEMORY_H
#define	MEMORY_H

#include <stdio.h>
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

/* prints the entire Memory to target in the specified base (2, 8, 10, 16) */
void printMemory(Memory *memory, FILE *target, int base);

/* prints a Word to target in the specified base (2, 8, 10, 16) */
void printWord(Word word, FILE *target, int base);
#endif	/* MEMORY_H */

