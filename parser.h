/* 
 * File:   parser.h
 * Author: daniel
 *
 * Created on July 11, 2013, 11:10 AM
 */

#ifndef PARSER_H
#define	PARSER_H

typedef struct tSourceLine
{
    char *text;
    char *start;
    int lineNumber;
    char *fileName;
} SourceLine, *ptrSourceLine;


SourceLine initSourceLine(char *text, int lineNumber, char* fileName);
void freeSourceLine(SourceLine *line);
char *skipWhitespace(char *sourceLine);
char *getLabel(SourceLine *sourceLine);
Boolean isBlankLine(char *sourceCodeLine);
Boolean isCommentLine(char *sourceCodeLine);
Boolean isImaginaryGuidance(char *sourceLine);

#endif	/* PARSER_H */

