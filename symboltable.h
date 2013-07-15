/* 
 * File:   symboltable.h
 * Author: daniel
 *
 * Created on July 12, 2013, 2:44 PM
 */

#include "consts.h"
#include "types.h"
#ifndef SYMBOLTABLE_H
#define	SYMBOLTABLE_H

typedef struct tSymbolTableEntry{
    char symbolName[MAX_LABEL_LENGTH]; /* labels are no longer than 30 chars */
    int value;
    SymbolType symbolType;
    struct tSymbolTableEntry *next;
} SymbolTableEntry, *ptrSymbolTableEntry;

typedef struct {
    int count;
    ptrSymbolTableEntry root;
    ptrSymbolTableEntry last;
} SymbolTable, *ptrSymbolTable;

SymbolTable initSymbolTable();
void insertSymbol(ptrSymbolTable table, char *symbol, SymbolType symbolType, int value);
ptrSymbolTableEntry findSymbol(ptrSymbolTable table, char *symbol);
void freeSymbolTable(ptrSymbolTable table);
#endif	/* SYMBOLTABLE_H */
