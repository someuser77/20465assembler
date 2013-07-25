#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "consts.h"
#include "types.h"
#include "opcodemap.h"
#include "parser.h"

typedef enum { 
            ValidOperandAddressing_None = 0, 
            ValidOperandAddressing_Instant = 1, 
            ValidOperandAddressing_Direct = 2, 
            ValidOperandAddressing_VaryingIndexing = 4, 
            ValidOperandAddressing_DirectRegister = 8,
            ValidOperandAddressing_All = ValidOperandAddressing_Instant | 
                    ValidOperandAddressing_Direct | 
                    ValidOperandAddressing_VaryingIndexing | 
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
    void fillLeaOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    
    if (OPCODE_TO_HANDLER[0] != NULL) return;
    
    OPCODE_TO_HANDLER[Opcode_mov] = fillMovOpcode; 
    OPCODE_TO_HANDLER[Opcode_cmp] =  NULL;
    OPCODE_TO_HANDLER[Opcode_add] =  NULL;
    OPCODE_TO_HANDLER[Opcode_sub] =  NULL;
    OPCODE_TO_HANDLER[Opcode_not] =  NULL;
    OPCODE_TO_HANDLER[Opcode_clr] =  NULL;
    OPCODE_TO_HANDLER[Opcode_lea] =  fillLeaOpcode;
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

Boolean tryReadOpcode(SourceLinePtr sourceLine, Opcode *opcode)
{
    int i;
    
    initNameMap();
    
    for (i = 0; i < sizeof(OPCODE_TO_NAME) / sizeof(char *); i++)
    {
        if (strncmp(sourceLine->text, OPCODE_TO_NAME[i], strlen(OPCODE_TO_NAME[i])) == 0)
        {
            *opcode = i;
            sourceLine->text += strlen(OPCODE_TO_NAME[i]);
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


void readInstantAddressingOperand(SourceLinePtr sourceLine, OperandPtr operand)
{
    int value;
    if (*sourceLine->text == INSTANT_ADDRESSING_OPERAND_PREFIX)
    {
        sourceLine->text += INSTANT_ADDRESSING_OPERAND_PREFIX_LENGTH;
    }
    
    if (!tryReadNumber(sourceLine, &value))
    {
        setSourceLineError(sourceLine, "Unable to read number for direct addressing operand.");
        return;
    }
    
    operand->addressing =  OperandAddressing_Instant;
    operand->address.value = value;
}

void readDirectRegisterAddressingOperand(SourceLinePtr sourceLine, OperandPtr operand)
{
    int registerId;
    if (*sourceLine->text == REGISTER_NAME_PREFIX)
    {
        sourceLine->text += REGISTER_NAME_PREFIX_LENGTH;
    }
    
    if (!tryReadNumber(sourceLine, &registerId))
    {
        setSourceLineError(sourceLine, "Unable to read register id for direct register address operand.");
        return;
    }
    
    if (!IS_VALID_REGISTER_ID(registerId))
    {
        setSourceLineError(sourceLine, "%d is not a valid register id.", registerId);
        return;
    }
    
    operand->addressing =  OperandAddressing_DirectRegister;
    operand->address.reg[0] = REGISTER_NAME_PREFIX;
    operand->address.reg[1] = registerId + '0';
    operand->address.reg[2] = EOL;
}

void readDirectAddressingOperand(SourceLinePtr sourceLine, OperandPtr operand)
{
    char *start, *end;
    int length;
    
    start = sourceLine->text;
    end = strchr(start, OPERAND_SEPERATOR);
    if (end == NULL)
    {
        end = strchr(start, EOL);
    }
    
    if (!isValidLabel(sourceLine, start, end))
    {
        setSourceLineError(sourceLine, "Invalid label '%s' for direct addressing operand.", start);
        return;
    }
        
    length = end - start;
    operand->addressing =  OperandAddressing_Direct;
    operand->address.label = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(operand->address.label, start, length);
    operand->address.label[length] = EOL;
}

char *cloneString(char *str, int length)
{
    char *result;
    result = (char *)malloc(sizeof(char) * length + 1);
    strncpy(result, str, length);
    result[length] = EOL;
    return result;
}

void readVaryingAddressingOperand(SourceLinePtr sourceLine, OperandPtr operand)
{
    char *start, *end, *openingBracket, *closingBracket;
    char *label;
    int length;
    
    start = sourceLine->text;
    end = strchr(start, OPERAND_SEPERATOR);

    if (end == NULL)
    {
        end = strchr(start, EOL);
        end -= 1; /* EOL */
    } 
    else 
    {
        end -= OPERAND_SEPERATOR_LENGTH;
    }
    

    openingBracket = strchr(start, VARYING_INDEXING_OPENING_TOKEN);
    if (openingBracket == NULL)
    {
        setSourceLineError(sourceLine, "Missing '%c' token from varying address operand.", VARYING_INDEXING_OPENING_TOKEN);
        return;
    }
    
    closingBracket = strchr(start, VARYING_INDEXING_CLOSING_TOKEN);
    if (closingBracket == NULL)
    {
        setSourceLineError(sourceLine, "Missing '%c' token from varying address operand.", VARYING_INDEXING_CLOSING_TOKEN);
        return;
    }
    
    label = cloneString(start, openingBracket - start);
    
    if (*(openingBracket + VARYING_INDEXING_OPENING_TOKEN_LENGTH) == VARYING_INDEXING_LABEL_TOKEN)
    {
        start = openingBracket + VARYING_INDEXING_OPENING_TOKEN_LENGTH + VARYING_INDEXING_LABEL_TOKEN_LENGTH;
        if (!isValidLabel(sourceLine, start, closingBracket - 1))
        {
            setSourceLineError(sourceLine, "Invalid label name for labeled varying indexing.");
            return;
        }
        
        length = end - start;
        
        operand->addressing = OperandAddressing_VaryingIndexing;
        operand->address.varyingAddress.label = label;
        operand->address.varyingAddress.adressing = OperandVaryingAddressing_Direct;
        operand->address.varyingAddress.address.label = cloneString(start, length);
        
        
        sourceLine->text = closingBracket + VARYING_INDEXING_CLOSING_TOKEN_LENGTH;
        return;
    }
    
    length = end - start;
    
    operand->addressing =  OperandAddressing_Direct;
    operand->address.label = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(operand->address.label, start, length);
    operand->address.label[length] = EOL;
}

Boolean readSourceOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing, InstructionLayoutPtr instruction)
{
    Boolean readOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing, OperandPtr operand);
    char *operandEnd;
    
    skipWhitespace(sourceLine);
    
    operandEnd = strchr(sourceLine->text, OPERAND_SEPERATOR);
    
    if (operandEnd == NULL)
    {
        setSourceLineError(sourceLine, "Missing operand separator '%c'.", OPERAND_SEPERATOR);
        return False;
    }

    if (!readOperand(sourceLine, validAddressing, &instruction->leftOperand))
    {
        return False;
    }
    
    return True;
}


Boolean readDestinationOperand(SourceLinePtr sourceLine, 
        ValidOperandAddressing validAddressing, 
        InstructionLayoutPtr instruction)
{
    Boolean readOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing, OperandPtr operand);
    char *operandEnd;
    
    skipWhitespace(sourceLine);
    
    operandEnd = strchr(sourceLine->text, EOL);
    
    if (operandEnd == NULL)
    {
        setSourceLineError(sourceLine, "Missing EOL.");
        return False;
    }

    if (!readOperand(sourceLine, validAddressing, &instruction->rightOperand))
    {
        return False;
    }
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text != EOL)
    {
        setSourceLineError(sourceLine, "There should be no text after the second operand.");
        return False;
    }
    
    return True;
}

