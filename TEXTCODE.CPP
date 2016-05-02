// textcode.cpp - uuencode, uudecode
// By Tom Weatherhead                           July 4, 1994

// This code should be portable, except for the path separator
// character: '\\' for DOS, '/' for UNIX.


#include <stdio.h>
#include <string.h>

#define PATH_SEP		'\\'
#define UUENCODE_LEN            45

// Here, uuencode(0) == '`'.
// Some encoders render uuencode(0) as ' '.
// The decoder here accepts either.
#define uuencode(xxx)           (xxx==0)?(int)'`':xxx+32
#define uudecode(xxx)           (xxx=='`')?0:xxx-32;


void textencode( void )		// ie. uue
{
    static char in_filename[80], out_filename[80];
    char outbuf[5], line[UUENCODE_LEN], * ptr;
    int bytes_read, bytes_writ, bits, linenum;
    FILE * in_stream, * out_stream;


    printf( "Input file: " );
    scanf( "%s", in_filename );

    if( (in_stream = fopen( in_filename, "rb" )) == NULL ) {
	printf( "Can't open input file '%s'\n", in_filename );
	return;
    }

    printf( "Output file: " );
    scanf( "%s", out_filename );

    if( (out_stream = fopen( out_filename, "w" )) == NULL ) {
	printf( "Can't open output file '%s'\n",
	  out_filename );
	fclose( in_stream );
	return;
    }

    printf( "Working...\n" );
    fprintf( out_stream, "begin 666 %s\n", in_filename );
    outbuf[4] = '\0';

    for( linenum = 1; ; linenum++ ) { // read in 45-byte chunks
	memset( line, 0, sizeof(line) );
	bytes_read = fread( line, 1, sizeof(line), in_stream );

	if( bytes_read == 0 ) break;

	// print length character
	fputc( uuencode(bytes_read), out_stream );
	bytes_writ = 0;

	for( ptr = line; bytes_read > 0; bytes_read -= 3, ptr += 3 ) {
	    // chars at or beyond ptr[bytes_read] are 0
	    bits = (ptr[0]>>2)&0x3f;
	    outbuf[0] = uuencode(bits);
	    bits = ((ptr[0]<<4)&0x30) | ((ptr[1]>>4)&0x0f);
	    outbuf[1] = uuencode(bits);
	    bits = ((ptr[1]<<2)&0x3c) | ((ptr[2]>>6)&0x03);
	    outbuf[2] = uuencode(bits);
	    bits = ptr[2]&0x3f;
	    outbuf[3] = uuencode(bits);
	    fprintf( out_stream, "%s", outbuf );
	    bytes_writ += 4;
	} // for

	fputc( '\n', out_stream );
    } // for

    printf( "Done; %d uuencoded lines written\n", linenum - 1 );
    fprintf( out_stream, "%c\nend\n", uuencode(0) );
    fclose( in_stream );
    fclose( out_stream );
} // textencode()


void textdecode( void )		// ie. uud
{
    int reading = 0;    // FALSE
    // filename, out_path not used concurrently; could be merged
    static char line[80], filename[80], out_path[100];
    char * inptr, outbuf[4], cmd[10], * chp;
    int i, num_to_write, bits[4], prot_mask, linenum, out_path_len;
    FILE * in_stream, * out_stream = NULL;

    printf( "Input file: " );
    scanf( "%s", filename );

    if( (in_stream = fopen( filename, "r" )) == NULL ) {
	printf( "Can't open input file '%s'\n", filename );
	return;
    }

    for( ; ; ) {	// Don't accept empty input
	printf( "Output directory (excluding filename)\n: " );
        // scanf probably ensures non-empty input, but be sure
    	scanf( "%s", out_path );
	out_path_len = strlen( out_path );

	if( out_path_len > 0 ) break;

	printf( "\nYou must enter a non-empty pathname\n" );
    }

    chp = out_path + out_path_len - 1;  // Points to last char
    // The following is DOS-specific; similar for UNIX ('/', not '\\'; no ':')

    if( *chp != PATH_SEP  &&  *chp != ':' ) {
    	chp++;
	*chp = PATH_SEP;	// Append path separator if needed
    }

    printf( "Working...\n" );

    for( linenum = 1; !feof( in_stream ); linenum++ ) {
	memset( line, '\0', sizeof( line ) );
	fgets( line, sizeof( line ), in_stream );

	if( !reading ) {

	    if( memcmp( line, "begin", 5 ) ) continue;

	    reading = 1;        // TRUE

	    if( out_stream != NULL ) continue; // already open

	    sscanf( line + 6, "%d %s", &prot_mask, chp + 1 );
	    printf( "Output pathname: %s\n", out_path );
	    out_stream = fopen( out_path, "wb" );

	    if( out_stream == NULL ) {
		printf( "Error opening output file\n" );
		fclose( in_stream );
		return;
	    }

	    continue;
	} else if( !memcmp( line, "end", 3 ) ) {
	    reading = 0;        // FALSE
	    continue;
	}

	if( strlen( line ) > 62 ) {
	    printf( "Line %d too long\n", linenum );
	    fclose( in_stream );
	    fclose( out_stream );
	    return;
	}

	num_to_write = uudecode(line[0]);

	for( inptr = line + 1; num_to_write > 0;
	  inptr += 4, num_to_write -= 3 ) {

	    for( i = 0; i < 4; i++ ) {

		if( inptr[i] < ' '  ||  inptr[i] > '`' ) {
		    printf( "Illegal char %d in line %d\nContinue? (y/n): ",
		      (int)(inptr[i]), linenum );
		    scanf( "%s", cmd );

		    if( cmd[0] != 'y' ) {
			fclose( in_stream );
			fclose( out_stream );
			return;
		    }
		}

		bits[i] = uudecode(inptr[i]);
	    }

	    outbuf[0] = (bits[0]<<2) | ((bits[1]&0x30)>>4);
	    outbuf[1] = ((bits[1]&0x0f)<<4) | ((bits[2]&0x3c)>>2);
	    outbuf[2] = ((bits[2]&0x03)<<6) | (bits[3]&0x3f);
	    fwrite( outbuf, 1, (num_to_write>3)?3:num_to_write,
	      out_stream );
	} // for
    } // while

    fclose( in_stream );

    if( out_stream != NULL ) {
	fclose( out_stream );
    }

    printf( "Done; %d lines uudecoded\n", linenum - 1 );
} // textdecode()

// ******** End of textcode.cpp ********
