#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
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
            ValidOperandAddressing_AllExceptInstant = ValidOperandAddressing_Direct | 
                    ValidOperandAddressing_VaryingIndexing | 
                    ValidOperandAddressing_DirectRegister,
            ValidOperandAddressing_All = ValidOperandAddressing_Instant | 
                    ValidOperandAddressing_Direct | 
                    ValidOperandAddressing_VaryingIndexing | 
                    ValidOperandAddressing_DirectRegister
} ValidOperandAddressing;


static char *OPCODE_TO_NAME[NUMBER_OF_OPCODES] = { NULL };
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
    
    OPCODE_TO_NAME[Opcode_mov]  = "mov"; 
    OPCODE_TO_NAME[Opcode_cmp]  = "cmp";
    OPCODE_TO_NAME[Opcode_add]  = "add";
    OPCODE_TO_NAME[Opcode_sub]  = "sub";
    OPCODE_TO_NAME[Opcode_not]  = "not";
    OPCODE_TO_NAME[Opcode_clr]  = "clr";
    OPCODE_TO_NAME[Opcode_lea]  = "lea";
    OPCODE_TO_NAME[Opcode_inc]  = "inc";
    OPCODE_TO_NAME[Opcode_dec]  = "dec";
    OPCODE_TO_NAME[Opcode_jmp]  = "jmp";
    OPCODE_TO_NAME[Opcode_bne]  = "bne";
    OPCODE_TO_NAME[Opcode_red]  = "red";
    OPCODE_TO_NAME[Opcode_prn]  = "prn";
    OPCODE_TO_NAME[Opcode_jsr]  = "jsr";
    OPCODE_TO_NAME[Opcode_rts]  = "rts";
    OPCODE_TO_NAME[Opcode_stop] = "stop";
}

void initHandlerMap()
{
    void fillMovOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillCmpOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillAddOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillSubOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillNotOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillClrOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillLeaOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillIncOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillDecOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillJmpOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillBneOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillRedOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillPrnOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillJsrOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillRtsOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    void fillStopOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);
    
    if (OPCODE_TO_HANDLER[0] != NULL) return;
    
    OPCODE_TO_HANDLER[Opcode_mov]  = fillMovOpcode; 
    OPCODE_TO_HANDLER[Opcode_cmp]  = fillCmpOpcode;
    OPCODE_TO_HANDLER[Opcode_add]  = fillAddOpcode;
    OPCODE_TO_HANDLER[Opcode_sub]  = fillSubOpcode;
    OPCODE_TO_HANDLER[Opcode_not]  = fillNotOpcode;
    OPCODE_TO_HANDLER[Opcode_clr]  = fillClrOpcode;
    OPCODE_TO_HANDLER[Opcode_lea]  = fillLeaOpcode;
    OPCODE_TO_HANDLER[Opcode_inc]  = fillIncOpcode;
    OPCODE_TO_HANDLER[Opcode_dec]  = fillDecOpcode;
    OPCODE_TO_HANDLER[Opcode_jmp]  = fillJmpOpcode;
    OPCODE_TO_HANDLER[Opcode_bne]  = fillBneOpcode;
    OPCODE_TO_HANDLER[Opcode_red]  = fillRedOpcode;
    OPCODE_TO_HANDLER[Opcode_prn]  = fillPrnOpcode;
    OPCODE_TO_HANDLER[Opcode_jsr]  = fillJsrOpcode;
    OPCODE_TO_HANDLER[Opcode_rts]  = fillRtsOpcode;
    OPCODE_TO_HANDLER[Opcode_stop] = fillStopOpcode;
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

void setSourceLineError(SourceLinePtr sourceLine, char *error, ...)
{
    char buffer[MESSAGE_BUFFER_LENGTH];
    va_list ap;
    va_start(ap, error);
    vsprintf(buffer, error, ap);
    va_end(ap);
    
    sourceLine->error = cloneString(buffer, strlen(buffer));
}

