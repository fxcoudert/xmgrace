/*
 * grace_np - a library for interfacing with Grace using pipes
 * 
 * Copyright (c) 1997-1998 Henrik Seidel
 * Copyright (c) 1999-2000 Grace Development Team
 *
 *
 *                           All Rights Reserved
 * 
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Library General Public
 *    License as published by the Free Software Foundation; either
 *    version 2 of the License, or (at your option) any later version.
 * 
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 * 
 *    You should have received a copy of the GNU Library General Public
 *    License along with this library; if not, write to the Free
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GRACE_NPIPE_H_
#define GRACE_NPIPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* register a user function to print errors */
/* (the default function appends a newline and prints to standard error) */
typedef void (*GraceErrorFunctionType) (const char *);
GraceErrorFunctionType GraceRegisterErrorFunction(GraceErrorFunctionType f);

/* launch a grace subprocess and a communication channel with it */
int GraceOpenVA(char* exe, int bs, ...);

/* a simplified (obsolete) version of the above */
int GraceOpen(int bs);

/* test if a grace subprocess is currently connected */
int GraceIsOpen(void);

/* close the communication channel and exit the grace subprocess */
int GraceClose(void);

/* close the communication channel and leave the grace subprocess alone */
int GraceClosePipe(void);

/* flush all the data remaining in the buffer */
int GraceFlush(void);

/* format a command and send it to the grace subprocess */
int GracePrintf(const char*, ...);

/* send an already formated command to the grace subprocess */
int GraceCommand(const char*);

#ifdef __cplusplus
}
#endif

#endif /* GRACE_NPIPE_H */
