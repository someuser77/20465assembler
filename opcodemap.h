/* 
 * File:   instructionmap.h
 * Author: daniel
 *
 * Created on July 12, 2013, 5:17 PM
 */
#include "types.h"

#ifndef INSTRUCTIONMAP_H
#define	INSTRUCTIONMAP_H


#define OPCODE_NAME_LENGTH 4
#define NUMBER_OF_OPCODES 16


typedef void (*OpcodeHandler)(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);

Boolean isValidOpcodeName(char *instruction);
Boolean tryReadOpcode(SourceLinePtr sourceLine, Opcode *opcode);
InstructionLayoutPtr getInstructionLayout(SourceLinePtr sourceLine, Opcode opcode);

/* returns a string with the opcode name. 
   must be freed after use. */
char* getOpcodeName(Opcode opcode);

/* returns a function pointer to the method that handles this opcode */
OpcodeHandler getOpcodeHandler(Opcode opcode);

#endif	/* INSTRUCTIONMAP_H */

