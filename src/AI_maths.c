/* -*- mode: c; c-basic-offset: 2; -*- */

#include <stdio.h>
#include <assert.h>

#include "arbint.h"
#include "arbint-priv.h"

#include "AI_new.h"
#include "AI_utils.h"

STATIC int
ai_get_hsb_position(aibase_t val);

STATIC ArbInt *
ai_add_unsigned(ArbInt const *A, ArbInt const *B);

STATIC ArbInt *
ai_sub_unsigned(ArbInt const *A, ArbInt const *B);

STATIC aibase_t
ai_add_single_carry(aibase_t a, aibase_t b, int *carry);

STATIC aibase_t
ai_sub_single_carry(aibase_t a, aibase_t b, int *carry);

STATIC int
ai_compare_matchinglength(ArbInt const *A, ArbInt const *B);

STATIC aibase_t
ai_mul_single_carry(aibase_t a, aibase_t b, int *carry);

STATIC ArbInt *
ai_mul_signed(ArbInt const *A, ArbInt const *B);

STATIC ArbInt *
ai_mul_single_stage(ArbInt const *A, aibase_t b);

STATIC ArbInt *
ai_mul_add_with_lshift(ArbInt const *A, ArbInt const *B, size_t B_lshift);

STATIC ArbInt *
ai_add_unsigned_with_lshift(ArbInt const *A, ArbInt const *B, size_t B_lshift);

STATIC aibase_t
ai_msbit_in_value(aibase_t val);

STATIC aibase_t
ai_msbit_in_arbint(ArbInt const *val);

STATIC ArbInt *
ai_div_find_largest_multiple_smaller_than(ArbInt const *multi, ArbInt const *bigger);

STATIC ArbInt *
ai_div_unsigned_by_scaled_subtraction(ArbInt const *A, ArbInt const *B, ArbInt **remainder);

STATIC ArbInt *
ai_div_unsigned_by_subtraction(ArbInt const *A, ArbInt const *B, ArbInt **remainder);

ArbInt *AI_Add(ArbInt const *A, ArbInt const *B)
{
  if (A->sign != B->sign) {
    ArbInt temp;
    /*
     * Do subtraction instead
     */
    temp.data = B->data;
    temp.dataLen = B->dataLen;
    temp.sign = -1 * B->sign;
    return AI_Sub(A, &temp);
  }
  else {
    ArbInt *ans;
    ans = ai_add_unsigned(A, B);
    ans->sign = A->sign;
    return ans;
  }
}

ArbInt *AI_Add_Value(ArbInt const *A, unsigned long val, int sign)
{
  ArbInt *B = AI_NewArbInt_FromValue(val, sign);
  ArbInt *res = AI_Add(A, B);
  AI_FreeArbInt(B);
  return res;
}

ArbInt *AI_Add_And_Free(ArbInt *A, ArbInt *B)
{
  ArbInt *res = AI_Add(A, B);
  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  return res;
}

ArbInt *AI_Sub(ArbInt const *A, ArbInt const *B)
{
  if (A->sign != B->sign) {
    if (A->sign > 0 && B->sign < 0) {
      ArbInt temp;
      temp = *B;
      temp.sign = 1;
      return AI_Add(A, &temp);
    }
    else {
      /* A < 0 and B > 0 */
      ArbInt *ans;
      ans = AI_Add(AI_Abs(A), AI_Abs(B));
      ans->sign = -1;
      return ans;
    }
  }
  else if (A->sign < 0) {
    ArbInt tA = *A;
    ArbInt tB = *B;
    tA.sign = 1;
    tB.sign = 1;
    return AI_Sub(&tB, &tA);
  }
  else {
    return ai_sub_unsigned(A, B);
  }
}

ArbInt *AI_Sub_And_Free(ArbInt *A, ArbInt *B)
{
  ArbInt *res = AI_Sub(A, B);
  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  return res;
}

