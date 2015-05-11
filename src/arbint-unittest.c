#include <stdio.h>
#include <stdlib.h>

#define STATIC

#include "arbint.h"
#include "arbint-priv.h"

#include "arbint-unittest.h"

/*
 * Functions return non-zero on success
 */

int test_ai_add_unsigned_with_lshift(void)
{
  TEST_HEADER();
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
  /* printf("Result:   %s\n", AI_ToString(C)); */
  /* printf("Expected: %s\n", AI_ToString(expected)); */
  /* if (!AI_Equal(C, expected)) */
  /*   return 0; */
  TEST_EQUAL(C, expected, AI_Equal, AI_ToString);

  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  AI_FreeArbInt(C);
  AI_FreeArbInt(expected);


  A = AI_NewArbInt_FromString("0x12123434565600000000");
  B = AI_NewArbInt_FromString("0x999988886666");
  B_shift = 2;

  C = ai_add_unsigned_with_lshift(A, B, B_shift);
  expected = AI_NewArbInt_FromString("0x9999888878783434565600000000");
  /* printf("Result:   %s\n", AI_ToString(C)); */
  /* printf("Expected: %s\n", AI_ToString(expected)); */
  /* if (!AI_Equal(C, expected)) */
  /*   return 0; */

  /* return 1; */
  TEST_EQUAL(C, expected, AI_Equal, AI_ToString);

  TEST_FOOTER();
}

int test_ai_div_unsigned_by_subtraction__even_divide(void)
{
  TEST_HEADER();
  ArbInt *A = AI_NewArbInt_FromLong(70);
  ArbInt *B = AI_NewArbInt_FromLong(5);
  ArbInt *remainder;
  ArbInt *expected_div;
  ArbInt *expected_mod;

  ArbInt *C = ai_div_unsigned_by_subtraction(A, B, &remainder);
  expected_div = AI_NewArbInt_FromLong(14);
  expected_mod = AI_NewArbInt_FromLong(0);
  TEST_EQUAL(C, expected_div, AI_Equal, AI_ToString);
  TEST_EQUAL(remainder, expected_mod, AI_Equal, AI_ToString);
  
  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  AI_FreeArbInt(C);
  AI_FreeArbInt(remainder);
  AI_FreeArbInt(expected_div);
  AI_FreeArbInt(expected_mod);

  TEST_FOOTER();
}

int test_ai_div_unsigned_by_subtraction__zero_result(void)
{
  TEST_HEADER();
  ArbInt *A = AI_NewArbInt_FromLong(5);
  ArbInt *B = AI_NewArbInt_FromLong(12);
  ArbInt *remainder;
  ArbInt *expected_div;
  ArbInt *expected_mod;

  ArbInt *C = ai_div_unsigned_by_subtraction(A, B, &remainder);
  expected_div = AI_NewArbInt_FromLong(0);
  expected_mod = AI_NewArbInt_FromLong(5);
  TEST_EQUAL(C, expected_div, AI_Equal, AI_ToString);
  TEST_EQUAL(remainder, expected_mod, AI_Equal, AI_ToString);

  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  AI_FreeArbInt(C);
  AI_FreeArbInt(remainder);
  AI_FreeArbInt(expected_div);
  AI_FreeArbInt(expected_mod);

  TEST_FOOTER();
}

int test_AI_Greater(void) {
  TEST_HEADER();
  ArbInt *negative = AI_NewArbInt_FromLong(-5);
  ArbInt *zero = AI_NewArbInt_FromLong(0);
  ArbInt *positive = AI_NewArbInt_FromLong(6);

  TEST_TRUE(AI_Greater(zero, negative));
  TEST_TRUE(AI_Greater(positive, zero));
  TEST_TRUE(AI_Greater(positive, negative));

  TEST_FOOTER();
}

int test_all(void)
{
  return
    test_AI_Greater() &&
    test_ai_add_unsigned_with_lshift() &&
    test_ai_div_unsigned_by_subtraction__zero_result() &&
    test_ai_div_unsigned_by_subtraction__even_divide() &&
    1;
}
