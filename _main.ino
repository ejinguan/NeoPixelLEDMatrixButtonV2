#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "Button.h"

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int val = 50;


Button green_btn(9), red_btn(8);
int state = 0;


uint16_t serial_r, serial_c;



// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  
  pinMode(green_btn.pin, INPUT_PULLUP);
  pinMode(red_btn.pin, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  
  strip.setBrightness(val);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);

/*
  for (int j=0; j<8; j++) 
    for (int i=0; i<8; i++) {
      XY_COLOUR(Wheel(200), i, j);
      strip.show();
      delay(1000);
    }
    */
}




void loop() {
  // Read potentiometer
  updateBrightness();
  
  // Update buttons
  green_btn.Update();
  red_btn.Update();

  // Increment if green or red button is pressed or held
  if (green_btn.isJustPressed || green_btn.isHeld) state++;
  if (  red_btn.isJustPressed ||   red_btn.isHeld) state--;
  // Allow only states 0~14
  if (state == 16) state =  0;               // Wrap around at 15
  if (state == -1) state = 15;               // Wrap around at -1

/*
  Serial.print("Green: ");
  Serial.print(green_btn.isPressed);
  Serial.print(", Red: ");
  Serial.print(red_btn.isPressed);
  Serial.print(", state: ");
  Serial.println(state);
*/

  // Show button press state on built in LED
  digitalWrite(13, (green_btn.isPressed || red_btn.isPressed)?HIGH:LOW);

  uint32_t c1, c2, c3, c4;
  
  switch (state) {
    case 15:
      // Serial access
      if (green_btn.isJustReleased || red_btn.isJustReleased) {
        // Reset the grid only if the buttons are just pressed
        strip.clear();
        strip.show();
      } else {
        
        // Read serial and update grid
        while (Serial.available() > 0) {
          
          int incomingByte = 0; // for incoming serial data
          incomingByte = Serial.read();

          // Read r char or c char
          if (incomingByte=='r') {
            // Wait for Serial to be available
            while(!Serial.available()){}
            
            incomingByte = Serial.read();
            if (incomingByte > 10) serial_r = (incomingByte - '0') & 0xFF; // offset from char 0
            else serial_r = (byte) incomingByte;
            //Serial.print("r = ");
            //Serial.println(serial_r);
          }
          if (incomingByte=='c') {
            // Wait for Serial to be available
            while(!Serial.available()){}
            
            incomingByte = Serial.read();
            if (incomingByte > 10) serial_c = (incomingByte - '0') & 0xFF; // offset from char 0
            else serial_c = (byte) incomingByte;
            //Serial.print("c = ");
            //Serial.println(serial_c);
          }

          if (incomingByte=='#') {
            uint32_t colour = 0;
            int tmp = 0;

            // Hold the loop and read 6 bytes
            for (int i = 0; i < 6; i++) {
              if (colour > 0) colour *= 16; // multiply by 16 each time
              
              // Wait for Serial to be available
              while (!Serial.available()) { }
              // Read one char
              incomingByte = Serial.read();
              if (incomingByte >= '0' && incomingByte <= '9') tmp = (incomingByte - '0') & 0xFF;
              else if (incomingByte >= 'A' && incomingByte <= 'F') tmp = 10 + (incomingByte - 'A') & 0xFF; // 'A' is 10
              else if (incomingByte >= 'a' && incomingByte <= 'f') tmp = 10 + (incomingByte - 'a') & 0xFF; // 'a' is 10

              colour = colour + tmp;
            }
            //Serial.print("col = ");
            //Serial.println(colour, HEX);
            
            XY_COLOUR(colour, serial_c, serial_r);
            strip.show();
          }
        }
      }
      break;
    case 14:
      c1 = Wheel((millis()/5   ) & 255);
      c2 = Wheel((millis()/5+10) & 255);
      c3 = Wheel((millis()/5+20) & 255);
      c4 = Wheel((millis()/5+30) & 255);
      boxFrame(c1, c2, c3, c4);
      // rainbowCycleFrame(1);
      break;
    case 13:
      rainbowCycleFrame(-millis()/5);
      break;
    case 12:
      rainbowCycleFrame(millis()/5);
      break;
    default:
      c1 = Wheel((255 * state/12     ) & 255);
      c2 = Wheel((255 * state/12 + 21) & 255);
      c3 = Wheel((255 * state/12 + 42) & 255);
      c4 = Wheel((255 * state/12 + 63) & 255);
      boxFrame(c1, c2, c3, c4);
      /*
      Serial.print("c1: "); Serial.print(c1);
      Serial.print(", c2: "); Serial.print(c2);
      Serial.print(", c3: "); Serial.print(c3);
      Serial.print(", c4: "); Serial.println(c4);
      */
      strip.show();
  }

  delayRead(1);
}

// Fill the dots one after the other with a color
void colorFill(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}




void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delayRead(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delayRead(wait);
  }
}

// Make rainbow equally distributed throughout but take time variable as parameter input
void rainbowCycleFrame(uint8_t frame) {
  // "frame" acts as a timer aspect (j in other subroutine)
  uint16_t i;
  
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + frame) & 255));
  }
  strip.show();
}

// Set "concentric" frames with 4 colours
void boxFrame(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
  int i; int j;
  uint32_t c[4]; int z;

  c[0] = c1; c[1] = c2; c[2] = c3; c[3] = c4;
  
  // Loop from 0-3, 0-3
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      z = (i<=j ? i : j);
      XY_COLOUR(c[z],   i,   j);
      XY_COLOUR(c[z], 7-i,   j);
//      Serial.print("7-i:");  Serial.print(7-i); Serial.print(", j:");  Serial.println(j);
      XY_COLOUR(c[z],   i, 7-j);
      XY_COLOUR(c[z], 7-i, 7-j);
    }
  }

  strip.show();
}



/*
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delayRead(wait);
  }
}
void colorWipeRainbow(uint8_t wait) {
  for (int j=0; j < 256; j=j+256/12) {     // 12 colours
    colorWipe(Wheel(j & 255), wait);
  }
}


//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  theaterChaseMultiple(c, wait, 10);
}
void theaterChaseMultiple(uint32_t c, uint8_t wait, uint8_t repeat) {
  for (int j=0; j<repeat; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delayRead(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
void theaterChaseRainbowDiscrete(uint8_t wait) {
  for (int j=0; j < 256; j=j+256/6) {     // 6 colours
    theaterChase(Wheel(j % 255), wait);
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delayRead(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
*/

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}






// Custom delay code
// Adjust the brightness while waiting for the delay to be up
void delayRead(unsigned long period) {
  unsigned long time_now;
  
  time_now = millis();
   
  while(millis() < time_now + period){
    updateBrightness();
  }
}


void updateBrightness() {
  val = analogRead(A2);
  val = map(val, 0, 1023, 1, MAX_BRIGHTNESS); // Convert to output range
  
  strip.setBrightness(val);
}

