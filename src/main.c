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

/*Variable a encontrar:*/
#define Text2Find "emf"

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

/*Matriz de strings, guarda cada variable leida en una fila distinta:*/
char 	DataString[15][15];

/*Variable para contar el tiempo de lectura:*/
float 	OpTime = 0;

/*Variable para contar los caracteres:*/
int 	StringLength = 0;

/*Contador de filas de DataString:*/
uint32_t Row = 0;

/*Contador de columnas de DataString:*/
uint32_t Col = 0;

/*Flag para indicar que la variable fue encontrada:*/
uint32_t FlagVarFound = 0;

/*Flag para indicar que se guardo el valor efectivo de la variable:*/
uint32_t FlagVarValue = 0;

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

	/*Inicialización del TIM3 para refresco del LCD:*/
	INIT_TIM3();
	SET_TIM3(TimeBase, Freq);

/*------------------------------------------------------------------------------
BUCLE PRINCIPAL:
------------------------------------------------------------------------------*/
    while(1)
    {
		/*Mientras se reciba un dato y no se haya encontrado la varible a buscar:*/
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET && FlagVarValue == 0){
			/*Se guarda lo recibido en la varibale Data:*/
			CharRec[0] = USART_ReceiveData(USART2);

			/*Si se llego al fin de linea y no se encontro la variable:*/
			if (!strcmp(CharRec, "\n") && FlagVarFound == 0 )
				/*Se sigue almacenando en la siguiente fila:*/
				Row++;
			/*Si se encontro la variable y se llego al fin de linea:*/
			else if (!strcmp(CharRec, "\n") && FlagVarFound == 1)
				/*Se pone en 1 el flag para no entrar al while:*/
				FlagVarValue = 1;
			/*Si lo que esta antes del '=' es lo mismo a la variable a encontrar:*/
			else if (!strcmp(CharRec, "=") && !strcmp(DataString[Row],Text2Find))
				/*Se pone en 1 el flag de variable encontrada:*/
				FlagVarFound = 1;
			/*Se concatena el caracter leido actual con los anteriores:*/
			else
				strcat(DataString[Row], CharRec);

			/*Calculo de la cantidad de caracteres en la cadena:*/
			StringLength++;
		}
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

		/*Mensaje para indicar el valor de la variable encontrada:*/
		PRINT_LCD_2x16(LCD_2X16, 0, 0, "Var: ");
		if (FlagVarFound == 1)
		{
			/*Calculo del tiempo de operacion:*/
			OpTime = (float) StringLength / BaudRate;

			/*Imprimir fila de DataString en el display LCD:*/
			sprintf(BufferStringData, "%s", DataString[Row]);
			PRINT_LCD_2x16(LCD_2X16, 5, 0, BufferStringData);

			/*Imprimir fila de DataString en la consola de la PC:*/
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			{}
			/*Si no se llego al final de la fila de DataString:*/
			if (DataString[Row][Col] != "\0") {
				/*Continuar imprimiendo datos en la PC:*/
				USART_SendData(USART2, DataString[Row][Col]);
				Col++;
			}
		}

		/*Mensaje para indicar la cantidad de caracteres leidos:*/
		sprintf(BufferStringLength, "%d", StringLength);
		PRINT_LCD_2x16(LCD_2X16, 0, 1, "Cant:");
		PRINT_LCD_2x16(LCD_2X16, 5, 1, BufferStringLength);

		/*Mensaje para indicar el tiempo de operacion:*/
		sprintf(BufferOpTime, "%.2f", OpTime);
		PRINT_LCD_2x16(LCD_2X16, 10, 1, "T:");
		PRINT_LCD_2x16(LCD_2X16, 12, 1, BufferOpTime);
	}
}
