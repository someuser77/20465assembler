#include <stdlib.h>
#include "list.h"
#include "error.h"
#include "logging.h"

List initList(NodeType listType)
{
    List list;
    list.head = NULL;
    list.last = NULL;
    list.listType = listType;
    list.length = 0;
    return list;
}

/* utility function to print a type of the node */
char *getNodeTypeName(NodeType nodeType)
{
    static char **listTypeNames;
    static Boolean initialized = False;
    
    if (!initialized)
    {
        listTypeNames = (char **)malloc(sizeof(char *) * 3);
        listTypeNames[NodeType_Symbol] = "Symbol";
        listTypeNames[NodeType_Instruction] = "Instruction";
        listTypeNames[NodeType_SymbolLocation] = "SymbolLocation";
        initialized = True;
    }
    
    return listTypeNames[nodeType];
}

ListNodePtr insertNode(ListPtr list, ListNodeDataPtr data, NodeType nodeType)
{
    ListNodePtr node;
    
    if (nodeType != list->listType)
    {
        logError("Attempted to insert node of type %s into list of type %s", getNodeTypeName(nodeType), getNodeTypeName(list->listType));
        return NULL;
    }
    
    node = (ListNodePtr)malloc(sizeof(ListNode));
    node->next = NULL;
    node->data = data;
    node->nodeType = nodeType;

    if (list->head == NULL)
    {
        list->head = node;
    }
    else
    {
        list->last->next = node;
    }
    
    list->last = node;
        
    list->length++;
    
    return node;    
}

ListNodePtr findNode(ListPtr list, ListNodeDataPtr data, Boolean (*comparer)(ListNodeDataPtr a, ListNodeDataPtr b))
{
    ListNodePtr node = list->head;
    
    while (node != NULL)
    {
        if ((*comparer)(node->data, data) == True)
        {
            return node;
        }
        
        node = node->next;
    }
    
    return NULL;
}

void freeList(ListPtr list, void (*freeNodeData)(ListNodeDataPtr nodeData))
{
    ListNodePtr node = list->head;
    ListNodePtr next = node;
    
    /* because the node are freed we cant use 'actOnList' */
    
    while (node != NULL)
    {
        next = node->next;
        
        if (freeNodeData != NULL)
        {
                (*freeNodeData)(node->data);
        }
        
        free(node->data);
        
        node = next;
    }
}

void actOnList(ListPtr list, void (*action)(ListNodeDataPtr nodeData, void *context), void *context)
{
    ListNodePtr node = list->head;
    
    while (node != NULL)
    {
        (*action)(node->data, context);
        
        node = node->next;
    }
}

int getListLength(ListPtr list)
{
    return list->length;
}