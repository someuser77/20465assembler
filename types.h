/* 
 * File:   types.h
 * Author: daniel
 *
 * Created on July 8, 2013, 10:12 PM
 */

#ifndef TYPES_H
#define	TYPES_H

#include "consts.h"


#define DESTINATION_OPERAND_LOW_NIBBLE 0
#define DESTINATION_OPERAND_HIGH_NIBBLE 1
#define SOURCE_OPERAND_LOW_NIBBLE 0
#define SOURCE_OPERAND_HIGH_NIBBLE 2

typedef enum {False = 0, True = 1} Boolean;

/* although the values are increasing the values
 * are set explicitly in case some one will add a value in the middle 
 * of the list and it will shift the other values. */
typedef enum {
    Opcode_mov = 0, 
    Opcode_cmp = 1, 
    Opcode_add = 2, 
    Opcode_sub = 3, 
    Opcode_not = 4, 
    Opcode_clr = 5, 
    Opcode_lea = 6, 
    Opcode_inc = 7, 
    Opcode_dec = 8, 
    Opcode_jmp = 9, 
    Opcode_bne = 10, 
    Opcode_red = 11, 
    Opcode_prn = 12,
    Opcode_jsr = 13, 
    Opcode_rts = 14, 
    Opcode_stop = 15
} Opcode;

typedef enum { InstructionOperandSize_Large = 0, InstructionOperandSize_Small = 1 } InstructionOperandSize;
typedef enum { InstructionRepetition_Single = 0, InstructionRepetition_Double = 1 } InstructionRepetition;
typedef enum { OperandTargetBits_HighNibble = 1, OperandTargetBits_LowNibble = 0 } OperandTargetBits;
typedef enum { OperandAddressing_Instant = 0, OperandAddressing_Direct = 1, OperandAddressing_VaringIndexing = 2, OperandAddressing_DirectRegister = 3 } OperandAddressing;

/* The memory representation of an opcode in the target machine */
typedef struct tOpcodeLayout {
    unsigned int comb : 2;
    unsigned int dest_register : 3;
    unsigned int dest_addressing : 2;
    unsigned int source_register : 3;
    unsigned int source_addressing : 2;
    unsigned int opcode : 4;
    unsigned int type : 1;
    unsigned int dbl : 1;
    unsigned int reserved : 2;
} OpcodeLayout, *OpcodeLayoutPtr;

typedef struct tInstructionLayout {
    OpcodeLayout opcode; /* this might be illegal :-) */ 
    unsigned int word1 : MACHINE_WORD_LENGTH;
    unsigned int word2 : MACHINE_WORD_LENGTH;
} InstructionLayout, *InstructionLayoutPtr;

typedef enum {Empty, Comment, Guide, Operation} StatentType;

typedef enum {GuidanceType_Data, GuidanceType_String, GuidanceType_Entry, GuidanceType_Extern} GuidanceType;

typedef struct tSourceLine
{
    /* a pointer to the position inside the line of code */
    char *text;
    
    /* a pointer to the beginning of the line*/
    char *start;
    
    /* the number of the line in the source file pointed by fileName */
    int lineNumber;
    
    /* the name of the file this line belongs to */
    char *fileName;
    
    /* an error associated with this line.
     * if NULL then this line contains to errors.
     * can be used by any code to specify errors specific to the line.
     * code is not required to append to the error and the existence of
     * any error should be enough to stop parsing the line */
    char *error;
} SourceLine, *SourceLinePtr;


#endif	/* TYPES_H */

