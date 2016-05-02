// rsa.h - Header file for implementation of RSA-scheme
// encryption
// By Tom Weatherhead
// Started March 7, 1994

// Definitions

#define RESIZE_INC			10
#define NUM_WITNESSES			40
#define NUM_COLS			60

#define resize_if(num,newlen) \
    if((num)->buf_size<newlen){(num)->resize(newlen);}


enum { ENCRYPT, DECRYPT };


// Type and class definitions

#ifdef FALSE
typedef char bool;
#else
typedef enum { FALSE, TRUE } bool;
#endif


typedef enum {
    CT_LT,
    CT_EQ,
    CT_GT
} compare_type;


class bignum_class {
  public:
    unsigned short len, buf_size;
    unsigned char * buf;

    bignum_class( void );
    void resize( unsigned short );
    unsigned int to_int( void );
    void print( void );
    void trim( void );
    void zero_pad( unsigned short );
    void complement( void );
    void inc( void );
    void dec( void );
    void ror( void );
    void random( unsigned short );
    void fileread( FILE * );
    void filewrite( FILE * );
#if 0
    operator =();
    operator +();
    operator ++();
    operator -();
    operator --();
    operator %();
    operator *();
#endif
    ~bignum_class( void );
}; // bignum_class


void textencode( void );
void textdecode( void );

// **** End of rsa.h ****