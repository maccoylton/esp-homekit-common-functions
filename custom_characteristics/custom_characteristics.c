#include <sysparam.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <flashchip.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <custom_characteristics.h>
#include <shared_functions.h>



void homekit_characteristic_bounds_check (homekit_characteristic_t *ch){
    
    LOG(LOG_FLOW, "%s: %s: ",__func__, ch->description);
    switch (ch->format) {
        case homekit_format_bool:
            LOG(LOG_FLOW, "Checking boolean bounds");
            if ((ch->value.bool_value != 0) && (ch->value.bool_value != 1)){
                LOG(LOG_FLOW, " Out of bounds setting to false");
                ch->value.bool_value = false;
            }
            break;
        case homekit_format_uint8:
            LOG(LOG_FLOW, "Checking uint8 bounds");
            if (ch->value.int_value > *ch->max_value){
                LOG(LOG_FLOW, " Greater than max, setting to max");
                ch->value.int_value = *ch->max_value;
            }
            if (ch->value.int_value < *ch->min_value){
                LOG(LOG_FLOW, " Lower than min, setting to min");
                ch->value.int_value = *ch->min_value;
            }
            break;
        case homekit_format_int:
        case homekit_format_uint16:
        case homekit_format_uint32:
            LOG(LOG_FLOW, "Checking integer bounds");
            if (ch->value.int_value > *ch->max_value){
                LOG(LOG_FLOW, " Greater than max, setting to max");
                ch->value.int_value = *ch->max_value;
            }
            if (ch->value.int_value < *ch->min_value){
                LOG(LOG_FLOW, " Lower than min, setting to min");
                ch->value.int_value = *ch->min_value;
            }
            break;
        case homekit_format_string:
            break;
        case homekit_format_float:
            LOG(LOG_FLOW, "Checking float bounds");
            if (ch->value.float_value > *ch->max_value){
                LOG(LOG_FLOW, " Greater than max, setting to max");
                ch->value.float_value = *ch->max_value;
            }
            if (ch->value.float_value < *ch->min_value){
                LOG(LOG_FLOW, " Lower than min, setting to min");
                ch->value.float_value = *ch->min_value;
            }
            break;
        case homekit_format_uint64:
        case homekit_format_tlv:
        default:
            LOG(LOG_ERR, "Unknown characteristic format\n");
    }
    LOG(LOG_FLOW, "\n");
}



void print_binary_value(char *key, uint8_t *value, size_t len) {
    size_t i;
    
    LOG(LOG_FLOW, "  %s:", key);
    for (i = 0; i < len; i++) {
        if (!(i & 0x0f)) {
            LOG(LOG_FLOW, "\n   ");
        }
        LOG(LOG_FLOW, " %02x", value[i]);
    }
    LOG(LOG_FLOW, "\n");
}


