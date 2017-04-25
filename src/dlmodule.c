/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-99 Grace Development Team
 * 
 * Maintained by Evgeny Stambulchik
 * 
 * 
 *                           All Rights Reserved
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * dlmodule.c - DLL stuff for Grace
 * The following interfaces are supported:
 *  + dlopen() (Linux, SunOS, Solaris, OSF, IRIX, AIX-4, UnixWare, ...)
 *  + shl_load() (HP/UX)
 *  + AIX-3 - there is a free dlopen() emulation library
 *  - VMS ??
 */

#include <config.h>

#if defined(HAVE_DLOPEN)
#  include <dlfcn.h>
#endif

#if defined(HAVE_SHL_LOAD)
#  include <dl.h>
#endif

#include <string.h>

#include "dlmodule.h"

#include "defines.h"
#include "globals.h"
#include "utils.h"
#include "parser.h"

int dl_load_fast = TRUE; /* controls type of DL module load */
/* TODO: make it tunable through a command */

int load_module(char *fname, char *dl_function, char *dl_key, int dl_type)
{
#if defined(HAVE_DL)

    int dlflag;
    void *handle;
    const char *error;
    symtab_entry newkey;
    int retval;
    
    if ((dl_type < 0) || (dl_key == NULL) || (dl_function == NULL)) {
        errmsg("Improper call to load_module()");
	return RETURN_FAILURE;
    }
    
#if defined(HAVE_DLOPEN)
#  if defined(HAVE_RTLD_NOW)
    if (dl_load_fast == TRUE) {
        dlflag = RTLD_LAZY;
    } else {
        dlflag = RTLD_NOW;
    }
#  else
    dlflag = 1;
#  endif
    
    handle = dlopen(fname, dlflag);
    if (!handle) {
        errmsg(dlerror());
        return RETURN_FAILURE;
    }
    
    newkey.data = dlsym(handle, dl_function);
    if (!newkey.data && (error = dlerror()) != NULL) {
        errmsg(error);
        dlclose(handle);
        return RETURN_FAILURE;
    }

#endif /* end dlopen interface */

#if defined(HAVE_SHL_LOAD)

    if (dl_load_fast == TRUE) {
        dlflag = BIND_DEFERRED;
    } else {
        dlflag = BIND_IMMEDIATE;
    }
    
    handle = (void *) shl_load (fname, dlflag, 0L);
    if (!handle) {
#if defined(HAVE_STRERROR)
        errmsg(strerror(errno));
#else
# if defined(HAVE_SYS_ERRLIST_DECL)
        errmsg(sys_errlist[errno]);
# else
        errmsg("DL module initialization failed");
# endif
#endif
        return RETURN_FAILURE;
    }
    
    if (shl_findsym(handle, dl_function, TYPE_UNDEFINED, &newkey.data) != NULL) {
#if defined(HAVE_STRERROR)
        errmsg(strerror(errno));
#else
# if defined(HAVE_SYS_ERRLIST_DECL)
        errmsg(sys_errlist[errno]);
# else
        errmsg("Error while resolving symbol");
# endif
#endif
        shl_unload(handle);
        return RETURN_FAILURE;
    }

#endif /* end shl_load interface */

    newkey.type = dl_type;
    newkey.s = copy_string(NULL, dl_key);
    
    retval = addto_symtab(newkey);
    xfree(newkey.s);
    return retval;

#else /* no support for DL */
    errmsg("No support for DL modules on your OS");
    return RETURN_FAILURE;
#endif
}
