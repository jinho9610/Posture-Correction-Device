#define MPU6050_ADDR 0x68 // GY521(MPU6050) 주소

#define READBIT 0x01 // SCL + R
#define WRITEBIT 0x00 // SCL + W
#define ACK 1
#define NACK 0
/*==============================TWI================================*/
#define TWI_CTRLA (*((volatile uint8_t*) 0x08A0))
#define TWI_MCTRLA (*((volatile uint8_t*) 0x08A3))
#define TWI_MCTRLB (*((volatile uint8_t*) 0x08A4))
#define TWI_MSTATUS (*((volatile uint8_t*) 0x08A5))
#define TWI_MBAUD (*((volatile uint8_t*) 0x08A6))
#define TWI_MADDR (*((volatile uint8_t*) 0x08A7))
#define TWI_MDATA (*((volatile uint8_t*) 0x08A8))
/*=====================================uart========================*/
#define UART1_RXDATA (*((volatile uint8_t*) 0x0820)) // 수신 데이터 
#define UART1_TXDATA (*((volatile uint8_t*) 0x0822)) // 송신 데이터
#define UART1_STATUS (*((volatile uint8_t*) 0x0824)) // 상태
#define UART1_CTRLA (*((volatile uint8_t*) 0x0825)) // Interrupt 
#define UART1_CTRLB (*((volatile uint8_t*) 0x0826)) // TX, RX enable
#define UART1_CTRLC (*((volatile uint8_t*) 0x0827)) // 모드
#define UART1_BAUDL (*((volatile uint8_t*) 0x0828)) // 전송 속도 L [7:0]
#define UART1_BAUDH (*((volatile uint8_t*) 0x0829)) // 전송 속도 H [15:8]
#define DIRC (*((volatile uint8_t*) 0x0440)) // 포트 C 입출력 설정
/*==============================PORT================================*/
#define DIRA (*((volatile unsigned char*) 0x0400)) // 포트 A 입, 출력 방향 설정
#define PORTA (*((volatile unsigned char*) 0x0404)) // 포트 A 출력 값
#define DIRE (*((volatile unsigned char*) 0x0480)) // 포트 E 입, 출력 방향 설정
#define PORTE (*((volatile unsigned char*) 0x0484)) // 포트 E 출력 값
#define DIRF (*((volatile unsigned char*) 0x04A0)) // 포트 F 입, 출력 방향 설정
#define PORTF (*((volatile unsigned char*) 0x04A4)) // 포트 F 출력 값
/*==============================TCA, TCB================================*/
#define TCA_CTRLA (*((volatile unsigned char*) 0x0A00)) // 몇 분주, TCA Enable
#define TCA_CTRLB (*((volatile unsigned char*) 0x0A01)) // 비교 채널 활성화 여부
#define TCA_CTRLC (*((volatile unsigned char*) 0x0A02))
#define TCA_CTRLD (*((volatile unsigned char*) 0x0A03))
#define TCA_CTRLESET (*((volatile unsigned char*) 0x0A05))
#define TCA_EVCTRL (*((volatile unsigned char*) 0x0A09)) // 모드
#define TCA_INTCTRL (*((volatile unsigned char*) 0x0A0A)) // 인터럽트 활성화 여부
#define TCA_INTFLAGS (*((volatile unsigned char*) 0x0A0B)) // 0-bit 오버플로우 체크
#define TCA_CNTL (*((volatile unsigned char*) 0x0A20)) // 카운터 [7:0]
#define TCA_CNTH (*((volatile unsigned char*) 0x0A21)) // 카운터 [15:8]
#define TCA_PERL (*((volatile unsigned char*) 0x0A26)) // TOP [7:0]
#define TCA_PERH (*((volatile unsigned char*) 0x0A27)) // TOP [15:8]
#define TCA_PERBUFL (*((volatile unsigned char*) 0x0A36)) // TOP [15:8]
#define TCA_PERBUFH (*((volatile unsigned char*) 0x0A37)) // TOP [15:8]
#define TCA_CTRLESET (*((volatile unsigned char*) 0x0A05)) // TOP [15:8]
#define TCA_CMP0L (*((volatile unsigned char*) 0x0A28)) // TOP [15:8]
#define TCA_CMP0H (*((volatile unsigned char*) 0x0A29)) // TOP [15:8]
#define PORTMUX_TCAROUTEA (*((volatile unsigned char*) 0x05E4))

