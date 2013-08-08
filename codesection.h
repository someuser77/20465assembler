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

/* represents the code section of the code */
typedef struct 
{
    SymbolTablePtr symbolTable;
    /* the underlying memory buffer */
    Memory *memory;
    /* for each slot in 'memory', the memoryType array will hold a value from the 
     * MemoryType enum to indicate the coresponding slot's memory type */
    MemoryType *memoryType;
    /* the base offset of the code */
    Word codeBaseAddress;
    /* a list of external symbols encountered in the CodeSection */
    List externalSymbols;
} CodeSection;

CodeSection *initCodeSection(SymbolTablePtr symbolTable);
void freeCodeSection(CodeSection *codeSection);
/* writes the instruction with the operands to the code section */
int writeInstruction(CodeSection *codeSection, InstructionLayoutPtr instruction, SourceLinePtr sourceLine);
void printCodeSection(CodeSection *codeSection);
void setCodeBaseAddress(CodeSection *codeSection, Word address);
Word getAbsoluteInstructionCounter(CodeSection *codeSection);
Word getRelativeInstructionCounter(CodeSection *codeSection);
void fixDataOffset(CodeSection *codeSection, int offset);
void writeExternalSymbols(CodeSection *codeSection, FILE *file);
void printExternalSymbols(CodeSection *codeSection);
Word getCodeSectionSize(CodeSection *codeSection);
/* dumps the code section with a specific format onto the target FILE* */
void writeCodeSection(CodeSection *codeSection, FILE *file);
int getNumberOfExternalSymbols(CodeSection *codeSection);
#endif	/* CODE_H */

