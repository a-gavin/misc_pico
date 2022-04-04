/**
 * @brief USB Printer, defs.h
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

// Pico
#include "hardware/gpio.h"
#include "pico/stdlib.h"

// TinyUSB
#include "bsp/board.h"
#include "tusb.h"

// Local
#include "myAssert.h"
#include "usb_descriptors.h"

/* Macro definitions */
#define LOW                 (0)
#define HIGH                (1)

#define HEARTBEAT_DELAY     500

/* Globals */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];