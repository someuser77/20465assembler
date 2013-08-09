#include <stdlib.h>
#include "list.h"
#include "instructionqueue.h"

InstructionQueuePtr initInstructionQueue()
{
    InstructionQueuePtr queue;
    queue = (InstructionQueuePtr)malloc(sizeof(InstructionQueue));
    queue->list = initList(NodeType_Instruction);
    queue->nodeToIterate = NULL;
    queue->iterationIsDone = False;
    return queue;
}

InstructionLayoutPtr insertInstruction(InstructionQueuePtr queue, InstructionLayoutPtr instruction)
{
    ListNodePtr node;
    ListNodeDataPtr data;
    
    data = (ListNodeDataPtr)malloc(sizeof(ListNodeData));
    data->instruction = instruction;
    
    node = insertNode(&queue->list, data, NodeType_Instruction);
    
    return node->data->instruction;
}

InstructionLayoutPtr getNextInstruction(InstructionQueuePtr queue)
{
    InstructionLayoutPtr result;
    
    if (queue->iterationIsDone) return NULL;
    
    if (!queue->iterationIsDone && queue->nodeToIterate == NULL)
    {
        queue->nodeToIterate = queue->list.head;
        result = queue->nodeToIterate->data->instruction;
        queue->nodeToIterate = queue->nodeToIterate->next;
        return result;
    }
    
    result = queue->nodeToIterate->data->instruction;
    
    queue->nodeToIterate = queue->nodeToIterate->next;
    
    if (queue->nodeToIterate == NULL)
    {
        queue->iterationIsDone = True;
    }
    
    return result;
}

void freeInstructionLayoutPtr(ListNodeDataPtr nodeData)
{
    free(nodeData->instruction);
}

void freeInstructionQueue(InstructionQueuePtr queue)
{
    freeList(&queue->list, freeInstructionLayoutPtr);
    free(queue);
}