void get_sysparam_info() {
    
    LOG(LOG_MEM, "%s: , Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());

    static uint32_t base_addr,num_sectors;
    static sysparam_iter_t sysparam_iter;
    static sysparam_status_t sysparam_status;
    
    sysparam_get_info(&base_addr, &num_sectors);
    
    LOG(LOG_FLOW, "%s: Sysparam base address %i, num_sectors %i\n", __func__, base_addr, num_sectors);
    
    uint32_t lcm_sysparam = 0xF7000;
    uint32_t app_sysparam = sdk_flashchip.chip_size - 9 * sdk_flashchip.sector_size;
    if (app_sysparam != lcm_sysparam)
        LOG(LOG_ERR, "%s: Sysparam mismatch! app 0x%X vs LCM 0x%X (flash %dMB) - use -fs 1MB\n",
            __func__, app_sysparam, lcm_sysparam, sdk_flashchip.chip_size >> 20);
    sysparam_status = sysparam_iter_start (&sysparam_iter);
    if (sysparam_status != 0){
        LOG(LOG_ERR, "%s: iter_start status %d\n",__func__, sysparam_status);
    }
    while (sysparam_status==0){
        sysparam_status = sysparam_iter_next (&sysparam_iter);
        if (sysparam_status==0){
            if (!sysparam_iter.binary)
                if (strncmp(sysparam_iter.key,"wifi_password", 13 ) == 0) {
                    if (strlen ((char *)sysparam_iter.value) == 0){
                        LOG(LOG_FLOW, "%s: sysparam name: %s, value: blank\n", __func__, sysparam_iter.key);
                    } else {
                        LOG(LOG_FLOW, "%s: sysparam name: %s, value:%s\n", __func__, sysparam_iter.key,"**********");
                    }
                } else {
                    LOG(LOG_FLOW, "%s: sysparam name: %s, value:%s, key length:%d, value length:%d\n", __func__, sysparam_iter.key, (char *)sysparam_iter.value, sysparam_iter.key_len, sysparam_iter.value_len);
                }
                else
                    print_binary_value(sysparam_iter.key, sysparam_iter.value, sysparam_iter.value_len);
        } else {
            LOG(LOG_FLOW, "%s: while loop status %d\n",__func__, sysparam_status);
        }
    }
    sysparam_iter_end (&sysparam_iter);
    if (sysparam_status != SYSPARAM_NOTFOUND) {
        //   SYSPARAM_NOTFOUND is the normal status when we've reached the end of all entries.
        LOG(LOG_ERR, "%s: sysparam iter_end error:%d\n", __func__, sysparam_status);
    }
    LOG(LOG_MEM, "%s: , Freep Heap=%d\n", __func__, xPortGetFreeHeapSize());
}


void save_int32_param ( const char *description, int32_t new_value){

    static sysparam_status_t status = SYSPARAM_OK;
    static int32_t current_value;
    
    status = sysparam_get_int32(description, &current_value);
    if (status == SYSPARAM_OK && current_value != new_value) {
        status = sysparam_set_int32(description, new_value);
    } else  if (status == SYSPARAM_NOTFOUND) {
        status = sysparam_set_int32(description, new_value);
    }
}


void save_float_param ( const char *description, float new_float_value){
    
    static sysparam_status_t status = SYSPARAM_OK;
    static int32_t current_value, new_value;
    
    new_value = (int) (new_float_value*100);
    
    status = sysparam_get_int32(description, &current_value);
    
    switch (status) {
        case SYSPARAM_OK:
            if (current_value != new_value) {
                status = sysparam_set_int32(description, new_value);
                LOG(LOG_FLOW, "%s: description: %s, value: %f, stored: %d\n", __func__, description, new_float_value, new_value);
            } else {
                LOG(LOG_MEM, "%s: No change to value no update required, description: %s, value: %f\n", __func__, description, new_float_value);
            }
            break;
        case SYSPARAM_NOTFOUND:
            status = sysparam_set_int32(description, new_value);
            LOG(LOG_FLOW, "%s: description: %s, value: %f\n", __func__, description, new_float_value);
            break;
        default:{
            LOG(LOG_ERR, "%s: error search for sysparam, %s, error no: %d\n", __func__, description, status);
        }
    }
}



void save_characteristic_to_flash(homekit_characteristic_t *ch, homekit_value_t value){
    
    static sysparam_status_t status = SYSPARAM_OK;
    static bool bool_value;
    static int8_t int8_value;
    static char *string_value=NULL;

    LOG(LOG_FLOW, "%s: %s: ",__func__, ch->description);
    switch (ch->format) {
        case homekit_format_bool:
            LOG(LOG_FLOW, "writing bool value to flash %s\n", ch->value.bool_value ? "true" : "false");
            status = sysparam_get_bool(ch->description, &bool_value);
            if (status == SYSPARAM_OK && bool_value != ch->value.bool_value) {
                status = sysparam_set_bool(ch->description, ch->value.bool_value);
            } else if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_bool(ch->description, ch->value.bool_value);
            } 
            break;
        case homekit_format_uint8:
            LOG(LOG_FLOW, "writing int8 value to flash %d\n", ch->value.int_value);
            status = sysparam_get_int8(ch->description, &int8_value);
            if (status == SYSPARAM_OK && int8_value != ch->value.int_value) {
                status = sysparam_set_int8(ch->description, ch->value.int_value);
            } else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_int8(ch->description, ch->value.int_value);
            }
            break;
        case homekit_format_int:
        case homekit_format_uint16:
        case homekit_format_uint32:
            LOG(LOG_FLOW, "writing int32 value to flash %d\n",  ch->value.int_value);
            save_int32_param (ch->description, ch->value.int_value);
            break;
        case homekit_format_string:
            LOG(LOG_FLOW, "writing string value to flash %s\n", ch->value.string_value);
            status = sysparam_get_string(ch->description, &string_value);
            if (status == SYSPARAM_OK && strcmp (string_value, ch->value.string_value) != 0) {
                status = sysparam_set_string(ch->description, ch->value.string_value);
            }  else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_string(ch->description, ch->value.string_value);
            }
            free(string_value);
            break;
        case homekit_format_float:
            LOG(LOG_FLOW, "writing float value to flash %f\n", ch->value.float_value);
            save_float_param (ch->description,ch->value.float_value);
            break;
        case homekit_format_uint64:
        case homekit_format_tlv:
        default:
            LOG(LOG_ERR, "Unknown characteristic format\n");
    }
    if (status != SYSPARAM_OK){
        LOG(LOG_ERR, "%s: Error in sysparams error:%i\n", __func__, status);
    }
    
}



