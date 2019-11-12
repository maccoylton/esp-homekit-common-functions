/*
shared functions used all accessorys

*/
#include <stdio.h>
#include <homekit/homekit.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <led_codes.h>
#include <FreeRTOS.h>
#include <task.h>
#include <wifi_config.h>
#include <udplogger.h>
#include <custom_characteristics.h>
#include <esp/uart.h>
#include <ota-api.h>



extern const int status_led_gpio;
extern bool accessory_paired;
extern homekit_server_config_t config;

void accessory_init(void);
void create_accessory_name(const char* name, const char* model, homekit_characteristic_t *accessory_name, homekit_characteristic_t *accessory_serial);
void identify(homekit_value_t _value);
void relay_write(bool on, int gpio);
void led_write(bool on, int gpio);
void on_homekit_event(homekit_event_t event);
void reset_configuration();
void wifi_reset_set(homekit_value_t value);
void on_wifi_ready ( void) ;
void standard_init (homekit_characteristic_t *name, homekit_characteristic_t *manufacturer, homekit_characteristic_t *model, homekit_characteristic_t *serial, homekit_characteristic_t *revision);



