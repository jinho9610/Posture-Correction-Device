#include "macro.h"
#include "uart.h"
#include "adc.h"
#include "TC.h"
#include "Delay.h"
#include "twi.h"
#include "sd_spi.h"
#include <math.h>

int16_t accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z; // 센서 값

volatile double angle_ax, angle_ay, angle_az;
volatile double angle_gx, angle_gy, angle_gz;

const double RADIAN_TO_DEGREE = 180 / 3.14159;
const double ALPHA = 0.96;
const double DEGREE_PER_SECOND = 32767 / 250;

double baseAX, baseAY, baseAZ, baseGX, baseGY, baseGZ;

int system_Seconds = 0;
int ticks = 0;
bool time_flag = false;

int flexSensor;
int id = 1;
int tmp1 = 0;
int tmp2 = 0;
bool sd_flag = false;
double roll = 0, last_roll;
unsigned long now = 0, past = 0;
double dt = 0;

uint8_t res[5], RD_Buf[512], token, WR_buf[512];
uint32_t SD_blockAddr = 0x00000200;
uint16_t startPos;

int start_index = 0;
double rollAVG = 0;

void getDT();
void calibrateSensor();
void getBaseAngle();
void getLogs();

void setup() {
  UART_init(); // UART 초기화
  TWI_init(); // TWI 초기화
  TCA_PWM_init();
  ADC_init();
  SPI_init();
  RTC_init();

  TWI_Start((MPU6050_ADDR << 1) | WRITEBIT);
  TWI_Write(0x6B);
  TWI_Write(0x00);
  TWI_Stop();

  DIRF = 0x04;
  PORTF = 0x00;
  DIRE = 0x01;
  PORTE = 0x00;

  for (;;)
  {
    int a = SD_init();
    if (a == SD_SUCCESS)
    {
      PutString("sd카드 초기화 성공\n");
      PORTF = 0x04; // sd카드 인식용 led
      break;
    }
    else PutString("sd카드 초기화 실패\n");
  }

  // getLogs();

  while (1)
  {
    SD_readSingleBlock(SD_blockAddr, RD_Buf, &token); // 0x00000200부터 탐색
    if (RD_Buf[0] != 0) SD_blockAddr++; // 이전까지 기록 중에 뭐라도 적혀있다면 다음 페이지부터 기록
    else break;
  }
  
  calibrateSensor();
  getBaseAngle();
  past = millis();
}

void loop() {
  getDATA();
  getDT();

  last_roll = roll + rollAVG;

  angle_gx = (gyro_x - baseGX) / DEGREE_PER_SECOND * dt + last_roll;
  angle_ax = atan(accel_y / sqrt(pow(accel_x, 2) + pow(accel_z, 2))) * RADIAN_TO_DEGREE;

  roll = ALPHA * angle_gx + (1.000 - ALPHA) * angle_ax - rollAVG;
  getFlexSensor();

  if ((roll < -5 || roll > 5) || flexSensor < 650)
  {
    RTC_CNT = 0x00;
    Time_counts(); // 몸이 기운 시점부터 시간 재기 시작
    if (system_Seconds >= 3) // 3초를 넘기면
    {
      if (system_Seconds == 3) // 3가 되는 시점의 구부림, 각도 임시 저장
      {
        tmp1 = roll;
        tmp2 = flexSensor;
        sd_flag = true;
      }
      PORTE |= 1 << 0; // 알림
      if (roll > 0)
      {
        if (system_Seconds < 10)
        { if (flexSensor < 650 && flexSensor >= 400) TCA_CMP0L = max(129 + (650 - flexSensor) / 2, 255 - (9 - roll / 10) * 14);
          else if (flexSensor > 650) TCA_CMP0L = 255 - (9 - roll / 10) * 14;
          else if (flexSensor < 400) TCA_CMP0L = 0xFF;
          TCA_CMP0H = 0x00;
        }
        else
        {
          TCA_CMP0L = 0xFF;
          TCA_CMP0H = 0x00;
        } 
      }
      else
      {
        if (system_Seconds < 10)
        { if (flexSensor < 650 && flexSensor >= 400) TCA_CMP0L = max(129 + (650 - flexSensor) / 2, 255 - (9 + roll / 10) * 14);
          else if (flexSensor > 650) TCA_CMP0L = 255 - (9 + roll / 10) * 14;
          else if (flexSensor < 400) TCA_CMP0L = 0xFF;
          TCA_CMP0H = 0x00;
        }
        else
        {
          TCA_CMP0L = 0xFF;
          TCA_CMP0H = 0x00;
        }
      }
    }
  }
  else // 자세 기울기가 제 상태로 돌아간다면
  {
    RTC_CNT = 0x00;
    if (sd_flag && system_Seconds >= 2)
    {
      makeBufData(id++, tmp1, tmp2, system_Seconds);
      SD_writeSingleBlock(SD_blockAddr, WR_buf, &token);
      if (id != 1 && id % 64 == 1) SD_blockAddr++; // 한 페이지(블록) 꽉 채우면 다음 페이지로 이동
    }
    PORTE &= ~(1 << 0); // 알림 중단
    TCA_CMP0L = 0x00;
    TCA_CMP0H = 0x00;
    system_Seconds = 0;
    ticks = 0;

    sd_flag = false;
  }

  //  //PutString("roll : ");
  //uart_int_transmit(angle_ax);
  //Putch(' ');
  //uart_int_transmit(roll);
  //Putch(' ');
  //uart_int_transmit(rollAVG);
  //  //PutString("\tpitch : ");
    //Putch(' ');
  //  //uart_int_transmit(angle_ay);
  //  //PutString("second :  ");
  //Putch(' ');
  //uart_int_transmit(system_Seconds);
  //Putch(' ');
  //  //  //Delay(200);*/
  //  //Putch('\n');
  //  //PutString("flexSensor : ");
  //uart_int_transmit(flexSensor);
  //  //Putch('\n');
  //Putch(' ');
  //uart_int_transmit(TCA_CMP0L);
  //  uart_int_transmit(id);
  //Putch('\n');
}

