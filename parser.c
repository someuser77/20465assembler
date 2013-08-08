#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include "types.h"
#include "consts.h"
#include "parser.h"
#include "opcodemap.h"
#include "logging.h"
#include "datasection.h"
#include "instructionqueue.h"

typedef enum {Pass_First, Pass_Second} Pass;

SourceLine initSourceLine(char *text, int lineNumber, char* fileName)
{
    SourceLine line;
    int length = strlen(text);
    line.text = (char*)malloc(sizeof(char) * (length + 1));
    strncpy(line.text, text, length+1);
    
    line.start = line.text;
    
    length = strlen(fileName);
    line.fileName = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(line.fileName, fileName, length);
    
    line.lineNumber = lineNumber;
    
    line.error = NULL;
    
    return line;
}

void freeSourceLine(SourceLine *line)
{
    free(line->text);
    free(line->fileName);
    /* calling free on NULL should be safe */
    free(line->error);
}

int getNumberOfWordsNeededForVaryingIndexing(VaryingAddress varyingAddress)
{
    int size = 1; /* for varying index label */
    
    switch (varyingAddress.adressing)
    {
        case OperandVaryingAddressing_Direct:
            size++;
            break;
        case OperandVaryingAddressing_Instant:
            size++;
            break;
        case OperandVaryingAddressing_DirectRegister:
            /* not necessary but makes it more clear */
            size += 0;
            break;
    }
    
    return size;
}

int getNumberOfWordsNeededForOperand(OperandPtr operand)
{
    int size = 0;
    
    if (operand->empty == False)
    {
        switch (operand->addressing)
        {
            case OperandAddressing_Direct:
                size++;
                break;
            case OperandAddressing_Instant:
                size++;
                break;
            case OperandAddressing_VaryingIndexing:
                size += getNumberOfWordsNeededForVaryingIndexing(operand->address.varyingAddress);
                break;
            case OperandAddressing_DirectRegister:
                /* not necessary but makes it more clear */
                size += 0;
                break;
        }
    }
    
    return size;
}

int getInstructionSizeInWords(InstructionLayoutPtr instruction)
{
    int size = 0;
    int sizeOfOpcode = 1;
    
    size += sizeOfOpcode;
    
    size += getNumberOfWordsNeededForOperand(&instruction->leftOperand);
    
    size += getNumberOfWordsNeededForOperand(&instruction->rightOperand);
    
    return size;
}

SymbolPtr handleExtern(SourceLinePtr sourceLine, SymbolTablePtr symbolTable)
{
    char *end;
    char *label;
    SymbolPtr symbol;
    Word externalSymbolValue = {EXTERN_SYMBOL_VALUE};
    
    skipWhitespace(sourceLine);
    
    /* we assume this is really an entry otherwise we would not be called */
    if (strncmp(sourceLine->text, EXTERN_GUIDANCE_TOKEN, EXTERN_GUIDANCE_TOKEN_LENGTH) == 0)
    {
        sourceLine->text += EXTERN_GUIDANCE_TOKEN_LENGTH;
    }

    skipWhitespace(sourceLine);
    
    end = sourceLine->text;
    
    while (!isspace(*end)) end++;
    
    label = cloneString(sourceLine->text, end - sourceLine->text);
    
    symbol = insertSymbol(symbolTable, label, SymbolSection_Code, externalSymbolValue);

    return symbol;
}

SymbolPtr handleEntry(SourceLinePtr sourceLine, SymbolTablePtr symbolTable)
{
    char *end;
    char *label;
    SymbolPtr symbol;

    skipWhitespace(sourceLine);
    
    /* we assume this is really an entry otherwise we would not be called */
    if (strncmp(sourceLine->text, ENTRY_GUIDANCE_TOKEN, ENTRY_GUIDANCE_TOKEN_LENGTH) == 0)
    {
        sourceLine->text += ENTRY_GUIDANCE_TOKEN_LENGTH;
    }

    skipWhitespace(sourceLine);
    
    end = sourceLine->text;
    
    while (!isspace(*end)) end++;
    
    label = cloneString(sourceLine->text, end - sourceLine->text);
    
    symbol = markEntry(symbolTable, label);

    return symbol;
}

