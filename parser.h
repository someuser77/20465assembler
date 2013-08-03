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
#include "datasection.h"
#include "codesection.h"
#include "instructionqueue.h"

SourceLine initSourceLine(char *text, int lineNumber, char* fileName);
void freeSourceLine(SourceLine *line);
void skipWhitespace(SourceLine *sourceLine);
char *getLabel(SourceLine *sourceLine);
Boolean isBlankLine(SourceLine *sourceLine);
Boolean isCommentLine(SourceLine *sourceLine);
Boolean isImaginaryGuidance(SourceLine *sourceLine);
Boolean tryGetGuidanceType(SourceLine *sourceLine, GuidanceType *guidanceType);
int firstPass(FILE *sourceFile, SymbolTablePtr symbolTable, InstructionQueuePtr instructionQueue, DataSection *dataSection, char *sourceFileName);
Boolean secondPass(FILE *sourceFile, CodeSection *codeSection, InstructionQueuePtr instructionQueue, char *sourceFileName);
Boolean tryReadNumber(SourceLinePtr sourceLine, int *value);
Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd);
char *tryReadLabel(SourceLine *sourceLine);
char *tryReadLabelWithEndToken(SourceLine *sourceLine, char token);
char *cloneString(char *str, int length);

#endif	/* PARSER_H */

