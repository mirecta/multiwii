/*
  Analog input, analog out        put, serial output
 
 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 
 */
 // D2 - D6
 
 
 //throtle 115 925
 //yaw 155 850
 //roll 195 825
 //pitch 175 845
 
 //upstream
//14 bytes 12 x 8 bit - 12 =  84 bit usable -> 6 analog channels(11bit) + 18 digital channels 
//11000000 0aaaaaaa 0xxxaaaa 0bbbbbbb 0xxxbbbb 0ccccccc 0xxxcccc 0ddddddd 0xxxdddd 0eeeeeee 0xxxeeee 0fffffff 0xxxffff 10000000


//downstream
//position  2x BCD
//     180         59        12.56
//d dddd dddd   mmm mmmm    sss ssss . ssss ssss  = 31 bits x 2 = 61 bits
//rest is 22 bits for battery level altitude and ground speed

//11110000 

// 1500 = 1011 / 0101 1100 = 0b/5c 
  // 1011101 1100

//static uint8_t rcChannel[RC_CHANS] = {PITCH,YAW,THROTTLE,ROLL,AUX1,AUX2,AUX3};
 
int smoothRead(int pin){
  
  int value = 0;
  for(int i = 0; i < 16; ++i){
      value += analogRead(pin); 
  }
  return value >> 4;
}

inline int correct(int value){
  if (value < 1000)
    return 1000;
  if (value > 2000)
    return 2000;
  return value;
}


int analogPin = 0;
int pitch = 0;
int yaw = 0;
int roll = 0;
int throttle = 0;
int aux = 0;

int b1 = 0;
int b2 = 0;
int b3 = 0;
int b4 = 0;
int b5 = 0;

uint8_t buffer[14];
int defaultValue = 1500; 
byte lo = defaultValue & 0x7f;
byte hi = defaultValue >> 7;



void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);

  
  
}

void loop() {
  
  // Smoothing with window size 10
  
  //int value = map(analogSmooth,112,560,1000,2000);
  
  pitch = correct(map(smoothRead(0),175,845,1000,2000));
  roll = correct(map(smoothRead(1),195,825,1000,2000));
  throttle = correct(map(smoothRead(2),115,925,1000,2000));
  yaw = correct(map(smoothRead(3),155,850,1000,2000));
  aux = correct(map(smoothRead(4),0,1024,1000,2000));
  
  b1 = digitalRead(2);
  b2 = digitalRead(5);
  b3 = digitalRead(4);
  b4 = digitalRead(3);
  b5 = digitalRead(6);
  
  //build packet
  buffer[0] = 0xc0;
  
  buffer[1] = pitch & 0x7f;
  buffer[2] = pitch >> 7;
  //add three buttons
  buffer[2] |= (b1 << 6) | (b2 << 5) | (b3 << 4);
  
  buffer[3] = yaw & 0x7f;
  buffer[4] = yaw >> 7;
  //add three buttons
  buffer[4] |= (b4 << 6) | (b3 << 5);
  
  buffer[5] = throttle & 0x7f;
  buffer[6] = throttle >> 7;
  buffer[7] = roll & 0x7f;
  buffer[8] = roll >> 7;
  buffer[9] = aux & 0x7f;
  buffer[10] = aux >> 7;
  
  buffer[11] = lo;
  buffer[12] = hi;
  
  buffer[13] = 0x80;
  
  Serial.write(buffer,14);
  
 /*
  Serial.print(" P:");
  Serial.print(pitch);
    
  Serial.print(" R:");
  Serial.print(roll);
  
  Serial.print(" T:");
  Serial.print(throttle);
  
  Serial.print(" Y:");
  Serial.print(yaw);
  
  Serial.print(" A:");
  Serial.print(aux);
  
  
  Serial.print(" 1:");
  Serial.print(b1); 
  
  Serial.print(" 2:");
  Serial.print(b2);
  
  Serial.print(" 3:");
  Serial.print(b3);
  
  Serial.print(" 4:");
  Serial.print(b4);
  
  Serial.print(" 5:");
  Serial.print(b5);
   
  Serial.println("");
*/


 

  delay(50);
}
 

