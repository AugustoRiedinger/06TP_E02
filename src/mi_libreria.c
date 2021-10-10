#include "mi_libreria.h"

/*------------------------------------------------------------------------------
DECLARACION DE FUNCIONES INTERNAS:
------------------------------------------------------------------------------*/
//General:
uint32_t FIND_CLOCK(GPIO_TypeDef* Port);

//ADC:
ADC_TypeDef* FIND_ADC_TYPE(GPIO_TypeDef* Port, uint32_t Pin);
uint32_t FIND_RCC_APB(ADC_TypeDef* ADCX);
uint8_t FIND_CHANNEL(GPIO_TypeDef* Port, uint32_t Pin);

//LCD:
void P_LCD_2x16_InitIO(LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Delay(volatile uint32_t nCount);
void P_LCD_2x16_InitSequenz(LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Clk(LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Cmd(uint8_t wert, LCD_2X16_t* LCD_2X16);
void P_LCD_2x16_Cursor(LCD_2X16_t* LCD_2X16, uint8_t x, uint8_t y);
void P_LCD_2x16_Data(uint8_t wert, LCD_2X16_t* LCD_2X16);

//TIM4:
uint8_t FIND_PINSOURCE(uint32_t Pin);

//EXTINT:
uint8_t FIND_EXTI_PORT_SOURCE(GPIO_TypeDef* Port);
uint8_t FIND_EXTI_PIN_SOURCE(uint32_t Pin);
uint32_t FIND_EXTI_LINE(uint32_t Pin);
uint32_t FIND_EXTI_HANDLER(uint32_t Pin);

//DAC:
uint32_t FIND_DAC_CHANNEL(GPIO_TypeDef* Port, uint32_t Pin);

/*****************************************************************************
INIT_DI:

	* @author	A. Riedinger.
	* @brief	Inicialiiza entradas digitales.
	* @returns	void
	* @param
		- Port	Puerto del pin a inicializar. Ej: GPIOX.
		- Clock Clock del pin a inicializar. Ej: RCC_AHB1Periph_GPIOX.
		- Pin	Pin a inicializar. Ej: GPIO_Pin_X
	* @ej
		- INIT_DI(GPIOX, RCC_AHB1Periph_GPIOX, GPIO_Pin_X);
******************************************************************************/
void INIT_DI(GPIO_TypeDef* Port, uint32_t Pin)
{
	//Estructura de configuracion:
	GPIO_InitTypeDef GPIO_InitStructure;

	//Habilitacion de la senal de reloj para el periferico:
	uint32_t Clock;
	Clock = FIND_CLOCK(Port);
	RCC_AHB1PeriphClockCmd(Clock, ENABLE);

	//Ahora se configura el pin como entrada (GPI0_MODE_IN):
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

	//Se aplica la configuracion definida anteriormente al puerto:
	GPIO_Init(Port, &GPIO_InitStructure);
}



/*****************************************************************************
INIT_DO

	* @author	A. Riedinger.
	* @brief	Inicialiiza salidas digitales.
	* @returns	void
	* @param
		- Port	Puerto del pin a inicializar. Ej: GPIOX.
		- Clock Clock del pin a inicializar. Ej: RCC_AHB1Periph_GPIOX.
		- Pin	Pin a inicializar. Ej: GPIO_Pin_X
	* @ej
		- INIT_DO(GPIOX, RCC_AHB1Periph_GPIOX, GPIO_Pin_X);
******************************************************************************/
void INIT_DO(GPIO_TypeDef* Port, uint32_t Pin)
{
	//Estructura de configuracion
	GPIO_InitTypeDef GPIO_InitStructure;

	//Habilitacion de la senal de reloj para el periferico:
	uint32_t Clock;
	Clock = FIND_CLOCK(Port);
	RCC_AHB1PeriphClockCmd(Clock, ENABLE);

	//Se configura el pin como entrada (GPI0_MODE_IN):
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

	//Se aplica la configuracion definida anteriormente al puerto:
	GPIO_Init(Port, &GPIO_InitStructure);
}



/*****************************************************************************
DELAY

	* @author	Catedra UTN-BHI TDII
	* @brief	Crea un retardo con un bucle.
	* @returns	void
	* @param
		- n		Cantidad de veces digitales equivalente a segundos.
	* @ej
		- DELAY(100e3)
******************************************************************************/
void DELAY(volatile uint32_t n)
{
  while(n--) {};
}



/*****************************************************************************
READ_DI

	* @author	Catedra UTN-BHI TDII
	* @brief	Lee el estado de un pulsador.
	* @returns
		- 1		SWITCH en estado ALTO.
		- 0 	SWITCH en estado BAJO.
	* @param
		- Port	Puerto del pin a leer. Ej: GPIOX.
		- Pin	Pin a leer. Ej: GPIO_Pin_X
	* @ej
		- READ_SWITCH(GPIOX, GPIO_Pin_X);
******************************************************************************/
int READ_DI(GPIO_TypeDef* Port, uint16_t Pin)
{
	DELAY(Delay_Debouncing);

	if(GPIO_ReadInputDataBit(Port, Pin))
		return 1;
	else
		return 0;
}



/*****************************************************************************
LED_ON

	* @author	A. Riedinger.
	* @brief	Prende un LED por un determinado tiempo y luego lo apaga.
	* @returns	void
	* @param
		- Port		Puerto del LED a prender. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X
		- ON_DELAY	Tiempo que el LED estará encendido.
	* @ej
		- LED_ON(GPIOX, GPIO_Pin_X, 100e3);
******************************************************************************/
void LED_ON(GPIO_TypeDef* Port, uint16_t LED, int ON_Delay)
{
	GPIO_SetBits(Port, LED);
	DELAY(ON_Delay);
	GPIO_ResetBits(Port, LED);
}



/*****************************************************************************
INIT_ADC

	* @author	Catedra UTN-BHI TDII / A. Riedinger.
	* @brief	Inicializa una entrada analogica como ADC.
	* @returns	void
	* @param
		- Port		Puerto del ADC a inicializar. Ej: GPIOX.
		- Clock		Clock del ADC a inicializar. Ej: RCC_AHB1Periph_GPIOX
		- ADCX		Nombre del ADC a inicializar. Ej: ADCX.
		- Pin		Pin del ADC a inicializar. Ej: GPIO_Pin_X
		- RCC_APB 	Activacion del puerto del ADC. Ej: RCC_APB2Periph_ADCX
		- Channel 	Canal del ADC a inicializar. Ej: ADC_Channel_1X
	* @ej
		- INIT_ADC(GPIOX, RCC_AHB1Periph_GPIOX, ADCX, GPIO_Pin_X, RCC_APB2Periph_ADCX, ADC_Channel_1X);
******************************************************************************/
void INIT_ADC(GPIO_TypeDef* Port, uint16_t Pin)
{
	uint32_t Clock;
	Clock = FIND_CLOCK(Port);

	ADC_TypeDef* ADCX;
	ADCX = FIND_ADC_TYPE(Port, Pin);

	uint32_t RCC_APB;
	RCC_APB = FIND_RCC_APB(ADCX);

	uint8_t Channel;
	Channel = FIND_CHANNEL(Port, Pin);

    GPIO_InitTypeDef        GPIO_InitStructure;
    ADC_InitTypeDef         ADC_InitStructure;
    ADC_CommonInitTypeDef   ADC_CommonInitStructure;

    //Habilitacion del Clock para el puerto donde esta conectado el ADC:
    RCC_AHB1PeriphClockCmd(Clock, ENABLE);

    //Configuracion del PIN del ADC como entrada ANALOGICA.
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     = Pin;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL ;
    GPIO_Init(Port, &GPIO_InitStructure);

    //Activar ADC:
    RCC_APB2PeriphClockCmd(RCC_APB, ENABLE);

    //ADC Common Init:
    ADC_CommonStructInit(&ADC_CommonInitStructure);
    ADC_CommonInitStructure.ADC_Mode                = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler           = ADC_Prescaler_Div4; // max 36 MHz segun datasheet
    ADC_CommonInitStructure.ADC_DMAAccessMode       = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay    = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    //ADC Init:
    ADC_StructInit (&ADC_InitStructure);
    ADC_InitStructure.ADC_Resolution             = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode           = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode     = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge   = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign              = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion        = 1;
    ADC_Init(ADCX, &ADC_InitStructure);

    //Establecer la configuración de conversion:
    ADC_InjectedSequencerLengthConfig(ADCX, 1);
    ADC_SetInjectedOffset(ADCX, ADC_InjectedChannel_1, 0);
    ADC_InjectedChannelConfig(ADCX, Channel, 1, ADC_SampleTime_480Cycles);

    /* Poner en marcha ADC ----------------------------------------------------*/
    ADC_Cmd(ADCX, ENABLE);
}



/*****************************************************************************
READ_ADC

	* @author	Catedra UTN-BHI TDII / A. Riedinger.
	* @brief	Prende un LED por un determinado tiempo y luego lo apaga.
	* @returns
		- ADC_DATA	Devuelve el valor DIGITAL de la lectura en el ADCX.
	* @param
		- ADCX		Nombre del ADC a inicializar. Ej: ADCX.
	* @ej
		- READ_ADC(ADCX);
******************************************************************************/
int32_t READ_ADC(GPIO_TypeDef* Port, uint16_t Pin)
{
    uint32_t ADC_DATA;

    ADC_TypeDef* ADCX;
    ADCX = FIND_ADC_TYPE(Port, Pin);

    ADC_ClearFlag(ADCX, ADC_FLAG_JEOC);
    ADC_SoftwareStartInjectedConv(ADCX);
    while (ADC_GetFlagStatus(ADCX, ADC_FLAG_JEOC) == RESET);

    ADC_DATA = ADC_GetInjectedConversionValue(ADCX, ADC_InjectedChannel_1);
    return ADC_DATA;
}



/*****************************************************************************
DAC_FUNC

	* @author	A. Riedinger.
	* @brief	Convierte un valor leido por el ADC a su equivalente analogico.
	* @returns
		- return	Devuelve el valor ANALOGICO de la lectura en el ADCX.
	* @param
		- ADC_DATA	Valor DIGITAL leido en el ADC. Ej: a = READ_ADC(ADCX);
		- MAX_AN 	Valor ANALOGICO maximo que puede tomar la variable.
	* @ej
		- DAC_FUNC(ValActualADC_CuentasDigitales, ValorMaximoAnalogico);
******************************************************************************/
int DAC_FUNC(uint32_t ADC_DATA, int MAX_AN)
{
	return (float) ADC_DATA * MAX_AN / MaxDigCount;
}



/*****************************************************************************
INIT_LCD_2x16

	* @author	A. Riedinger.
	* @brief	Inicializa los pines del display.
	* @returns	void
	* @param
		- LCD_2x16	Arreglo tipo LCD_2X16_t con los pines del LCD. Ej:
					LCD_2X16_t LCD_2X16[] = {
 	 	 	 	 	// Name  , PORT,    PIN     ,          CLOCK     , Init
  	  	  	  	  	{TLCD_RS ,GPIOC,GPIO_Pin_10 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_E  ,GPIOC,GPIO_Pin_11 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_D4 ,GPIOC,GPIO_Pin_12 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_D5 ,GPIOD,GPIO_Pin_2  ,RCC_AHB1Periph_GPIOD,Bit_RESET},
  	  	  	  	  	{TLCD_D6 ,GPIOF,GPIO_Pin_9  ,RCC_AHB1Periph_GPIOF,Bit_RESET},
  	  	  	  	  	{TLCD_D7 ,GPIOF,GPIO_Pin_7  ,RCC_AHB1Periph_GPIOF,Bit_RESET},};
	* @ej
		- INIT_LCD_2x16(LCD_2X16);
******************************************************************************/
void INIT_LCD_2x16(LCD_2X16_t* LCD_2X16)
{
	  //Inicialización de los pines del LCD:
	  P_LCD_2x16_InitIO(LCD_2X16);
	  // kleine Pause
	  P_LCD_2x16_Delay(TLCD_INIT_PAUSE);
	  // Init Sequenz starten
	  P_LCD_2x16_InitSequenz(LCD_2X16);
	  // LCD-Settings einstellen
	  P_LCD_2x16_Cmd(TLCD_CMD_INIT_DISPLAY, LCD_2X16);
	  P_LCD_2x16_Cmd(TLCD_CMD_ENTRY_MODE, LCD_2X16);
	  // Display einschalten
	  P_LCD_2x16_Cmd(TLCD_CMD_DISP_M1, LCD_2X16);
	  // Display l�schen
	  P_LCD_2x16_Cmd(TLCD_CMD_CLEAR, LCD_2X16);
	  // kleine Pause
	  P_LCD_2x16_Delay(TLCD_PAUSE);
}



/*****************************************************************************
CLEAR_LCD_2x16

	* @author	A. Riedinger.
	* @brief	Refresca la pantalla del LCD.
	* @returns	void
	* @param
		- LCD_2x16	Arreglo tipo LCD_2X16_t con los pines del LCD. Ej:
					LCD_2X16_t LCD_2X16[] = {
 	 	 	 	 	// Name  , PORT,    PIN     ,          CLOCK     , Init
  	  	  	  	  	{TLCD_RS ,GPIOC,GPIO_Pin_10 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_E  ,GPIOC,GPIO_Pin_11 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_D4 ,GPIOC,GPIO_Pin_12 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_D5 ,GPIOD,GPIO_Pin_2  ,RCC_AHB1Periph_GPIOD,Bit_RESET},
  	  	  	  	  	{TLCD_D6 ,GPIOF,GPIO_Pin_9  ,RCC_AHB1Periph_GPIOF,Bit_RESET},
  	  	  	  	  	{TLCD_D7 ,GPIOF,GPIO_Pin_7  ,RCC_AHB1Periph_GPIOF,Bit_RESET},};
	* @ej
		- UB_LCD_2x16_Clear(LCD_2X16);
******************************************************************************/
void CLEAR_LCD_2x16(LCD_2X16_t* LCD_2X16)
{
  // Display l�schen
  P_LCD_2x16_Cmd(TLCD_CMD_CLEAR, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_PAUSE);
}



/*****************************************************************************
PRINT_LCD_2x16

	* @author	A. Riedinger.
	* @brief	Imprime una string en el LCD.
	* @returns	void
	* @param
		- LCD_2x16	Arreglo tipo LCD_2X16_t con los pines del LCD. Ej:
					LCD_2X16_t LCD_2X16[] = {
 	 	 	 	 	// Name  , PORT,    PIN     ,          CLOCK     , Init
  	  	  	  	  	{TLCD_RS ,GPIOC,GPIO_Pin_10 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_E  ,GPIOC,GPIO_Pin_11 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_D4 ,GPIOC,GPIO_Pin_12 ,RCC_AHB1Periph_GPIOC,Bit_RESET},
  	  	  	  	  	{TLCD_D5 ,GPIOD,GPIO_Pin_2  ,RCC_AHB1Periph_GPIOD,Bit_RESET},
  	  	  	  	  	{TLCD_D6 ,GPIOF,GPIO_Pin_9  ,RCC_AHB1Periph_GPIOF,Bit_RESET},
  	  	  	  	  	{TLCD_D7 ,GPIOF,GPIO_Pin_7  ,RCC_AHB1Periph_GPIOF,Bit_RESET},};
  	  	- x			Indicador de fila. Ej: 0 ... 1.
  	  	- y 		Indicador de columna. Ej: 0 .. 16.
  	  	- ptr		Puntero a la string a imprimir.

	* @ej
		- PRINT_LCD_2x16(LCD_2X16, 0, 0, STR);
******************************************************************************/
void PRINT_LCD_2x16(LCD_2X16_t* LCD_2X16, uint8_t x, uint8_t y, char *ptr)
{
  // Cursor setzen
  P_LCD_2x16_Cursor(LCD_2X16,x,y);
  // kompletten String ausgeben
  while (*ptr != 0) {
    P_LCD_2x16_Data(*ptr, LCD_2X16);
    ptr++;
  }
}



/*****************************************************************************
INIT_SYSTICK

	* @author	A. Riedinger.
	* @brief	Inicializa la Interrupcion por tiempo definido por el usuario.
	* @returns	void
	* @param
		- seg	Tiempo en segundos a los que se realizara la interrupcion.

	* @ej
		- INIT_SYSTICK(1/1000); //Interrupcion cada 1 mseg.
******************************************************************************/
void INIT_SYSTICK(float div)
{
	SysTick_Config(SystemCoreClock * div);
	RCC_ClocksTypeDef Clocks_Values;
	RCC_GetClocksFreq(&Clocks_Values);
}



/*****************************************************************************
INIT_TIM4

	* @author	A. Riedinger.
	* @brief	Inicializa salidas como timers.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void INIT_TIM4(GPIO_TypeDef* Port, uint16_t Pin)
{
	  GPIO_InitTypeDef GPIO_InitStructure;


	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	  //Habilitacion de la senal de reloj para el periferico:
	  uint32_t Clock;
	  Clock = FIND_CLOCK(Port);
	  RCC_AHB1PeriphClockCmd(Clock, ENABLE);

	  /* GPIOC Configuration: TIM4 CH1 (PD12),CH2 (PD13),CH3 (PD14)CH4 (PD15) */
	  GPIO_InitStructure.GPIO_Pin = Pin;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	  GPIO_Init(Port, &GPIO_InitStructure);

	  //Definición de GPIO_PinSourceXX:
	  uint8_t PinSource;
	  PinSource = FIND_PINSOURCE(Pin);

	  /* Connect TIM4 pins to AF2 */
	  GPIO_PinAFConfig(Port, PinSource, GPIO_AF_TIM4);
}



/*****************************************************************************
SET_TIM4 [1]

	* @author	A. Riedinger.
	* @brief	Setea el TIM4 a una determinada frecuencia.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void SET_TIM4(uint16_t Pin, uint32_t TimeBase, uint32_t Freq, uint32_t DutyCycle)
{
	uint32_t DT_Value;
	uint16_t PrescalerValue = 0;
	uint16_t TIM_Period = 0;

	//Actualización de los valores del TIM4:
	SystemCoreClockUpdate();
	TIM_ARRPreloadConfig(TIM4, DISABLE);
	TIM_Cmd(TIM4, DISABLE);

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TimeBase) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = TimeBase / Freq - 1;
	TIM_Period = TimeBase / Freq - 1;

	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	//Configuración del Duty Cycle para cada pin:
	DT_Value = DutyCycle * (TIM_Period + 1) / 100;

	if (Pin == GPIO_Pin_12) {
		/* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = DT_Value;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC1Init(TIM4, &TIM_OCInitStructure);

		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	} else if (Pin == GPIO_Pin_13) {
		/* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = DT_Value;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC2Init(TIM4, &TIM_OCInitStructure);

		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	} else if (Pin == GPIO_Pin_14) {
		/* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = DT_Value;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC3Init(TIM4, &TIM_OCInitStructure);

		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	} else if (Pin == GPIO_Pin_15) {
		/* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = DT_Value;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC4Init(TIM4, &TIM_OCInitStructure);

		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}

	//Cargar valores al TIM4:
	TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}



/*****************************************************************************
INIT_TIM1

	* @author	A. Riedinger.
	* @brief	Inicializa salidas como timers.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void INIT_TIM1(GPIO_TypeDef* Port, uint16_t Pin)
{
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	  //Habilitacion de la senal de reloj para el periferico:
	  uint32_t Clock;
	  Clock = FIND_CLOCK(Port);
	  RCC_AHB1PeriphClockCmd(Clock, ENABLE);

	  /* GPIOC Configuration: TIM4 CH1 (PD12),CH2 (PD13),CH3 (PD14)CH4 (PD15) */
	  GPIO_InitStructure.GPIO_Pin = Pin;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	  GPIO_Init(Port, &GPIO_InitStructure);

	  //Definición de GPIO_PinSourceXX:
	  uint8_t PinSource;
	  PinSource = FIND_PINSOURCE(Pin);

	  /* Connect TIM4 pins to AF2 */
	  GPIO_PinAFConfig(Port, PinSource, GPIO_AF_TIM1);
	  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}



/*****************************************************************************
SET_TIM1
	* @author	A. Riedinger.
	* @brief	Setea el TIM1 a una determinada frecuencia.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void SET_TIM1(uint16_t Pin, uint32_t TimeBase, uint32_t Freq, uint32_t DutyCycle)
{
	uint32_t DT_Value;
	uint16_t PrescalerValue = 0;
	uint16_t TIM_Period = 0;

	//Actualización de los valores del TIM4:
	SystemCoreClockUpdate();
	TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_Cmd(TIM1, DISABLE);

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TimeBase) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = TimeBase / Freq - 1;
	TIM_Period = TimeBase / Freq - 1;

	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	//Configuración del Duty Cycle para cada pin:
	DT_Value = DutyCycle * (TIM_Period + 1) / 100;

	if (Pin == GPIO_Pin_9) {
		/* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = DT_Value;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC1Init(TIM1, &TIM_OCInitStructure);

		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	} else if (Pin == GPIO_Pin_11) {
		/* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = DT_Value;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC2Init(TIM1, &TIM_OCInitStructure);

		TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	} else if (Pin == GPIO_Pin_13) {
		/* PWM1 Mode configuration: Channel1 : para TIM4 es PD12 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = DT_Value;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC3Init(TIM1, &TIM_OCInitStructure);

		TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	}
	//Cargar valores al TIM4:
	TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}



/*****************************************************************************
INIT_TIM3

	* @author	A. Riedinger.
	* @brief	Inicializa las interrupciones del TIM3.
	* @returns	void
	* @param
	* @ej
		- INIT_TIM4();
******************************************************************************/
void INIT_TIM3()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}




