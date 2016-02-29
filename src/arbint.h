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
 * Free an ArbInt created using any of the above initialisers
 */
void AI_FreeArbInt(ArbInt *aival);



/*********************************************************************
 Output
 *********************************************************************/

/*
 * Convert to string
 */
char const *AI_ToString(ArbInt const *value);


/*********************************************************************
 Mathematics
 *********************************************************************/

/*
 * Addition
 */
ArbInt *AI_Add(ArbInt const *A, ArbInt const *B);
ArbInt *AI_Add_Value(ArbInt const *A, unsigned long val, int sign);

/*
 * Subtraction (A - B)
 */
ArbInt *AI_Sub(ArbInt const *A, ArbInt const *B);

/*
 * Multiplication
 */
ArbInt *AI_Mul(ArbInt const *A, ArbInt const *B);
ArbInt *AI_Mul_Value(ArbInt const *A, unsigned long val, int sign);

/*
 * Division (A / B)
 */
ArbInt *AI_Div(ArbInt const *A, ArbInt const *B, ArbInt **remainder);

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

#endif /* _ARBINT_H_ */
