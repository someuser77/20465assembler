/* 
 * File:   types.h
 * Author: daniel
 *
 * Created on July 8, 2013, 10:12 PM
 */

#ifndef TYPES_H
#define	TYPES_H

/* Instruction type declaration */

#define DESTINATION_OPERAND_LOW_NIBBLE 0
#define DESTINATION_OPERAND_HIGH_NIBBLE 1
#define SOURCE_OPERAND_LOW_NIBBLE 0
#define SOURCE_OPERAND_HIGH_NIBBLE 2

typedef enum {False = 0, True = 1} Boolean;

typedef enum {
    mov = 0, cmp, add, sub, not, clr, lea, inc, 
    dec, jmp, bne, red, prn, jsr, rts, stop
} Opcode;

typedef enum { Large = 0, Small = 1 } InstructionType;
typedef enum { Single = 0, Double = 1} InstructionRepetition;

typedef struct tInstruction {
    unsigned int comb : 2;
    unsigned int dest_register : 2;
    unsigned int dest_addressing : 2;
    unsigned int source_register : 2;
    unsigned int source_addressing : 2;
    Opcode opcode : 4;
    InstructionType type : 1;
    InstructionRepetition dbl : 1;
    unsigned int reserved : 2;
} Instruction;

/* statement type declaration */
typedef enum {Empty, Comment, Guide, Operation} StatentType;


#endif	/* TYPES_H */

