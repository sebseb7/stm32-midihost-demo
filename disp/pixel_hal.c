#include "pixel_hal.h"
#include "GLCD.h"



static uint16_t leds[LCD_HEIGHT-100][LCD_WIDTH];
static uint16_t leds_a[100][LCD_WIDTH] __attribute__((section(".ccm")));

	

void clearDisplay(void)
{
	int x, y;
	for(x = 0; x < LCD_WIDTH; x++) {
		for(y = 0; y < LCD_HEIGHT; y++) {
			if(y < (LCD_HEIGHT-100))
			{
				leds[y][x] = 0;
			}else{
				leds_a[y-140][x] = 0;
			}
//			leds[y][x][1] = 0;
//			leds[y][x][2] = 0;
		}
	}
}

void setLedXY(uint16_t x, uint16_t y, uint8_t red,uint8_t green, uint8_t blue) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
	if(y < (LCD_HEIGHT-100))
	{
		leds[y][x] = (( (red   >> 3) & 0x001f ) << 11 | ( (green >> 2) & 0x003f ) << 5 | ((blue  >> 3) & 0x001f));
	}
	else
	{
		leds_a[y-140][x] = (( (red   >> 3) & 0x001f ) << 11 | ( (green >> 2) & 0x003f ) << 5 | ((blue  >> 3) & 0x001f));
	}
//	leds[y][x][1] = green;
//	leds[y][x][2] = blue;
}

void invLedXY(uint16_t x, uint16_t y) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
//	leds[y][x][0] = 255 - leds[y][x][0];
//	leds[y][x][1] = 255 - leds[y][x][1];
//	leds[y][x][2] = 255 - leds[y][x][2];
}
void getLedXY(uint16_t x, uint16_t y, uint8_t* red,uint8_t* green, uint8_t* blue) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
	
	if(y < (LCD_HEIGHT-100))
	{
		*red = (uint8_t) (leds[y][x] >> 11)<<3;
		*green = (uint8_t) (leds[y][x] >> 5) << 2;
		*blue = (uint8_t) leds[y][x] << 3;
	}else{
		*red = (uint8_t) (leds_a[y-140][x] >> 11)<<3;
		*green = (uint8_t) (leds_a[y-140][x] >> 5) << 2;
		*blue = (uint8_t) leds_a[y-140][x] << 3;
	}
	
}
		
void sync_frame(void)
{
		LCD_SetCursor(0,0); 

		Clr_Cs;

		/* selected LCD register */ 
		LCD_WriteIndex(0x0022);

	
		Set_Rs;  /* RS high */
		
		
		for(int y = 0; y < LCD_PIXEL_HEIGHT; y++) 
		{

			for(int x = 0; x < LCD_PIXEL_WIDTH; x++) 
			{

				
				if(y < (LCD_HEIGHT-100))
				{
					GPIOE->ODR = leds[y][x];
				}
				else
				{
					GPIOE->ODR = leds_a[y-140][x];
				}

				Clr_nWr;
				//__ASM volatile ("nop");
				Set_nWr; 
			}
		}
		Set_Cs;
}

void testimage(void)
{
		LCD_SetCursor(0,0); 

		Clr_Cs;  /* Cs low */

		/* selected LCD register */ 
		LCD_WriteIndex(0x0022);

		for( int index = 0; index < 320*80; index++ )
		{
			int d = index % 320;
			LCD_WriteData( d/10 );
		}
		for( int index = 0; index < 320*80; index++ )
		{
			int d = index % 320;
			LCD_WriteData( (d/5)<<5 );
		}
		for( int index = 0; index < 320*80; index++ )
		{
			int d = index % 320;
			LCD_WriteData( (d/10)<<11 );
		}

		Set_Cs; 
}