char *getOpcodeNameToken(SourceLinePtr sourceLine)
{
    char *separator;
    char *name;
    
    separator = strchr(sourceLine->text, OPCODE_CONTROL_PARAMETER_SEPARATOR);
    
    if (separator == NULL)
    {
        return NULL;
    }
    
    name = cloneString(sourceLine->text, separator - sourceLine->text);
    
    return name;
}

Boolean tryReadOpcode(SourceLinePtr sourceLine, Opcode *opcode)
{
    int i;
    char *separator;
    char *name;
    Boolean found = False;
    
    initNameMap();
    
    separator = strchr(sourceLine->text, OPCODE_CONTROL_PARAMETER_SEPARATOR);
    if (separator == NULL)
    {
        setSourceLineError(sourceLine, "Missing opcode and opcode parameter separator '%c'.", OPCODE_CONTROL_PARAMETER_SEPARATOR);
        return False;
    }
    
    /* the opcode parameter separator is assumed to come right after the '/' */
    
    name = getOpcodeNameToken(sourceLine);
    
    for (i = 0; i < sizeof(OPCODE_TO_NAME) / sizeof(char *); i++)
    {
        if (strcmp(name, OPCODE_TO_NAME[i]) == 0)
        {
            *opcode = i;
            sourceLine->text = separator;
            found = True;
            break;
        }        
    }
    
    free(name);
    
    return found;
}

