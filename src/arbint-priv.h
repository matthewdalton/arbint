/*
 * Arbint private header file
 */
#ifndef _ARBINT_PRIV_H
#define _ARBINT_PRIV_H

#include "arbint.h"

struct arbint
{
  /*
   * LSB is at the end of the array
   * ie. data[dataLen - 1]
   */
  aibase_t *data;
  unsigned long dataLen;
  /* aibase_t memLen; */
  int           sign;
};

/*
 * This should work on both 32 and 64 bit systems.
 */
#define AI_MAX_LENGTH ((size_t)-1) / sizeof(aibase_t) 

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifndef STATIC
#define STATIC static
#endif

#endif /* _ARBINT_PRIV_H */