#define TCB0_CTRLA (*((volatile unsigned char*) 0x0A80))
#define TCB0_INTCTRL (*((volatile unsigned char*) 0x0A85))
#define TCB0_INTFLAGS (*((volatile unsigned char*) 0x0A86))
#define TCB0_CNT (*((volatile unsigned char*) 0x0A8A))
#define TCB0_CCMPL (*((volatile unsigned char*) 0x0A8C))
#define TCB0_CCMPH (*((volatile unsigned char*) 0x0A8D))
/*==============================RTC================================*/
#define RTC_CTRLA (*((volatile uint8_t*) 0x0140))
#define RTC_CLKSEL (*((volatile uint8_t*) 0x0147))
#define RTC_PITCTRLA (*((volatile uint8_t*) 0x0150))
#define RTC_PITINTCTRL (*((volatile uint8_t*) 0x0152))
#define RTC_PITINTFLAGS (*((volatile uint8_t*) 0x0153))
#define RTC_CNT (*((volatile uint16_t*) 0x0158))
/*==============================ADC================================*/
#define ADC_CTRLA (*((volatile unsigned char*) 0x0600)) // resolution
#define ADC_CTRLB (*((volatile unsigned char*) 0x0601)) // accumulation
#define ADC_CTRLC (*((volatile unsigned char*) 0x0602))
#define ADC_CTRLD (*((volatile unsigned char*) 0x0603))
#define ADC_MUXPOS (*((volatile unsigned char*) 0x0606))
#define ADC_COMMAND (*((volatile unsigned char*) 0x0608))
#define ADC_INTCTRL (*((volatile unsigned char*) 0x060A))
#define ADC_INTFLAG (*((volatile unsigned char*) 0x060B))
#define ADC_RESL (*((volatile unsigned char*) 0x0610)) // RES[7:0]
#define ADC_RESH (*((volatile unsigned char*) 0x0611)) // RES[15:8]
/*==============================SPI & SD================================*/
#define SPI_CTRLA (*((volatile unsigned char*) 0x08C0)) // 0b0010 0101
#define SPI_CTRLB (*((volatile unsigned char*) 0x08C1)) // 0b0100 01(00)
#define SPI_INTFLAGS (*((volatile unsigned char*) 0x08C3)) // 노말모드이냐 버퍼모드이냐에 따라 다름
#define SPI_DATA (*((volatile unsigned char*) 0x08C4))

#define DDR_SPI DIRA
#define PORT_SPI PORTA
#define CS 7
#define SCK 6
#define MISO 5
#define MOSI 4

#define SD_SUCCESS  0
#define SD_ERROR    1

#define CMD0        0
#define CMD0_ARG    0x00000000
#define CMD0_CRC    0x95
#define CMD8        8
#define CMD8_ARG    0x000001AA // 0000001aa인가
#define CMD8_CRC    0x87 //(1000011 << 1)
#define CMD55       55
#define CMD55_ARG   0x00000000
#define CMD55_CRC   0x00
#define CMD58       58
#define CMD58_ARG   0x00000000
#define CMD58_CRC   0x00
#define ACMD41      41
#define ACMD41_ARG  0x40000000
#define ACMD41_CRC  0x00
#define CMD17                   17
#define CMD17_CRC               0x00
#define CMD24                   24
#define CMD24_CRC               0x00

#define CS_ENABLE() PORT_SPI &= ~(1 << CS)
#define CS_DISABLE() PORT_SPI |= (1 << CS)

#define SD_IN_IDLE_STATE    0x01
#define SD_READY            0x00
#define SD_R1_NO_ERROR(X)   X < 0x02

#define SD_ERROR                1
#define SD_SUCCESS              0
#define SD_MAX_READ_ATTEMPTS    1563
#define SD_MAX_WRITE_ATTEMPTS   3907

#define SD_START_TOKEN          0xFE
#define SD_ERROR_TOKEN          0x00