Boolean addressingTypeIsAllowed(ValidOperandAddressing addressingMask, ValidOperandAddressing addressingTypeToCheck)
{
    return (addressingMask & addressingTypeToCheck) == addressingTypeToCheck;
}

Boolean readOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing, OperandPtr operand)
{
    char *start, *end, *openingBracket, *closingBracket;
    
    skipWhitespace(sourceLine);
    
    if (addressingTypeIsAllowed(validAddressing, ValidOperandAddressing_Instant))
    {
        if ((*sourceLine->text) == INSTANT_ADDRESSING_OPERAND_PREFIX)
        {
            readInstantAddressingOperand(sourceLine, operand);
#ifdef DEBUG
            printf("Found an instant address operand with the value of %d\n", operand->address.value);
#endif
            return True;
        }
    }
    
    if (addressingTypeIsAllowed(validAddressing, ValidOperandAddressing_DirectRegister))
    {
        if ((*sourceLine->text) == REGISTER_NAME_PREFIX)
        {
            readDirectRegisterAddressingOperand(sourceLine, operand);
#ifdef DEBUG
            printf("Found a direct register address operand with the value of %s\n", operand->address.reg);
#endif
            return True;
        }
    }
    
    if (addressingTypeIsAllowed(validAddressing, ValidOperandAddressing_VaryingIndexing))
    {
        start = sourceLine->text;
        end = strchr(start, OPERAND_SEPERATOR);
        if (end == NULL)
        {
            end = strchr(start, EOL);
        }
        openingBracket = strchr(start, VARYING_INDEXING_OPENING_TOKEN);
        closingBracket = strchr(start, VARYING_INDEXING_CLOSING_TOKEN);
        
        if (openingBracket != NULL && 
            closingBracket != NULL && 
            openingBracket < closingBracket && 
            closingBracket <= end)
        {
            readVaryingAddressingOperand(sourceLine, operand);
#ifdef DEBUG
            printf("Found varying addressing operand %s with direct address %s.\n", operand->address.varyingAddress.label, operand->address.varyingAddress.address.label);
#endif
            
            return True;
        }
        
    }
    
    
    setSourceLineError(sourceLine, "Unknown operand addressing scheme.");
    return False;
}

