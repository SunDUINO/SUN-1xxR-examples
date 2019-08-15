/******************************************************************************
 * SunDUINO.pl
 *
 * Przyklad demonstrujacy sterowanie PWM przez ADC 
 * na plytce SUN-1xxR .
 *
 * SunRiver  / forum.sunduino.pl
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "systick.h"
#include <stdio.h>

#ifdef __GNUC__
  
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void TIM_Configuration(void);
void ADC_Configuration(void);

/* Private variables ---------------------------------------------------------*/
float AD_value;
uint16_t TimerPeriod = 0;
uint16_t Channel2Pulse = 0;

/*******************************************************************************
* Function Name  : main
*******************************************************************************/
int main(void)
{
	GPIO_Configuration();
	ADC_Configuration();
	delay_init();
	
	
    /* Nieskonczona petla */
    while (1)
	  {
	    delay_ms( 1000 );
	    AD_value = ADC_GetConversionValue(ADC1);
	    Channel2Pulse = (AD_value/4096)*1000;
      TIM_Configuration();
    }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Konfiguracja GPIO
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);

  /* Konfiguracja PB0 (ADC CH8) jako wejscie analogowe -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);     
  						 
  /* Konfiguracja PB5 (TIM3 CH2) jako wyjscie PWM typu Push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);	 /* Remaping */              
}

/*******************************************************************************
* Function Name  : TIM_Configuration
* Description    : Konfiguracja Timerów.
*******************************************************************************/
void TIM_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
  /* -----------------------------------------------------------------------
    TIM3CLK = 36 MHz, Prescaler = 35, TIM3 counter clock = 1 MHz
    TIM3 ARR Register = 999 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
    TIM3 Frequency = 1 KHz.
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 
----------------------------------------------------------------------- */
   /* Nasze ustawienia dla 1MHz TIM3 clock = 1MHz */
  TimerPeriod = (uint16_t) (SystemCoreClock / 1000000) - 1;

  /* 1Mhz/(999 + 1 )=1kHz */
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseStructure.TIM_Prescaler = TimerPeriod; 
  TIM_TimeBaseStructure.TIM_Period = 999;   /* TIM3 ARR Register */
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  /* TIM_Pulse(CCRx/ARR)*100*/ 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Channel2Pulse; 
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);    

  TIM_ARRPreloadConfig(TIM3, ENABLE);  /* Ladujemy ustawienia */
  TIM_Cmd(TIM3, ENABLE);			  /* wlaczamy  Timer */
}

/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : Konfiguracja ADC
*******************************************************************************/
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
    
  /* ADC1 */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	   
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;			   
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	  
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		      
  ADC_InitStructure.ADC_NbrOfChannel = 1;					   
  ADC_Init(ADC1, &ADC_InitStructure);

 /* ADC1 ustawiamy CH8 */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);  
  ADC_Cmd(ADC1, ENABLE);                 /* Wlaczamy ADC1 */                      
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
}



/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
