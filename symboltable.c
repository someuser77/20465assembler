#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symboltable.h"
#include "list.h"

SymbolTable initSymbolTable()
{
    SymbolTable table;
    table.list = initList(NodeType_Symbol);
    return table;
}

Boolean symbolNameComparer(ListNodeDataPtr a, ListNodeDataPtr b)
{
    return strcmp(a->symbol.symbolName, b->symbol.symbolName) == 0;
}

SymbolPtr insertSymbol(SymbolTablePtr table, char *symbol, SymbolType symbolType, int value)
{
    ListNodeData dataToSearch;
    ListNodeDataPtr dataToInsert;
    ListNodePtr inserted;
    Boolean symbolExists;
    
    strncpy(dataToSearch.symbol.symbolName, symbol, MAX_LABEL_LENGTH);
    
    symbolExists = (findNode(&table->list, &dataToSearch, symbolNameComparer) != NULL);
    
    if (symbolExists)
    {
        return NULL;
    }
    
    dataToInsert = (ListNodeDataPtr)malloc(sizeof(ListNodeData));
    
    dataToInsert->symbol.entry = False;
    dataToInsert->symbol.symbolType = symbolType;
    dataToInsert->symbol.value = value;
    
    strncpy(dataToInsert->symbol.symbolName, symbol, MAX_LABEL_LENGTH);
    
    inserted = insertNode(&table->list, dataToInsert, NodeType_Symbol);
    
    return &inserted->data->symbol;
}

SymbolPtr findSymbol(SymbolTablePtr table, char *symbol)
{
    ListNodePtr node;
    ListNodeData data;
    strncpy(data.symbol.symbolName, symbol, MAX_LABEL_LENGTH);

    node = findNode(&table->list, &data, symbolNameComparer);
    
    if (node == NULL)
    {
        return NULL;
    }
    
    return &node->data->symbol;
}

void freeSymbolTable(SymbolTablePtr table)
{
    freeList(&table->list, NULL);
}

void printSymbol(ListNodeDataPtr dataPtr, void *context)
{
    printf("%s\t%d\t%o\n", dataPtr->symbol.symbolName, dataPtr->symbol.value, dataPtr->symbol.value);
}

void printSymbolTable(SymbolTablePtr table)
{
    printf("Symbol table: \n");
    actOnList(&table->list, printSymbol, NULL);
}

void fixDataOffsetForNode(ListNodeDataPtr dataPtr, void *context)
{
    if (dataPtr->symbol.symbolType == SymbolType_Data)
    {
        dataPtr->symbol.value += *(int*)(context);
    }
}

void fixDataOffset(SymbolTablePtr table, int offset)
{
    actOnList(&table->list, fixDataOffsetForNode, &offset);
}