void load_float_param ( const char *description, float *new_float_value){
    
    static sysparam_status_t status = SYSPARAM_OK;
    static int32_t int32_value;
    
    status = sysparam_get_int32(description, &int32_value);
    
    if (status == SYSPARAM_OK ) {
        *new_float_value = int32_value * 1.0f / 100;
        LOG(LOG_FLOW, "%s: %s value %f, stored: %d\n", __func__, description, *new_float_value, int32_value);
    }
}


void load_characteristic_from_flash (homekit_characteristic_t *ch){
    
    static sysparam_status_t status = SYSPARAM_OK;
    static bool bool_value;
    static int8_t int8_value;
    static int32_t int32_value;
    static char *string_value = NULL;
    
    LOG(LOG_FLOW, "%s: %s: ",__func__, ch->description);
    switch (ch->format){
        case homekit_format_bool:
            LOG(LOG_FLOW, "bool: ");
            status = sysparam_get_bool(ch->description, &bool_value);
            if (status == SYSPARAM_OK ) {
                ch->value.bool_value = bool_value;
                LOG(LOG_FLOW, "%d %s\n", ch->value.bool_value , ch->value.bool_value ? "true" : "false");
                homekit_characteristic_bounds_check(ch);
            }
            break;
        case homekit_format_uint8:
            LOG(LOG_FLOW, "uint8: ");
            status = sysparam_get_int8(ch->description, &int8_value);
            if (status == SYSPARAM_OK) {
                ch->value.int_value = int8_value;
                LOG(LOG_FLOW, "%d\n", ch->value.int_value);
                homekit_characteristic_bounds_check(ch);
            }
            break;
        case homekit_format_int:
            LOG(LOG_FLOW, "int: ");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK ) {
                ch->value.int_value = (int)int32_value;
                LOG(LOG_FLOW, "%d\n", ch->value.int_value);
                homekit_characteristic_bounds_check(ch);
            }
            break;
        case homekit_format_uint16:
            LOG(LOG_FLOW, "uint16: ");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK ) {
                ch->value.int_value = (uint16_t)int32_value;
                LOG(LOG_FLOW, "%d\n", ch->value.int_value);
                homekit_characteristic_bounds_check(ch);
            }
            break;
        case homekit_format_uint32:
            LOG(LOG_FLOW, "uint32: ");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK ) {
                ch->value.int_value = int32_value;
                LOG(LOG_FLOW, "%d\n", ch->value.int_value);
                homekit_characteristic_bounds_check(ch);
            }
            break;
        case homekit_format_string:
            LOG(LOG_FLOW, "string: ");
            status = sysparam_get_string(ch->description, &string_value);
            if (status == SYSPARAM_OK) {
                ch->value = HOMEKIT_STRING(string_value);
                LOG(LOG_FLOW, "%s\n", string_value);
                homekit_characteristic_bounds_check(ch);
            }
            break;
        case homekit_format_float:
            LOG(LOG_FLOW, "float: ");
            load_float_param ( ch->description, &ch->value.float_value);
            homekit_characteristic_bounds_check(ch);
            break;
        case homekit_format_uint64:
        case homekit_format_tlv:
        default:
            LOG(LOG_ERR, "%s: Unknown characteristic format\n", __func__);
    }
    if (status != SYSPARAM_OK){
        LOG(LOG_ERR, "%s: Error in sysparams error:%i loading characteristic\n", __func__, status);
    }
}
