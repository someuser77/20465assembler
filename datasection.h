/* 
 * File:   data.h
 * Author: daniel
 *
 * Created on July 27, 2013, 3:22 PM
 */

#ifndef DATA_H
#define	DATA_H

#include "types.h"

typedef struct 
{
    Memory memory;    
} DataSection;

#define DATA_WRITE_ERROR -1

int writeDataArray(DataSection *dataSection, SourceLinePtr sourceLine);
int writeDataString(DataSection *dataSection, SourceLinePtr sourceLine);

#endif	/* DATA_H */

