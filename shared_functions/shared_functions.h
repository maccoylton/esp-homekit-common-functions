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
extern homekit_characteristic_t wifi_check_interval;

void accessory_init(void);
/* initalise anything you don't want started until wifi and pairing is confirmed */

void accessory_init_not_paired(void);
/* initalise anything you don't want started until wifi and homekit imitialisation is confirmed, but not paired */

void standard_init (homekit_characteristic_t *name, homekit_characteristic_t *manufacturer, homekit_characteristic_t *model, homekit_characteristic_t *serial, homekit_characteristic_t *revision);
/* initalises UART, UDPLOG, gets sys param info, creates accessory name and readys OAT parameters */

void reset_button_callback(uint8_t gpio, void* args, uint8_t param);
/* called when reset button is pressed, calls reset_buton */

void identify(homekit_value_t _value);
/* Led on to show identify accerros */

void relay_write(bool on, int gpio);
/* write on value to gpio */

void led_write(bool on, int gpio);
/* write on value to gpio */

void create_accessory_name(const char* name, const char* model, homekit_characteristic_t *accessory_name, homekit_characteristic_t *accessory_serial);
/* creates the accesory name with manufacturer-device-chipID */

void on_homekit_event(homekit_event_t event);
/* caled when a homekit event occours, service inti, pairing added, pairing removed, client connect, client disconnect  etc */

void reset_configuration();
/*resets WiFi and Homekit and restarts device */

void wifi_reset_set(homekit_value_t value);
/* called when wifi-reeet characteristic is set */

void on_wifi_ready ( void) ;
/* called when WiFi is connected */


void wifi_check_interval_set (homekit_value_t value);
/* used to alter the interval of the Wifi Check≤ intervals of 10 seconds, 0 for no check */