int firstPass(FILE *sourceFile, CodeSection *codeSection, InstructionQueuePtr instructionQueue, DataSection *dataSection, char *sourceFileName)
{
    char *tryReadLabel(SourceLine *sourceLine);
    char buffer[MAX_CODE_LINE_LENGTH + 1] = {0};
    Word dataCounter = {0};
    Word instructionCounter;
    SymbolTablePtr symbolTable = codeSection->symbolTable;
    SourceLine line;
    SourceLinePtr sourceLine = &line;
    char *bufferPos;
    char *label;
    Boolean foundSymbol;
    GuidanceType guidanceType;
    Opcode opcode;
    InstructionLayoutPtr instructionLayout;
    Boolean successfulPass = True;
    int instructionSize;
    char *opcodeToken;
    int lineNumber = 0;

#ifdef DEBUG
    printf("\n\n === FIRST PASS === \n\n");
#endif
    
    instructionCounter = codeSection->codeBaseAddress;
    
    while (fgets(buffer, MAX_CODE_LINE_LENGTH, sourceFile) != NULL)
    {
        bufferPos = buffer;
        lineNumber++;
#ifdef DEBUG
        printf("\n%d: %s\n", lineNumber, buffer);
#endif
        line = initSourceLine(bufferPos, lineNumber, sourceFileName);
        
        if (isBlankLine(sourceLine))
        {
            /* blank line */
#ifdef DEBUG
            printf("\n(Blank Line)\n");
#endif
            continue;
        }
        
        skipWhitespace(sourceLine);
        
        if (isCommentLine(sourceLine))
        {
            /* comment line */
#ifdef DEBUG
            printf("\n(Comment Line)\n");
#endif
            continue;
        }
        
        /* if we'll use the regular tryReadtoken it will fail the label validity check because of the ':' */
        label = tryReadLabelWithEndToken(sourceLine, LABEL_TOKEN);
        if (label != NULL)
        {
#ifdef DEBUG
            printf("Label: %s\n", label);
#endif
            foundSymbol = True;
        } 
        else
        {
            foundSymbol = False;    
        }
        
        skipWhitespace(sourceLine);
        
        if (*sourceLine->text == GUIDANCE_TOLEN)
        {            
            if (tryGetGuidanceType(sourceLine, &guidanceType))
            {
                switch (guidanceType)
                {
                    case GuidanceType_Data:
#ifdef DEBUG
                        printf("Found Data.\n");
                        printf("Data Offset: %ld\n", dataCounter);
#endif
                        if (label != NULL)
                        {
                                if (insertSymbol(symbolTable, label, SymbolSection_Data, dataCounter) == NULL)
                                {
                                    logErrorInLine(sourceLine, "Found duplicate label: %s", label);
                                    successfulPass = False;
                                    goto end;
                                }
                        }
                        
                        if ((dataCounter = writeDataArray(dataSection, sourceLine)) < 0)
                        {
                            logError("Unable to write data array to memory.");
                            successfulPass = False;
                            goto end;
                        }
                        
                        continue;
                    case GuidanceType_String:
#ifdef DEBUG
                        printf("Found String.\n");
                        printf("Data Offset: %ld\n", dataCounter);
#endif
                        if (label != NULL)
                        {
                                if (insertSymbol(symbolTable, label, SymbolSection_Data, dataCounter) == NULL)
                                {
                                    logErrorInLine(sourceLine, "Found duplicate label: %s", label);
                                    successfulPass = False;
                                    goto end;
                                }
                        }
                        
                        if ((dataCounter = writeDataString(dataSection, sourceLine)) < 0)
                        {
                            logError("Unable to write string to memory.");
                            successfulPass = False;
                            goto end;
                        }
                        continue;
                    case GuidanceType_Entry:
                        /* entries will be dealt with on the second pass*/
                        continue;
                        
                    case GuidanceType_Extern:
#ifdef DEBUG
                        printf("Found Extern.\n");
#endif
                        handleExtern(sourceLine, symbolTable);
                        continue;
                }
            }
            else
            {
                logErrorInLine(sourceLine, "Unknown guidance type.");
                successfulPass = False;
                continue;
            }
        }
        
        if (foundSymbol)
        {
            insertSymbol(symbolTable, label, SymbolSection_Code, instructionCounter);
        }
        
        if (!tryReadOpcode(sourceLine, &opcode))
        {
            opcodeToken = getOpcodeNameToken(sourceLine);
            logErrorInLine(sourceLine, "Unrecognized opcode '%s'.", opcodeToken);
            free(opcodeToken);
            successfulPass = False;
            continue;
        }
        
        if (*sourceLine->text != OPCODE_CONTROL_PARAMETER_SEPARATOR)
        {
            logErrorInLine(sourceLine, "Missing separator '%c' after opcode", OPCODE_CONTROL_PARAMETER_SEPARATOR);
            successfulPass = False;
            continue;
        }
        
        sourceLine->text += OPCODE_CONTROL_PARAMETER_SEPERATOR_LENGTH;
        
        instructionLayout = getInstructionLayout(sourceLine, opcode);
        
        if (instructionLayout == NULL)
        {
            logErrorInLine(sourceLine, "Unable to parse opcode.");
            successfulPass = False;
            continue;
        }
        
        instructionLayout->instructionAddress = instructionCounter;
        
        insertInstruction(instructionQueue, instructionLayout);
        
        instructionSize = getInstructionSizeInWords(instructionLayout);
        
#ifdef DEBUG
        printf("Instruction size in words: %d\n", instructionSize);
        printf("Instruction Offset: %ld\n", instructionCounter);
#endif
        
        instructionCounter += instructionSize;
        
        if (sourceLine->error != NULL)
        {
            logErrorInLine(sourceLine, "Error parsing line.");
            successfulPass = False;
            continue;
        }
    }
    
#ifdef DEBUG
    printf("\nNext slot after last instruction: %ld\n", instructionCounter);
#endif
end:    
    return successfulPass ? instructionCounter : -1;
}

