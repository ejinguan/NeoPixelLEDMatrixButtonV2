#define PIN 10
#define N_LEDS 64

#define MAX_BRIGHTNESS 255

#define XY_TO_LED(x, y) (8*(y) + (((y) & 1) ? (7-(x)): (x)))
#define XY_COLOUR(c, x, y) strip.setPixelColor(XY_TO_LED(x, y), c)

