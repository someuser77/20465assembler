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

/* whenever an instruction is encountered it will be added to this queue 
 * so it will be written in order in the second pass. 
 * this queue will be filled with instructions, and then dumped as the code section */
typedef struct tInstructionQueue {
    List list;
    /* this queue is iterated using nodeToIterate that will hold the pointer 
     * to the node to return so the user won't have to maintain it. */
    ListNodePtr nodeToIterate;
    Boolean iterationIsDone;
} InstructionQueue, *InstructionQueuePtr;

InstructionQueuePtr initInstructionQueue();
/* adds a new instruction to the queue */
InstructionLayoutPtr insertInstruction(InstructionQueuePtr queue, InstructionLayoutPtr instruction);
/* used to iterate the instruction when all instructions were inserted */
InstructionLayoutPtr getNextInstruction(InstructionQueuePtr queue);
void freeInstructionQueue(InstructionQueuePtr queue);
#endif	/* INSTRUCTIONTABLE_H */

