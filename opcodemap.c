#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h> 
#include "consts.h"
#include "types.h"
#include "opcodemap.h"
#include "parser.h"

typedef enum { 
            ValidOperandAddressing_None = 0, 
            ValidOperandAddressing_Instant = 1, 
            ValidOperandAddressing_Direct = 2, 
            ValidOperandAddressing_VaringIndexing = 4, 
            ValidOperandAddressing_DirectRegister = 8,
            ValidOperandAddressing_All = ValidOperandAddressing_Instant | 
                    ValidOperandAddressing_Direct | 
                    ValidOperandAddressing_VaringIndexing | 
                    ValidOperandAddressing_DirectRegister
} ValidOperandAddressing;


static const char *OPCODE_TO_NAME[NUMBER_OF_OPCODES] = { NULL };
static OpcodeHandler OPCODE_TO_HANDLER[NUMBER_OF_OPCODES] = { NULL };
/*
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", 
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop" };
*/
typedef struct tOpcodeMapEntry{
    char name[OPCODE_NAME_LENGTH];
    OpcodeLayout opcode;
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
    void fillMovOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
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

char* getOpcodeName(Opcode opcode)
{
    int length;
    char *name;
    
    /* at this stage the opcode was already checked for existence */
    
    initNameMap();
    length = strlen(OPCODE_TO_NAME[opcode]) + 1;
    name = (char *)malloc(sizeof(char) * (length));
    strncpy(name, OPCODE_TO_NAME[opcode], length);
    return name;
}

OpcodeHandler getOpcodeHandler(Opcode opcode)
{
    initHandlerMap();
    return OPCODE_TO_HANDLER[opcode];
}

Boolean readInstructionOperandSize(SourceLinePtr sourceLine, OpcodeLayoutPtr instruction)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    /* assumed to be a single character, otherwise more 
     * clever string manipulation is required */
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
    
    /* skip 'type' value */
    sourceLine->text++;
    
    return True;
}

Boolean readInstructionRepetition(SourceLinePtr sourceLine, OpcodeLayoutPtr instruction)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    /* assumed to be a single character, otherwise more 
     * clever string manipulation is required */
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
    sourceLine->text++;
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

void fillMovOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    Boolean setComb(SourceLinePtr sourceLine, OpcodeLayoutPtr opcodeLayout);
    instructionRepresentation->opcode = Opcode_mov;
    
    if (!readInstructionOperandSize(sourceLine, instructionRepresentation->opcode))
    {
        return;
    }
    
    /* if the operand size is small (10 bits instead of 20) we need to set 
     the comb bits to tell how to address each operand */
    
    if (!setComb(sourceLine, instructionRepresentation->opcode))
    {
        return;
    }
    
    if (*sourceLine->text != OPCODE_TYPE_AND_DBL_SEPERATOR)
    {
        setSourceLineError(sourceLine, "Missing '%c' character between operand type and dbl.", OPCODE_TYPE_AND_DBL_SEPERATOR);
        return;
    }
    
    if (!readInstructionRepetition(sourceLine, instructionRepresentation->opcode))
    {
        return;
    }
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text == EOL)
    {
        setSourceLineError(sourceLine, "Missing operands.");
        return;
    }
    
    
}

Boolean readSourceOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing)
{
    char *operandStart;
    char *operandEnd;
    char *endptr;
    char *buffer;
    int length;
    int directAddressValue;
    
    skipWhitespace(sourceLine);
    
    operandStart = sourceLine->text;
    
    operandEnd = strchr(sourceLine->text, OPERAND_SEPERATOR);
    
    if (operandEnd == NULL)
    {
        setSourceLineError(sourceLine, "Missing operand separator '%c'.", OPERAND_SEPERATOR);
        return False;
    }
    
    length = operandEnd - operandStart;
    
    if ((validAddressing & ValidOperandAddressing_Direct) == ValidOperandAddressing_Direct)
    {
        /* try to get a direct addressing */
        if ((*sourceLine->text) == DIRECT_ADDRESSING_OPERAND_PREFIX)
        {
            operandStart++;
            length = operandEnd - operandStart;
            
            buffer = (char *)malloc(sizeof(char) * (length + 1));
    
            strncpy(buffer, operandStart, length);
            buffer[length] = EOL;
            
            directAddressValue = strtol(buffer, &endptr, 10);
            if (errno == ERANGE)
            {
                setSourceLineError(sourceLine, "Value out of range when reading direct address value.");
                return False;
            }
            
            if ((directAddressValue == 0 && endptr == buffer) || *endptr == EOL)
            {
                setSourceLineError(sourceLine, "Unable to convert direct address to number.");
                return False;
            }
        }
    }
    
    sourceLine->text = operandEnd + 1;
    
    return True;
}

Boolean readDestinationOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing)
{
    return True;
}

Boolean setComb(SourceLinePtr sourceLine, OpcodeLayoutPtr instructionRepresentation)
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
