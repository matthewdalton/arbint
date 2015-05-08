/* -*- mode: c; c-basic-offset: 2; -*- */
#include "arbint.h"
#include "arbint-priv.h"
#include "AI_mem.h"

#define AI_VAL_TO_CHAR(val) \
  (((val) <= 9) ? ((val) + '0') : ((val) + 55))

static char const *nullstr = "(null)";

char const *AI_ToString(ArbInt const *value)
{
  if (value == NULL) return nullstr;

  size_t i;
  int zero_crop = 1;
  int len =
    2 * value->dataLen * sizeof(unsigned long) + 1 + (value->sign == 0 ? 0 : 1);
  char *str = (char *)AI_malloc(len);
  char *p = str;
  unsigned long *vp = value->data;
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