/*****************************************************************************
SET_TIM3
	* @author	A. Riedinger.
	* @brief	Setea el TIM1 a una determinada frecuencia.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void SET_TIM3(uint32_t TimeBase, uint32_t Freq)
{
	uint16_t PrescalerValue = 0;

	//Actualización de los valores del TIM4:
	SystemCoreClockUpdate();
	TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
	TIM_Cmd(TIM3, DISABLE);

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / TimeBase) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = TimeBase / Freq - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* TIM Interrupts enable */
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}



/*****************************************************************************
INIT_EXTINT
	* @author	A. Riedinger.
	* @brief	Inicializa las interrupciones externas en una determinada linea.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void INIT_EXTINT(GPIO_TypeDef* Port, uint16_t Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	uint32_t Clock;
	Clock = FIND_CLOCK(Port);
	RCC_AHB1PeriphClockCmd(Clock, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_Init(Port, &GPIO_InitStructure);

	/* Connect EXTI Line to pin */
	SYSCFG_EXTILineConfig(FIND_EXTI_PORT_SOURCE(Port), FIND_EXTI_PIN_SOURCE(Pin));

	/* Configure EXTI Line0 */
	EXTI_InitStructure.EXTI_Line = FIND_EXTI_LINE(Pin);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = FIND_EXTI_HANDLER(Pin);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*****************************************************************************
