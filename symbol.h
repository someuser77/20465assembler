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

typedef enum {SymbolSection_Code, SymbolSection_Data} SymbolSection;

typedef struct tSymbol
{
    char symbolName[MAX_LABEL_LENGTH]; /* labels are no longer than 30 chars */
    int value;
    SymbolSection symbolSection;
    Boolean entry;
} Symbol, *SymbolPtr;

#endif	/* SYMBOL_H */

