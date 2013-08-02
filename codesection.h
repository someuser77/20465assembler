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
typedef struct 
{
    SymbolTablePtr symbolTable;
    Memory memory;    
} CodeSection;

int writeInstruction(CodeSection *codeSection, InstructionLayoutPtr instruction);

#endif	/* CODE_H */

