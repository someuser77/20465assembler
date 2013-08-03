/* 
 * File:   list.h
 * Author: daniel
 *
 * Created on July 30, 2013, 4:28 PM
 */

#ifndef LIST_H
#define	LIST_H

#include "symbol.h"

typedef enum {NodeType_Symbol, NodeType_Instruction} NodeType;

typedef union {
       Symbol symbol; 
       InstructionLayoutPtr instruction; 
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
} List, *ListPtr;


List initList(NodeType type);
ListNodePtr insertNode(ListPtr list, ListNodeDataPtr data, NodeType nodeType);
ListNodePtr findNode(ListPtr list, ListNodeDataPtr data, Boolean (*comparer)(ListNodeDataPtr a, ListNodeDataPtr b));
void freeList(ListPtr list, void (*freeNodeData)(ListNodeDataPtr nodeData));
void actOnList(ListPtr list, void (*action)(ListNodeDataPtr nodeData, void *context), void *context);
#endif	/* LIST_H */

