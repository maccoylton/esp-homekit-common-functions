
#define LED_RGB_SCALE 255       // this is the scaling factor used for color conversion
#define max(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })
#define min(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a < _b ? _a : _b; })


typedef union {
    struct {
        uint16_t white;
        uint16_t blue;
        uint16_t green;
        uint16_t red;
    };
    uint64_t color;
} rgb_color_t;

// Color smoothing variables

typedef union {
    struct {
        float hue;
        float saturation;
        int brightness;
    };
    uint64_t hsi_color;
} hsi_color_t;


void hsi2rgb(float h, float s, float i, rgb_color_t* rgbw);

void hsi2rgbw(float h, float s, float i, rgb_color_t* rgbw);

void  HSVtoRGB(float h, float s,  float  v, rgb_color_t* rgbw) ;

void RBGtoRBGW ( rgb_color_t* rgbw, bool pure_white);


