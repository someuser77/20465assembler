#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "types.h"
#include "consts.h"
#include "parser.h"
#include "opcodemap.h"

#define REGISTER_NAME_LENGTH 2


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
    free(line->error);
}

void logParsingError(char *errorText, SourceLine *line)
{
    fprintf(stderr, "Error parsing file '%s' line %d: %s\n", line->fileName, line->lineNumber, errorText);
    if (line->error != NULL)
    {
        fprintf(stderr, "%s\n", line->error);
    }
}

Boolean firstPass(FILE *sourceFile, SymbolTablePtr symbolTable, char *sourceFileName)
{
    char buffer[LINE_BUFFER_LENGTH + 1] = {0};
    int dataCounter = 0;
    int instructionCounter = 0;
    SourceLine line;
    SourceLinePtr linePtr = &line;
    char *bufferPos;
    char *label;
    Boolean foundSymbol;
    GuidanceType guidanceType;
    Opcode opcode;
    InstructionRepresentationPtr instructionRepresentation;
    
    int lineNumber = 0;
    
    while (fgets(buffer, LINE_BUFFER_LENGTH, sourceFile) != NULL)
    {
        bufferPos = buffer;
        puts(bufferPos);
        
        
        line = initSourceLine(bufferPos, ++lineNumber, sourceFileName);
        
        if (isBlankLine(linePtr))
        {
            /* blank line */
            continue;
        }
        
        skipWhitespace(linePtr);
        
        if (isCommentLine(linePtr))
        {
            /* comment line */
            continue;
        }

        label = getLabel(linePtr);
        if (label != NULL)
        {
            printf("Label: %s\n", label);
            linePtr->text += strlen(label);
            linePtr->text += 1; /* skip the ':' */
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
                        break;
                    case GuidanceType_String:
                        insertSymbol(symbolTable, label, SymbolType_Data, dataCounter);
                        /* insert into memory */
                        continue;
                        break;
                    case GuidanceType_Entry:
                        insertSymbol(symbolTable, label, SymbolType_Code, instructionCounter);
                        continue;
                        break;
                    case GuidanceType_Extern:
                        insertSymbol(symbolTable, label, SymbolType_Code, EMPTY_SYMBOL_VALUE);
                        continue;
                        break;
                }                
            }
        }
        
        if (foundSymbol)
        {
            insertSymbol(symbolTable, label, SymbolType_Code, instructionCounter);
        }
        
        if (!tryGetOpcode(linePtr, &opcode))
        {
            logParsingError("Unrecognized opcode.", linePtr);
            return False;
        }
        
        instructionRepresentation = getInstructionRepresentation(linePtr, opcode);
        
        if (instructionRepresentation == NULL)
        {
            logParsingError("Unable to parse instruction.", linePtr);
            return False;
        }
    }
    
    return True;
}

void skipWhitespace(SourceLine *sourceLine)
{
    while (*sourceLine->text != EOL && isspace(*sourceLine->text)) sourceLine->text++;
}

char *skipWhitespaceInString(char *str)
{
    while (*str != EOL && isspace(*str)) str++;
    return str;
}

char *getLabel(SourceLine *sourceLine)
{
    char *label;
    char *line = sourceLine->text;
    char *labelEnd = strchr(line, LABEL_TOKEN);
    int length;
    Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd);
    
    if (labelEnd == NULL) return NULL;
    
    label = line;
    
    if (!isValidLabel(sourceLine, label, labelEnd)) return NULL;
    length = labelEnd - line;
    label = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(label, sourceLine->text, length);
    label[length] = EOL;
    
    return label;
}

Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd)
{
    Boolean valid = True;
    int length;
    int i;
    char *label = sourceLine->text;
    char msg[MESSAGE_BUFFER_LENGTH] = {0};
    
    if (!isalpha(*label))
    {
        logParsingError("Label must start with a letter:", sourceLine);
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
        if (label[0] == REGISTER_PREFIX && ((label[1] - '0') >= MIN_REGISTER_ID && (label[1] - '0') <= MAX_REGISTER_ID))
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


InstructionRepresentationPtr getInstructionRepresentation(SourceLinePtr sourceLine, Opcode opcode)
{
    Boolean getInstructionOperandSize(SourceLinePtr sourceLine, InstructionRepresentationPtr instruction);
    Boolean getInstructionRepetition(SourceLinePtr sourceLine, InstructionRepresentationPtr instruction);
    void setSourceLineError(SourceLinePtr sourceLine, char *error, ...);
    char *opcodeNamePtr;
    char opcodeName[OPCODE_NAME_LENGTH + 1] = {0};
    int opcodeLength;
    InstructionRepresentationPtr result;
    OpcodeHandler handler;
    
    opcodeNamePtr = getOpcodeName(opcode);
    strcpy(opcodeName, opcodeNamePtr);
    free(opcodeNamePtr);
    opcodeLength = strlen(opcodeName);
    
    /* if the text still points to the opcode skip it */
    if (strncmp(sourceLine->text, opcodeName, opcodeLength) == 0)
    {
        sourceLine->text += opcodeLength;
    }
    
    result = (InstructionRepresentationPtr)malloc(sizeof(InstructionRepresentation));
    memset(result, 0, sizeof(InstructionRepetition));
    
    
    /* the parameters of the command (type, operand size, dbl) are assumed to be 
       consecutive without spaces between them as seen on the course forum */
    
    handler = getOpcodeHandler(opcode);
    
    if (handler == NULL)
    {
        setSourceLineError(sourceLine, "No handler found for opcode %s", opcodeName);
        return NULL;
    }
    
    (*handler)(sourceLine, result);

    return result;
}
