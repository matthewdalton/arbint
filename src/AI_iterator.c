#include <assert.h>
#include <stdio.h>

#include "arbint.h"
#include "arbint-priv.h"
#include "AI_mem.h"

ArbInt_BitIterator *AI_Make_Bit_Iter(ArbInt const *ai)
{
  ArbInt_BitIterator *bi = AI_malloc(sizeof(ArbInt_BitIterator));
  if (bi != NULL) {
    bi->begin = ai->data + ai->dataLen - 1;
    bi->end = ai->data - 1;
    bi->byte = bi->begin;
    bi->bit = 0x01;
  }
  return bi;
}

int AI_Bit_Iter_Inc(ArbInt_BitIterator *bi)
{
  bi->bit <<= 1;
  if (bi->bit == 0) {
    bi->byte--;
    bi->bit = 0x01;
  }
  if (bi->byte > bi->end) {
    return 1;
  } else {
    bi->byte = bi->end;
    return 0;
  }
}

int AI_Bit_Iter_Get(ArbInt_BitIterator const *bi)
{
  return (*(bi->byte) & bi->bit) ? 1 : 0;
}

