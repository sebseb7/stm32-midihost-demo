/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.c
** Descriptions:		This example describes how to use glcd
**						
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-3-29
** Version:			2.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 


/* Program is based on the STM32 MCU */
/* If you are not use STM32, you must transplant program */

/*******************************************************************************
* Function Name  : Lcd_Configuration
* Description    : setting the IO to control LCD
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_Configuration(void)
{
        /* If you are not use STM32, you must change it */

	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIOC and GPIOE clocks */
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);  
	          
/* 
    PE.00(D0), PE.01(D1), PE.02(D2), PE.03(D3), PE.04(D4), PE.05(D5), PE.06(D6), PE.07(D7), PE.08(D8)
    PE.09(D9), PE.10(D10), PE.11(D11), PE.12(D12), PE.13(D13), PE.14(D14), PE.15(D15)   */
        
	/* setting the IO to output mode */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* PC.08(RS), PC.07(WR), PC.06(RD) */

        /* setting the IO to output mode */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* PC.09(CS)*/

        /* setting the IO to output mode */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCD write register address ( on include data only address )
* Input          : - index: register address
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_WriteIndex(unsigned short index)
{
        /* If you are not use STM32, you must change it */

	Clr_Rs;   /* RS low */
	Set_nRd;  /* RD high */
	
        /* write data */
	GPIOE->ODR = index;	 /* GPIO_Write(GPIOE,index); */
	
	Clr_nWr;  /* Wr low */
	Set_nWr;  /* Wr high */
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCD write register data
* Input          : - index: register data
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_WriteData(unsigned short data)
{
        /* If you are not use STM32, you must change it */

	Set_Rs;  /* RS high */
	
        /* write data */
	GPIOE->ODR = data;	 /* GPIO_Write(GPIOE,data); */
	
	Clr_nWr;  /* Wr low */ 
	Set_nWr;  /* Wr high */
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : LCD read data
* Input          : None
* Output         : None
* Return         : return data
* Attention	 : None
*******************************************************************************/
unsigned short LCD_ReadData(void)
{ 
        /* If you are not use STM32, you must change it */

	unsigned short value;

	Set_Rs;  /* RS high */ 
	Set_nWr; /* Wr high */
	Clr_nRd; /* Rd low */ 

/* 
    PE.00(D0), PE.01(D1), PE.02(D2), PE.03(D3), PE.04(D4), PE.05(D5), PE.06(D6), PE.07(D7), PE.08(D8)
    PE.09(D9), PE.10(D10), PE.11(D11), PE.12(D12), PE.13(D13), PE.14(D14), PE.15(D15)   */
 

    /* setting the IO to input mode */

    GPIO_InitTypeDef GPIO_InitStructure; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    /* read data */
    value = GPIO_ReadInputData(GPIOE);
    value = GPIO_ReadInputData(GPIOE);
    
    /* Read twice to ensure correct data */

    /* read data OK */

/*  
    PE.00(D0), PE.01(D1), PE.02(D2), PE.03(D3), PE.04(D4), PE.05(D5), PE.06(D6), PE.07(D7), PE.08(D8)
    PE.09(D9), PE.10(D10), PE.11(D11), PE.12(D12), PE.13(D13), PE.14(D14), PE.15(D15)   
*/
    
    /* setting the IO to output mode */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    Set_nRd;   /* Rd high */ 

    return value;  /* return data */
}


/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention	 : None
*******************************************************************************/
void LCD_WriteReg( unsigned short LCD_Reg, unsigned short LCD_RegValue )
{ 
	/* Write 16-bit Index, then Write Reg */

	Clr_Cs;   /* Cs low */ 

        /* selected LCD register */ 
	LCD_WriteIndex(LCD_Reg);
      
	/* Write register data */
	LCD_WriteData(LCD_RegValue);
  
	Set_Cs;   /* Cs high */
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD register data.
* Input          : - LCD_Reg: address of the selected register.
* Output         : None
* Return         : return Register data.
* Attention	 : None
*******************************************************************************/
unsigned short LCD_ReadReg(unsigned short LCD_Reg)
{
	unsigned short LCD_RAM;
	
	Clr_Cs;  /* Cs low */ 
 
        /* selected LCD register */ 
	LCD_WriteIndex(LCD_Reg);

	/* Write register data */
	LCD_RAM = LCD_ReadData(); 
     	
	Set_Cs;  /* Cs high */

        /* return read data */
	return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetCursor( unsigned short Xpos, unsigned short Ypos )
{
    LCD_WriteReg(0x0020, Xpos );     
    LCD_WriteReg(0x0021, Ypos );     
}


/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention	 : None
*******************************************************************************/
void LCD_Initializtion(void)
{
	unsigned short LCD_ID;
	
	LCD_Configuration();

	LCD_ID = LCD_ReadReg(0x0000);		/* read the LCD ID, if Controller is ILI9320, The ID value is 0x9320 */
	
	
	if( LCD_ID == 0x9320 || LCD_ID == 0x9300 )
	{
	    LCD_WriteReg(0x00,0x0000);
	    LCD_WriteReg(0x01,0x0100);	/* Driver Output Contral */
	    LCD_WriteReg(0x02,0x0700);	/* LCD Driver Waveform Contral */
	    LCD_WriteReg(0x03,0x1018);	/* Entry Mode Set */
		
	    LCD_WriteReg(0x04,0x0000);	/* Scalling Contral */
	    LCD_WriteReg(0x08,0x0202);	/* Display Contral */
	    LCD_WriteReg(0x09,0x0000);	/* Display Contral 3.(0x0000) */
	    LCD_WriteReg(0x0a,0x0000);	/* Frame Cycle Contal.(0x0000) */
	    LCD_WriteReg(0x0c,(1<<0));	/* Extern Display Interface Contral */
	    LCD_WriteReg(0x0d,0x0000);	/* Frame Maker Position */
	    LCD_WriteReg(0x0f,0x0000);	/* Extern Display Interface Contral 2. */
		
	    delay_ms(100);  /* delay 100 ms */		
	    LCD_WriteReg(0x07,0x0101);	/* Display Contral */
	    delay_ms(100);  /* delay 100 ms */		
	
	    LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	/* Power Control 1.(0x16b0)	*/
	    LCD_WriteReg(0x11,0x0007);								/* Power Control 2 */
	    LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				/* Power Control 3.(0x0138)	*/
	    LCD_WriteReg(0x13,0x0b00);								/* Power Control 4 */
	    LCD_WriteReg(0x29,0x0000);								/* Power Control 7 */
		
	    LCD_WriteReg(0x2b,(1<<14)|(1<<4));
			
	    LCD_WriteReg(0x50,0);       /* Set X Start */
	    LCD_WriteReg(0x51,239);	    /* Set X End */
	    LCD_WriteReg(0x52,0);	    /* Set Y Start */
	    LCD_WriteReg(0x53,319);	    /* Set Y End */
		
	    LCD_WriteReg(0x60,0x2700);	/* Driver Output Control */
	    LCD_WriteReg(0x61,0x0001);	/* Driver Output Control */
	    LCD_WriteReg(0x6a,0x0000);	/* Vertical Srcoll Control */
		
	    LCD_WriteReg(0x80,0x0000);	/* Display Position? Partial Display 1 */
	    LCD_WriteReg(0x81,0x0000);	/* RAM Address Start? Partial Display 1 */
	    LCD_WriteReg(0x82,0x0000);	/* RAM Address End-Partial Display 1 */
	    LCD_WriteReg(0x83,0x0000);	/* Displsy Position? Partial Display 2 */
	    LCD_WriteReg(0x84,0x0000);	/* RAM Address Start? Partial Display 2 */
	    LCD_WriteReg(0x85,0x0000);	/* RAM Address End? Partial Display 2 */
		
	    LCD_WriteReg(0x90,(0<<7)|(16<<0));	/* Frame Cycle Contral.(0x0013)	*/
	    LCD_WriteReg(0x92,0x0000);	/* Panel Interface Contral 2.(0x0000) */
	    LCD_WriteReg(0x93,0x0001);	/* Panel Interface Contral 3. */
	    LCD_WriteReg(0x95,0x0110);	/* Frame Cycle Contral.(0x0110)	*/
	    LCD_WriteReg(0x97,(0<<8));	
	    LCD_WriteReg(0x98,0x0000);	/* Frame Cycle Contral */
	    LCD_WriteReg(0x07,0x0173);
	}
        else
        {
              /* read LCD ID fail, testing terminated */
              /* fatal error */
              while(1);
        }					
    delay_ms(50);   /* delay 50 ms */		
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : clear LCD screen
* Input          : - Color: LCD screen color
* Output         : None
* Return         : None
* Attention	 : None
*******************************************************************************/
void LCD_Clear( unsigned short color )
{
	unsigned int index=0;
	
	LCD_SetCursor(0,0); 
	
	Clr_Cs;  /* Cs low */

        /* selected LCD register */ 
	LCD_WriteIndex(0x0022);

	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
                /* Write data */
		LCD_WriteData( color );
	}

	Set_Cs;  /* Cs high */
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB change BBBBBGGGGGGRRRRR
* Input          : - color: BRG color value
* Output         : None
* Return         : RGB color value
* Attention	 : None
*******************************************************************************/
unsigned short LCD_BGR2RGB( unsigned short color )
{
	unsigned short  r, g, b, rgb;
	
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	
	rgb =  (b<<11) + (g<<5) + (r<<0);
	
	return( rgb );
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : Get the color value of the specified coordinates
* Input          : - Xpos: Row Coordinate
*                  - Xpos: Line Coordinate 
* Output         : None
* Return         : Screen Color
* Attention	 : None
*******************************************************************************/
unsigned short LCD_GetPoint( unsigned short Xpos, unsigned short Ypos )
{
	unsigned short dummy;
	
	LCD_SetCursor(Xpos,Ypos);

	Clr_Cs;   /* Cs low */

        /* selected LCD register */ 
	LCD_WriteIndex(0x0022);  
	

        dummy = LCD_ReadData();   /* NOP read necessary */
        dummy = LCD_ReadData();   /* read data */

        Set_Cs;   /* Cs high */
        
        /* RRRRRGGGGGGBBBBB change BBBBBGGGGGGRRRRR and return data */
        return  LCD_BGR2RGB( dummy );

}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : Draw point at the specified coordinates
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}



