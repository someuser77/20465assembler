#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "datasection.h"
#include "logging.h"
#include "parser.h"
#include "memory.h"



int writeDataArray(DataSection *dataSection, SourceLinePtr sourceLine)
{
    char *dataToken = cloneString(sourceLine->text, DATA_GUIDANCE_TOKEN_LENGTH);
    char charAfterToken = *(sourceLine->text + DATA_GUIDANCE_TOKEN_LENGTH);
    
    int num;
    int pos;
    
    if (strcmp(dataToken, DATA_GUIDANCE_TOKEN) != 0 || !isspace(charAfterToken))
    {
        logParsingError(sourceLine, "Tried to write data but no data token found.");
        free(dataToken);
        return DATA_WRITE_ERROR;
    }
    
    sourceLine->text += DATA_GUIDANCE_TOKEN_LENGTH;
    
    while (*sourceLine->text != EOL)
    {
        skipWhitespace(sourceLine);
        
        if (!tryReadNumber(sourceLine, &num))
        {
            logParsingError(sourceLine, "Unable to parse number.");
            break;
        }

#ifdef DEBUG
        printf("Found data value: %d\n", num);
#endif
        skipWhitespace(sourceLine);

        if (*sourceLine->text != EOL && *sourceLine->text != DATA_GUIDANCE_SEPARATOR)
        {
            logParsingErrorFormat(sourceLine, "Missing separator after value %d.", num);
            break;
        }
        
        if (*sourceLine->text == DATA_GUIDANCE_SEPARATOR)
        {
            sourceLine->text += DATA_GUIDANCE_SEPARATOR_LENGTH;
        }
        
        pos = writeInt(&dataSection->memory, num);
        if (pos == MEMORY_OUT_OF_MEMORY)
        {
            logError("Unable to write data. Out of memory.");
            return DATA_WRITE_ERROR;
        }
    }

    free(dataToken);
    return pos;
}

int writeDataString(DataSection *dataSection, SourceLinePtr sourceLine)
{
    char *stringToken = cloneString(sourceLine->text, STRING_GUIDANCE_TOKEN_LENGTH);
    char *start, *end;
    char value;
    int length;
    int i;
    int pos;
    
    if (strcmp(stringToken, STRING_GUIDANCE_TOKEN) != 0)
    {
        logParsingError(sourceLine, "Tried to write string but no string token found.");
        free(stringToken);
        return DATA_WRITE_ERROR;
    }
    
    free(stringToken);
    
    sourceLine->text += STRING_GUIDANCE_TOKEN_LENGTH;
    
    skipWhitespace(sourceLine);
    
    if (*sourceLine->text != '"')
    {
        logParsingError(sourceLine, "Expected '\"'.");
        
        return DATA_WRITE_ERROR;
    }
    
    start = sourceLine->text;
    
    end = strchr(start + 1, '"');
    
    if (end == NULL)
    {
        logParsingError(sourceLine, "Expected '\"' at end of string.");
        return DATA_WRITE_ERROR;
    }
    
    length = end - start - 1;
    
    for (i = 0; i < length; i++)
    {
        value = *sourceLine->text;
        pos = writeInt(&dataSection->memory, (int)value);
        if (pos == MEMORY_OUT_OF_MEMORY)
        {
            logParsingError(sourceLine, "Unable to write string, Out of memory.");
            return DATA_WRITE_ERROR;
        }
    }
    
    pos = writeInt(&dataSection->memory, 0);
    if (pos == MEMORY_OUT_OF_MEMORY)
    {
        logParsingError(sourceLine, "Unable to write string, Out of memory.");
        return DATA_WRITE_ERROR;
    }
    
    return pos;
}