Boolean setComb(SourceLinePtr sourceLine, OpcodeLayoutPtr instructionRepresentation)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    int sourceOperandTargetBits;
    int targetOperandTargetBits;
    
    if (instructionRepresentation->type == InstructionOperandSize_Large)
    {
        /* the instruction operates on the entire word size so 
           there is no need to set cmb */
        instructionRepresentation->comb = 0;
        return True;
    }
    
    if ((*sourceLine->text) == OPCODE_CONTROL_PARAMETER_SEPERATOR)
    {
        sourceLine->text += OPCODE_CONTROL_PARAMETER_SEPERATOR_LENGTH;
    }
    
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

void fillSingleOperandOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation, ValidOperandAddressing validAddressing)
{
    Boolean setComb(SourceLinePtr sourceLine, OpcodeLayoutPtr opcodeLayout);
    
    if (!readInstructionOperandSize(sourceLine, &instructionRepresentation->opcode))
    {
        return;
    }
    
    /* if the operand size is small (10 bits instead of 20) we need to set 
     the comb bits to tell how to address each operand */
    
    if (!setComb(sourceLine, &instructionRepresentation->opcode))
    {
        return;
    }
    
    if (*sourceLine->text != OPCODE_TYPE_AND_DBL_SEPERATOR)
    {
        setSourceLineError(sourceLine, "Missing '%c' character between operand type and dbl.", OPCODE_TYPE_AND_DBL_SEPERATOR);
        return;
    }
    
    sourceLine->text += OPCODE_TYPE_AND_DBL_SEPERATOR_LENGTH;
    
    if (!readInstructionRepetition(sourceLine, &instructionRepresentation->opcode))
    {
        return;
    }
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text == EOL)
    {
        setSourceLineError(sourceLine, "Missing operands.");
        return;
    }
    
    readSourceOperand(sourceLine, validAddressing, instructionRepresentation);
    
}

void fillDoubleOperandOpcode(SourceLinePtr sourceLine, 
        InstructionLayoutPtr instructionRepresentation, 
        ValidOperandAddressing validSourceAddressing, 
        ValidOperandAddressing validDestinationAddressing)
{
    fillSingleOperandOpcode(sourceLine, instructionRepresentation, validSourceAddressing);
    
    if (sourceLine->error != NULL) return;
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text != OPERAND_SEPERATOR)
    {
        setSourceLineError(sourceLine, "Missing second operand.");
        return;
    }
    
    sourceLine->text += OPERAND_SEPERATOR_LENGTH;
    
    skipWhitespace(sourceLine);
    
    readDestinationOperand(sourceLine, validDestinationAddressing, instructionRepresentation);
}

void fillMovOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    ValidOperandAddressing validDestinationAddressing = ValidOperandAddressing_Direct |
            ValidOperandAddressing_DirectRegister |
            ValidOperandAddressing_VaryingIndexing;
            
    instructionRepresentation->opcode.opcode = Opcode_mov;
    
    fillDoubleOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_All, validDestinationAddressing);
}

void fillLeaOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    ValidOperandAddressing validAddressing = ValidOperandAddressing_Direct | 
            ValidOperandAddressing_DirectRegister |
            ValidOperandAddressing_VaryingIndexing;
    
    instructionRepresentation->opcode.opcode = Opcode_lea;
    
    fillDoubleOperandOpcode(sourceLine, instructionRepresentation, validAddressing, validAddressing);
}