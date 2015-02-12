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

__ALIGN_BEGIN USB_OTG_CORE_HANDLE		USB_OTG_Core_dev __ALIGN_END;

__ALIGN_BEGIN USBH_HOST					USB_Host __ALIGN_END;

/*====================================================================================================*/

static __IO uint32_t TimingDelay;
void delay_ms(__IO uint32_t nTime) {
	TimingDelay = nTime*10;

	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void) {
	if (TimingDelay != 0x00) {
		TimingDelay--;
	}
}

static uint16_t usb_ready = 0;

void USB_Host_Handle() {
	if (usb_ready == 1) {
		USBH_Process(&USB_OTG_Core_dev, &USB_Host);
	}
}

void init_audio(void);

int main(void) {

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	// SysTick end of count event each 0.1ms
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);

	// Initialize Discovery board LEDS
	STM_EVAL_LEDInit(LED_Green);
	STM_EVAL_LEDInit(LED_Orange);
	STM_EVAL_LEDInit(LED_Red);
	STM_EVAL_LEDInit(LED_Blue);


	printf("usb-init\n");
	USBH_Init(&USB_OTG_Core_dev,
			USB_OTG_FS_CORE_ID,
			&USB_Host,
			&MIDI_cb,
			&USR_Callbacks);
	printf("usb-init done\n");

	init_audio();

	usb_ready = 1;
/*
	MIDI_EventPacket_t packet;
	packet.channel = 0;
	packet.type = CC;
	packet.cc = 32;
*/

	while (1) {
		delay_ms(100);
	//	STM_EVAL_LEDToggle(LED_Green);
/*
		packet.value = 127;
		MIDI_send(packet);
		delay_ms(100);
		packet.value = 0;
		MIDI_send(packet);
*/
	}
}



enum {
	BUFFER_SIZE = 640,
	MAX_VOLUME = 87,
	MIXRATE = I2S_AudioFreq_48k
};

typedef struct {
	float phase;
	float speed;
	float pulsewidth;

} Channel;


Channel chan = {
	0,
	440.0 / MIXRATE,
	0.3
};

void mix(float frame[2]) {
	chan.pulsewidth += 0.00001;
	chan.pulsewidth -= (int) chan.pulsewidth;


	chan.phase += chan.speed;
	chan.phase -= (int) chan.phase;

	float amp = chan.phase < chan.pulsewidth ? -1 : 1;

	frame[0] = amp;
	frame[1] = amp;

}



uint16_t audio_buffer[BUFFER_SIZE] = {0};
static void fill_audio_buffer(uint16_t offset, uint16_t length) {
	float frame[2];
	for (int i = 0; i < length; i += 2) {
		mix(frame);
		audio_buffer[offset + i + 0] = frame[0] * 0x3000 + 0x8000;
		audio_buffer[offset + i + 1] = frame[1] * 0x3000 + 0x8000;
	}
}


void init_audio(void) {

	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
//	EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, 70, MIXRATE);
	EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, 60, MIXRATE);

	EVAL_AUDIO_Play(audio_buffer, BUFFER_SIZE);
/*
	while (1) {
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		delay_ms(200);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		delay_ms(200);
	}
*/

}
void EVAL_AUDIO_HalfTransfer_CallBack(__attribute__((unused)) uint32_t pBuffer, __attribute__((unused)) uint32_t Size) {
	GPIO_SetBits(GPIOD, GPIO_Pin_15);
	fill_audio_buffer(0, BUFFER_SIZE/2);
}
void EVAL_AUDIO_TransferComplete_CallBack(__attribute__((unused)) uint32_t pBuffer, __attribute__((unused)) uint32_t Size) {
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	fill_audio_buffer(BUFFER_SIZE/2, BUFFER_SIZE/2);
}
uint32_t Codec_TIMEOUT_UserCallback(void) { return 0; }
uint16_t EVAL_AUDIO_GetSampleCallBack(void) { return 0; }




void MIDI_recv_cb(MIDI_EventPacket_t packet) {
	//MIDI_send(packet);
	STM_EVAL_LEDToggle(LED_Orange);
	chan.speed = 440.0 / MIXRATE * powf(2, (packet.note - 69) / 12.0);

}


