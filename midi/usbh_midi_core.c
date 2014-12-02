/**
 ******************************************************************************
 * @file    usbh_midi_core.c
 * @author	Xavier Halgand
 * @version
 * @date
 * @brief   Very basic driver for USB Host MIDI class.
 *
 * @verbatim
 *
 *          ===================================================================
 *                                MIDI Class  Description
 *          ===================================================================
 *
 *
 *  @endverbatim
 *
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
#include "usbh_midi_core.h"
#include <string.h>
//#include "usbh_midi_controller.h"


/** @defgroup USBH_MIDI_CORE_Private_Variables
 * @{
 */
extern USB_OTG_CORE_HANDLE	USB_OTG_Core_dev  ;

MIDI_Machine_t				MIDI_Machine  ;

USB_Setup_TypeDef			MIDI_Setup  ;

//USBH_MIDIDesc_t			MIDI_Desc  ;

__IO uint8_t 				start_toggle = 0;

//int State;

/*-----------------------------------------------------------------------------------------*/

static USBH_Status 	USBH_MIDI_InterfaceInit(USB_OTG_CORE_HANDLE *pdev , void *phost);

static void 		USBH_MIDI_InterfaceDeInit(USB_OTG_CORE_HANDLE *pdev , void *phost);

static USBH_Status 	USBH_MIDI_ClassRequest(USB_OTG_CORE_HANDLE *pdev , void *phost);

static USBH_Status 	USBH_MIDI_Handle(USB_OTG_CORE_HANDLE *pdev , void *phost);


/****************** MIDI interface ****************************/

USBH_Class_cb_TypeDef  MIDI_cb =
{
		USBH_MIDI_InterfaceInit,
		USBH_MIDI_InterfaceDeInit,
		USBH_MIDI_ClassRequest,
		USBH_MIDI_Handle
};


#define SEND_BUFFER_SIZE 128 // (8, 16, 32, 64 ...)
#define SEND_BUFFER_MASK (SEND_BUFFER_SIZE-1)

static struct {
	uint8_t by1[SEND_BUFFER_SIZE];
	uint8_t by2[SEND_BUFFER_SIZE];
	uint8_t by3[SEND_BUFFER_SIZE];
	uint8_t read;
	uint8_t write; 
} send_buffer = {{}, {},{},0, 0};


/*-----------------------------------------------------------------------------------------*/
/**
 * @brief  USBH_MIDI_InterfaceInit
 *         The function init the MIDI class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval  USBH_Status :Response for USB MIDI driver intialization
 */
static USBH_Status USBH_MIDI_InterfaceInit ( USB_OTG_CORE_HANDLE *pdev,	void *phost)
{

	USBH_HOST *pphost = phost;
	USBH_Status status = USBH_BUSY ;
	MIDI_Machine.state_out = MIDI_ERROR;
	MIDI_Machine.state_in = MIDI_ERROR;


	if((pphost->device_prop.Itf_Desc[0].bInterfaceClass == USB_AUDIO_CLASS) && \
			(pphost->device_prop.Itf_Desc[0].bInterfaceSubClass == USB_MIDISTREAMING_SubCLASS))
	{
		if(pphost->device_prop.Ep_Desc[0][0].bEndpointAddress & 0x80)
		{
			MIDI_Machine.MIDIBulkInEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
			MIDI_Machine.MIDIBulkInEpSize  = pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
		}
		else
		{
			MIDI_Machine.MIDIBulkOutEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
			MIDI_Machine.MIDIBulkOutEpSize  = pphost->device_prop.Ep_Desc[0] [0].wMaxPacketSize;
		}

		if(pphost->device_prop.Ep_Desc[0][1].bEndpointAddress & 0x80)

		{
			MIDI_Machine.MIDIBulkInEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
			MIDI_Machine.MIDIBulkInEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;
		}
		else
		{
			MIDI_Machine.MIDIBulkOutEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
			MIDI_Machine.MIDIBulkOutEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;
		}

		MIDI_Machine.hc_num_out = USBH_Alloc_Channel(pdev,
				MIDI_Machine.MIDIBulkOutEp);
		MIDI_Machine.hc_num_in = USBH_Alloc_Channel(pdev,
				MIDI_Machine.MIDIBulkInEp);

		/* Open the new channels */
		USBH_Open_Channel  (pdev,
				MIDI_Machine.hc_num_out,
				pphost->device_prop.address,
				pphost->device_prop.speed,
				EP_TYPE_BULK,
				MIDI_Machine.MIDIBulkOutEpSize);

		USBH_Open_Channel  (pdev,
				MIDI_Machine.hc_num_in,
				pphost->device_prop.address,
				pphost->device_prop.speed,
				EP_TYPE_BULK,
				MIDI_Machine.MIDIBulkInEpSize);

		MIDI_Machine.state_out  = MIDI_DATA;
		MIDI_Machine.state_in  = MIDI_DATA;
		start_toggle =0;
		status = USBH_OK;

	}

	else
	{
		pphost->usr_cb->DeviceNotSupported();
	}

	return status ;

}


