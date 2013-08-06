#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "types.h"
#include "memory.h"

Word writeWord(Memory *memory, Word value)
{
    if (memory->position == MAX_MEMORY_SIZE)
    {
        return MEMORY_OUT_OF_MEMORY;
    }
    
    memory->buffer[memory->position] = value;
    memory->position++;
    
    return memory->position;
}

Word writeInt(Memory *memory, int value)
{
    Word word = value;
    return writeWord(memory, word);
}

void printWord(Word word, FILE *target, int base)
{
    static char base_digits[16] =
	 {'0', '1', '2', '3', '4', '5', '6', '7',
	  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    int position;
    char *value;
    
    double maxValueBase2 = pow(2, MACHINE_WORD_LENGTH + 1);
    double base10nominator = log10(maxValueBase2);
    double base10denominator = log10(base);
    /* calculate log in base's base. */
    int digits = ceil(base10nominator / base10denominator);
    unsigned long unsignedValue;
    
    if (word < 0) unsignedValue = maxValueBase2 + word;
    else unsignedValue = word;
    
    value = (char *)malloc(sizeof(char) * (digits + 1));

    sprintf(value, "%7.0lo", word);
    
    return;
    
    memset(value, '0', digits + 1);
    
    position = digits - 1;
    
    while (unsignedValue != 0)
    {
        value[position] = base_digits[unsignedValue % base];
        unsignedValue = unsignedValue / base;
        position--;
    }
    
    value[digits] = EOL;
    
    fprintf(target, "%s", value);
    
    free(value);
}

void printMemory(Memory *memory, FILE *target, int base)
{
    int i;
    for (i = 0; i < memory->position; i++)
    {
        printWord(memory->buffer[i], target, base);
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
        memory->buffer[i] = 0;
    }
    
    return memory;
}

void freeMemory(Memory *memory)
{
    free(memory);
}

Word getMemoryPosition(Memory *memory)
{
    return memory->position;    
}