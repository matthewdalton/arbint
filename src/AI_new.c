#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "arbint.h"
#include "arbint-priv.h"
#include "AI_mem.h"

static
ArbInt *ai_new_empty(void);

static void
ai_assign_value(ArbInt *ai, aibase_t *value, unsigned long len, int sign);

#define AI_CHAR_TO_VAL(chr) \
  (((chr) <= '9') ? ((chr) - '0') : ((chr) - 'A' + 10))

/*
 * Initialise to 0
 */
ArbInt *AI_NewArbInt(void)
{
  return AI_NewArbInt_FromLong(0);
}

/*
 * Initialise to value held in string
 * Accepted formats:
 *   [-]+[0-9]*		base 10 decimal
 *   [-]+0x[0-9a-fA-F]*	base 16 decimal
 */
ArbInt *AI_NewArbInt_FromString(char const *value)
{
  ArbInt *aival;
  long len;
  int is_hex = 0;
  char const *p;
  aibase_t *d;
  int nibbles;
  char c;
  int padding;
  int sign = 1;

  if (value == NULL) {
    return NULL;
  }

  /*
   * Work out how much memory we need.
   * The number of characters, minus any '-' or '0x',
   * divided by 2 is a reasonable guess for now.
   */
  len = strlen(value);
  p = value;
  if (len >= 1 && *p == '-') {
    --len;
    ++p;
    sign = -1;
  }
  if (len >= 2 && (*p == '0' && *(p + 1) == 'x')) {
    len -= 2;
    is_hex = !0;
    p += 2;
  }


  if (is_hex) {
    /*
     * Hexadecimal conversion
     */

    aival = ai_new_empty();
    if (aival == NULL) {
      goto error_exit;
    }
    aival->data = NULL;

    aival->dataLen = (len + 7) / 8;

    nibbles = aival->dataLen * 8 - len;
    //nibbles = 0;

    aival->data = AI_malloc(aival->dataLen * sizeof(aibase_t));
    if (aival->data == NULL) {
      goto error_exit;
    }

    AI_memset(aival->data, 0, aival->dataLen * sizeof(aibase_t));

    /*
     * Assign the string, depending on format
     */
    aival->sign = sign;
    d = aival->data;

    while (*p != '\0') {
      c = *p;
      if (c >= 'a' && c <= 'f') {
	/*
	 * Convert to uppercase
	 */
	c -= 'a' - 'A';
      }
      if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
	/*
	 * Shift the previous values, if necessary
	 */
	if (nibbles > 0) {
	  *d = *d << 4;
	}
	*d |= (AI_CHAR_TO_VAL(c) & 0x0000000F);
	++nibbles;

	/*
	 * If full, move to the next position.
	 */
	if (nibbles > 7) {
	  ++d;
	  nibbles = 0;
	}
      }
      ++p;
    }
  }
  else {
    /*
     * Decimal conversion
     */

    aival = AI_NewArbInt();
    d = aival->data;

    if (*p != '\0' && *p >= '0' && *p <= '9') {
      d[aival->dataLen-1] = AI_CHAR_TO_VAL(*p);
      ++p;
    }
    while (*p != '\0' && *p >= '0' && *p <= '9') {
      ArbInt *tmp = AI_Mul_Value(aival, 10, 1);
      /* printf("Mul by 10 gives %s\n", AI_ToString(tmp)); */
      ArbInt *tmp2 = AI_Add_Value(tmp, AI_CHAR_TO_VAL(*p), aival->sign);
      /* printf("Adding %d gives %s\n", AI_CHAR_TO_VAL(*p), AI_ToString(tmp2)); */
      AI_FreeArbInt(aival);
      AI_FreeArbInt(tmp);
      aival = tmp2;
      ++p;
    }
  }

  return aival;

 error_exit:
  AI_FreeArbInt(aival);
}

/*
 * Initialise using various value types
 */
ArbInt *AI_NewArbInt_FromLong(int32_t value)
{
  int sign = 1;

  if (value < 0) {
    value = -value;
    sign = -1;
  }

  return AI_NewArbInt_FromValue((aibase_t)value, sign);
}

ArbInt *AI_NewArbInt_From32(int32_t value)
{
  return AI_NewArbInt_FromLong(value);
}

