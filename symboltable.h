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

/* representation of the found symbols, allows to search for them. */

typedef struct {
    /* the list of symbols */
    List list;
    /* the number of symbols that are entries */
    int entries;
} SymbolTable, *SymbolTablePtr;

SymbolTable initSymbolTable();
SymbolPtr insertSymbol(SymbolTablePtr table, char *symbol, SymbolSection symbolSection, Word value);
SymbolPtr findSymbol(SymbolTablePtr table, char *symbol);
void freeSymbolTable(SymbolTablePtr table);
void printSymbolTable(SymbolTablePtr table);
void writeEntries(SymbolTablePtr table, FILE *file);
void printEntries(SymbolTablePtr symbolTable);
int getNumberOfEntries(SymbolTablePtr symbolTable);
/* marks the given symbol as a entry in the symbol table */
SymbolPtr markEntry(SymbolTablePtr table, char *symbol);
#endif	/* SYMBOLTABLE_H */
