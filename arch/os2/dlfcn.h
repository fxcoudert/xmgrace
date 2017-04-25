/* dlfcn.h */
/* Implementation of dlopen() interface for OS/2               */
/* This code is released into public domain                    */

#ifndef DLFCN_H
#define DLFCN_H

#if defined (__cplusplus)
extern "C" {
#endif

extern void  *dlopen  (const char *filename, int flag);
extern char  *dlerror (void);
extern void  *dlsym   (void *handle, char *symbol);
extern int    dlclose (void *handle);


/*
   We do not actually use the definitions below but have
   to keep them for compatibility ... 
   Values taken from linux
*/

#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL	0x100

#if defined (__cplusplus)
}
#endif

#endif /* not DLFCN_H */
