#ifndef __ARBINT_UNITTEST_H__
#define __ARBINT_UNITTEST_H__

#ifndef STATIC
#define STATIC static
#endif

int test_ai_add_unsigned_with_lshift(void);
int test_all(void);
int test_ai_div_unsigned_by_subtraction__zero_result(void);
int test_ai_div_unsigned_by_subtraction__even_divide(void);

STATIC int
ai_get_hsb_position(unsigned long val);

STATIC ArbInt *
ai_add_unsigned(ArbInt const *A, ArbInt const *B);

STATIC ArbInt *
ai_sub_unsigned(ArbInt const *A, ArbInt const *B);

STATIC unsigned long
ai_add_single_carry(unsigned long a, unsigned long b, int *carry);

STATIC unsigned long
ai_sub_single_carry(unsigned long a, unsigned long b, int *carry);

STATIC int
ai_compare_matchinglength(ArbInt const *A, ArbInt const *B);

STATIC unsigned long
ai_mul_single_carry(unsigned long a, unsigned long b, unsigned long *carry);

STATIC ArbInt *
ai_mul_signed(ArbInt const *A, ArbInt const *B);

STATIC ArbInt *
ai_mul_single_stage(ArbInt const *A, unsigned long b);

STATIC ArbInt *
ai_mul_add_with_lshift(ArbInt const *A, ArbInt const *B, size_t B_lshift);

STATIC ArbInt *
ai_add_unsigned_with_lshift(ArbInt const *A, ArbInt const *B, size_t B_lshift);

STATIC ArbInt *
ai_div_unsigned_by_subtraction(ArbInt const *A, ArbInt const *B, ArbInt **remainder);

/*
 * Testing framework
 */
#define TEST_HEADER()  

#define TEST_TRUE(exp)                                                  \
  if (!(exp)) {                                                         \
    printf("TEST_TRUE Failure (%s, %d): %s\n", __func__, __LINE__, #exp); \
    goto test_exit;                                                     \
  }

#define TEST_EQUAL(got, expt, cmp, toStr)                               \
  if (!(cmp)((got), (expt))) {                                          \
    printf("TEST_EQUAL Failure (%s, line %d):\n", __func__, __LINE__);       \
    printf("    expected %s, got %s\n", (toStr)((expt)), (toStr)((got))); \
    goto test_exit;                                                     \
  }

#define TEST_EQUAL_STR(got, expt)                                       \
  if (strcmp((got), (expt)) != 0) {                                     \
    printf("TEST_EQUAL_STR Failure (%s, %d):\n", __func__, __LINE__);   \
    printf("    expected %s, got %s\n", (expt), (got));                     \
    goto test_exit;                                                     \
  }

#define TEST_FOOTER()                           \
  printf("PASSED : %s\n", __func__);             \
  return 1;                                     \
test_exit:                                      \
 exit(1)

#endif /* __ARBINT_UNITTEST_H__ */
