/*
 * Grace - Graphics for Exploratory Data Analysis
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
 * filter files before they are input and output
 *
 */
 
#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(HAVE_FNMATCH)
#  include <fnmatch.h>
#endif

#include "defines.h"
#include "utils.h"
#include "files.h"
#include "protos.h"

typedef struct filter {
	char *command;
	int method;
	char *id;
	int idlen;
} Filter;

Filter *ifilt, *ofilt;
int numIfilt=0;
int numOfilt=0;

int add_input_filter( int method, char *id, char *comm );
int add_output_filter( int method, char *id, char *comm );
static void hex2char( Filter *, char * );
static int test_magic( int len, char *magic, FILE *in );
static int test_pattern( char *ext, char *fn );

int add_io_filter( int type, int method, char *id, char *comm )
{
	if( type == FILTER_INPUT )
		return add_input_filter( method, id, comm );
	else if( type == FILTER_OUTPUT )
		return add_output_filter( method, id, comm );
	else
		return 1;
}

/*
 * if method == 0 -> PATTERN MATCHING else method = the number of bytes to
 * match in MAGIC NUMBER
 */
int add_input_filter( int method, char *id, char *comm )
{
	ifilt = xrealloc( ifilt, ++numIfilt*sizeof(Filter) );
	ifilt[numIfilt-1].command = copy_string(NULL, comm);
	strcpy( ifilt[numIfilt-1].command, comm );
	ifilt[numIfilt-1].method = method;
	if( method == FILTER_PATTERN ) {
		ifilt[numIfilt-1].id = xmalloc( strlen(id)+1 );
		strcpy( ifilt[numIfilt-1].id, id );
		ifilt[numIfilt-1].idlen = strlen( ifilt[numIfilt-1].id );
	} else {
		ifilt[numIfilt-1].id = xmalloc(strlen(id)/2+1);
		hex2char( &ifilt[numIfilt-1], id );
	}
	if( ifilt[numIfilt-1].idlen == 0 ) {
		numIfilt--;
		return 1;
	} else
		return 0;
}


int add_output_filter( int method, char *id, char *comm )
{
	ofilt = xrealloc( ofilt, ++numOfilt*sizeof(Filter) );
	ofilt[numOfilt-1].command = copy_string(NULL, comm);
	strcpy( ofilt[numOfilt-1].command, comm );
	ofilt[numOfilt-1].id = xmalloc( strlen(id)+1 );
	strcpy( ofilt[numOfilt-1].id, id );
	ofilt[numOfilt-1].method = FILTER_PATTERN;
	return 0;
}


/*
 * eliminate filters:
 * f = 0->output filters, f=1->input filters
 */
void clear_io_filters( int f )
{
	Filter *filt;
	int *i;
	
	if( f==FILTER_OUTPUT ){
		i = &numOfilt;
		filt = ofilt;
	} else {
		i = &numIfilt;
		filt = ifilt;
	}

	for( ; *i>0; (*i)-- ) {
		xfree( filt[*i-1].command );
		xfree( filt[*i-1].id );
	}
	xfree(filt);
}
	

/* 
 * filter input file and return pointer to a pipe
 */
FILE *filter_read( char *fn )
{
	char buf[1024];
	int i;
	FILE *in;
	
	/* check if file name o.k. */
	if( (in=fopen( fn, "rb")) == NULL )
		return NULL;

	for( i=0; i<numIfilt; i++ )
		if( ifilt[i].method==FILTER_PATTERN && test_pattern(ifilt[i].id, fn ))
				break;
		else if( ifilt[i].method==FILTER_MAGIC && 
							test_magic(ifilt[i].idlen, ifilt[i].id, in ) )
				break;

	if( i != numIfilt ){
		fclose( in );
		sprintf( buf, ifilt[i].command, fn );
		fflush( stdout );
		return popen(grace_exe_path(buf), "r");
	} else {
		return in;
	}
}


/*
 * filter output file and return pointer to a pipe or file
 */
FILE *filter_write(  char *fn )
{
	char buf[1024];
	int i;
	FILE *out;
	
	/* check if file name o.k. */
	if( (out=fopen( fn, "wb")) == NULL )
		return NULL;
		
	/* see if we get a match */
	for( i=0; i<numOfilt; i++ )
		if( ofilt[i].method==FILTER_PATTERN && test_pattern( ofilt[i].id, fn ) )
				break;
		else if( ofilt[i].method==FILTER_MAGIC && 
						test_magic( ofilt[i].method, ofilt[i].id, out ) )
				break;

	if( i != numOfilt ){
		fclose( out );
		sprintf( buf, ofilt[i].command, fn );
		fflush( stdin );
		return popen(grace_exe_path(buf), "w");
	} else {
		return out;
	}
}


/*
 * test for a magic number
 * if found at the beginning of the file, return 1
 * else 0
 */
static int test_magic( int len, char *magic, FILE *in )
{
	char buf[512], rstr[50];

	if( in == NULL )
		return 0;

	sprintf( rstr, "%%%dc", len );
	fscanf( in, rstr, buf );
	rewind( in );

	return !memcmp( buf, magic, len );
}

/*
 * test for a pattern match
 * if found return 1, else 0
 */
static int test_pattern( char *ext, char *fn )
{
#if defined(HAVE_FNMATCH)
	return !fnmatch( ext, fn, 0 );
#else
	/* you are out of luck */
	return 0;
#endif
}


/*
 * convert hex string to character string interpreting 2 hex digits as 1
 * byte 
 */
static void hex2char( Filter *f, char *hex )
{
	int i;
	char tmp[3], *ptr;

	tmp[2] = '\0';
	f->idlen = 0;
	for( i=0; i<strlen(hex)/2; i++ ) {
		tmp[0] = hex[2*i];
		tmp[1] = hex[2*i+1];
		f->id[i] = strtol( tmp, &ptr, 16 );
		if( f->id[i]==0 && ptr==NULL ){
			f->id[0] = '\0';
			f->idlen = 0;
			break;
		} else
			f->idlen++;
	}
	f->id[i] = '\0';
}
