/* 
 * File:   symbol.h
 * Author: daniel
 *
 * Created on July 30, 2013, 5:02 PM
 */

#ifndef SYMBOL_H
#define	SYMBOL_H

#include "consts.h"
#include "types.h"

/* represents a symbol in the symbol table. 
 declared separately to avoid a cyclic reference between List and the SymbolTable
 */
typedef enum {SymbolSection_Code, SymbolSection_Data} SymbolSection;

typedef struct tSymbol
{
    char symbolName[MAX_LABEL_LENGTH]; /* labels are no longer than 30 chars */
    /* the value this symbol should be replaced with */
    Word value;
    /* holds if this symbol is located in the code section or the data section? */
    SymbolSection symbolSection;
    /* is this symbol an entry? */
    Boolean entry;
} Symbol, *SymbolPtr;

#endif	/* SYMBOL_H */