INIT_DAC_CONT
	* @author	A. Riedinger.
	* @brief	Inicializa una salida como DAC de continua.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_DAC_CONT(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como DAC.
******************************************************************************/
void INIT_DAC_CONT(GPIO_TypeDef* Port, uint16_t Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	uint32_t Clock;
	Clock = FIND_CLOCK(Port);
	RCC_AHB1PeriphClockCmd(Clock, ENABLE);

	/* Configura el Pin como salida Analogica */
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(Port, &GPIO_InitStructure);

	/* DAC: activar clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* DAC configuracion canal */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(FIND_DAC_CHANNEL(Port,Pin), &DAC_InitStructure);
	//**************************************************

	//DAC ON Channel 1
	DAC_Cmd(FIND_DAC_CHANNEL(Port,Pin), ENABLE);
}

/*****************************************************************************
DAC_CONT
	* @author	A. Riedinger.
	* @brief	Genera una señal DAC de continua.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X
		- Value		El valor de continua a generar en milivolts.

	* @ej
		- INIT_DAC_CONT(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como DAC.
******************************************************************************/
void DAC_CONT(GPIO_TypeDef* Port, uint16_t Pin, int16_t MiliVolts)
{
	uint16_t Data;

	Data = (MiliVolts * MaxDigCount) / MaxMiliVoltRef;

	if(FIND_DAC_CHANNEL(Port,Pin) == DAC_Channel_1)
		DAC_SetChannel1Data(DAC_Align_12b_R, MiliVolts);
	else
		DAC_SetChannel2Data(DAC_Align_12b_R, Data);
}



/*****************************************************************************
INIT_DAC_SINE
	* @author	A. Riedinger.
	* @brief	Inicializa una salida como DAC como una onda seno.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_DAC_Sine(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como DAC.
******************************************************************************/
void INIT_DAC_SINE(GPIO_TypeDef* Port, uint16_t Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* DMA1 and DAC clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* Enable GPIO clock */
	uint32_t Clock;
	Clock = FIND_CLOCK(Port);
	RCC_AHB1PeriphClockCmd(Clock, ENABLE);

	/* Configura el Pin como salida Analogica */
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(Port, &GPIO_InitStructure);
}



/*****************************************************************************
DAC_SINE32BIT
	* @author	A. Riedinger.
	* @brief	Genera una onda seno de determinada frecuencia en una salida DAC.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X
		- SineWave  Arreglo con los valores de la onda seno.
		- Freq 		Frecuencia de la onda seno.

	* @ej
		- INIT_DAC_Sine(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como DAC.
******************************************************************************/
void DAC_SINE32BIT(GPIO_TypeDef* Port, uint16_t Pin, const uint16_t *SineWave ,uint32_t Freq)
{
	/*Configuración del TIM6 como base de tiempo: */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* TIM6 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	/*Configuracion de la frecuencia de operacion: */
	/*
	 * TIM6CLK = SystemCoreClock / 2 por definicion.
	 *
	 * Ej.: para 32 puntos en memoria
	 * 1/90x10^6 = 11.11 nseg x (280+1) = 3.122 useg x 32 muestras = 99,9 useg= periodo seno
	 * --> F[KHz]= 10.254 KHz; por lo tanto, con =280 de TIM_Period llego a 10 KHz por ej.
	 *
	 * Entonces, de forma generica a partir del ejemplo:
	 * (1/TIM6CLK)*(TIM_Perio+1) = 1 / Freq
	 * Despejando de lo anterior:
	 * TIM_Period = (TIM6CLK / Freq) - 1 es el valor que hay que poner a partir de una
	 * frecuencia definida.
	 *
	 * Reemplazando TIM6CLK por su definicion:
	 * TIM_Period = (SystemCoreClock/(2*Freq))-1
	*/
	TIM_TimeBaseStructure.TIM_Period = (SystemCoreClock/(2*Freq))-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	/* TIM6 TRGO selection */
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);

	/*Configuración del DMA para la generacion independiente de la senal: */
	DMA_InitTypeDef DMA_InitStructure;

	/* DAC channel Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	 /* DMA1_Stream6 channel7 configuration*/
	DMA_DeInit(DMA1_Stream6);
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) DAC_DHR12R2_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &SineWave;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = Res32Bit;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);

	/* Enable DMA1_Stream6 */
	DMA_Cmd(DMA1_Stream6, ENABLE);

	/* Enable DAC Channel */
	DAC_Cmd(FIND_DAC_CHANNEL(Port,Pin), ENABLE);

	/* Enable DMA for DAC Channel */
	DAC_DMACmd(FIND_DAC_CHANNEL(Port,Pin), ENABLE);
}


