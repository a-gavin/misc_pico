/**
 * @brief USB Printer, defs.h
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <task.h>
#include <timers.h>

// Pico
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#define LED_PIN   PICO_DEFAULT_LED_PIN // Unecessary??

// TinyUSB
#include "bsp/board.h"
#include "bsp/rp2040/board.h" // Unecessary??
#include "tusb.h"

// Local
#include "myAssert.h"

/* Macro definitions */
#define LOW                 (0)
#define HIGH                (1)

#define HEARTBEAT_DELAY     500

// Increase stack size when debug log is enabled
#define USBD_STACK_SIZE    (3*configMINIMAL_STACK_SIZE/2) * (CFG_TUSB_DEBUG ? 2 : 1)

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};