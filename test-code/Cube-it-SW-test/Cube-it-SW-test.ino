#include <Adafruit_NeoPixel.h>

#define NUMLEDS  A0
#define CUBESW1  16
#define CUBESW2  15
#define CUBESW3  07
#define CUBESW4  11
#define CUBESW5  30
#define CUBESW6  27

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, A0, NEO_GRB + NEO_KHZ800);

int colorMode;

struct SWITCH {  // a structure to hold the switches
  byte cubeSwitch1;
  byte cubeSwitch2;
  byte cubeSwitch3;
  byte cubeSwitch4;
  byte cubeSwitch5;
  byte cubeSwitch6;
};

struct SWITCH swNow = {0, 0, 0, 0, 0, 0};  // define a structure to hold the current switch values
struct SWITCH swLast = {0, 0, 0, 0, 0, 0};  // define a structure to hold the previous switch values

void setup() {
  Serial.begin(9600);
    
  strip.begin();
  strip.setBrightness(32);
  strip.show(); // Initialize all pixels to 'off'

  pinMode(CUBESW1, INPUT);   // input from the Side 1 switch
  pinMode(CUBESW2, INPUT);   // input from the Side 2 switch
  pinMode(CUBESW3, INPUT);   // input from the Side 3 switch
  pinMode(CUBESW4, INPUT);   // input from the Side 4 switch
  pinMode(CUBESW5, INPUT);   // input from the Side 5 switch
  pinMode(CUBESW6, INPUT);   // input from the Side 6 switch
  
}

// Main execution loop ***************************************************************************************

void loop() {

// Code start ************************************************************************************************
  swNow = debounceSwitch();     // read the input switches:

  if (swNow.cubeSwitch1 == 0){
     colorWipe(0xFFFFFF,5);  //color , face
  } else {
     colorWipe(0,5);         //color , face
  }
  
  if (swNow.cubeSwitch2 == 0){
     colorWipe(0xFFFFFF,4);  //color , face
  } else {
     colorWipe(0,4);         //color , face
  }

  if (swNow.cubeSwitch3 == 0){
     colorWipe(0xFFFFFF,3);  //color , face
  } else {
     colorWipe(0,3);         //color , face
  }
  
  if (swNow.cubeSwitch4 == 0){
     colorWipe(0xFFFFFF,2);  //color , face
  } else {
     colorWipe(0,2);         //color , face
  }
 
  if (swNow.cubeSwitch5 == 0){
     colorWipe(0xFFFFFF,1);  //color , face
  } else {
     colorWipe(0,1);         //color , face
  }

  if (swNow.cubeSwitch6 == 0){
     colorWipe(0xFFFFFF,0);  //color , face
  } else {
     colorWipe(0,0);         //color , face
  }
 
}
// END of Main Loop ***************************************************************************************

/*******************************************************************************************/
/* debounceSwitch: reads all switch inputs 32 times then divides by 25 to return a "1" if
/*   if input is high 25 or greater times.
/*******************************************************************************************/
struct SWITCH debounceSwitch () {
  struct SWITCH sw = {0,0,0,0,0,0};
  
  for (int i = 0; i < 32; i++){
    sw.cubeSwitch1 = sw.cubeSwitch1 + digitalRead(CUBESW1);
    sw.cubeSwitch2 = sw.cubeSwitch2 + digitalRead(CUBESW2);
    sw.cubeSwitch3 = sw.cubeSwitch3 + digitalRead(CUBESW3);
    sw.cubeSwitch4 = sw.cubeSwitch4 + digitalRead(CUBESW4);
    sw.cubeSwitch5 = sw.cubeSwitch5 + digitalRead(CUBESW5);
    sw.cubeSwitch6 = sw.cubeSwitch6 + digitalRead(CUBESW6);
  }

  sw.cubeSwitch1 = sw.cubeSwitch1 / 25;
  sw.cubeSwitch2 = sw.cubeSwitch2 / 25;
  sw.cubeSwitch3 = sw.cubeSwitch3 / 25;
  sw.cubeSwitch4 = sw.cubeSwitch4 / 25;
  sw.cubeSwitch5 = sw.cubeSwitch5 / 25;
  sw.cubeSwitch6 = sw.cubeSwitch6 / 25;  
  return (sw);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t face) {
  for(uint16_t i=face*4; i<face*4+4; i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

