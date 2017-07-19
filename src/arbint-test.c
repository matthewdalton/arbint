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

  AI_FreeArbInt(expected);
  AI_FreeArbInt(val_plus);
  AI_FreeArbInt(val_C);
  AI_FreeArbInt(val_B);
  AI_FreeArbInt(val_A);

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

  {
    char *s = "0x123";
    char *t = "0x123";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString(t);

    val_sub = AI_Sub(val_A, val_B);
    expected = AI_NewArbInt();
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

  {
    ArbInt *mul;
    numerator = AI_NewArbInt_FromValue(4000000000, 1);
    denominator = AI_NewArbInt_FromLong(2);
    expected = AI_NewArbInt_FromString("0x1DCD65000"); // 8 000 000 000
    mul = AI_Mul(numerator, denominator);
    printf("%s x %s = %s\n", AI_ToStringDec(numerator), AI_ToStringDec(denominator), AI_ToStringDec(mul));
    TEST_EQUAL(mul, expected, AI_Equal, AI_ToString);
    div = AI_Div(mul, denominator, &mod);
    TEST_EQUAL(div, numerator, AI_Equal, AI_ToString);
  }

  {
    ArbInt *mul;
    numerator = AI_NewArbInt_FromString("0x1000000");
    denominator = AI_NewArbInt_FromLong(0x100);
    expected = AI_NewArbInt_FromString("0x100000000");
    mul = AI_Mul(numerator, denominator);
    printf("%s x %s = %s\n", AI_ToStringDec(numerator), AI_ToStringDec(denominator), AI_ToStringDec(mul));
    TEST_EQUAL(mul, expected, AI_Equal, AI_ToString);
    div = AI_Div(mul, denominator, &mod);
    TEST_EQUAL(div, numerator, AI_Equal, AI_ToString);
  }

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

int test_string__base()
{
  TEST_HEADER();

  {
    char *hex = "0x7048860ddf79";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    TEST_EQUAL_STR(AI_ToStringDec(val), "123456789012345");
    AI_FreeArbInt(val);
  }

  {
    char *hex = "0x7048860d";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    TEST_EQUAL_STR(AI_ToStringDec(val), "1883801101");
    AI_FreeArbInt(val);
  }

  {
    char *hex = "0x7048860d";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    char const *bin = AI_ToStringBase(val, 2, (strlen(hex) - 2) * 4);
    TEST_EQUAL_STR(bin, "1110000010010001000011000001101");
    AI_FreeArbInt(val);
  }

  {
    char *hex = "-0x7048860d";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    TEST_EQUAL_STR(AI_ToStringDec(val), "-1883801101");
    AI_FreeArbInt(val);
  }

  {
    char *hex = "-0x7048860D";
    ArbInt *val = AI_NewArbInt_FromString(hex);
    TEST_EQUAL_STR(AI_ToStringBase(val, 16, strlen(hex)), hex);
    AI_FreeArbInt(val);
  }

  TEST_FOOTER();
}

int test_string__speed()
{
  TEST_HEADER();

  {
    char *hex = "0x92389876349827364DF5B2938470923840987230980986109273460ACC765BE8768976F8998308937019834709870DDCBBBBBBBBBBBEFFDEAC98746598273465982736958729876983726";
    /* char *hex = "0x92389876349827364DF5B2938470923840987230980986109273460ACC765BE8768976"; */
    ArbInt *val = AI_NewArbInt_FromString(hex);
    char const *hexstr = AI_ToStringBase(val, 16, strlen(hex));
    printf("As hex: %s\n", hexstr);
    printf("As dec: %s\n", AI_ToStringDec(val));
    TEST_EQUAL_STR(hexstr, hex);
    AI_FreeArbInt(val);
  }

  TEST_FOOTER();
}

int test_setbit__basic()
{
  TEST_HEADER();

  ArbInt *val;
  ArbInt *expected;

  {
    val = AI_NewArbInt_SetBit(0);
    expected = AI_NewArbInt_FromString("0x1");
    TEST_EQUAL(val, expected, AI_Equal, AI_ToString);
  }

  {
    val = AI_NewArbInt_SetBit(1);
    expected = AI_NewArbInt_FromString("0x00000002");
    TEST_EQUAL(val, expected, AI_Equal, AI_ToString);
  }

  {
    val = AI_NewArbInt_SetBit(31);
    expected = AI_NewArbInt_FromString("0x80000000");
    TEST_EQUAL(val, expected, AI_Equal, AI_ToString);
  }

  {
    val = AI_NewArbInt_SetBit(32);
    expected = AI_NewArbInt_FromString("0x100000000");
    TEST_EQUAL(val, expected, AI_Equal, AI_ToString);
  }

  TEST_FOOTER();
}

int test_bititer__basic()
{
  TEST_HEADER();
  ArbInt *val;
  {
    int expected[8] = {0,1,0,1,0,0,1,1};
    val = AI_NewArbInt_FromString("0xCA"); /* 11001010 */
    ArbInt_BitIterator *bi = AI_Make_Bit_Iter(val);
    int i = 0;
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i]); ++i;
    AI_Bit_Iter_Inc(bi);
  }
  TEST_FOOTER();
}

