
#include "main.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE		USB_OTG_dev __ALIGN_END  ;
__ALIGN_BEGIN USBH_HOST					USB_Host __ALIGN_END ;

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
			USBH_Process(&USB_OTG_dev , &USB_Host);
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

	USBH_Init(&USB_OTG_dev,
			USB_OTG_FS_CORE_ID,
			&USB_Host,
			&MIDI_cb,
			&USR_Callbacks);

	usb_ready=1;

	while (1)
	{
		delay_ms(30);

		{	
			mios32_midi_package_t package;

			package.type     = CC;
			package.event    = CC;
			package.note     = 34;
			package.velocity = 127;

			USB_MIDI_PackageSend_NonBlocking(package);
		}
		delay_ms(30);

		{	
			mios32_midi_package_t package;

			package.type     = CC;
			package.event    = CC;
			package.note     = 32;
			package.velocity = 127;

			USB_MIDI_PackageSend_NonBlocking(package);
		}	
		delay_ms(30);

		{	
			mios32_midi_package_t package;

			package.type     = CC;
			package.event    = CC;
			package.note     = 34;
			package.velocity = 0;

			USB_MIDI_PackageSend_NonBlocking(package);
		}
		delay_ms(30);

		{	
			mios32_midi_package_t package;

			package.type     = CC;
			package.event    = CC;
			package.note     = 32;
			package.velocity = 0;

			USB_MIDI_PackageSend_NonBlocking(package);
		}	

	}
}
