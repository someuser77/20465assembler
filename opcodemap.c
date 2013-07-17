#include <string.h>
#include <stdlib.h>
#include "consts.h"
#include "types.h"
#include "parser.h"

#define OPCODE_NAME_LENGTH 4
#define NUMBER_OF_OPCODES 16
#define OPCODE_CONTROL_PARAMETER_SEPERATOR '/'

typedef void (*OpcodHandler)(InstructionRepresentationPtr instructionRepresentation, SourceLinePtr sourceLine);

static const char *OPCODE_TO_NAME[NUMBER_OF_OPCODES] = { NULL };
static OpcodHandler OPCODE_TO_HANDLER[NUMBER_OF_OPCODES] = { NULL };
/*
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", 
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop" };
*/
typedef struct tOpcodeMapEntry{
    char name[OPCODE_NAME_LENGTH];
    InstructionRepresentation instructionRepresentation;
} OpcodeMapEntry, *OpcodeMapEntryPtr;

void initNameMap()
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

void initHandlerMap()
{
    void fillMovOpcode(InstructionRepresentationPtr instructionRepresentation, SourceLinePtr sourceLine);
    if (OPCODE_TO_HANDLER[0] != NULL) return;
    
    OPCODE_TO_HANDLER[Opcode_mov] = fillMovOpcode; 
    OPCODE_TO_HANDLER[Opcode_cmp] =  NULL;
    OPCODE_TO_HANDLER[Opcode_add] =  NULL;
    OPCODE_TO_HANDLER[Opcode_sub] =  NULL;
    OPCODE_TO_HANDLER[Opcode_not] =  NULL;
    OPCODE_TO_HANDLER[Opcode_clr] =  NULL;
    OPCODE_TO_HANDLER[Opcode_lea] =  NULL;
    OPCODE_TO_HANDLER[Opcode_inc] =  NULL;
    OPCODE_TO_HANDLER[Opcode_dec] =  NULL;
    OPCODE_TO_HANDLER[Opcode_jmp] =  NULL;
    OPCODE_TO_HANDLER[Opcode_bne] =  NULL;
    OPCODE_TO_HANDLER[Opcode_red] =  NULL;
    OPCODE_TO_HANDLER[Opcode_prn] =  NULL;
    OPCODE_TO_HANDLER[Opcode_jsr] =  NULL;
    OPCODE_TO_HANDLER[Opcode_rts] =  NULL;
    OPCODE_TO_HANDLER[Opcode_stop] = NULL;
}

Boolean isValidOpcodeName(char *instruction)
{
    int i;
    
    initNameMap();
    
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
    
    initNameMap();
    
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

InstructionRepresentationPtr getInstructionRepresentation(SourceLinePtr sourceLine, Opcode opcode)
{
    char *errorMessage;
    const char *opcodeName;
    int opcodeLength = strlen(OPCODE_TO_NAME[opcode]);
    InstructionRepresentationPtr result;
    
    opcodeName = OPCODE_TO_NAME[opcode];
    
    initNameMap();
    initHandlerMap();
    
    /* if the text still points to the opcode skip it */
    if (strncmp(sourceLine->text, opcodeName, opcodeLength) == 0)
    {
        sourceLine->text += opcodeLength;
    }
    
    if (*sourceLine->text != OPCODE_CONTROL_PARAMETER_SEPERATOR)
    {        
        errorMessage = "Missing %c token for opcode %s.";
        sourceLine->error = (char *)malloc(sizeof(strlen(errorMessage) + 1 + opcodeLength));
        
        if (sourceLine->error == NULL)
        {
            fprintf(stderr, "malloc error.\n");
            exit(EXIT_FAILURE);
        }
        
        sprintf(sourceLine->error, errorMessage, OPCODE_CONTROL_PARAMETER_SEPERATOR, opcodeName);
        
        return NULL;
    }
    
    result = (InstructionRepresentationPtr)malloc(sizeof(InstructionRepresentation));
    
    (*OPCODE_TO_HANDLER[opcode])(result, sourceLine);
    
    return result;
}
    
void fillMovOpcode(InstructionRepresentationPtr instructionRepresentation, SourceLinePtr sourceLine)
{
    
}