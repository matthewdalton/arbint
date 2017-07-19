/* -*- mode: c; c-basic-offset: 2; -*- */

#include <stdio.h>
#include <assert.h>

#include "arbint.h"
#include "arbint-priv.h"

#include "AI_utils.h"

static ArbInt *ai_pow_positive(ArbInt const *A, ArbInt const *B);

static int ai_is_even(ArbInt const *A);

static ArbInt *ai_pow_by_power2(ArbInt const *A, aibase_t exponent);

ArbInt *AI_Pow(ArbInt const *A, ArbInt const *B)
{
  ArbInt *zero = AI_NewArbInt_From32(0);
  ArbInt *one = AI_NewArbInt_From32(1);
  if (AI_Equal(B, zero)) {
    AI_FreeArbInt(zero);
    return one;
  }
  if (AI_Equal(B, one)) {
    AI_FreeArbInt(zero);
    AI_FreeArbInt(one);
    return AI_NewArbInt_FromCopy(A);
  }

  if (AI_Less(B, zero)) {
    AI_FreeArbInt(zero);
    AI_FreeArbInt(one);
    /*
     * We're integer-only, so we have to return 0 here
     */
    return zero;
  }

  AI_FreeArbInt(zero);
  AI_FreeArbInt(one);
  return ai_pow_positive(A, B);
}

int ai_is_even(ArbInt const *A)
{
  if (A == NULL) return 1;
  if (A->dataLen == 0) return 1;
  return (A->data[A->dataLen - 1] & 0x1) ? 0 : 1;
}

/*
 * Multiplies A by 2^exponent
 * exponent must be 0, 1 or a power of 2
 * eg. 2, 4, 8, 16, 32, etc.
 */
ArbInt *ai_pow_by_power2(ArbInt const *A, aibase_t exponent)
{
  ArbInt *ans = AI_NewArbInt_FromCopy(A);
  ArbInt *val;
  while (exponent > 0) {
    val = AI_Mul(ans, ans);
    ans = val;
    exponent--;
  }
  return ans;
}

ArbInt *ai_pow_by_val(ArbInt const *A, aibase_t val)
{
  /* Simple implementation for now */
  if (val == 0) {
    return AI_NewArbInt_From32(1);
  } else if (val == 1) {
    return AI_NewArbInt_FromCopy(A);
  }

  ArbInt *result = AI_Mul(A, A);
  val -= 2;
  while (val > 0) {
    ArbInt *tmp = AI_Mul(result, A);
    AI_FreeArbInt(result);
    result = tmp;
    val--;
  }
  return result;
}

ArbInt *__ai_pow_by_power2(ArbInt const *A, aibase_t exponent)
{
  return ai_pow_by_power2(A, exponent);
}

ArbInt *ai_pow_positive(ArbInt const *A, ArbInt const *B)
{
  ArbInt *ans = AI_NewArbInt_From32(1);
  ArbInt *val;
  aibase_t hsb = arbint_get_hsb_position(B);
  ArbInt_BitIterator *bi = AI_Make_Bit_Iter(B);
  aibase_t bitNum = 0;
  aibase_t exp = 1;
  while (bitNum < hsb) {
    int isSet = AI_Bit_Iter_Get(bi);
    if (isSet) {
      val = AI_Mul(ans, ai_pow_by_power2(A, bitNum));
      AI_FreeArbInt(ans);
      ans = val;
    }
    bitNum++;
    exp++;
    if (!AI_Bit_Iter_Inc(bi)) {
      break;
    }
  }
  return ans;
}
