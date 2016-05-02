// RSA testing excerpts


// Menu options


#ifdef RSA_TEST
	printf( "1: Use Miller_Rabin() to test a given number\n" );
	printf( "5: Add\n" );
	printf( "6: Subtract\n" );
	printf( "7: Generate a random bignum\n" );
	printf( "8: Multiply\n" );
	printf( "9: Divide\n" );
	printf( "11: Inverse in a group\n" );
	printf( "12: Find a random big prime\n" );
	printf( "13: Modular exponentiation\n" );
#endif


#ifdef RSA_TEST
	    case 1:				// primality test
		printf( "Bignum: " );
		read_bignum( &n );
		printf( "Number of witnesses: " );
		scanf( "%d", &s );

		printf( "Miller_Rabin: %s\n",
		  Miller_Rabin( &n, s )
		  ? "TRUE" : "FALSE" );
		break;

	    case 5:					// +
            	printf( "p: " );
		read_bignum( &p );
		printf( "q: " );
		read_bignum( &q );
		add_bignum( &p, &q, &n );
                printf( "n: %d\n\n", n.to_int() );
		break;

	    case 6:					// -
            	printf( "p: " );
		read_bignum( &p );
		printf( "q: " );
		read_bignum( &q );
		sub_bignum( &p, &q, &n );
                printf( "n: %d\n\n", n.to_int() );
		break;

	    case 7:				// random
		printf( "Length: " );
                scanf( "%d", &len );
		n.random( len );
		printf( "n: ");
		n.print();
		break;

	    case 8:					// *
            	printf( "p: " );
		read_bignum( &p );
		printf( "q: " );
		read_bignum( &q );
		mult_bignum( &p, &q, &n );
		printf( "product: %d\n\n", n.to_int() );
                n.print();
		break;

	    case 9:					// /
            	printf( "p: " );
		read_bignum( &p );
		printf( "q: " );
		read_bignum( &q );
		div_bignum( &p, &q, &d, &e );
		printf( "quot: %d, rem: %d\n\n", d.to_int(),
		  e.to_int() );
		break;

	    case 11:					// inverse
            	printf( "p: " );
		read_bignum( &p );
		printf( "group size: " );
		read_bignum( &q );

		if( inverse_under_mod( &p, &q, &n ) ) {
		    printf( "Inverse is %d\n\n", n.to_int() );
		}

		break;

	    case 12:				// random_bigprime
		random_bigprime( &n );
		break;

	    case 13:			// mod_exp
		printf( "num: " );
		read_bignum( &p );
		printf( "exp: " );
		read_bignum( &q );
		printf( "mod: " );
		read_bignum( &d );
		modular_exp( &p, &q, &d, &e );
		printf( "Result: %d\n", e.to_int() );
		break;
#endif
