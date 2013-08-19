/*
 * Arbitrary precision integers - public interface
 */
#ifndef _ARBINT_H_
#define _ARBINT_H_

#include <stdlib.h>

typedef struct arbint	ArbInt;


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
ArbInt *AI_NewArbInt_FromLong(long value);

/*
 * Initialise to value held in a size_t, with a separate sign indicator
 */
ArbInt *AI_NewArbInt_FromSizeT(size_t value, int sign);


/*********************************************************************
 Output
 *********************************************************************/

/*
 * Convert to string
 */
char *AI_ToString(ArbInt const *value);


/*********************************************************************
 Mathematics
 *********************************************************************/

/*
 * Addition
 */
ArbInt *AI_Add(ArbInt const *A, ArbInt const *B);

/*
 * Subtraction (A - B)
 */
ArbInt *AI_Sub(ArbInt const *A, ArbInt const *B);

/*
 * Multiplication
 */
ArbInt *AI_Mul(ArbInt const *A, ArbInt const *B);

/*
 * Division (A / B)
 */
ArbInt *AI_Div(ArbInt const *A, ArbInt const *B);

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

#endif /* _ARBINT_H_ */
