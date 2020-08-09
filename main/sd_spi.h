#ifndef SD_SPI_H_
#define SD_SPI_H_

#include "macro.h"

extern uint8_t res[5], RD_Buf[512], token, WR_buf[512];

void SPI_init();
uint8_t SPI_transfer(uint8_t);
uint8_t SD_init();
void SD_powerUpSeq();
void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_readRes1();
void SD_readRes3_7(uint8_t *res);
void SD_readOCR(uint8_t *res);
uint8_t SD_goIdleState();
void SD_sendIfCond(uint8_t *res);
uint8_t SD_sendApp();
uint8_t SD_sendOpCond();
uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token);
uint8_t SD_writeSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token);

void SPI_init()
{
  DDR_SPI |= (1 << CS) | (1 << MOSI) | (1 << SCK);

  // 얘는 꼭 필요한지는 모르겠네
  DDR_SPI |= (1 << MISO);

  SPI_CTRLA = 0x27;
}

uint8_t SPI_transfer(uint8_t data)
{
  SPI_DATA = data;

  while (!(SPI_INTFLAGS & 0x80));

  return SPI_DATA;
}

uint8_t SD_init()
{
  uint8_t res[5], cmdAttempts = 0;

  SD_powerUpSeq();

  while ((res[0] = SD_goIdleState()) != 0x01) // cmd0 보내고 idle상태아니라면
  {
    cmdAttempts++;
    if (cmdAttempts == 255) return SD_ERROR; // 10번이상 읽었는데도 아이들 상태가 아니면 오류
  }

  Delay(1);

  SD_sendIfCond(res); // cmd8보내기

  if (res[0] != 0x01) return SD_ERROR; // cmd8보내고 아이들 아니면 오류

  if (res[4] != 0xAA) return SD_ERROR; // 에코 결과가 170아니면 오류

  cmdAttempts = 0;
  do
  {
    if (cmdAttempts == 255) return SD_ERROR;

    res[0] = SD_sendApp(); // CMD55

    if (SD_R1_NO_ERROR(res[0])) res[0] = SD_sendOpCond(); // CMD55 이후로 IDLE이면 // ACMD41

    Delay(1);

    cmdAttempts++;
  }
  while (res[0] != 0x00);

  SD_readOCR(res);

  return SD_SUCCESS;
}

void SD_powerUpSeq()
{
  CS_DISABLE();

  Delay(1);

  for (uint8_t i = 0; i < 10; i++)
    SPI_transfer(0xFF);

  CS_DISABLE();
  SPI_transfer(0xFF);
}

void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  SPI_transfer(cmd | 0x40); // 0b01xx xxxx
  SPI_transfer((uint8_t)(arg >> 24));
  SPI_transfer((uint8_t)(arg >> 16));
  SPI_transfer((uint8_t)(arg >> 8));
  SPI_transfer((uint8_t)(arg));

  SPI_transfer(crc | 0x01); // 아마 7비트인 crc에 END BIT인 1까지 포함해서 보낸 느것 같아
}

uint8_t SD_readRes1()
{
  uint8_t i = 0, res1;

  while ((res1 = SPI_transfer(0xFF)) == 0xFF)
  {
    i++;

    if (i > 8) break;
  }

  return res1;
}

void SD_readRes3_7(uint8_t *res)
{
  res[0] = SD_readRes1();
  
  if (res[0] > 1) return;

  res[1] = SPI_transfer(0xFF);
  res[2] = SPI_transfer(0xFF);
  res[3] = SPI_transfer(0xFF);
  res[4] = SPI_transfer(0xFF);
}

void SD_readOCR(uint8_t *res)
{
  SPI_transfer(0xFF);
  CS_ENABLE();
  SPI_transfer(0xFF);

  SD_command(CMD58, CMD58_ARG, CMD58_CRC);

  SD_readRes3_7(res);

  SPI_transfer(0xFF);
  CS_DISABLE();
  SPI_transfer(0xFF);
}

uint8_t SD_goIdleState()
{
  SPI_transfer(0xFF);
  CS_ENABLE();
  SPI_transfer(0xFF);

  SD_command(CMD0, CMD0_ARG, CMD0_CRC);

  uint8_t res1 = SD_readRes1();

  SPI_transfer(0xFF);
  CS_DISABLE();
  SPI_transfer(0xFF);

  return res1;
}

void SD_sendIfCond(uint8_t *res)
{
  // 칩 선택
  SPI_transfer(0xFF);
  CS_ENABLE();
  SPI_transfer(0xFF);

  // send CMD8
  SD_command(CMD8, CMD8_ARG, CMD8_CRC);

  SD_readRes3_7(res);

  // 칩 해제
  SPI_transfer(0xFF);
  CS_DISABLE();
  SPI_transfer(0xFF);
}

uint8_t SD_sendApp()
{
  SPI_transfer(0xFF);
  CS_ENABLE();
  SPI_transfer(0xFF);

  SD_command(CMD55, CMD55_ARG, CMD55_CRC);

  uint8_t res1 = SD_readRes1();

  SPI_transfer(0xFF);
  CS_DISABLE();
  SPI_transfer(0xFF);

  return res1;
}

uint8_t SD_sendOpCond()
{
  SPI_transfer(0xFF);
  CS_ENABLE();
  SPI_transfer(0xFF);

  SD_command(ACMD41, ACMD41_ARG, ACMD41_CRC);

  uint8_t res1 = SD_readRes1();

  SPI_transfer(0xFF);
  CS_DISABLE();
  SPI_transfer(0xFF);

  return res1;
}

uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
  uint8_t res1, Read;
  uint16_t readAttempts;

  // 무의미 토큰
  *token = 0xFF;

  // 칩 선택
  SPI_transfer(0xFF);
  CS_ENABLE();
  SPI_transfer(0xFF);

  // send CMD17
  SD_command(CMD17, addr, CMD17_CRC);

  res1 = SD_readRes1();

  if (res1 != 0xFF)
  {
    readAttempts = 0;
    while (++readAttempts != SD_MAX_READ_ATTEMPTS)
      if ((Read = SPI_transfer(0xFF)) != 0xFF) break;

    if (Read == 0xFE)
    {
      for (uint16_t i = 0; i < 512; i++) *buf++ = SPI_transfer(0xFF);

      // CRC
      SPI_transfer(0xFF);
      SPI_transfer(0xFF);
    }

    *token = Read;
  }

  SPI_transfer(0xFF);
  CS_DISABLE();
  SPI_transfer(0xFF);

  return res1;
}

uint8_t SD_writeSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
  uint8_t res1, Read;
  uint16_t readAttempts;

  *token = 0xFF;

  SPI_transfer(0xFF);
  CS_ENABLE();
  SPI_transfer(0xFF);

  SD_command(CMD24, addr, CMD24_CRC);

  res1 = SD_readRes1();

  if (res1 == 0x00) // 오류 없이 카드가 ready 상태라면
  {
    SPI_transfer(0xFE); // 스타트 토큰

    for (uint16_t i = 0; i < 512; i++)
      SPI_transfer(buf[i]); // 데이터 전송

    readAttempts = 0;
    while (++readAttempts != 3907)
    {
      if ((Read = SPI_transfer(0xFF)) != 0xFF)
      {
        *token = 0xFF;
        break;
      }
    }

    if ((Read & 0x1F) == 0x05) // 0bxxx00101이라면
    {
      *token = 0x05;

      readAttempts = 0;
      while (SPI_transfer(0xFF) == 0x00)
      {
        if (++readAttempts == 3907)
        {
          *token = 0x00;
          break;
        }
      }
    }

    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
  }
}

#endif