Boolean secondPass(FILE *sourceFile, CodeSection *codeSection, InstructionQueuePtr instructionQueue, char *sourceFileName)
{
    char *tryReadLabel(SourceLine *sourceLine);
    char buffer[MAX_CODE_LINE_LENGTH + 1] = {0};
    int instructionCounter = 0;
    SourceLine line;
    SourceLinePtr sourceLine = &line;
    char *bufferPos;
    GuidanceType guidanceType;
    Opcode opcode;
    InstructionLayoutPtr instructionLayout;
    SymbolTablePtr symbolTable = codeSection->symbolTable;
    
    int instructionSize;
    
    int lineNumber = 0;

#ifdef DEBUG
    printf("\n\n === SECOND PASS === \n\n");
#endif    

    while (fgets(buffer, MAX_CODE_LINE_LENGTH, sourceFile) != NULL)
    {
        bufferPos = buffer;
        lineNumber++;
#ifdef DEBUG
        printf("\n%d: %s\n", lineNumber, buffer);
#endif
        line = initSourceLine(bufferPos, lineNumber, sourceFileName);
        
        if (isBlankLine(sourceLine))
        {
            continue;
        }
        
        skipWhitespace(sourceLine);
        
        if (isCommentLine(sourceLine))
        {
            continue;
        }
        
        tryReadLabelWithEndToken(sourceLine, LABEL_TOKEN);
        
        skipWhitespace(sourceLine);
        
        if (*sourceLine->text == GUIDANCE_TOLEN)
        {            
            if (tryGetGuidanceType(sourceLine, &guidanceType))
            {
                if (guidanceType == GuidanceType_Entry)
                {
#ifdef DEBUG
                    printf("Found Entry.\n");
#endif
                    if (handleEntry(sourceLine, symbolTable) == NULL)
                    {
                            logErrorInLine(sourceLine, "Unable to find the label for the entry.");
                    }
                }
            }
            
            continue;
        }
        
        /* if there were invalid opcdoes this pass would not run */
        tryReadOpcode(sourceLine, &opcode);
        
        instructionLayout = getNextInstruction(instructionQueue);

#ifdef DEBUG            
        printf("Writing: %s\n", getOpcodeName(instructionLayout->opcode.opcode));
#endif
        
        if (writeInstruction(codeSection, instructionLayout, sourceLine) == -1)
        {
            logErrorInLine(sourceLine, "Error writing instruction %s.", getOpcodeName(instructionLayout->opcode.opcode));
            return False;
        }
        
        
        instructionSize = getInstructionSizeInWords(instructionLayout);
        
#ifdef DEBUG
        printf("Instruction size in words: %d\n", instructionSize);
#endif
        
        instructionCounter += instructionSize;
        
        if (sourceLine->error != NULL)
        {
            logErrorInLine(sourceLine, "Error parsing line.");
            continue;
        }
    }
    
    return True;
}

