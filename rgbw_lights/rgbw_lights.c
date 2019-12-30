//
//
//  rgbw_lights.c
//
//  Created by David B Brown on 30/12/2019
//  Copyright © 2018 maccoylton. All rights reserved.
//
/*
 * This is an a library used to implment RGBW lights and strips with esp-homekit
 *
 * more info about the controller and flashing can be found here:
 *
 */

#include <rgbw_lights.h>

void save_characteristics ( ){
    
    printf ("%s", __func__);
    save_characteristic_to_flash(&on, on.value);
    save_characteristic_to_flash(&saturation, saturation.value);
    save_characteristic_to_flash(&hue, hue.value);
    save_characteristic_to_flash(&brightness, brightness.value);
    
}

void update_pins_and_save (homekit_characteristic_t* r_gpio, homekit_characteristic_t* g_gpio, homekit_characteristic_t* b_gpio, homekit_characteristic_t* w_gpio,int r_pin, int g_pin, int b_pin, int w_pin) {
    
    if ( r_gpio->value.int_value != r_pin) {
        r_gpio->value = HOMEKIT_INT(r_pin);
        save_characteristic_to_flash(r_gpio,r_gpio->value );
    }
    
    if ( g_gpio->value.int_value != g_pin) {
        g_gpio->value = HOMEKIT_INT(g_pin);
    }
    
    if ( b_gpio->value.int_value != b_pin){
        b_gpio->value = HOMEKIT_INT(b_pin);
    }
    
    if (w_gpio->value.int_value != w_pin){
        w_gpio->value = HOMEKIT_INT(w_pin);
        
    }
    
    save_characteristic_to_flash(r_gpio,r_gpio->value );
    save_characteristic_to_flash(g_gpio,g_gpio->value );
    save_characteristic_to_flash(b_gpio,b_gpio->value );
    save_characteristic_to_flash(w_gpio,w_gpio->value );
    
    multipwm_stop(&pwm_info);
    multipwm_set_pin(&pwm_info, white_pin, w_pin );
    multipwm_set_pin(&pwm_info, red_pin, r_pin );
    multipwm_set_pin(&pwm_info, green_pin, g_pin);
    multipwm_set_pin(&pwm_info, blue_pin, b_pin);
    if (led_on==true){
        multipwm_start(&pwm_info);
        sdk_os_timer_arm (&rgbw_set_timer, RGBW_STRIP_SET_DELAY, 0 );
    } else {
        sdk_os_timer_disarm (&rgbw_set_timer );
    }
    
    printf ("%s: Set gpios as follows : W=%d, R=%d, G=%d, B=%d\n",__func__, w_pin, r_pin, b_pin, g_pin);
}

void gpio_update_set( ){
    if (red_gpio.value.int_value == green_gpio.value.int_value ||
        red_gpio.value.int_value == blue_gpio.value.int_value ||
        red_gpio.value.int_value == white_gpio.value.int_value ||
        green_gpio.value.int_value == blue_gpio.value.int_value ||
        green_gpio.value.int_value == white_gpio.value.int_value ||
        blue_gpio.value.int_value == white_gpio.value.int_value)
    {
        printf("%s: Some of the GPIOs are equal, so default_rgbw_pins\n", __func__);
        update_pins_and_save (&red_gpio, &green_gpio, &blue_gpio, &white_gpio, red_default_gpio, green_default_gpio, blue_default_gpio, white_default_gpio );
        printf ("%s: Set Default GPIOS W=%d, R=%d, G=%d, B=%d\n",__func__,white_default_gpio, red_default_gpio, green_default_gpio, blue_default_gpio );
        
    } else {
        
        update_pins_and_save (&red_gpio, &green_gpio, &blue_gpio, &white_gpio, red_gpio.value.int_value , green_gpio.value.int_value, blue_gpio.value.int_value , white_gpio.value.int_value );
        
        printf ("%s: Set gpios as follows : W=%d, R=%d, G=%d, B=%d\n",__func__, white_gpio.value.int_value,red_gpio.value.int_value, green_gpio.value.int_value, blue_gpio.value.int_value );
        sdk_os_timer_arm (&rgbw_set_timer, RGBW_STRIP_SET_DELAY, 0 );
    }
}

void on_update (homekit_characteristic_t *ch, homekit_value_t value, void *context    ){
    sdk_os_timer_arm (&gpio_timer,5000 , 0 );
}

