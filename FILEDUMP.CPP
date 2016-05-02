// fread.cpp
// By Tom Weatherhead			    July 25, 1994

#include <stdlib.h>
#include <stdio.h>


void main( void )
{
    char filename[80], buf[20];
    int bytes_read, total_size = 0, i, gulp;
    FILE * fp;

    printf( "Filename: " );
    scanf( "%s", filename );
    fp = fopen( filename, "r" );

    if( fp == NULL ) exit( 0 );

    do {
    	printf( "Bytes to read at a time: " );
    	scanf( "%d", &gulp );
    } while( gulp == 0  ||  gulp > 16 );

    for( ; ; ) {
	bytes_read = fread( buf, 1, gulp, fp );

	if( bytes_read == 0 ) break;

	total_size += bytes_read;

	for( i = 0; i < bytes_read; i++ ) {
	    printf( "%02x  ", (unsigned int)(unsigned char)(buf[i]) );
	}

	printf( "\n" );

        if( bytes_read < gulp ) break;
    }

    printf( "\nTotal size: %d\n", total_size );
    fclose( fp );
} // main()
