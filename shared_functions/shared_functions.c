#define _GNU_SOURCE

/*
 *Copyright 2018 David B Brown (@maccoylton)
 *
 * Shared functions used all accessories
 *
*/


#include <shared_functions.h>
#include <sysparam.h>


bool accessory_paired = false;
int log_level = LOG_ACTION;
TaskHandle_t task_stats_task_handle = NULL;
TaskHandle_t wifi_check_interval_task_handle = NULL;
ETSTimer save_timer;
int power_cycle_count = 0;
bool sntp_on = false;
struct sdk_rst_info* reset_information;
bool wifi_connected;


void setup_sntp(){
    
  #ifdef EXTRAS_TIMEKEEPING
    /* Start SNTP */
    printf("%s: Starting SNTP... ", __func__);

    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    
    tzset();
    
    printf (" tset");
    
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    
    printf (" operting mode set");
    
    sntp_setservername(0, "0.pool.ntp.org");
    sntp_setservername(1, "1.pool.ntp.org");
    sntp_setservername(2, "2.pool.ntp.org");
    
    printf (" server names set");
    
    sntp_init();
   
    printf (" init called");
    
    /* SNTP will request an update each 5 minutes */
    
    time_t ts = time(NULL);
    
    printf (" time called ");
    
    printf("TIME: %s", ctime(&ts));
    printf("%s: DONE!\n", __func__);
    
    printf ("SNTP_CHECK_RESPONSE: %d\n", SNTP_CHECK_RESPONSE);
    printf ("SNTP_COMP_ROUNDTRIP: %d\n" , SNTP_COMP_ROUNDTRIP);
    printf ("SNTP_SERVER_DNS: %d\n", SNTP_SERVER_DNS);
    printf ("SNTP_MAX_SERVERS: %d\n", SNTP_MAX_SERVERS);
    printf ("SNTP_UPDATE_DELAY: %d\n", SNTP_UPDATE_DELAY);

    sntp_on = true;
    /* used in homekit common functions */

   #endif
    
 #ifdef EXTRAS_SNTP
    
    #define SNTP_SERVERS 	"0.pool.ntp.org", "1.pool.ntp.org", \
        "2.pool.ntp.org", "3.pool.ntp.org"
    
    const char *servers[] = {SNTP_SERVERS};
    /* Start SNTP */
    printf("Starting SNTP... ");
    /* SNTP will request an update each 5 minutes */
    sntp_set_update_delay(5*60000);
    /* Set GMT+1 zone, daylight savings off */
    const struct timezone tz = {1*60, 0};
    /* SNTP initialization */
    sntp_initialize(&tz);
    /* Servers must be configured right after initialization */
    sntp_set_servers(servers, sizeof(servers) / sizeof(char*));
    printf("DONE!\n");

    sntp_on = true;
    /* used in homekit common functions */

 #endif
}


void task_stats_task ( void *args)
{
    static UBaseType_t uxArraySize;
    static uint32_t ulTotalRunTime;
    static TaskStatus_t pxTaskStatusArray[32];
    
    LOG(LOG_FLOW, "%s", __func__);
    
    while (1) {
        uxArraySize = uxTaskGetNumberOfTasks();
        if (uxArraySize > 32) uxArraySize = 32;
        
        LOG(LOG_FLOW, ", uxTaskGetNumberOfTasks %ld", uxArraySize);
        
        if (uxArraySize > 0)
        {
            uxArraySize = uxTaskGetSystemState( pxTaskStatusArray,
                                               uxArraySize,
                                               &ulTotalRunTime );
            
            LOG(LOG_FLOW, ", uxTaskGetSystemState, ulTotalRunTime %d, array size %ld\n", ulTotalRunTime, uxArraySize);
            
            for (UBaseType_t x = 0; x < uxArraySize; x++ )
            {
                LOG(LOG_FLOW, "Name:%-20s,  Runtime Counter:%-3d,\u00a0Current State:%-3d, Current Priority:%-5ld, Base Priority:%-5ld, High Water Mark (bytes) %-5d\n",
                        pxTaskStatusArray[ x ].pcTaskName,
                        pxTaskStatusArray[ x ].ulRunTimeCounter,
                        pxTaskStatusArray[ x ].eCurrentState,
                        pxTaskStatusArray[ x ].uxCurrentPriority ,
                        pxTaskStatusArray[ x ].uxBasePriority,
                        pxTaskStatusArray[x].usStackHighWaterMark);
            }
        }
        vTaskDelay(TASK_STATS_INTERVAL/ portTICK_PERIOD_MS);
    }
}


