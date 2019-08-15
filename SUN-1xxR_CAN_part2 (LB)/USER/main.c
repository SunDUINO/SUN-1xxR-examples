/******************************************************************************
 * SunDUINO.pl
 *
 * Przyklad demonstrujacy komunikacje CAN dane sa retransmitowane na uart UART 
 * (przez obecny na plytce MCP2200) na plytce SUN-1xxR .
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

/* ZMIENNE */
__IO uint32_t Result = 0; // dla powrotu z przerwania 

/* Prototypy funkcji */
void CAN_test(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void NVIC_Configuration(void);
FunctionalState CAN_Polling(void);
FunctionalState CAN_Interrupt(void);

/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/*******************************************************************************
* Function Name  : main
*******************************************************************************/
int main(void)
{
   volatile FunctionalState TestRx;

	USART_Configuration();	
	NVIC_Configuration();
	GPIO_Configuration();
	
	printf("**************************************************************\r\n");
  printf("*   SunDUINO                                                 *\r\n");
	printf("*   Dziekuje za uzywanie SunEVB z modulem SUN-1xxR  ^_^      *\r\n");
	printf("*   USART1 , 115200bps                                       *\r\n");
	printf("*   --- forum.sunduino.pl                                    *\r\n");
	printf("**************************************************************\r\n");
  
	printf("Test CAN \r\n");

	CAN_test();

    /* Transmisja z szybkoscia 100Kb/s , odbiór przez odpytywanie w trybie loopback */
    TestRx = CAN_Polling();

    if (TestRx == DISABLE)
    {
      printf("(100Kb/s) Polling w trybie loopback : Error \r\n");
    }
    else
    {
	    printf("(100kb/s) Polling w trybie loopback : OK \r\n");
    }

    /* Transmisja z szybkoscia 500Kb/s , odbiór przez odpytywanie w trybie loopback */
    TestRx = CAN_Interrupt();

    if (TestRx == DISABLE)
    {
      printf("500Kb/s) Polling w trybie loopback : Error \r\n");
    }
    else
    {
      printf("(500kb/s) Polling w trybie loopback : OK \r\n");
    }
   
		
		
    while (1){

    }
}

void CAN_test(void)	
{	   
  // ustawienia pinów CAN dla transmitera SN65HVD230 
  uint8_t  err = 0;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  printf("\r\n SN65HVD230 \r\n");

  GPIO_SetBits(GPIOB, GPIO_Pin_9);
  Delay(0x1fffff);

  if (GPIO_ReadInputDataBit(GPIOB ,GPIO_Pin_8) == 0)
  {
	 err = 1;
  }

  GPIO_ResetBits(GPIOB, GPIO_Pin_9);
  Delay(0x1fffff);
  if (GPIO_ReadInputDataBit(GPIOB ,GPIO_Pin_8) != 0 )
  {
	 err = 1;
  }

  /* konfiguracja pinu: RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  /* konfiguracja pinu: TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	   

  if (err == 1)
  {
	 printf("Problem z transmiterem CAN \r\n");
  }
  else
  {
	 printf("CAN pracuje prawidlowo ! \r\n");
  }
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Konfiguracja kontrolera przerwan
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Wlaczenie przerwania IRQ dla  CAN1 RX0  */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************************
* Function Name  : CAN_Polling
* Description    : Konfiguracja polingu dla CAN .
* Return         : PASSED jesli odbiornik pracuje prawidlowo, FAILED w kazdym innym wypadku
********************************************************************************************/
FunctionalState CAN_Polling(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  CanTxMsg TxMessage;
  CanRxMsg RxMessage;
  uint32_t i = 0;
  uint8_t TransmitMailbox = 0;

  /* Ustawienia rejestrów */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* Ustawienia komórek*/
  CAN_InitStructure.CAN_TTCM=DISABLE;
  CAN_InitStructure.CAN_ABOM=DISABLE;
  CAN_InitStructure.CAN_AWUM=DISABLE;
  CAN_InitStructure.CAN_NART=DISABLE;
  CAN_InitStructure.CAN_RFLM=DISABLE;
  CAN_InitStructure.CAN_TXFP=DISABLE;
  CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;
  CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
  CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
  CAN_InitStructure.CAN_Prescaler=5;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* Ustawienia filtrów */
  CAN_FilterInitStructure.CAN_FilterNumber=0;
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /******* Nadawanie **************/
  TxMessage.StdId=0x11;
  TxMessage.RTR=CAN_RTR_DATA;
  TxMessage.IDE=CAN_ID_STD;
  TxMessage.DLC=2;
  TxMessage.Data[0]=0xCA;
  TxMessage.Data[1]=0xFE;

  TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
  i = 0;
  while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
  {
    i++;
  }

  i = 0;
  while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
  {
    i++;
  }

  /****** Odbieranie ***********/
  RxMessage.StdId=0x00;
  RxMessage.IDE=CAN_ID_STD;
  RxMessage.DLC=0;
  RxMessage.Data[0]=0x00;
  RxMessage.Data[1]=0x00;
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

  if (RxMessage.StdId!=0x11)
  {
    return DISABLE;  
  }

  if (RxMessage.IDE!=CAN_ID_STD)
  {
    return DISABLE;
  }

  if (RxMessage.DLC!=2)
  {
    return DISABLE;  
  }

  if ((RxMessage.Data[0]<<8|RxMessage.Data[1])!=0xCAFE)
  {
    return DISABLE;
  }
  
  return ENABLE; /* Test wypadl pomyslnie */
}

/***********************************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Konfiguracja przerwan dla odbioru i nadawania CAN
* Return         : PASSED jesli odbiornik pracuje prawidlowo, FAILED w kazdym innym wypadku
************************************************************************************************/
FunctionalState CAN_Interrupt(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  CanTxMsg TxMessage;
  uint32_t i = 0;

  /* Inicjalizacja rejestrów CAN */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  CAN_InitStructure.CAN_TTCM=DISABLE;
  CAN_InitStructure.CAN_ABOM=DISABLE;
  CAN_InitStructure.CAN_AWUM=DISABLE;
  CAN_InitStructure.CAN_NART=DISABLE;
  CAN_InitStructure.CAN_RFLM=DISABLE;
  CAN_InitStructure.CAN_TXFP=DISABLE;
  CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;
  CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
  CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
  CAN_InitStructure.CAN_Prescaler=1;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* ustawienia filtrów */
  CAN_FilterInitStructure.CAN_FilterNumber=1;
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /* CAN FIFO --  wiadomosc oczekuje na przerwanie */ 
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

  /* Nadawanie 1 wiadomosci */
  TxMessage.StdId=0x00;
  TxMessage.ExtId=0x1234;
  TxMessage.IDE=CAN_ID_EXT;
  TxMessage.RTR=CAN_RTR_DATA;
  TxMessage.DLC=2;
  TxMessage.Data[0]=0xDE;
  TxMessage.Data[1]=0xCA;
  CAN_Transmit(CAN1, &TxMessage);

  /* Inicjowanie wartosci która zostaje zwrócona*/
  Result = 0xFF;
       
  /* odebranie wiadomosci z obsluga przerwania */
  i=0;
  while((Result == 0xFF) && (i < 0xFFF))
  {
    i++;
  }
  
  if (i == 0xFFF)
  {
    Result=0;  
  }

  /* wylaczenie obslugi przerwania */
  CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);

  return (FunctionalState)Result;
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Konfiguracja ustawien portów GPIO
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB , ENABLE); 
  /* CAN1 wlaczenie zegara  */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);						 
  
  /* Ustawienie pinu: TX dla CAN */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinRemapConfig(AFIO_MAPR_CAN_REMAP_REMAP2 , ENABLE);		      
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Konfiguracja USART1
*******************************************************************************/
void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
  /*
    *  USART1_TX -> PA9  
	*  USART1_RX ->	PA10
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

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

