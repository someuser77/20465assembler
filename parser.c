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
/*
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
*/
void readDataGuidance(SourceLinePtr line)
{
    
}

Boolean firstPass(FILE *sourceFile, SymbolTablePtr symbolTable, char *sourceFileName)
{
    char *tryReadLabel(SourceLine *sourceLine);
    char buffer[MAX_CODE_LINE_LENGTH + 1] = {0};
    int dataCounter = 0;
    int instructionCounter = 0;
    SourceLine line;
    SourceLinePtr sourceLine = &line;
    char *bufferPos;
    char *label;
    Boolean foundSymbol;
    GuidanceType guidanceType;
    Opcode opcode;
    InstructionLayoutPtr instructionLayout;
    
    DataSection dataSection;
    
    int lineNumber = 0;
    
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
#endif
                        if (label != NULL)
                        {
                                insertSymbol(symbolTable, label, SymbolType_Data, dataCounter);
                        }
                        
                        writeDataArray(&dataSection, sourceLine);
                        
                        continue;
                    case GuidanceType_String:
#ifdef DEBUG
                        printf("Found String.\n");
#endif
                        if (label != NULL)
                        {
                                insertSymbol(symbolTable, label, SymbolType_Data, dataCounter);
                        }
                        /* insert into memory */
                        continue;
                    case GuidanceType_Entry:
#ifdef DEBUG
                        printf("Found Entry.\n");
#endif
                        if (label != NULL)
                        {
                                insertSymbol(symbolTable, label, SymbolType_Code, instructionCounter);
                        }
                        continue;
                    case GuidanceType_Extern:
#ifdef DEBUG
                        printf("Found Extern.\n");
#endif
                        if (label != NULL)
                        {
                                insertSymbol(symbolTable, label, SymbolType_Code, EMPTY_SYMBOL_VALUE);
                        }
                        continue;
                }
            }
        }
        
        if (foundSymbol)
        {
            insertSymbol(symbolTable, label, SymbolType_Code, instructionCounter);
        }
        
        if (!tryReadOpcode(sourceLine, &opcode))
        {
            logParsingErrorFormat(sourceLine, "Unrecognized opcode.");
            continue;
        }
        
        if (*sourceLine->text != OPCODE_CONTROL_PARAMETER_SEPARATOR)
        {
            logParsingErrorFormat(sourceLine, "Missing seperator '%c' after opcode", OPCODE_CONTROL_PARAMETER_SEPARATOR);
            continue;
        }
        
        sourceLine->text += OPCODE_CONTROL_PARAMETER_SEPERATOR_LENGTH;
        
        instructionLayout = getInstructionLayout(sourceLine, opcode);
        
        if (instructionLayout == NULL)
        {
            logParsingError(sourceLine, "Unable to parse opcode.");
            continue;
        }
        
        if (sourceLine->error != NULL)
        {
            logParsingError(sourceLine, "Error parsing line.");
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
    
    if (!isalpha(*labelStart))
    {
        logParsingError(sourceLine, "Label must start with a letter: ");
        valid = False;
    }
    
    length = labelEnd - labelStart;
    
    if (length > MAX_LABEL_LENGTH)
    {
        logParsingErrorFormat(sourceLine, "Label must be %d chars or less", MAX_LABEL_LENGTH);
        valid = False;
    }
    
    for (i = 0; i < length; i++)
    {
        if (!isalnum(label[i]))
        {
            logParsingError(sourceLine, "Label contains non alphanumeric characters");
            valid = False;
            break;
        }
    }

    if (length == REGISTER_NAME_LENGTH)
    {
        if (label[0] == REGISTER_NAME_PREFIX && IS_VALID_REGISTER_ID(label[1] - '0'))
        {
            logParsingError(sourceLine, "Label must not be a valid register name");
            valid = False;
        }
    }
    
    if (isValidOpcodeName(label))
    {
        logParsingError(sourceLine, "Label must not be a valid opcode name");
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
    
end:
    free(guidance);
    return found;
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
    
    if (*numberEnd == '-' || *numberEnd == '+') numberEnd++;
    
    while (isdigit(*numberEnd)) numberEnd++;
    
    if (numberEnd == numberStart || (numberStart + 1 == numberEnd && *numberEnd == '-'))
    {
        setSourceLineError(sourceLine, "Unable to find number in line.");
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