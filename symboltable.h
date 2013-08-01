/* 
 * File:   symboltable.h
 * Author: daniel
 *
 * Created on July 12, 2013, 2:44 PM
 */

#ifndef SYMBOLTABLE_H
#define	SYMBOLTABLE_H

#include "consts.h"
#include "types.h"
#include "list.h"

#define EMPTY_SYMBOL_VALUE -1

typedef struct {
    List list;
} SymbolTable, *SymbolTablePtr;

SymbolTable initSymbolTable();
SymbolPtr insertSymbol(SymbolTablePtr table, char *symbol, SymbolType symbolType, int value);
SymbolPtr findSymbol(SymbolTablePtr table, char *symbol);
void freeSymbolTable(SymbolTablePtr table);
#endif	/* SYMBOLTABLE_H */
