/* 
 * File:   parser.h
 * Author: daniel
 *
 * Created on July 11, 2013, 11:10 AM
 */

#ifndef PARSER_H
#define	PARSER_H

#include "types.h"
#include "symboltable.h"

typedef struct tSourceLine
{
    char *text;
    char *start;
    int lineNumber;
    char *fileName;
} SourceLine, *SourceLinePtr;


SourceLine initSourceLine(char *text, int lineNumber, char* fileName);
void freeSourceLine(SourceLine *line);
void skipWhitespace(SourceLine *sourceLine);
char *getLabel(SourceLine *sourceLine);
Boolean isBlankLine(SourceLine *sourceLine);
Boolean isCommentLine(SourceLine *sourceLine);
Boolean isImaginaryGuidance(SourceLine *sourceLine);
Boolean tryGetGuidanceType(SourceLine *sourceLine, SymbolType *symbolType);
Boolean firstPass(FILE *sourceFile, SymbolTablePtr symbolTable, char *sourceFileName);
#endif	/* PARSER_H */