/*-----------------------------------------------------------------------------------------*/
/**
 * @brief  USBH_MIDI_InterfaceDeInit
 *         The function DeInit the Host Channels used for the MIDI class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval None
 */
void USBH_MIDI_InterfaceDeInit ( USB_OTG_CORE_HANDLE *pdev,
		void *phost)
{
	if ( MIDI_Machine.hc_num_out)
	{
		USB_OTG_HC_Halt(pdev, MIDI_Machine.hc_num_out);
		USBH_Free_Channel  (pdev, MIDI_Machine.hc_num_out);
		MIDI_Machine.hc_num_out = 0;     /* Reset the Channel as Free */
	}

	if ( MIDI_Machine.hc_num_in)
	{
		USB_OTG_HC_Halt(pdev, MIDI_Machine.hc_num_in);
		USBH_Free_Channel  (pdev, MIDI_Machine.hc_num_in);
		MIDI_Machine.hc_num_in = 0;     /* Reset the Channel as Free */
	}
	start_toggle = 0;
}
/*-----------------------------------------------------------------------------------------*/
/**
 * @brief  USBH_MIDI_ClassRequest
 *         The function is responsible for handling MIDI Class requests
 *         for MIDI class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval  USBH_Status :Response for USB Set Protocol request
 */
static USBH_Status USBH_MIDI_ClassRequest(USB_OTG_CORE_HANDLE *pdev ,
		void *phost)
{
	USBH_Status status = USBH_OK ;

	return status;
}

void MIDI_send(uint8_t by1,uint8_t by2, uint8_t by3)
{

	uint8_t next = ((send_buffer.write + 1) & SEND_BUFFER_MASK);
	if (send_buffer.read != next)
	{
		send_buffer.by1[send_buffer.write & SEND_BUFFER_MASK] = by1;
		send_buffer.by2[send_buffer.write & SEND_BUFFER_MASK] = by2;
		send_buffer.by3[send_buffer.write & SEND_BUFFER_MASK] = by3;
		send_buffer.write = next;
	}
}


/*-----------------------------------------------------------------------------------------*/
/**
 * @brief  USBH_MIDI_Handle
 *         The function is for managing state machine for MIDI data transfers
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval USBH_Status
 */
