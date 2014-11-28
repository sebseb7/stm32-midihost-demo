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
//#include "usbh_midi_controller.h"


/** @defgroup USBH_MIDI_CORE_Private_Variables
 * @{
 */
extern USB_OTG_CORE_HANDLE  USB_OTG_dev;
#define MIOS32_USB_MIDI_DATA_IN_SIZE           64
#define MIOS32_USB_MIDI_DATA_OUT_SIZE          64
static uint32_t USB_rx_buffer[MIOS32_USB_MIDI_DATA_OUT_SIZE/4];
static uint32_t USB_tx_buffer[MIOS32_USB_MIDI_DATA_IN_SIZE/4];

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

/*-----------------------------------------------------------------------------------------*/
/**
 * @brief  USBH_MIDI_InterfaceInit
 *         The function init the MIDI class.
 * @param  pdev: Selected device
 * @param  hdev: Selected device property
 * @retval  USBH_Status :Response for USB MIDI driver intialization
 */
static u8  USBH_hc_num_in;
static u8  USBH_hc_num_out;
static u8  USBH_BulkOutEp;
static u8  USBH_BulkInEp;
static u8  USBH_BulkInEpSize;
static u8  USBH_tx_count;
static u16 USBH_BulkOutEpSize;

typedef enum {
  USBH_MIDI_IDLE,
  USBH_MIDI_RX,
  USBH_MIDI_TX,
} USBH_MIDI_transfer_state_t;

static USBH_MIDI_transfer_state_t USBH_MIDI_transfer_state;

#define MIOS32_USB_MIDI_RX_BUFFER_SIZE   64 // packages
#define MIOS32_USB_MIDI_TX_BUFFER_SIZE   64 // packages

// Rx buffer
static u32 rx_buffer[MIOS32_USB_MIDI_RX_BUFFER_SIZE];
static volatile u16 rx_buffer_tail;
static volatile u16 rx_buffer_head;
static volatile u16 rx_buffer_size;
static volatile u8 rx_buffer_new_data;

// Tx buffer
static u32 tx_buffer[MIOS32_USB_MIDI_TX_BUFFER_SIZE];
static volatile u16 tx_buffer_tail;
static volatile u16 tx_buffer_head;
static volatile u16 tx_buffer_size;
static volatile u8 tx_buffer_busy;

// transfer possible?
static u8 transfer_possible = 0;

s32 MIOS32_USB_MIDI_ChangeConnectionState(u8 connected)
{
  // in all cases: re-initialize USB MIDI driver
  // clear buffer counters and busy/wait signals again (e.g., so that no invalid data will be sent out)
  rx_buffer_tail = rx_buffer_head = rx_buffer_size = 0;
  rx_buffer_new_data = 0; // no data received yet
  tx_buffer_tail = tx_buffer_head = tx_buffer_size = 0;

  if( connected ) {
    transfer_possible = 1;
    tx_buffer_busy = 0; // buffer not busy anymore

    USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
  } else {
    // cable disconnected: disable transfers
    transfer_possible = 0;
    tx_buffer_busy = 1; // buffer busy
  }

  return 0; // no error
}

