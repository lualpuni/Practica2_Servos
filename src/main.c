#include "stm32F10x.h"
#include "STM32vldiscovery.h"


TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
TIM_OCInitTypeDef  TIM_OCInitStruct;

//Valor mínimo 17, valor máximo 166
//uint16_t DutyC1 = 90; //Valor medio
//uint16_t DutyC2 = 90;

uint16_t DutyC1 = 17; //Valor mínimo
uint16_t DutyC2 = 17;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void EnableTimerInterrupt(void);

void RCC_Configuration(void)
 {
	// Habilitamos el reloj en los GPIOs
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
	                       RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	 // Habilitamos el reloj en el Timer3
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );
 }

void GPIO_Configuration(void)
 {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure); // Reset init structure

	//PINES SALIDA
	// Setup Blue LED on STM32-Discovery Board to use PWM.
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;            // Alt Function - Push Pull
	GPIO_Init( GPIOC, &GPIO_InitStructure );
	GPIO_PinRemapConfig( GPIO_FullRemap_TIM3, ENABLE );        // Map TIM3_CH3 to GPIOC.Pin8 and TIM3_CH4 to GPIOC.Pin9

	//PINES DE ENTRADA
	// GPIO_Pin_0: button, GPIO_Pin_1: left, GPIO_Pin_2: right, GPIO_Pin_3: up; GPIO_Pin_4: down
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; //Botón
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	//select input pull-up
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//highest speed available
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 }

void TIMER_Configuration(void)
 {

    TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );

	//TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_Period = 665;   // 0..999
    TIM_TimeBaseInitStruct.TIM_Prescaler = 715; // Div 240
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseInitStruct );
 }

//configuras la interrupción para el Timer 3
//Cada vez que se genera el evento, se detiene el contador de programa y se ejecuta la función TIM3_IRQHandler
void EnableTimerInterrupt()
  {
      NVIC_InitTypeDef nvicStructure;
      nvicStructure.NVIC_IRQChannel = TIM3_IRQn; // le decimos que el evento lo genere en el Timer 3
      nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
      nvicStructure.NVIC_IRQChannelSubPriority = 1;
      nvicStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&nvicStructure);
  }

//En cada periodo del PWM, se crea un evento y es cuando se ejecuta esta función
void TIM3_IRQHandler(){
	 //TIM_OCInitTypeDef TIM_OCInitStruct;
	 if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	 {
	    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

	    //Left Button
	    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0){
	    	if (DutyC1 < 17+5) {
	    		DutyC1 = 17;
	    	}
	    	else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0){
	    		DutyC1 = DutyC1 - 10;
	    	}
	    	else{
	    		DutyC1 = DutyC1 - 5;
	    	}
	    }

	    //Right Button
	    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0){
	    	 if (DutyC1 > 166-5) {
	    	 	DutyC1 = 166;
	    	 }
	    	 else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0){
	    	 	DutyC1 = DutyC1 + 10;
	    	 }
	    	 else{
	    	 	DutyC1 = DutyC1 + 5;
	    	 }
	    }

	    //down Button
	    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0){
	    	 if (DutyC2 < 17+5) {
	    	 	DutyC2 = 17;
	    	 }
	    	 else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0){
	    	 	DutyC2 = DutyC2 - 10;
	    	 }
	    	 else{
	    	 	DutyC2 = DutyC2 - 5;
	    	 }
	    }

	    //Up Button
	    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0){
	    	if (DutyC2 > 166-5) {
	    		DutyC2 = 166;
	    	}
	    	else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0){
	    		DutyC2 = DutyC2 + 10;
	    	}
	    	else{
	    		DutyC2 = DutyC2 + 5;
	    	}
	    }

	 	//TIM_OCStructInit( &TIM_OCInitStruct );

	 	/* PWM1 Mode configuration: Channel1 */
	 	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	 	TIM_OCInitStruct.TIM_Pulse = DutyC1;

	 	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	 	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	 	//TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	 	/* PWM1 Mode configuration: Channel2 */
	 	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	 	TIM_OCInitStruct.TIM_Pulse = DutyC2;

	 	TIM_OC4Init(TIM3, &TIM_OCInitStruct);
	 	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	 }

}


int main(void)
{

  	RCC_Configuration();

	/* GPIO Configuration */
	GPIO_Configuration();

	//Timer parameters configuration
	TIMER_Configuration();


	//PW1 and PW2 config

	TIM_OCStructInit( &TIM_OCInitStruct );

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    // Initial duty cycle equals 0%. Value can range from zero to 1000.
    TIM_OCInitStruct.TIM_Pulse = DutyC1; // 0 .. 1000 (0=Always Off, 1000=Always On)

    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC3Init( TIM3, &TIM_OCInitStruct );
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = DutyC2;

	TIM_OC4Init(TIM3, &TIM_OCInitStruct);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);



	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);


   EnableTimerInterrupt();


  while(1) { // Do not exit

  }
}
