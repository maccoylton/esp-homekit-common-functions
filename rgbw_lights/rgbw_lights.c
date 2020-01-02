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

TaskHandle_t colours_effect_handle = NULL;
float saved_brightness = 0;
int previous_colour_effect = off_effect;

void save_characteristics ( ){
    
    printf ("%s:\n", __func__);
    save_characteristic_to_flash(&on, on.value);
    save_characteristic_to_flash(&saturation, saturation.value);
    save_characteristic_to_flash(&hue, hue.value);
    save_characteristic_to_flash(&brightness, brightness.value);
    save_characteristic_to_flash(&wifi_check_interval, wifi_check_interval.value);
}

IRAM void set_colours (uint16_t red_colour, uint16_t green_colour, uint16_t blue_colour, uint16_t white_colour){

    printf ("%s: Stopping multipwm\n",__func__);
    multipwm_stop(&pwm_info);
    multipwm_set_duty(&pwm_info, white_pin, white_colour);
    multipwm_set_duty(&pwm_info, blue_pin, blue_colour);
    multipwm_set_duty(&pwm_info, green_pin, green_colour);
    multipwm_set_duty(&pwm_info, red_pin, red_colour);
    multipwm_start(&pwm_info);
    printf ("%s: Starting multipwm\n",__func__);
}


void cycle_colours_task(){
    
    printf ("%s: \n", __func__);
    colours_gpio_test.value.bool_value = true;
    homekit_characteristic_notify(&colours_gpio_test,colours_gpio_test.value );
    while (1){
        set_colours (COLOUR_MAX, 0, 0, 0);
        vTaskDelay (EFFECT_DELAY);
        set_colours ( 0, COLOUR_MAX, 0, 0);
        vTaskDelay (EFFECT_DELAY);
        set_colours (0, 0, COLOUR_MAX, 0);
        vTaskDelay (EFFECT_DELAY);
        set_colours (0, 0, 0, COLOUR_MAX);
        vTaskDelay (EFFECT_DELAY);
    }
}


homekit_value_t colours_gpio_test_get ( ) {
    return (HOMEKIT_BOOL(colours_gpio_test.value.bool_value));
}

void strobe_colours_task(){
    
    printf ("%s: \n", __func__);
    colours_strobe.value.bool_value = true;
    homekit_characteristic_notify(&colours_strobe,colours_strobe.value );
    while (1){
        set_colours (COLOUR_MAX, COLOUR_MAX, COLOUR_MAX, COLOUR_MAX);
        vTaskDelay (FIFTY_MS/portTICK_PERIOD_MS);
        set_colours (0, 0, 0, 0);
        vTaskDelay (ONE_S/portTICK_PERIOD_MS);
    }
}


homekit_value_t colours_strobe_get ( ) {
    return (HOMEKIT_BOOL(colours_strobe.value.bool_value));
}


void flash_colours_task(){
    
    printf ("%s: \n", __func__);
    colours_flash.value.bool_value = true;
    homekit_characteristic_notify(&colours_flash,colours_flash.value );

    while (1){
        multipwm_stop(&pwm_info);
        vTaskDelay (ONE_S/portTICK_PERIOD_MS);
        multipwm_start(&pwm_info);
        vTaskDelay (ONE_S/portTICK_PERIOD_MS);
    }
}


homekit_value_t colours_flash_get ( ) {
    return (HOMEKIT_BOOL(colours_flash.value.bool_value));
}


void fade_colours_task(){

    float fade_factor = 50;
    int fade_r, fade_g, fade_b, fade_w, r, g, b, w, i;
    
    printf ("%s: \n", __func__);
    colours_fade.value.bool_value = true;
    homekit_characteristic_notify(&colours_fade,colours_fade.value );
    
    r = current_color.red;
    g = current_color.green;
    b = current_color.blue;
    w = current_color.white;
    fade_r = r /fade_factor;
    fade_g = g /fade_factor;
    fade_b = b /fade_factor;
    fade_w = w /fade_factor;
    printf("%s:Current colour after set r=%d, g=%d, b=%d, w=%d, r f=%d, g f=%d, b f=%d, w f=%d\n",__func__, r, g, b, w, fade_r, fade_g, fade_b, fade_w );
    while (1){

        for (i=0; i< fade_factor ; i++)
        {
            r -= fade_r;
            g -= fade_g;
            b -= fade_b;
            w -= fade_w;
            set_colours (r, g, b, w);
            vTaskDelay (TEN_MS/portTICK_PERIOD_MS);
        }

        vTaskDelay (FIFTY_MS/portTICK_PERIOD_MS);
        
        for (i=0; i< fade_factor ; i++)
        {
            r += fade_r;
            g += fade_g;
            b += fade_b;
            w += fade_w;
            set_colours (r, g, b, w);
            vTaskDelay (TEN_MS/portTICK_PERIOD_MS);
        }
        
        vTaskDelay (FIFTY_MS/portTICK_PERIOD_MS);
    }
}


homekit_value_t colours_fade_get ( ) {
    return (HOMEKIT_BOOL(colours_fade.value.bool_value));
}