void task_stats_set (homekit_value_t value) {
    
    LOG(LOG_MEM, "%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    if (value.bool_value)
        {
            if (task_stats_task_handle == NULL){
                xTaskCreate(task_stats_task, "task_stats_task", TASK_STATS_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &task_stats_task_handle);
            } else {
                LOG(LOG_ERR, "%s task_Status_set TRUE, but task pointer not NULL\n", __func__);
            }
        }
    else
    {
        if (task_stats_task_handle != NULL){
            vTaskDelete (task_stats_task_handle);
            task_stats_task_handle = NULL;
        } else {
            LOG(LOG_ERR, "%s task_Status_set FALSE, but task pointer is NULL\n", __func__);
        }
    }
    LOG(LOG_MEM, "%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

}


void checkWifiTask(void *pvParameters)
{
    uint8_t status ;
    
    wifi_connected = false;
    
    ip_addr_t dns_target_ip;
    int dns_error_count = 0;
    int ret=0;

    while (1)
    {
        if (wifi_check_interval.value.int_value != 0 && accessory_paired == true) {
            /* only check if no zero */
            LOG(LOG_WIFI, "\n%s interval %d, Status: ", __func__, wifi_check_interval.value.int_value);
            status = sdk_wifi_station_get_connect_status();
            switch (status)
            {
                case STATION_WRONG_PASSWORD:
                    LOG(LOG_WIFI, "wrong password: ");
                    led_code (status_led_gpio, WIFI_ISSUE);
                    wifi_connected = false;
                    break;
                case STATION_NO_AP_FOUND:
                    LOG(LOG_WIFI, "AP not found: ");
                    led_code (status_led_gpio, WIFI_ISSUE);
                    wifi_connected = false;
                    break;
                case STATION_CONNECT_FAIL:
                    LOG(LOG_WIFI, "connection failed: ");
                    led_code (status_led_gpio, WIFI_ISSUE);
                    wifi_connected = false;
                    break;
                case STATION_GOT_IP:
                    LOG(LOG_WIFI, "connection ok: ");
                    wifi_connected = true;
                    led_code (status_led_gpio,  WIFI_CONNECTED);
                    break;
                default:
                    LOG(LOG_WIFI, " default = %d: ", status);
                    led_code (status_led_gpio, WIFI_ISSUE);
                    break;
                    
            }

    	    ret = netconn_gethostbyname(HOST, &dns_target_ip);
    	    switch (ret){
        	case ERR_OK:
                	LOG(LOG_WIFI, "DNS OK ");
                    dns_error_count = 0;
                	break;
        	default:
                	LOG(LOG_WIFI, "DNS failed: %d ", ret);
                    led_code (status_led_gpio, WIFI_ISSUE);
                    dns_error_count++;
                    if (dns_error_count > DNS_CHECK_MAX_RETRIES){
                        LOG(LOG_WIFI, "DNS check max retries exceeded restarting accessory\n");
                        save_characteristics();
                        sdk_system_restart();
                    }
            }
        }
        else {
            LOG(LOG_WIFI, "\n%s : no check performed, check interval: %d, accessory paired: %d", __func__, wifi_check_interval.value.int_value, accessory_paired);
            
        }
        
        if (sntp_on == true) {
            time_t ts = time(NULL);
            LOG(LOG_WIFI, "TIME: %s ", ctime(&ts));
        }
        
        printf ("Free Heap=%d, Free Stack=%lu\n", xPortGetFreeHeapSize(), uxTaskGetStackHighWaterMark(NULL)/4);
        /*uxTaskGetStackHighWaterMark returns a number in bytes, stack is created in words, so device by 4 to get nujber of words left on stack */
        
        
        if ( wifi_check_interval.value.int_value==0) {
            vTaskDelay((10000) / portTICK_PERIOD_MS);
        } else {
            vTaskDelay((1000*wifi_check_interval.value.int_value) / portTICK_PERIOD_MS);
        }
    }
}



void wifi_check_stop_start (int interval)
{
    LOG(LOG_MEM, "%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    if (interval==0){
        /* check interval is 0 so make sure the task is not running */
        if (wifi_check_interval_task_handle != NULL)
        {
            LOG(LOG_ACTION, "%s Stopping Task\n", __func__);
            vTaskDelete(wifi_check_interval_task_handle);
            wifi_check_interval_task_handle = NULL;
        }
    } else {
        /* check interval > 0 so make sure the task is running */
        if (wifi_check_interval_task_handle == NULL)
        {
            LOG(LOG_ACTION, "%s Starting Task\n", __func__);
            xTaskCreate (checkWifiTask, "Check WiFi Task", CHECK_WIFI_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &wifi_check_interval_task_handle);
        }
    }
    LOG(LOG_MEM, "%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
}


void preserve_state_set (homekit_value_t value){
    
    preserve_state.value.bool_value = value.bool_value;
    LOG(LOG_ACTION, "%s Preserve State: %s\n", __func__,  preserve_state.value.bool_value ? "true" : "false");
    sdk_os_timer_arm (&save_timer, WIFI_CHECK_INTERVAL_SAVE_DELAY, 0);
}


void wifi_check_interval_set (homekit_value_t value){
    
    wifi_check_interval.value.int_value = value.int_value;
    LOG(LOG_ACTION, "%s Wifi Check Interval: %d\n", __func__, wifi_check_interval.value.int_value);
    wifi_check_stop_start (wifi_check_interval.value.int_value);
    sdk_os_timer_arm (&save_timer, WIFI_CHECK_INTERVAL_SAVE_DELAY, 0);
}


void wifi_reset_set(homekit_value_t value){
    LOG(LOG_ACTION, "Resetting Wifi Config\n");
    wifi_config_reset();
    LOG(LOG_ACTION, "Restarting\n");
    sdk_system_restart();
}


void ota_beta_set ( homekit_value_t value){
    LOG(LOG_ACTION, "%s:\n", __func__);
    ota_beta.value.bool_value = value.bool_value;
    save_characteristic_to_flash(&ota_beta, ota_beta.value );
    LOG(LOG_ACTION, "%s: Restarting\n", __func__);
    sdk_system_restart();
}


void lcm_beta_set ( homekit_value_t value){
    LOG(LOG_ACTION, "%s:\n", __func__);
    lcm_beta.value.bool_value = value.bool_value;
    save_characteristic_to_flash(&lcm_beta, lcm_beta.value );
    LOG(LOG_ACTION, "%s: Restarting\n", __func__);
    sdk_system_restart();
}


void lcm_emergency_set (homekit_value_t value){
    if (value.bool_value) {
        int step = 3;
        char *val;
        if (sysparam_get_string("ota_count_step", &val) == SYSPARAM_OK) {
            if (*val > '0' && *val < '4' && strlen(val) == 1)
                step = *val - '0';
            free(val);
        }
        char count_str[4];
        snprintf(count_str, sizeof(count_str), "%d", 5 + step * 2);
        sysparam_set_string("ota_count", count_str);
        sysparam_set_bool("lcm_beta", true);
        rboot_set_temp_rom(1);
        sdk_system_restart();
    }
}


void log_level_set (homekit_value_t value){
    if (value.format == homekit_format_uint8) {
        log_level = value.uint8_value;
    } else if (value.format == homekit_format_float) {
        log_level = (int)value.float_value;
    } else if (value.format == homekit_format_int) {
        log_level = value.int_value;
    }
    LOG(LOG_ACTION, "%s: log level set to %d\n", __func__, log_level);
}


void identify_task(void *_args) {
    // We identify the Device by Flashing it's LED.
    led_code( status_led_gpio, IDENTIFY_ACCESSORY);
    vTaskDelete(NULL);
}

void identify(homekit_value_t _value) {
    LOG(LOG_ACTION, "Identify\n");
    xTaskCreate(identify_task, "Identify", IDENTIFY_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
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
    LOG(LOG_ACTION, "Resetting Wifi Config\n");
    
    wifi_config_reset();
    led_code( status_led_gpio, WIFI_CONFIG_RESET);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    LOG(LOG_ACTION, "Resetting HomeKit Config\n");
    
    homekit_server_reset();
    led_code( status_led_gpio, EXTRA_CONFIG_RESET);
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    LOG(LOG_ACTION, "Restarting\n");
    led_code( status_led_gpio, RESTART_DEVICE);
    
    sdk_system_restart();
    
    vTaskDelete(NULL);
}


void reset_configuration() {
    LOG(LOG_ACTION, "Resetting Device configuration\n");
    xTaskCreate(reset_configuration_task, "Reset configuration", RESET_CONFIG_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
}


void reset_button_callback(uint8_t gpio, void* args, uint8_t param) {
    LOG(LOG_ACTION, "Reset Button event long press on GPIO : %d\n", gpio);
    reset_configuration();
    
}


void create_accessory_name(const char* name, const char* model, homekit_characteristic_t *accessory_name, homekit_characteristic_t *accessory_serial){
    
    LOG(LOG_MEM, "%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
    int serialLength = snprintf(NULL, 0, "%d", sdk_system_get_chip_id());
    
    char *serialNumberValue = malloc(serialLength + 1);
    if (!serialNumberValue) {
        LOG(LOG_ERR, "%s: malloc failed\n", __func__);
        return;
    }
    
    snprintf(serialNumberValue, serialLength + 1, "%d", sdk_system_get_chip_id());
    
    char *name_value;
    int name_len = asprintf(&name_value, "%s-%s-%s",
                            name,
                            model,
                            serialNumberValue);
    
    if (name_len < 0 || !name_value) {
        LOG(LOG_ERR, "%s: asprintf failed\n", __func__);
        free(serialNumberValue);
        return;
    }
    
    if (name_len > 63) {
        name_value[63] = 0;
    }
    
    accessory_name->value = HOMEKIT_STRING(name_value);
    accessory_serial->value = HOMEKIT_STRING(serialNumberValue);
    
    LOG(LOG_MEM, "%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

}


void on_homekit_event(homekit_event_t event) {
    
    switch (event) {
        case HOMEKIT_EVENT_SERVER_INITIALIZED:
            LOG(LOG_EVENT, "%s: Server initialised, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            if (homekit_is_paired()){
                /* if server has started and we already have a pairing then initialise*/
                accessory_paired = true;
                LOG(LOG_EVENT, "%s: Acessory is paired on initialisation, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
                accessory_init ();
                led_code( status_led_gpio, WIFI_CONNECTED);
                wifi_check_stop_start (wifi_check_interval.value.int_value);
            }
            else
            {
                LOG(LOG_EVENT, "%s: Acessory is NOT paired on initialisation, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
                accessory_paired = false;
                /* stop wifi check to reduce interference with pairing*/
                accessory_init_not_paired ();
                wifi_check_stop_start (0);
            }
            break;
        case HOMEKIT_EVENT_CLIENT_CONNECTED:
            LOG(LOG_EVENT, "%s: Client connected, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            break;
        case HOMEKIT_EVENT_CLIENT_VERIFIED:
            LOG(LOG_EVENT, "%s: Client verified, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            /* we weren't paired on started up but we now are */
            if (!accessory_paired ){
                accessory_paired = true;
                LOG(LOG_EVENT, "%s: Acessory is paired on after client validaiton, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
                accessory_init();
                led_code( status_led_gpio, WIFI_CONNECTED);
                wifi_check_stop_start (wifi_check_interval.value.int_value);
            }
            break;
        case HOMEKIT_EVENT_CLIENT_DISCONNECTED:
            LOG(LOG_EVENT, "%s: Client disconnected, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            break;
        case HOMEKIT_EVENT_PAIRING_ADDED:
            LOG(LOG_EVENT, "%s: Pairing added, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            break;
        case HOMEKIT_EVENT_PAIRING_REMOVED:
            LOG(LOG_EVENT, "%s: Pairing removed, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            if (!homekit_is_paired()){
            /* if we have no more pairings then restart */
                LOG(LOG_EVENT, "%s: no more pairings so restart\n", __func__);
                accessory_paired = false;
                sdk_system_restart();
                wifi_check_stop_start (0);
            }
            break;
        default:
            LOG(LOG_EVENT, "%s: Default event %d,  Free Heap=%d\n", __func__, event, xPortGetFreeHeapSize());
    }
    LOG(LOG_MEM, "%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
}


void on_wifi_ready ( void) {
    
    udplog_init(tskIDLE_PRIORITY+1);
    LOG(LOG_FLOW, "%s: UDP Log Init Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    LOG(LOG_MEM, "%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    get_sysparam_info();
    reset_information = sdk_system_get_rst_info();
    switch (reset_information->reason){
        case DEFAULT_RST:
            LOG(LOG_ERR, "%s Reset Reason: Default Reset\n", __func__);
            break;
        case WDT_RST:
            LOG(LOG_ERR, "%s Reset Reason: Whatchdog Rest\n", __func__);
            recover_from_reset (reset_information->reason);
            break;
        case EXCEPTION_RST:
            LOG(LOG_ERR, "%s Reset Reason: Excepton Rest\n", __func__);
            recover_from_reset (reset_information->reason);
            break;
        case SOFT_RST:
            LOG(LOG_ERR, "%s Reset Reason: Soft Reeet\n", __func__);
            break;
        default:
            LOG(LOG_ERR, "%s Reset Reason: Unknown\n", __func__);
            
    }
    LOG(LOG_ERR, "%s: Exception Cause: %d\nEPC 1: %d\nEPC 2: %d\nEPC 3: %d\nExv virtul address: %d\nDEPC: %d\nReturn Address:%d\n", __func__,  reset_information->exccause, reset_information->epc1, reset_information->epc2, reset_information->epc3, reset_information->excvaddr, reset_information->depc,reset_information->rtn_addr);
    
    LOG(LOG_FLOW, "%s: Calling homekit_server_init, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    homekit_server_init(&config);
    LOG(LOG_FLOW, "%s: After calling homekit_Server_init, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    LOG(LOG_MEM, "%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
}


void standard_init (homekit_characteristic_t *name, homekit_characteristic_t *manufacturer, homekit_characteristic_t *model, homekit_characteristic_t *serial, homekit_characteristic_t *revision){

    LOG(LOG_FLOW, "%s: Start, SDK version: %s, Freep Heap=%d\n", __func__, sdk_system_get_sdk_version(), xPortGetFreeHeapSize());

    rboot_rtc_data rtc;

    if (rboot_get_rtc_data(&rtc)) {
        power_cycle_count = rtc.temp_rom;
        LOG(LOG_FLOW, "%s: RTC power cycle count = %d\n", __func__, power_cycle_count);

    } else {
        LOG(LOG_ERR, "%s: Error reading RTC\n", __func__);
    }
    LOG(LOG_FLOW, "%s: RTC Data, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    uart_set_baud(0, 115200);
    LOG(LOG_FLOW, "%s: UART, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    get_sysparam_info();
    
    load_characteristic_from_flash (&wifi_check_interval);
    load_characteristic_from_flash (&ota_beta);
    load_characteristic_from_flash (&lcm_beta);
    load_characteristic_from_flash (&preserve_state);

    LOG(LOG_EVENT, "%s: Load charactersitics Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
        create_accessory_name(name->value.string_value, model->value.string_value, name, serial);
    
    int c_hash=ota_read_sysparam(&manufacturer->value.string_value,&serial->value.string_value,
                                 &model->value.string_value,&revision->value.string_value);
    if (c_hash==0) c_hash=1;
    config.accessories[0]->config_number=c_hash;
    LOG(LOG_FLOW, "%s: ota_read_sysparams Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
    /*wifi_check_stop_start (wifi_check_interval.value.int_value);*/

    sdk_os_timer_setfn(&save_timer, save_characteristics, NULL);
    LOG(LOG_FLOW, "%s: Save Timer Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
    LOG(LOG_MEM, "%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

}