/*****************************************************************************
INIT_USART_TX
	* @author	A. Riedinger.
	* @brief	Inicializa un pin como TX USART.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void INIT_USART_TX(GPIO_TypeDef* Port, uint16_t Pin, uint32_t BaudRate)
{
	/* System Clocks Configuration:*/
	/* USART clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* GPIO clock enable */
	RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port), ENABLE);

	/*GPIO Configuration:*/
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Port, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(Port, FIND_PINSOURCE(Pin), GPIO_AF_USART2);

	/* USARTx configuration*/
	  /* USARTx configurada como sigue:
	        - BaudRate = 9600 baud
	        - Largo de palabra = 8 Bits
	        - Un Bit de stop
	        - Sin paridad
	        - COntrol de flujo por hardware deshabilitado (RTS and CTS signals)
	        - Recepcion y transmision habilitadas
	  */
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_InitStructure.USART_Mode = USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
}


/*****************************************************************************
INIT_USART_RX
	* @author	A. Riedinger.
	* @brief	Inicializa un pin como RX USART.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void INIT_USART_RX(GPIO_TypeDef* Port, uint16_t Pin, uint32_t BaudRate)
{
	/* System Clocks Configuration:*/
	/* USART3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* GPIO clock enable */
	RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port), ENABLE);

	/*GPIO Configuration:*/
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Port, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(Port, FIND_PINSOURCE(Pin), GPIO_AF_USART2);

	/* USARTx configuration*/
	  /* USARTx configurada como sigue:
	        - BaudRate = 9600 baud
	        - Largo de palabra = 8 Bits
	        - Un Bit de stop
	        - Sin paridad
	        - COntrol de flujo por hardware deshabilitado (RTS and CTS signals)
	        - Recepcion y transmision habilitadas
	  */
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_InitStructure.USART_Mode = USART_Mode_Rx;

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
}



