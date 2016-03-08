/* -*- mode: c; c-basic-offset: 2; -*- */
#include "arbint.h"
#include "arbint-priv.h"
#include "AI_mem.h"

#include <string.h>

#define AI_VAL_TO_CHAR(val) \
  (((val) <= 9) ? ((val) + '0') : ((val) + 55))

static char const *nullstr = "(null)";

char const *AI_ToString(ArbInt const *value)
{
  if (value == NULL) return nullstr;

  size_t i;
  int zero_crop = 1;
  int len =
    2 * value->dataLen * sizeof(aibase_t) + 1 + (value->sign == 0 ? 0 : 1);
  char *str = (char *)AI_malloc(len);
  char *p = str;
  aibase_t *vp = value->data;
  int shift = 28;

  if (p == NULL) {
    return NULL;
  }

  if (value->sign == -1) {
    *p++ = '-';
  }

  while (AI_VAL_TO_CHAR((*vp >> shift) & 0x0000000F) == '0' && shift >= 0) {
    shift -= 4;
  }

  *p++ = '0';
  *p++ = 'x';

  for (i = 0; i < value->dataLen; i++) {
    while (shift >= 0) {
      *p++ = AI_VAL_TO_CHAR((*vp >> shift) & 0x0000000F);
      shift -= 4;
    }
    shift = 28;
    vp++;
  }
  *p = '\0';

  return str;
}

char const *AI_ToStringDec(ArbInt const *value)
{
  if (value == NULL) return nullstr;

  return AI_ToStringBase(value, 10, 9 * value->dataLen);
}

char const *ai_reverse_str(char *in_out) /* in-place */
{
  int len = strlen(in_out);
  char swap;
  int i;
  for (i = 0; i < len/2; i++) {
    swap = in_out[i];
    in_out[i] = in_out[len - i - 1];
    in_out[len - i - 1] = swap;
  }
  return in_out;
}

char const *AI_ToStringBase(ArbInt const *value, int base, int approxLen)
{
  if (base > 10 || base <= 0 || value == NULL) return nullstr;

  int sign = value->sign;
  char *str = AI_malloc(approxLen + 2);
  char *p = str;
  ArbInt *ai_base = AI_NewArbInt_FromLong(base);
  ArbInt *copy = AI_NewArbInt_FromCopy(value);
  copy->sign = 1;
  ArbInt *digit;
  while (AI_Greater(copy, ai_base)) {
    ArbInt *tmp = AI_Div(copy, ai_base, &digit);
    AI_FreeArbInt(copy);
    copy = tmp;
    *p++ = AI_VAL_TO_CHAR(digit->data[0]);
    if (approxLen-- == 0) {
      /* printf("String too long for approxLen!\n"); */
      return nullstr;
    }
    AI_FreeArbInt(digit);
  }
  *p++ = AI_VAL_TO_CHAR(copy->data[0]);
  if (sign == -1) *p++ = '-';
  return ai_reverse_str(str);
}
