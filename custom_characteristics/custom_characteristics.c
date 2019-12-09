#include <sysparam.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <custom_characteristics.h>



void print_binary_value(char *key, uint8_t *value, size_t len) {
    size_t i;
    
    printf("  %s:", key);
    for (i = 0; i < len; i++) {
        if (!(i & 0x0f)) {
            printf("\n   ");
        }
        printf(" %02x", value[i]);
    }
    printf("\n");
}

void get_sysparam_info() {
    uint32_t base_addr,num_sectors;
    sysparam_iter_t sysparam_iter;
    sysparam_status_t sysparam_status;
    
    sysparam_get_info(&base_addr, &num_sectors);
    
    printf ("get_sysparam_info - Sysparam base address %i, num_sectors %i\n", base_addr, num_sectors);
    sysparam_status = sysparam_iter_start (&sysparam_iter);
    if (sysparam_status != 0){
        printf("get_sysparam_info - iter_start status %d\n",sysparam_status);
    }
    while (sysparam_status==0){
        sysparam_status = sysparam_iter_next (&sysparam_iter);
        if (sysparam_status==0){
            if (!sysparam_iter.binary)
                if (strncmp(sysparam_iter.key,"wifi_password", 13 ) == 0) {
                    if (strlen (sysparam_iter.value) == 0){
                        printf("get_sysparam_info - sysparam name: %s, value: blank\n", sysparam_iter.key);
                    } else {
                        printf("get_sysparam_info - sysparam name: %s, value:%s\n", sysparam_iter.key,"**********");
                    }
                } else {
                    printf("get_sysparam_info - sysparam name: %s, value:%s\n", sysparam_iter.key, (char *)sysparam_iter.value);
                }
                else
                    print_binary_value(sysparam_iter.key, sysparam_iter.value, sysparam_iter.value_len);
        } else {
            printf("get_sysparam_info - while loop status %d\n",sysparam_status);
            
        }
    }
    sysparam_iter_end (&sysparam_iter);
    if (sysparam_status != SYSPARAM_NOTFOUND) {
        //   SYSPARAM_NOTFOUND is the normal status when we've reached the end of all entries.
        printf ("get_sysparam_info - sysparam iter_end error:%d\n", sysparam_status);
    }
}



void save_characteristic_to_flash(homekit_characteristic_t *ch, homekit_value_t value){
    
    sysparam_status_t status = SYSPARAM_OK;
    bool bool_value;
    int8_t int8_value;
    int32_t int32_value;
    float float_value;
    char *string_value=NULL;    

    printf ("Save characteristic to flash %s\n",ch->description);
    switch (ch->format) {
        case homekit_format_bool:
            printf ("writing bool value to flash %s\n", ch->value.bool_value ? "true" : "false");
            status = sysparam_get_bool(ch->description, &bool_value);
            if (status == SYSPARAM_OK && bool_value != ch->value.bool_value) {
                status = sysparam_set_bool(ch->description, ch->value.bool_value);
            } else if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_bool(ch->description, ch->value.bool_value);
            }
            break;
        case homekit_format_uint8:
            printf ("writing int8 value to flash %d\n", ch->value.int_value);
            status = sysparam_get_int8(ch->description, &int8_value);
            if (status == SYSPARAM_OK && int8_value != ch->value.int_value) {
                status = sysparam_set_int8(ch->description, ch->value.int_value);
            } else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_int8(ch->description, ch->value.int_value);
            }
            break;
        case homekit_format_uint16:
        case homekit_format_uint32:
            printf ("writing int32 value to flash %d\n",  ch->value.int_value);
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK && int32_value != ch->value.int_value) {
                status = sysparam_set_int32(ch->description, ch->value.int_value);
            } else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_int32(ch->description, ch->value.int_value);
            }
            break;
        case homekit_format_string:
            printf ("writing string value to flash %s\n", ch->value.string_value);
            status = sysparam_get_string(ch->description, &string_value);
            if (status == SYSPARAM_OK && !strcmp (string_value, ch->value.string_value)) {
                status = sysparam_set_string(ch->description, ch->value.string_value);
            }  else  if (status == SYSPARAM_NOTFOUND) {
                status = sysparam_set_string(ch->description, ch->value.string_value);
            }
            free(string_value);
            break;
        case homekit_format_float:
            printf ("writing float value to flash %f\n", ch->value.float_value);
            status = sysparam_get_int32(ch->description, &int32_value);
            float_value = int32_value * 1.00f / 100;
            if (status == SYSPARAM_OK && float_value != ch->value.float_value) {
		int32_value = (int)ch->value.float_value*100;
		status = sysparam_set_int32(ch->description, int32_value);
            } else if (status == SYSPARAM_NOTFOUND) {
		int32_value = (int)ch->value.float_value*100;
                status = sysparam_set_int32(ch->description, int32_value);
            }
	    break;
        case homekit_format_uint64:
        case homekit_format_int:
        case homekit_format_tlv:
        default:
            printf ("Unknown characteristic format in save_charactersitics_to_flash\n");
    }
    if (status != SYSPARAM_OK){
        printf ("Error in sysparams error:%i writing characteristic\n", status);
    }
    
}

void load_characteristic_from_flash (homekit_characteristic_t *ch){
              
                    
    sysparam_status_t status = SYSPARAM_OK;
    bool bool_value;
    int8_t int8_value;
    int32_t int32_value;
    char *string_value = NULL;
    printf ("Loading sysparam %s\n",ch->description);
    switch (ch->format){
        case homekit_format_bool:
            printf("Loading bool ");
            status = sysparam_get_bool(ch->description, &bool_value);
            if (status == SYSPARAM_OK ) {
                ch->value.bool_value = bool_value;
                printf("%s\n", ch->value.bool_value ? "true" : "false");
            }
            break;
        case homekit_format_uint8:
            printf("Loading int8 ");
            status = sysparam_get_int8(ch->description, &int8_value);
            if (status == SYSPARAM_OK) {
                ch->value.int_value = int8_value;
                printf("%d\n", ch->value.int_value);
            }
            break;
        case homekit_format_uint16:
        case homekit_format_uint32:
            printf("Loading in32 ");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK ) {
                ch->value.int_value = int32_value;
                printf("%d\n", ch->value.int_value);
            }
            break;
        case homekit_format_string:
            printf("Loading string ");
            status = sysparam_get_string(ch->description, &string_value);
            if (status == SYSPARAM_OK) {
                ch->value = HOMEKIT_STRING(string_value);
                printf("%s\n", string_value);
            }
            break;
        case homekit_format_float:
            printf("Loading float ");
            status = sysparam_get_int32(ch->description, &int32_value);
            if (status == SYSPARAM_OK ) {
                ch->value.float_value = int32_value * 1.0f /100;
                printf("%f\n", ch->value.float_value);
                }
            break;
        case homekit_format_uint64:
        case homekit_format_int:
        case homekit_format_tlv:
        default:
            printf ("Unknown characteristic format in save_charactersitics_to_flash\n");
    }
    if (status != SYSPARAM_OK){
        printf ("Error in sysparams error:%i loading characteristic\n", status);
    }
    
    
    
}