/*****************************************************************************
INIT_USART_RX_TX
	* @author	A. Riedinger.
	* @brief	Inicializa dos pines como RX y TX.
	* @returns	void
	* @param
		- Port		Puerto del timer a inicializar. Ej: GPIOX.
		- Pin		Pin del LED. Ej: GPIO_Pin_X

	* @ej
		- INIT_TIM4(GPIOX, GPIO_Pin_X); //Inicialización del Pin PXXX como TIMER4.
******************************************************************************/
void INIT_USART_RX_TX(GPIO_TypeDef* Port1, uint16_t Pin1, GPIO_TypeDef* Port2, uint16_t Pin2, uint32_t BaudRate)
{
	/* System Clocks Configuration:*/
	/* USART clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* GPIO clock enable */
	RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port1), ENABLE);
	RCC_AHB1PeriphClockCmd(FIND_CLOCK(Port2), ENABLE);


	/*GPIO Configuration:*/
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = Pin1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Port1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = Pin2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Port2, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(Port1, FIND_PINSOURCE(Pin1), GPIO_AF_USART2);
	GPIO_PinAFConfig(Port2, FIND_PINSOURCE(Pin2), GPIO_AF_USART2);

	/* USARTx configuration*/
	  /* USARTx configurada como sigue:
	        - BaudRate = 9600 baud
	        - Largo de palabra = 8 Bits
	        - Un Bit de stop
	        - Sin paridad
	        - COntrol de flujo por hardware deshabilitado (RTS and CTS signals)
	        - Recepcion y transmision habilitadas
	  */
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
}


