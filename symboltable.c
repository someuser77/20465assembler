#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symboltable.h"
#include "list.h"
#include "codesection.h"

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

SymbolPtr insertSymbol(SymbolTablePtr table, char *symbol, SymbolSection symbolSection, Word value)
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
    dataToInsert->symbol.symbolSection = symbolSection;
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
    printf("%s\t%d\t%o\n", dataPtr->symbol.symbolName, dataPtr->symbol.value.value, dataPtr->symbol.value.value);
}

void printSymbolTable(SymbolTablePtr table)
{
    printf("Symbol table: \n");
    actOnList(&table->list, printSymbol, NULL);
}

void writeEntry(ListNodeDataPtr dataPtr, void *context)
{
    FILE *file = (FILE*)(context);
    
    if (dataPtr->symbol.entry)
    {
        fprintf(file, "%s\t%o\n", dataPtr->symbol.symbolName, dataPtr->symbol.value.value);
    }
}

void writeEntries(SymbolTablePtr table, FILE *file)
{
    actOnList(&table->list, writeEntry, file);
}

void printEntries(SymbolTablePtr symbolTable)
{
    printf("\n\n === ENTRIES === \n\n");
    
    writeEntries(symbolTable, stdout);
}