//
//
//  rgbw_lights.h
//
//  Created by David B Brown on 30/12/2019.
//  Copyright © 2018 maccoylton. All rights reserved.
//
/*
 * This is an a library used to implment RGBW lights and strips with esp-homekit
 *
 * more info about the controller and flashing can be found here:
 *
 */

#ifndef RGBW_LIGHTS_H
#define RGBW_LIGHTS_H

#define PWM_SCALE 255
#define RGBW_STRIP_SET_DELAY 100
#define SAVE_DELAY 5000

#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <espressif/esp_system.h>
#include <esp/uart.h>
#include <etstimer.h>
#include <esplibs/libmain.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>
#include <math.h>
#include <sysparam.h>


#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#include <multipwm/multipwm.h>
#include <custom_characteristics.h>
#include <udplogger.h>
#include <shared_functions.h>
#include <colour_conversion.h>


enum {white_pin=0, blue_pin=1, green_pin=2, red_pin=3} led_pins;

static pwm_info_t pwm_info;
ETSTimer rgbw_set_timer;
ETSTimer gpio_timer;

// Global variables
extern float led_hue;              // hue is scaled 0 to 360
extern float led_saturation;      // saturation is scaled 0 to 100
extern float led_brightness;     // brightness is scaled 0 to 100
extern bool led_on;            // on is boolean on or off

extern int white_default_gpio;
extern int red_default_gpio;
extern int green_default_gpio;
extern int blue_default_gpio;

extern rgb_color_t current_color;
extern rgb_color_t target_color;

extern homekit_characteristic_t on;
extern homekit_characteristic_t brightness;
extern homekit_characteristic_t hue;
extern homekit_characteristic_t saturation;
extern homekit_characteristic_t red_gpio;
extern homekit_characteristic_t green_gpio;
extern homekit_characteristic_t blue_gpio;
extern homekit_characteristic_t white_gpio;


void on_update(homekit_characteristic_t *ch, homekit_value_t value, void *context);

homekit_value_t led_on_get();

void led_on_set(homekit_value_t value);

homekit_value_t led_brightness_get();

void led_brightness_set(homekit_value_t value);

homekit_value_t led_hue_get();

void led_hue_set(homekit_value_t value);

homekit_value_t led_saturation_get();

void led_saturation_set(homekit_value_t value);

void rgbw_set ();

void gpio_update_set();

void rgbw_lights_init();

#endif