/*------------------------------------------------------------------------------
 FUNCIONES INTERNAS:
------------------------------------------------------------------------------*/
//General:
uint32_t FIND_CLOCK(GPIO_TypeDef* Port)
{
	uint32_t Clock;

	if		(Port == GPIOA) Clock = RCC_AHB1Periph_GPIOA;
	else if (Port == GPIOB) Clock = RCC_AHB1Periph_GPIOB;
	else if (Port == GPIOC) Clock = RCC_AHB1Periph_GPIOC;
	else if (Port == GPIOD) Clock = RCC_AHB1Periph_GPIOD;
	else if (Port == GPIOE) Clock = RCC_AHB1Periph_GPIOE;
	else if (Port == GPIOF) Clock = RCC_AHB1Periph_GPIOF;
	else if (Port == GPIOG) Clock = RCC_AHB1Periph_GPIOG;
	return Clock;
}



//ADC:
ADC_TypeDef* FIND_ADC_TYPE (GPIO_TypeDef* Port, uint32_t Pin)
{
	ADC_TypeDef* ADCX;

	if 		((Port == GPIOA && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1   || Pin == GPIO_Pin_2 ||
								Pin == GPIO_Pin_3 || Pin == GPIO_Pin_4   || Pin == GPIO_Pin_5 ||
								Pin == GPIO_Pin_6 || Pin == GPIO_Pin_7)) ||

			 (Port == GPIOB && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1)) ||

			 (Port == GPIOC && (Pin == GPIO_Pin_0 || Pin == GPIO_Pin_1   || Pin == GPIO_Pin_2 ||
					  	  	  	Pin == GPIO_Pin_3 || Pin == GPIO_Pin_4   || Pin == GPIO_Pin_5)))
		ADCX = ADC1;

	else if ((Port == GPIOF && (Pin == GPIO_Pin_3 || Pin == GPIO_Pin_4   || Pin == GPIO_Pin_5 ||
								Pin == GPIO_Pin_6 || Pin == GPIO_Pin_7   || Pin == GPIO_Pin_8 ||
								Pin == GPIO_Pin_9 || Pin == GPIO_Pin_10)))
		ADCX = ADC3;

	else
		ADCX = NULL;

	return ADCX;
}

uint32_t FIND_RCC_APB(ADC_TypeDef* ADCX)
{
	uint32_t RCC_APB;

	if 		(ADCX == ADC1) RCC_APB = RCC_APB2Periph_ADC1;
	else if (ADCX == ADC3) RCC_APB = RCC_APB2Periph_ADC3;
	else 				   RCC_APB = 0;

	return RCC_APB;
}

uint8_t FIND_CHANNEL(GPIO_TypeDef* Port, uint32_t Pin)
{
	uint8_t Channel;

	if 		(Port == GPIOA && Pin == GPIO_Pin_0)  Channel = ADC_Channel_0;  else if (Port == GPIOA && Pin == GPIO_Pin_1)  Channel = ADC_Channel_1;
	else if (Port == GPIOA && Pin == GPIO_Pin_2)  Channel = ADC_Channel_2;  else if (Port == GPIOA && Pin == GPIO_Pin_3)  Channel = ADC_Channel_3;
	else if (Port == GPIOA && Pin == GPIO_Pin_4)  Channel = ADC_Channel_4;  else if (Port == GPIOA && Pin == GPIO_Pin_5)  Channel = ADC_Channel_5;
	else if (Port == GPIOA && Pin == GPIO_Pin_6)  Channel = ADC_Channel_6;  else if (Port == GPIOA && Pin == GPIO_Pin_7)  Channel = ADC_Channel_7;
	else if (Port == GPIOB && Pin == GPIO_Pin_0)  Channel = ADC_Channel_8;  else if (Port == GPIOB && Pin == GPIO_Pin_1)  Channel = ADC_Channel_9;
	else if (Port == GPIOC && Pin == GPIO_Pin_0)  Channel = ADC_Channel_10; else if (Port == GPIOC && Pin == GPIO_Pin_1)  Channel = ADC_Channel_11;
	else if (Port == GPIOC && Pin == GPIO_Pin_2)  Channel = ADC_Channel_12; else if (Port == GPIOC && Pin == GPIO_Pin_3)  Channel = ADC_Channel_13;
	else if (Port == GPIOC && Pin == GPIO_Pin_4)  Channel = ADC_Channel_14;	else if (Port == GPIOC && Pin == GPIO_Pin_5)  Channel = ADC_Channel_15;
	else if (Port == GPIOF && Pin == GPIO_Pin_3)  Channel = ADC_Channel_9;	else if (Port == GPIOF && Pin == GPIO_Pin_4)  Channel = ADC_Channel_14;
	else if (Port == GPIOF && Pin == GPIO_Pin_5)  Channel = ADC_Channel_15;	else if (Port == GPIOF && Pin == GPIO_Pin_6)  Channel = ADC_Channel_4;
	else if (Port == GPIOF && Pin == GPIO_Pin_7)  Channel = ADC_Channel_5;	else if (Port == GPIOF && Pin == GPIO_Pin_8)  Channel = ADC_Channel_6;
	else if (Port == GPIOF && Pin == GPIO_Pin_9)  Channel = ADC_Channel_7;	else if (Port == GPIOF && Pin == GPIO_Pin_10) Channel = ADC_Channel_8;
	else 										  Channel = 0;

	return Channel;
}