char *skipWhitespaceInString(char *str)
{
    while (*str != EOL && isspace(*str)) str++;
    return str;
}

void skipWhitespace(SourceLine *sourceLine)
{
    sourceLine->text = skipWhitespaceInString(sourceLine->text);
}

char *tryReadLabelWithEndToken(SourceLine *sourceLine, char token)
{
    char *label;
    char *start = sourceLine->text;
    char *end;
    int length;
    Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd);
    
    end = strchr(start, token);
    
    if (end == NULL) return NULL;
    
    label = start;
    
    if (!isValidLabel(sourceLine, label, end)) return NULL;
    
    length = end - start;
    
    label = cloneString(start, length);
    
    sourceLine->text += length;
    sourceLine->text += sizeof(token);
    return label;
}

/* tries to reads a label off the source line.
 * if there is no label NULL is returned.
 * if label is not valid the cursor is not moved. */
char *tryReadLabel(SourceLine *sourceLine)
{
    static char tokens[] = { OPERAND_SEPERATOR, EOL };
    char *start = sourceLine->text;
    char *end;
    char *last;
    char *label;
    int i;
    
    for (i = 0; i < sizeof(tokens) / sizeof(char); i++)
    {
        end = strchr(start, tokens[i]);
        if (end != NULL)
        {
            break;
        }
    }

    /* end points to the separator but there might be spaces 
     * between the label and the separators */
    end--;
    
    last = end;
    
    while (last > start && isspace(*last)) last--;
    
    /* last now points to the last char of the label */
    last++;
    
    label = cloneString(start, last - start);
    
    sourceLine->text = last;
    
    return label;
}

Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd)
{
    Boolean valid = True;
    int length;
    int i;
    char *label = labelStart;
    
    if (!isalpha(*labelStart))
    {
        logErrorInLine(sourceLine, "Label must start with a letter: ");
        valid = False;
    }
    
    length = labelEnd - labelStart;
    
    if (length > MAX_LABEL_LENGTH)
    {
        logErrorInLine(sourceLine, "Label must be %d chars or less", MAX_LABEL_LENGTH);
        valid = False;
    }
    
    for (i = 0; i < length; i++)
    {
        if (!isalnum(label[i]))
        {
            logErrorInLine(sourceLine, "Label contains non alphanumeric characters");
            valid = False;
            break;
        }
    }

    if (length == REGISTER_NAME_LENGTH)
    {
        if (label[0] == REGISTER_NAME_PREFIX && IS_VALID_REGISTER_ID(label[1] - '0'))
        {
            logErrorInLine(sourceLine, "Label must not be a valid register name");
            valid = False;
        }
    }
    
    if (isValidOpcodeName(label))
    {
        logErrorInLine(sourceLine, "Label must not be a valid opcode name");
        valid = False;
    }
    return valid;
}

Boolean isBlankLine(SourceLine *sourceLine)
{
    char *ptr = skipWhitespaceInString(sourceLine->text);
    return *ptr == EOL ? True : False;
}

Boolean isCommentLine(SourceLine *sourceLine)
{
    char *ptr = skipWhitespaceInString(sourceLine->text);
    return *ptr == COMMENT_TOKEN ? True : False;
}

Boolean isImaginaryGuidance(SourceLine *sourceLine)
{
    return (strncmp(sourceLine->text, DATA_GUIDANCE_TOKEN, DATA_GUIDANCE_TOKEN_LENGTH) == 0 || 
            strncmp(sourceLine->text, STRING_GUIDANCE_TOKEN, STRING_GUIDANCE_TOKEN_LENGTH) == 0) ? True : False;
}

Boolean tryGetGuidanceType(SourceLine *sourceLine, GuidanceType *guidanceType)
{
    char *end;
    char *guidance;
    Boolean found = False;
    
    end = sourceLine->text;
    
    while (!isspace(*end)) end++;
    
    guidance = cloneString(sourceLine->text, end - sourceLine->text);
    
    if (strcmp(guidance, DATA_GUIDANCE_TOKEN) == 0)
    {
        *guidanceType = GuidanceType_Data;
        found = True;
        goto end;
    }
    
    if (strcmp(guidance, STRING_GUIDANCE_TOKEN) == 0)
    {
        *guidanceType = GuidanceType_String;
        found = True;
        goto end;
    }
    
    if (strcmp(guidance, EXTERN_GUIDANCE_TOKEN) == 0)
    {
        *guidanceType = GuidanceType_Extern;
        found = True;
        goto end;
    }
    
    if (strcmp(guidance, ENTRY_GUIDANCE_TOKEN) == 0)
    {
        *guidanceType = GuidanceType_Entry;
        found = True;
        goto end;
    }
    
end:
    free(guidance);
    return found;
}

