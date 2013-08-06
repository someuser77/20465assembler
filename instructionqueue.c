#include <stdlib.h>
#include "list.h"
#include "instructionqueue.h"

InstructionQueue initInstructionQueue()
{
    InstructionQueue queue;
    queue.list = initList(NodeType_Instruction);
    queue.nodeToIterate = NULL;
    queue.iterationIsDone = False;
    return queue;
}

InstructionLayoutPtr insertInstruction(InstructionQueuePtr table, InstructionLayoutPtr instruction)
{
    ListNodePtr node;
    ListNodeDataPtr data;
    
    data = (ListNodeDataPtr)malloc(sizeof(ListNodeData));
    data->instruction = instruction;
    
    node = insertNode(&table->list, data, NodeType_Instruction);
    
    return node->data->instruction;
}

InstructionLayoutPtr getNextInstruction(InstructionQueuePtr table)
{
    InstructionLayoutPtr result;
    
    if (table->iterationIsDone) return NULL;
    
    if (!table->iterationIsDone && table->nodeToIterate == NULL)
    {
        table->nodeToIterate = table->list.head;
        result = table->nodeToIterate->data->instruction;
        table->nodeToIterate = table->nodeToIterate->next;
        return result;
    }
    
    result = table->nodeToIterate->data->instruction;
    
    table->nodeToIterate = table->nodeToIterate->next;
    
    if (table->nodeToIterate == NULL)
    {
        table->iterationIsDone = True;
    }
    
    return result;
}

void freeInstructionLayoutPtr(ListNodeDataPtr nodeData)
{
    free(nodeData->instruction);
}

void freeInstructionTable(InstructionQueuePtr table)
{
    freeList(&table->list, freeInstructionLayoutPtr);
}
