Loop BACK obsluga CAN w STM32 
Prezentowane rozwiazanie na plytce SunEVB z modulem SUN-1xxR
z mikrokontrolerem STM32F103RB.



-- USART1 -- TX -> PA9,  RX ->	PA10 ; 115200bps 
-- CAN -- RX --> PB8  , TX --> PB9  

-- program sprawdza czy jest prawidlowo podlaczony transmiter SN65HVD230
-- i informuje o prawidlowym lub nieprawidlowym funkcjonowaniu CAN 
		
-- kwarc : 8MHz
-- lib   : StdPherip_Driver

-- Debug : ST-Link v2

// ----------------------------------------------------------------------
// --  Odwiedz  ...
// ----------------------------------------------------------------------
//
// sunduino.pl
// forum.sunduino.pl
