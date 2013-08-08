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

/* the main set of functions to parse the source file */

SourceLine initSourceLine(char *text, int lineNumber, char* fileName);
void freeSourceLine(SourceLine *line);
void skipWhitespace(SourceLine *sourceLine);
char *getLabel(SourceLine *sourceLine);
Boolean isBlankLine(SourceLine *sourceLine);
Boolean isCommentLine(SourceLine *sourceLine);
Boolean isImaginaryGuidance(SourceLine *sourceLine);
Boolean tryGetGuidanceType(SourceLine *sourceLine, GuidanceType *guidanceType);

/* the first pass checks syntax and enqueues the instructions to write in the instructionQueue and writes the data found in the data sectin */
int firstPass(FILE *sourceFile, CodeSection *codeSection, InstructionQueuePtr instructionQueue, DataSection *dataSection, char *sourceFileName);

/* the second pass dumps the instructionQueue onto the code section */
Boolean secondPass(FILE *sourceFile, CodeSection *codeSection, InstructionQueuePtr instructionQueue, char *sourceFileName);

/* attempts to parse a number from the current position the the line */
Boolean tryReadNumber(SourceLinePtr sourceLine, int *value);

/* determines if the label in the given line that starts at 'labelStart' and ends in 'labelEnd' is a valid label name */
Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd);

/* reads a label from the line stopping at an EOL or an operand seperator that are not considered part of the label */
char *tryReadLabel(SourceLine *sourceLine);

/* reads a label from the line looking for a given token that is considered part of the label */
char *tryReadLabelWithEndToken(SourceLine *sourceLine, char token);

/* a utility function that returns a new array with 'length' chars from 'str' */
char *cloneString(char *str, int length);

#endif	/* PARSER_H */

