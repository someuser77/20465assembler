#include <string.h>
#include "types.h"
#include "codesection.h"
#include "memory.h"
#include "symboltable.h"
#include "logging.h"

#define REGISTER_DIGIT_INDEX 1

typedef void (*OperandWriter)(CodeSection *codeSection, OperandPtr operand);

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

void writeSymbolAddress(CodeSection *codeSection, char *symbol)
{
    int address;
    SymbolPtr symbolPtr;
    
    if (symbol == NULL)
    {
        logError("Symbol address to write was NULL.");
        return;
    }
    
    symbolPtr = findSymbol(codeSection->symbolTable, symbol);
    
    if (symbolPtr == NULL)
    {
        logErrorFormat("Unable to find symbol '%s' in symbol table.\n", symbol);
        return;
    }
    
    address = symbolPtr->value;
    
    writeInt(&codeSection->memory, address);
}

void writeInstantAddress(CodeSection *codeSection, int address)
{
    writeInt(&codeSection->memory, address);
}

void setSourceRegister(InstructionLayoutPtr instruction, int registerId)
{
    instruction->opcode.source_register = registerId;
}

void setDestinationRegister(InstructionLayoutPtr instruction, int registerId)
{
    instruction->opcode.dest_register = registerId;
}

void writeOperandWithInstantAddressing(CodeSection *codeSection, OperandPtr operand)
{
    if (operand->empty) return;

    writeInstantAddress(codeSection, operand->address.value);
}

void writeOperandWithDirectAddressing(CodeSection *codeSection, OperandPtr operand)
{
    if (operand->empty) return;
    
    writeSymbolAddress(codeSection, operand->address.label);
}

void writeOperandWithDirectRegisterAddressing(CodeSection *codeSection, OperandPtr operand)
{
    /* just a place holder */
    return;
}

void writeOperandWithVaryingIndexingAddressing(CodeSection *codeSection, OperandPtr operand)
{
    if (operand->empty) return;
    
    writeSymbolAddress(codeSection, operand->address.varyingAddress.label);
    
    switch (operand->address.varyingAddress.adressing)
    {
        case OperandVaryingAddressing_Direct:
            writeSymbolAddress(codeSection, operand->address.varyingAddress.address.label);
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

int writeInstruction(CodeSection *codeSection, InstructionLayoutPtr instruction)
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
    
    writeWord(&codeSection->memory, word);
    
    (*operandWriter[instruction->leftOperand.addressing])(codeSection, &instruction->leftOperand);
    
    (*operandWriter[instruction->rightOperand.addressing])(codeSection, &instruction->rightOperand);
    
    return codeSection->memory.position;
}
