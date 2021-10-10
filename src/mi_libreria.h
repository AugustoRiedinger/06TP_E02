#ifndef mi_libreria_H
#define mi_libreria_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stdio.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_usart.h"

//--------------------------------------------------------------
// Defines
//--------------------------------------------------------------
#define  TLCD_INIT_PAUSE  100000  // pause beim init (>=70000)
#define  TLCD_PAUSE        50000  // kleine Pause (>=20000)
#define  TLCD_CLK_PAUSE     1000  // pause f�r Clock-Impuls (>=500)
#define  TLCD_MAXX            16  // max x-Position (0...15)
#define  TLCD_MAXY             2  // max y-Position (0...1)
#define  Delay_Debouncing 100e3
#define	 BufferLength 	  20
#define  MaxDigCount 	  4095
#define  MaxMiliVoltRef	  3320
#define  Res32Bit		  32
#define  DAC_DHR12R2_ADDRESS   0x40007414


TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef  		TIM_OCInitStructure;
EXTI_InitTypeDef   		EXTI_InitStructure;
DAC_InitTypeDef 		DAC_InitStructure;

//--------------------------------------------------------------
// LCD Kommandos (siehe Datenblatt)
//--------------------------------------------------------------
#define  TLCD_CMD_INIT_DISPLAY  0x28   // 2 Zeilen Display, 5x7 Punkte
#define  TLCD_CMD_ENTRY_MODE    0x06   // Cursor increase, Display fix
#define  TLCD_CMD_DISP_M0       0x08   // Display=AUS, Cursor=Aus, Blinken=Aus
#define  TLCD_CMD_DISP_M1       0x0C   // Display=EIN, Cursor=AUS, Blinken=Aus
#define  TLCD_CMD_DISP_M2       0x0E   // Display=EIN, Cursor=EIN, Blinken=Aus
#define  TLCD_CMD_DISP_M3       0x0F   // Display=EIN, Cursor=EIN, Blinken=EIN
#define  TLCD_CMD_CLEAR         0x01   // loescht das Display

//--------------------------------------------------------------
// Liste aller Pins fur das Display
// (keine Nummer doppelt und von 0 beginnend)
//--------------------------------------------------------------
typedef enum
{
  TLCD_RS = 0,  // RS-Pin
  TLCD_E  = 1,  // E-Pin
  TLCD_D4 = 2,  // DB4-Pin
  TLCD_D5 = 3,  // DB5-Pin
  TLCD_D6 = 4,  // DB6-Pin
  TLCD_D7 = 5   // DB7-Pin
}TLCD_NAME_t;

#define  TLCD_ANZ   6 // Anzahl von TLCD_NAME_t

//--------------------------------------------------------------
// Display Modes
//--------------------------------------------------------------
typedef enum {
  TLCD_OFF = 0,    // Display=AUS, Cursor=Aus, Blinken=Aus
  TLCD_ON,        // Display=EIN, Cursor=Aus, Blinken=Aus
  TLCD_CURSOR,    // Display=EIN, Cursor=EIN, Blinken=Aus
  TLCD_BLINK      // Display=EIN, Cursor=EIN, Blinken=EIN
}TLCD_MODE_t;

//--------------------------------------------------------------
// Struktur eines Pins
//--------------------------------------------------------------
typedef struct {
  TLCD_NAME_t TLCD_NAME;   // Name
  GPIO_TypeDef* TLCD_PORT; // Port
  const uint16_t TLCD_PIN; // Pin
  const uint32_t TLCD_CLK; // Clock
  BitAction TLCD_INIT;     // Init
}LCD_2X16_t;

//--------------------------------------------------------------
// FUNCIONES GLOBALES
//--------------------------------------------------------------
void 	INIT_DI(GPIO_TypeDef*, uint32_t);
void 	INIT_DO(GPIO_TypeDef*, uint32_t);

void 	DELAY(volatile uint32_t);

int 	READ_DI(GPIO_TypeDef*, uint16_t);

void 	LED_ON(GPIO_TypeDef*, uint16_t, int);

void 	INIT_ADC(GPIO_TypeDef*, uint16_t);
int32_t READ_ADC(GPIO_TypeDef*, uint16_t);
int 	DAC_FUNC(uint32_t, int);

void	INIT_LCD_2x16(LCD_2X16_t*);
void	CLEAR_LCD_2x16(LCD_2X16_t*);
void	PRINT_LCD_2x16(LCD_2X16_t*, uint8_t, uint8_t, char*);

void 	INIT_SYSTICK(float);

void INIT_TIM4(GPIO_TypeDef*, uint16_t);
void SET_TIM4(uint16_t, uint32_t T, uint32_t, uint32_t);
void INIT_TIM1(GPIO_TypeDef*, uint16_t);
void SET_TIM1(uint16_t , uint32_t , uint32_t , uint32_t );
void INIT_TIM3(void);
void SET_TIM3(uint32_t, uint32_t);

void INIT_EXTINT(GPIO_TypeDef*, uint16_t);

void INIT_DAC_CONT(GPIO_TypeDef* , uint16_t);
void DAC_CONT(GPIO_TypeDef*, uint16_t, int16_t);
void INIT_DAC_SINE(GPIO_TypeDef*, uint16_t);
void DAC_SINE32BIT(GPIO_TypeDef*, uint16_t, const uint16_t * ,uint32_t );

void INIT_USART_RX(GPIO_TypeDef*, uint16_t, uint32_t);
void INIT_USART_TX(GPIO_TypeDef*, uint16_t, uint32_t);

#endif //mi_libreria_H
