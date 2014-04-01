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
  unsigned long *data;
  unsigned long dataLen;
  /* unsigned long memLen; */
  int           sign;
};

/*
 * This should work on both 32 and 64 bit systems.
 */
#define AI_MAX_LENGTH ((size_t)-1) / sizeof(unsigned long) 

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))


#endif /* _ARBINT_PRIV_H */
