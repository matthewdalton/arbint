/*
 * Arbint private header file
 */
#ifndef _ARBINT_PRIV_H
#define _ARBINT_PRIV_H

#include "arbint.h"

struct arbint
{
  unsigned long *data;
  unsigned long dataLen;
  int           sign;
};

/*
 * This should work on both 32 and 64 bit systems.
 */
#define AI_MAX_LENGTH ((size_t)-1) / sizeof(unsigned long) 

#endif /* _ARBINT_PRIV_H */
