#include <stdlib.h>
#include <string.h>
#include "AI_mem.h"

void *AI_malloc(size_t size)
{
  return malloc(size);
}


void AI_free(void *mem)
{
  free(mem);
}

void *AI_memcpy(void *dest, void const *src, size_t size)
{
  return memcpy(dest, src, size);
}

void *AI_memset(void *dest, int val, size_t len)
{
  return memset(dest, val, len);
}
