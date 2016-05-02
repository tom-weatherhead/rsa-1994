// bignum.cpp
// By Tom Weatherhead                         July 16, 1994

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "rsa.h"


bignum_class::bignum_class( void )
{
    buf = NULL;
    len = buf_size = 0;
} // bignum_class::bignum_class


bignum_class::~bignum_class( void )
{
    if( buf == NULL ) return;

    free( buf );
    bignum_class();     // In case the object is reused
} // bignum_class::~bignum_class


void bignum_class::resize( unsigned short size )
{
    size = (1 + (size - 1) / RESIZE_INC) * RESIZE_INC;

    if( buf_size < size ) {
	buf_size = size;
	// printf( "Resizing to %d\n", buf_size );
	buf = (unsigned char *)( (buf == NULL)
	  ? malloc( buf_size )
	  : realloc( buf, buf_size ) );
    }

    memset( buf + len, 0, buf_size - len );
} // bignum_class::resize


unsigned int bignum_class::to_int( void )
{
    unsigned char * ptr = buf;
    unsigned int i, rtn = 0, mult = 1;

    if( len > 4 ) {
	printf( "num won't fit in uns. int; len == %d\n", len );
	return( 0 );
    }

    for( i = 0; i < len; i++ ) {
	rtn += mult * (unsigned int)*ptr;
	ptr++;
	mult <<= 8;     /* mult *= 2; */
    }

    return( rtn );
} // bignum_class::to_int


void bignum_class::print( void )
{
    int i;

    if( len == 0 ) {
	printf( "0\n" );
	return;
    }

    printf( "0x" );

    for( i = len - 1; i >= 0; i-- ) {
	printf( "%02x", buf[i] );
    }

    printf( "\n" );
} // bignum_class::print


void bignum_class::trim( void )
{
    unsigned int i;

    for( i = len; i > 0; i-- ) {

	if( buf[i-1] != 0x0 ) break;
    }

    len = i;
} // bignum_class::trim


// Zero-pad to a length of newlen

void bignum_class::zero_pad( unsigned short newlen )
{
    if( newlen <= (int)len ) return;    // newlen may be negative

    resize( newlen );                   // Increase size, if needed
    memset( buf + len, 0, newlen - len );
    len = newlen;
} // bignum_class::zero_pad


void bignum_class::complement( void )
{
    unsigned int i;

    for( i = 0; i < len; i++ ) {
	buf[i] = ~(buf[i]);
    }
} // bignum_class::complement


void bignum_class::inc( void )  // Redundant?
{
    unsigned int i;

    for( i = 0; i < len; i++ ) {

	if( buf[i] == 0xff ) {
	    buf[i] = 0;         /* Carry */
	} else break;
    }

    if( i == len ) {
	resize( len + 1 );
	buf[len++] = 1;
    } else {
	buf[i]++;
    }
} // bignum_class::inc


void bignum_class::dec( void )  // Redundant?
{
    unsigned int i;

    for( i = 0; i < len; i++ ) {

	if( buf[i] == 0x0 ) {
	    buf[i] = 0xff;              // Borrow
	} else break;
    }

    if( i == len ) {
	printf( "ERROR: tried to decrement 0\n" );
	exit( 1 );
    }

    buf[i]--;

    if( buf[i] == 0  &&  i == len - 1 ) {
	len--;  // No trimming necessary
    }
} // bignum_class::dec


void bignum_class::ror( void ) // Bitwise rotate right
{
    unsigned int i;

    // i < len catches buf == NULL cases

    for( i = 0; i < len; i++ ) {
	buf[i] >>= 1;

	if( i >= len - 1 ) {

	    if( buf[i] == 0x0 ) {
		len--;
	    }

	    break;
	}

	if( buf[i+1] & 0x1 ) {
	    buf[i] |= 0x80;
	}
    }
} // bignum_class::ror


void bignum_class::random( unsigned short newlen )
{
    unsigned int i;

    resize( newlen );

    for( i = 0; i < newlen; i++ ) {
	// >> 8 to get random low bits
	buf[i] = (rand() >> 8) & 0xff;
    }

    len = newlen;
    trim();
} // bignum_class::random


void bignum_class::fileread( FILE * fp )
{
    int rtn;

    rtn = fread( (char *)&len, 1, sizeof( len ), fp );
    assert( rtn == sizeof( len ) );
    assert( len > 0 );
    resize( len );
    assert( buf != NULL );
    rtn = fread( buf, 1, len, fp );
    assert( rtn == (int)len );
} // bignum_class::fileread


void bignum_class::filewrite( FILE * fp )
{
    int rtn;

    assert( len > 0 );
    rtn = fwrite( (char *)&len, 1, sizeof( len ), fp );
    assert( rtn == sizeof( len ) );
    assert( buf_size >= len );
    assert( buf != NULL );
    rtn = fwrite( buf, 1, len, fp );
    assert( rtn == (int)len );
} // bignum_class::filewrite


// ******** End of bignum.cpp ********
