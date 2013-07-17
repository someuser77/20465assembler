/* 
 * File:   parser.h
 * Author: daniel
 *
 * Created on July 11, 2013, 11:10 AM
 */

#ifndef PARSER_H
#define	PARSER_H

#include <stdio.h>
#include "types.h"
#include "symboltable.h"

typedef struct tSourceLine
{
    /* a pointer to the position inside the text line */
    char *text;
    /* a pointer to the beginning of the line*/
    char *start;
    /* the number of the line in the source file */
    int lineNumber;
    /* the name of the file this line belogs to */
    char *fileName;
    /* an error associated with this line*/
    char *error;
} SourceLine, *SourceLinePtr;


SourceLine initSourceLine(char *text, int lineNumber, char* fileName);
void freeSourceLine(SourceLine *line);
void skipWhitespace(SourceLine *sourceLine);
char *getLabel(SourceLine *sourceLine);
Boolean isBlankLine(SourceLine *sourceLine);
Boolean isCommentLine(SourceLine *sourceLine);
Boolean isImaginaryGuidance(SourceLine *sourceLine);
Boolean tryGetGuidanceType(SourceLine *sourceLine, GuidanceType *guidanceType);
Boolean firstPass(FILE *sourceFile, SymbolTablePtr symbolTable, char *sourceFileName);
#endif	/* PARSER_H */

