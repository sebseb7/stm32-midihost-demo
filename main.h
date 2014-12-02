#ifndef __MAIN_H
#define __MAIN_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_midi_core.h"

#include "my_stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32fxxx_it.h"

void USB_Host_Handle(void);
void TimingDelay_Decrement(void);
void delay_ms(__IO uint32_t nTime);
#endif
