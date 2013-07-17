/* 
 * File:   instructionmap.h
 * Author: daniel
 *
 * Created on July 12, 2013, 5:17 PM
 */
#include "types.h"

#ifndef INSTRUCTIONMAP_H
#define	INSTRUCTIONMAP_H

Boolean isValidOpcodeName(char *instruction);
Boolean tryGetOpcode(SourceLinePtr sourceLine, Opcode *opcode);
#endif	/* INSTRUCTIONMAP_H */

