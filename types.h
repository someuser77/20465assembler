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
    Opcode_mov = 0, Opcode_cmp, Opcode_add, Opcode_sub, Opcode_not, Opcode_clr, Opcode_lea, Opcode_inc, 
    Opcode_dec, Opcode_jmp, Opcode_bne, Opcode_red, Opcode_prn, Opcode_jsr, Opcode_rts, Opcode_stop
} Opcode;

typedef enum { InstructionOperandSize_Large = 0, InstructionOperandSize_Small = 1 } InstructionOperandSize;
typedef enum { Single = 0, Double = 1} InstructionRepetition;

typedef struct tInstructionRepresentation {
    unsigned int comb : 2;
    unsigned int dest_register : 2;
    unsigned int dest_addressing : 2;
    unsigned int source_register : 2;
    unsigned int source_addressing : 2;
    unsigned int opcode : 4;
    unsigned int type : 1;
    unsigned int dbl : 1;
    unsigned int reserved : 2;
} InstructionRepresentation, *InstructionRepresentationPtr;

/* statement type declaration */
typedef enum {Empty, Comment, Guide, Operation} StatentType;

typedef enum {GuidanceType_Data, GuidanceType_String, GuidanceType_Entry, GuidanceType_Extern} GuidanceType;

typedef struct tSourceLine
{
    /* a pointer to the position inside the text line */
    char *text;
    /* a pointer to the beginning of the line*/
    char *start;
    /* the number of the line in the source file */
    int lineNumber;
    /* the name of the file this line belogs to */
    char *fileName;
    /* an error associated with this line*/
    char *error;
} SourceLine, *SourceLinePtr;


#endif	/* TYPES_H */