ArbInt *AI_Mul(ArbInt const *A, ArbInt const *B)
{
  return ai_mul_signed(A, B);
}

ArbInt *AI_Mul_Value(ArbInt const *A, unsigned long val, int sign)
{
  ArbInt *B = AI_NewArbInt_FromValue(val, sign);
  ArbInt *res = AI_Mul(A, B);
  AI_FreeArbInt(B);
  return res;
}

ArbInt *AI_Mul_And_Free(ArbInt *A, ArbInt *B)
{
  ArbInt *res = AI_Mul(A, B);
  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  return res;
}

ArbInt *AI_Div(ArbInt const *A, ArbInt const *B, ArbInt **remainder)
{
  if (A->sign == B->sign) {
    int mod_sign = 1;
    if (A->sign < 0) {
      ArbInt posA = *A;
      ArbInt posB = *B;
      posA.sign = 1;
      posB.sign = 1;
      A = &posA;
      B = &posB;
      mod_sign = -1;
    }
    ArbInt *result = ai_div_unsigned_by_scaled_subtraction(A, B, remainder);
    (*remainder)->sign = mod_sign;
    return result;
  } else {
    ArbInt posA = *A;
    ArbInt posB = *B;
    posA.sign = 1;
    posB.sign = 1;
    A = &posA;
    B = &posB;
    ArbInt *posRemainder;
    ArbInt *result = ai_div_unsigned_by_scaled_subtraction(A, B, &posRemainder);
    result->sign = -1;
    *remainder = AI_Sub(&posB, posRemainder);
    (*remainder)->sign = 1;
    AI_FreeArbInt(posRemainder);
    return result;
  }
}

ArbInt *AI_Div_And_Free(ArbInt *A, ArbInt *B, ArbInt **remainder)
{
  ArbInt *res = AI_Div(A, B, remainder);
  AI_FreeArbInt(A);
  AI_FreeArbInt(B);
  return res;
}

ArbInt *AI_Abs(ArbInt const *A)
{
  ArbInt *ret = AI_NewArbInt();
  *ret = *A;
  ret->sign = 1;
  return ret;
}

int AI_Greater(ArbInt const *A, ArbInt const *B)
{
  int invert;
  aibase_t *ptr_a;
  aibase_t *ptr_b;

  if (A->sign != B->sign) {
    if (A->sign == 1) {
      return 1;	/* true */
    }
    else {
      return 0;	/* false */
    }
  }

  invert = 0;
  if (A->sign == -1) {
    invert = 1;
  }

  if (A->dataLen == B->dataLen && A->dataLen == 1) {
    return invert ? B->data[0] > A->data[0] : A->data[0] > B->data[0];
  }

  if (A->dataLen > B->dataLen) {
    return invert ? 0 : 1;
  }

  if (A->dataLen < B->dataLen) {
    return invert ? 1 : 0;
  }

  /*
  if (ai_get_hsb_position(A->data[0]) > ai_get_hsb_position(B->data[0])) {
  return invert ? 0 : 1;
  }
  */

  return (invert ? 1 : -1) * (ai_compare_matchinglength(A, B) == 1 ? 1 : 0);
}

int AI_Equal(ArbInt const *A, ArbInt const *B)
{
  if (A == NULL || B == NULL) {
	if (A == B) return 1;
	else return 0;
  }


  if (A->sign != B->sign) {
    return 0;
  }

  if (A->dataLen != B->dataLen) {
    return 0;
  }

  return (ai_compare_matchinglength(A, B) == 0 ? 1 : 0);
}

