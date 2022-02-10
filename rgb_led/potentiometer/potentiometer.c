/**
 * @brief Potentiometer experimentation
 * 
 * Copyright (c) 2022 Alex Gavin
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/* Defines */
#define SW1_PIN          15
#define RED_PIN         16
#define GREEN_PIN       17
#define BLUE_PIN        18
#define LED_PIN         25
#define ADC_GPIO_PIN    26

#define ADC_INPUT       0

#define HEARTBEAT_DELAY 500
#define SAMPLE_DELAY    100

/* Includes */
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

/* Globals */
uint slice_num_red;
uint slice_num_green;
uint slice_num_blue;

uint16_t gpio_pwm_level;
uint cur_led_pin = RED_PIN;

/* Prototypes */
void gpio_int_callback(uint gpio, uint32_t events_unused);
void heartbeat(void* unused);
void change_brightness(void* unused);
void hardware_init(void);

/* Code */
int main()
{
    printf("program start\n");
    stdio_init_all();
    hardware_init();

    printf("create tasks\n");
    xTaskCreate(change_brightness, "CHANGE_BRIGHTNESS_task", 256, NULL, 1, NULL);
    xTaskCreate(heartbeat, "LED_Task", 256, NULL, tskIDLE_PRIORITY, NULL);

    printf("start scheduler\n");
    vTaskStartScheduler();

    while(1)
        tight_loop_contents();
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

void change_brightness(void* notUsed)
{   
    while (true) {
        uint16_t adc_raw_sample = adc_read();
        if (adc_raw_sample < 30) {
            adc_raw_sample = 0;
        }

        printf("ADC raw value: %d\n", adc_raw_sample);

        gpio_pwm_level = adc_raw_sample << 4;
        pwm_set_gpio_level(cur_led_pin, gpio_pwm_level);
        vTaskDelay(SAMPLE_DELAY);
    }
}

/* Interrupt functions */
void gpio_int_callback(uint gpio, uint32_t events_unused) {
    printf("in gpio callback\n");
    if (gpio == SW1_PIN) {
        pwm_set_gpio_level(cur_led_pin, 0);

        switch (cur_led_pin) {
            // RED -> GREEN -> BLUE -> wrap and cont...
            case RED_PIN:
                cur_led_pin = GREEN_PIN;
                break;
            case GREEN_PIN:
                cur_led_pin = BLUE_PIN;
                break;
            case BLUE_PIN:
                cur_led_pin = RED_PIN;
                break;
        }

        pwm_set_gpio_level(cur_led_pin, gpio_pwm_level);
    }
}

/* Initialization functions */
void hardware_init(void)
{
    printf("hardware init\n");

    // GPIO built-in LED pin
    printf("init PICO_DEFAULT_LED_PIN\n");
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // GPIO SW pin
    gpio_init(SW1_PIN);
    gpio_pull_up(SW1_PIN);
    gpio_set_dir(SW1_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(SW1_PIN, GPIO_IRQ_EDGE_FALL, true, gpio_int_callback);

    // GPIO RED pin
    gpio_init(RED_PIN);
    gpio_set_dir(RED_PIN, GPIO_OUT);

    gpio_set_function(RED_PIN, GPIO_FUNC_PWM);
    slice_num_red = pwm_gpio_to_slice_num(RED_PIN);

    // GPIO GREEN pin
    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);

    gpio_set_function(GREEN_PIN, GPIO_FUNC_PWM);
    slice_num_green = pwm_gpio_to_slice_num(GREEN_PIN);

    // GPIO BLUE pin
    gpio_init(BLUE_PIN);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);

    gpio_set_function(BLUE_PIN, GPIO_FUNC_PWM);
    slice_num_blue = pwm_gpio_to_slice_num(BLUE_PIN);

    // General PWM setup
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);

    // Assume start with RED pin, init others to disabled
    pwm_init(slice_num_green, &config, true);
    pwm_init(slice_num_blue, &config, true);
    pwm_init(slice_num_red, &config, true);

    // ADC init
    adc_init();
    adc_gpio_init(ADC_GPIO_PIN);
    adc_select_input(ADC_INPUT);
}