static USBH_Status USBH_MIDI_Handle(USB_OTG_CORE_HANDLE *pdev ,
		void   *phost)
{
	USBH_HOST *pphost = phost;
	USBH_Status status = USBH_OK;

	//uint8_t appliStatus = 0;
	//USBH_Status status = USBH_BUSY;

	if(HCD_IsDeviceConnected(pdev))
	{
		//appliStatus = pphost->usr_cb->UserApplication(); // this will call USBH_USR_MIDI_Application()


		//todo:
		//
		//two state machines one for send one for receive
		//recive urb_idle is received afer send is issued until a packet is received then urb_done is received


		switch (MIDI_Machine.state_in)
		{

			case MIDI_DATA:

				memset(MIDI_Machine.buff,0,USBH_MIDI_MPS_SIZE);
				USBH_BulkReceiveData( &USB_OTG_Core_dev, MIDI_Machine.buff, 64, MIDI_Machine.hc_num_in);
				MIDI_Machine.state_in = MIDI_POLL;

				break;


			case MIDI_POLL:


				if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_in) == URB_IDLE)
				{
				}
				else if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_in) == URB_DONE)
				{
					int i = 1;
					while((i < USBH_MIDI_MPS_SIZE) && (MIDI_Machine.buff[i] != 0))
					{
						// do nothing with the received packet, if it is a button just send it back, to control the LEDs
						if(MIDI_Machine.buff[2] > 23)
						{
							uint8_t next = ((send_buffer.write + 1) & SEND_BUFFER_MASK);
							if (send_buffer.read != next)
							{
								send_buffer.by1[send_buffer.write & SEND_BUFFER_MASK] = MIDI_Machine.buff[i];
								send_buffer.by2[send_buffer.write & SEND_BUFFER_MASK] = MIDI_Machine.buff[i+1];
								send_buffer.by3[send_buffer.write & SEND_BUFFER_MASK] = MIDI_Machine.buff[i+2];
								send_buffer.write = next;
							}
							else
							{
								start_LED_On(LED_Blue, 8);
							}
						}
						i+=4;
					}
					MIDI_Machine.state_in = MIDI_DATA;
				}
				else if(HCD_GetURB_State(pdev, MIDI_Machine.hc_num_in) == URB_STALL) /* IN Endpoint Stalled */
				{

					/* Issue Clear Feature on IN endpoint */
					if( (USBH_ClrFeature(pdev,
									pphost,
									MIDI_Machine.MIDIBulkInEp,
									MIDI_Machine.hc_num_in)) == USBH_OK)
					{
						/* Change state to issue next IN token */
						start_LED_On(LED_Blue, 8);
						MIDI_Machine.state_in = MIDI_DATA;

					}
				}
				break;

			default:
				break;
		}


		switch (MIDI_Machine.state_out)
		{
			case MIDI_DATA:

				if (send_buffer.read != send_buffer.write)
				{
					MIDI_Machine.buff[0] = 0xb;
					MIDI_Machine.buff[1] = send_buffer.by1[send_buffer.read];
					MIDI_Machine.buff[2] = send_buffer.by2[send_buffer.read];; 
					MIDI_Machine.buff[3] = send_buffer.by3[send_buffer.read];; 
					send_buffer.read = (send_buffer.read+1) & SEND_BUFFER_MASK;


					USBH_BulkSendData( &USB_OTG_Core_dev, MIDI_Machine.buff, 4, MIDI_Machine.hc_num_out);
					MIDI_Machine.state_out = MIDI_POLL;
				}

				break;
			case MIDI_POLL:

				if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_out) == URB_DONE)
				{
					//	start_LED_On(LED_Blue, 8);
					MIDI_Machine.state_out = MIDI_DATA;
				}
				else if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_out) == URB_NOTREADY)
				{
					MIDI_Machine.state_out = MIDI_DATA;
				}
				break;

			default:
				break;
		}

	}

	return status;

}
/*-----------------------------------------------------------------------------------------*/
/* look up a MIDI message size from spec */
/*Return */
/* 0 : undefined message */
/* > 0 : valid message size (1-3) */
uint8_t MIDI_lookupMsgSize(uint8_t midiMsg)
{
	uint8_t msgSize = 0;

	if( midiMsg < 0xf0 ) midiMsg &= 0xf0;
	switch(midiMsg) {
		//3 bytes messages
		case 0xf2 : //system common message(SPP)
		case 0x80 : //Note off
		case 0x90 : //Note on
		case 0xa0 : //Poly KeyPress
		case 0xb0 : //Control Change
		case 0xe0 : //PitchBend Change
			msgSize = 3;
			break;

			//2 bytes messages
		case 0xf1 : //system common message(MTC)
		case 0xf3 : //system common message(SongSelect)
		case 0xc0 : //Program Change
		case 0xd0 : //Channel Pressure
			msgSize = 2;
			break;

			//1 bytes messages
		case 0xf8 : //system realtime message
		case 0xf9 : //system realtime message
		case 0xfa : //system realtime message
		case 0xfb : //system realtime message
		case 0xfc : //system realtime message
		case 0xfe : //system realtime message
		case 0xff : //system realtime message
			msgSize = 1;
			break;

			//undefine messages
		default :
			break;
	}
	return msgSize;
}

/*-----------------------------------------------------------------------------------------*/



/*****************************END OF FILE****/
