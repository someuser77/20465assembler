#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "datasection.h"
#include "logging.h"
#include "parser.h"


int writeDataArray(DataSection *dataSection, SourceLinePtr sourceLine)
{
    char *dataToken = cloneString(sourceLine->text, DATA_GUIDANCE_TOKEN_LENGTH);
    char charAfterToken = *(sourceLine->text + DATA_GUIDANCE_TOKEN_LENGTH);
    
    int value;
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
        
        if (!tryReadNumber(sourceLine, &value))
        {
            logParsingError(sourceLine, "Unable to parse number.");
            break;
        }

#ifdef DEBUG
        printf("Found data value: %d\n", value);
#endif
        skipWhitespace(sourceLine);

        if (*sourceLine->text != EOL && *sourceLine->text != DATA_GUIDANCE_SEPARATOR)
        {
            logParsingErrorFormat(sourceLine, "Missing separator after value %d.", value);
            break;
        }
        
        if (*sourceLine->text == DATA_GUIDANCE_SEPARATOR)
        {
            sourceLine->text += DATA_GUIDANCE_SEPARATOR_LENGTH;
        }
        
        pos = dataSection->memory.position;
        dataSection->memory.buffer[pos].value = value;
        pos++;
        dataSection->memory.position = pos;
    }

    free(dataToken);
    return pos;
}

int writeDataString(DataSection *dataSection, SourceLinePtr sourceLine)
{
    return 0;
}