void smooth_colours_task(){
    
    float smooth_value = 1000;
    int r = COLOUR_MAX;
    int b = 0;
    int g = 0;
    int w = 0;
 
    colours_smooth.value.bool_value = true;
    homekit_characteristic_notify(&colours_smooth,colours_smooth.value );
    
    printf ("%s: \n", __func__);
    
    while (1) {
        for (/* no initialization */; r>=0 && b<COLOUR_MAX; b=b+smooth_value, r=r-smooth_value) /*red -> blue*/
        {
            set_colours (r, 0, b, 0);
            vTaskDelay (TEN_MS/portTICK_PERIOD_MS);
        }
        
        for (/* no initialization */; b>=0 && g<COLOUR_MAX; g=g+smooth_value, b=b-smooth_value) /*blue -> green*/
        {
            set_colours (0, g, b, 0);
            vTaskDelay (TEN_MS/portTICK_PERIOD_MS);
        }
        
        for (/* no initialization */; g>=0 && w<COLOUR_MAX; w=w+smooth_value, g=g-smooth_value) /*green -> white*/
        {
            set_colours (0, g, 0, w);
            vTaskDelay (TEN_MS/portTICK_PERIOD_MS);
        }
        
        for (/* no initialization */; w>=0 && r<COLOUR_MAX; r=r+smooth_value, w=w-smooth_value) /*white -> red*/
        {
            set_colours (r, 0, 0, w);
            vTaskDelay (TEN_MS/portTICK_PERIOD_MS);
        }
    }
}


homekit_value_t colours_smooth_get ( ) {
    return (HOMEKIT_BOOL(colours_smooth.value.bool_value));
}


void colour_effect_reset (){
    
    if (previous_colour_effect == strobe_effect) {
        colours_strobe.value.bool_value = false;
        homekit_characteristic_notify(&colours_strobe,colours_strobe.value );
    }
    
    if (previous_colour_effect == flash_effect){
        colours_flash.value.bool_value = false;
        homekit_characteristic_notify(&colours_flash,colours_flash.value );
    }
    
    
    if (previous_colour_effect == fade_effect){
        colours_fade.value.bool_value = false;
        homekit_characteristic_notify(&colours_fade,colours_fade.value );
    }
    
    if (previous_colour_effect == smooth_effect ){
        colours_smooth.value.bool_value = false;
        homekit_characteristic_notify(&colours_smooth,colours_smooth.value );
    }
    
    if (previous_colour_effect == cycle_effect ){
        colours_gpio_test.value.bool_value = false;
        homekit_characteristic_notify(&colours_gpio_test,colours_gpio_test.value );
    }
    
    if (colours_effect_handle != NULL){
        /* if an effect is already running then stop it as we are either here
         to stop the existing one or start a new one*/
        vTaskDelete (colours_effect_handle);
        colours_effect_handle = NULL;
    }
}


void colour_effect_start_stop (int effect_set) {
    
    colour_effect_reset(); /* switch off any running effect and toggle buttons to off */

    if (effect_set == previous_colour_effect){
        /* toggle switch, so second press = off */
        
        effect_set = off_effect;
    }
   
    if (effect_set!=off_effect) /*start the effect running */
    {
        sdk_os_timer_disarm (&rgbw_set_timer );
        switch (effect_set) {
            case cycle_effect:
                xTaskCreate(cycle_colours_task, "cycle_colours_task", 512 , NULL, tskIDLE_PRIORITY+1, &colours_effect_handle);
                printf ("%s: Effect: Cycle\n",__func__);
                break;
            case strobe_effect:
                xTaskCreate(strobe_colours_task, "srtobe_colours_task", 512 , NULL, tskIDLE_PRIORITY+1, &colours_effect_handle);
                printf ("%s: Effect: STROBE\n",__func__);
                break;
            case flash_effect:
                xTaskCreate(flash_colours_task, "flash_colours_task", 512 , NULL, tskIDLE_PRIORITY+1, &colours_effect_handle);
                printf ("%s: Effect: Flash\n",__func__);
                break;
            case fade_effect:
                xTaskCreate(fade_colours_task, "fade_colours_task", 512 , NULL, tskIDLE_PRIORITY+1, &colours_effect_handle);
                printf ("%s: Effect: Fade\n",__func__);
                break;
            case smooth_effect:
                xTaskCreate(smooth_colours_task, "smooth_colours_task", 512 , NULL, tskIDLE_PRIORITY+1, &colours_effect_handle);
                printf ("%s: Effect: Smoth\n",__func__);
                break;
            default:
                printf ("%s: Unknown effect: %d\n",__func__, effect_set);
        }
    }
    else
    {
        printf ("%s: Effect off setting back to original\n",__func__);
        sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );
    }
    
    previous_colour_effect = effect_set;
}


void colours_gpio_test_set (homekit_value_t value) {
    
    printf("%s:\n", __func__);
    colour_effect_start_stop (cycle_effect);
    colours_gpio_test.value.bool_value = value.bool_value;
}


