#include "arbint.h"
#include "arbint-priv.h"

int
ai_get_hsb_position(aibase_t val)
{
  int bit = 0;
  unsigned short val_short;
  unsigned char  val_byte;
  unsigned char  val_nibble;
  unsigned char  mask;

  if (val == 0) {
    return 0;
  }

  /*
   * There's a slightly better version of essentially the same
   * method in the Linux kernel.
   */
  if (val & 0xFFFF0000) {
    bit += 16;
    val_short = (unsigned short)(val >> 16);
  }
  else {
    val_short = (unsigned short)val;
  }

  if (val_short & 0xFF00) {
    bit += 8;
    val_byte = (unsigned char)(val_short >> 8);
  }
  else {
    val_byte = (unsigned char)val_short;
  }

  if (val_byte & 0xF0) {
    bit += 4;
    val_nibble = (val_byte >> 4);
  }
  else {
    val_nibble = val_byte;
  }

  bit += 4;
  mask = 0x08;
  while (mask && !(val_nibble & mask)) {
    bit -= 1;
    mask >>= 1;
  }

  return bit;
}

aibase_t arbint_get_hsb_position(ArbInt const *ai)
{
  int i;
  int base = (ai->dataLen - 1) * sizeof(aibase_t);
  for (i = 0; i < ai->dataLen; i++) {
    if (ai->data[i] != 0) {
      return base + ai_get_hsb_position(ai->data[i]);
    }
    base -= sizeof(aibase_t);
  }
  return 0;
}
