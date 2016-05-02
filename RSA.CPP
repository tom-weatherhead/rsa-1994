// rsa.cpp - Implementation of the RSA Encryption Scheme
// Adapted by Tom Weatherhead
// Based on pp. 831 - 844 of "Introduction to Algorithms",
// By Cormen, Leiserson, and Rivest
// Started February 1994


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "rsa.h"


void copy_bignum( bignum_class * src, bignum_class * dst )
{
    resize_if(dst,src->len);
    assert( dst->buf_size >= src->len );
    memcpy( dst->buf, src->buf, src->len );
    dst->len = src->len;
} // copy_bignum


void int_to_bignum( unsigned int in, bignum_class * out )
{
    unsigned int i;

    for( i = 0; in > 0; i++ ) {
	resize_if(out,i+1);
	out->buf[i] = (unsigned char)(in & 0xff);
	in >>= 8;
    }

    out->len = i;
} // int_to_bignum


void read_bignum( bignum_class * num )
{
    unsigned int n;

    while( scanf( "%d", &n ) != 1 );

    int_to_bignum( n, num );
} // read_bignum


// **** Arithmetic operations on bignums ****

void add_bignum( bignum_class * in1, bignum_class * in2,
  bignum_class * out )
{
    unsigned char * in1ptr, * in2ptr;
    register unsigned short i, carry = 0, sum;
    unsigned short in1len, in2len;
    static bignum_class temp_out;
    unsigned char * temp_out_ptr;
    bignum_class * temp_ptr;

    if( in1->len < in2->len ) {
	temp_ptr = in1;
	in1 = in2;
	in2 = temp_ptr;
    }

    in1ptr = in1->buf;
    in2ptr = in2->buf;
    in1len = in1->len;
    in2len = in2->len;

    copy_bignum( in1, &temp_out ); // in1 is the longer arg
    temp_out_ptr = temp_out.buf;

    for( i = 0; i < in1len; i++ ) {
	// assert( carry <= 1 );
	sum = *in1ptr + carry;
	in1ptr++;

	if( i < in2len ) {
	    sum += *in2ptr;
	}

	*temp_out_ptr = (unsigned char)(sum & 0xff);
	temp_out_ptr++;
	carry = sum >> 8;

	if( i >= in2len  &&  carry == 0 ) break;

	in2ptr++;
    }

    if( carry > 0 ) {
	resize_if(&temp_out,i+1);
	temp_out.buf[i] = (unsigned char)carry;
	temp_out.len = i + 1;  // ie. ++
    }

    copy_bignum( &temp_out, out );
} // add_bignum()


// Assume that numbers are trimmed

compare_type cmp_bignum( bignum_class * n1, bignum_class * n2 )
{
    int i, diff;

    if( n1->len < n2->len ) return( CT_LT );

    if( n1->len > n2->len ) return( CT_GT );

    for( i = n1->len - 1; i >= 0; i-- ) {
	diff = (int)((unsigned int)(n1->buf[i])
	  - (unsigned int)(n2->buf[i]));

	if( diff == 0 ) continue;

	return( (diff > 0) ? CT_GT : CT_LT );
    }

    return( CT_EQ );
} // cmp_bignum


void sub_bignum( bignum_class * in1, bignum_class * in2, bignum_class * out )
{
    static bignum_class temp;

    if( cmp_bignum( in1, in2 ) == CT_LT ) {
	printf( "Subtraction error: negative result\n" );
	exit( 1 );
    }

    copy_bignum( in2, &temp );
    temp.zero_pad( in1->len );
    temp.complement();                  // 1's complement
    add_bignum( in1, &temp, &temp );
    temp.inc();                         // 2's complement
    temp.len = in1->len;                // Ignore carry bit
    temp.trim();
    copy_bignum( &temp, out );
} // sub_bignum


