#ifndef Delay_H_
#define Delay_H_

#include "macro.h"

void Delay(unsigned int);

void Delay(unsigned int cnt)
{
  register unsigned int i, j;

  for (i = 0; i < 3 * cnt; i++)
  {
    for (j = 0; j < cnt; j++)
    {
      asm("NOP");
      asm("NOP");
      asm("NOP");
    }
  }
}

#endif
