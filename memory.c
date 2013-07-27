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