#include <stdlib.h>
#include "symbollocation.h"

SymbolLocationPtr initSymbolLocation(char *symbol, Word location)
{
    SymbolLocationPtr symbolLocation;
    
    symbolLocation = (SymbolLocationPtr)malloc(sizeof(SymbolLocation));
    
    symbolLocation->symbol = symbol;
    symbolLocation->location = location;
    
    return symbolLocation;
}