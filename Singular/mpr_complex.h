#ifndef MPR_COMPLEX_H
#define MPR_COMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_complex.h,v 1.4 1999-06-24 07:46:51 wenk Exp $ */

/* 
* ABSTRACT - multipolynomial resultants - real floating-point numbers using gmp
*            and complex numbers based on pairs of real floating-point numbers
*   
*/

//-> include & define stuff
// must have gmp version >= 2
extern "C" { 
#include <gmp.h>
}
#include "numbers.h"
#include "ring.h"
#include "mpr_global.h"

#define DEFPREC        20         // minimum number of digits (output operations)

#define GMP_DEFAULT_PREC_BITS 512 // size of mantissa of floating-point number
#define GMP_NEEDEQUAL_BITS    512-64 // a == b for the first gmp_equalupto_bits bits
//<-

void setGMPFloatPrecBytes( unsigned long int bytes );
unsigned long int getGMPFloatPrecBytes();

void setGMPFloatDigits( size_t digits );
size_t getGMPFloatDigits();

//-> class gmp_float
/**
 * @short wrapper class for GNU Multi Precision Floats
 */
class gmp_float
{
public:
  gmp_float( const int v = 0 )
  {
    mpf_init2( t, gmp_default_prec_bits );
    mpf_set_si( t, (signed long int) v );
  }
  gmp_float( const long v )
  {
    mpf_init2( t, gmp_default_prec_bits );
    mpf_set_si( t, (signed long int) v );
  }
  gmp_float( const mprfloat v ) // double
  {
    mpf_init2( t, gmp_default_prec_bits );
    mpf_set_d( t, (double) v );
  }
  gmp_float( const mpf_t v )
  {
    mpf_init2( t, gmp_default_prec_bits );
    mpf_set( t, v );
  }
  gmp_float( const mpz_t v ) // gnu mp Z
  {
    mpf_init2( t, gmp_default_prec_bits );
    mpf_set_z( t, v );
  }
  gmp_float( const gmp_float & v ) // copy constructor
  {
    //mpf_init2( t, mpf_get_prec( v.t ) );
    mpf_init2( t, gmp_default_prec_bits );
    mpf_set( t, v.t );
  }

  ~gmp_float()
  {
    mpf_clear( t );
  }

  friend gmp_float operator + ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator - ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator * ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator / ( const gmp_float & a, const gmp_float & b );

  inline gmp_float & operator += ( const gmp_float & a );
  inline gmp_float & operator -= ( const gmp_float & a );
  inline gmp_float & operator *= ( const gmp_float & a );
  inline gmp_float & operator /= ( const gmp_float & a );

  friend bool operator == ( const gmp_float & a, const gmp_float & b );
  friend bool operator  > ( const gmp_float & a, const gmp_float & b );
  friend bool operator  < ( const gmp_float & a, const gmp_float & b );
  friend bool operator >= ( const gmp_float & a, const gmp_float & b );
  friend bool operator <= ( const gmp_float & a, const gmp_float & b );

  friend gmp_float operator - ( const gmp_float & a );

  gmp_float & operator = ( const gmp_float & a );
  gmp_float & operator = ( const mpz_t & a );
  gmp_float & operator = ( const mprfloat a );
  gmp_float & operator = ( const long a );

  inline int sign();    // t>0:+1, t==0:0, t<0:-1
  inline bool isZero();  // t == 0 ?
  inline bool isOne();   // t == 1 ?
  inline bool isMOne();  // t == -1 ?

  inline bool setFromStr( char * in );
  
  // access 
  inline const mpf_t *mpfp() const;

  inline operator double();
  inline operator double() const;

  inline operator int();
  inline operator int() const;

public:
  static void setPrecision( const unsigned long int prec ) 
    { gmp_default_prec_bits= prec; }
  static void setEqualBits( const unsigned long int prec )
    { gmp_needequal_bits= prec; }

  static const unsigned long int getPrecision() 
    { return gmp_default_prec_bits; }
  static const unsigned long int getEqualBits() 
    { return gmp_needequal_bits; }

private:
  static unsigned long int gmp_default_prec_bits;
  static unsigned long int gmp_needequal_bits;

  mpf_t t;
};

static const gmp_float  gmpOne= 1;
static const gmp_float gmpMOne= -1;
static const gmp_float gmpZero= 0;

// <gmp_float> operator <gmp_float>
inline gmp_float & gmp_float::operator += ( const gmp_float & a )
{
  mpf_add( t, t, a.t );
  return *this;
}
inline gmp_float & gmp_float::operator -= ( const gmp_float & a )
{
  mpf_sub( t, t, a.t );
  return *this;
}
inline gmp_float & gmp_float::operator *= ( const gmp_float & a )
{
  mpf_mul( t, t, a.t );
  return *this;
}
inline gmp_float & gmp_float::operator /= ( const gmp_float & a )
{
  mpf_div( t, t, a.t );
  return *this;
}

// <gmp_float> = <*>
inline gmp_float & gmp_float::operator = ( const gmp_float & a )
{
  mpf_set( t, a.t );
  return *this;
}
inline gmp_float & gmp_float::operator = ( const mpz_t & a )
{
  mpf_set_z( t, a );
  return *this;
}
inline gmp_float & gmp_float::operator = ( const mprfloat a )
{
  mpf_set_d( t, (double) a );
  return *this;
}
inline gmp_float & gmp_float::operator = ( const long a )
{
  mpf_set_si( t, (signed long int) a );
  return *this;
}

