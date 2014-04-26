#include <stdio.h>
#include <assert.h>

#include "arbint.h"
#include "arbint-priv.h"

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

ArbInt *AI_Mul(ArbInt const *A, ArbInt const *B)
{
  ArbInt *ans = ai_mul_signed(A, B);
  if (ans == NULL)
    goto error_exit;

  return ans;
 error_exit:
  return NULL;
}

ArbInt *AI_Mul_Value(ArbInt const *A, unsigned long val, int sign)
{
  ArbInt *B = AI_NewArbInt_FromValue(val, sign);
  ArbInt *res = AI_Mul(A, B);
  AI_FreeArbInt(B);
  return res;
}

ArbInt *AI_Div(ArbInt const *A, ArbInt const *B)
{
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
  unsigned long *ptr_a;
  unsigned long *ptr_b;

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
  unsigned long *ptr_a;
  unsigned long *ptr_b;

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
  unsigned long *ptr_a;
  unsigned long *ptr_b;

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

STATIC int
ai_get_hsb_position(unsigned long val)
{
  int bit = 0;
  unsigned short val_short;
  unsigned char  val_byte;
  unsigned char  val_nibble;
  unsigned char  mask;

  if (val == 0) {
    return 0;
  }

  /*
   * There's a slightly better version of essentially the same
   * method in the Linux kernel.
   */
  if (val & 0xFFFF0000) {
    bit += 16;
    val_short = (unsigned short)(val >> 16);
  }
  else {
    val_short = (unsigned short)val;
  }

  if (val_short & 0xFF00) {
    bit += 8;
    val_byte = (unsigned char)(val_short >> 8);
  }
  else {
    val_byte = (unsigned char)val_short;
  }

  if (val_byte & 0xF0) {
    bit += 4;
    val_nibble = (val_byte >> 4);
  }

  bit += 4;
  mask = 0x08;
  while (mask && !(val_nibble & mask)) {
    bit -= 1;
    mask >>= 1;
  }

  return bit;
}


STATIC ArbInt *
ai_add_unsigned(ArbInt const *A, ArbInt const *B)
{
#if 0
  return ai_add_unsigned_with_lshift(A, B, 0);
#else
  ArbInt *ans = AI_NewArbInt();
  int carry = 0;
  int i;
  int j;

  if (ans == NULL)
    goto error_exit;

  ans->dataLen = (A->dataLen > B->dataLen ? A->dataLen : B->dataLen);
  
  AI_Resize(ans, ans->dataLen);

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
      ai_add_single_carry
      (A->data[A->dataLen - 1 - i],
       B->data[B->dataLen - 1 - i], 
       &carry);

    if (carry) {
      AI_Resize(ans, ans->dataLen + 1);
    }
  }

  for (; i < A->dataLen; ++i) {
    ans->data[j - i] = 
      ai_add_single_carry(A->data[A->dataLen - 1 - i], 0, &carry);

    if (carry) {
      AI_Resize(ans, ans->dataLen + 1);
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
  int carry = 0;
  int i;
  int j;

  ans->dataLen = (A->dataLen > B->dataLen ? A->dataLen : B->dataLen);
  
  AI_Resize(ans, ans->dataLen);

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
   * If there is still a carry, we continue subtracting against
   * the carry until it goes away.
   */
  for (; carry; ++i) {
    assert(j - i >= 0);
    ans->data[j - i] =
      ai_sub_single_carry(A->data[A->dataLen - 1 - i], 0, &carry);
  }

  return ans;
}


/*
 * @param[in,out] carry		Carry must be valid and set to either 0 or 1
 *				as input. Will be set to 0 or 1 on return.
 */
STATIC unsigned long
ai_add_single_carry(unsigned long a, unsigned long b, int *carry)
{
  unsigned long ans;
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
  printf("    Adding %x, %x and %d: %x + carry of %d\n", a, b, inp_carry, 
	 ans, *carry);

  return ans;
}


STATIC unsigned long
ai_sub_single_carry(unsigned long a, unsigned long b, int *carry)
{
  unsigned long ans;
  int inp_carry = *carry;

  assert(carry != NULL);
  assert(*carry == 0 || *carry == 1);

  ans = a - b - *carry;

  *carry = (b > a) ? 1 : 0;

  printf("Subtracting %lu, %lu and %d: %lu & carry of %d\n", a, b, inp_carry,
	 ans, *carry);

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
  unsigned long *ptr_a;
  unsigned long *ptr_b;
  size_t len;

  assert(A->dataLen == B->dataLen);

  len = A->dataLen;
  ptr_a = A->data;
  ptr_b = B->data;
  while (*ptr_a == *ptr_b && len > 0) {
    ++ptr_a;
    ++ptr_b;
    --len;
  }
  return *ptr_b == *ptr_a ? 0 : (*ptr_a < *ptr_b ? -1 : 1);
}


STATIC unsigned long
ai_mul_single_carry(unsigned long a, unsigned long b, unsigned long *carry)
{
  unsigned long result16_1;
  unsigned long result16_2;
  unsigned long result16_3;
  unsigned long result16_4;
  unsigned long final_result = 0;
  unsigned long final_carry = 0;

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
  unsigned long result;
  unsigned long carry;
  int index = 0;
  ArbInt *ans;

  ans = AI_NewArbInt();
  if (ans == NULL)
    goto error_exit;

#if 1
  /*
   * Oh dear. O(n^2). Replace with something like Karatsuba algorithm.
   */
  ArbInt *partial;
  ArbInt *old_ans;
  for (index = B->dataLen - 1; index >= 0; --index) {
    partial = ai_mul_single_stage(A, B->data[index]);
    old_ans = ans;
    ans = ai_add_unsigned_with_lshift(ans, partial, B->dataLen-1-index);
    AI_FreeArbInt(old_ans);
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
    AI_Resize(ans, 2);
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
ai_mul_single_stage(ArbInt const *A, unsigned long b)
{
  ArbInt *ans = AI_NewArbInt();	/* == 0 */
  ArbInt *old_ans;

  int i;
  ArbInt *partial = AI_NewArbInt();
  AI_Resize(partial, 2);
  partial->dataLen = 2;
  unsigned long carry;
  unsigned long val;
  for (i = A->dataLen - 1; i >= 0; --i) {
    val = ai_mul_single_carry(A->data[i], b, &carry);
    partial->data[0] = carry;
    partial->data[1] = val;
    old_ans = ans;
    ans = ai_add_unsigned_with_lshift(ans, partial, A->dataLen-1-i);
    /* TODO: Use a memory pool to avoid repeated alloc/frees */
    printf("Freeing %x\n", old_ans);
    printf("Partial answer: %s\n", AI_ToString(ans));
    AI_FreeArbInt(old_ans);
  }
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
  int b;
  int a;

  if (B_lshift > AI_MAX_LENGTH - B->dataLen) {
    /* Too big */
    goto error_exit;
  }

  ans = AI_NewArbInt();
  if (ans == NULL)
    goto error_exit;

  AI_Resize(ans, MAX(A->dataLen, (B->dataLen + B_lshift)));

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
  while (A->dataLen + a > 0 && b > 0) {
    ans->data[B->dataLen + b - 1] = A->data[A->dataLen + a - 1];
    a--;
    b--;
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
    while (A->dataLen + a > 0 && B->dataLen + b > 0) {
      ans->data[B->dataLen + b - 1] =
	ai_add_single_carry( B->data[B->dataLen + b - 1],
			     A->data[A->dataLen + a - 1],
			     &carry );
      a--;
      b--;
    }

    if (B->dataLen + b == 0) {
      /*
        B___ +
      AAAAAA
      */
      while (A->dataLen + a > 0) {
	ans->data[B->dataLen + b - 1] =
	  ai_add_single_carry( 0,
			       A->data[A->dataLen + a - 1],
			       &carry );
	a--;
	b--;
      }
    }
    else {
      /*
      BBB___ +
        AAAA
      */
      while (B->dataLen + b > 0) {
	ans->data[B->dataLen + b - 1] =
	  ai_add_single_carry( B->data[B->dataLen + b - 1],
			       0,
			       &carry );
	/* a--; */
	b--;
      }
    }
  }
  else {
    /*
    A->dataLen + a < 0
    
    B____ +
      AAA
    */
    while (B->dataLen + b > 0) {
      ans->data[B->dataLen + b - 1] =
	ai_add_single_carry( B->data[B->dataLen + b - 1],
			     0,
			     &carry );
      /* a--; */
      b--;
    }
  }

  if (carry > 0) {
    AI_Resize(ans, ans->dataLen + 1);
    /* error check? */
    ans->dataLen += 1;
    ans->data[0] = carry;
  }

  return ans;

 error_exit:
  if (ans != NULL)
    AI_FreeArbInt(ans);
  return NULL;
}