//LCD:
void P_LCD_2x16_InitIO(LCD_2X16_t* LCD_2X16)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TLCD_NAME_t lcd_pin;

	for (lcd_pin = 0; lcd_pin < TLCD_ANZ; lcd_pin++)
	{
		//Habilitacion del Clock para cada PIN:
		RCC_AHB1PeriphClockCmd(LCD_2X16[lcd_pin].TLCD_CLK, ENABLE);

		//Configuracion como salidas digitales:
		GPIO_InitStructure.GPIO_Pin = LCD_2X16[lcd_pin].TLCD_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LCD_2X16[lcd_pin].TLCD_PORT, &GPIO_InitStructure);

		//Default Wert einstellen
		if(LCD_2X16[lcd_pin].TLCD_INIT == Bit_RESET)
			P_LCD_2x16_PinLo(lcd_pin, LCD_2X16);
		else
			P_LCD_2x16_PinHi(lcd_pin, LCD_2X16);
	}
}

void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16)
{
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRH = LCD_2X16[lcd_pin].TLCD_PIN;
}

void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin, LCD_2X16_t* LCD_2X16)
{
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRL = LCD_2X16[lcd_pin].TLCD_PIN;
}

void P_LCD_2x16_Delay(volatile uint32_t nCount)
{
  while(nCount--)
  {
  }
}

void P_LCD_2x16_InitSequenz(LCD_2X16_t* LCD_2X16)
{
  //Inicializacion de la secuencia:
  P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16);
  P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  // Erster Init Impuls
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // Zweiter Init Impuls
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // Dritter Init Impuls
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // LCD-Modus einstellen (4Bit-Mode)
  P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  P_LCD_2x16_Delay(TLCD_PAUSE);
}

void P_LCD_2x16_Clk(LCD_2X16_t* LCD_2X16)
{
  // Pin-E auf Hi
  P_LCD_2x16_PinHi(TLCD_E, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);
  // Pin-E auf Lo
  P_LCD_2x16_PinLo(TLCD_E, LCD_2X16);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);
}

void P_LCD_2x16_Cmd(uint8_t wert, LCD_2X16_t* LCD_2X16)
{
  // RS=Lo (Command)
  P_LCD_2x16_PinLo(TLCD_RS, LCD_2X16);
  // Hi-Nibble ausgeben
  if((wert&0x80)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x40)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x20)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x10)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  // Lo-Nibble ausgeben
  if((wert&0x08)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x04)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x02)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x01)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
}

void P_LCD_2x16_Cursor(LCD_2X16_t* LCD_2X16, uint8_t x, uint8_t y)
{
  uint8_t wert;

  if(x>=TLCD_MAXX) x=0;
  if(y>=TLCD_MAXY) y=0;

  wert=(y<<6);
  wert|=x;
  wert|=0x80;
  P_LCD_2x16_Cmd(wert,LCD_2X16);
}

void P_LCD_2x16_Data(uint8_t wert, LCD_2X16_t* LCD_2X16)
{
  // RS=Hi (Data)
  P_LCD_2x16_PinHi(TLCD_RS, LCD_2X16);
  // Hi-Nibble ausgeben
  if((wert&0x80)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x40)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x20)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x10)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
  // Lo-Nibble ausgeben
  if((wert&0x08)!=0) P_LCD_2x16_PinHi(TLCD_D7, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D7, LCD_2X16);
  if((wert&0x04)!=0) P_LCD_2x16_PinHi(TLCD_D6, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D6, LCD_2X16);
  if((wert&0x02)!=0) P_LCD_2x16_PinHi(TLCD_D5, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D5, LCD_2X16);
  if((wert&0x01)!=0) P_LCD_2x16_PinHi(TLCD_D4, LCD_2X16); else P_LCD_2x16_PinLo(TLCD_D4, LCD_2X16);
  P_LCD_2x16_Clk(LCD_2X16);
}

//Configuración del TIM4:
uint8_t FIND_PINSOURCE(uint32_t Pin)
{
	if     (Pin == GPIO_Pin_0)  return GPIO_PinSource0;
	else if(Pin == GPIO_Pin_1)  return GPIO_PinSource1;
	else if(Pin == GPIO_Pin_2)  return GPIO_PinSource2;
	else if(Pin == GPIO_Pin_3)  return GPIO_PinSource3;
	else if(Pin == GPIO_Pin_4)  return GPIO_PinSource4;
	else if(Pin == GPIO_Pin_5)  return GPIO_PinSource5;
	else if(Pin == GPIO_Pin_6)  return GPIO_PinSource6;
	else if(Pin == GPIO_Pin_7)  return GPIO_PinSource7;
	else if(Pin == GPIO_Pin_8)  return GPIO_PinSource8;
	else if(Pin == GPIO_Pin_9)  return GPIO_PinSource9;
	else if(Pin == GPIO_Pin_10) return GPIO_PinSource10;
	else if(Pin == GPIO_Pin_11) return GPIO_PinSource11;
	else if(Pin == GPIO_Pin_12) return GPIO_PinSource12;
	else if(Pin == GPIO_Pin_13) return GPIO_PinSource13;
	else if(Pin == GPIO_Pin_14) return GPIO_PinSource14;
	else if(Pin == GPIO_Pin_15) return GPIO_PinSource15;
	else
		return 0;
}

