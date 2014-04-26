#include <stdio.h>
#include <stdlib.h>

#define STATIC

#include "arbint.h"
#include "arbint-priv.h"

/*
 * Functions return non-zero on success
 */

int test_ai_add_unsigned_with_lshift(void)
{
  ArbInt *A;
  ArbInt *B;
  ArbInt *C;
  size_t B_shift;
  ArbInt *expected;

  A = AI_NewArbInt_FromLong(0x0000DEAD);
  B = AI_NewArbInt_FromLong(0xBEEF0000);
  B_shift = 1;

  C = ai_add_unsigned_with_lshift(A, B, B_shift);
  expected = AI_NewArbInt_FromString("0xBEEF00000000DEAD");
  printf("Result:   %s\n", AI_ToString(C));
  printf("Expected: %s\n", AI_ToString(expected));
  if (!AI_Equal(C, expected))
    return 0;

  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  AI_FreeArbInt(C);
  AI_FreeArbInt(expected);


  A = AI_NewArbInt_FromString("0x12123434565600000000");
  B = AI_NewArbInt_FromString("0x999988886666");
  B_shift = 2;

  C = ai_add_unsigned_with_lshift(A, B, B_shift);
  expected = AI_NewArbInt_FromString("0x9999888878783434565600000000");
  printf("Result:   %s\n", AI_ToString(C));
  printf("Expected: %s\n", AI_ToString(expected));
  if (!AI_Equal(C, expected))
    return 0;

  return 1;
}
