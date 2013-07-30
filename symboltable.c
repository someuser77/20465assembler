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

SymbolPtr insertSymbol(SymbolTablePtr table, char *symbol, SymbolType symbolType, int value)
{
    SymbolTableEntryPtr entry;
    
    if (findSymbol(table, symbol) != NULL)
    {
        return NULL;
    }
    
    entry = (SymbolTableEntryPtr)malloc(sizeof(SymbolTableEntry));
    entry->next = NULL;
    entry->symbol.value = value;
    entry->symbol.symbolType = symbolType;
    entry->symbol.entry = False;
    
    strcpy(entry->symbol.symbolName, symbol);
    
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
    
    return &entry->symbol;
}

SymbolPtr findSymbol(SymbolTablePtr table, char *symbol)
{
    SymbolTableEntryPtr entry = table->root;
    while (entry != NULL)
    {
        if (strcmp(entry->symbol.symbolName, symbol) == 0)
        {
            return &entry->symbol;
        }
        
        entry = entry->next;
    }
    return NULL;
}


void freeSymbolTableEntry(SymbolTableEntryPtr tableEntry)
{
    free(tableEntry);
}

void freeSymbolTable(SymbolTablePtr table)
{
    SymbolTableEntryPtr entry = table->root;
    SymbolTableEntryPtr next = entry;
    
    while (entry != NULL)
    {
        next = entry->next;
        freeSymbolTableEntry(entry);
        entry = next;
    }
}
