/* -*- mode: c; c-basic-offset: 2; -*- */

#include <stdio.h>
#include <assert.h>

#include "arbint.h"
#include "arbint-priv.h"

static ArbInt *ai_pow_positive(ArbInt const *A, ArbInt const *B);

static int ai_is_even(ArbInt const *A);

static ArbInt *ai_pow_by_power2(ArbInt const *A, aibase_t exponent);

ArbInt *AI_Pow(ArbInt const *A, ArbInt const *B)
{
  ArbInt *zero = AI_NewArbInt_From32(0);
  if (AI_Equal(B, zero)) {
    AI_FreeArbInt(zero);
    return AI_NewArbInt_From32(1);
  }

  if (AI_Less(B, zero)) {
    /*
     * We're integer-only, so we have to return 0 here
     */
    return zero;
  }

  return ai_pow_positive(A, B);
}

int ai_is_even(ArbInt const *A)
{
  if (A == NULL) return 1;
  if (A->dataLen == 0) return 1;
  return (A->data[A->dataLen - 1] & 0x1) ? 0 : 1;
}

ArbInt *ai_pow_by_power2(ArbInt const *A, aibase_t exponent)
{
  if (exponent == 0) return AI_NewArbInt_From32(0);
  if (exponent == 1) return AI_NewArbInt_FromCopy(A);
  
}

ArbInt *ai_pow_positive(ArbInt const *A, ArbInt const *B)
{
  return NULL;
}
