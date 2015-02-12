#include "main.h"

#include "mcugui/rect.h"
#include "libs/armmath.h"
#include "pixel_hal.h"
#include "GLCD.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE		USB_OTG_Core_dev __ALIGN_END;
__ALIGN_BEGIN USBH_HOST					USB_Host __ALIGN_END;


static __IO uint32_t TimingDelay;

void delay_ms(__IO uint32_t nTime) {
	TimingDelay = nTime * 10;
	while (TimingDelay != 0) {}
}

void TimingDelay_Decrement(void) {
	if (TimingDelay > 0) TimingDelay--;
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
	
	clearDisplay();

	LCD_Initializtion();
	

	printf("usb-init\n");
	USBH_Init(&USB_OTG_Core_dev,
			USB_OTG_FS_CORE_ID,
			&USB_Host,
			&MIDI_cb,
			&USR_Callbacks);
	printf("usb-init done\n");


	init_audio();

	usb_ready = 1;
	
	while (1) {
		draw_filledRect(randr(0,320),randr(0,240),randr(50,100),randr(50,100),randr(0,255),randr(0,255),randr(0,255));
		sync_frame();
	}

	while (1) {
		draw_filledRect(randr(0,320),randr(0,240),randr(50,100),randr(50,100),randr(0,255),randr(0,255),randr(0,255));
		sync_frame();
	}
}



enum {
	BUFFER_SIZE = 640,
	MAX_VOLUME = 87,
	MIXRATE = I2S_AudioFreq_48k
};

enum { RELEASE, ATTACK, HOLD, };

typedef struct {
	uint8_t	note;
	uint8_t	state;
	float	level;
	float	phase;
	float	speed;
	float	pulsewidth;
} Channel;

Channel chan = {
	0,
	ATTACK,
	0,
	0,
	440.0 / MIXRATE,
	0.5
};

void mix(float frame[2]) {

	// pwm
	//chan.pulsewidth += 0.00001;
	//chan.pulsewidth -= (int) chan.pulsewidth;

	// osc
	chan.phase += chan.speed;
	chan.phase -= (int) chan.phase;
	float amp = chan.phase < chan.pulsewidth ? -1 : 1;


	// env
	float attack	= 0.005;
	float sustain	= 0.7;
	float decay		= 0.9999;
	float release	= 0.9998;

	switch (chan.state) {
	case ATTACK:
		chan.level += attack;
		if (chan.level > 1) {
			chan.level = 1;
			chan.state = HOLD;
		}
		break;
	case HOLD:
		chan.level = sustain + (chan.level - sustain) * decay;
		break;
	case RELEASE:
	default:
		chan.level *= release;
		break;
	}

	amp *= chan.level;

	frame[0] = amp;
	frame[1] = amp;
}



uint16_t audio_buffer[BUFFER_SIZE] = {};
static void fill_audio_buffer(uint16_t offset, uint16_t length) {
	float frame[2];
	for (int i = 0; i < length; i += 2) {
		mix(frame);
		audio_buffer[offset + i + 0] = (uint16_t)((int16_t)(20000.0 * frame[0]));
		audio_buffer[offset + i + 1] = (uint16_t)((int16_t)(20000.0 * frame[1]));
	}
}


void init_audio(void) {
	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
	EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, 70, MIXRATE);
	EVAL_AUDIO_Play(audio_buffer, BUFFER_SIZE);
}

void EVAL_AUDIO_HalfTransfer_CallBack(__attribute__((unused)) uint32_t pBuffer, __attribute__((unused)) uint32_t Size) {
	fill_audio_buffer(0, BUFFER_SIZE/2);
}

void EVAL_AUDIO_TransferComplete_CallBack(__attribute__((unused)) uint32_t pBuffer, __attribute__((unused)) uint32_t Size) {
	fill_audio_buffer(BUFFER_SIZE/2, BUFFER_SIZE/2);
}

uint32_t Codec_TIMEOUT_UserCallback(void) { return 0; }
uint16_t EVAL_AUDIO_GetSampleCallBack(void) { return 0; }

void MIDI_recv_cb(MIDI_EventPacket_t packet) {
	//MIDI_send(packet);
	STM_EVAL_LEDToggle(LED_Orange);

	if (packet.type == NoteOn) {
		chan.state = ATTACK;
		chan.level = 0;
		chan.note = packet.data1;
		chan.speed = (440.0 / MIXRATE) * powf(2, (chan.note  - 69) * (1 / 12.0));
	}
	if (packet.type == NoteOff && packet.data1 == chan.note) {
		chan.state = RELEASE;
	}
}