int AI_Less(ArbInt const *A, ArbInt const *B)
{
  /*
   * Not implemented in terms of "Greater" and "Equal" for
   * performance reasons.
   */
  int invert;
  aibase_t *ptr_a;
  aibase_t *ptr_b;

  if (A->sign != B->sign) {
    if (A->sign == -1) {
      return 1;	/* true */
    }
    else {
      return 0;	/* false */
    }
  }

  invert = 0;
  if (A->sign == -1) {
    invert = 1;
  }

  if (A->dataLen == B->dataLen && A->dataLen == 1) {
    return invert ? B->data[0] < A->data[0] : A->data[0] < B->data[0];
  }

  if (A->dataLen < B->dataLen) {
    return invert ? 0 : 1;
  }

  if (A->dataLen > B->dataLen) {
    return invert ? 1 : 0;
  }

  return (invert ? 1 : -1) * (ai_compare_matchinglength(A, B) == -1 ? 1 : 0);
}

int AI_GreaterOrEqual(ArbInt const *A, ArbInt const *B)
{
  return AI_Greater(A, B) || AI_Equal(A, B);
}

int AI_LessOrEqual(ArbInt const *A, ArbInt const *B)
{
  return AI_Less(A, B) || AI_Equal(A, B);
}

ArbInt *AI_Neg(ArbInt const *A)
{
  ArbInt *ret = AI_NewArbInt();
  *ret = *A;
  ret->sign = -1 * A->sign;
  return ret; 
}

/*********************************************************************
 Private functions
 *********************************************************************/

STATIC ArbInt *
ai_add_unsigned(ArbInt const *A, ArbInt const *B)
{
#if 1
  return ai_add_unsigned_with_lshift(A, B, 0);
#else
  ArbInt *ans = AI_NewArbInt();
  int carry = 0;
  int i;
  int j;

  if (ans == NULL)
    goto error_exit;

  ans->dataLen = (A->dataLen > B->dataLen ? A->dataLen : B->dataLen);
  
  ai_resize(ans, ans->dataLen);

  /*
   * Make B hold the lower value
   */
  if (B->dataLen > A->dataLen) {
    ArbInt const *temp;
    temp = B;
    B = A;
    A = temp;
  }

  j = ans->dataLen - 1;
  for (i = 0; i < B->dataLen; ++i) {
    ans->data[j - i] = 
      ai_add_single_carry(A->data[A->dataLen - 1 - i],
			  B->data[B->dataLen - 1 - i], 
			  &carry);

    if (carry) {
      ai_resize(ans, ans->dataLen + 1);
    }
  }

  for (; i < A->dataLen; ++i) {
    ans->data[j - i] = 
      ai_add_single_carry(A->data[A->dataLen - 1 - i], 0, &carry);

    if (carry) {
      ai_resize(ans, ans->dataLen + 1);
    }
  }

  if (carry) {
    ans->data[0] = 1;
  }

  return ans;
 error_exit:
  return NULL;
#endif
}


STATIC ArbInt *
ai_sub_unsigned(ArbInt const *A, ArbInt const *B)
{
  ArbInt *ans = AI_NewArbInt();
  unsigned long newlen;
  int carry = 0;
  int i;
  int j;

  newlen = (A->dataLen > B->dataLen ? A->dataLen : B->dataLen);
  
  if (!ans || !ai_resize(ans, newlen)) {
    AI_FreeArbInt(ans);
    return NULL;
  }

  /*
   * Make B hold the lower value
   */
  if (AI_Greater(B, A)) {
    ArbInt const *temp;
    temp = B;
    B = A;
    A = temp;

    /*
     * Result will be negative
     */
    ans->sign = -1;
  }

  j = ans->dataLen - 1;
  for (i = 0; i < B->dataLen; ++i) {
    ans->data[j - i] =
      ai_sub_single_carry
      (A->data[A->dataLen - 1 - i],
       B->data[B->dataLen - 1 - i],
       &carry);
  }

  /*
   * Continue subtracting to fill out the value, taking into account
   * any carry.
   */
  for (; j - i >= 0; ++i) {
    assert(j - i >= 0);
    ans->data[j - i] =
      ai_sub_single_carry(A->data[A->dataLen - 1 - i], 0, &carry);
  }

  ai_normalise(ans);

  return ans;
}


/*
 * @param[in,out] carry		Carry must be valid and set to either 0 or 1
 *				as input. Will be set to 0 or 1 on return.
 */