static USBH_Status USBH_MIDI_InterfaceInit ( USB_OTG_CORE_HANDLE *pdev, void *phost)
{
  USBH_HOST *pphost = phost;

  MIOS32_USB_MIDI_ChangeConnectionState(0);

  int i;
  for(i=0; i<pphost->device_prop.Cfg_Desc.bNumInterfaces && i < USBH_MAX_NUM_INTERFACES; ++i) {
    //MIOS32_MIDI_DebugPortSet(UART0);
    //MIOS32_MIDI_SendDebugMessage("InterfaceInit %d %d %d", i, pphost->device_prop.Itf_Desc[i].bInterfaceClass, pphost->device_prop.Itf_Desc[i].bInterfaceSubClass);

    if( (pphost->device_prop.Itf_Desc[i].bInterfaceClass == 1) &&
	(pphost->device_prop.Itf_Desc[i].bInterfaceSubClass == 3) ) {

      if( pphost->device_prop.Ep_Desc[i][0].bEndpointAddress & 0x80 ) {
	USBH_BulkInEp = (pphost->device_prop.Ep_Desc[i][0].bEndpointAddress);
	USBH_BulkInEpSize  = pphost->device_prop.Ep_Desc[i][0].wMaxPacketSize;
      } else {
	USBH_BulkOutEp = (pphost->device_prop.Ep_Desc[i][0].bEndpointAddress);
	USBH_BulkOutEpSize  = pphost->device_prop.Ep_Desc[i] [0].wMaxPacketSize;
      }

      if( pphost->device_prop.Ep_Desc[i][1].bEndpointAddress & 0x80 ) {
	USBH_BulkInEp = (pphost->device_prop.Ep_Desc[i][1].bEndpointAddress);
	USBH_BulkInEpSize  = pphost->device_prop.Ep_Desc[i][1].wMaxPacketSize;
      } else {
	USBH_BulkOutEp = (pphost->device_prop.Ep_Desc[i][1].bEndpointAddress);
	USBH_BulkOutEpSize  = pphost->device_prop.Ep_Desc[i][1].wMaxPacketSize;
      }

      USBH_hc_num_out = USBH_Alloc_Channel(pdev, USBH_BulkOutEp);
      USBH_hc_num_in = USBH_Alloc_Channel(pdev, USBH_BulkInEp);

      /* Open the new channels */
      USBH_Open_Channel(pdev,
			USBH_hc_num_out,
			pphost->device_prop.address,
			pphost->device_prop.speed,
			EP_TYPE_BULK,
			USBH_BulkOutEpSize);

      USBH_Open_Channel(pdev,
			USBH_hc_num_in,
			pphost->device_prop.address,
			pphost->device_prop.speed,
			EP_TYPE_BULK,
			USBH_BulkInEpSize);

      MIOS32_USB_MIDI_ChangeConnectionState(1);
      break;
    }
	else
	{
    	pphost->usr_cb->DeviceNotSupported();
	}
  }
	
  return USBH_OK;

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
  
  if( USBH_hc_num_out ) {
    USB_OTG_HC_Halt(pdev, USBH_hc_num_out);
    USBH_Free_Channel  (pdev, USBH_hc_num_out);
    USBH_hc_num_out = 0;     /* Reset the Channel as Free */
  }

  if( USBH_hc_num_in ) {
    USB_OTG_HC_Halt(pdev, USBH_hc_num_in);
    USBH_Free_Channel  (pdev, USBH_hc_num_in);
    USBH_hc_num_in = 0;     /* Reset the Channel as Free */
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
static USBH_Status USBH_MIDI_ClassRequest(USB_OTG_CORE_HANDLE *pdev ,
		void *phost)
{
  USBH_Status status = USBH_OK;
  return status;
}

void MIDI_send(USB_OTG_CORE_HANDLE *pdev)
{

	uint8_t *buff = {176,32,127};

	USBH_Status status = USBH_BulkSendData ( pdev, buff, 3,MIDI_Machine.hc_num_out);
}


static uint32_t nested_ctr;

// stored priority level before IRQ has been disabled (important for co-existence with vPortEnterCritical)
static uint32_t prev_primask;


void IRQ_Disable(void)
{
	// get current priority if nested level == 0
	if( !nested_ctr ) {
		__asm volatile (			   \
				"	mrs %0, primask\n" \
				: "=r" (prev_primask)  \
				);
	}

	// disable interrupts
	__asm volatile ( \
			"	mov r0, #1     \n" \
			"	msr primask, r0\n" \
			:::"r0"	 \
			);

	++nested_ctr;

}

int32_t IRQ_Enable(void)
{
	// check for nesting error
	if( nested_ctr == 0 )
		return -1; // nesting error

	// decrease nesting level
	--nested_ctr;

	// set back previous priority once nested level reached 0 again
	if( nested_ctr == 0 ) {
		__asm volatile ( \
				"	msr primask, %0\n" \
				:: "r" (prev_primask)  \
				);
	}

	return 0; // no error
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
	if( transfer_possible ) {
		USBH_HOST *pphost = phost;

		if( HCD_IsDeviceConnected(pdev) ) {

//			start_LED_On2(LED_Red, 8)	;
			u8 force_rx_req = 0;
					

			if( USBH_MIDI_transfer_state == USBH_MIDI_TX ) {
				URB_STATE URB_State = HCD_GetURB_State(pdev, USBH_hc_num_out);
				
				start_LED_On2(LED_Red, 8)	;
					

				if( URB_State == URB_IDLE ) {
					// wait...
					
					// why does this happen? it hangs ...
					STM_EVAL_LEDOn(LED_Orange);
					//USBH_BulkSendData(&USB_OTG_dev, (u8 *)USB_tx_buffer, USBH_tx_count, USBH_hc_num_out);
					USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
				} else if( URB_State == URB_DONE ) {
					STM_EVAL_LEDOff(LED_Orange);
					USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
				} else if( URB_State == URB_STALL ) {
					// Issue Clear Feature on OUT endpoint
					if( USBH_ClrFeature(pdev, pphost, USBH_BulkOutEp, USBH_hc_num_out) == USBH_OK ) {
						USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
					}
				} else if( URB_State == URB_NOTREADY ) {
					// send again
					USBH_BulkSendData(&USB_OTG_dev, (u8 *)USB_tx_buffer, USBH_tx_count, USBH_hc_num_out);
				} else if( URB_State == URB_ERROR ) {
					USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
				}
			} else if( USBH_MIDI_transfer_state == USBH_MIDI_RX ) {
				URB_STATE URB_State = HCD_GetURB_State(pdev, USBH_hc_num_in);
				if( URB_State == URB_IDLE || URB_State == URB_DONE ) {
					// data received from receive
					//u32 count = HCD_GetXferCnt(pdev, USBH_hc_num_in) / 4;
					// Note: HCD_GetXferCnt returns a counter which isn't zeroed immediately on a USBH_BulkReceiveData() call
					u32 count = USB_OTG_dev.host.hc[USBH_hc_num_in].xfer_count / 4;

					// push data into FIFO
					if( !count ) {
						USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
					} else if( count < (MIOS32_USB_MIDI_RX_BUFFER_SIZE-rx_buffer_size) ) {
						u32 *buf_addr = (u32 *)USB_rx_buffer;

						// copy received packages into receive buffer
						// this operation should be atomic
						IRQ_Disable();
						do {
	
	
							//MIDI_Decode(MIDI_Machine.buff);

							start_LED_On(LED_Blue, 8)	;
	
							mios32_midi_package_t package;
							package.ALL = *buf_addr++;

							//if( MIOS32_MIDI_SendPackageToRxCallback(USB0 + package.cable, package) == 0 ) 
					/*		{
								rx_buffer[rx_buffer_head] = package.ALL;

								if( ++rx_buffer_head >= MIOS32_USB_MIDI_RX_BUFFER_SIZE )
									rx_buffer_head = 0;
								++rx_buffer_size;
							}*/
						} while( --count > 0 );
						IRQ_Enable();
							

						USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
						force_rx_req = 1;
					}
				} else if( URB_State == URB_STALL ) {
					// Issue Clear Feature on IN endpoint
					if( USBH_ClrFeature(pdev, pphost, USBH_BulkInEp, USBH_hc_num_in) == USBH_OK ) {
						USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
					}
				} else if( URB_State == URB_ERROR || URB_State == URB_NOTREADY ) {
					USBH_MIDI_transfer_state = USBH_MIDI_IDLE;
				}
			}


			if( USBH_MIDI_transfer_state == USBH_MIDI_IDLE ) {
				if( !force_rx_req && tx_buffer_size && transfer_possible ) {
					// atomic operation to avoid conflict with other interrupts
					IRQ_Disable();

					s16 count = (tx_buffer_size > (USBH_BulkOutEpSize/4)) ? (USBH_BulkOutEpSize/4) : tx_buffer_size;

					// send to IN pipe
					tx_buffer_size -= count;

					u32 *buf_addr = (u32 *)USB_tx_buffer;
					int i;
					for(i=0; i<count; ++i) {
						*(buf_addr++) = tx_buffer[tx_buffer_tail];
						if( ++tx_buffer_tail >= MIOS32_USB_MIDI_TX_BUFFER_SIZE )
							tx_buffer_tail = 0;
					}


					USBH_tx_count = count * 4;
					USBH_BulkSendData(&USB_OTG_dev, (u8 *)USB_tx_buffer, USBH_tx_count, USBH_hc_num_out);

					USBH_MIDI_transfer_state = USBH_MIDI_TX;

					IRQ_Enable();
				} else {
					// request data from device
					USBH_BulkReceiveData(&USB_OTG_dev, (u8 *)USB_rx_buffer, USBH_BulkInEpSize, USBH_hc_num_in);
					USBH_MIDI_transfer_state = USBH_MIDI_RX;
				}
			}
		}
	}

	return USBH_OK;

	/*

	   USBH_HOST *pphost = phost;
	   USBH_Status status = USBH_OK;

	//uint8_t appliStatus = 0;
	//USBH_Status status = USBH_BUSY;


	if(HCD_IsDeviceConnected(pdev))
	{
	//appliStatus = pphost->usr_cb->UserApplication(); // this will call USBH_USR_MIDI_Application()

	switch (MIDI_Machine.state)
	{

	case MIDI_GET_DATA:

	USBH_BulkReceiveData( &USB_OTG_Core_dev, MIDI_Machine.buff, 64, MIDI_Machine.hc_num_in);
	start_toggle = 1;
	MIDI_Machine.state = MIDI_POLL;
	//STM_EVAL_LEDOn(LED_Blue);

	break;

	case MIDI_POLL:

	if(HCD_GetURB_State(pdev , MIDI_Machine.hc_num_in) <= URB_DONE)
	{
	if(start_toggle == 1)  
	{
	start_toggle = 0;
	//MIDI_Machine.cb->Decode(MIDI_Machine.buff);
	MIDI_Decode(MIDI_Machine.buff);
	MIDI_Machine.buff[1] = 0; // the whole buffer should be cleaned...

	}
	MIDI_Machine.state = MIDI_GET_DATA;
	}
	else if(HCD_GetURB_State(pdev, MIDI_Machine.hc_num_in) == URB_STALL) 
	{

	if( (USBH_ClrFeature(pdev,
	pphost,
	MIDI_Machine.MIDIBulkInEp,
	MIDI_Machine.hc_num_in)) == USBH_OK)
	{
	MIDI_Machine.state = MIDI_GET_DATA;

	}

	}
	break;

	default:
	break;
	}

	}

	return status;
	*/

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

/* Receive data from MIDI device */

/*
uint8_t MIDI_RcvData(uint8_t *outBuf)
{
	URB_STATE urb_status;
	urb_status = HCD_GetURB_State( &USB_OTG_Core_dev , MIDI_Machine.hc_num_in);

	if(urb_status <= URB_DONE)
	{
		USBH_BulkReceiveData( &USB_OTG_Core_dev, MIDI_Machine.buff, 64, MIDI_Machine.hc_num_in);
		if( MIDI_Machine.buff[0] == 0 && MIDI_Machine.buff[1] == 0 && MIDI_Machine.buff[2] == 0 && MIDI_Machine.buff[3] == 0 )
			return 0;
		outBuf[0] = MIDI_Machine.buff[1];
		outBuf[1] = MIDI_Machine.buff[2];
		outBuf[2] = MIDI_Machine.buff[3];
		return MIDI_lookupMsgSize(MIDI_Machine.buff[1]);
	}
	else return 0;

}

*/


/*-----------------------------------------------------------------------------------------*/
s32 USB_MIDI_PackageSend_NonBlocking(mios32_midi_package_t package)
{

// device available?
  if( !transfer_possible )
    return -1;

  // buffer full?
  if( tx_buffer_size >= (MIOS32_USB_MIDI_TX_BUFFER_SIZE-1) ) {
    // call USB handler, so that we are able to get the buffer free again on next execution
    // (this call simplifies polling loops!)
    //MIOS32_USB_MIDI_TxBufferHandler();

    // device still available?
    // (ensures that polling loop terminates if cable has been disconnected)
    if( !transfer_possible )
      return -1;

    // notify that buffer was full (request retry)
    return -2;
  }

  // put package into buffer - this operation should be atomic!
	IRQ_Disable();
  tx_buffer[tx_buffer_head++] = package.ALL;
  if( tx_buffer_head >= MIOS32_USB_MIDI_TX_BUFFER_SIZE )
    tx_buffer_head = 0;
  ++tx_buffer_size;
	IRQ_Enable();

  return 0;
}


/*****************************END OF FILE****/
