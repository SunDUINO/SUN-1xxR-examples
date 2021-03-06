/******************************************************************************
 * SunDUINO.pl
 *
 * Przyklad demonstrujacy komunikacje UART (przez obecny na plytce MCP2200)
 * na plytce SUN-1xxR .
 *
 * SunRiver  / forum.sunduino.pl
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#ifdef __GNUC__
  
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void USART_Configuration(void);

/*******************************************************************************
* Function Name  : main
*******************************************************************************/
int main(void)
{
	USART_Configuration();
	
	printf("**************************************************************\r\n");
  printf("*   SunDUINO                                                 *\r\n");
	printf("*   Dziekuje za uzywanie SunEVB z modulem SUN-1xxR  ^_^      *\r\n");
	printf("*   USART1 , 115200bps                                       *\r\n");
	printf("*   --- forum.sunduino.pl                                    *\r\n");
	printf("**************************************************************\r\n");
  printf("\r\n Wprowadz slowo lub znaki :\r\n");
	
    /* Nieskonczona petla */
    while (1)
			{
		    /* RXNE = 1  ( emulacja echa w terminalu )*/
        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		    USART_SendData(USART1,USART_ReceiveData(USART1));
      }
}


/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Ustawienia USART1 
*******************************************************************************/
void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
  /*
   *  USART1_TX ---> PA9  
	 *  USART1_RX ---> PA10
   */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART1,USART_FLAG_TC);
  USART_Cmd(USART1, ENABLE);
}


PUTCHAR_PROTOTYPE
{
 
  USART_SendData(USART1, (uint8_t) ch);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}



/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

