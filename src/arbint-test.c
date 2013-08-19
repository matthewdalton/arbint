#include <stdio.h>
#include <stdlib.h>

#include "arbint.h"

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

  val_A = AI_NewArbInt_FromLong(0x10000000);
  val_B = AI_NewArbInt_FromLong(0x7FFFFFFF);
  val_C = AI_NewArbInt_FromLong(2);

  printf("val_A: %s\n", AI_ToString(val_A));
  printf("val_B: %s\n", AI_ToString(val_B));

  val_plus = AI_Add(val_A, val_B);
  val_plus = AI_Add(val_plus, val_C);

  printf("val_A + val_B = %s\n", AI_ToString(val_plus));

  loop_add(&val_plus, val_A, mult);

  printf("val_A * %u = %s\n", mult, AI_ToString(val_plus));

  /* sub */
  val_A = AI_NewArbInt_FromLong(32);
  val_B = AI_NewArbInt_FromLong(26);

  printf("val_A: %s\n", AI_ToString(val_A));
  printf("val_B: %s\n", AI_ToString(val_B));

  val_sub = AI_Sub(val_A, val_B);
  printf("val_A - val_B = %s\n", AI_ToString(val_sub));

  val_sub = AI_Sub(val_B, val_A);
  printf("val_B - val_A = %s\n", AI_ToString(val_sub));


  val_A = AI_NewArbInt_FromLong(34258);
  val_B = AI_NewArbInt_FromLong(0x7000000F);

  printf("val_A: %s\n", AI_ToString(val_A));
  printf("val_B: %s\n", AI_ToString(val_B));

  val_sub = AI_Sub(val_A, val_B);
  printf("val_A - val_B = %s\n", AI_ToString(val_sub));

  val_sub = AI_Sub(val_B, val_A);
  printf("val_B - val_A = %s\n", AI_ToString(val_sub));

  loop_sub(&val_sub, val_B, 24);
  printf("loop_sub result: %s\n", AI_ToString(val_sub));


  printf("FromString tests\n");

  {
    char *s = "0x1234567890";
    val_A = AI_NewArbInt_FromString(s);
    printf("Assigned  %s,\nprints as 0x%s\n", s, AI_ToString(val_A));
  }

  {
    char *s = "0x123456789012345678901234";
    val_A = AI_NewArbInt_FromString(s);
    printf("Assigned  %s,\nprints as 0x%s\n", s, AI_ToString(val_A));
  }

  {
    char *s = "0x1234567890123456789012345678901234567";
    val_A = AI_NewArbInt_FromString(s);
    printf("Assigned  %s,\nprints as 0x%s\n", s, AI_ToString(val_A));
  }

  {
    char *s = "0x12345678901234567890123456789012345678901234567890123456789";
    val_A = AI_NewArbInt_FromString(s);
    printf("Assigned  %s,\nprints as 0x%s\n", s, AI_ToString(val_A));
  }

  printf("Sub tests\n");

  {
    char *s = "0x1000000000";
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x26");

    val_sub = AI_Sub(val_A, val_B);

    printf("0x%s - 0x%s = 0x%s\n", 
	   AI_ToString(val_A), 
	   AI_ToString(val_B), 
	   AI_ToString(val_sub));
    printf("Expected:             0xFFFFFFFDA\n");

    val_sub = AI_Sub(val_B, val_A);

    printf("0x%s - 0x%s = 0x%s\n", 
	   AI_ToString(val_B), 
	   AI_ToString(val_A), 
	   AI_ToString(val_sub));

  }

  printf("Mul tests\n");

  { // 1
    char *s = "0x107";
    printf("Mul test %d\n", mul_test++);
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x2");

    val_mul = AI_Mul(val_A, val_B);

    printf("0x%s * 0x%s = 0x%s\n", 
	   AI_ToString(val_A), 
	   AI_ToString(val_B), 
	   AI_ToString(val_mul));
    printf("Expected:     0x20E\n");
  }

  { // 2
    char *s = "0xB107";
    printf("Mul test %d\n", mul_test++);
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x2");

    val_mul = AI_Mul(val_A, val_B);

    printf("0x%s * 0x%s = 0x%s\n", 
	   AI_ToString(val_A), 
	   AI_ToString(val_B), 
	   AI_ToString(val_mul));
    printf("Expected:      0x1620E\n");
  }

  { // 3
    char *s = "0xB107";
    printf("Mul test %d\n", mul_test++);
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x2735");

    val_mul = AI_Mul(val_A, val_B);

    printf("0x%s * 0x%s = 0x%s\n", 
	   AI_ToString(val_A), 
	   AI_ToString(val_B), 
	   AI_ToString(val_mul));
    printf("Expected:         0x1B1CB773\n");
  }

  { // 4
    char *s = "0xB107";
    printf("Mul test %d\n", mul_test++);
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0xFF735");

    val_mul = AI_Mul(val_A, val_B);

    printf("0x%s * 0x%s = 0x%s\n", 
	   AI_ToString(val_A), 
	   AI_ToString(val_B), 
	   AI_ToString(val_mul));
    printf("Expected:          0xB0A5B6773\n");
  }

  { // 5
    char *s = "0x1010";
    printf("Mul test %d\n", mul_test++);
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0x10101010");

    val_mul = AI_Mul(val_A, val_B);

    printf("0x%s * 0x%s = 0x%s\n", 
	   AI_ToString(val_A), 
	   AI_ToString(val_B), 
	   AI_ToString(val_mul));
    printf("Expected:             0x10202020100\n");
  }

  { // 6
    char *s = "0xfb563ac9";
    printf("Mul test %d\n", mul_test++);
    val_A = AI_NewArbInt_FromString(s);
    val_B = AI_NewArbInt_FromString("0xc56a7def");

    val_mul = AI_Mul(val_A, val_B);

    printf("0x%s * 0x%s = 0x%s\n", 
	   AI_ToString(val_A), 
	   AI_ToString(val_B), 
	   AI_ToString(val_mul));
    printf("Expected:                 0xC1D1E894C46F06A7\n");
  }

  return 0;
}