#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "consts.h"
#include "types.h"

#define OPCODE_NAME_LENGTH 4
#define NUMBER_OF_OPCODES 16
#define OPCODE_CONTROL_PARAMETER_SEPERATOR '/'

typedef void (*OpcodHandler)(SourceLinePtr sourceLine, InstructionRepresentationPtr instructionRepresentation);

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
    void fillMovOpcode(SourceLinePtr sourceLine, InstructionRepresentationPtr instructionRepresentation);
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
    Boolean getInstructionOperandSize(SourceLinePtr sourceLine, InstructionRepresentationPtr instruction);
    Boolean getInstructionRepetition(SourceLinePtr sourceLine, InstructionRepresentationPtr instruction);
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
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
        setSourceLineError(sourceLine, "Missing %c token for opcode %s.", OPCODE_CONTROL_PARAMETER_SEPERATOR, opcodeName);
        return NULL;
    }
    
    /* skip the OPCODE_CONTROL_PARAMETER_SEPERATOR */
    sourceLine->text++;
    
    result = (InstructionRepresentationPtr)malloc(sizeof(InstructionRepresentation));
    memset(result, 0, sizeof(InstructionRepetition));
    
    if (!getInstructionOperandSize(sourceLine, result))
    {
        return NULL;
    }
    
    /* skip operand size mark */
    sourceLine->text++;
    
    (*OPCODE_TO_HANDLER[opcode])(sourceLine, result);
    
    if (*sourceLine->text != ',')
    {        
        setSourceLineError(sourceLine, "Missing ',' token for opcode %s.", OPCODE_CONTROL_PARAMETER_SEPERATOR, opcodeName);
        return NULL;
    }
    
    /* skip the ',' */
    sourceLine->text++;
    
    if (!getInstructionRepetition(sourceLine, result))
    {
        return NULL;
    }
    
    return result;
}

Boolean getInstructionRepetition(SourceLinePtr sourceLine, InstructionRepresentationPtr instruction)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    int dbl = (*sourceLine->text) - '0';
    switch (dbl)
    {
        case InstructionRepetition_Single:
            instruction->dbl = InstructionRepetition_Single;
            break;
        case InstructionRepetition_Double:
            instruction->dbl = InstructionRepetition_Double;
            break;
        default:
            setSourceLineError(sourceLine, "Unknown instruction repetition type %d", dbl);
            return False;
    }
    return True;
}

Boolean getInstructionOperandSize(SourceLinePtr sourceLine, InstructionRepresentationPtr instruction)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    int type = (*sourceLine->text) - '0';
    switch (type)
    {
        case InstructionOperandSize_Small:
            instruction->type = InstructionOperandSize_Small;
            break;
        case InstructionOperandSize_Large:
            instruction->type = InstructionOperandSize_Large;
            break;
        default:
            setSourceLineError(sourceLine, "Unknown operand size type %d", type);
            return False;
    }
    return True;
}

void setSourceLineError(SourceLinePtr sourceLine, char *error, ...)
{
    char buffer[MESSAGE_BUFFER_LENGTH];
    int length;
    va_list ap;
    va_start(ap, error);
    vsprintf(buffer, error, ap);
    va_end(ap);
    length = strlen(buffer);
    sourceLine->error = (char *)malloc(sizeof(char) * length + 1);
    if (sourceLine->error == NULL)
    {
        fprintf(stderr, "malloc error.");
        return;
    }
    strncpy(sourceLine->error, buffer, length + 1);
}

void fillMovOpcode(SourceLinePtr sourceLine, InstructionRepresentationPtr instructionRepresentation)
{
    Boolean setComb(SourceLinePtr sourceLine, InstructionRepresentationPtr instructionRepresentation);
    instructionRepresentation->opcode = Opcode_mov;
    
    /* if the operand size is small (10 bits instead of 20) we need to set 
     the comb bits to tell how to address each operand */
    
    if (!setComb(sourceLine, instructionRepresentation))
    {
        return;
    }
}

Boolean setComb(SourceLinePtr sourceLine, InstructionRepresentationPtr instructionRepresentation)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    int sourceOperandTargetBits;
    int targetOperandTargetBits;
    
    if (instructionRepresentation->type == InstructionOperandSize_Small)
    {
        instructionRepresentation->comb = 0;
        return True;
    }
    
    if ((*sourceLine->text) != OPCODE_CONTROL_PARAMETER_SEPERATOR)
    {
        setSourceLineError(sourceLine, "'%c' required on small size operand address", OPCODE_CONTROL_PARAMETER_SEPERATOR);
        return False;
    }
    
    /* skip the '/' */
    sourceLine->text++;
    
    /* the first operand is always the source and the second is always the target */
    /* so the first number relates to the source and the second to the target */
    
    sourceOperandTargetBits = (*sourceLine->text) - '0';
    switch (sourceOperandTargetBits)
    {
        case OperandTargetBits_HighNibble:
            instructionRepresentation->comb &= ~COMB_SOURCE_OPERAND_MASK;
            break;
        case OperandTargetBits_LowNibble:
            instructionRepresentation->comb |= COMB_SOURCE_OPERAND_MASK;
            break;
        default:
            setSourceLineError(sourceLine, "Unknown value '%d' for bits to use.", sourceOperandTargetBits);
            return False;
    }
    
    /* skip the value */
    sourceLine->text++;
    
    if ((*sourceLine->text) != OPCODE_CONTROL_PARAMETER_SEPERATOR)
    {
        setSourceLineError(sourceLine, "'%c' required between the operand bit sizes to use.", OPCODE_CONTROL_PARAMETER_SEPERATOR);
        return False;
    }
    
    /* skip the '/' */
    sourceLine->text++;
    
    targetOperandTargetBits = (*sourceLine->text) - '0';
    switch (targetOperandTargetBits)
    {
        case OperandTargetBits_HighNibble:
            instructionRepresentation->comb &= ~COMB_TARGET_OPERAND_MASK;
            break;
        case OperandTargetBits_LowNibble:
            instructionRepresentation->comb |= COMB_TARGET_OPERAND_MASK;
            break;
        default:
            setSourceLineError(sourceLine, "Unknown value '%d' for bits to use.", targetOperandTargetBits);
            return False;
    }
    
    /* skip the value */
    sourceLine->text++;
    
    return True;
}