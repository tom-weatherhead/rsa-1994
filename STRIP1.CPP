// strip1.cpp - strips beginning, ending from a file's lines
// By Tom Weatherhead                   September 3, 1994

#include <stdio.h>
#include <string.h>

#define LINE_WIDTH                      140


void main( void )
{
    static char line[LINE_WIDTH], in_name[80], out_name[80], stripstr[20],
      killstr[20];
    int stripstrlen, killstrlen, killkeeplen, linelen, num_found = 0,
      num_killed = 0, linenum;
    FILE * in, * out;

    printf( "Input filename: " );
    scanf( "%s", in_name );
    in = fopen( in_name, "r" );

    if( in == NULL ) {
	printf( "Can't read %s\n", in_name );
	return;
    }

    printf( "Output filename: " );
    scanf( "%s", out_name );
    out = fopen( out_name, "w" );

    if( out == NULL ) {
	printf( "Can't write to %s\n", out_name );
	fclose( in );
	return;
    }

    printf( "Strip string ([K): " );
    scanf( "%s", stripstr );
    stripstrlen = strlen( stripstr );

    printf( "Delete part of string starting with ([7mShown): " );
    scanf( "%s", killstr );
    killstrlen = strlen( killstr );
    printf( "Keep last ? chars (61): " );
    scanf( "%d", &killkeeplen );

    for( linenum = 0; !feof( in ); linenum++ ) {
	fgets( line, sizeof( line ), in );

	if( feof( in ) ) break;

	linelen = strlen( line );

	if( linelen == 0 ) continue;	// if blank line

	if( line[linelen-1] == '\n' ) {         // Truncate newline
	    linelen--;
	    line[linelen] = '\0';
	}

	if( !strcmp( line + linelen - stripstrlen, stripstr ) ) {
	    num_found++;
            linelen -= stripstrlen;
	    line[linelen] = '\0'; // Truncate
	}

	if( !memcmp( killstr, line, killstrlen ) ) {
	    memcpy( line, line + linelen - killkeeplen, killkeeplen + 1 );
//	    linelen = killkeeplen;
            num_killed++;
        }

	fprintf( out, "%s\n", line );
    } // for

    fclose( in );
    fclose( out );
    printf( "All done!  %d lines, %d trimmed, %d killed\n", linenum,
      num_found, num_killed );
} // main()
