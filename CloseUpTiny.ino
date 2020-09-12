#include <Adafruit_NeoPixel.h>

#ifdef __AVR__

#include <avr/power.h> // Required for 16 MHz Adafruit Trinket

#endif
#include <Servo.h>


Servo servo;

int photo1 = A4;
int photo2 = A3;
int photo3 = A2;
int photo4 = A1;
int photo5 = A0;
int button = A6;

int LED = 7;

//initially turn all red if light on photosensors
//check repeatedly for next sensor to go low after a certain time threshold
//if several turn low all at once, or if threshold isn't met, retrun to all red
//When all green, open, and then turn off all lights until photosensors are exposed again, then turn all red.
Adafruit_NeoPixel pixels(6, LED, NEO_GRB + NEO_KHZ800);

int  lights[] = {0, 0, 0, 0, 0, 0};
int values[] = {0, 0, 0, 0, 0};
int last = 0;
unsigned long lastTime;
int maxval = 100;
void setup() {
 
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)

  clock_prescale_set(clock_div_1);

  #endif
  
  pixels.begin();
  pixels.show();
  pinMode(photo1, INPUT);
  pinMode(photo2, INPUT);
  pinMode(photo3, INPUT);
  pinMode(photo4, INPUT);
  pinMode(photo5, INPUT);
  pinMode(button, INPUT);
  
  pinMode(LED, OUTPUT);
  lastTime = millis();
  //maxval = (analogRead(photo1) + analogRead(photo2) + analogRead(photo3) + analogRead(photo4) + analogRead(photo5))/5;
  
}

void loop() {
 
  values[0] = analogRead(photo1);
  values[1] = analogRead(photo2);
  values[2] = analogRead(photo3);
  values[3] = analogRead(photo4); 
  values[4] = analogRead(photo5);

  if((values[0]+values[1] + values[2] + values[3] + values [4])<(maxval/2*5) && lights[0]==0){
    
  }
  else{
    if ((values[0]+values[1] + values[2] + values[3] + values [4])>(maxval/2*5) && lights[0]==0){
      //set all LEDS to red
      for (int i = 0; i<6; i++){
        lights[i] = 1;
        
        delay(500);
      }
      turnOnLights();
    }
    processPhoto();
    if (analogRead(button)>600){
      openBook();
    }
  
    int avg = 0;
    for(int i = 0; i<5; i++){
  
      avg = avg + values[i];
    }
    avg = avg/5;
    if(avg>maxval){
      maxval = avg;
    }
    
  
  
    delay(500);
  }
}

void openBook(){
  
  if(lights[5]!=2){ 
    
    pulse();
    
  }
  else{
    servo.attach(5);
    servo.write(90);
    delay(10000);
   
    servo.write(0);
    delay(550);
    servo.detach();
    last = 0;
  
     for (int i = 0; i<6; i++){
        lights[i] = 0;
        
      }
    
    turnOnLights();
  }
}
void pulse(){
      pixels.setBrightness(50);
      turnOnLights();
      
      delay(250);
      pixels.setBrightness(255);
      turnOnLights();
      delay(250);
      pixels.setBrightness(50);
      turnOnLights();
      
      delay(250);
      pixels.setBrightness(255);
      turnOnLights();
}
void processPhoto(){
  unsigned long currentTime = millis();
  boolean r = false;
  if(values[last]<(maxval/2) && last<5){
    if((currentTime - lastTime)>2000){
      
      lights[last] = 2;
      last = last+1;
      lastTime = currentTime;
    }
    else{
      r = true;
      
    }
  }
  int i = last;
  
  while (i<5){
    if(values[i]<(maxval/2)){
      r = true;
    } 
    i = i+1;
  }
  if (r){
    resetVals();
    turnOnLights();
  }
  else{
    if(lights[4] == 2){
        turnOnLights();
        delay(500);
        lights[5] = 2;
        turnOnLights();
        
    }
    else{
        turnOnLights(); 
    }
  }
  
}
void resetVals(){

    last = 0;
    for (int i = 0; i<5; i++){
      lights[i] = 1;
      
    }
    lights[5] = 1;
    pulse();
}
void turnOnLights(){
//if 1 ->red, 2->green, 0 ->off
  boolean clr = false;
  for(int i = 0; i<6; i++){
    if(lights[i] == 1){
      pixels.setPixelColor(i, 255, 0, 0);    
    }
    else{
      if(lights[i]==2){
        pixels.setPixelColor(i, 0, 255, 0);
      }
      else{
        clr = true;
      }
    }
  }
  if(clr){
    pixels.clear();
    pixels.show();
  }
  else{
    pixels.show();
  }

}