char* getOpcodeName(Opcode opcode)
{
    int length;
    char *name;
    
    /* at this stage the opcode was already checked for existence */
    
    initNameMap();
    length = strlen(OPCODE_TO_NAME[opcode]) + 1;
    name = cloneString(OPCODE_TO_NAME[opcode], length);
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

/* returns a pointer to the first OPERAND_SEPERATOR or EOL. 
 * never NULL because EOL is always present.*/
char *getOperandTokenEnd(SourceLinePtr sourceLine)
{
    char *start = sourceLine->text;
    char *end = strchr(start, OPERAND_SEPERATOR);
    
    if (end == NULL)
    {
        end = strchr(start, EOL);
    }
    
    end--;
    
    while (end > start && isspace(*end)) end--;
    
    /* end now points to the last char of the label */
    end++;
    
    return end;
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
    operand->empty = False;
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
    operand->empty = False;
}

void readDirectAddressingOperand(SourceLinePtr sourceLine, OperandPtr operand)
{
    char *getOperandTokenEnd(SourceLinePtr sourceLine);
    char *start, *end, *label;
    
    start = sourceLine->text;
    end = getOperandTokenEnd(sourceLine);
    
    if (!isValidLabel(sourceLine, start, end))
    {
        setSourceLineError(sourceLine, "Invalid label '%s' for direct addressing operand.", start);
        return;
    }
    
    label = tryReadLabel(sourceLine);
    
    operand->addressing =  OperandAddressing_Direct;
    operand->address.label = label;
    operand->empty = False;
}


void readVaryingAddressingOperand(SourceLinePtr sourceLine, OperandPtr operand)
{
    char *start, *opening, *closing;
    char *label;
    char *originalPosition;
    int length;
    int registerId;
    int offset;
    char *offsetStr;
    
    start = sourceLine->text;

    opening = strchr(start, VARYING_INDEXING_OPENING_TOKEN);
    if (opening == NULL)
    {
        setSourceLineError(sourceLine, "Missing '%c' token from varying address operand.", VARYING_INDEXING_OPENING_TOKEN);
        return;
    }
    
    closing = strchr(start, VARYING_INDEXING_CLOSING_TOKEN);
    
    if (closing == NULL)
    {
        setSourceLineError(sourceLine, "Missing '%c' token from varying address operand.", VARYING_INDEXING_CLOSING_TOKEN);
        return;
    }
    
    label = cloneString(start, opening - start);
    
    start = opening + VARYING_INDEXING_OPENING_TOKEN_LENGTH;
    
    if (*start == VARYING_INDEXING_LABEL_TOKEN)
    {
        start +=  VARYING_INDEXING_LABEL_TOKEN_LENGTH;
        if (!isValidLabel(sourceLine, start, closing - 1))
        {
            setSourceLineError(sourceLine, "Invalid label name for labeled varying indexing.");
            return;
        }
        
        length = closing - start;
        
        operand->address.varyingAddress.adressing = OperandVaryingAddressing_Direct;
        operand->address.varyingAddress.address.label = cloneString(start, length);
        goto postProcess;
    }
    
    if (*start == REGISTER_NAME_PREFIX && closing - start == REGISTER_NAME_LENGTH)
    {
        start += REGISTER_NAME_PREFIX_LENGTH;
        sourceLine->text = start;
        
        if (!tryReadNumber(sourceLine, &registerId) || !IS_VALID_REGISTER_ID(registerId))
        {
            setSourceLineError(sourceLine, "Invalid register id.");
            return;
        }
        
        operand->address.varyingAddress.adressing = OperandVaryingAddressing_DirectRegister;
        operand->address.varyingAddress.address.reg[0] = REGISTER_NAME_PREFIX;
        operand->address.varyingAddress.address.reg[1] = registerId + '0';
        operand->address.varyingAddress.address.reg[2] = EOL;
        goto postProcess;
    }
    
    originalPosition = sourceLine->text;
    
    sourceLine->text = start;
    
    if (!tryReadNumber(sourceLine, &offset))
    {
        offsetStr = cloneString(start, closing - start);
        setSourceLineError(sourceLine, "Invalid offset value '%s' for varying index %s.", offsetStr, label);
        /* restore the cursor to its original position */
        sourceLine->text = originalPosition;
        return;
    }
    
    operand->address.varyingAddress.adressing = OperandVaryingAddressing_Instant;
    operand->address.varyingAddress.address.value = offset;
    
    
    
postProcess:
    operand->addressing = OperandAddressing_VaryingIndexing;
    operand->address.label = label;
    operand->empty = False;
    sourceLine->text = closing + VARYING_INDEXING_CLOSING_TOKEN_LENGTH;
    
}

Boolean readSourceOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing, InstructionLayoutPtr instruction)
{
    Boolean readOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing, OperandPtr operand);
    skipWhitespace(sourceLine);
        
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
    char *message;
    
    skipWhitespace(sourceLine);
    
    operandEnd = strchr(sourceLine->text, EOL);
    
    if (operandEnd == NULL)
    {
        setSourceLineError(sourceLine, "Missing EOL?!");
        return False;
    }

    if (!readOperand(sourceLine, validAddressing, &instruction->rightOperand))
    {
        return False;
    }
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text != EOL)
    {
        message = "There should be no text after the second operand.";
        
        if (strchr(sourceLine->text, VARYING_INDEXING_OPENING_TOKEN) == NULL)
        {
            setSourceLineError(sourceLine, "%s", message);
        }
        else
        {
            setSourceLineError(sourceLine, "%s There should be no white space between the label and the index token '%c'.", message, VARYING_INDEXING_OPENING_TOKEN);
        }
        return False;
    }
    
    return True;
}

/* tells if the current operand addressing is on. */
/* could be written with a macro but because we are dealing 
 * with enums a functions looks more appropriate. */
Boolean addressingTypeIsAllowed(ValidOperandAddressing addressingMask, ValidOperandAddressing allowedAddressing)
{
    if (allowedAddressing == ValidOperandAddressing_None)
    {
        return addressingMask == ValidOperandAddressing_None;
    }
    
    return (addressingMask & allowedAddressing) == allowedAddressing;
}

