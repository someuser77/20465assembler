/* 
 * File:   consts.h
 * Author: daniel
 *
 * Created on July 11, 2013, 11:16 AM
 */

#ifndef CONSTS_H
#define	CONSTS_H

#define LABEL_TOKEN ':'
#define COMMENT_TOKEN ';'
#define GUIDANCE_TOLEN '.'
#define DATA_GUIDANCE_TOKEN ".data"
#define DATA_GUIDANCE_TOKEN_LENGTH strlen(DATA_GUIDANCE_TOKEN)
#define STRING_GUIDANCE_TOKEN ".string"
#define STRING_GUIDANCE_TOKEN_LENGTH strlen(STRING_GUIDANCE_TOKEN)
#define ENTRY_GUIDANCE_TOKEN ".emtry"
#define ENTRY_GUIDANCE_TOKEN_LENGTH strlen(ENTRY_GUIDANCE_TOKEN)
#define EXTERN_GUIDANCE_TOKEN ".extern"
#define EXTERN_GUIDANCE_TOKEN_LENGTH strlen(EXTERN_GUIDANCE_TOKEN)

#define LINE_BUFFER_LENGTH 255
#define EOL '\0'
#endif	/* CONSTS_H */