void colours_strobe_set (homekit_value_t value) {
    
    printf("%s:\n", __func__);
    colour_effect_start_stop (strobe_effect);
    colours_gpio_test.value.bool_value = value.bool_value;
}


void colours_flash_set (homekit_value_t value) {
    
    printf("%s:\n", __func__);
    colour_effect_start_stop (flash_effect);
    colours_gpio_test.value.bool_value = value.bool_value;
}


void colours_fade_set (homekit_value_t value) {
    
    printf("%s:\n", __func__);
    colour_effect_start_stop (fade_effect);
    colours_gpio_test.value.bool_value = value.bool_value;
}


void colours_smooth_set (homekit_value_t value) {
    
    printf("%s:\n", __func__);
    colour_effect_start_stop (smooth_effect);
    colours_gpio_test.value.bool_value = value.bool_value;
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
        sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );
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
        sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );
    }
}

void on_update (homekit_characteristic_t *ch, homekit_value_t value, void *context    ){
    sdk_os_timer_arm (&gpio_timer,5000 , 0 );
}

void rgbw_set(){

    /* make sure there are no effects running */
    colour_effect_reset();
    previous_colour_effect = off_effect;
    
    printf("\n%s\n", __func__);
    printf("%s: Current colour before set r=%d,g=%d, b=%d, w=%d,\n",__func__, current_color.red,current_color.green, current_color.blue, current_color.white );
    if (led_on==true) {
        // convert HSI to RGBW
        
        HSVtoRGB(led_hue, led_saturation, led_brightness, &target_color);
        printf("%s: h=%d,s=%d,b=%d => r=%d,g=%d, b=%d\n",__func__, (int)led_hue,(int)led_saturation,(int)led_brightness, target_color.red,target_color.green, target_color.blue );
        
        RBGtoRBGW (&target_color);
        printf("%s: h=%d,s=%d,b=%d => r=%d,g=%d, b=%d, w=%d,\n",__func__, (int)led_hue,(int)led_saturation,(int)led_brightness, target_color.red,target_color.green, target_color.blue, target_color.white );
        printf ("%s: GPIOS are set as follows : W=%d, R=%d, G=%d, B=%d\n",__func__, white_gpio.value.int_value,red_gpio.value.int_value, green_gpio.value.int_value, blue_gpio.value.int_value );
        current_color.red = target_color.red * PWM_SCALE;
        current_color.green = target_color.green * PWM_SCALE;
        current_color.blue = target_color.blue * PWM_SCALE;
        current_color.white = target_color.white * PWM_SCALE;
        
        set_colours (current_color.red, current_color.green, current_color.blue, current_color.white);

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
    
    on.value.bool_value = value.bool_value;
    led_on = value.bool_value;
    if (led_on == false )
    {
        printf ("%s: Led on false so stopping Multi PWM\n", __func__);
        multipwm_stop(&pwm_info);
        sdk_os_timer_disarm (&rgbw_set_timer);
    } else
    {
        printf ("%s: Led on TRUE so setting colour\n", __func__);
        sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );
    }
    sdk_os_timer_arm (&save_timer, SAVE_DELAY, 0 );
}

homekit_value_t led_brightness_get() {
    return HOMEKIT_INT(led_brightness);
}

void led_brightness_set(homekit_value_t value) {
    if (value.format != homekit_format_int) {
        printf("%s: Invalid brightness-value format: %d\n", __func__, value.format);
        return;
    }
    brightness.value.int_value = value.int_value;
    led_brightness = value.int_value;
    printf ("%s: timer armed, Brightness: %f\n", __func__, led_brightness);
    sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );
    sdk_os_timer_arm (&save_timer, SAVE_DELAY, 0 );

}

homekit_value_t led_hue_get() {
    return HOMEKIT_FLOAT(led_hue);
}

void led_hue_set(homekit_value_t value) {
    if (value.format != homekit_format_float) {
        printf("%s: Invalid hue-value format: %d\n", __func__, value.format);
        return;
    }
    hue.value.int_value = value.int_value;
    led_hue = value.float_value;
    printf ("%s: timer armed, HUE: %f\n", __func__, led_hue);
    sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );
    sdk_os_timer_arm (&save_timer, SAVE_DELAY, 0 );
}

homekit_value_t led_saturation_get() {
    return HOMEKIT_FLOAT(led_saturation);
}

void led_saturation_set(homekit_value_t value) {
    if (value.format != homekit_format_float) {
        printf("%s: Invalid sat-value format: %d\n", __func__, value.format);
        return;
    }
    saturation.value.int_value = value.int_value;
    led_saturation = value.float_value;
    printf ("%s: timer armed, Saturation: %f\n", __func__, led_saturation);
    sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );
    sdk_os_timer_arm (&save_timer, SAVE_DELAY, 0 );

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
    sdk_os_timer_setfn(&save_timer, save_characteristics, NULL);
    
    sdk_os_timer_arm (&rgbw_set_timer, RGBW_SET_DELAY, 0 );

    printf ("%s: sdk_os_timer_Setfn called\n", __func__);
    
}
