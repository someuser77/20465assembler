#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "logging.h"

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

void logParsingError(SourceLinePtr line, char *error)
{
    logParsingErrorFormat(line, error);
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
