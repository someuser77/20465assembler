#include <stdlib.h>
#include "list.h"
#include "instructionqueue.h"


InstructionQueue initInstructionQueue()
{
    InstructionQueue queue;
    queue.list = initList(NodeType_Instruction);
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
    static ListNodePtr node = NULL;
    static Boolean done = False;
    
    InstructionLayoutPtr result;
    
    if (done) return NULL;
    
    if (!done && node == NULL)
    {
        node = table->list.head;
        result = node->data->instruction;
        node = node->next;
        return result;
    }
    
    result = node->data->instruction;
    
    node = node->next;
    
    if (node == NULL)
    {
        done = True;
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
