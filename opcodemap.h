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

/* a set of functions to parse opcodes and operands into instructions */

typedef void (*OpcodeHandler)(SourceLinePtr sourceLine, InstructionLayoutPtr instructionRepresentation);

Boolean isValidOpcodeName(char *instruction);
Boolean tryReadOpcode(SourceLinePtr sourceLine, Opcode *opcode);
/* parses a line into an Instruction */
InstructionLayoutPtr getInstructionLayout(SourceLinePtr sourceLine, Opcode opcode);
char *getOpcodeNameToken(SourceLinePtr sourceLine);
/* returns a string with the opcode name. 
   must be freed after use. */
char* getOpcodeName(Opcode opcode);

/* returns a function pointer to the method that handles the given opcode */
OpcodeHandler getOpcodeHandler(Opcode opcode);

#endif	/* INSTRUCTIONMAP_H */

