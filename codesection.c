#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "codesection.h"
#include "memory.h"
#include "symboltable.h"
#include "logging.h"

#define REGISTER_DIGIT_INDEX 1

typedef void (*OperandWriter)(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine);

CodeSection *initCodeSection(SymbolTablePtr symbolTable)
{
    CodeSection *codeSection;
    int i;
    
    codeSection = (CodeSection *)malloc(sizeof(CodeSection));
    codeSection->symbolTable = symbolTable;
    codeSection->memory = initMemory();
    codeSection->codeBaseAddress.value = BASE_ADDRESS;
    codeSection->memoryType = (MemoryType *)malloc(sizeof(MemoryType) * MAX_MEMORY_SIZE);
    
    for (i = 0; i < MAX_MEMORY_SIZE; i++)
    {
        codeSection->memoryType[i] = MemoryType_Unknown;
    }
    
    return codeSection;
}

void freeCodeSection(CodeSection *codeSection)
{
    free(codeSection->memory);
    free(codeSection->memoryType);
    free(codeSection);
}

int getRegisterId(char registerString[REGISTER_NAME_LENGTH + 1])
{
    int id;
    
    if (REGISTER_DIGIT_INDEX > REGISTER_NAME_LENGTH - 1)
    {
        logErrorFormat("Error extracting register digit: the digit position %d is longer than the register name %d.", REGISTER_DIGIT_INDEX, (REGISTER_NAME_LENGTH + 1));
        return -1;
    }
    
    id = registerString[REGISTER_DIGIT_INDEX] - '0';
    
    if (!IS_VALID_REGISTER_ID(id))
    {
        id = -1;
    }
    
    return id;
}

void setCurrentMemoryLocationType(CodeSection *codeSection, MemoryType memoryType)
{
    int pos = codeSection->memory->position;
    codeSection->memoryType[pos] = memoryType;
}

void writeOffsetToSymbol(CodeSection *codeSection, char *symbol, Word instructionAddress, SourceLinePtr sourceLine)
{
    Word symbolAddress;
    int offset;
    
    SymbolPtr symbolPtr;
    
    if (symbol == NULL)
    {
        logError("Symbol address to write was NULL.");
        return;
    }
    
    symbolPtr = findSymbol(codeSection->symbolTable, symbol);
    
    if (symbolPtr == NULL)
    {
        logErrorInLineFormat(sourceLine, "Unable to find symbol '%s' in symbol table.\n", symbol);
        return;
    }
    
    if (symbolPtr->symbolSection == EXTERN_SYMBOL_VALUE)
    {
        logErrorInLineFormat(sourceLine, "Unable to calculate offset to an external symbol '%s'.\n", symbol);
        return;
    }
    
    symbolAddress.value = symbolPtr->value;
    
    offset = symbolAddress.value - instructionAddress.value; 
    
    setCurrentMemoryLocationType(codeSection, MemoryType_Absolute);
    
    writeInt(codeSection->memory, offset);
}

void writeSymbolAddress(CodeSection *codeSection, char *symbol, SourceLinePtr sourceLine)
{
    Word symbolAddress;
    SymbolPtr symbolPtr;
    MemoryType memoryType;
    
    if (symbol == NULL)
    {
        logError("Symbol address to write was NULL.");
        return;
    }
    
    symbolPtr = findSymbol(codeSection->symbolTable, symbol);
    
    if (symbolPtr == NULL)
    {
        logErrorInLineFormat(sourceLine, "Unable to find symbol '%s' in symbol table.\n", symbol);
        return;
    }
    
    symbolAddress.value = symbolPtr->value;
    memoryType = MemoryType_Relocatable;
    
    if (symbolAddress.value == EXTERN_SYMBOL_VALUE)
    {
        symbolAddress.value = DEFAULT_EXTERNAL_SYMBOL_ADDRESS;
        memoryType = MemoryType_External;
    }
    
    setCurrentMemoryLocationType(codeSection, memoryType);
    
    writeWord(codeSection->memory, symbolAddress);
}

void writeInstantAddress(CodeSection *codeSection, int address)
{
    setCurrentMemoryLocationType(codeSection, MemoryType_Absolute);
    writeInt(codeSection->memory, address);
}

void setSourceRegister(InstructionLayoutPtr instruction, int registerId)
{
    instruction->opcode.source_register = registerId;
}

void setDestinationRegister(InstructionLayoutPtr instruction, int registerId)
{
    instruction->opcode.dest_register = registerId;
}

void writeOperandWithInstantAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    if (operand->empty) return;

    writeInstantAddress(codeSection, operand->address.value);
}

void writeOperandWithDirectAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    if (operand->empty) return;
    
    writeSymbolAddress(codeSection, operand->address.label, sourceLine);
}

void writeOperandWithDirectRegisterAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    /* just a place holder */
    return;
}

void writeOperandWithVaryingIndexingAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    if (operand->empty) return;
    
    writeSymbolAddress(codeSection, operand->address.varyingAddress.label, sourceLine);
    
    switch (operand->address.varyingAddress.adressing)
    {
        case OperandVaryingAddressing_Direct:
            writeOffsetToSymbol(codeSection, operand->address.varyingAddress.address.label, operand->instruction->instructionAddress , sourceLine);
            break;
        case OperandVaryingAddressing_Instant:
            writeInstantAddress(codeSection, operand->address.varyingAddress.address.value);
            break;
        case OperandVaryingAddressing_DirectRegister:
            /* should have been handled earlier. */
            break;
    }
    
}

void setRegisterAddressing(InstructionLayoutPtr instruction)
{
    int registerId;
    
    /* can't be functioned out because there is no way to tell if its the source or the target*/
    
    if (instruction->leftOperand.addressing == OperandAddressing_DirectRegister)
    {
        registerId = getRegisterId(instruction->leftOperand.address.reg);
        setSourceRegister(instruction, registerId);
    } 
    else if (instruction->leftOperand.addressing == OperandAddressing_VaryingIndexing && 
            instruction->leftOperand.address.varyingAddress.adressing == OperandVaryingAddressing_DirectRegister)
    {
        registerId = getRegisterId(instruction->leftOperand.address.varyingAddress.address.reg);
        setSourceRegister(instruction, registerId);
    }
    
    if (instruction->rightOperand.addressing == OperandAddressing_DirectRegister)
    {
        registerId = getRegisterId(instruction->rightOperand.address.reg);
        setDestinationRegister(instruction, registerId);
    } 
    else if (instruction->rightOperand.addressing == OperandAddressing_VaryingIndexing && 
            instruction->rightOperand.address.varyingAddress.adressing == OperandVaryingAddressing_DirectRegister)
    {
        registerId = getRegisterId(instruction->rightOperand.address.varyingAddress.address.reg);
        setDestinationRegister(instruction, registerId);
    }
}

int writeInstruction(CodeSection *codeSection, InstructionLayoutPtr instruction, SourceLinePtr sourceLine)
{
    static OperandWriter operandWriter[ADDRESSING_TYPES];
    static Boolean operandWritersInitialized = False;
    
    Word word;

    if (!operandWritersInitialized)
    {
        operandWriter[OperandAddressing_Direct] = writeOperandWithDirectAddressing;
        operandWriter[OperandAddressing_DirectRegister] = writeOperandWithDirectRegisterAddressing;
        operandWriter[OperandAddressing_Instant] = writeOperandWithInstantAddressing;
        operandWriter[OperandAddressing_VaryingIndexing] = writeOperandWithVaryingIndexingAddressing;
        operandWritersInitialized = True;
    }
    
    setRegisterAddressing(instruction);
    
    memcpy(&word, &instruction->opcode, sizeof(OpcodeLayout));
    
    setCurrentMemoryLocationType(codeSection, MemoryType_Absolute);
    
    writeWord(codeSection->memory, word);
    
    (*operandWriter[instruction->leftOperand.addressing])(codeSection, &instruction->leftOperand, sourceLine);
    
    (*operandWriter[instruction->rightOperand.addressing])(codeSection, &instruction->rightOperand, sourceLine);
    
    return codeSection->memory->position;
}

char getMemoryTypeSymbol(MemoryType memoryType)
{
    switch (memoryType)
    {
        case MemoryType_Absolute: return 'a';
        case MemoryType_External: return 'e';
        case MemoryType_Relocatable: return 'r';
        default: return 'u';
    }
}

void printCodeSection(CodeSection *codeSection)
{
    Memory *memory = codeSection->memory;
    int i;
    
    printf("\n\n === CODE SECTION: === \n\n");
    
    for (i = 0; i < memory->position; i++)
    {
        printf("%d:\t", i);
        
        printWord(memory->buffer[i]);
        
        printf(" %c", getMemoryTypeSymbol(codeSection->memoryType[i]));
        
        printf("\n\n");
    }
}

void setCodeBaseAddress(CodeSection *codeSection, Word address)
{
    codeSection->codeBaseAddress = address;
}

int getAbsoluteInstructionCounter(CodeSection *codeSection)
{
    return codeSection->memory->position + codeSection->codeBaseAddress.value;
}

int getRelativeInstructionCounter(CodeSection *codeSection)
{
    return codeSection->memory->position;
}