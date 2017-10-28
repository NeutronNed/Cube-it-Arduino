#include <Adafruit_NeoPixel.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

#define NUMLEDS  A0
#define CUBESW0  16
#define CUBESW1  15
#define CUBESW2  07
#define CUBESW3  11
#define CUBESW4  30
#define CUBESW5  27

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, A0, NEO_GRB + NEO_KHZ800);

long colorValue[5] = {0x000000, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF}; //Black, Red, Green, Blue, White


struct SWITCH {  // a structure to hold the switches
  byte cubeSwitch0;
  byte cubeSwitch1;
  byte cubeSwitch2;
  byte cubeSwitch3;
  byte cubeSwitch4;
  byte cubeSwitch5;
};

struct SWITCH swNow = {0, 0, 0, 0, 0, 0};  // define a structure to hold the current switch values
struct SWITCH swLast = {0, 0, 0, 0, 0, 0};  // define a structure to hold the previous switch values

char inByte = 0;         // incoming serial byte

void setup() {
  Serial.begin(9600);

  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  strip.begin();
  strip.setBrightness(32);
  strip.show(); // Initialize all pixels to 'off'

  pinMode(CUBESW0, INPUT);   // input from the Side 1 switch
  pinMode(CUBESW1, INPUT);   // input from the Side 2 switch
  pinMode(CUBESW2, INPUT);   // input from the Side 3 switch
  pinMode(CUBESW3, INPUT);   // input from the Side 4 switch
  pinMode(CUBESW4, INPUT);   // input from the Side 5 switch
  pinMode(CUBESW5, INPUT);   // input from the Side 6 switch

  bno.setExtCrystalUse(true);
}

// Main execution loop ********************************************************************************* LOOP

void loop() {

// Code start ***************************************************************************************** START
   sensors_event_t event; 
   bno.getEvent(&event);
   Serial.print("X");
   Serial.print(event.orientation.x, 0);
   Serial.print(", Y");
   Serial.print(event.orientation.y, 0);
   Serial.print(", Z");
   Serial.print(event.orientation.z, 0);
   Serial.println("");

// Process button switches - always send switch change events ****************************** PROCESS SWITCHES
  swNow = debounceSwitch();     // read the input switches:
  
  if (swNow.cubeSwitch0 != swLast.cubeSwitch0){
    Serial.print("S0");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch0);
  }
  if (swNow.cubeSwitch1 != swLast.cubeSwitch1){
    Serial.print("S1");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch1);
  }
  if (swNow.cubeSwitch2 != swLast.cubeSwitch2){
    Serial.print("S2");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch2);
  }
  if (swNow.cubeSwitch3 != swLast.cubeSwitch3){
    Serial.print("S3");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch3);
  }
  if (swNow.cubeSwitch4 != swLast.cubeSwitch4){
    Serial.print("S4");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch4);
  }
  if (swNow.cubeSwitch5 != swLast.cubeSwitch5){
    Serial.print("S5");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch5);
  } 
  swLast = swNow;

// Process commands from the serial port ********************************************* PROCESS SERIAL COMMANDS
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    switch (inByte) {
      case 'S':  // send Switch values:
         Serial.print(swNow.cubeSwitch0);
         Serial.print(",");
         Serial.print(swNow.cubeSwitch1);
         Serial.print(",");
         Serial.print(swNow.cubeSwitch2);
         Serial.print(",");
         Serial.print(swNow.cubeSwitch3);
         Serial.print(",");
         Serial.print(swNow.cubeSwitch4);
         Serial.print(",");
         Serial.println(swNow.cubeSwitch5);
         break;
         
      case 'F':  // set the Face color as specified
         int face = Serial.parseInt();
         int color = Serial.parseInt();
         face = 5 - constrain(face, 0, 5);
         color = constrain(color, 0, 4);
         colorFace(face, colorValue[color]);  //set the specified face to that color
         break;

    }
  }
  
}
// END of Main Loop ***************************************************************************************

/*******************************************************************************************/
/* debounceSwitch: reads all switch inputs 32 times then divides by 25 to return a "1" if
/*   if input is high 25 or greater times.
/*******************************************************************************************/
struct SWITCH debounceSwitch () {
  struct SWITCH sw = {0,0,0,0,0,0};
  
  for (int i = 0; i < 64; i++){
    sw.cubeSwitch0 = sw.cubeSwitch0 + digitalRead(CUBESW0);
    sw.cubeSwitch1 = sw.cubeSwitch1 + digitalRead(CUBESW1);
    sw.cubeSwitch2 = sw.cubeSwitch2 + digitalRead(CUBESW2);
    sw.cubeSwitch3 = sw.cubeSwitch3 + digitalRead(CUBESW3);
    sw.cubeSwitch4 = sw.cubeSwitch4 + digitalRead(CUBESW4);
    sw.cubeSwitch5 = sw.cubeSwitch5 + digitalRead(CUBESW5);

  }

  sw.cubeSwitch0 = 1 - (sw.cubeSwitch0 / 50);  
  sw.cubeSwitch1 = 1 - (sw.cubeSwitch1 / 50);
  sw.cubeSwitch2 = 1 - (sw.cubeSwitch2 / 50);
  sw.cubeSwitch3 = 1 - (sw.cubeSwitch3 / 50);
  sw.cubeSwitch4 = 1 - (sw.cubeSwitch4 / 50);
  sw.cubeSwitch5 = 1 - (sw.cubeSwitch5 / 50);

  return (sw);
}

// Fill the dots one after the other with a color
void colorFace(int face1, long c) {
  for(int i=face1*4; i<face1*4+4; i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

//Check the switches and if there are changes, send out the information
void switchCheck(){
  if (swNow.cubeSwitch0 != swLast.cubeSwitch0){
    Serial.print("S0");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch0);
  }
  if (swNow.cubeSwitch1 != swLast.cubeSwitch1){
    Serial.print("S1");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch1);
  }
  if (swNow.cubeSwitch2 != swLast.cubeSwitch2){
    Serial.print("S2");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch2);
  }
  if (swNow.cubeSwitch3 != swLast.cubeSwitch3){
    Serial.print("S3");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch3);
  }
  if (swNow.cubeSwitch4 != swLast.cubeSwitch4){
    Serial.print("S4");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch4);
  }
  if (swNow.cubeSwitch5 != swLast.cubeSwitch5){
    Serial.print("S5");
    Serial.print(",");
    Serial.println(swNow.cubeSwitch5);
  }

}

