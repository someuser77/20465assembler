#include <string.h>
#include "consts.h"
#include "types.h"

static const char *OPCODES[] = { 
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", 
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop" };

Boolean isValidOpcodeName(char *instruction)
{
    int i;
    
    for (i = 0; i < sizeof(OPCODES) / sizeof(char *); i++)
    {
        if (strcmp(instruction, OPCODES[i]) == 0)
        {
            return True;
        }        
    }
    
    return False;
}
