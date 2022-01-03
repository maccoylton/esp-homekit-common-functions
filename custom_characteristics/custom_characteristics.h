/*
 * HomeKit Custom Characteristics
 *
 * Copyright 2018 David B Brown (@maccoylton)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <FreeRTOS.h>


#ifndef __HOMEKIT_DBB_CUSTOM_CHARACTERISTICS__
#define __HOMEKIT_DBB_CUSTOM_CHARACTERISTICS__

#define HOMEKIT_CUSTOM_UUID_DBB(value) (value"-4772-4466-80fd-a6ea3d5bcd55")


#define HOMEKIT_CHARACTERISTIC_CUSTOM_POLL_PERIOD HOMEKIT_CUSTOM_UUID_DBB("F0000001")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_POLL_PERIOD(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_POLL_PERIOD, \
.description = "Poll Period", \
.format = homekit_format_uint8, \
.unit = homekit_unit_seconds, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {1000}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_SQL_LOG HOMEKIT_CUSTOM_UUID_DBB("F0000002")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_SQL_LOG(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_SQL_LOG, \
.description = "SQL LOG", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_LPG_LEVEL HOMEKIT_CUSTOM_UUID_DBB("F0000003")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_LPG_LEVEL(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_LPG_LEVEL, \
.description = "LPG Level", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_METHANE_LEVEL HOMEKIT_CUSTOM_UUID_DBB("F0000004")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_METHANE_LEVEL(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_METHANE_LEVEL, \
.description = "Methane Level", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_AMMONIUM_LEVEL HOMEKIT_CUSTOM_UUID_DBB("F0000005")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_AMMONIUM_LEVEL(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_AMMONIUM_LEVEL, \
.description = "Amonium Level", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {10000}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_WIFI_RESET HOMEKIT_CUSTOM_UUID_DBB("F0000006")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_WIFI_RESET(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_WIFI_RESET, \
.description = "Reset WiFi", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_WIFI_CHECK_INTERVAL HOMEKIT_CUSTOM_UUID_DBB("F0000007")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_WIFI_CHECK_INTERVAL(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_WIFI_CHECK_INTERVAL, \
.description = "WiFi check interval", \
.format = homekit_format_int, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {300}, \
.min_step = (float[]) {10}, \
.value = HOMEKIT_INT_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_RED_GPIO HOMEKIT_CUSTOM_UUID_DBB("F0000008")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_RED_GPIO(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_RED_GPIO, \
.description = "RED GPIO", \
.format = homekit_format_uint8, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {16}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_GREEN_GPIO HOMEKIT_CUSTOM_UUID_DBB("F0000009")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_GREEN_GPIO(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_GREEN_GPIO, \
.description = "GREEN GPIO", \
.format = homekit_format_uint8, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {16}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_BLUE_GPIO HOMEKIT_CUSTOM_UUID_DBB("F000000A")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_BLUE_GPIO(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_BLUE_GPIO, \
.description = "BLUE GPIO", \
.format = homekit_format_uint8, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {16}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_WHITE_GPIO HOMEKIT_CUSTOM_UUID_DBB("F000000B")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_WHITE_GPIO(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_WHITE_GPIO, \
.description = "WHITE GPIO", \
.format = homekit_format_uint8, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {16}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_LED_BOOST HOMEKIT_CUSTOM_UUID_DBB("F000000C")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_LED_BOOST(_value, ...) .type = HOMEKIT_CHARACTERISTIC_CUSTOM_LED_BOOST, \
.description = "LED BOOST", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {5}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_TASK_STATS HOMEKIT_CUSTOM_UUID_DBB("F000000D")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_TASK_STATS(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_TASK_STATS, \
.description = "Debug Task Stats", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_GPIO_TEST HOMEKIT_CUSTOM_UUID_DBB("F000000E")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_COLOURS_GPIO_TEST(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_GPIO_TEST, \
.description = "Test Colour GPIOs", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_STROBE HOMEKIT_CUSTOM_UUID_DBB("F0000010")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_COLOURS_STROBE(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_STROBE, \
.description = "Strobe Effect", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_FLASH HOMEKIT_CUSTOM_UUID_DBB("F0000011")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_COLOURS_FLASH(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_FLASH, \
.description = "Flash Effect", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_FADE HOMEKIT_CUSTOM_UUID_DBB("F0000012")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_COLOURS_FADE(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_FADE, \
.description = "Fade Effect", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_SMOOTH HOMEKIT_CUSTOM_UUID_DBB("F0000013")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_COLOURS_SMOOTH(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_SMOOTH, \
.description = "Smooth Effect", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_PURE_WHITE HOMEKIT_CUSTOM_UUID_DBB("F0000014")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_COLOURS_PURE_WHITE(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_COLOURS_PURE_WHITE, \
.description = "Pure White", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_OTA_BETA HOMEKIT_CUSTOM_UUID_DBB("F0000015")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_OTA_BETA(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_OTA_BETA, \
.description = "ota_beta", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_LCM_BETA HOMEKIT_CUSTOM_UUID_DBB("F0000016")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_LCM_BETA(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_LCM_BETA, \
.description = "lcm_beta", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_WATTS HOMEKIT_CUSTOM_UUID_DBB("F0000017")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_WATTS(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_WATTS, \
.description = "WATTS", \
.format = homekit_format_uint16, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {3120}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT16_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_VOLTS HOMEKIT_CUSTOM_UUID_DBB("F0000018")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_VOLTS(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_VOLTS, \
.description = "VOLTS", \
.format = homekit_format_uint16, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {240}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT16_(_value), \
##__VA_ARGS__

#define HOMEKIT_CHARACTERISTIC_CUSTOM_AMPS HOMEKIT_CUSTOM_UUID_DBB("F0000019")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_AMPS(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_AMPS, \
.description = "AMPS", \
.format = homekit_format_float, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {13}, \
.min_step = (float[]) {0.01}, \
.value = HOMEKIT_FLOAT_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_CALIBRATE_POW HOMEKIT_CUSTOM_UUID_DBB("F000001A")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_CALIBRATE_POW(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_CALIBRATE_POW, \
.description = "Calibrate POW", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_CALIBRATE_VOLTS HOMEKIT_CUSTOM_UUID_DBB("F000001B")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_CALIBRATE_VOLTS(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_CALIBRATE_VOLTS, \
.description = "Calibrate Volts", \
.format = homekit_format_uint16, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {1}, \
.max_value = (float[]) {240}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT16_(_value), \
##__VA_ARGS__



#define HOMEKIT_CHARACTERISTIC_CUSTOM_CALIBRATE_WATTS HOMEKIT_CUSTOM_UUID_DBB("F000001C")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_CALIBRATE_WATTS(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_CALIBRATE_WATTS, \
.description = "Calibrate WATTS", \
.format = homekit_format_uint16, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {1}, \
.max_value = (float[]) {3120}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT16_(_value), \
##__VA_ARGS__



#define HOMEKIT_CHARACTERISTIC_CUSTOM_POWER_ON_STRATEGY HOMEKIT_CUSTOM_UUID_DBB("F000001D")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_POWER_ON_STRATEGY(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_POWER_ON_STRATEGY, \
.description = "Power-on Strategy", \
.format = homekit_format_uint8, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {2}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__



#define HOMEKIT_CHARACTERISTIC_CUSTOM_INIT_FX_MODE HOMEKIT_CUSTOM_UUID_DBB("F000001E")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_INIT_FX_MODE(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_INIT_FX_MODE, \
.description = "Init WS2812FX Mode", \
.format = homekit_format_uint8, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {53}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__



#define HOMEKIT_CHARACTERISTIC_CUSTOM_EXTERNALTEMP_SENSOR HOMEKIT_CUSTOM_UUID_DBB("F000001F")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_EXTERNALTEMP_SENSOR(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_EXTERNALTEMP_SENSOR, \
.description = "External temperature Sensor", \
.format = homekit_format_bool, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.value = HOMEKIT_BOOL_(_value), \
##__VA_ARGS__


#define HOMEKIT_CHARACTERISTIC_CUSTOM_RGBIC_EFFECT HOMEKIT_CUSTOM_UUID_DBB("F0000020")
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM_RGBIC_EFFECT(_value, ...) \
.type = HOMEKIT_CHARACTERISTIC_CUSTOM_RGBIC_EFFECT, \
.description = "RGBIC EFFECT", \
.format = homekit_format_uint8, \
.permissions = homekit_permissions_paired_read \
| homekit_permissions_paired_write \
| homekit_permissions_notify, \
.min_value = (float[]) {0}, \
.max_value = (float[]) {53}, \
.min_step = (float[]) {1}, \
.value = HOMEKIT_UINT8_(_value), \
##__VA_ARGS__

#endif

void save_characteristic_to_flash (homekit_characteristic_t *ch, homekit_value_t value);

void load_characteristic_from_flash (homekit_characteristic_t *ch);

void get_sysparam_info();

void save_int32_param ( const char *description, int32_t new_value);

void save_float_param ( const char *description, float new_float_value);

void load_float_param ( const char *description, float *new_float_value);

void homekit_characteristic_bounds_check (homekit_characteristic_t *ch);
/* check that integers and floats are within min and max values */




