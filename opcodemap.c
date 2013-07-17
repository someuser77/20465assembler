#include <string.h>
#include "consts.h"
#include "types.h"
#include "parser.h"

#define OPCODE_NAME_LENGTH 4
#define NUMBER_OF_OPCODES 16
#define OPCODE_CONTROL_PARAMETER_SEPERATOR '/'

static const char *OPCODE_TO_NAME[NUMBER_OF_OPCODES] = { NULL };

/*
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", 
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop" };
*/
typedef struct tOpcodeMapEntry{
    char name[OPCODE_NAME_LENGTH];
    InstructionRepresentation instructionRepresentation;
} OpcodeMapEntry, *OpcodeMapEntryPtr;

void initMap()
{
    if (OPCODE_TO_NAME[0] != NULL) return;
    
    OPCODE_TO_NAME[Opcode_mov] = "mov"; 
    OPCODE_TO_NAME[Opcode_cmp] =  "cmp";
    OPCODE_TO_NAME[Opcode_add] =  "add";
    OPCODE_TO_NAME[Opcode_sub] =  "sub";
    OPCODE_TO_NAME[Opcode_not] =  "not";
    OPCODE_TO_NAME[Opcode_clr] =  "clr";
    OPCODE_TO_NAME[Opcode_lea] =  "lea";
    OPCODE_TO_NAME[Opcode_inc] =  "inc";
    OPCODE_TO_NAME[Opcode_dec] =  "dec";
    OPCODE_TO_NAME[Opcode_jmp] =  "jmp";
    OPCODE_TO_NAME[Opcode_bne] =  "bne";
    OPCODE_TO_NAME[Opcode_red] =  "red";
    OPCODE_TO_NAME[Opcode_prn] =  "prn";
    OPCODE_TO_NAME[Opcode_jsr] =  "jsr";
    OPCODE_TO_NAME[Opcode_rts] =  "rts";
    OPCODE_TO_NAME[Opcode_stop] = "stop";
    
}

Boolean isValidOpcodeName(char *instruction)
{
    int i;
    
    initMap();
    
    for (i = 0; i < sizeof(OPCODE_TO_NAME) / sizeof(char *); i++)
    {
        if (strcmp(instruction, OPCODE_TO_NAME[i]) == 0)
        {
            return True;
        }        
    }
    
    return False;
}

Boolean tryGetOpcode(SourceLinePtr sourceLine, Opcode *opcode)
{
    int i;
    
    initMap();
    
    /*
    end = strchr(sourceLine->text, OPCODE_CONTROL_PARAMETER_SEPERATOR);
    
    if (end == NULL)
    {
        return False;
    }
    */
    for (i = 0; i < sizeof(OPCODE_TO_NAME) / sizeof(char *); i++)
    {
        if (strncmp(sourceLine->text, OPCODE_TO_NAME[i], strlen(OPCODE_TO_NAME[i])) == 0)
        {
            *opcode = i;
            return True;
        }        
    }
    
    return False;
}
