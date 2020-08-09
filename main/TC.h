#ifndef TC_H_
#define TC_H_

#include "macro.h"

extern int system_Seconds;
extern int ticks;

void TCA_PWM_init();
void TCB0_init();
void RTC_init();
void Time_counts();

void TCA_PWM_init()
{
  PORTMUX_TCAROUTEA = 0x04; // PORTE로 PWM 제어
  TCA_CTRLA = 0x05; // 4분주
  TCA_CTRLB = 0x13; // COMP0 enable, single-slope 모드
  TCA_CTRLD = 0x00;
  TCA_CTRLESET = 0b00000010;
  TCA_INTCTRL = 0b00010000; // compare chaennel0 Interrupt Enable
  TCA_EVCTRL = 0b00000000;

  TCA_PERL = 0xFF; // TOP 255
  TCA_PERH = 0x00;

  TCA_CMP0L = 0x00;
  TCA_CMP0H = 0x00;
}

void TCB0_init()
{
  TCB0_CCMPL = 0xFF;
  TCB0_CCMPH = 0x00;
  TCB0_CTRLA = 0x05;
  TCB0_INTCTRL = 0X01;
}

void RTC_init()
{
  RTC_CTRLA = 0x01;
  RTC_CLKSEL = 0x00;
  RTC_PITCTRLA = 0x49;
}

void Time_counts()
{
  if ((RTC_PITINTFLAGS & 0x01) == 1)
  {
    ticks++; // 오버플로우 일어나면
    RTC_PITINTFLAGS = 0x01; // TCA_INTFLAGS 0-bit 초기화
  }

  if (ticks == 32)
  {
    system_Seconds++;
    ticks = 0;
  }
}

#endif
