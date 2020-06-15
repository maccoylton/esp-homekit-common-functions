/*
shared functions used all accessorys

*/


#ifndef SHARED_FUNCTIONS_H
#define SHARED_FUNCTIONS_H

#include <stdio.h>
#include <homekit/homekit.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <esplibs/libmain.h>
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
#include <rboot-api.h>


extern const int status_led_gpio;
extern bool accessory_paired;
extern homekit_server_config_t config;
extern homekit_characteristic_t wifi_check_interval;
extern homekit_characteristic_t ota_beta;
extern homekit_characteristic_t lcm_beta;
struct sdk_rst_info* reset_information;
extern int power_cycle_count;
extern ETSTimer save_timer;


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

void task_stats_set (homekit_value_t value);
/* turns on or off priting tasks stats */

void recover_from_reset (int reason);
/* called if we restarted abnormally */

void save_characteristics (  );
/* called by a timer function to save charactersitics */

void ota_beta_set ( homekit_value_t value);
/* called to set the ota_beta flag */

void lcm_beta_set ( homekit_value_t value);
/* called to set the lcm_beta flag */

void wifi_check_stop_start (int interval);
/* called to set the wifi check taks runing or stop it if the value passed is 0 */

void homekit_characteristic_bounds_check (homekit_characteristic_t *ch);
/* check that integers and floats are within min and max values */

#endif
