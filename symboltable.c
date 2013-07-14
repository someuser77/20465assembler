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

void insertSymbol(ptrSymbolTable table, char *symbol, int value)
{
    ptrSymbolTableEntry entry;
    
    entry = (ptrSymbolTableEntry)malloc(sizeof(SymbolTableEntry));
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

ptrSymbolTableEntry findSymbol(ptrSymbolTable table, char *symbol)
{
    ptrSymbolTableEntry entry = table->root;
    while (entry != NULL)
    {
        if (strcmp(entry->symbolName, symbol) == 0) return entry;
        entry = entry->next;
    }
    return NULL;
}

void freeSymbolTable(ptrSymbolTable table)
{
    void freeSymbolTableEntry(ptrSymbolTableEntry);
    ptrSymbolTableEntry entry = table->root;
    ptrSymbolTableEntry next = entry;
    
    while (entry != NULL)
    {
        next = entry->next;
        freeSymbolTableEntry(entry);
        entry = next;
    }
}

void freeSymbolTableEntry(ptrSymbolTableEntry tableEntry)
{
    free(tableEntry);
}