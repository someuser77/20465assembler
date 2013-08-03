#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "logging.h"

void logErrorInLineFormat(SourceLinePtr line, char *error, ...)
{
    va_list ap;
    
    fprintf(stderr, "Error in file '%s' line %d: ", line->fileName, line->lineNumber);
    
    va_start(ap, error);
    vfprintf(stderr, error, ap);
    va_end(ap);
    
    if (line->error != NULL)
    {
        fprintf(stderr, " %s\n", line->error);
    }
    
    fprintf(stderr, "\n");
}

void logErrorInLine(SourceLinePtr line, char *error)
{
    logErrorInLineFormat(line, error);
}

void logErrorFormat(char *error, ...)
{
    va_list ap;
    va_start(ap, error);
    vfprintf(stderr, error, ap);
    va_end(ap);
}

void logError(char *error)
{
    logErrorFormat(error);
}
