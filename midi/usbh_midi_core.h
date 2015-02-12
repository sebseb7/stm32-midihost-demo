#pragma once

#include "usbh_core.h"
#include "usbh_stdreq.h"
#include "usb_bsp.h"
#include "usbh_ioreq.h"
#include "usbh_hcs.h"
#include "usbh_usr.h"

typedef enum
{
	MIDI_DATA=0,
	MIDI_POLL,
	MIDI_ERROR
}
MIDI_State_t;

typedef struct _MIDI_Process
{
	MIDI_State_t	state_out;
	MIDI_State_t	state_in;
	uint8_t			buff_in[USBH_MIDI_MPS_SIZE];
	uint8_t			buff_out[USBH_MIDI_MPS_SIZE];
	uint8_t			hc_num_in;
	uint8_t 		hc_num_out;
	uint8_t			MIDIBulkOutEp;
	uint8_t			MIDIBulkInEp;
	uint16_t		MIDIBulkInEpSize;
	uint16_t		MIDIBulkOutEpSize;
}
MIDI_Machine_t;


typedef enum {
	NoteOff       = 0x8,
	NoteOn        = 0x9,
	PolyPressure  = 0xa,
	CC            = 0xb,
	ProgramChange = 0xc,
	Aftertouch    = 0xd,
	PitchBend     = 0xe
} midi_event_t;

typedef union {
	uint32_t all;
	struct {
		union {
			uint8_t status;
			struct {
				uint8_t channel:4;
				uint8_t type:4;
			};
		};
		uint8_t data1;
		uint8_t data2;
	};
} MIDI_EventPacket_t;

extern USBH_Class_cb_TypeDef  MIDI_cb;

int MIDI_send(MIDI_EventPacket_t packet);
void MIDI_recv_cb(MIDI_EventPacket_t packet);

