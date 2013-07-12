#include <stdlib.h>
#include "symboltable.h"

SymbolTable initSymbolTable()
{
    SymbolTable table;
    table.count = 0;
    table.table = NULL;
    return table;
}
void insertSymbol(ptrSymbolTable table, char *symbol, int value);
ptrSymbolTableEntry findSymbol(ptrSymbolTable table, char *symbol);
void freeSymbolTable();
