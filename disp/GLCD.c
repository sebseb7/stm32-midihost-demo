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
	
	/* PC.08(RS), PC.11(WR), PC.06(RD) */

        /* setting the IO to output mode */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_11 | GPIO_Pin_8;
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
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
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
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
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
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");

        /* selected LCD register */ 
	LCD_WriteIndex(LCD_Reg);
      
	/* Write register data */
	LCD_WriteData(LCD_RegValue);
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
  
	Set_Cs;   /* Cs high */
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
	__ASM volatile ("nop");
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

	printf("id: %i\n",LCD_ID);

//9324

//	if( LCD_ID == 0x9320 || LCD_ID == 0x9300 )
	{
		LCD_SetCursor(0,0); 

		Clr_Cs;  /* Cs low */

		/* selected LCD register */ 
		LCD_WriteIndex(0x0022);

		for( int index = 0; index < 320*240; index++ )
		{
			LCD_WriteData( 0 );
		}

		Set_Cs; 

		LCD_WriteReg( 0x0001, 0x0100); /* set SS and SM bit */
		LCD_WriteReg( 0x0002, 0x0700); /* set 1 line inversion */
		LCD_WriteReg( 0x0003, 0x1028); /* RGB inversion */
		LCD_WriteReg( 0x0004, 0x0000); /* Resize register */
		LCD_WriteReg( 0x0008, 0x0207); /* set the back porch and front porch */
		LCD_WriteReg( 0x0009, 0x0000); /* set non-display area refresh cycle */
		LCD_WriteReg( 0x000A, 0x0000); /* FMARK function */
		LCD_WriteReg( 0x000C, 0x0001); /* RGB interface setting was 0*/
		LCD_WriteReg( 0x000D, 0x0000); /* Frame marker Position */
		LCD_WriteReg( 0x000F, 0x0000); /* RGB interface polarity */

		/* ----------- Power On sequence ----------- */
		LCD_WriteReg( 0x0010, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg( 0x0011, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg( 0x0012, 0x0000); /* VREG1OUT voltage */
		LCD_WriteReg( 0x0013, 0x0000); /* VDV[4:0] for VCOM amplitude */
		delay_ms(100); /* Dis-charge capacitor power voltage */
		LCD_WriteReg( 0x0010, 0x14b0);
		delay_ms(50);
		LCD_WriteReg( 0x0011, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0] */
		delay_ms(50); /* Delay 50ms */
		LCD_WriteReg( 0x0012, 0x005e); /* Internal reference voltage= Vci; */
		LCD_WriteReg( 0x0029, 0x0015); /* Set VCM[5:0] for VCOMH */
		delay_ms(50); /* Delay 50ms */
		LCD_WriteReg( 0x002B, 0x000e); /* Set Frame Rate */ // 5 flickers but should not: 40fps
		LCD_WriteReg( 0x0020, 0x0000); /* GRAM horizontal Address */
		LCD_WriteReg( 0x0021, 0x0000); /* GRAM Vertical Address */

		/*------------------ Set GRAM area --------------- */
		LCD_WriteReg( 0x0050, 0x0000); /* Horizontal GRAM Start Address */
		LCD_WriteReg( 0x0051, 0x00EF); /* Horizontal GRAM End Address */
		LCD_WriteReg( 0x0052, 0x0000); /* Vertical GRAM Start Address */
		LCD_WriteReg( 0x0053, 0x013F); /* Vertical GRAM Start Address */
		LCD_WriteReg( 0x0060, 0xA700); /* Gate Scan Line */
		LCD_WriteReg( 0x0061, 0x0001); /* NDL,VLE, REV */
		LCD_WriteReg( 0x006A, 0x0000); /* set scrolling line */
	
		LCD_WriteReg(0x30, 0x0000);  
		LCD_WriteReg(0x31, 0x0107);  
		LCD_WriteReg(0x32, 0x0000);  
		LCD_WriteReg(0x35, 0x0203);  
		LCD_WriteReg(0x36, 0x0402);  
		LCD_WriteReg(0x37, 0x0000);  
		LCD_WriteReg(0x38, 0x0207);  
		LCD_WriteReg(0x39, 0x0000);  
		LCD_WriteReg(0x3C, 0x0203);  
		LCD_WriteReg(0x3D, 0x0403);  

		/*-------------- Partial Display Control --------- */
		LCD_WriteReg( 0x0080, 0x0000);
		LCD_WriteReg( 0x0081, 0x0000);
		LCD_WriteReg( 0x0082, 0x0000);
		LCD_WriteReg( 0x0083, 0x0000);
		LCD_WriteReg( 0x0084, 0x0000);
		LCD_WriteReg( 0x0085, 0x0000);

		/*-------------- Panel Control ------------------- */
		LCD_WriteReg( 0x0090, 0x0029);//was 0001
		LCD_WriteReg( 0x0092, 0x0600);
		LCD_WriteReg( 0x0007, 0x0133); /* 262K color and display ON */
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