void rgbw_set (){
    
    printf("\n%s\n", __func__);
    printf("%s: Current colour before set r=%d,g=%d, b=%d, w=%d,\n",__func__, current_color.red,current_color.green, current_color.blue, current_color.white );
    if (led_on) {
        // convert HSI to RGBW
        
        HSVtoRGB(led_hue, led_saturation, led_brightness, &target_color);
        printf("%s: h=%d,s=%d,b=%d => r=%d,g=%d, b=%d\n",__func__, (int)led_hue,(int)led_saturation,(int)led_brightness, target_color.red,target_color.green, target_color.blue );
        
        RBGtoRBGW (&target_color);
        printf("%s: h=%d,s=%d,b=%d => r=%d,g=%d, b=%d, w=%d,\n",__func__, (int)led_hue,(int)led_saturation,(int)led_brightness, target_color.red,target_color.green, target_color.blue, target_color.white );
        
        current_color.red = target_color.red * PWM_SCALE;
        current_color.green = target_color.green * PWM_SCALE;
        current_color.blue = target_color.blue * PWM_SCALE;
        current_color.white = target_color.white * PWM_SCALE;
        
        printf ("%s: Stopping multipwm \n",__func__);
        multipwm_stop(&pwm_info);
        multipwm_set_duty(&pwm_info, white_pin, current_color.white);
        multipwm_set_duty(&pwm_info, blue_pin, current_color.blue);
        multipwm_set_duty(&pwm_info, green_pin, current_color.green);
        multipwm_set_duty(&pwm_info, red_pin, current_color.red);
        multipwm_start(&pwm_info);
        printf ("%s: Starting multipwm \n\n",__func__);
        
    } else {
        printf("%s: led srtip off\n", __func__);
        multipwm_stop(&pwm_info);
    }
    
    printf("%s:Current colour after set r=%d,g=%d, b=%d, w=%d,\n",__func__, current_color.red,current_color.green, current_color.blue, current_color.white );
    
}


homekit_value_t led_on_get() {
    return HOMEKIT_BOOL(led_on);
}

void led_on_set(homekit_value_t value) {
    if (value.format != homekit_format_bool) {
        printf("%s: Invalid on-value format: %d\n", __func__, value.format);
        return;
    }
    
    led_on = value.bool_value;
    if (led_on == false )
    {
        printf ("%s: Led on false so stopping Multi PWM\n", __func__);
        multipwm_stop(&pwm_info);
        sdk_os_timer_disarm (&rgbw_set_timer);
    } else
    {
        printf ("%s: Led on TRUE so setting colour\n", __func__);
        sdk_os_timer_arm (&rgbw_set_timer, RGBW_STRIP_SET_DELAY, 0 );
    }
    
}

homekit_value_t led_brightness_get() {
    return HOMEKIT_INT(led_brightness);
}

void led_brightness_set(homekit_value_t value) {
    if (value.format != homekit_format_int) {
        printf("%s: Invalid brightness-value format: %d\n", __func__, value.format);
        return;
    }
    led_brightness = value.int_value;
    printf ("%s: timer armed, Brightness: %f\n", __func__, led_brightness);
    sdk_os_timer_arm (&rgbw_set_timer, RGBW_STRIP_SET_DELAY, 0 );
}

homekit_value_t led_hue_get() {
    return HOMEKIT_FLOAT(led_hue);
}

void led_hue_set(homekit_value_t value) {
    if (value.format != homekit_format_float) {
        printf("%s: Invalid hue-value format: %d\n", __func__, value.format);
        return;
    }
    led_hue = value.float_value;
    printf ("%s: timer armed, HUE: %f\n", __func__, led_hue);
    sdk_os_timer_arm (&rgbw_set_timer, RGBW_STRIP_SET_DELAY, 0 );
    
}

homekit_value_t led_saturation_get() {
    return HOMEKIT_FLOAT(led_saturation);
}

void led_saturation_set(homekit_value_t value) {
    if (value.format != homekit_format_float) {
        printf("%s: Invalid sat-value format: %d\n", __func__, value.format);
        return;
    }
    led_saturation = value.float_value;
    printf ("%s: timer armed, Saturation: %f\n", __func__, led_saturation);
    sdk_os_timer_arm (&rgbw_set_timer, RGBW_STRIP_SET_DELAY, 0 );
}


void rgbw_lights_init() {
    
    /* load saved values frm pervious runs */
    load_characteristic_from_flash(&white_gpio);
    load_characteristic_from_flash(&red_gpio);
    load_characteristic_from_flash(&green_gpio);
    load_characteristic_from_flash(&blue_gpio);
    
    /* set up multipwm */
    pwm_info.channels = 4;     
    multipwm_init(&pwm_info);
    multipwm_set_freq(&pwm_info, 65535);
    multipwm_set_pin(&pwm_info, white_pin, white_gpio.value.int_value );
    multipwm_set_pin(&pwm_info, red_pin, red_gpio.value.int_value );
    multipwm_set_pin(&pwm_info, green_pin, green_gpio.value.int_value );
    multipwm_set_pin(&pwm_info, blue_pin, blue_gpio.value.int_value );
    
    printf ("%s: Set gpios as follows : W=%d, R=%d, G=%d, B=%d\n",__func__, white_gpio.value.int_value,red_gpio.value.int_value, green_gpio.value.int_value, blue_gpio.value.int_value );

     
     /* load last saved HSI colour */
    load_characteristic_from_flash(&saturation);
    load_characteristic_from_flash(&hue);
    load_characteristic_from_flash(&brightness);
    
    sdk_os_timer_setfn(&rgbw_set_timer, rgbw_set, NULL);
    sdk_os_timer_setfn(&gpio_timer, gpio_update_set, NULL);
    
    printf ("%s: sdk_os_timer_Setfn called\n", __func__);
    
}
