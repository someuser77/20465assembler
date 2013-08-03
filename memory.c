#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "memory.h"

int writeWord(Memory *memory, Word value)
{
    if (memory->position == MAX_MEMORY_SIZE)
    {
        return MEMORY_OUT_OF_MEMORY;
    }
    
    memory->buffer[memory->position] = value;
    memory->position++;
    
    return memory->position;
}

int writeInt(Memory *memory, int value)
{
    Word word;
    word.value = value;
    return writeWord(memory, word);
}

void printWord(Word word)
{
    int i;
    int value;
    
    for (i = MACHINE_WORD_LENGTH - 1; i >= 0; i--)
    {
        value = (word.value & (1 << i)) == (1 << i);
        printf("%d", value);
    }
}

void printMemory(Memory *memory)
{
    int i;
    for (i = 0; i < memory->position; i++)
    {
        printf("%d:\t", i);
        printWord(memory->buffer[i]);
        printf("\n\n");
    }
}

Memory *initMemory()
{
    Memory *memory;
    int i;
    
    memory = (Memory *)malloc(sizeof(Memory));
    memory->position = 0;
    
    for (i = 0; i < MAX_MEMORY_SIZE; i++)
    {
        memory->buffer[i].value = 0;
    }
    
    return memory;
}

void freeMemory(Memory *memory)
{
    free(memory);
}