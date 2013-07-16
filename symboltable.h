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

#define EMPTY_SYMBOL_VALUE -1

typedef enum {SymbolType_Code, SymbolType_Data} SymbolType;

typedef struct tSymbolTableEntry{
    char symbolName[MAX_LABEL_LENGTH]; /* labels are no longer than 30 chars */
    int value;
    SymbolType symbolType;
    struct tSymbolTableEntry *next;
} SymbolTableEntry, *SymbolTableEntryPtr;

typedef struct {
    int count;
    SymbolTableEntryPtr root;
    SymbolTableEntryPtr last;
} SymbolTable, *SymbolTablePtr;

SymbolTable initSymbolTable();
void insertSymbol(SymbolTablePtr table, char *symbol, SymbolType symbolType, int value);
SymbolTableEntryPtr findSymbol(SymbolTablePtr table, char *symbol);
void freeSymbolTable(SymbolTablePtr table);
#endif	/* SYMBOLTABLE_H */
