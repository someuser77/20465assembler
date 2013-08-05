/* 
 * File:   code.h
 * Author: daniel
 *
 * Created on July 27, 2013, 3:22 PM
 */

#ifndef CODE_H
#define	CODE_H

#include "types.h"
#include "memory.h"
#include "symboltable.h"

typedef enum { MemoryType_Unknown, MemoryType_Absolute, MemoryType_Relocatable, MemoryType_External } MemoryType;

typedef struct 
{
    SymbolTablePtr symbolTable;
    Memory *memory;
    MemoryType *memoryType;
    Word codeBaseAddress;
    List externalSymbols;
} CodeSection;

CodeSection *initCodeSection(SymbolTablePtr symbolTable);
void freeCodeSection(CodeSection *codeSection);
int writeInstruction(CodeSection *codeSection, InstructionLayoutPtr instruction, SourceLinePtr sourceLine);
void printCodeSection(CodeSection *codeSection);
void setCodeBaseAddress(CodeSection *codeSection, Word address);
Word getAbsoluteInstructionCounter(CodeSection *codeSection);
Word getRelativeInstructionCounter(CodeSection *codeSection);
void fixDataOffset(CodeSection *codeSection, int offset);
void writeExternalSymbols(CodeSection *codeSection, FILE *file);
void printExternalSymbols(CodeSection *codeSection);
#endif	/* CODE_H */

