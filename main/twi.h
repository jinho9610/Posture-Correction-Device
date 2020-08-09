#ifndef TWI_H_
#define TWI_H_

#include "macro.h"

extern int16_t accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z;

void TWI_init();
uint8_t TWI_Start(uint8_t);
void TWI_Read(uint8_t*, int8_t);
void TWI_Write(uint8_t);
void TWI_Stop();
void getDATA();

void TWI_init()
{
  TWI_MBAUD = 0x1F; // 보드레이트 설정
  TWI_MCTRLB |= 0x08; // flush 초기화z
  TWI_MSTATUS |= (0x80 | 0x40); // RIF WIF 클리어(초기화)
  TWI_MCTRLA = 0x03; // 스마트 모드, MASTER모드 ENABLE
  TWI_MSTATUS |= 0x01; // 버스 상태 IDLE로 만들어주기
}

uint8_t TWI_Start(uint8_t addr)
{
  for (;;)
  {
    if ((TWI_MSTATUS & 0x03) != 0x03) break;
  }
  if ((TWI_MSTATUS & 0x03) != 0x03) // BUS가 BUSY상태 아니면
  {
    TWI_MCTRLB &= ~(1 << 2); // FLUSH
    TWI_MADDR = addr; // SLA + R 또는 SLA + W

    if (addr & 1) // READ라면
    {
      while (!(TWI_MSTATUS & 0x80)); // RIF가 1이 될 때까지 대기
    }
    else // WRITE라면
    {
      while (!(TWI_MSTATUS & 0x40)); // WIF가 1이 될 때까지 대기
    }
    return TWI_MSTATUS;
  }
  else
    return TWI_MSTATUS;
}

uint8_t TWI_Read(char ACKorNACK)
{
  if ((TWI_MSTATUS & 0x03) == 0x02) // TWI가 버스 제어중이라면
  {
    while (!(TWI_MSTATUS & 0x80)); // RIF 1될 때까지 대기

    if (ACKorNACK)
      TWI_MCTRLB &= ~(1 << 2); // 마스터가 슬레이브한테 ACK 전송 즉, 지속적인 통신의미
    else
      TWI_MCTRLB |= 1 << 2; // 마스터가 슬레이브한테 NACK 전송 즉, 통신 종료 의미

    return TWI_MDATA;
  }
}

void TWI_Write(uint8_t data)
{

  while (!((TWI_MSTATUS & 0x40) | (TWI_MSTATUS & 0x10))); // WIF가 셋되거나, 슬레이브로부터 가장 최근에 받은 신호가 NACK라면

  TWI_MDATA = data;

}

void TWI_Stop()
{
  TWI_MCTRLB |= 0x03; // STOP 커맨드
}

void getDATA() // MPU 6050으로부터 가속도, 자이로 값 가져옴
{
  uint8_t accel_x_H, accel_x_L, accel_y_H, accel_y_L, accel_z_H, accel_z_L, temp_H, temp_L, gyro_x_H, gyro_x_L, gyro_y_H, gyro_y_L, gyro_z_H, gyro_z_L;
  TWI_Start((MPU6050_ADDR << 1) | WRITEBIT);
  TWI_Write(0x3B);
  TWI_Start((MPU6050_ADDR << 1) | READBIT);
  accel_x_H=TWI_Read(ACK);
  accel_x_L=TWI_Read(ACK);
  accel_y_H=TWI_Read(ACK);
  accel_y_L=TWI_Read(ACK);
  accel_z_H=TWI_Read(ACK);
  accel_z_L=TWI_Read(ACK);
  temp_H=TWI_Read(ACK);
  temp_L=TWI_Read(ACK);
  gyro_x_H=TWI_Read(ACK);
  gyro_x_L=TWI_Read(ACK);
  gyro_y_H=TWI_Read(ACK);
  gyro_y_L=TWI_Read(ACK);
  gyro_z_H=TWI_Read(ACK);
  gyro_z_L=TWI_Read(NACK);
  TWI_Stop();
  accel_x = (accel_x_L | (accel_x_H << 8));
  accel_y = (accel_y_L | (accel_y_H << 8));
  accel_z = (accel_z_L | (accel_z_H << 8));
  gyro_x = (gyro_x_L | (gyro_x_H << 8));
  gyro_y = (gyro_y_L | (gyro_y_H << 8));
  gyro_z = (gyro_z_L | (gyro_z_H << 8));
}
#endif