STATIC aibase_t
ai_add_single_carry(aibase_t a, aibase_t b, int *carry)
{
  aibase_t ans;
  int inp_carry = *carry;

  assert(carry != NULL);
  assert(*carry == 0 || *carry == 1);

  ans = a + b + *carry;

  if (ans < a + *carry || ans < b + *carry) {
    *carry = 1;
  }
  else {
    *carry = 0;
  }

  /* printf("    Adding %lu, %lu and %d: %lu + carry of %d\n", a, b, inp_carry,  */
  /* 	 ans, *carry); */
  /* printf("    Adding %x, %x and %d: %x + carry of %d\n", a, b, inp_carry,  */
  /* 	 ans, *carry); */

  return ans;
}


STATIC aibase_t
ai_sub_single_carry(aibase_t a, aibase_t b, int *carry)
{
  aibase_t ans;
  int inp_carry = *carry;

  assert(carry != NULL);
  assert(*carry == 0 || *carry == 1);

  ans = a - b - *carry;

  *carry = (b > a) ? 1 : 0;

  /* printf("Subtracting %lu, %lu and %d: %lu & carry of %d\n", a, b, inp_carry, */
  /* 	 ans, *carry); */

  return ans;
}


/**
 * Assumes A and B are have the same data length
 *
 * @returns
 *	-1 if A < B
 *	 0 if A == B
 *	 1 if A > B
 */
STATIC int
ai_compare_matchinglength(ArbInt const *A, ArbInt const *B)
{
  aibase_t *ptr_a;
  aibase_t *ptr_b;
  size_t len;

  assert(A->dataLen == B->dataLen);

  len = A->dataLen;
  ptr_a = A->data;
  ptr_b = B->data;
  while (len > 1 && *ptr_a == *ptr_b) {
    ++ptr_a;
    ++ptr_b;
    --len;
  }
  return *ptr_b == *ptr_a ? 0 : (*ptr_a < *ptr_b ? -1 : 1);
}


STATIC aibase_t
ai_mul_single_carry(aibase_t a, aibase_t b, int *carry)
{
  aibase_t result16_1;
  aibase_t result16_2;
  aibase_t result16_3;
  aibase_t result16_4;
  aibase_t final_result = 0;
  int final_carry = 0;

#define HIGH16(val) (((val) & 0xFFFF0000) >> 16)
#define LOW16(val)  ((val) & 0x0000FFFF)
#define CARRY(val)  HIGH16(val)
#define REMOVE_CARRY(val) ((val) &= 0x0000FFFF)

  /*
   * Perform long multiplication using 16 bit chunks. This is to
   * avoid using a 64bit type which may not be available on all
   * systems.
   *
   *      aa x
   *      bb
   *     ---
   *     ???
   */
  result16_1 = LOW16(b) * LOW16(a);
  result16_2 = LOW16(b) * HIGH16(a) + CARRY(result16_1);
  REMOVE_CARRY(result16_1);
  result16_3 = HIGH16(b) * LOW16(a);
  result16_4 = HIGH16(b) * HIGH16(a) + CARRY(result16_3);
  REMOVE_CARRY(result16_3);

  final_result  = result16_1;
  final_carry   = CARRY(result16_2);
  REMOVE_CARRY(result16_2);

  final_carry += CARRY(result16_2 + result16_3);

  final_result += (result16_2 << 16);

  final_result += (result16_3 << 16);
  final_carry  += result16_4;

  *carry = final_carry;
  return final_result;
}

