/* -*- mode: c; c-basic-offset: 2; -*- */
#include "arbint.h"
#include "arbint-priv.h"
#include "AI_mem.h"

#include <stdio.h>
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

  return AI_ToStringBase(value, 1000000000, 10 * value->dataLen);
  /* return AI_ToStringBase(value, 10, 10 * value->dataLen); */
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

char const *AI_ToStringHex(ArbInt const *value)
{
  // TODO: overflow
  aibase_t approxLen = value->dataLen * sizeof(aibase_t) * 2;
  int sign = value->sign;
  char *str = AI_malloc(approxLen + 3);
  char *p = str;

  int i, j;
  char s[9];
  *p++ = '0';
  *p++ = 'x';
  sprintf(s, "%08X", value->data[0]);
  j = 0;
  while (s[j] == '0' && j < 8) {
    j++;
  }
  while (j < 8) {
    *p++ = s[j];
    j++;
  }
  for (i = 1; i < value->dataLen; i++) {
    sprintf(s, "%08X", value->data[i]);
    for (j = 0; j < 8; j++) {
      *p++ = s[j];
    }
  }
  return str;
}

char const *AI_ToStringBase(ArbInt const *value, int base, int approxLen)
{
  if (base > 10 || base <= 0 || base != 1000000000 || base != 16, value == NULL) return nullstr;

  int sign = value->sign;
  char *str = AI_malloc(approxLen + 2);
  char *p = str;
  ArbInt *ai_base = AI_NewArbInt_FromLong(base);
  ArbInt *copy = AI_NewArbInt_FromCopy(value);
  copy->sign = 1;
  ArbInt *digit;
  while (AI_Greater(copy, ai_base)) {
    int i;
    ArbInt *tmp = AI_Div(copy, ai_base, &digit);
    AI_FreeArbInt(copy);
    copy = tmp;
    if (base != 1000000000) {
      *p++ = AI_VAL_TO_CHAR(digit->data[0]);
      if (approxLen-- == 0) {
        /* String too long for approxLen */
        return nullstr;
      }
    } else {
      {
        char s[11];
        sprintf(s, "%09u", digit->data[0]);
        /* printf("Converted %u to %s\n", digit->data[0], s); */
        for (i = 0; i < 9; i++) {
          *p++ = s[8 - i];
        }
      }
      if (approxLen-- == 0) {
        /* String too long for approxLen */
        return nullstr;
      }
      if (approxLen-- == 0) {
        /* String too long for approxLen */
        return nullstr;
      }
      if (approxLen-- == 0) {
        /* String too long for approxLen */
        return nullstr;
      }
    }
    AI_FreeArbInt(digit);
  }
  if (base != 1000000000) {
    *p++ = AI_VAL_TO_CHAR(copy->data[0]);
  } else {
    {
      int i;
      char s[11];
      int prezero = 0;
      sprintf(s, "%09u", copy->data[0]);
      /* how many pre-zeros? */
      for (i = 0; i < 9; i++) {
        if (s[i] != '0') break;
        prezero++;
      }
      for (i = 0; i < 9 - prezero; i++) {
          *p++ = s[8 - i];
      }
    }
  }
  if (base == 16) { *p++ = 'x'; *p++ = '0'; }
  if (sign == -1) *p++ = '-';
  *p = '\0';
  return ai_reverse_str(str);
}
