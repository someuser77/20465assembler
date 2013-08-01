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
    return list;
}

char *getNodeTypeName(NodeType nodeType)
{
    static char **listTypeNames;
    static Boolean initialized = False;
    
    if (!initialized)
    {
        listTypeNames = (char **)malloc(sizeof(char *) * 2);
        listTypeNames[NodeType_Symbol] = "Symbol";
        initialized = True;
    }
    
    return listTypeNames[nodeType];
}

ListNodePtr insertNode(ListPtr list, ListNodeDataPtr data, NodeType nodeType)
{
    ListNodePtr node;
    
    if (nodeType != list->listType)
    {
        logErrorFormat("Attempted to insert node of type %s into list of type %s", getNodeTypeName(nodeType), getNodeTypeName(list->listType));
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
    
    while (node != NULL)
    {
        next = node->next;
        if (freeNodeData != NULL)
        {
                (*freeNodeData)(node->data);
        }
        node = next;
    }
}