InstructionLayoutPtr initInstructionLayout()
{
    InstructionLayoutPtr result = (InstructionLayoutPtr)malloc(sizeof(InstructionLayout));
    if (result == NULL)
    {
        logError("malloc error.");
        return NULL;
    }
    memset(result, 0, sizeof(InstructionRepetition));
    result->leftOperand.empty = True;
    result->leftOperand.instruction = result;
    result->rightOperand.empty = True;
    result->rightOperand.instruction = result;
    
    return result;
}

InstructionLayoutPtr getInstructionLayout(SourceLinePtr sourceLine, Opcode opcode)
{
    Boolean getInstructionOperandSize(SourceLinePtr sourceLine, OpcodeLayoutPtr instruction);
    Boolean getInstructionRepetition(SourceLinePtr sourceLine, OpcodeLayoutPtr instruction);
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    char *opcodeNamePtr;
    char opcodeName[OPCODE_NAME_LENGTH + 1] = {0};
    int opcodeLength;
    InstructionLayoutPtr instruction;
    OpcodeHandler handler;
    
    opcodeNamePtr = getOpcodeName(opcode);
    strcpy(opcodeName, opcodeNamePtr);
    free(opcodeNamePtr);
    opcodeLength = strlen(opcodeName);
    
    /* if the text still points to the opcode skip it */
    if (strncmp(sourceLine->text, opcodeName, opcodeLength) == 0)
    {
        sourceLine->text += opcodeLength;
        sourceLine->text += OPCODE_CONTROL_PARAMETER_SEPERATOR_LENGTH;
    }
    
    instruction = initInstructionLayout();
    
    if (instruction == NULL) return NULL;
    
    /* the parameters of the command (type, operand size, dbl) are assumed to be 
       consecutive without spaces between them as seen on the course forum */
    
    handler = getOpcodeHandler(opcode);
    
    if (handler == NULL)
    {
        setSourceLineError(sourceLine, "No handler found for opcode '%s'.", opcodeName);
        return NULL;
    }
    
    (*handler)(sourceLine, instruction);

    
    if (instruction->leftOperand.empty == False)
    {
        instruction->opcode.source_addressing = instruction->leftOperand.addressing;
        
    }
    
    if (instruction->rightOperand.empty == False)
    {
        instruction->opcode.dest_addressing = instruction->rightOperand.addressing;
    }
    
    return instruction;
}

Boolean tryReadNumber(SourceLinePtr sourceLine, int *value)
{
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    char *numberStart;
    char *numberEnd;
    char *endptr;
    char *buffer;
    int length;
    char *line;
    
    line = sourceLine->text;
    
    skipWhitespaceInString(line);
    
    numberStart = line;
    
    numberEnd = numberStart;
    
    /* as the specification says a number starts with a sign followed by digits */
    if (*numberEnd == '-' || *numberEnd == '+') numberEnd++;
    
    while (isdigit(*numberEnd)) numberEnd++;
    
    if (numberEnd == numberStart || (numberStart + 1 == numberEnd && *numberEnd == '-'))
    {
        setSourceLineError(sourceLine, "Unable to find valid number in line.");
        return False;
    }
    
    length = numberEnd - numberStart;
    buffer = cloneString(numberStart, length);
    
    errno = 0;
    
    *value = strtol(buffer, &endptr, 10);
            
    if (errno == ERANGE)
    {
        setSourceLineError(sourceLine, "Number out of range.");
        free(buffer);
        return False;
    }
            
    if ((*value == 0 && endptr == buffer) || (errno != 0 && *value == 0))
    {
        setSourceLineError(sourceLine, "Unable to convert value to number.");
        free(buffer);
        return False;
    }
    
    sourceLine->text += length;
    free(buffer);
    return True;
}

char *cloneString(char *str, int length)
{
    char *result;
    
    result = (char *)malloc(sizeof(char) * (length + 1));
    
    if (result == NULL)
    {
        fprintf(stderr, "malloc error.");
        exit(EXIT_FAILURE);
    }
    
    strncpy(result, str, length);
    
    result[length] = EOL;
    
    return result;
}