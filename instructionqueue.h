/* 
 * File:   instructiontable.h
 * Author: daniel
 *
 * Created on August 1, 2013, 12:24 PM
 */

#ifndef INSTRUCTIONTABLE_H
#define	INSTRUCTIONTABLE_H

#include "consts.h"
#include "types.h"
#include "list.h"

typedef struct {
    List list;
    ListNodePtr nodeToIterate;
    Boolean iterationIsDone;
} InstructionQueue, *InstructionQueuePtr;

InstructionQueue initInstructionQueue();
InstructionLayoutPtr insertInstruction(InstructionQueuePtr table, InstructionLayoutPtr instruction);
InstructionLayoutPtr getNextInstruction(InstructionQueuePtr table);
void freeInstructionTable(InstructionQueuePtr table);
#endif	/* INSTRUCTIONTABLE_H */

