/*
 * Arbitrary precision integers - public interface
 */
#ifndef _ARBINT_H_
#define _ARBINT_H_

#define STATIC

#include <stdlib.h>
#include <inttypes.h>

typedef struct arbint	ArbInt;
typedef uint32_t        aibase_t;

typedef struct arbint_bititerator ArbInt_BitIterator;


/*********************************************************************
 Initialisers
 *********************************************************************/

/*
 * Initialise to 0
 */
ArbInt *AI_NewArbInt(void);

/*
 * Initialise to value held in string
 */
ArbInt *AI_NewArbInt_FromString(char const *value);

/*
 * Initialise to value held in a long
 */
ArbInt *AI_NewArbInt_FromLong(int32_t value); /* deprecated */
ArbInt *AI_NewArbInt_From32(int32_t value);

/*
 * Initialise to value held in a size_t, with a separate sign indicator
 */
ArbInt *AI_NewArbInt_FromValue(aibase_t value, int sign);

/*
 * Copy constructor
 */
ArbInt *AI_NewArbInt_FromCopy(ArbInt const *ai);

/*
 * Initialise with a bit in the specified position.
 * Produces 2^bit.
 */
ArbInt *AI_NewArbInt_SetBit(aibase_t bit);

/*
 * Free an ArbInt created using any of the above initialisers
 */
void AI_FreeArbInt(ArbInt *aival);



/*********************************************************************
 Output
 *********************************************************************/

/*
 * Convert to string. Returns a hexadecimal representation.
 */
char const *AI_ToString(ArbInt const *value);

/*
 * Convert to string, decimal representation
 */
char const *AI_ToStringDec(ArbInt const *value);
char const *AI_ToStringHex(ArbInt const *value);

/*
 * Convert to string, arbitrary base representation. Base must be <= 10.
 */
char const *AI_ToStringBase(ArbInt const *value, int base, int approxLen);



/*********************************************************************
 Mathematics
 *********************************************************************/

/*
 * Addition
 */
ArbInt *AI_Add(ArbInt const *A, ArbInt const *B);
ArbInt *AI_Add_Value(ArbInt const *A, unsigned long val, int sign);
ArbInt *AI_Add_And_Free(ArbInt *A, ArbInt *B);

/*
 * Subtraction (A - B)
 */
ArbInt *AI_Sub(ArbInt const *A, ArbInt const *B);
ArbInt *AI_Sub_And_Free(ArbInt *A, ArbInt *B);

/*
 * Multiplication
 */
ArbInt *AI_Mul(ArbInt const *A, ArbInt const *B);
ArbInt *AI_Mul_Value(ArbInt const *A, unsigned long val, int sign);
ArbInt *AI_Mul_And_Free(ArbInt *A, ArbInt *B);

/*
 * Division (A / B)
 */
ArbInt *AI_Div(ArbInt const *A, ArbInt const *B, ArbInt **remainder);
ArbInt *AI_Div_And_Free(ArbInt *A, ArbInt *B, ArbInt **remainder);

/*
 * Greater than (A > B)
 */
int AI_Greater(ArbInt const *A, ArbInt const *B);

/*
 * A == B
 */
int AI_Equal(ArbInt const *A, ArbInt const *B);

/*
 * A < B
 */
int AI_Less(ArbInt const *A, ArbInt const *B);

/*
 * A >= B
 */
int AI_GreaterOrEqual(ArbInt const *A, ArbInt const *B);

/*
 * A <= B
 */
int AI_LessOrEqual(ArbInt const *A, ArbInt const *B);

/*
 * Abs
 */
ArbInt *AI_Abs(ArbInt const *A);

/*
 * Negate
 */
ArbInt *AI_Neg(ArbInt const *A);

/*
 * Is it zero?
 */
int AI_IsZero(ArbInt const *A);

/*
 * A^B
 */
ArbInt *AI_Pow(ArbInt const *A, ArbInt const *B);

/*
 * Square root
 */
ArbInt *AI_isqrt(ArbInt const *n);

/*********************************************************************
 Iterators
 *********************************************************************/

/*
 * Bit iterator
 */
ArbInt_BitIterator *AI_Make_Bit_Iter(ArbInt const *ai);

int AI_Bit_Iter_Inc(ArbInt_BitIterator *bi);

int AI_Bit_Iter_Get(ArbInt_BitIterator const *bi);

#endif /* _ARBINT_H_ */
