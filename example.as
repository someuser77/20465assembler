        ;comment

MAIN:   mov/0,0		LENGTH,r1
	lea/1/1/1,0	STR{*LENGTH}, r4
LOOP:	jmp/1/0.0,0	END
	prn/1/1/0,0	STR{r3}
	sub/0,0#1, r1
	inc/0,0		r0
	mov/0,1		r3,STR{7}
	bne/0,0		LOOP
END:	stop/0,0
STR:	.string "abcdef"
LENGT:	.data 6
K:	.data 2