/********************************************************************************
  * @file    main.c
  * @author  G. Garcia & A. Riedinger
  * @version 0.1
  * @date    04-10-21
  * @brief   Envio y recepcion de datos a través del protocolo RS232 a una PC
  * 		 por un cable RS232-USB.

  * SALIDAS:
  	  *	LCD  	Conexion Estandar TPs
	  * RS232 	TX - PA2
	  * RS232 	RX - PA3
  * ENTRADAS:
  	  * UserBut PC13
********************************************************************************/

/*------------------------------------------------------------------------------
LIBRERIAS:
------------------------------------------------------------------------------*/
#include "mi_libreria.h"

/*------------------------------------------------------------------------------
DEFINICIONES:
------------------------------------------------------------------------------*/
/*Parámetros de configuración del TIM3 para refresco del LCD:*/
#define Freq 	 4
#define TimeBase 200e3

/*Pines del RX UART:*/
#define RX_Port	GPIOA
#define RX		GPIO_Pin_3

/*Pines del TX UART:*/
#define TX_Port	GPIOA
#define TX		GPIO_Pin_2

/*Velocidad de trabajo del UART:*/
#define BaudRate 9600

/*Maximos de palabra:*/
#define MaxDataBits 8

/*------------------------------------------------------------------------------
VARIABLES GLOBALES:
------------------------------------------------------------------------------*/
/*Definicion de los pines del LCD: */
LCD_2X16_t LCD_2X16[] = {
			// Name  , PORT ,   PIN      ,         CLOCK       ,   Init
			{ TLCD_RS, GPIOC, GPIO_Pin_10, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_E,  GPIOC, GPIO_Pin_11, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_D4, GPIOC, GPIO_Pin_12, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_D5, GPIOD, GPIO_Pin_2,  RCC_AHB1Periph_GPIOD, Bit_RESET },
			{ TLCD_D6, GPIOF, GPIO_Pin_6,  RCC_AHB1Periph_GPIOF, Bit_RESET },
			{ TLCD_D7, GPIOF, GPIO_Pin_7,  RCC_AHB1Periph_GPIOF, Bit_RESET }, };

/*Variable para almacenamiento de chars recibidos:*/
char 	CharRec[MaxDataBits];

/*Variable para concatenar los caracteres recibidos en una cadena:*/
char 	DataString[5][100];

/*Variable para contar el tiempo de lectura:*/
float 	OpTime = 0;

/*Variable para contar los caracteres:*/
int 	StringLength;

/*Contador de filas de DataString*/
uint32_t n = 0;

int main(void)
{
/*------------------------------------------------------------------------------
CONFIGURACION DEL MICRO:
------------------------------------------------------------------------------*/
	SystemInit();

	/*Inicializacion del DISPLAY LCD:*/
	INIT_LCD_2x16(LCD_2X16);

	/*Inicializacion del puerto serie RX y TX:*/
	INIT_USART_RX_TX(RX_Port, RX, TX_Port, TX, BaudRate);

	//Inicialización del TIM3 para refresco del LCD:
	INIT_TIM3();
	SET_TIM3(TimeBase, Freq);

/*------------------------------------------------------------------------------
BUCLE PRINCIPAL:
------------------------------------------------------------------------------*/
    while(1)
    {
		/*Dato recibido:*/
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) {
			/*Se guarda lo recibido en la varibale Data:*/
			CharRec[0] = USART_ReceiveData(USART2);

			if (strcmp(CharRec, "\n")==0)
				n++;
			else
				strcat(DataString[n], CharRec);


			/*Enviar datos:*/
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			{}
			USART_SendData(USART2, CharRec[0]);
		}

		/*Calculo de la cantidad de caracteres en la cadena:*/
		StringLength = strlen(DataString[n]);

		/*Calculo del tiempo de operacion:*/
		OpTime = (float) StringLength / BaudRate;
    }
}
/*------------------------------------------------------------------------------
INTERRUPCIONES:
------------------------------------------------------------------------------*/
/*Interrupción por agotamiento de cuenta del TIM3 cada 250mseg (4 Hz):*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

		/*Buffers para almacenamiento de datos:*/
		char BufferStringLength[1000*MaxDataBits];
		char BufferStringData[1000*MaxDataBits];
		char BufferOpTime[BufferLength];

		/*Refresco del LCD: */
		CLEAR_LCD_2x16(LCD_2X16);

		/*Copiar datos a los buffers para imprimir:*/
		sprintf(BufferStringData, "%s", DataString[n-1]);
		sprintf(BufferStringLength, "%d", StringLength);
		sprintf(BufferOpTime, "%.2f", OpTime);

		/*Mensaje para indicar el ultimo caracter leido:*/
		PRINT_LCD_2x16(LCD_2X16, 0, 0, "Var: ");
		PRINT_LCD_2x16(LCD_2X16, 5, 0, BufferStringData);

		/*Mensaje para indicar la cantidad de caracteres leidos:*/
		PRINT_LCD_2x16(LCD_2X16, 0, 1, "Cant:");
		PRINT_LCD_2x16(LCD_2X16, 5, 1, BufferStringLength);

		/*Mensaje para indicar el tiempo de operacion.*/
		PRINT_LCD_2x16(LCD_2X16, 10, 1, "T:");
		PRINT_LCD_2x16(LCD_2X16, 12, 1, BufferOpTime);
	}
}
