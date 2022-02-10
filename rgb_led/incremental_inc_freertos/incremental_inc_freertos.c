/**
 * @brief Incremental LED Fade-in/out PWM FreeRTOS
 * 
 * Copyright (c) 2022 Alex Gavin
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/* Constants */
#define CHANGE_COLOR_PIN        14
#define INC_BRIGHTNESS_PIN      15
#define RED_PIN                 16
#define GREEN_PIN               17
#define BLUE_PIN                18

#define HEARTBEAT_DELAY 500


/* Includes */
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"


/* Globals */
static uint slice_num_red;
static uint slice_num_green;
static uint slice_num_blue;

// LED PWM pulse variables
static int fade = 0;
static bool going_up = false;
static uint pin = RED_PIN; // Start on RED PIN
static bool change_color = false;


/* Prototypes */
void gpio_int_callback(uint gpio, uint32_t events_unused);
void on_pwm_wrap(void);
void hardware_init(void);
void heartbeat(void* unused);


/* Code */
int main()
{
    printf("program start\n");
    stdio_init_all();
    hardware_init();

    printf("create tasks\n");
    xTaskCreate(heartbeat, "LED_Task", 256, NULL, 1, NULL);

    printf("start scheduler\n");
    vTaskStartScheduler();

    while(1)
        tight_loop_contents();
}

/* Interrupt handlers */
void gpio_int_callback(uint gpio, uint32_t events_unused) 
{
    irq_set_enabled(PWM_IRQ_WRAP, false);
    switch (gpio) {
        case CHANGE_COLOR_PIN:
            change_color = true;
            break;
        case INC_BRIGHTNESS_PIN:
            if (going_up) {
                fade += 100;
                if (fade > 255) {
                    fade = 255;
                    going_up = false;
                }
            } else {
                fade -= 100;
                if (fade < 0) {
                    fade = 0;
                    going_up = true;
                }
            }

            // Square fade for better ~aesthetics~
            pwm_set_gpio_level(pin, fade * fade);

            // Switch LED colors when completely faded
            if (change_color) {
                pwm_set_gpio_level(pin, 0);
                switch (pin) {
                    // RED -> GREEN -> BLUE -> wrap and cont...
                    case RED_PIN:
                        pin = GREEN_PIN;
                        pwm_set_enabled(slice_num_red, false);
                        pwm_set_enabled(slice_num_green, true);
                        break;
                    case GREEN_PIN:
                        pin = BLUE_PIN;
                        pwm_set_enabled(slice_num_green, false);
                        pwm_set_enabled(slice_num_blue, true);
                        break;
                    case BLUE_PIN:
                        pin = RED_PIN;
                        pwm_set_enabled(slice_num_blue, false);
                        pwm_set_enabled(slice_num_red, true);
                        break;
                }
                change_color = false;
            }
            break;
        default:
            assert(false);
            break;
    }
    irq_set_enabled(PWM_IRQ_WRAP, true);
}

void on_pwm_wrap() {
    // Clear interrupt
    pwm_clear_irq(pwm_gpio_to_slice_num(pin));
}

/* Handler functions */
void heartbeat(void* notUsed)
{   
    while (true) {
        printf("hb-tick: %d\n", HEARTBEAT_DELAY);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        vTaskDelay(HEARTBEAT_DELAY);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(HEARTBEAT_DELAY);
    }
}


/* Initialization functions */
void hardware_init(void)
{
    printf("hardware init\n");

    // GPIO CHANGE_COLOR pin
    printf("init CHANGE_COLOR_PIN\n");
    gpio_init(CHANGE_COLOR_PIN);
    gpio_pull_up(CHANGE_COLOR_PIN);
    gpio_set_dir(CHANGE_COLOR_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(CHANGE_COLOR_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_int_callback);

    // GPIO INC_BRIGHTNESS pin
    printf("init INC_BRIGHTNESS_PIN\n");
    gpio_init(INC_BRIGHTNESS_PIN);
    gpio_pull_up(INC_BRIGHTNESS_PIN);
    gpio_set_dir(INC_BRIGHTNESS_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(INC_BRIGHTNESS_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_int_callback);

    // GPIO pin 16
    printf("init RED_PIN\n");
    gpio_init(RED_PIN);
    gpio_set_dir(RED_PIN, GPIO_OUT);

    gpio_set_function(RED_PIN, GPIO_FUNC_PWM);
    slice_num_red = pwm_gpio_to_slice_num(RED_PIN);
    pwm_clear_irq(slice_num_red);
    pwm_set_irq_enabled(slice_num_red, true);

    // GPIO pin 17
    printf("init GREEN_PIN\n");
    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);

    gpio_set_function(GREEN_PIN, GPIO_FUNC_PWM);
    slice_num_green = pwm_gpio_to_slice_num(GREEN_PIN);
    pwm_clear_irq(slice_num_green);
    pwm_set_irq_enabled(slice_num_green, true);

    // GPIO pin 18
    printf("init BLUE_PIN\n");
    gpio_init(BLUE_PIN);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);

    gpio_set_function(BLUE_PIN, GPIO_FUNC_PWM);
    slice_num_blue = pwm_gpio_to_slice_num(BLUE_PIN);
    pwm_clear_irq(slice_num_blue);
    pwm_set_irq_enabled(slice_num_blue, true);

    // General PWM setup
    printf("init PWM\n");
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);

    // Assume start with RED pin, init others to disabled
    pwm_init(slice_num_green, &config, false);
    pwm_init(slice_num_blue, &config, false);
    pwm_init(slice_num_red, &config, true);
}