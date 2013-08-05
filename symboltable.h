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

#define EXTERN_SYMBOL_VALUE -1

typedef struct {
    List list;
} SymbolTable, *SymbolTablePtr;

SymbolTable initSymbolTable();
SymbolPtr insertSymbol(SymbolTablePtr table, char *symbol, SymbolSection symbolSection, Word value);
SymbolPtr findSymbol(SymbolTablePtr table, char *symbol);
void freeSymbolTable(SymbolTablePtr table);
void printSymbolTable(SymbolTablePtr table);
void writeEntries(SymbolTablePtr table, FILE *file);
void printEntries(SymbolTablePtr symbolTable);
#endif	/* SYMBOLTABLE_H */