Boolean readOperand(SourceLinePtr sourceLine, ValidOperandAddressing validAddressing, OperandPtr operand)
{
    char *start, *end, *openingBracket, *closingBracket;
    
    skipWhitespace(sourceLine);

    if (addressingTypeIsAllowed(validAddressing, ValidOperandAddressing_None))
    {
        if ((*sourceLine->text) == EOL)
        {
#ifdef DEBUG
            printf("No operands.\n");
#endif
            return True;
        }
        else
        {
            setSourceLineError(sourceLine, "Opcode should have no operands.");
            return False;
        }
    }
    
    if (addressingTypeIsAllowed(validAddressing, ValidOperandAddressing_Instant))
    {
        if ((*sourceLine->text) == INSTANT_ADDRESSING_OPERAND_PREFIX)
        {
            readInstantAddressingOperand(sourceLine, operand);
#ifdef DEBUG
            printf("Found an instant address operand with the value of %ld\n", operand->address.value);
#endif
            return sourceLine->error == NULL;
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
            return sourceLine->error == NULL;
        }
    }
    
    if (addressingTypeIsAllowed(validAddressing, ValidOperandAddressing_VaryingIndexing))
    {
        start = sourceLine->text;
        
        end = getOperandTokenEnd(sourceLine);
        
        openingBracket = strchr(start, VARYING_INDEXING_OPENING_TOKEN);
        closingBracket = strchr(start, VARYING_INDEXING_CLOSING_TOKEN);
        
        if (openingBracket != NULL && 
            closingBracket != NULL && 
            openingBracket < closingBracket && 
            closingBracket <= end)
        {
            readVaryingAddressingOperand(sourceLine, operand);
#ifdef DEBUG
            
            switch (operand->address.varyingAddress.adressing)
            {
                case OperandVaryingAddressing_Direct:
                    printf("Found varying addressing operand %s with direct address %s.\n", operand->address.varyingAddress.label, operand->address.varyingAddress.address.label);
                    break;
                case OperandVaryingAddressing_DirectRegister:
                    printf("Found varying addressing operand %s with direct register %s.\n", operand->address.varyingAddress.label, operand->address.varyingAddress.address.reg);
                    break;
                case OperandVaryingAddressing_Instant:
                    printf("Found varying addressing operand %s with offset %ld.\n", operand->address.varyingAddress.label, operand->address.varyingAddress.address.value);
                    break;
                default:
                    printf("Unknown OperandVaryingAddressing value!\n");
            }
            
            
#endif
            return sourceLine->error == NULL;
        }
    }
    
    if (addressingTypeIsAllowed(validAddressing, ValidOperandAddressing_Direct))
    {
        start = sourceLine->text;
        end = getOperandTokenEnd(sourceLine);
        if (isValidLabel(sourceLine, start, end))
        {
            readDirectAddressingOperand(sourceLine, operand);
#ifdef DEBUG
            printf("Found direct addressing operand with the label %s.\n", operand->address.label);
#endif
            return sourceLine->error == NULL;
        }
    }
    
    setSourceLineError(sourceLine, "Unknown operand addressing scheme.\n");
    return False;
}

Boolean setComb(SourceLinePtr sourceLine, OpcodeLayoutPtr instructionRepresentation)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    OperandTargetBits sourceOperandTargetBits;
    OperandTargetBits targetOperandTargetBits;
 
    /* if the operand size is small (10 bits instead of 20) we need to set 
     the comb bits to tell how to address each operand */
    
    if (instructionRepresentation->type == InstructionOperandSize_Large)
    {
        /* the instruction operates on the entire word size so 
           there is no need to set cmb */
        instructionRepresentation->comb = 0;
        return True;
    }
    
    if ((*sourceLine->text) == OPCODE_CONTROL_PARAMETER_SEPARATOR)
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
    
    if ((*sourceLine->text) != OPCODE_CONTROL_PARAMETER_SEPARATOR)
    {
        setSourceLineError(sourceLine, "'%c' required between the operand bit sizes to use.", OPCODE_CONTROL_PARAMETER_SEPARATOR);
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

Boolean readInstructionModifiers(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    Boolean setComb(SourceLinePtr sourceLine, OpcodeLayoutPtr opcodeLayout);
    
    if (!readInstructionOperandSize(sourceLine, &instructionRepresentation->opcode))
    {
        return False;
    }
    
    if (!setComb(sourceLine, &instructionRepresentation->opcode))
    {
        return False;
    }
    
    if (*sourceLine->text != OPCODE_TYPE_AND_DBL_SEPERATOR)
    {
        setSourceLineError(sourceLine, "Missing '%c' character between operand type and dbl.", OPCODE_TYPE_AND_DBL_SEPERATOR);
        return False;
    }
    
    sourceLine->text += OPCODE_TYPE_AND_DBL_SEPERATOR_LENGTH;
    
    if (!readInstructionRepetition(sourceLine, &instructionRepresentation->opcode))
    {
        return False;
    }
    
    return True;
}

Boolean hasSpaceBetweenOpcodeAndOperands(SourceLinePtr sourceLine)
{
    return isspace(*sourceLine->text);
}

void readNoOperandOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    if (!readInstructionModifiers(sourceLine, instructionRepresentation))
    {
        return;
    }
    
    /* if the ocode is the last opcode in the file it will have no new line mark */
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text != EOL)
    {
        setSourceLineError(sourceLine, "This opcode should have no operands.");
    }