int test_bititer__long()
{
  TEST_HEADER();
  ArbInt *val;
  {
    int expected[8] = {1,0,1,0,1,1,0,1};
    val = AI_NewArbInt_FromString("0xB500000000"); /* 10110101 */
    ArbInt_BitIterator *bi = AI_Make_Bit_Iter(val);
    int i = 0;
    while (i < 32) {
      TEST_TRUE(AI_Bit_Iter_Get(bi) == 0);
      ++i;
      AI_Bit_Iter_Inc(bi);
    }
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
    AI_Bit_Iter_Inc(bi);
    TEST_TRUE(AI_Bit_Iter_Get(bi) == expected[i-32]); ++i;
  }
  TEST_FOOTER();
}

int test_hsb__1()
{
  TEST_HEADER();
  ArbInt *one = AI_NewArbInt_From32(1);
  aibase_t hsb = arbint_get_hsb_position(one);
  TEST_TRUE(hsb == 1);
  TEST_FOOTER();
}

int test_hsb__2()
{
  TEST_HEADER();
  ArbInt *two = AI_NewArbInt_From32(2);
  aibase_t hsb = arbint_get_hsb_position(two);
  TEST_TRUE(hsb == 2);
  TEST_FOOTER();
}

int test_hsb__4()
{
  TEST_HEADER();
  ArbInt *val = AI_NewArbInt_From32(4);
  aibase_t hsb = arbint_get_hsb_position(val);
  TEST_TRUE(hsb == 3);
  TEST_FOOTER();
}

ArbInt *__ai_pow_by_power2(ArbInt const *A, aibase_t exponent);
int test_pow_by_pow2__1()
{
  TEST_HEADER();
  ArbInt *e = AI_NewArbInt_From32(2);
  ArbInt *val = AI_NewArbInt_From32(15);
  ArbInt *ans = __ai_pow_by_power2(val, 1); /* 15^(2^1) */
  ArbInt *expected = AI_NewArbInt_From32(225);
  TEST_EQUAL(ans, expected, AI_Equal, AI_ToString);
  TEST_FOOTER();
}

int test_pow_by_pow2__2()
{
  TEST_HEADER();
  ArbInt *e = AI_NewArbInt_From32(2);
  ArbInt *val = AI_NewArbInt_From32(15);
  ArbInt *ans = __ai_pow_by_power2(val, 2); /* 15^(2^2) */
  ArbInt *expected = AI_NewArbInt_From32(50625);
  TEST_EQUAL(ans, expected, AI_Equal, AI_ToStringDec);
  TEST_FOOTER();
}

int test_pow__two()
{
  TEST_HEADER();
  ArbInt *e = AI_NewArbInt_From32(2);
  ArbInt *val = AI_NewArbInt_From32(15);
  ArbInt *ans = AI_Pow(val, e);
  ArbInt *expected = AI_NewArbInt_From32(225);
  TEST_EQUAL(ans, expected, AI_Equal, AI_ToString);
  TEST_FOOTER();
}

int test_pow__three()
{
  TEST_HEADER();
  ArbInt *e = AI_NewArbInt_From32(3);
  ArbInt *val = AI_NewArbInt_From32(15);
  ArbInt *ans = AI_Pow(val, e);
  ArbInt *expected = AI_NewArbInt_From32(3375);
  TEST_EQUAL(ans, expected, AI_Equal, AI_ToString);
  TEST_FOOTER();
}

int test_pow__seven()
{
  TEST_HEADER();
  ArbInt *e = AI_NewArbInt_From32(7);
  ArbInt *val = AI_NewArbInt_From32(15);
  ArbInt *ans = AI_Pow(val, e);
  ArbInt *expected = AI_NewArbInt_From32(15*15*15*15*15*15*15);
  TEST_EQUAL(ans, expected, AI_Equal, AI_ToString);
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
    test_string__base() &&
    test_setbit__basic() &&
    test_bititer__basic() &&
    test_bititer__long() &&
    test_hsb__1() &&
    test_hsb__2() &&
    test_hsb__4() &&
    test_pow_by_pow2__1() &&
    test_pow_by_pow2__2() &&
    test_pow__two() &&
    test_pow__three() &&
    test_pow__seven() &&
    /* test_string__speed() && */
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

  /* gnomon doesn't work properly without this! */
  setbuf(stdout, NULL);

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
