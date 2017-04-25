/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2000 Grace Development Team
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

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "globals.h"
#include "graphutils.h"
#include "utils.h"
#include "device.h"

static unsigned int ndevices = 0;
static int curdevice = 0;
static Device_entry *device_table = NULL;

int is_valid_page_geometry(Page_geometry pg)
{
    if (pg.width  > 0 &&
	pg.height > 0 &&
        pg.dpi > 0.0) {
	return TRUE;
    } else {
        return FALSE;
    }
}

int set_page_geometry(Page_geometry pg)
{
    if (is_valid_page_geometry(pg) == TRUE) {
        device_table[curdevice].pg = pg;
	return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

Page_geometry get_page_geometry(void)
{
    return (device_table[curdevice].pg);
}

int set_page_dimensions(int wpp, int hpp, int rescale)
{
    int i;
    
    if (wpp <= 0 || hpp <= 0) {
        return RETURN_FAILURE;
    } else {
	if (rescale) {
            int wpp_old, hpp_old;
            
            get_device_page_dimensions(curdevice, &wpp_old, &hpp_old);
            if (hpp*wpp_old - wpp*hpp_old != 0) {
                /* aspect ratio changed */
                double ext_x, ext_y;
                double old_aspectr, new_aspectr;
                
                old_aspectr = (double) wpp_old/hpp_old;
                new_aspectr = (double) wpp/hpp;
                if (old_aspectr >= 1.0 && new_aspectr >= 1.0) {
                    ext_x = new_aspectr/old_aspectr;
                    ext_y = 1.0;
                } else if (old_aspectr <= 1.0 && new_aspectr <= 1.0) {
                    ext_x = 1.0;
                    ext_y = old_aspectr/new_aspectr;
                } else if (old_aspectr >= 1.0 && new_aspectr <= 1.0) {
                    ext_x = 1.0/old_aspectr;
                    ext_y = 1.0/new_aspectr;
                } else {
                    ext_x = new_aspectr;
                    ext_y = old_aspectr;
                }

                rescale_viewport(ext_x, ext_y);
            } 
        }
        for (i = 0; i < ndevices; i++) {
            Page_geometry *pg = &device_table[i].pg;
            pg->width  = (unsigned long) rint((double) wpp*(pg->dpi/72));
            pg->height = (unsigned long) rint((double) hpp*(pg->dpi/72));
        }
        return RETURN_SUCCESS;
    }
}

int get_device_page_dimensions(int dindex, int *wpp, int *hpp)
{
    if (dindex >= ndevices || dindex < 0) {
        return RETURN_FAILURE;
    } else {
        Page_geometry *pg = &device_table[dindex].pg;
        *wpp = (int) rint((double) pg->width*72/pg->dpi);
        *hpp = (int) rint((double) pg->height*72/pg->dpi);
        return RETURN_SUCCESS;
    }
}

int register_device(Device_entry device)
{
    int dindex;
    
    ndevices++;
    dindex = ndevices - 1;
    device_table = xrealloc(device_table, ndevices*sizeof(Device_entry));

    device_table[dindex] = device;
    device_table[dindex].name = copy_string(NULL, device.name);
    device_table[dindex].fext = copy_string(NULL, device.fext);
    
    return dindex;
}

int select_device(int dindex)
{
    if (dindex >= ndevices || dindex < 0) {
        return RETURN_FAILURE;
    } else {
        curdevice = dindex;
	return RETURN_SUCCESS;
    }
}

/*
 * set the current print device
 */
int set_printer(int device)
{
    if (device >= ndevices || device < 0 ||
        device_table[device].type == DEVICE_TERM) {
        return RETURN_FAILURE;
    } else {
        hdevice = device;
        if (device_table[device].type != DEVICE_PRINT) {
            set_ptofile(TRUE);
        }
	return RETURN_SUCCESS;
    }
}

int set_printer_by_name(char *dname)
{
    int device;
    
    device = get_device_by_name(dname);
    
    return set_printer(device);
}

int get_device_by_name(char *dname)
{
    int i;
    
    i = 0;
    while (i < ndevices) {
        if (strncmp(device_table[i].name, dname, strlen(dname)) == 0) {
            break;
        } else {
            i++;
        }
    }
    if (i >= ndevices) {
        return -1;
    } else {
	return i;
    }
}

int initgraphics(void)
{
    return ((*device_table[curdevice].init)());
}

Device_entry get_device_props(int device)
{
    return (device_table[device]);
}

Device_entry get_curdevice_props()
{
    return (device_table[curdevice]);
}

char *get_device_name(int device)
{
    return (device_table[device].name);
}

void *get_curdevice_data(void)
{
    return (device_table[curdevice].data);
}

void set_curdevice_data(void *data)
{
    device_table[curdevice].data = data;
}

int set_device_props(int deviceid, Device_entry device)
{
    if (deviceid >= ndevices || deviceid < 0 ||
        is_valid_page_geometry(device.pg) != TRUE) {
        return RETURN_FAILURE;
    }
    
    device_table[deviceid].type = device.type;
/*
 *     device_table[deviceid].init = device.init;
 *     device_table[deviceid].parser = device.parser;
 *     device_table[deviceid].setup = device.setup;
 */
    device_table[deviceid].devfonts = device.devfonts;
    device_table[deviceid].fontaa = device.fontaa;
    device_table[deviceid].pg = device.pg;
    device_table[deviceid].data = device.data;

    return RETURN_SUCCESS;
}

void set_curdevice_props(Device_entry device)
{
    set_device_props(curdevice, device);
}

int parse_device_options(int dindex, char *options)
{
    char *p, *oldp, opstring[64];
    int n;
        
    if (dindex >= ndevices || dindex < 0 || 
            device_table[dindex].parser == NULL) {
        return RETURN_FAILURE;
    } else {
        oldp = options;
        while ((p = strchr(oldp, ',')) != NULL) {
	    n = MIN2((p - oldp), 64 - 1);
            strncpy(opstring, oldp, n);
            opstring[n] = '\0';
            if (device_table[dindex].parser(opstring) != RETURN_SUCCESS) {
                return RETURN_FAILURE;
            }
            oldp = p + 1;
        }
        return device_table[dindex].parser(oldp);
    }
}

int number_of_devices(void)
{
    return (ndevices);
}

void get_page_viewport(double *vx, double *vy)
{
    *vx = device_table[curdevice].pg.width/device_table[curdevice].pg.dpi;
    *vy = device_table[curdevice].pg.height/device_table[curdevice].pg.dpi;
    if (*vx < *vy) {
        *vy /= *vx;
        *vx = 1.0;
    } else {
        *vx /= *vy;
        *vy = 1.0;
    }
}

int terminal_device(void)
{
    if (device_table[curdevice].type == DEVICE_TERM) {
        return TRUE;
    } else {
        return FALSE;
    }
}

PageFormat get_page_format(int device)
{
    Page_geometry pg;
    int width_pp, height_pp;
    
    pg = device_table[device].pg;
    width_pp  = (int) rint((double) 72*pg.width/pg.dpi);
    height_pp = (int) rint((double) 72*pg.height/pg.dpi);
    
    if ((width_pp == 612 && height_pp == 792) ||
        (height_pp == 612 && width_pp == 792)) {
        return PAGE_FORMAT_USLETTER;
    } else if ((width_pp == 595 && height_pp == 842) ||
               (height_pp == 595 && width_pp == 842)) {
        return PAGE_FORMAT_A4;
    } else {
        return PAGE_FORMAT_CUSTOM;
    }
}

/*
 * flag to indicate destination of hardcopy output,
 * ptofile = 0 means print to printer, otherwise print to file
 */

static int ptofile = FALSE;
                           
void set_ptofile(int flag)
{
    ptofile = flag;
}

int get_ptofile(void)
{
    return ptofile;
}
