#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "codesection.h"
#include "memory.h"
#include "symboltable.h"
#include "logging.h"

#define REGISTER_DIGIT_INDEX 1

typedef Boolean (*OperandWriter)(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine);

CodeSection *initCodeSection(SymbolTablePtr symbolTable)
{
    CodeSection *codeSection;
    int i;
    
    codeSection = (CodeSection *)malloc(sizeof(CodeSection));
    codeSection->symbolTable = symbolTable;
    codeSection->memory = initMemory();
    codeSection->codeBaseAddress.value = BASE_ADDRESS;
    codeSection->memoryType = (MemoryType *)malloc(sizeof(MemoryType) * MAX_MEMORY_SIZE);
    codeSection->externalSymbols = initList(NodeType_SymbolLocation);
    
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

Boolean writeOffsetToSymbol(CodeSection *codeSection, char *symbol, Word instructionAddress, SourceLinePtr sourceLine)
{
    Word symbolAddress;
    int offset;
    
    SymbolPtr symbolPtr;
    
    if (symbol == NULL)
    {
        logError("Symbol address to write was NULL.");
        return False;
    }
    
    symbolPtr = findSymbol(codeSection->symbolTable, symbol);
    
    if (symbolPtr == NULL)
    {
        logErrorInLineFormat(sourceLine, "Unable to find symbol '%s' in symbol table.\n", symbol);
        return False;
    }
    
    if (symbolPtr->symbolSection == EXTERN_SYMBOL_VALUE)
    {
        logErrorInLineFormat(sourceLine, "Unable to calculate offset to an external symbol '%s'.\n", symbol);
        return False;
    }
    
    symbolAddress = symbolPtr->value;
    
    offset = symbolAddress.value - instructionAddress.value; 
    
    setCurrentMemoryLocationType(codeSection, MemoryType_Absolute);
    
    writeInt(codeSection->memory, offset);
    
    return True;
}

Boolean writeSymbolAddress(CodeSection *codeSection, char *symbol, SourceLinePtr sourceLine)
{
    Word symbolAddress;
    Word symbolLocation;
    SymbolPtr symbolPtr;
    MemoryType memoryType;
    ListNodeDataPtr dataPtr;
    
    if (symbol == NULL)
    {
        logError("Symbol address to write was NULL.");
        return False;
    }
    
    symbolPtr = findSymbol(codeSection->symbolTable, symbol);
    
    if (symbolPtr == NULL)
    {
        logErrorInLineFormat(sourceLine, "Unable to find symbol '%s' in symbol table.\n", symbol);
        return False;
    }
    
    symbolAddress = symbolPtr->value;
    memoryType = MemoryType_Relocatable;
    
    if (symbolAddress.value == EXTERN_SYMBOL_VALUE)
    {
        symbolAddress.value = DEFAULT_EXTERNAL_SYMBOL_ADDRESS;
        memoryType = MemoryType_External;
        
        dataPtr = (ListNodeDataPtr)malloc(sizeof(ListNodeData));
        symbolLocation = getAbsoluteInstructionCounter(codeSection);
        dataPtr->symbolLocation = initSymbolLocation(symbol, symbolLocation);
        
        insertNode(&codeSection->externalSymbols, dataPtr, NodeType_SymbolLocation);
    }
    
    setCurrentMemoryLocationType(codeSection, memoryType);
    
    writeWord(codeSection->memory, symbolAddress);
    
    return True;
}

Boolean writeInstantAddress(CodeSection *codeSection, Word address)
{
    setCurrentMemoryLocationType(codeSection, MemoryType_Absolute);
    writeWord(codeSection->memory, address);
    return True;
}

void setSourceRegister(InstructionLayoutPtr instruction, int registerId)
{
    instruction->opcode.source_register = registerId;
}

void setDestinationRegister(InstructionLayoutPtr instruction, int registerId)
{
    instruction->opcode.dest_register = registerId;
}

Boolean writeOperandWithInstantAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    if (operand->empty) return True;

    writeInstantAddress(codeSection, operand->address.value);
    
    return True;
}

Boolean writeOperandWithDirectAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    if (operand->empty) return True;
    
    return writeSymbolAddress(codeSection, operand->address.label, sourceLine);
}

Boolean writeOperandWithDirectRegisterAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    /* just a place holder */
    return True;
}

Boolean writeOperandWithVaryingIndexingAddressing(CodeSection *codeSection, OperandPtr operand, SourceLinePtr sourceLine)
{
    if (operand->empty) return True;
    
    writeSymbolAddress(codeSection, operand->address.varyingAddress.label, sourceLine);
    
    switch (operand->address.varyingAddress.adressing)
    {
        case OperandVaryingAddressing_Direct:
            return writeOffsetToSymbol(codeSection, operand->address.varyingAddress.address.label, operand->instruction->instructionAddress , sourceLine);
        case OperandVaryingAddressing_Instant:
            return writeInstantAddress(codeSection, operand->address.varyingAddress.address.value);
        case OperandVaryingAddressing_DirectRegister:
            /* should have been handled earlier. */
            break;
    }
    return True;
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
    
    Boolean result;
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
    
    result = (*operandWriter[instruction->leftOperand.addressing])(codeSection, &instruction->leftOperand, sourceLine);
    
    if (!result)
    {
        return -1;
    }
    
    result = (*operandWriter[instruction->rightOperand.addressing])(codeSection, &instruction->rightOperand, sourceLine);
    
    if (!result)
    {
        return -1;
    }
    
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

void writeCodeSection(CodeSection *codeSection, FILE *file)
{
    Memory *memory = codeSection->memory;
    int i;
    
    for (i = 0; i < memory->position; i++)
    {
        fprintf(file, "%o\t", i + codeSection->codeBaseAddress.value);
        
        printWord(memory->buffer[i], file, OUTPUT_BASE);
        
        fprintf(file, "\t%c", getMemoryTypeSymbol(codeSection->memoryType[i]));
        
        fprintf(file, "\n");
    }
}

void printCodeSection(CodeSection *codeSection)
{
    printf("\n\n === CODE SECTION: === \n\n");
    
    writeCodeSection(codeSection, stdout);
    
    printf("\n");
}

Word getAbsoluteInstructionCounter(CodeSection *codeSection)
{
    Word word;
    word.value = codeSection->memory->position + codeSection->codeBaseAddress.value;
    return word;
}

Word getRelativeInstructionCounter(CodeSection *codeSection)
{
    Word word; 
    word.value = codeSection->memory->position;
    return word;
}

void fixDataOffsetForSymbolAddress(ListNodeDataPtr dataPtr, void *context)
{
    if (dataPtr->symbol.symbolSection == SymbolSection_Data)
    {
        dataPtr->symbol.value.value += *(int*)(context);
    }
}

/* used to adjust the offsest of the symbols pointing to data after we know the code size  */
void fixDataOffset(CodeSection *codeSection, int offset)
{
    actOnList(&codeSection->symbolTable->list, fixDataOffsetForSymbolAddress, &offset);
}

void writeExternalSymbol(ListNodeDataPtr nodeData, void *context)
{
    fprintf((FILE*)context, "%s\t%o\n", nodeData->symbolLocation->symbol, nodeData->symbolLocation->location.value);
}

void writeExternalSymbols(CodeSection *codeSection, FILE *file)
{
    actOnList(&codeSection->externalSymbols, writeExternalSymbol, file);
}

void printExternalSymbols(CodeSection *codeSection)
{
    printf("\n\n === EXTERNAL SYMBOLS === \n\n");
    
    writeExternalSymbols(codeSection, stdout);
}