void mult_bignum( bignum_class * in1, bignum_class * in2,
  bignum_class * out )
{
    unsigned char mask, * in1ptr, in1ch;
    int i;
    unsigned short j, in1len = in1->len,
      out_len = in1len + in2->len;
    static bignum_class temp_out, temp_in2;

    temp_out.len = 0;   // ie. temp_out := 0
    // So buf != NULL and is 0'd and right size:
    resize_if(&temp_out,out_len);
    memset( temp_out.buf, 0, out_len );

    resize_if(&temp_in2,out_len);
    memset( temp_in2.buf, 0, in1len );
    memcpy( temp_in2.buf + in1len, in2->buf, in2->len );
    temp_in2.len = out_len;
    temp_in2.ror();
    in1ptr = &in1->buf[in1len - 1];

    for( i = in1len - 1; i >= 0; i-- ) {
	in1ch = *in1ptr;
	in1ptr--;
	mask = 0x80;

	for( j = 0; j < 8; j++ ) {

	    if( in1ch & mask ) {
		add_bignum( &temp_out, &temp_in2, &temp_out );
	    }

	    temp_in2.ror();
	    mask >>= 1;
	}
    }

    copy_bignum( &temp_out, out );
} // mult_bignum()


// If quot == NULL, we just want the remainder (ie. modulus)

void div_bignum( bignum_class * in1, bignum_class * in2, bignum_class * quot,
  bignum_class * rem )
{
    unsigned char mask, * temp_quot_ptr;
    int i, j, len_diff = in1->len - in2->len;
    static bignum_class temp_quot, temp_rem, temp_in2;

    if( len_diff < 0 ) {

	if( quot != NULL ) {
	    int_to_bignum( 0, quot );
	}

	copy_bignum( in1, rem );
	return;
    }

    resize_if(&temp_quot,len_diff+1);
    memset( temp_quot.buf, 0, len_diff + 1 );
    temp_quot.len = len_diff + 1;

    copy_bignum( in1, &temp_rem );

    temp_in2.len = 0;
    resize_if(&temp_in2,in1->len+1);
    memset( temp_in2.buf, 0, len_diff + 1 );
    memcpy( temp_in2.buf + len_diff + 1,
      in2->buf, in2->len );
    temp_in2.len = in1->len + 1;
    temp_in2.ror();
    temp_quot_ptr = &temp_quot.buf[len_diff];

    for( i = len_diff; i >= 0; i-- ) {
	mask = 0x80;

	for( j = 0; j < 8; j++ ) {

	    if( cmp_bignum( &temp_rem, &temp_in2 ) != CT_LT ){
		sub_bignum( &temp_rem, &temp_in2, &temp_rem );
		*temp_quot_ptr |= mask;
	    }

	    temp_in2.ror();
	    mask >>= 1;
	}

	temp_quot_ptr--;
    }

    if( quot != NULL ) {
	copy_bignum( &temp_quot, quot );
	quot->trim();
    }

    copy_bignum( &temp_rem, rem );
} // div_bignum


bool inverse_under_mod( bignum_class * in, bignum_class * n,
  bignum_class * out )
{
    unsigned int i, lim;
    static bignum_class num1, rem, temp_out;

    // Can overcome the 4-byte barrier by making i a bignum

    if( in->len > 4 ) {
	printf( "inverse: element is too big\n" );
	return( FALSE );
    }

    lim = in->to_int();
    int_to_bignum( 1, &num1 );

    for( i = 1; i < lim; i++ ) {
	add_bignum( &num1, n, &num1 ); // ie. num1 = n*i + 1
	div_bignum( &num1, in, &temp_out, &rem );

	if( rem.len == 0 ) {
	    printf( "inverse: an inverse has been found\n" );
	    copy_bignum( &temp_out, out );
	    return( TRUE );
	}
    }

    printf( "inverse: no inverse found\n" );
    return( FALSE );
}


// Page 829;  d := (a^b) mod n

void modular_exp( bignum_class * a, bignum_class * b,
  bignum_class * n, bignum_class * d )
{
    unsigned char mask, * b_ptr;
    int i, j;
    static bignum_class temp_d;

    int_to_bignum( 1, &temp_d );
    b_ptr = &b->buf[b->len - 1];

    for( i = b->len - 1; i >= 0; i-- ) {
	mask = 0x80;

	for( j = 0; j < 8; j++ ) {
	    mult_bignum( &temp_d, &temp_d, &temp_d );
	    div_bignum( &temp_d, n, NULL, &temp_d );

	    if( *b_ptr & mask ) {
		mult_bignum( &temp_d, a, &temp_d );
		div_bignum( &temp_d, n, NULL, &temp_d );
	    }

	    mask >>= 1;
	}

	b_ptr--;
    }

    copy_bignum( &temp_d, d );
} // modular_exp()


// p. 840 of CS466 text