#ifdef DEBUG    
    else
    {
        printf("Opcode with no operands.\n");
    }
#endif
}


void readUnaryOperandOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation, ValidOperandAddressing validAddressing)
{
    if (!readInstructionModifiers(sourceLine, instructionRepresentation))
    {
        return;
    }
    
    if (!hasSpaceBetweenOpcodeAndOperands(sourceLine))
    {
        setSourceLineError(sourceLine, "Missing space between opcode and operand.");
        return;
    }
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text == EOL)
    {
        setSourceLineError(sourceLine, "Missing operand.");
        return;
    }
    
    readDestinationOperand(sourceLine, validAddressing, instructionRepresentation);
}

void readBinaryOperandOpcode(SourceLinePtr sourceLine, 
        InstructionLayoutPtr instructionRepresentation, 
        ValidOperandAddressing validSourceAddressing, 
        ValidOperandAddressing validDestinationAddressing)
{
    if (!readInstructionModifiers(sourceLine, instructionRepresentation))
    {
        return;
    }
    
    if (!hasSpaceBetweenOpcodeAndOperands(sourceLine))
    {
        setSourceLineError(sourceLine, "Missing space between opcode and operand.");
        return;
    }
    
    skipWhitespace(sourceLine);
    
    readSourceOperand(sourceLine, validSourceAddressing, instructionRepresentation);
    
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
    instructionRepresentation->opcode.opcode = Opcode_mov;
    
    readBinaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_All, ValidOperandAddressing_AllExceptInstant);
}

void fillCmpOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_cmp;
    
    readBinaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_All, ValidOperandAddressing_All);
}

void fillAddOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    ValidOperandAddressing validAddressing = ValidOperandAddressing_AllExceptInstant;
            
    instructionRepresentation->opcode.opcode = Opcode_add;
    
    readBinaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_All, validAddressing);
}

void fillSubOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    ValidOperandAddressing validAddressing = ValidOperandAddressing_AllExceptInstant;
            
    instructionRepresentation->opcode.opcode = Opcode_sub;
    
    readBinaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_All, validAddressing);
}

void fillNotOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_not;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_AllExceptInstant);
}

void fillClrOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_clr;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_AllExceptInstant);
}

void fillLeaOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    ValidOperandAddressing validAddressing = ValidOperandAddressing_AllExceptInstant;
    
    instructionRepresentation->opcode.opcode = Opcode_lea;
    
    readBinaryOperandOpcode(sourceLine, instructionRepresentation, validAddressing, validAddressing);
}

void fillIncOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_inc;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_AllExceptInstant);
}

void fillDecOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_dec;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_AllExceptInstant);
}

void fillJmpOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_jmp;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_AllExceptInstant);
}

void fillBneOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_bne;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_AllExceptInstant);
}

void fillRedOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_red;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_AllExceptInstant);
}

void fillPrnOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_prn;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_All);
}

void fillJsrOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_jsr;
    
    readUnaryOperandOpcode(sourceLine, instructionRepresentation, ValidOperandAddressing_Direct);
}

void fillRtsOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_rts;
    
    readNoOperandOpcode(sourceLine, instructionRepresentation);
}

void fillStopOpcode(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation)
{
    instructionRepresentation->opcode.opcode = Opcode_stop;
    
    readNoOperandOpcode(sourceLine, instructionRepresentation);
}