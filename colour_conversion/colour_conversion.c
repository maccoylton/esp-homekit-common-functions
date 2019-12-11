#include <espressif/esp_common.h>
#include <colour_conversion.h>
#include <math.h>

#define DEG_TO_RAD(X) (M_PI*(X)/180)
//http://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white
void hsi2rgbw(float h, float s, float i, rgb_color_t* rgbw) {

    float cos_h, cos_1047_h;
    int r, g, b, w;
    
    while (h < 0) { h += 360.0F; };     // cycle h around to 0-360 degrees
    while (h >= 360) { h -= 360.0F; };
    h = 3.14159F*h / 180.0F;            // convert to radians.
    s /= 100.0F;                        // from percentage to ratio
    i /= 100.0F;                        // from percentage to ratio
    s = s > 0 ? (s < 1 ? s : 1) : 0;    // clamp s and i to interval [0,1]
    i = i > 0 ? (i < 1 ? i : 1) : 0;    // clamp s and i to interval [0,1]
    //i = i * sqrt(i);                    // shape intensity to have finer granularity near 0
    
    if (h < 2.09439) {
        cos_h = cos(h);
        cos_1047_h = cos(1.047196667 - h);
        r = s * LED_RGB_SCALE * i / 3 * (1 + s * cos_h / cos_1047_h);
        g = s * LED_RGB_SCALE * i / 3 * (1 + s * (1 - cos_h / cos_1047_h));
        b = 0;
    }
    else if (h < 4.188787) {
        h = h - 2.09439;
        cos_h = cos(h);
        cos_1047_h = cos(1.047196667 - h);
        g = s * LED_RGB_SCALE * i  / 3 * (1 + s * cos_h / cos_1047_h);
        b = s * LED_RGB_SCALE * i  / 3 * (1 + s * (1 - cos_h / cos_1047_h));
        r = 0;
    }
    else {
        h = h - 4.188787;
        cos_h = cos(h);
        cos_1047_h = cos(1.047196667 - h);
        b = s * LED_RGB_SCALE * i  / 3 * (1 + s * cos_h / cos_1047_h);
        r = s * LED_RGB_SCALE * i  / 3 * (1 + s * (1 - cos_h / cos_1047_h));
        g = s * 0;
    }
    
    w = LED_RGB_SCALE * i * (1 -s);
    
    rgbw->red = (uint8_t) r;
    rgbw->green = (uint8_t) g;
    rgbw->blue = (uint8_t) b;
    rgbw->white= (uint8_t) w;
}


void hsi2rgb(float h, float s, float i, rgb_color_t* rgbw) {
    float cos_h, cos_1047_h;
    int r, g, b, w;
    
    while (h < 0) { h += 360.0F; };     // cycle h around to 0-360 degrees
    while (h >= 360) { h -= 360.0F; };
    h = 3.14159F*h / 180.0F;            // convert to radians.
    s /= 100.0F;                        // from percentage to ratio
    i /= 100.0F;                        // from percentage to ratio
    s = s > 0 ? (s < 1 ? s : 1) : 0;    // clamp s and i to interval [0,1]
    i = i > 0 ? (i < 1 ? i : 1) : 0;    // clamp s and i to interval [0,1]
    //i = i * sqrt(i);                    // shape intensity to have finer granularity near 0
    
    if (h < 2.09439) {
        cos_h = cos(h);
        cos_1047_h = cos(1.047196667 - h);
        r = LED_RGB_SCALE * i / 3 * (1 + s * cos_h/cos_1047_h);
        g = LED_RGB_SCALE * i / 3 * (1 + s * (1 - cos_h/cos_1047_h));
        b = LED_RGB_SCALE * i / 3 * (1 - s);
    }
    else if (h < 4.188787) {
        h = h - 2.09439;
        cos_h = cos(h);
        cos_1047_h = cos(1.047196667 - h);
        g = LED_RGB_SCALE * i / 3 * (1 + s * cos_h / cos_1047_h);
        b = LED_RGB_SCALE * i / 3 * (1 + s * (1 - cos_h / cos_1047_h));
        r = LED_RGB_SCALE * i / 3 * (1 - s);
    }
    else {
        h = h - 4.188787;
        cos_h = cos(h);
        cos_1047_h = cos(1.047196667 - h);
        b = LED_RGB_SCALE * i / 3 * (1 + s * cos_h / cos_1047_h);
        r = LED_RGB_SCALE * i / 3 * (1 + s * (1 - cos_h / cos_1047_h));
        g = LED_RGB_SCALE * i / 3 * (1 - s);
    }
    
    w = LED_RGB_SCALE * i * (1 -s);
    
    rgbw->red = (uint8_t) r;
    rgbw->green = (uint8_t) g;
    rgbw->blue = (uint8_t) b;
    rgbw->white= (uint8_t) w;
}


void RBGtoRBGW ( rgb_color_t* rgbw){
    //Get the maximum between R, G, and B
    float colour_max = max (rgbw->red, max(rgbw->green, rgbw->blue));
    
    //If the maximum value is 0, immediately return pure black.
    if(colour_max == 0)
    {
        rgbw->white = 0;
    } else {
        
        //This section serves to figure out what the color with 100% hue is
        float multiplier = 255.0f / colour_max;
        float hR = rgbw->red * multiplier;
        float hG = rgbw->green * multiplier;
        float hB = rgbw->blue * multiplier;
        
        //This calculates the Whiteness (not strictly speaking Luminance) of the color
        float M = max(hR, max(hG, hB));
        float m = min(hR, min(hG, hB));
        float Luminance = ((M + m) / 2.0f - 127.5f) * (255.0f/127.5f) / multiplier;
        
        //Calculate the output values
        int Wo = (int)(Luminance);
/*
        int Bo = (int)(rgbw->blue - Luminance);
        int Ro = (int)(rgbw->red - Luminance);
        int Go = (int)(rgbw->green - Luminance);
 */
        int Bo = (int)(rgbw->blue);
        int Ro = (int)(rgbw->red);
        int Go = (int)(rgbw->green);
        
        
        //Trim them so that they are all between 0 and 255
        if (Wo < 0) Wo = 0;
        if (Bo < 0) Bo = 0;
        if (Ro < 0) Ro = 0;
        if (Go < 0) Go = 0;
        if (Wo > 255) Wo = 255;
        if (Bo > 255) Bo = 255;
        if (Ro > 255) Ro = 255;
        if (Go > 255) Go = 255;
        
        rgbw->red = Ro;
        rgbw->green = Go;
        rgbw->blue = Bo;
        rgbw->white = Wo;
    }
}



void  HSVtoRGB(float hue, float saturation,  float brightness, rgb_color_t* rgbw) {
    float f, p, q, t, h, s, v, r=0, g=0, b=0;
    int  i;
    
    h=hue/360;
    s=saturation/100;
    v=brightness/100;
    /*hue comes in 0-360 degrees, 0-100 for saturation and brightness, alogorithm expects 0-1*/
    
    i = floor(h * 6);
    printf ("i: %d  ", i);
    f = h * 6 - i;
    printf ("f: %f  ", f);
    p = v * (1 - s);
    printf ("p: %f  ", p);
    q = v * (1 - f * s);
    printf ("q: %f  ", q);
    t = v * (1 - (1 - f) * s);
    printf ("t: %f\n", t);
    switch (i % 6) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
            r = v;
            g = p;
            b = q;
            break;
    }
    rgbw->red = round(r * 255);
    rgbw->green = round(g * 255);
    rgbw->blue = round(b * 255);
}
