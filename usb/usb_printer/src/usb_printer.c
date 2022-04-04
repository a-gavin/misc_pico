/**
 * @brief USB Printer, usb_printer.c
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Includes */
#include "defs.h"

/* Prototypes */
void heartbeat(void* not_used);

/* Code */
int main() {
    /* General initialization */
    stdio_init_all();


    /* Create tasks */
    xTaskCreate(heartbeat, "heartbeat", 256, NULL, tskIDLE_PRIORITY, NULL);

    /* Start scheduler */
    vTaskStartScheduler();

    while (1)
        tight_loop_contents();
}

void heartbeat(void* not_used) {
    while (true) {
#ifndef NDEBUG
        printf("flwr tick: %d\n", HEARTBEAT_DELAY);
#endif

        gpio_put(PICO_DEFAULT_LED_PIN, HIGH);
        vTaskDelay(HEARTBEAT_DELAY);

        gpio_put(PICO_DEFAULT_LED_PIN, LOW);
        vTaskDelay(HEARTBEAT_DELAY);
    }
}