/* 
 * File:   symboltable.h
 * Author: daniel
 *
 * Created on July 12, 2013, 2:44 PM
 */

#ifndef SYMBOLTABLE_H
#define	SYMBOLTABLE_H

typedef struct tSymbolTableEntry{
    char *symbol;
    int value;
    struct tSymbolTableEntry *next;
} SymbolTableEntry, *ptrSymbolTableEntry;

typedef struct {
    int count;
    ptrSymbolTableEntry table;
} SymbolTable, *ptrSymbolTable;

SymbolTable initSymbolTable();
void insertSymbol(ptrSymbolTable table, char *symbol, int value);
ptrSymbolTableEntry findSymbol(ptrSymbolTable table, char *symbol);
void freeSymbolTable();
#endif	/* SYMBOLTABLE_H */
