#include "FastLED.h"
#include <EEPROM.h>

#define NUM_LEDS 50 
CRGB leds[NUM_LEDS];
#define PIN 6 

#define BUTTON 2
byte selectedEffect = 0;
byte interrupted = 0;

void setup()
{
  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  
  pinMode(BUTTON, INPUT_PULLUP);// internal pull-up resistor  
  attachInterrupt (digitalPinToInterrupt (BUTTON), interruptEffect, LOW); // pressed
}


void loop() { 
  EEPROM.get(0, selectedEffect); 
  
  if(selectedEffect > 4) { 
    selectedEffect = 0;
    EEPROM.put(0, 0); 
  } 
  
  switch(selectedEffect) { 
              
    case 1 : {
               // theaterChaseRainbow - Speed delay
               theaterChaseRainbow(100);
               break;
             }

    case 2 : { 
               // TwinkleRandom - twinkle count, speed delay, only one (true/false)
               TwinkleRandom(20, 100, false);
               break;
             }

    case 3 : {
               // meteorRainRandom - meteor size, trail decay, random trail decay (true/false), speed delay 
               meteorRainRandom(10, 64, true, 30);
               break;
             }
             
    case 4 : {
               // meteorRain - Color (red, green, blue), meteor size, trail decay, random trail decay (true/false), speed delay 
               meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
               break;
             }
             
//    case 5:  {
//               RunningLightsRainbow(50);
//               break;
//             }
             
    default: {
               // rainbowCycle - speed delay
               rainbowCycle(20);
               break;
             }
  }
}

void changeEffect() {
    selectedEffect++;
    EEPROM.put(0, selectedEffect);
    setAll(0, 0, 0);
}

void interruptEffect() {
  if (digitalRead (BUTTON) == HIGH) {
    interrupted = 1;
  }
}


// *************************
// ** LEDEffect Functions **
// *************************


void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
     
     if(!showStrip()) {
       return;
     }
     
     delay(SpeedDelay);
     
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }
  
  delay(SpeedDelay);
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    
    if(!showStrip()) {
      return;
    }
    
    delay(SpeedDelay);
  }
}

// used by rainbowCycle and theaterChaseRainbow
byte * Wheel(byte WheelPos) {
  static byte c[3];
  
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void theaterChaseRainbow(int SpeedDelay) {
  byte *c;
  
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < NUM_LEDS; i=i+3) {
          c = Wheel( (i+j) % 255);
          setPixel(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
        }
        
        if(!showStrip()) {
          return;
        }
       
        delay(SpeedDelay);
       
        for (int i=0; i < NUM_LEDS; i=i+3) {
          setPixel(i+q, 0,0,0);        //turn every third pixel off
        }
    }
  }
}

//void RunningLightsRainbow(int WaveDelay) {
//  byte *c;
//  for (int j=0; j < 256; j++) {   
//    c = Wheel(j);
//    RunningLights(*c, *(c+1), *(c+2), WaveDelay);
//  }
//}
//
//void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
//  int Position=0;
//  
//  for(int i=0; i<NUM_LEDS*2; i++)
//  {
//      Position++;
//      for(int i=0; i<NUM_LEDS; i++) {
//        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
//                   ((sin(i+Position) * 127 + 128)/255)*green,
//                   ((sin(i+Position) * 127 + 128)/255)*blue);
//      }
//      
//      if(!showStrip()) {
//        return;
//      }
//    
//      delay(WaveDelay);
//  }
//}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
  
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {   
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      } 
    }
   
    if(!showStrip()) {
      return;
    }
    
    delay(SpeedDelay);
  }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue) {
  leds[ledNo].fadeToBlackBy( fadeValue );
}

void meteorRainRandom(byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  meteorRain(random(0,255),random(0,255),random(0,255), meteorSize, meteorTrailDecay, meteorRandomDecay, SpeedDelay);
}


// ***********************
// **  Common Functions **
// ***********************

// Apply LED color changes
byte showStrip() {
  FastLED.show();

  if(interrupted) {
    interrupted = 0;
    changeEffect();
    return 0;
  }
  
  return 1;
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
