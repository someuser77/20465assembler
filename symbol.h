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

typedef enum {SymbolType_Code, SymbolType_Data} SymbolType;

typedef struct tSymbol
{
    char symbolName[MAX_LABEL_LENGTH]; /* labels are no longer than 30 chars */
    int value;
    SymbolType symbolType;
    Boolean entry;
} Symbol, *SymbolPtr;

#endif	/* SYMBOL_H */

