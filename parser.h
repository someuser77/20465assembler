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


SourceLine initSourceLine(char *text, int lineNumber, char* fileName);
void freeSourceLine(SourceLine *line);
void skipWhitespace(SourceLine *sourceLine);
char *getLabel(SourceLine *sourceLine);
Boolean isBlankLine(SourceLine *sourceLine);
Boolean isCommentLine(SourceLine *sourceLine);
Boolean isImaginaryGuidance(SourceLine *sourceLine);
Boolean tryGetGuidanceType(SourceLine *sourceLine, GuidanceType *guidanceType);
Boolean firstPass(FILE *sourceFile, SymbolTablePtr symbolTable, char *sourceFileName);
Boolean tryReadNumber(SourceLinePtr sourceLine, int *value);
#endif	/* PARSER_H */

