#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "arbint.h"
#include "arbint-priv.h"
#include "AI_mem.h"

static
ArbInt *ai_new_empty(void);

static void
ai_assign_value(ArbInt *ai, unsigned long *value, unsigned long len, int sign);

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
  int len;
  int is_hex = 0;
  char const *p;
  unsigned long *d;
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

    aival->data = AI_malloc(aival->dataLen * sizeof(unsigned long));
    if (aival->data == NULL) {
      goto error_exit;
    }

    AI_memset(aival->data, 0, aival->dataLen * sizeof(unsigned long));

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
      printf("Mul by 10 gives %s\n", AI_ToString(tmp));
      ArbInt *tmp2 = AI_Add_Value(tmp, AI_CHAR_TO_VAL(*p), aival->sign);
      printf("Adding %d gives %s\n", AI_CHAR_TO_VAL(*p), AI_ToString(tmp2));
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
ArbInt *AI_NewArbInt_FromLong(long value)
{
  int sign = 1;

  if (value < 0) {
    value = -value;
    sign = -1;
  }

  return AI_NewArbInt_FromValue((unsigned long)value, sign);
}

ArbInt *AI_NewArbInt_FromULong(unsigned long value)
{
  return AI_NewArbInt_FromValue(value, 1);
}

ArbInt *AI_NewArbInt_FromValue(unsigned long value, int sign)
{
  ArbInt *ai = ai_new_empty();

  if (ai != NULL) {
    unsigned long *lv = AI_malloc(sizeof(unsigned long));
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

/*
 * Initialise to value held in a size_t, with a separate sign indicator
 */
ArbInt *AI_NewArbInt_FromSizeT(size_t value, int sign)
{
  /* unimplemented */
  return NULL;
}

ArbInt *AI_NewArbInt_FromCopy(ArbInt const *ai)
{
  ArbInt *ret = ai_new_empty();
  if (ret == NULL)
    return NULL;

  unsigned long *lv = AI_malloc(ai->dataLen);
  if (lv == NULL) {
    AI_FreeArbInt(ret);
  }

  memcpy(ai->data, lv, ai->dataLen);
  ai_assign_value(ret, lv, ai->dataLen, ai->sign);
  return ret;
}

#if 0
void AI_Resize(ArbInt *val, unsigned long newsize)
{
  unsigned long *newdata;

  if (val == NULL) {
    return;
  }

  if (newsize > AI_MAX_LENGTH) {
    newsize = AI_MAX_LENGTH;
  }

  if (newsize <= val->dataLen) {
    return;
  }

  newdata = AI_malloc(newsize * sizeof(unsigned long));

  if (newdata != NULL) {
    if (val->data != NULL) {
      AI_memcpy(newdata, val->data, val->dataLen * sizeof(unsigned long));
      AI_free(val->data);
    }
    val->data = newdata;
    val->dataLen = newsize;
  }
}
#else
void AI_Resize(ArbInt *val, unsigned long newsize)
{
  unsigned long *newdata;

  if (val == NULL) {
    return;
  }

  if (newsize > AI_MAX_LENGTH) {
    newsize = AI_MAX_LENGTH;
  }

  if (newsize <= val->dataLen) {
    return;
  }

  newdata = AI_malloc(newsize * sizeof(unsigned long));
  AI_memset(newdata, 0, newsize * sizeof(unsigned long));  

  if (newdata != NULL) {
    if (val->data != NULL) {
      AI_memcpy(newdata + (newsize - val->dataLen),
		val->data,
		val->dataLen * sizeof(unsigned long));
      AI_free(val->data);
    }
    val->data = newdata;
    val->dataLen = newsize;
  }
}
#endif
void AI_FreeArbInt(ArbInt *aival)
{
  if (aival->data != NULL) {
    AI_free(aival->data);
  }
  if (aival != NULL) {
    AI_free(aival);
  }
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
ai_assign_value(ArbInt *ai, unsigned long *value, unsigned long len, int sign)
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
