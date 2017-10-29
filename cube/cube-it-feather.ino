#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

#define CUBESW0  13
#define CUBESW1  12
#define CUBESW2  11
#define CUBESW3  10
#define CUBESW4  9
#define CUBESW5  6

Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

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

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup(void)
{ 
  delay(500);
  Serial.begin(115200);

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
  
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin() )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  ble.echo(false);
  ble.verbose(false);

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }
  
  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

void loop(void)
{
  // Check for user input
  char n, inputs[BUFSIZE+1];

      sensors_event_t event; 
       bno.getEvent(&event);

      //send XYZ coordinate to Pi
       ble.print("X,");
       ble.print(event.orientation.x, 0);
       ble.print(", Y,");
       ble.print(event.orientation.y, 0);
       ble.print(", Z,");
       ble.print(event.orientation.z, 0);
       ble.print(",");
   
    swNow = debounceSwitch();     // read the input switches:
  
    if (swNow.cubeSwitch0 != swLast.cubeSwitch0){
      ble.print("S,0");
      ble.print(",");
      ble.print(swNow.cubeSwitch0);
      ble.print(",");
    }
    if (swNow.cubeSwitch1 != swLast.cubeSwitch1){
      ble.print("S,1");
      ble.print(",");
      ble.print(swNow.cubeSwitch1);
      ble.print(",");
    }
    if (swNow.cubeSwitch2 != swLast.cubeSwitch2){
      ble.print("S,2");
      ble.print(",");
      ble.print(swNow.cubeSwitch2);
      ble.print(",");
    }
    if (swNow.cubeSwitch3 != swLast.cubeSwitch3){
      ble.print("S,3");
      ble.print(",");
      ble.print(swNow.cubeSwitch3);
      ble.print(",");
    }
    if (swNow.cubeSwitch4 != swLast.cubeSwitch4){
      ble.print("S,4");
      ble.print(",");
      ble.print(swNow.cubeSwitch4);
      ble.print(",");
    }
    if (swNow.cubeSwitch5 != swLast.cubeSwitch5){
      ble.print("S,5");
      ble.print(",");
      ble.print(swNow.cubeSwitch5);
      ble.print(",");
    } 
    swLast = swNow;
 

  while ( ble.available() )
  { 
    inByte = ble.read();

    Serial.println(inByte);
    
    switch (inByte) {
      case 'S':  // send Switch values:
         ble.print(swNow.cubeSwitch0);
         ble.print(",");
         ble.print(swNow.cubeSwitch1);
         ble.print(",");
         ble.print(swNow.cubeSwitch2);
         ble.print(",");
         ble.print(swNow.cubeSwitch3);
         ble.print(",");
         ble.print(swNow.cubeSwitch4);
         ble.print(",");
         ble.println(swNow.cubeSwitch5);
         break;
         
      case 'F':  // set the Face color as specified
         int face = ble.parseInt();
         int color = ble.parseInt();
         Serial.print(face);
         Serial.println(color);
         face = 5 - constrain(face, 0, 5);
         color = constrain(color, 0, 4);
         colorFace(face, colorValue[color]);  //set the specified face to that color
         break;
    }
  }
}

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
  for (int i = 0; i < 24; i++){
    strip.setPixelColor(i, 0);
  }
  
  for(int i=face1*4; i<face1*4+4; i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

//Check the switches and if there are changes, send out the information
void switchCheck(){
  if (swNow.cubeSwitch0 != swLast.cubeSwitch0){
    ble.print("S0");
    ble.print(",");
    ble.println(swNow.cubeSwitch0);
  }
  if (swNow.cubeSwitch1 != swLast.cubeSwitch1){
    ble.print("S1");
    ble.print(",");
    ble.println(swNow.cubeSwitch1);
  }
  if (swNow.cubeSwitch2 != swLast.cubeSwitch2){
    ble.print("S2");
    ble.print(",");
    ble.println(swNow.cubeSwitch2);
  }
  if (swNow.cubeSwitch3 != swLast.cubeSwitch3){
    ble.print("S3");
    ble.print(",");
    ble.println(swNow.cubeSwitch3);
  }
  if (swNow.cubeSwitch4 != swLast.cubeSwitch4){
    ble.print("S4");
    ble.print(",");
    ble.println(swNow.cubeSwitch4);
  }
  if (swNow.cubeSwitch5 != swLast.cubeSwitch5){
    ble.print("S5");
    ble.print(",");
    ble.println(swNow.cubeSwitch5);
  }

}