STATIC ArbInt *
ai_mul_signed(ArbInt const *A, ArbInt const *B)
{
  aibase_t result;
  int carry;
  int index = 0;
  ArbInt *ans;

  ans = AI_NewArbInt();
  if (ans == NULL)
    goto error_exit;

#if 1
  /*
   * Basic O(n^2) algorithm. Replace with something like Karatsuba algorithm.
   */
  ArbInt *partial;
  ArbInt *old_ans;
  int pad = 24;
  for (index = B->dataLen - 1; index >= 0; --index) {
    partial = ai_mul_single_stage(A, B->data[index]);
    old_ans = ans;

    /* {
      char *debug_str =  "            %%%ds\n";
      char buff[64];
      sprintf(buff, debug_str, pad-8*(B->dataLen-1-index));
      printf(buff, AI_ToString(partial));
      } */
    ans = ai_add_unsigned_with_lshift(ans, partial, B->dataLen-1-index);
    AI_FreeArbInt(old_ans);
    AI_FreeArbInt(partial);
  }
  ans->sign *= B->sign;
  return ans;
#else
  /* temporary */
  if (A->dataLen > 1 || B->dataLen > 1)
    return NULL;
  if (A->sign < 0 || B->sign < 0)
    return NULL;

  result = ai_mul_single_carry(A->data[0], B->data[0], &carry);


  if (carry > 0) {
    ai_resize(ans, 2);
    /* error check? */
    ans->dataLen = 2;
    ans->data[0] = carry;
    ans->data[1] = result;
  }
  else {
    ans->data[0] = result;
  }

  ans->sign = A->sign * B->sign;

  return ans;

  /* end temporary */
#endif

 error_exit:
  return NULL;
}

STATIC ArbInt *
ai_mul_single_stage(ArbInt const *A, aibase_t b)
{
  ArbInt *ans = AI_NewArbInt();	/* == 0 */
  ArbInt *old_ans;

  int i;
  ArbInt *partial = AI_NewArbInt();
  if (!ai_resize(partial, 2)) {
    AI_FreeArbInt(ans);
    AI_FreeArbInt(partial);
    return NULL;
  }

  int carry;
  aibase_t val;
  for (i = A->dataLen - 1; i >= 0; --i) {
    val = ai_mul_single_carry(A->data[i], b, &carry);
    if (carry) {
      partial->data[0] = carry;
      partial->data[1] = val;
      partial->dataLen = 2;
    }
    else {
      partial->data[0] = val;
      partial->dataLen = 1;
    }
    old_ans = ans;
    ans = ai_add_unsigned_with_lshift(ans, partial, A->dataLen-1-i);
    /* TODO: Use a memory pool to avoid repeated alloc/frees */
    /* printf("Freeing %x\n", old_ans); */
    /* printf("Partial answer: %s\n", AI_ToString(ans)); */
    AI_FreeArbInt(old_ans);
  }
  AI_FreeArbInt(partial);
  ans->sign = A->sign;
  return ans;
}

