/* 
 * File:   logging.h
 * Author: daniel
 *
 * Created on July 27, 2013, 4:28 PM
 */

#ifndef LOGGING_H
#define	LOGGING_H

#include "types.h"

/* set of utility function to centralize the message output */
void logError(char *error, ...);
void logErrorInLine(SourceLinePtr line, char *error, ...);



#endif	/* LOGGING_H */

