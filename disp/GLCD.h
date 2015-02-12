/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.h
** Descriptions:		None
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-3-29
** Version:				2.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

#ifndef __GLCD_H 
#define __GLCD_H

/* Includes ------------------------------------------------------------------*/
#include "../main.h"
#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
#define DISP_ORIENTATION  0  /* angle 0 90 */ 

#if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )

#define  MAX_X  320
#define  MAX_Y  240   

#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

#define  MAX_X  240
#define  MAX_Y  320   

#endif

/*
#define Set_Cs        GPIO_SetBits(GPIOC , GPIO_Pin_9);
#define Clr_Cs        GPIO_ResetBits(GPIOC , GPIO_Pin_9);

#define Set_Rs        GPIO_SetBits(GPIOC , GPIO_Pin_8);
#define Clr_Rs        GPIO_ResetBits(GPIOC , GPIO_Pin_8);

#define Set_nWr       GPIO_SetBits(GPIOC , GPIO_Pin_7);
#define Clr_nWr       GPIO_ResetBits(GPIOC , GPIO_Pin_7);

#define Set_nRd       GPIO_SetBits(GPIOC , GPIO_Pin_6);
#define Clr_nRd       GPIO_ResetBits(GPIOC , GPIO_Pin_6);
*/

#define Set_Cs        GPIOC->ODR  |= ( 1<<9 );
#define Clr_Cs        GPIOC->ODR  &= ~( 1<<9 );

#define Set_Rs        GPIOC->ODR  |= ( 1<<8 );
#define Clr_Rs        GPIOC->ODR  &= ~( 1<<8 );

#define Set_nWr       GPIOC->ODR  |= ( 1<<11 );
#define Clr_nWr       GPIOC->ODR  &= ~( 1<<11 );

#define Set_nRd       GPIOC->ODR  |= ( 1<<6 );
#define Clr_nRd       GPIOC->ODR  &= ~( 1<<6 );

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

/* Private function prototypes -----------------------------------------------*/
void LCD_Configuration(void);
void LCD_Initializtion(void);
void LCD_Clear(uint16_t Color);
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos);
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point);
void LCD_SetCursor( unsigned short Xpos, unsigned short Ypos );
void LCD_WriteData(unsigned short data);
void LCD_WriteIndex(unsigned short index);
#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
