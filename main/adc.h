#ifndef ADC_H_
#define ADC_H_

#include "macro.h"

extern int flexSensor;

void ADC_init();
void getFlexSensor();

void ADC_init()
{
  ADC_CTRLA = 0x00; // ADC not enabled yet 10-bit resolution
  ADC_CTRLB = 0x00; // no accumulation
  ADC_CTRLC = 0x57; // Vref = Vdd
  ADC_MUXPOS = 0x00; // 포트 D, 0번 핀 사용
}

void getFlexSensor()
{
  ADC_CTRLA = 0x01; // ADC enabled
  ADC_COMMAND = 0x01; // conversion 시작

  while (!(ADC_INTFLAG & 0x01)); // Result Ready Flag 체크, ADC 완료 여부 체크
  flexSensor = (int)ADC_RESL + ((int)(ADC_RESH) << 8); // ADC 결과 값을 정수로 변환하여 저장
}

#endif
