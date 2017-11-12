/* -*- mode: c; c-basic-offset: 2; -*- */

#include <stdio.h>
#include <assert.h>

#include "arbint.h"
#include "arbint-priv.h"

#include "AI_utils.h"

ArbInt *next_iteration(const ArbInt *curr, const ArbInt *n)
{
  /* next = (curr + n/curr) / 2 */

  ArbInt *rem;
  ArbInt *divResult = AI_Div(n, curr, &rem);
  AI_FreeArbInt(rem);
  ArbInt *addResult = AI_Add(curr, divResult);
  AI_FreeArbInt(divResult);
  ArbInt *two = AI_NewArbInt_FromLong(2);
  ArbInt *next = AI_Div(addResult, two, &rem);
  AI_FreeArbInt(addResult);
  AI_FreeArbInt(two);
  AI_FreeArbInt(rem);
  return next;
}

ArbInt *AI_isqrt(ArbInt const *n)
{
  ArbInt *zero = AI_NewArbInt_From32(0);
  if (AI_Less(n, zero)) {
    AI_FreeArbInt(zero);
    return NULL;
  }
  AI_FreeArbInt(zero);
  ArbInt *k = AI_NewArbInt_FromCopy(n);
  ArbInt *next_k;
  ArbInt *k_plus_one = NULL;

  // terminate if next_k == k or next_k == k + 1
  for (;;) {
    next_k = next_iteration(k, n);
    /* printf("next_k = %s\n", AI_ToStringDec(next_k)); */
    k_plus_one = AI_Add_Value(k, 1, 1);
    if (AI_Equal(next_k, k) || AI_Equal(next_k, k_plus_one)) {
      break;
    }
    AI_FreeArbInt(k);
    AI_FreeArbInt(k_plus_one);
    k = next_k;
  }
  AI_FreeArbInt(k);
  AI_FreeArbInt(k_plus_one);
  return next_k;
}
