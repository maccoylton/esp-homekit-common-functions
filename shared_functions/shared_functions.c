
/*
 *Copyright 2018 David B Brown (@maccoylton)
 *
 * Shared functions used all accessories
 *
*/


#include <shared_functions.h>


bool accessory_paired = false;
TaskHandle_t task_stats_task_handle = NULL;
TaskHandle_t wifi_check_interval_task_handle = NULL;
ETSTimer save_timer;
int power_cycle_count = 0;
bool sntp_on = false;
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
    static TaskStatus_t *pxTaskStatusArray;
    static UBaseType_t uxArraySize, x;
    static uint32_t ulTotalRunTime;
    
    printf ("%s", __func__);
    
    while (1) {
        /* Take a snapshot of the number of tasks in case it changes while this
         function is executing. */
        uxArraySize = uxTaskGetNumberOfTasks();
        
        printf (", uxTaskGetNumberOfTasks %ld", uxArraySize);
        /* Allocate a TaskStatus_t structure for each task.  An array could be
         allocated statically at compile time. */
        pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );
        
        printf (", pvPortMalloc");
        
        if( pxTaskStatusArray != NULL )
        {
            /* Generate raw status information about each task. */
            uxArraySize = uxTaskGetSystemState( pxTaskStatusArray,
                                               uxArraySize,
                                               &ulTotalRunTime );
            
            printf (", uxTaskGetSystemState, ulTotalRunTime %d, array size %ld\n", ulTotalRunTime, uxArraySize);
            
            /* Avoid divide by zero errors. */
            /*        if( ulTotalRunTime > 0 )
             {*/
            /* For each populated position in the pxTaskStatusArray array,
             format the raw data as human readable ASCII data. */
            for( x = 0; x < uxArraySize; x++ )
            {

                printf ( "Name:%-20s,  Runtime Counter:%-3d, Current State:%-3d, Current Priority:%-5ld, Base Priority:%-5ld, High Water Mark (bytes) %-5d\n",
                        pxTaskStatusArray[ x ].pcTaskName,
                        pxTaskStatusArray[ x ].ulRunTimeCounter,
                        pxTaskStatusArray[ x ].eCurrentState,
                        pxTaskStatusArray[ x ].uxCurrentPriority ,
                        pxTaskStatusArray[ x ].uxBasePriority,
                        pxTaskStatusArray[x].usStackHighWaterMark);
                
            }
            /*        }*/
            
            /* The array is no longer needed, free the memory it consumes. */
            vPortFree( pxTaskStatusArray );
            printf ("%s, vPortFree\n", __func__);
        }
        vTaskDelay(TASK_STATS_INTERVAL/ portTICK_PERIOD_MS);
    }
}


