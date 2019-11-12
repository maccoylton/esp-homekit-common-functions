
/*
 shared functions used all accessorys
 
 */


#include <shared_functions.h>

bool accessory_paired = false;

void wifi_reset_set(homekit_value_t value){
    printf("Resetting Wifi Config\n");
    wifi_config_reset();
    printf("Restarting\n");
    sdk_system_restart();
}

void identify_task(void *_args) {
    // We identify the Device by Flashing it's LED.
    led_code( status_led_gpio, IDENTIFY_ACCESSORY);
    vTaskDelete(NULL);
}

void identify(homekit_value_t _value) {
    printf("Identify\n");
    xTaskCreate(identify_task, "Identify", 128, NULL, 2, NULL);
}


void relay_write(bool on, int gpio) {
    gpio_write(gpio, on ? 1 : 0);
}


void led_write(bool on, int gpio) {
    gpio_write(gpio, on ? 0 : 1);
}


void reset_configuration_task() {
    //Flash the LED first before we start the reset
    led_code (status_led_gpio, WIFI_CONFIG_RESET);
    printf("Resetting Wifi Config\n");
    
    wifi_config_reset();
    led_code( status_led_gpio, WIFI_CONFIG_RESET);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    printf("Resetting HomeKit Config\n");
    
    homekit_server_reset();
    led_code( status_led_gpio, EXTRA_CONFIG_RESET);
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    printf("Restarting\n");
    led_code( status_led_gpio, RESTART_DEVICE);
    
    sdk_system_restart();
    
    vTaskDelete(NULL);
}


void reset_configuration() {
    printf("Resetting Device configuration\n");
    xTaskCreate(reset_configuration_task, "Reset configuration", 256, NULL, 2, NULL);
}


void reset_button_callback(uint8_t gpio, void* args, uint8_t param) {
    printf("Reset Button event long press on GPIO : %d\n", gpio);
    reset_configuration();
    
}


void create_accessory_name(const char* name, const char* model, homekit_characteristic_t accessory_name, homekit_characteristic_t accessory_serial) {
    
    int serialLength = snprintf(NULL, 0, "%d", sdk_system_get_chip_id());
    
    char *serialNumberValue = malloc(serialLength + 1);
    
    snprintf(serialNumberValue, serialLength + 1, "%d", sdk_system_get_chip_id());
    
    int name_len = snprintf(NULL, 0, "%s-%s-%s",
                            name,
                            model,
                            serialNumberValue);
    
    if (name_len > 63) {
        name_len = 63;
    }
    
    char *name_value = malloc(name_len + 1);
    
    snprintf(name_value, name_len + 1, "%s-%s-%s",
             name, model, serialNumberValue);
    
    
    accessory_name.value = HOMEKIT_STRING(name_value);
    accessory_serial.value = accessory_name.value;
}


void on_homekit_event(homekit_event_t event) {
    
    switch (event) {
        case HOMEKIT_EVENT_SERVER_INITIALIZED:
            printf("on_homekit_event: Server initialised\n");
            if (homekit_is_paired()){
                /* if server has started and we already have a pairing then initialise*/
                accessory_paired = true;
                printf("on_homekit_event: Acessory is paired on initialisation\n");
                accessory_init ();
                led_code( status_led_gpio, WIFI_CONNECTED);
            }
            else
            {
                printf("on_homekit_event: Acessory is NOT paired on initialisation\n");
            }
            break;
        case HOMEKIT_EVENT_CLIENT_CONNECTED:
            printf("on_homekit_event: Client connected\n");
            break;
        case HOMEKIT_EVENT_CLIENT_VERIFIED:
            printf("on_homekit_event: Client verified\n");
            /* we weren't paired on started up but we now are */
            if (!accessory_paired ){
                accessory_paired = true;
                printf("on_homekit_event: Acessory is paired on after client validaiton\n");
                accessory_init();
                led_code( status_led_gpio, WIFI_CONNECTED);
            }
            break;
        case HOMEKIT_EVENT_CLIENT_DISCONNECTED:
            printf("on_homekit_event: Client disconnected\n");
            break;
        case HOMEKIT_EVENT_PAIRING_ADDED:
            printf("on_homekit_event: Pairing added\n");
            break;
        case HOMEKIT_EVENT_PAIRING_REMOVED:
            printf("on_homekit_event: Pairing removed\n");
            if (!homekit_is_paired())
            /* if we have no more pairings then restart */
                printf("on_homekit_event: no more pairings so restart\n");
            sdk_system_restart();
            break;
        default:
            printf("on_homekit_event: Default event %d ", event);
    }
    
}
