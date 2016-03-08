#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbint.h"
#include "arbint-unittest.h"

void loop_add(ArbInt **ans, ArbInt *op, unsigned long mult)
{
  unsigned long i;

  *ans = op;

  for (i = 0; i < mult - 1; ++i) {
    *ans = AI_Add(*ans, op);
  }
}

void loop_sub(ArbInt **ans, ArbInt *op, unsigned long mult)
{
  unsigned long i;

  *ans = op;

  for (i = 0; i < mult - 1; ++i) {
    *ans = AI_Sub(*ans, op);
  }
}

int test_AI_FromString_ToString__short_string(void) {
  TEST_HEADER();
  char *s = "0x2BDC545D6B4B88";   /* 12345678901234567 decimal */
  ArbInt *val = AI_NewArbInt_FromString(s);
  char const *str = AI_ToString(val);

  TEST_EQUAL_STR(str, s);

  TEST_FOOTER();
}

int test_AI_FromString_ToString__long_string(void) {
  TEST_HEADER();
  char *s = "0x12345678901234567890123456789012345678901234567890123456789";
  ArbInt *val = AI_NewArbInt_FromString(s);
  char const *str = AI_ToString(val);

  TEST_EQUAL_STR(str, s);

  TEST_FOOTER();
}

int test_addition__overflow(void) {
  TEST_HEADER();
  ArbInt *val_A;
  ArbInt *val_B;
  ArbInt *val_C;

  val_A = AI_NewArbInt_FromString("0x10000000");
  val_B = AI_NewArbInt_FromString("0xF0000000");

  val_C = AI_Add(val_A, val_B);
  ArbInt *expt = AI_NewArbInt_FromString("0x100000000");
  TEST_EQUAL(val_C, expt, AI_Equal, AI_ToString);

  TEST_FOOTER();
}

int test_addition__basic(void) {
  TEST_HEADER();

  ArbInt *val_A;
  ArbInt *val_B;
  ArbInt *val_C;
  ArbInt *val_plus;
  unsigned int mult = 100;

  val_A = AI_NewArbInt_FromLong(0x10000000);
  val_B = AI_NewArbInt_FromLong(0x7FFFFFFF);
  val_C = AI_NewArbInt_FromLong(2);

  val_plus = AI_Add(val_A, val_B);
  val_plus = AI_Add(val_plus, val_C);

  ArbInt *expected = AI_NewArbInt_FromString("0x90000001");

  TEST_EQUAL(val_plus, expected, AI_Equal, AI_ToString);
  AI_FreeArbInt(expected);

  loop_add(&val_plus, val_A, mult);

  expected = AI_NewArbInt_FromString("0x640000000");
  TEST_EQUAL(val_plus, expected, AI_Equal, AI_ToString);

  TEST_FOOTER();
}

int test_subtraction__basic(void) {
  TEST_HEADER();

  ArbInt *val_A;
  ArbInt *val_B;
  ArbInt *val_sub;
  ArbInt *expected;

  val_A = AI_NewArbInt_FromLong(32);
  val_B = AI_NewArbInt_FromLong(26);

  val_sub = AI_Sub(val_A, val_B);
  expected = AI_NewArbInt_FromLong(6);
  TEST_EQUAL(val_sub, expected, AI_Equal, AI_ToString);

  val_sub = AI_Sub(val_B, val_A);
  expected = AI_NewArbInt_FromLong(-6);
  TEST_EQUAL(val_sub, expected, AI_Equal, AI_ToString);


  val_A = AI_NewArbInt_FromLong(34258);
  val_B = AI_NewArbInt_FromLong(0x7000000F);

  val_sub = AI_Sub(val_A, val_B);
  expected = AI_NewArbInt_FromLong(-0x6FFF7A3D);
  TEST_EQUAL(val_sub, expected, AI_Equal, AI_ToString);

  val_sub = AI_Sub(val_B, val_A);
  expected = AI_NewArbInt_FromLong(0x6FFF7A3D);
  TEST_EQUAL(val_sub, expected, AI_Equal, AI_ToString);

  loop_sub(&val_sub, val_B, 24);
  expected = AI_NewArbInt_FromString("-0x9A000014A");
  TEST_EQUAL(val_sub, expected, AI_Equal, AI_ToString);

  TEST_FOOTER();
}

