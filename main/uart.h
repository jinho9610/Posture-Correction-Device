#ifndef UART_H_
#define UART_H_

#include "macro.h"

extern int16_t accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z;

void uartMPU();
void UART_init();
void Putch(char);
char Getch();
void uart_int_transmit(int);
void puthex(uint8_t);
void PutString(char*);
void itoa(int, char*);

void uartMPU() // getDATA로 가져온 데이터 값 uart로 출력
{
  PutString("        Accel_x : ");
  uart_int_transmit(accel_x);
  PutString("        Accel_y : ");
  uart_int_transmit(accel_y);
  PutString("        Accel_z : ");
  uart_int_transmit(accel_z);
  PutString("        Gyro_x : ");
  uart_int_transmit(gyro_x);
  PutString("        Gyro_y : ");
  uart_int_transmit(gyro_y);
  PutString("        Gyro_z : ");
  uart_int_transmit(gyro_z);
  Putch('\n');
}

void UART_init()
{
  DIRC = 0x01; // C0를 출력 즉 송신으로 사용, C1은 수신

  UART1_RXDATA = 0x00;
  UART1_TXDATA = 0x00;
  UART1_STATUS = 0x20;

  UART1_CTRLA = 0x00;
  UART1_CTRLB = 0xC0;
  UART1_CTRLC = 0x03; // 8-bit data, none parity, 1 stop-bit

  UART1_BAUDH = 0x1A; // 0001 1010
  UART1_BAUDL = 0x0B; // 0000 1011 - 6667
}

void Putch(char data)
{
  // putty에서 엔터키 입력 시 Line Feed 까지 처리해준다
  if (data == '\r') Putch('\n');

  while (!(UART1_STATUS & 0x20)); // 송신할 준비가 되었는가
  UART1_TXDATA = data; // 데이터 보냄
}

char Getch()
{
  while (!(UART1_STATUS & 0x80)); // 수신할 준비가 되었는가
  return UART1_RXDATA; // 데이터 받음
}

void uart_int_transmit(int num)
{
  char temp1[100];
  itoa(num, temp1);
  char *temp2 = temp1;
  while (*temp2) Putch(*temp2++);
  //Putch('\n');
}

void puthex(uint8_t val)
{
  uint8_t upperNibble = (val & 0xF0) >> 4;
  uint8_t lowerNibble = val & 0x0F;

  upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
  lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';

  Putch(upperNibble);
  Putch(lowerNibble);
}

void PutString(char* s)
{
  while (*s) Putch(*s++);
}

void itoa(int n, char* buf)
{
  char    temp[10];
  int     rem, i = 0;
  int     sign = 0;
  unsigned int u = n;
  if (n == 0)
    temp[i++] = '0';

  if (n < 0)
  { // check sign
    sign = 1;
    u = -n;
  }

  while (u != 0)
  {
    rem = u % 10; 
    temp[i++] = rem + '0';
    u = u / 10;    
  }

  if (sign)                       // 음수
    *buf++ = '-';

  while (--i >= 0)             
    *buf++ = temp[i];

  *buf = '\0';            // end of string
}

#endif