ArbInt *AI_NewArbInt_FromValue(aibase_t value, int sign)
{
  ArbInt *ai = ai_new_empty();

  if (ai != NULL) {
    aibase_t *lv = AI_malloc(sizeof(aibase_t));
    if (lv == NULL) {
      goto error_exit;
    }

    *lv = value;

    ai_assign_value(ai, lv, 1, sign);
  }

  return ai;

 error_exit:
  if (ai != NULL) {
    AI_free(ai);
  }
  return NULL;
}

ArbInt *AI_NewArbInt_FromCopy(ArbInt const *ai)
{
  ArbInt *ret = ai_new_empty();
  if (ret == NULL)
    return NULL;

  aibase_t *lv = AI_malloc(ai->dataLen * sizeof(aibase_t));
  if (lv == NULL) {
    AI_FreeArbInt(ret);
    return NULL;
  }

  AI_memcpy(lv, ai->data, ai->dataLen * sizeof(aibase_t));
  ai_assign_value(ret, lv, ai->dataLen, ai->sign);

  assert(ai->dataLen == ret->dataLen);
  assert(ai->sign == ai->sign);
  assert(memcmp(ai->data, ret->data, ai->dataLen * sizeof(aibase_t)) == 0);

  return ret;
}

void *AI_Normalise(ArbInt *val);

ArbInt *AI_NewArbInt_SetBit(aibase_t bit) /* 0-based */
{
  static const aibase_t bits_per_unit = sizeof(aibase_t) * 8;
  aibase_t num_units = (bit + bits_per_unit) / bits_per_unit;
  aibase_t high_pos = (bit % bits_per_unit);

  ArbInt *ret = ai_new_empty();
  if (ret == NULL)
    return NULL;

  aibase_t *lv = AI_malloc(num_units * sizeof(aibase_t));
  if (lv == NULL) {
    AI_FreeArbInt(ret);
    return NULL;
  }

  AI_memset(lv, 0, num_units * sizeof(aibase_t));

  lv[0] = 1 << high_pos;
  ai_assign_value(ret, lv, num_units, 1);

  ret = AI_Normalise(ret);

  return ret;
}

ArbInt *AI_Resize(ArbInt *val, unsigned long newsize)
{
  aibase_t *newdata;

  if (val == NULL) {
    return val;
  }

  if (newsize > AI_MAX_LENGTH) {
    newsize = AI_MAX_LENGTH;
  }

  if (newsize <= val->dataLen) {
    return val;
  }

  newdata = AI_malloc(newsize * sizeof(aibase_t));
  if (!newdata) {
    return NULL;
  }

  AI_memset(newdata, 0, newsize * sizeof(aibase_t));  

  if (val->data != NULL) {
    AI_memcpy(newdata + (newsize - val->dataLen),
              val->data,
              val->dataLen * sizeof(aibase_t));
    AI_free(val->data);
  }
  val->data = newdata;
  val->dataLen = newsize;

  return val;
}

void AI_FreeArbInt(ArbInt *aival)
{
  if (aival->data != NULL) {
    AI_free(aival->data);
  }
  if (aival != NULL) {
    AI_free(aival);
  }
}

void *AI_Normalise(ArbInt *val)
{
  unsigned long dataLen = val->dataLen;
  unsigned long i;
  int isZero = 1;
  for (i = 0; i < val->dataLen; i++) {
    if (val->data[i] != 0) {
      isZero = 0;
      break;
    }
  }
  if (i == 0 || (i == 1 && dataLen == 1 && isZero))
    return val;

  if (isZero) i--;              /* If isZero, i >= 2 at this point */

  aibase_t *orig = val->data;
  val->data += i;
  val->dataLen -= i;
  ArbInt *normalised = AI_NewArbInt_FromCopy(val);
  val->data -= i;
  val->dataLen += i;
  ai_assign_value(val, normalised->data, normalised->dataLen, normalised->sign);
  return val;
}

/*********************************************************************
 "Package local" functions
 *********************************************************************/


/*********************************************************************
 File-local functions (static)
 *********************************************************************/

static
ArbInt *ai_new_empty(void)
{
  ArbInt *ai = AI_malloc(sizeof(ArbInt));
  if (ai != NULL) {
    ai->data = NULL;
    ai->dataLen = 0;
    ai->sign = 1;
  }
  return ai;
}

static void
ai_assign_value(ArbInt *ai, aibase_t *value, unsigned long len, int sign)
{
  assert(value != NULL);
  assert(len > 0);
  assert(sign == -1 || sign == 1);

  if (ai->data != NULL) {
    AI_free(ai->data);
  }

  ai->data = value;
  ai->dataLen = len;
  ai->sign = sign;
}
