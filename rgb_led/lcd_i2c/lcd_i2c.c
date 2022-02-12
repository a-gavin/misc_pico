/**
 * @brief Simple I2C LCD program
 * 
 * Copyright (c) 2022 Alex Gavin
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Includes */
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "constants.h"

/* Prototypes */
void hardware_init(void);
void i2c_write_byte(uint8_t val);

void task_heartbeat(void* unused);
void task_print_msg(void* unused);

void lcd_init(void);
void lcd_clear(void);
void lcd_toggle_enable(uint8_t val);
void lcd_send_byte(uint8_t val, int mode);
void lcd_set_cursor(int line, int position);
static void inline lcd_char(char val);
void lcd_string(const char *s);


/* Code */
int main()
{
    printf("program start\n");
    stdio_init_all();
    hardware_init();

    printf("create tasks\n");
    xTaskCreate(task_heartbeat, "LED_Task", 256, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(task_print_msg, "PRINTMSG_Task", 256, NULL, 1, NULL);

    printf("start scheduler\n");
    vTaskStartScheduler();

    while(1)
        tight_loop_contents();
}

/* Handler functions */
void task_heartbeat(void* notUsed)
{   
    while (true) {
        printf("hb-tick: %d\n", HEARTBEAT_DELAY_MS);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        vTaskDelay(HEARTBEAT_DELAY_MS);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(HEARTBEAT_DELAY_MS);
    }
}

void task_print_msg(void* unused)
{
    static char *message[] = {
            "lmao kiddo", "try harder"
        };

    while (true) {
        for (int m = 0; m < sizeof(message) / sizeof(message[0]); m += MAX_LINES) {
            for (int line = 0; line < MAX_LINES; line++) {
                lcd_set_cursor(line, (MAX_CHARS / 2) - strlen(message[m + line]) / 2);
                lcd_string(message[m + line]);
            }
            vTaskDelay(2000);
            lcd_clear();
        }
    }
}

/* LCD functions */
void lcd_init()
{
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(_I2C_NUM, lcd_addr, &val, 1, false);
}

void lcd_toggle_enable(uint8_t val) {
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
    sleep_ms(TOGGLE_DELAY_MS);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_ms(TOGGLE_DELAY_MS);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_ms(TOGGLE_DELAY_MS);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

void inline lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

/* Initialization functions */
void hardware_init(void)
{
    printf("hardware init\n");
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    i2c_init(_I2C_NUM, 100 * 1000);
    gpio_set_function(_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(_I2C_SDA_PIN);
    gpio_pull_up(_I2C_SCL_PIN);

    lcd_init();
}