bool witness( bignum_class * a, bignum_class * n )
{
    unsigned char mask, * b_ptr;
    int i, j;
    static bignum_class b, d, x, one;

    int_to_bignum( 1, &one );
    copy_bignum( n, &b );
    b.dec();                    // b := n-1
    b.trim();
    int_to_bignum( 1, &d );
    b_ptr = &b.buf[b.len - 1];

    for( i = b.len - 1; i >= 0; i-- ) {
	mask = 0x80;

	for( j = 0; j < 8; j++ ) {
	    copy_bignum( &d, &x );
	    mult_bignum( &d, &d, &d );
	    div_bignum( &d, n, NULL, &d );

	    if( cmp_bignum( &d, &one ) == CT_EQ
	      &&  cmp_bignum( &x, &one ) != CT_EQ
	      &&  cmp_bignum( &x, &b ) != CT_EQ )
		return( TRUE );

	    if( *b_ptr & mask ) {
		mult_bignum( &d, a, &d );
		div_bignum( &d, n, NULL, &d );
	    }

	    mask >>= 1;
	}

	b_ptr--;
    }

    return( (cmp_bignum( &d, &one ) != CT_EQ)
      ? TRUE : FALSE );
} // witness()


// p. 841: Is n prime?

bool Miller_Rabin( bignum_class * n, unsigned int num_wit )
{
    unsigned int j;
    static bignum_class wit;

    srand( time( NULL ) );

    for( j = 0; j < num_wit; j++ ) {

	do {
	    wit.random( n->len );
	    div_bignum( &wit, n, NULL, &wit ); // so wit < n
	} while( wit.len == 0 );

	printf( "    Witness %d\n", j );

	if( witness( &wit, n ) ) return( FALSE );
    }

    // TRUE => Prob(n is composite) < 2^-num_wit
    return( TRUE );
} // Miller_Rabin()


void random_bigprime( bignum_class * num, unsigned int len )
{
    unsigned int i;

    for( i = 0; ; i++ ) {
	do {
	    num->random( len );
	} while( num->len < 2 && !(num->buf[0] & 0xfe) );

	num->buf[0] |= 0x1;             // Ensure odd
	printf( "Attempt %d\n", i );

	if( Miller_Rabin( num, NUM_WITNESSES ) ) break;
    }

    printf( "Probable prime: " );
    num->print();
} // random_bigprime


void create_keys( void )
{
    static char filename[80], cmd[10];
    unsigned int len;
    FILE * fp;
    bignum_class p, q, n, phi_n, d, e;

    do {
	printf( "Desired key size in bytes: " );
    } while( scanf( "%d", &len ) < 1  ||  len < 2 );

    random_bigprime( &p, len / 2 );
    random_bigprime( &q, len / 2 );
    mult_bignum( &p, &q, &n );
    p.dec();
    q.dec();
    mult_bignum( &p, &q, &phi_n );

    do {
	random_bigprime( &d, 1 ); // Get small odd prime
    } while( !inverse_under_mod( &d, &phi_n, &e ) );

    // Test keys
    // New meanings for p and q
    // Theorem: (q^(d*e)) mod n == q for all 0 <= q < n
    // ie. (q^(d*e-1)) mod n == 1 for all 0 < q < n
    mult_bignum( &d, &e, &p );
    p.dec();

    printf( "Testing integrity of keys...\n" );

    do {

	do {
	    q.random( n.len );
	    div_bignum( &q, &n, NULL, &q ); // So q < n
	} while( q.len == 0 ); // Don't want q == 0

	modular_exp( &q, &p, &n, &q );
	printf( "Test output (0x01): " );
	q.print();

	if( q.len != 1  ||  q.buf[0] != 0x1 ) {
	    // q != 1 => error 
	    printf( "Bad keys\n" );
	    return;
	}

	printf( "Test key again? (y/n): " );
	scanf( "%s", cmd );
    } while( cmd[0] != 'n' );

    // Save keys

    for( ; ; ) {
	printf( "Public key filename: " );
	scanf( "%s", filename );

	if( !strcmp( filename, "quit" ) ) return;

	if( (fp = fopen( filename, "w" )) != NULL ) break;

	printf( "Can't open '%s'\n", filename );
    }

    d.filewrite( fp );
    n.filewrite( fp );
    fclose( fp );

    for( ; ; ) {
	printf( "Private key filename: " );
	scanf( "%s", filename );

	if( !strcmp( filename, "quit" ) ) return;

	if( (fp = fopen( filename, "w" )) != NULL ) break;

	printf( "Can't open '%s'\n", filename );
    }

    e.filewrite( fp );
    n.filewrite( fp );
    fclose( fp );
    printf( "Keys have been created.\n\n" );
} // create_keys()