STATIC ArbInt *
ai_add_unsigned_with_lshift(ArbInt const *A, ArbInt const *B, size_t B_lshift)
{
  ArbInt *ans = NULL;
  int carry;
  int i;
  int j;
  int c;
  int b;
  int a;

  if (B_lshift > AI_MAX_LENGTH - B->dataLen) {
    /* Too big */
    goto error_exit;
  }

  ans = AI_NewArbInt();
  if (ans == NULL)
    goto error_exit;

  if (!ai_resize(ans, MAX(A->dataLen, (B->dataLen + B_lshift))))
    goto error_exit;

  /*
   * a is the array index of A->data, but 0 is one past the last item.
   * eg. A->data[A->dataLen + a] where -dataLen <= a <= -1
   *
   * b is the array index of B->data and ans->data.
   * For the range  -dataLen <= b <= -1,
   *   B->data[B->dataLen + b] is valid.
   * For the range  -dataLen <= b <= B_lshift-1
   *   ans->data[B->dataLen + b] is valid.
   *
   * All of that, plus 1.
   */
  a = 0;
  b = B_lshift;
  c = ans->dataLen;
  while (A->dataLen + a > 0 && b > 0 && c > 0) {
    assert(B->dataLen + b - 1 < ans->dataLen);
    ans->data[c-1] = A->data[A->dataLen + a - 1];
    a--;
    b--;
    c--;
  }

  carry = 0;
  if (b == 0) {
    /*
    Either:
       B____ +
     AAAAAAA
     or:
     BBB____ +
      AAAAAA
    */

    /* Add the overlapping bytes */
    while (A->dataLen + a > 0 && B->dataLen + b > 0 && c > 0) {
      assert(B->dataLen + b - 1 < ans->dataLen);
      ans->data[c-1] =
	ai_add_single_carry( B->data[B->dataLen + b - 1],
			     A->data[A->dataLen + a - 1],
			     &carry );
      a--;
      b--;
      c--;
    }

    if (B->dataLen + b == 0) {
      /*
        B___ +
      AAAAAA
      */
      while (A->dataLen + a > 0 && c > 0) {
	/* assert(B->dataLen + b - 1 < ans->dataLen); */
	ans->data[c-1] =
	  ai_add_single_carry( 0,
			       A->data[A->dataLen + a - 1],
			       &carry );
	a--;
	b--;
        c--;
      }
    }
    else {
      /*
      BBB___ +
        AAAA
      */
      while (B->dataLen + b > 0 & c > 0) {
	assert(B->dataLen + b - 1 < ans->dataLen);
	ans->data[c-1] =
	  ai_add_single_carry( B->data[B->dataLen + b - 1],
			       0,
			       &carry );
	/* a--; */
	b--;
        c--;
      }
    }
  }
  else {
    /*
    A->dataLen + a < 0
    
    B____ +
      AAA
    */
    while (B->dataLen + b > 0 && c > 0) {
      assert(B->dataLen + b - 1 < ans->dataLen);
      ans->data[c-1] =
	ai_add_single_carry( B->data[B->dataLen + b - 1],
			     0,
			     &carry );
      /* a--; */
      b--;
      c--;
    }
  }

  if (carry > 0) {
    if (!ai_resize(ans, ans->dataLen + 1))
      goto error_exit;
    /* assert(0);			/\* Disabling as I suspect a bug *\/ */
    ans->data[0] = carry;
  }

  return ans;

 error_exit:
  if (ans != NULL)
    AI_FreeArbInt(ans);
  return NULL;
}

/**** Division ****/

STATIC aibase_t
ai_msbit_in_value(aibase_t val)
{
  aibase_t mask = 1 << (8 * sizeof(aibase_t) - 1);
  aibase_t bit = sizeof(aibase_t) * 8 - 1;
  while (mask && ((val & mask) == 0)) {
    mask >>= 1;
    --bit;
  }
  return bit;
}

STATIC aibase_t
ai_msbit_in_arbint(ArbInt const *val)
{
  unsigned long set_byte = 0;

  while (val->data[set_byte] == 0 && set_byte < val->dataLen)
    set_byte++;

  return (val->dataLen - set_byte - 1) * sizeof(aibase_t) * 8 +
    ai_msbit_in_value(val->data[set_byte]);
}