void task_stats_set (homekit_value_t value) {
    
    printf("%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    if (value.bool_value)
        {
            if (task_stats_task_handle == NULL){
                xTaskCreate(task_stats_task, "task_stats_task", 384 , NULL, tskIDLE_PRIORITY+1, &task_stats_task_handle);
            } else {
                printf ("%s task_Status_set TRUE, but task pointer not NULL\n", __func__);
            }
        }
    else
    {
        if (task_stats_task_handle != NULL){
            vTaskDelete (task_stats_task_handle);
            task_stats_task_handle = NULL;
        } else {
            printf ("%s task_Status_set FALSE, but task pointer is NULL\n", __func__);
        }
    }
    printf("%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

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
            printf("\n%s interval %d, Status: ", __func__, wifi_check_interval.value.int_value);
            status = sdk_wifi_station_get_connect_status();
            switch (status)
            {
                case STATION_WRONG_PASSWORD:
                    printf("wrong password: ");
                    led_code (status_led_gpio, WIFI_ISSUE);
                    wifi_connected = false;
                    break;
                case STATION_NO_AP_FOUND:
                    printf("AP not found: ");
                    led_code (status_led_gpio, WIFI_ISSUE);
                    wifi_connected = false;
                    break;
                case STATION_CONNECT_FAIL:
                    printf("connection failed: ");
                    led_code (status_led_gpio, WIFI_ISSUE);
                    wifi_connected = false;
                    break;
                case STATION_GOT_IP:
                    printf("connection ok: ");
                    wifi_connected = true;
                    led_code (status_led_gpio,  WIFI_CONNECTED);
                    break;
                default:
                    printf(" default = %d: ", status);
                    led_code (status_led_gpio, WIFI_ISSUE);
                    break;
                    
            }

    	    ret = netconn_gethostbyname(HOST, &dns_target_ip);
    	    switch (ret){
        	case ERR_OK:
                	printf ("DNS OK ");
                    dns_error_count = 0;
                	break;
        	default:
                	printf ("DNS failed: %d ", ret);
                    led_code (status_led_gpio, WIFI_ISSUE);
                    dns_error_count++;
                    if (dns_error_count > DNS_CHECK_MAX_RETRIES){
                        printf ("DNS check max retries exceeded restarting accessory\n");
                        save_characteristics();
                        sdk_system_restart();
                    }
            }
        }
        else {
            printf("\n%s : no check performed, check interval: %d, accessory paired: %d", __func__, wifi_check_interval.value.int_value, accessory_paired);
            
        }
        
        if (sntp_on == true) {
            time_t ts = time(NULL);
            printf("TIME: %s ", ctime(&ts));
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
    printf("%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    if (interval==0){
        /* check interval is 0 so make sure the task is not running */
        if (wifi_check_interval_task_handle != NULL)
        {
            printf ("%s Stopping Task\n", __func__);
            vTaskDelete(wifi_check_interval_task_handle);
            wifi_check_interval_task_handle = NULL;
        }
    } else {
        /* check interval > 0 so make sure the task is running */
        if (wifi_check_interval_task_handle == NULL)
        {
            printf ("%s Starting Task\n", __func__);
            xTaskCreate (checkWifiTask, "Check WiFi Task", 288, NULL, tskIDLE_PRIORITY+1, &wifi_check_interval_task_handle);
        }
    }
    printf("%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
}


void preserve_state_set (homekit_value_t value){
    
    preserve_state.value.bool_value = value.bool_value;
    printf ("%s Preserve State: %s\n", __func__,  preserve_state.value.bool_value ? "true" : "false");
    sdk_os_timer_arm (&save_timer, WIFI_CHECK_INTERVAL_SAVE_DELAY, 0);
}


void wifi_check_interval_set (homekit_value_t value){
    
    wifi_check_interval.value.int_value = value.int_value;
    printf ("%s Wifi Check Interval: %d\n", __func__, wifi_check_interval.value.int_value);
    wifi_check_stop_start (wifi_check_interval.value.int_value);
    sdk_os_timer_arm (&save_timer, WIFI_CHECK_INTERVAL_SAVE_DELAY, 0);
}


void wifi_reset_set(homekit_value_t value){
    printf("Resetting Wifi Config\n");
    wifi_config_reset();
    printf("Restarting\n");
    sdk_system_restart();
}


void ota_beta_set ( homekit_value_t value){
    printf("%s:\n", __func__);
    ota_beta.value.bool_value = value.bool_value;
    save_characteristic_to_flash(&ota_beta, ota_beta.value );
    printf("%s: Restarting\n", __func__);
    sdk_system_restart();
}


void lcm_beta_set ( homekit_value_t value){
    printf("%s:\n", __func__);
    lcm_beta.value.bool_value = value.bool_value;
    save_characteristic_to_flash(&lcm_beta, lcm_beta.value );
    printf("%s: Restarting\n", __func__);
    sdk_system_restart();
}


void identify_task(void *_args) {
    // We identify the Device by Flashing it's LED.
    led_code( status_led_gpio, IDENTIFY_ACCESSORY);
    vTaskDelete(NULL);
}

void identify(homekit_value_t _value) {
    printf("Identify\n");
    xTaskCreate(identify_task, "Identify", 128, NULL, tskIDLE_PRIORITY+1, NULL);
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
    xTaskCreate(reset_configuration_task, "Reset configuration", 256, NULL, tskIDLE_PRIORITY+1, NULL);
}


void reset_button_callback(uint8_t gpio, void* args, uint8_t param) {
    printf("Reset Button event long press on GPIO : %d\n", gpio);
    reset_configuration();
    
}


void create_accessory_name(const char* name, const char* model, homekit_characteristic_t *accessory_name, homekit_characteristic_t *accessory_serial){
    
    printf("%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
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
    
    
    accessory_name->value = HOMEKIT_STRING(name_value);
    accessory_serial->value = HOMEKIT_STRING(serialNumberValue);
    
    printf("%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

}


void on_homekit_event(homekit_event_t event) {
    
    switch (event) {
        case HOMEKIT_EVENT_SERVER_INITIALIZED:
            printf("%s: Server initialised, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            if (homekit_is_paired()){
                /* if server has started and we already have a pairing then initialise*/
                accessory_paired = true;
                printf("%s: Acessory is paired on initialisation, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
                accessory_init ();
                led_code( status_led_gpio, WIFI_CONNECTED);
                wifi_check_stop_start (wifi_check_interval.value.int_value);
            }
            else
            {
                printf("%s: Acessory is NOT paired on initialisation, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
                accessory_paired = false;
                /* stop wifi check to reduce interference with pairing*/
                accessory_init_not_paired ();
                wifi_check_stop_start (0);
            }
            break;
        case HOMEKIT_EVENT_CLIENT_CONNECTED:
            printf("%s: Client connected, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            break;
        case HOMEKIT_EVENT_CLIENT_VERIFIED:
            printf("%s: Client verified, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            /* we weren't paired on started up but we now are */
            if (!accessory_paired ){
                accessory_paired = true;
                printf("%s: Acessory is paired on after client validaiton, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
                accessory_init();
                led_code( status_led_gpio, WIFI_CONNECTED);
                wifi_check_stop_start (wifi_check_interval.value.int_value);
            }
            break;
        case HOMEKIT_EVENT_CLIENT_DISCONNECTED:
            printf("%s: Client disconnected, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            break;
        case HOMEKIT_EVENT_PAIRING_ADDED:
            printf("%s: Pairing added, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            break;
        case HOMEKIT_EVENT_PAIRING_REMOVED:
            printf("%s: Pairing removed, Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
            if (!homekit_is_paired()){
            /* if we have no more pairings then restart */
                printf("%s: no more pairings so restart\n", __func__);
                accessory_paired = false;
                sdk_system_restart();
                wifi_check_stop_start (0);
            }
            break;
        default:
            printf("%s: Default event %d,  Free Heap=%d\n", __func__, event, xPortGetFreeHeapSize());
    }
    printf("%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
}


void on_wifi_ready ( void) {
    
    udplog_init(tskIDLE_PRIORITY+1);
    printf("%s: UDP Log Init Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    printf("%s: Start, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    get_sysparam_info();
    reset_information = sdk_system_get_rst_info();
    switch (reset_information->reason){
        case DEFAULT_RST:
            printf ("%s Reset Reason: Default Reset\n", __func__);
            break;
        case WDT_RST:
            printf ("%s Reset Reason: Whatchdog Rest\n", __func__);
            recover_from_reset (reset_information->reason);
            break;
        case EXCEPTION_RST:
            printf ("%s Reset Reason: Excepton Rest\n", __func__);
            recover_from_reset (reset_information->reason);
            break;
        case SOFT_RST:
            printf ("%s Reset Reason: Soft Reeet\n", __func__);
            break;
        default:
            printf ("%s Reset Reason: Unknown\n", __func__);
            
    }
    printf ("%s: Exception Cause: %d\nEPC 1: %d\nEPC 2: %d\nEPC 3: %d\nExv virtul address: %d\nDEPC: %d\nReturn Address:%d\n", __func__,  reset_information->exccause, reset_information->epc1, reset_information->epc2, reset_information->epc3, reset_information->excvaddr, reset_information->depc,reset_information->rtn_addr);
    
    printf("%s: Calling homekit_server_init, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    homekit_server_init(&config);
    printf("%s: After calling homekit_Server_init, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    printf("%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
}


void standard_init (homekit_characteristic_t *name, homekit_characteristic_t *manufacturer, homekit_characteristic_t *model, homekit_characteristic_t *serial, homekit_characteristic_t *revision){

    printf("%s: Start, SDK version: %s, Freep Heap=%d\n", __func__, sdk_system_get_sdk_version(), xPortGetFreeHeapSize());

    rboot_rtc_data rtc;

    if (rboot_get_rtc_data(&rtc)) {
        power_cycle_count = rtc.temp_rom;
        printf("%s: RTC power cycle count = %d\n", __func__, power_cycle_count);

    } else {
        printf("%s: Error reading RTC\n", __func__);
    }
    printf("%s: RTC Data, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    uart_set_baud(0, 115200);
    printf("%s: UART, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    get_sysparam_info();
    
    load_characteristic_from_flash (&wifi_check_interval);
    load_characteristic_from_flash (&ota_beta);
    load_characteristic_from_flash (&lcm_beta);
    load_characteristic_from_flash (&preserve_state);

    printf("%s: Load charactersitics Free Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
        create_accessory_name(name->value.string_value, model->value.string_value, name, serial);
    
    int c_hash=ota_read_sysparam(&manufacturer->value.string_value,&serial->value.string_value,
                                 &model->value.string_value,&revision->value.string_value);
    if (c_hash==0) c_hash=1;
    config.accessories[0]->config_number=c_hash;
    printf("%s: ota_read_sysparams Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
    /*wifi_check_stop_start (wifi_check_interval.value.int_value);*/

    sdk_os_timer_setfn(&save_timer, save_characteristics, NULL);
    printf("%s: Save Timer Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
    
    printf("%s: End, Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

}

