#include "usbh_midi_core.h"
#include <string.h>


extern USB_OTG_CORE_HANDLE	USB_OTG_Core_dev;
MIDI_Machine_t				MIDI_Machine;
USB_Setup_TypeDef			MIDI_Setup;


static USBH_Status 	USBH_MIDI_InterfaceInit(USB_OTG_CORE_HANDLE *pdev , void *phost);
static void 		USBH_MIDI_InterfaceDeInit(USB_OTG_CORE_HANDLE *pdev , void *phost);
static USBH_Status 	USBH_MIDI_ClassRequest(USB_OTG_CORE_HANDLE *pdev , void *phost);
static USBH_Status 	USBH_MIDI_Handle(USB_OTG_CORE_HANDLE *pdev , void *phost);

USBH_Class_cb_TypeDef  MIDI_cb = {
		USBH_MIDI_InterfaceInit,
		USBH_MIDI_InterfaceDeInit,
		USBH_MIDI_ClassRequest,
		USBH_MIDI_Handle
};


#define SEND_BUFFER_SIZE 128 // (8, 16, 32, 64 ...)
#define SEND_BUFFER_MASK (SEND_BUFFER_SIZE-1)

static struct {
	MIDI_EventPacket_t event[SEND_BUFFER_SIZE];
	uint8_t read;
	uint8_t write;
} send_buffer;


/*-----------------------------------------------------------------------------------------*/
/**
 * @brief  USBH_MIDI_InterfaceInit
 *         The function init the MIDI class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval  USBH_Status :Response for USB MIDI driver intialization
 */
static USBH_Status USBH_MIDI_InterfaceInit(USB_OTG_CORE_HANDLE *pdev, void *phost) {

	USBH_HOST *pphost = phost;
	USBH_Status status = USBH_BUSY;
	MIDI_Machine.state_out = MIDI_ERROR;
	MIDI_Machine.state_in = MIDI_ERROR;

	#define INTERFACE 1

	printf("class %d-%d\n",
		pphost->device_prop.Itf_Desc[INTERFACE].bInterfaceClass,
		pphost->device_prop.Itf_Desc[INTERFACE].bInterfaceSubClass);



	if ((pphost->device_prop.Itf_Desc[INTERFACE].bInterfaceClass == USB_AUDIO_CLASS) && \
	    (pphost->device_prop.Itf_Desc[INTERFACE].bInterfaceSubClass == USB_MIDISTREAMING_SubCLASS))
	{
		if(pphost->device_prop.Ep_Desc[INTERFACE][0].bEndpointAddress & 0x80)
		{
			MIDI_Machine.MIDIBulkInEp = (pphost->device_prop.Ep_Desc[INTERFACE][0].bEndpointAddress);
			MIDI_Machine.MIDIBulkInEpSize  = pphost->device_prop.Ep_Desc[INTERFACE][0].wMaxPacketSize;
		}
		else
		{
			MIDI_Machine.MIDIBulkOutEp = (pphost->device_prop.Ep_Desc[INTERFACE][0].bEndpointAddress);
			MIDI_Machine.MIDIBulkOutEpSize  = pphost->device_prop.Ep_Desc[INTERFACE][0].wMaxPacketSize;
		}

		if(pphost->device_prop.Ep_Desc[INTERFACE][1].bEndpointAddress & 0x80)

		{
			MIDI_Machine.MIDIBulkInEp = (pphost->device_prop.Ep_Desc[INTERFACE][1].bEndpointAddress);
			MIDI_Machine.MIDIBulkInEpSize  = pphost->device_prop.Ep_Desc[INTERFACE][1].wMaxPacketSize;
		}
		else
		{
			MIDI_Machine.MIDIBulkOutEp = (pphost->device_prop.Ep_Desc[INTERFACE][1].bEndpointAddress);
			MIDI_Machine.MIDIBulkOutEpSize  = pphost->device_prop.Ep_Desc[INTERFACE][1].wMaxPacketSize;
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
		status = USBH_OK;

		send_buffer.read=0;
		send_buffer.write=0;

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
		__attribute__((unused)) void *phost)
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
static USBH_Status USBH_MIDI_ClassRequest(__attribute__((unused)) USB_OTG_CORE_HANDLE *pdev ,
		__attribute__((unused)) void *phost)
{
	USBH_Status status = USBH_OK ;

	return status;
}


int MIDI_send(MIDI_EventPacket_t packet)
{
	uint8_t next = ((send_buffer.write + 1) & SEND_BUFFER_MASK);
	if (send_buffer.read != next)
	{
		send_buffer.event[send_buffer.write & SEND_BUFFER_MASK] = packet;
		send_buffer.write = next;
		return 1;
	}
	return 0;
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

				memset(MIDI_Machine.buff_in,0,USBH_MIDI_MPS_SIZE);
				USBH_BulkReceiveData( &USB_OTG_Core_dev, MIDI_Machine.buff_in, 64, MIDI_Machine.hc_num_in);
				MIDI_Machine.state_in = MIDI_POLL;

				break;


			case MIDI_POLL:


				if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_in) == URB_IDLE)
				{
				}
				else if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_in) == URB_DONE)
				{
					int i = 1;
					while((i < USBH_MIDI_MPS_SIZE) && (MIDI_Machine.buff_in[i] != 0))
					{
						//printf("recv (%i) %i %i %i %i\n",i,MIDI_Machine.buff_in[i-1],MIDI_Machine.buff_in[i],MIDI_Machine.buff_in[i+1],MIDI_Machine.buff_in[i+2]);
						MIDI_EventPacket_t packet;
						packet.status = MIDI_Machine.buff_in[i];
						packet.data1 = MIDI_Machine.buff_in[i+1];
						packet.data2 = MIDI_Machine.buff_in[i+2];

						MIDI_recv_cb(packet);

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
						// Change state to issue next IN token
						MIDI_Machine.state_in = MIDI_DATA;
						//STM_EVAL_LEDToggle(LED_Blue);
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
					MIDI_Machine.buff_out[0] = send_buffer.event[send_buffer.read].type;
					MIDI_Machine.buff_out[1] = send_buffer.event[send_buffer.read].status;
					MIDI_Machine.buff_out[2] = send_buffer.event[send_buffer.read].data1;
					MIDI_Machine.buff_out[3] = send_buffer.event[send_buffer.read].data2;
					send_buffer.read = (send_buffer.read+1) & SEND_BUFFER_MASK;

					//printf("send %i %i %i %i\n",MIDI_Machine.buff_out[0],MIDI_Machine.buff_out[1],MIDI_Machine.buff_out[2],MIDI_Machine.buff_out[3]);

					USBH_BulkSendData( &USB_OTG_Core_dev, MIDI_Machine.buff_out, 4, MIDI_Machine.hc_num_out);
					MIDI_Machine.state_out = MIDI_POLL;
				}

				break;
			case MIDI_POLL:

				if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_out) == URB_DONE)
				{
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

void __attribute__((weak)) MIDI_recv_cb(MIDI_EventPacket_t packet);