void apply_key( unsigned int op )
{
//    bool pause;
    static char filename[80];
    int bytes_read, in_size, out_size;
    FILE * in, * out;
    bignum_class de, n,    // de is d or e, depending on op
      in_num, out_num;

    // Read the key from its file

    do {
	printf( "%scryption key filename: ",
	  (op == ENCRYPT) ? "En" : "De" );
    } while( scanf( "%s", filename ) != 1 );

    if( (in = fopen( filename, "r" )) == NULL ) {
	printf( "Can't open file\n" );
	return;
    }

    de.fileread( in );
    n.fileread( in );
//    printf( "n has %d bytes\n", n.len );
    fclose( in );
    in_size = out_size = n.len;

    if( op == ENCRYPT ) {
	in_size--; // so in_num <= n, out_buf can hold n
    } else {
	out_size--; // so in_buf can hold n, out_num <= n
    }

    in_num.resize( in_size );
    out_num.resize( out_size );

    // Open input, output filess

    do {
	printf( "Input filename: " );
    } while( scanf( "%s", filename ) != 1 );

    if( (in = fopen( filename, "rb" )) == NULL ) {
	printf( "Can't open input file\n" );
	return;
    }

    do {
	printf( "Output filename: " );
    } while( scanf( "%s", filename ) != 1 ); 

    if( (out = fopen( filename, "wb" )) == NULL ) {
	printf( "Can't open output file\n" );
	fclose( in );
	return;
    }

#if 0
    printf( "Pause?  " );
    scanf( "%s", filename );
    pause = (filename[0] == 'y') ? TRUE : FALSE;
#endif

    for( ; ; ) {
	// Read max_in_size objects, each 1 byte long
	bytes_read = fread( in_num.buf, 1, in_size, in );
	printf( "%d bytes read\n", bytes_read );

	if( bytes_read == 0 ) break;

	in_num.len = bytes_read;
	printf( "bignum in: " );
	in_num.print();
	in_num.trim();

	// Apply operation
	modular_exp( &in_num, &de, &n, &out_num );

	printf( "bignum out: " );
	out_num.print();
	out_num.zero_pad( out_size );
	fwrite( out_num.buf, out_size, 1, out );

	if( bytes_read < in_size ) break;
#if 0
	if( pause ) {
	    fflush(stdin);
	    printf( "Hit ENTER: " );
	    getchar();
	}
#endif
    } // for

    printf( "Input file EOF %sreached\n",
      feof( in ) ? "" : "not " );
    fclose( in );
    fclose( out );
    printf( "Key applied successfully\n" );
} // apply_key()


void main( void )
//int PASCAL WINMAIN( HANDLE hInstance, HANDLE hPrevInstance,
//  LPSTR lpCmdLine, int nCmdShow )
{
    int s;

    srand( time( NULL ) );
    printf( "RSA Encryption\n" );

    for( ; ; ) {
	printf( "\nMenu:\n" );
	printf( "0: Quit\n" );
	printf( "1: Generate a pair of keys\n" );
	printf( "2: Encrypt a file\n" );
	printf( "3: Decrypt a file\n" );
	printf( "4: uuencode a file\n" );
	printf( "5: uudecode a file\n" );

	do {
	    printf( "Selection: " );
	} while( scanf( "%d", &s ) != 1 );

	switch( s ) {
	    case 0:                             // quit
		printf( "\nExiting...\n" );
		exit( 0 );

	    case 1:                             // Create keys
		create_keys();
		break;

	    case 2:                                     // Encrypt
		apply_key( ENCRYPT );
		break;

	    case 3:                                     // Decrypt
		apply_key( DECRYPT );
		break;

	    case 4:
		textencode();
		break;

	    case 5:
		textdecode();
		break;

	    default:
		printf( "Unrecognized command\n\n" );
		break;
	} // switch

	printf( "Press RETURN: " );
	fflush( stdin );
	getchar();
    } // for
} // main()