// cast to double
inline gmp_float::operator double()
{
  return mpf_get_d( t );
}
inline gmp_float::operator double() const
{
  return mpf_get_d( t );
}

// cast to int
inline gmp_float::operator int()
{
  return (int)mpf_get_d( t );
}
inline gmp_float::operator int() const
{
  return (int)mpf_get_d( t );
}

// get sign of real number ( -1: t < 0; 0: t==0; 1: t > 0 )
inline int gmp_float::sign()
{
  return mpf_sgn( t );
}
// t == 0 ?
inline bool gmp_float::isZero()
{
#ifdef  VARIANTE_1
  return (mpf_sgn( t ) == 0);
#else
  return  mpf_eq( t , gmpZero.t , gmp_float::gmp_needequal_bits );
#endif
}
// t == 1 ?
inline bool gmp_float::isOne()
{
#ifdef  VARIANTE_1
  return (mpf_cmp_ui( t , 1 ) == 0);
#else
  return mpf_eq( t , gmpOne.t , gmp_float::gmp_needequal_bits );
#endif
}
// t == -1 ?
inline bool gmp_float::isMOne()
{
#ifdef VARIANTE_1
  return (mpf_cmp_si( t , -1 ) == 0);
#else
  return mpf_eq( t , gmpMOne.t , gmp_float::gmp_needequal_bits );
#endif
}

inline bool gmp_float::setFromStr( char * in )
{
  return ( mpf_set_str( t, in, 10 ) == 0 );
}

// access pointer
inline const mpf_t *gmp_float::mpfp() const
{
  return &t;
}

// built-in functions of GMP
gmp_float abs( const gmp_float & );
gmp_float sqrt( const gmp_float & );
gmp_float hypot( const gmp_float &, const gmp_float & );

// simulated functions using double functions
gmp_float sin( const gmp_float & );
gmp_float cos( const gmp_float & );
gmp_float log( const gmp_float & );
gmp_float exp( const gmp_float & );

gmp_float max( const gmp_float &, const gmp_float & );

gmp_float numberToFloat( number num );
char *floatToStr( const gmp_float & r, const unsigned int oprec );
//<-

//-> class gmp_complex
/**
 * @short gmp_complex numbers based on 
 */
class gmp_complex
{
private:
  gmp_float r, i;

public:
  gmp_complex( const gmp_float re= 0.0, const gmp_float im= 0.0 )
  {
    r= re;
    i= im;
  }
  gmp_complex( const mprfloat re, const mprfloat im = 0.0 )
  {
    r= re;
    i= im;
  }
  gmp_complex( const long re, const long im )
  {
    r= re;
    i= im;
  }
  gmp_complex( const gmp_complex & v )
  {
    r= v.r;
    i= v.i;
  }
  ~gmp_complex() {}

  friend gmp_complex operator + ( const gmp_complex & a, const gmp_complex & b );
  friend gmp_complex operator - ( const gmp_complex & a, const gmp_complex & b );
  friend gmp_complex operator * ( const gmp_complex & a, const gmp_complex & b );
  friend gmp_complex operator / ( const gmp_complex & a, const gmp_complex & b );

  // gmp_complex <operator> real
  inline friend gmp_complex operator + ( const gmp_complex & a, const gmp_float b_d );  
  inline friend gmp_complex operator - ( const gmp_complex & a, const gmp_float b_d );
  inline friend gmp_complex operator * ( const gmp_complex & a, const gmp_float b_d );
  inline friend gmp_complex operator / ( const gmp_complex & a, const gmp_float b_d );

  gmp_complex & operator += ( const gmp_complex & a );
  gmp_complex & operator -= ( const gmp_complex & a );
  gmp_complex & operator *= ( const gmp_complex & a );
  gmp_complex & operator /= ( const gmp_complex & a );

  inline friend bool operator == ( const gmp_complex & a, const gmp_complex & b );

  inline gmp_complex & operator = ( const gmp_complex & a );

  // access to real and imaginary part
  inline gmp_float real() const { return r; }
  inline gmp_float imag() const { return i; }

  inline void real( gmp_float val ) { r = val; }
  inline void imag( gmp_float val ) { i = val; }
};

// <gmp_complex> = <gmp_complex> operator <gmp_float>
//
inline gmp_complex operator + ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r + b_d, a.i );
}
inline gmp_complex operator - ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r - b_d, a.i );
}
inline gmp_complex operator * ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r * b_d, a.i * b_d );
}
inline gmp_complex operator / ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r / b_d, a.i / b_d );
}

// <gmp_complex> == <gmp_complex> ?
inline bool operator == ( const gmp_complex & a, const gmp_complex & b )
{
  return ( b.real() == a.real() ) && ( b.imag() == a.imag() );
}

// <gmp_complex> = <gmp_complex>
inline gmp_complex & gmp_complex::operator = ( const gmp_complex & a )
{
  r= a.r;
  i= a.i;
  return *this;
}

// Returns absolute value of a gmp_complex number
//
inline gmp_float abs( const gmp_complex & c )
{
  return hypot(c.real(),c.imag());
}

gmp_complex sqrt( const gmp_complex & x );

inline gmp_complex numberToGmp_Complex( number num )
{
  if (rField_is_long_C()) {
    return *(gmp_complex*)num;
  } else {
    return gmp_complex( numberToFloat(num) );
  }
}

char *complexToStr( const gmp_complex & c, const  unsigned int oprec );
//<-

#endif MPR_COMPLEX_H

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: *** 