STATIC ArbInt *
ai_div_find_largest_multiple_smaller_than(ArbInt const *multi, ArbInt const *bigger)
{

  ArbInt *largest;
  ArbInt *res;
  ArbInt *multiplier;
  ArbInt *mres;

  if (AI_IsZero(multi)) {
    return NULL;
  }

  largest = AI_NewArbInt_FromCopy(multi);
  multiplier = AI_NewArbInt_FromLong(1);

  /* First approximation */
  unsigned long set_byte = 0;
  while (bigger->data[set_byte] == 0 && set_byte < bigger->dataLen)
    set_byte++;

  if (set_byte == bigger->dataLen) {
    AI_FreeArbInt(multiplier);
    multiplier = NULL;
    goto free_mem;
  }

  /*
   * Set the first approximation to be the ms-bit of the byte one less
   * than the most significant byte in bigger. This guarantees the values is
   * lower than 'bigger' even after it's multiplied by 2 in the first
   * iteration of the while loop.
   *
   * Alternatively, just find the largest bit in the first non-zero byte
   * of 'bigger' and calculate the value from that. It's O(1) on the size of
   * bigger!
   */
  aibase_t val = bigger->data[set_byte];
  aibase_t mask = 1 << (8 * sizeof(aibase_t) - 1);
  while (mask && ((val & mask) == 0))
    mask >>= 1;

  aibase_t bit = 0;
  while (mask > 1) {
    mask >>= 1;
    ++bit;
  }

#if 0                           /* Experimental version not quite ready */
  ArbInt *mult = AI_NewArbInt_SetBit(bit
                                     + ((set_byte - (multi->dataLen - 1)) * sizeof(aibase_t) * 8)
                                     - ai_msbit_in_arbint(multi));

  printf("%s should be smaller than %s\n", AI_ToString(AI_Mul(multi, mult)), AI_ToString(bigger));

  return mult;
#endif

  /* Quick'n'dirty version */
  while (1) {
    res = AI_Mul_Value(largest, 2, 1);
    mres = AI_Mul_Value(multiplier, 2, 1);
    if (AI_Greater(res, bigger))
      goto free_mem;
    AI_FreeArbInt(largest);
    largest = res;
    AI_FreeArbInt(multiplier);
    multiplier = mres;
  }

  AI_FreeArbInt(multiplier);
  multiplier = NULL;

 free_mem:
  AI_FreeArbInt(largest);
  AI_FreeArbInt(res);
  AI_FreeArbInt(mres);
  return multiplier;
}

/*
 * Calculates integer A/B by subtracting scaled values of B from A.
 *
 * A and B must be unsigned values
 */
STATIC ArbInt *
ai_div_unsigned_by_scaled_subtraction(ArbInt const *A, ArbInt const *B, ArbInt **remainder)
{
  if (AI_IsZero(B)) {
    return NULL;
  }

  *remainder = AI_NewArbInt_FromCopy(A);

  if (AI_Greater(B, A)) {
    return AI_NewArbInt();      /* 0 */
  }

  ArbInt *multiple;
  ArbInt *result = AI_NewArbInt_FromLong(0);
  ArbInt *tally;
  ArbInt *tmp2;
  ArbInt *rmdr;
  while (AI_GreaterOrEqual(*remainder, B)) {
    /* Find the largest base-2 multiple of B smaller than A */
    multiple = ai_div_find_largest_multiple_smaller_than(B, *remainder);
    tally = AI_Mul(multiple, B);
    /* How many of these fit into A? */
    tmp2 = ai_div_unsigned_by_subtraction(*remainder, tally, &rmdr);
    multiple = AI_Mul_And_Free(multiple, tmp2);
    tally = AI_Mul(multiple, B);
    result = AI_Add_And_Free(result, multiple);
    *remainder = AI_Sub_And_Free(*remainder, tally);
  }
  return result;
}

/*
 * Calculates integer A/B by subtracting B from A until only the remainder remains.
 */
STATIC ArbInt *
ai_div_unsigned_by_subtraction(ArbInt const *A, ArbInt const *B, ArbInt **remainder)
{
  if (AI_IsZero(B)) {
    return NULL;
  }

  *remainder = AI_NewArbInt_FromCopy(A);

  if (AI_Greater(B, A)) {
    return AI_NewArbInt();      /* 0 */
  }

  ArbInt *tally = AI_NewArbInt();
  ArbInt *tally2;
  while (AI_GreaterOrEqual(*remainder, B)) {
    ArbInt *sub = AI_Sub(*remainder, B);
    AI_FreeArbInt(*remainder);
    *remainder = sub;

    tally2 = AI_Add_Value(tally, 1, 1);
    AI_FreeArbInt(tally);
    tally = tally2;
  }

  return tally;
}

int AI_IsZero(ArbInt const *A)
{
  int i;
  for (i = 0; i < A->dataLen; i++) {
    if (A->data[i] != 0) {
      return 0;
    }
  }
  return 1;
}
