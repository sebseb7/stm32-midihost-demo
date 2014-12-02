/**
 ******************************************************************************
 * @file    main.c
 * @author	Xavier Halgand
 * @version
 * @date
 * @brief   Dekrispator main file
 ******************************************************************************
 *
 *
 ******************************************************************************
 */

/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/*---------------------------------------------------------------------------*/

__ALIGN_BEGIN USB_OTG_CORE_HANDLE		USB_OTG_Core_dev __ALIGN_END  ;

__ALIGN_BEGIN USBH_HOST					USB_Host __ALIGN_END ;

/*====================================================================================================*/

static __IO uint32_t TimingDelay;
void delay_ms(__IO uint32_t nTime)
{
	TimingDelay = nTime*1;

	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

static uint16_t usb_ready=0;
void USB_Host_Handle()
{
	if(usb_ready==1)
	{
			USBH_Process(&USB_OTG_Core_dev , &USB_Host);
	}
}

int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;



	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

	/* Initialize Discovery board LEDS */
	STM_EVAL_LEDInit(LED_Green);
	STM_EVAL_LEDInit(LED_Orange);
	STM_EVAL_LEDInit(LED_Red);
	STM_EVAL_LEDInit(LED_Blue);

		USBH_Init(&USB_OTG_Core_dev,
				USB_OTG_FS_CORE_ID,
				&USB_Host,
				&MIDI_cb,
				&USR_Callbacks);

		usb_ready=1;

		while (1)
		{

			delay_ms(100);
			MIDI_send(0xb0,32,127);
			delay_ms(100);
			MIDI_send(0xb0,32,0);
		}
}
