#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "consts.h"
#include "parser.h"

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
    
    return line;
}

void freeSourceLine(SourceLine *line)
{
    free(line->text);
    free(line->fileName);
}

void logParsingError(char *errorText, SourceLine *line)
{
    fprintf(stderr, "Error parsing file '%s' line %d: %s\n", line->fileName, line->lineNumber, errorText);
}

char *skipWhitespace(char *sourceLine)
{
    while (isspace(*sourceLine)) sourceLine++;
    return sourceLine;
}

char *getLabel(SourceLine *sourceLine)
{
    char *label;
    Boolean valid = True;
    char *line = sourceLine->text;
    char *labelEnd = strchr(line, LABEL_TOKEN);
    int length;
    
    if (labelEnd == NULL) return NULL;
    length = labelEnd - line;
    
    /* validate label */
    label = line;
    if (!isalpha(*label))
    {
        logParsingError("Label does not start with a letter:", sourceLine);
        valid = False;
    }
    
    for (; label != labelEnd; label++)
    {
        if (!isalnum(*label))
        {
            logParsingError("Label contains non alphanumeric characters", sourceLine);
            valid = False;
        }
    }
    
    if (!valid) return NULL;
    
    label = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(label, sourceLine->text, length);
    label[length] = '\0';
    
    return label;
}

Boolean isBlankLine(char *sourceCodeLine)
{
    char *str = skipWhitespace(sourceCodeLine);
    return *str == EOL ? True : False;
}

Boolean isCommentLine(char *sourceCodeLine)
{
    return *sourceCodeLine == COMMENT_TOKEN ? True : False;
}