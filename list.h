/* 
 * File:   list.h
 * Author: daniel
 *
 * Created on July 30, 2013, 4:28 PM
 */

#ifndef LIST_H
#define	LIST_H

#include "symbol.h"
#include "symbollocation.h"

/* a generic list implementation.
 * each node has a union from a set of known types so have some sort of type safety. 
 * each list is initiated with its NodeType and only nodes of the same type
 * can be inserted into it. 
 */

typedef enum {NodeType_Symbol, NodeType_Instruction, NodeType_SymbolLocation} NodeType;

/* a union of the types a list can hold */
typedef union {
       Symbol symbol; 
       InstructionLayoutPtr instruction; 
       SymbolLocationPtr symbolLocation;
    } ListNodeData, *ListNodeDataPtr;

typedef struct tNode{
    ListNodeDataPtr data;
    NodeType nodeType;
    struct tNode *next;
} ListNode, *ListNodePtr;

typedef struct tList{
    NodeType listType;
    ListNode *head;
    ListNode *last;
    int length;
} List, *ListPtr;


List initList(NodeType type);
ListNodePtr insertNode(ListPtr list, ListNodeDataPtr data, NodeType nodeType);
/* returns a pointer to the node with the given data and for which the specified comparer returned True 
 allowing the user to determine what makes a nodes equal */
ListNodePtr findNode(ListPtr list, ListNodeDataPtr data, Boolean (*comparer)(ListNodeDataPtr a, ListNodeDataPtr b));
/* because we don't know how a node was allocated a user has to provide the function to free it */
void freeList(ListPtr list, void (*freeNodeData)(ListNodeDataPtr nodeData));
/* allows to mutate the list by providing an action to be called on each node with the given context */
void actOnList(ListPtr list, void (*action)(ListNodeDataPtr nodeData, void *context), void *context);
int getListLength(ListPtr list);
#endif	/* LIST_H */