//Interrupcion externa:
uint8_t FIND_EXTI_PORT_SOURCE(GPIO_TypeDef* Port)
{
	if (Port == GPIOA)
		return EXTI_PortSourceGPIOA;
	else if (Port == GPIOB)
		return EXTI_PortSourceGPIOB;
	else if (Port == GPIOC)
		return EXTI_PortSourceGPIOC;
	else if (Port == GPIOD)
		return EXTI_PortSourceGPIOD;
	else if (Port == GPIOE)
		return EXTI_PortSourceGPIOE;
	else if (Port == GPIOF)
		return EXTI_PortSourceGPIOF;
	else
		return 0;
}

uint8_t FIND_EXTI_PIN_SOURCE(uint32_t Pin)
{
	if (Pin == GPIO_Pin_0)
		return EXTI_PinSource0;
	else if (Pin == GPIO_Pin_1)
		return EXTI_PinSource1;
	else if (Pin == GPIO_Pin_1)
		return EXTI_PinSource1;
	else if (Pin == GPIO_Pin_2)
		return EXTI_PinSource2;
	else if (Pin == GPIO_Pin_3)
		return EXTI_PinSource3;
	else if (Pin == GPIO_Pin_4)
		return EXTI_PinSource4;
	else if (Pin == GPIO_Pin_5)
		return EXTI_PinSource5;
	else if (Pin == GPIO_Pin_6)
		return EXTI_PinSource6;
	else if (Pin == GPIO_Pin_7)
		return EXTI_PinSource7;
	else if (Pin == GPIO_Pin_8)
		return EXTI_PinSource8;
	else if (Pin == GPIO_Pin_9)
		return EXTI_PinSource9;
	else if (Pin == GPIO_Pin_10)
		return EXTI_PinSource10;
	else if (Pin == GPIO_Pin_11)
		return EXTI_PinSource11;
	else if (Pin == GPIO_Pin_12)
		return EXTI_PinSource12;
	else if (Pin == GPIO_Pin_13)
		return EXTI_PinSource13;
	else if (Pin == GPIO_Pin_14)
		return EXTI_PinSource14;
	else
		return 0;
}

uint32_t FIND_EXTI_LINE(uint32_t Pin)
{
	if (Pin == GPIO_Pin_0)
		return EXTI_Line0;
	else if (Pin == GPIO_Pin_1)
		return EXTI_Line1;
	else if (Pin == GPIO_Pin_2)
		return EXTI_Line2;
	else if (Pin == GPIO_Pin_3)
		return EXTI_Line3;
	else if (Pin == GPIO_Pin_4)
		return EXTI_Line4;
	else if (Pin == GPIO_Pin_5)
		return EXTI_Line5;
	else if (Pin == GPIO_Pin_6)
		return EXTI_Line6;
	else if (Pin == GPIO_Pin_7)
		return EXTI_Line7;
	else if (Pin == GPIO_Pin_8)
		return EXTI_Line8;
	else if (Pin == GPIO_Pin_9)
		return EXTI_Line9;
	else if (Pin == GPIO_Pin_10)
		return EXTI_Line10;
	else if (Pin == GPIO_Pin_11)
		return EXTI_Line11;
	else if (Pin == GPIO_Pin_12)
		return EXTI_Line12;
	else if (Pin == GPIO_Pin_13)
		return EXTI_Line13;
	else if (Pin == GPIO_Pin_14)
		return EXTI_Line14;
	else if (Pin == GPIO_Pin_15)
		return EXTI_Line15;
	else
		return 0;
}

uint32_t FIND_EXTI_HANDLER(uint32_t Pin)
{
	if (Pin == GPIO_Pin_0)
		return EXTI0_IRQn;
	else if (Pin == GPIO_Pin_1)
		return EXTI1_IRQn;
	else if (Pin == GPIO_Pin_2)
			return EXTI2_IRQn;
	else if (Pin == GPIO_Pin_3)
			return EXTI3_IRQn;
	else if (Pin == GPIO_Pin_4)
			return EXTI4_IRQn;
	else if (Pin == GPIO_Pin_5 || Pin == GPIO_Pin_5 || Pin == GPIO_Pin_7 ||
			 Pin == GPIO_Pin_8 || Pin == GPIO_Pin_9)
			return EXTI9_5_IRQn;
	else if (Pin == GPIO_Pin_10 || Pin == GPIO_Pin_11 || Pin == GPIO_Pin_12 ||
			 Pin == GPIO_Pin_13 || Pin == GPIO_Pin_14 || Pin == GPIO_Pin_15)
			return EXTI15_10_IRQn;
	else 	return 0;
}

uint32_t FIND_DAC_CHANNEL(GPIO_TypeDef* Port, uint32_t Pin)
{
	if(Port == GPIOA && Pin == GPIO_Pin_5) return DAC_Channel_2;
	else return 0;
}
/*------------------------------------------------------------------------------
NOTAS
------------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------
[1]	 AYUDA::::::TIM4 Configuration: generate 4 PWM signals with 4 different duty cycles.

 	 La base de tiempo del TIM4 es APB1 y PCLK1 x 2 (ver STM32 CubeMX- Clock Config)
 	 Para HCLK = 180 MHz (system core clock) la base de tiempo es de: 90 MHz

 	 Para reducir la base de tiempo utilizamos el prescaler del timer

 	 PrescalerValue= (90 MHz / Base de tiempo del timer elegida) -1

 	 Luego TIM_Period es el periodo de la senal de salida del timer

 	 Si Base de tiempo = 200.000 Hz y TIM_Period = 399

 	 La se�al de salida del timer es:

 	 	 	 	 1
 	 f =  --------------- = 500Hz
 	 	  (399+1)/200.000

 	 Otro metodo:

 	 	 	 	 	 	 	 1
 	 f=	----------------------------------------------		Donde Tim4_Clock= PCLK1 x 2 (ver Clock_Config.XLS)
 	 	(Tim_Period+1) * (PrescalerValue+1)/Tim4_Clock		en este ejemplo es 90 MHz

 	 Note:
 	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
 	 Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
 	 function to update SystemCoreClock variable value. Otherwise, any configuration
 	 based on this variable will be incorrect.
 ----------------------------------------------------------------------- */
