/******************************************************************************
 * SunDUINO.pl
 *
 * Przyklad demonstrujacy naprzemienne miganie dwóch ledów obecnych na 
 * plytce SUN-1xxR .
 *
 * SunRiver  / forum.sunduino.pl
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration(void);

/*******************************************************************************
 * Function Name  : Delay
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


/*******************************************************************************
 * Function Name  : main
 * Description    : MAIN 
*******************************************************************************/
int main(void)
{
	GPIO_Configuration();
    
	  // Nieskonczona petla 
    while (1)
			{
		   /*==== LED1-ON / LED2 -OFF =======*/
		   GPIO_SetBits(GPIOD , GPIO_Pin_2);
		   GPIO_ResetBits(GPIOA , GPIO_Pin_1);
		   Delay(0xfffff);
		   Delay(0xfffff);
		   Delay(0x5ffff);	

		   /*==== LED1-OFF / LED2 -ON =======*/ 
		   GPIO_ResetBits(GPIOD , GPIO_Pin_2);
		   GPIO_SetBits(GPIOA , GPIO_Pin_1);
		   Delay(0xfffff);
		   Delay(0xfffff);
		   Delay(0x5ffff);				
      }
}

/*******************************************************************************
 * Function Name  : GPIO_Configuration
 * Description    : Configure GPIO Pin
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE); 	
  	
  /**
   *	LED1-> PD2  --  dioda niebieska PD2
   */					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	//------------------------------------------------------------------------------
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 
  /**
   *	LED2  -> PC15  --- (uzyty PA1 podlaczenie kabelkiem do PC15) 
   *  (jesli nie podlaczony pin PC15 do LED2)
   */					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
}



/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
