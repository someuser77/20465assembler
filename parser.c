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

void logParsingErrorFormat(SourceLinePtr line, char *error, ...)
{
    va_list ap;
    fprintf(stderr, "Error parsing file '%s' line %d pos %d: ", line->fileName, line->lineNumber, (int)(line->text - line->start + 1));
    va_start(ap, error);
    vfprintf(stderr, error, ap);
    va_end(ap);
    if (line->error != NULL)
    {
        fprintf(stderr, "%s\n", line->error);
    }
}

void logParsingError(char *error, SourceLine *line)
{
    logParsingErrorFormat(line, error);
}

Boolean firstPass(FILE *sourceFile, SymbolTablePtr symbolTable, char *sourceFileName)
{
    char *tryReadLabel(SourceLine *sourceLine);
    char buffer[MAX_CODE_LINE_LENGTH + 1] = {0};
    int dataCounter = 0;
    int instructionCounter = 0;
    SourceLine line;
    SourceLinePtr linePtr = &line;
    char *bufferPos;
    char *label;
    Boolean foundSymbol;
    GuidanceType guidanceType;
    Opcode opcode;
    InstructionLayoutPtr instructionLayout;
    
    int lineNumber = 0;
    
    while (fgets(buffer, MAX_CODE_LINE_LENGTH, sourceFile) != NULL)
    {
        bufferPos = buffer;
        lineNumber++;
#ifdef DEBUG
        printf("\n%d: %s\n", lineNumber, buffer);
#endif
        line = initSourceLine(bufferPos, lineNumber, sourceFileName);
        
        if (isBlankLine(linePtr))
        {
            /* blank line */
#ifdef DEBUG
            printf("\n(Blank Line)\n");
#endif
            continue;
        }
        
        skipWhitespace(linePtr);
        
        if (isCommentLine(linePtr))
        {
            /* comment line */
#ifdef DEBUG
            printf("\n(Comment Line)\n");
#endif
            continue;
        }
        
        /* if we'll use the regular tryReadtoken it will fail the label validity check because of the ':' */
        label = tryReadLabelWithEndToken(linePtr, LABEL_TOKEN);
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
        
        skipWhitespace(linePtr);
        
        if (*linePtr->text == GUIDANCE_TOLEN)
        {            
            if (tryGetGuidanceType(linePtr, &guidanceType))
            {
                switch (guidanceType)
                {
                    case GuidanceType_Data:  
                        insertSymbol(symbolTable, label, SymbolType_Data, dataCounter);
                        /* insert into memory */
                        continue;
                    case GuidanceType_String:
                        insertSymbol(symbolTable, label, SymbolType_Data, dataCounter);
                        /* insert into memory */
                        continue;
                    case GuidanceType_Entry:
                        insertSymbol(symbolTable, label, SymbolType_Code, instructionCounter);
                        continue;
                    case GuidanceType_Extern:
                        insertSymbol(symbolTable, label, SymbolType_Code, EMPTY_SYMBOL_VALUE);
                        continue;
                }                
            }
        }
        
        if (foundSymbol)
        {
            insertSymbol(symbolTable, label, SymbolType_Code, instructionCounter);
        }
        
        if (!tryReadOpcode(linePtr, &opcode))
        {
            logParsingError("Unrecognized opcode.", linePtr);
            continue;
        }
        
        if (*linePtr->text != OPCODE_CONTROL_PARAMETER_SEPERATOR)
        {
            logParsingErrorFormat(linePtr, "Missing seperator '%c' after opcode", OPCODE_CONTROL_PARAMETER_SEPERATOR);
            continue;
        }
        
        linePtr->text += OPCODE_CONTROL_PARAMETER_SEPERATOR_LENGTH;
        
        instructionLayout = getInstructionLayout(linePtr, opcode);
        
        if (instructionLayout == NULL)
        {
            logParsingError("Unable to parse opcode.", linePtr);
            continue;
        }
        
        if (linePtr->error != NULL)
        {
            logParsingError("Error parsing line.", linePtr);
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
    char *start = sourceLine->text, *end;
    char *last;
    int i;
    
    for (i = 0; i < sizeof(tokens) / sizeof(char); i++)
    {
        end = strchr(start, tokens[i]);
        if (end != NULL)
        {
            break;
        }
    }
    
    last = end;
    while (last > start && isspace(*last)) last--;
    
    sourceLine->text = last;
    
    return cloneString(start, last - start);
}

Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd)
{
    Boolean valid = True;
    int length;
    int i;
    char *label = labelStart;
    char msg[MESSAGE_BUFFER_LENGTH] = {0};
    
    if (!isalpha(*labelStart))
    {
        logParsingError("Label must start with a letter: ", sourceLine);
        valid = False;
    }
    
    length = labelEnd - labelStart;
    
    if (length > MAX_LABEL_LENGTH)
    {
        sprintf(msg, "Label must be %d chars or less", MAX_LABEL_LENGTH);
        logParsingError(msg, sourceLine);
        valid = False;
    }
    
    for (i = 0; i < length; i++)
    {
        if (!isalnum(label[i]))
        {
            logParsingError("Label contains non alphanumeric characters", sourceLine);
            valid = False;
            break;
        }
    }

    if (length == REGISTER_NAME_LENGTH)
    {
        if (label[0] == REGISTER_NAME_PREFIX && IS_VALID_REGISTER_ID(label[1] - '0'))
        {
            logParsingError("Label must not be a valid register name", sourceLine);
            valid = False;
        }
    }
    
    if (isValidOpcodeName(label))
    {
        logParsingError("Label must not be a valid opcode name", sourceLine);
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
    if (strncmp(sourceLine->text, DATA_GUIDANCE_TOKEN, DATA_GUIDANCE_TOKEN_LENGTH) == 0)
    {
        *guidanceType = GuidanceType_Data;
        return True;
    }
    
    if (strncmp(sourceLine->text, STRING_GUIDANCE_TOKEN, STRING_GUIDANCE_TOKEN_LENGTH) == 0)
    {
        *guidanceType = GuidanceType_String;
        return True;
    }
    
    return False;
}


InstructionLayoutPtr getInstructionLayout(SourceLinePtr sourceLine, Opcode opcode)
{
    Boolean getInstructionOperandSize(SourceLinePtr sourceLine, OpcodeLayoutPtr instruction);
    Boolean getInstructionRepetition(SourceLinePtr sourceLine, OpcodeLayoutPtr instruction);
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    char *opcodeNamePtr;
    char opcodeName[OPCODE_NAME_LENGTH + 1] = {0};
    int opcodeLength;
    InstructionLayoutPtr result;
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
    
    result = (InstructionLayoutPtr)malloc(sizeof(InstructionLayout));
    memset(result, 0, sizeof(InstructionRepetition));
    
    
    /* the parameters of the command (type, operand size, dbl) are assumed to be 
       consecutive without spaces between them as seen on the course forum */
    
    handler = getOpcodeHandler(opcode);
    
    if (handler == NULL)
    {
        setSourceLineError(sourceLine, "No handler found for opcode '%s'.", opcodeName);
        return NULL;
    }
    
    (*handler)(sourceLine, result);
    
    return result;
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
    
    if (*numberEnd == '-') numberEnd++;
    
    while (isdigit(*numberEnd)) numberEnd++;
    
    if (numberEnd == numberStart || (numberStart + 1 == numberEnd && *numberEnd == '-'))
    {
        setSourceLineError(sourceLine, "Unable to find number in line.");
        return False;
    }
    
    length = numberEnd - numberStart;
            
    buffer = (char *)malloc(sizeof(char) * (length + 1));
    
    strncpy(buffer, numberStart, length);
    
    buffer[length] = EOL;
    
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