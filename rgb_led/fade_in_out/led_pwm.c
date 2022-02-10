/**
 * @brief LED Fade-in/fade-out PWM
 * 
 * Copyright (c) Alex Gavin
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Defines */
#define RED_PIN         16
#define GREEN_PIN       17
#define BLUE_PIN        18
#define LED_PIN         25

/* Includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

/* Globals */
uint slice_num_red;
uint slice_num_green;
uint slice_num_blue;

/* Prototypes */
void on_pwm_wrap(void);
void hardware_init(void);

/* Code */
int main() 
{
    hardware_init();

    while (1)
        tight_loop_contents();
}

void on_pwm_wrap() {
    static int fade = 0;
    static bool going_up = true;
    // Assume start on RED pin
    static uint pin = RED_PIN;

    // Clear interrupt
    pwm_clear_irq(pwm_gpio_to_slice_num(pin));

    if (going_up) {
        fade += 1;
        if (fade > 255) {
            fade = 255;
            going_up = false;
        }
    } else {
        fade -= 1;
        if (fade < 0) {
            fade = 0;
            going_up = true;
        }
    }

    // Square fade for better ~aesthetics~
    pwm_set_gpio_level(pin, fade * fade);

    // Switch LED colors when completely faded
    if (fade == 0 && going_up) {
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
    }
}

void hardware_init(void)
{
    // GPIO pin 16
    gpio_init(RED_PIN);
    gpio_set_dir(RED_PIN, GPIO_OUT);

    gpio_set_function(RED_PIN, GPIO_FUNC_PWM);
    slice_num_red = pwm_gpio_to_slice_num(RED_PIN);
    pwm_clear_irq(slice_num_red);
    pwm_set_irq_enabled(slice_num_red, true);

    // GPIO pin 17
    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);

    gpio_set_function(GREEN_PIN, GPIO_FUNC_PWM);
    slice_num_green = pwm_gpio_to_slice_num(GREEN_PIN);
    pwm_clear_irq(slice_num_green);
    pwm_set_irq_enabled(slice_num_green, true);

    // GPIO pin 18
    gpio_init(BLUE_PIN);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);

    gpio_set_function(BLUE_PIN, GPIO_FUNC_PWM);
    slice_num_blue = pwm_gpio_to_slice_num(BLUE_PIN);
    pwm_clear_irq(slice_num_blue);
    pwm_set_irq_enabled(slice_num_blue, true);

    // General PWM setup
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);

    // Assume start with RED pin, init others to disabled
    pwm_init(slice_num_green, &config, false);
    pwm_init(slice_num_blue, &config, false);
    pwm_init(slice_num_red, &config, true);
}