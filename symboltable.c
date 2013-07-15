#include <stdlib.h>
#include <string.h>
#include "symboltable.h"

SymbolTable initSymbolTable()
{
    SymbolTable table;
    table.count = 0;
    table.root = NULL;
    table.last = NULL;
    return table;
}

void insertSymbol(SymbolTablePtr table, char *symbol, SymbolType symbolType, int value)
{
    SymbolTableEntryPtr entry;
    
    entry = (SymbolTableEntryPtr)malloc(sizeof(SymbolTableEntry));
    entry->next = NULL;
    entry->value = value;
    
    strcpy(entry->symbolName, symbol);
    
    if (table->root == NULL)
    {
        table->root = entry;
        table->last = entry;
    }
    else
    {
        table->last->next = entry;
        table->last = entry;
    }
}

SymbolTableEntryPtr findSymbol(SymbolTablePtr table, char *symbol)
{
    SymbolTableEntryPtr entry = table->root;
    while (entry != NULL)
    {
        if (strcmp(entry->symbolName, symbol) == 0) return entry;
        entry = entry->next;
    }
    return NULL;
}

void freeSymbolTable(SymbolTablePtr table)
{
    void freeSymbolTableEntry(SymbolTableEntryPtr);
    SymbolTableEntryPtr entry = table->root;
    SymbolTableEntryPtr next = entry;
    
    while (entry != NULL)
    {
        next = entry->next;
        freeSymbolTableEntry(entry);
        entry = next;
    }
}

void freeSymbolTableEntry(SymbolTableEntryPtr tableEntry)
{
    free(tableEntry);
}