int test_from_string__basic()
{
  TEST_HEADER();

  ArbInt *val_A;

  {
    char *s = "0x1234567890";
    val_A = AI_NewArbInt_FromString(s);
    TEST_EQUAL_STR(AI_ToString(val_A), s);
  }

  {
    char *s = "0x123456789012345678901234";
    val_A = AI_NewArbInt_FromString(s);
    TEST_EQUAL_STR(AI_ToString(val_A), s);
  }

  {
    char *s = "0x1234567890123456789012345678901234567";
    val_A = AI_NewArbInt_FromString(s);
    TEST_EQUAL_STR(AI_ToString(val_A), s);
  }

  {
    char *s = "0x12345678901234567890123456789012345678901234567890123456789";
    val_A = AI_NewArbInt_FromString(s);
    TEST_EQUAL_STR(AI_ToString(val_A), s);
  }
  TEST_FOOTER();
}

int test_subtraction__basic2()
{
  TEST_HEADER();

  ArbInt *val_A;
  ArbInt *val_B;
  ArbInt *val_sub;
  ArbInt *expected;

  {
    char *s = "0x1000000000";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x26");

    val_sub = AI_Sub(val_A, val_B);
    expected = AI_NewArbInt_FromString("0xFFFFFFFDA");
    TEST_EQUAL(val_sub, expected, AI_Equal, AI_ToString);

    val_sub = AI_Sub(val_B, val_A);
    expected = AI_NewArbInt_FromString("-0xFFFFFFFDA");
    TEST_EQUAL(val_sub, expected, AI_Equal, AI_ToString);
  }


  TEST_FOOTER();
}

int test_multiplication__basic()
{
  TEST_HEADER();

  ArbInt *val_A;
  ArbInt *val_B;
  ArbInt *val_C;
  ArbInt *val_plus;
  ArbInt *val_sub;
  ArbInt *val_mul;
  int mul_test = 1;
  unsigned int mult = 100;
  ArbInt *expected;

  { // 1
    char *s = "0x107";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x2");

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0x20E");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 2
    char *s = "0xB107";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x2");

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0x1620E");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 3
    char *s = "0xB107";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x2735");

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0x1B1CB773");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 4
    char *s = "0xB107";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0xFF735");

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0xB0A5B6773");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 5
    char *s = "0x1010";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x10101010");

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0x10202020100");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 6
    char *s = "0xfb563ac9";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0xc56a7def");

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0xC1D1E894C46F06A7");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 7
    char *s = "0x13245B7CEE59";
    char *t = "0xAB873E0D793E8F217A";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString(t);

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0xCD365EC8228E7C0AD58B265FA0F6A");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 8
    char *s = "0xFFFFFFFFFFFFFFFF";
    char *t = "0x2";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString(t);

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0x1FFFFFFFFFFFFFFFE");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 9
    char *s = "0x123456789";
    char *t = "0x123456789";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString(t);

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0x14B66DC326FB98751");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  { // 10
    char *s = "0x123456789";
    char *t = "0x12";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString(t);

    val_mul = AI_Mul(val_A, val_B);
    expected = AI_NewArbInt_FromString("0x147AE147A2");
    TEST_EQUAL(val_mul, expected, AI_Equal, AI_ToString);
  }

  TEST_FOOTER();
}