void getDT()
{
  now = millis();
  dt = (now - past) / 1000.0;
  past = now;
}

void makeBufData(int id, int angle_ax, int flexSensor, int seconds)
{
  start_index = 8 * ((id - 1) % 64);
  for (uint16_t i = 0; i < 512; i++)
  {
    if (i == start_index)
      WR_buf[i] = id;
    else if (i == start_index + 1)
    {
      if (angle_ax < 0)
        WR_buf[i] = 0;
      else
        WR_buf[i] = 1;
    }
    else if (i == start_index + 2)
    {
      if (angle_ax < 0)
        WR_buf[i] = -angle_ax;
      else
        WR_buf[i] = angle_ax;
    }
    else if (i == start_index + 3)
      WR_buf[i] = flexSensor / 100;
    else if (i == start_index + 4)
      WR_buf[i] = flexSensor - WR_buf[i - 1] * 100;
    else if (i == start_index + 5)
      WR_buf[i] = seconds;
    else if (i > start_index + 5)
      WR_buf[i] = 0;
  }
}

uint16_t getNextPos(uint8_t* RD_Buf)
{
  for (uint16_t i = 0; i < 512; i += 8)
  {
    if (RD_Buf[i] == 0) return i;
    else continue;
  }
}

void calibrateSensor()
{
  double sumAX = 0, sumAY = 0, sumAZ = 0, sumGX = 0, sumGY = 0, sumGZ = 0;
  getDATA();
  for (int i = 0; i < 10; i++)
  {
    getDATA();
    sumAX += accel_x; sumAY = accel_y; sumAZ = accel_z;
    sumGX += gyro_x; sumGY = gyro_y; sumGZ = gyro_z;
    Delay(100);
  }
  baseAX = sumAX / 10;
  baseAY = sumAY / 10;
  baseAZ = sumAZ / 10;
  baseGX = sumGX / 10;
  baseGY = sumGY / 10;
  baseGZ = sumGZ / 10;
}

void getBaseAngle()
{
  for (int i = 0; i < 200; i++)
  {
    getDATA();
    getDT();

    last_roll = roll;

    angle_gx = (gyro_x - baseGX) / DEGREE_PER_SECOND * dt + last_roll;
    angle_ax = atan(accel_y / sqrt(pow(accel_x, 2) + pow(accel_z, 2))) * RADIAN_TO_DEGREE;

    roll = ALPHA * angle_gx + (1.000 - ALPHA) * angle_ax;
    if (i >= 100) rollAVG += roll;
  }
  rollAVG /= 100;
}

void getLogs()
{
  for (int j = 0; j < 10; j++)
  {
    res[0] = SD_readSingleBlock(SD_blockAddr++, RD_Buf, &token);
    PutString("\n===0x0000020");
    uart_int_transmit(j);
    PutString("===");
    PutString("\nID RL AG Fl.S SEC");
    if (res[0] < 0x02)
    {
      for (uint16_t i = 0; i < 120; i++)
      {
        if (i % 8 == 0) Putch('\n');
        if ((int)RD_Buf[i] < 10)
        {
          uart_int_transmit(0);
          uart_int_transmit(RD_Buf[i]);
        }
        else uart_int_transmit(RD_Buf[i]);
        Putch(' ');
      }
    }
  }
  SD_blockAddr=0x00000200;
}