int test_division__simple()
{
  TEST_HEADER();

  ArbInt *numerator;
  ArbInt *denominator;
  ArbInt *div;
  ArbInt *mod;
  ArbInt *expected;
  ArbInt *exp_mod;

  {
    numerator = AI_NewArbInt_FromLong(12560);
    denominator = AI_NewArbInt_FromLong(8);
    expected = AI_NewArbInt_FromLong(1570);
    div = AI_Div(numerator, denominator, &mod);
    TEST_TRUE(AI_IsZero(mod));
    TEST_EQUAL(div, expected, AI_Equal, AI_ToString);
  }

  /* { */
  /*   ArbInt *mul; */
  /*   numerator = AI_NewArbInt_FromLong(4000000000); */
  /*   denominator = AI_NewArbInt_FromLong(2); */
  /*   expected = AI_NewArbInt_FromString("0x1DCD65000"); // 8 000 000 000 */
  /*   mul = AI_Mul(numerator, denominator); */
  /*   printf("%s\n", AI_ToString(mul)); */
  /*   TEST_EQUAL(mul, expected, AI_Equal, AI_ToString); */
  /*   div = AI_Div(mul, denominator, &mod); */
  /*   TEST_EQUAL(div, numerator, AI_Equal, AI_ToString); */
  /* } */

  /* { */
  /*   ArbInt *mul; */
  /*   numerator = AI_NewArbInt_FromString( "0x1000000"); */
  /*   denominator = AI_NewArbInt_FromLong(0x100); */
  /*   expected = AI_NewArbInt_FromString("0x100000000"); */
  /*   mul = AI_Mul(numerator, denominator); */
  /*   printf("%s\n", AI_ToString(mul)); */
  /*   TEST_EQUAL(mul, expected, AI_Equal, AI_ToString); */
  /*   div = AI_Div(mul, denominator, &mod); */
  /*   TEST_EQUAL(div, numerator, AI_Equal, AI_ToString); */
  /* } */

  {
    numerator = AI_NewArbInt_FromValue(50000, 1);
    denominator = AI_NewArbInt_FromValue(67, 1); /* 746 remainder 18 */
    expected = AI_NewArbInt_FromValue(746, 1);
    exp_mod = AI_NewArbInt_FromValue(18, 1);
    div = AI_Div(numerator, denominator, &mod);
    TEST_EQUAL(div, expected, AI_Equal, AI_ToString);
    TEST_EQUAL(mod, exp_mod, AI_Equal, AI_ToString);
  }

  {
    numerator = AI_NewArbInt_FromValue(50000, -1);
    denominator = AI_NewArbInt_FromValue(67, 1); /* -746 remainder -49 */
    expected = AI_NewArbInt_FromValue(746, -1);
    exp_mod = AI_NewArbInt_FromValue(49, 1);
    div = AI_Div(numerator, denominator, &mod);
    TEST_EQUAL(div, expected, AI_Equal, AI_ToString);
    TEST_EQUAL(mod, exp_mod, AI_Equal, AI_ToString);
  }

  {
    numerator = AI_NewArbInt_FromString("0x7048860ddf79");
    TEST_TRUE(AI_Greater(numerator, AI_NewArbInt()));
    denominator = AI_NewArbInt_FromValue(45678, 1);
    TEST_TRUE(AI_Greater(denominator, AI_NewArbInt()));
    expected = AI_NewArbInt_FromValue(2702762577, 1);
    TEST_TRUE(AI_Greater(expected, AI_NewArbInt()));
    div = AI_Div(numerator, denominator, &mod);
    /* exp_mod = AI_NewArbInt_FromLong(20139); */

    exp_mod = AI_Sub(numerator, AI_Mul(expected, denominator));

    TEST_EQUAL(div, expected, AI_Equal, AI_ToString);
    TEST_EQUAL(mod, exp_mod, AI_Equal, AI_ToString);
  }

  TEST_FOOTER();
}

int test_division__by_zero()
{
  return 1;
}

int test_string__dec()
{
  TEST_HEADER();

  {
    char *hex = "0x7048860ddf79";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    char const *dec = AI_ToStringDec(val);
    TEST_EQUAL_STR(dec, "123456789012345");
  }

  {
    char *hex = "0x7048860d";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    char const *dec = AI_ToStringDec(val);
    TEST_EQUAL_STR(dec, "1883801101");
  }

  {
    char *hex = "0x7048860d";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    char const *bin = AI_ToStringBase(val, 2, (strlen(hex) - 2) * 4);
    TEST_EQUAL_STR(bin, "1110000010010001000011000001101");
  }

  {
    char *hex = "-0x7048860d";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    char const *dec = AI_ToStringDec(val);
    TEST_EQUAL_STR(dec, "-1883801101");
  }

  TEST_FOOTER();
}

int test_all_2()
{
  return
    test_AI_FromString_ToString__short_string() &&
    test_AI_FromString_ToString__long_string() &&
    test_addition__overflow() &&
    test_addition__basic() &&
    test_subtraction__basic() &&
    test_from_string__basic() &&
    test_multiplication__basic() &&
    test_division__simple() &&
    test_division__by_zero() &&
    test_string__dec() &&
    1;
}

int main()
{
  ArbInt *val_A;
  ArbInt *val_B;
  ArbInt *val_C;
  ArbInt *val_plus;
  ArbInt *val_sub;
  ArbInt *val_mul;
  int mul_test = 1;
  unsigned int mult = 100;

  if (!test_all()) {
    printf("Unit tests failed\n");
    return 1;
  }

  if (!test_all_2()) {
    printf("Unit tests failed\n");
    return 1;
  }

  return 0;
}
