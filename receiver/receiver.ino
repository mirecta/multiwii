

#define SCK_PIN   13
#define MISO_PIN  12
#define MOSI_PIN  11
#define SS_PIN    10



//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.


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

int error = 0; 



int defaultValue = 1500;

volatile byte buffer[2][14];
volatile byte telemetry[14];
volatile char set = 0;
char writeSet = 1;
int index = 0;
int nosignal = 0;

byte SPI_transfer(byte data = 0x00) {
  SPDR = data;
  while (!(SPSR & (1<<SPIF))) ;
  return SPDR; 
}

//interrupt
ISR (SPI_STC_vect){
 
  byte data = SPDR;
  char _set = set;
  SPCR &= ~_BV(SPIE);
  if (data == 0xc0){
    
    for(int i = 0 ; i < 14; ++i){
    telemetry[i] =  SPI_transfer(buffer[_set][i]); 
    }
  }
  SPCR |= _BV(SPIE);
}
 


void SPI_slaveInit(void){
  
  pinMode(SCK_PIN, INPUT);
  pinMode(MOSI_PIN, INPUT);
  pinMode(MISO_PIN, OUTPUT);
  pinMode(SS_PIN, INPUT);
 
  
  
  
  SPCR = _BV(SPE);
  //enable interrupt
  SPCR |= _BV(SPIE);
}


void setup(){

 byte lo = defaultValue & 0x7f;
 byte hi = defaultValue >> 7;  
 for (int i = 0; i < 2; ++i){
   buffer[i][0] = 0xc0;
   buffer[i][1] = lo;
   buffer[i][2] = hi;
   buffer[i][3] = lo;
   buffer[i][4] = hi;
   buffer[i][5] = lo;
   buffer[i][6] = hi;
   buffer[i][7] = lo;
   buffer[i][8] = hi;
   buffer[i][9] = lo;
   buffer[i][10] = hi;
   buffer[i][11] = lo;
   buffer[i][12] = hi;
   buffer[i][13] = 0x80;
 }

 Serial.begin(9600);

 
 SPI_slaveInit();
}


int stableAnalog(int pin){
 
  int sum = 0;
 for (byte i=0; i < 10; ++i){
   
   sum += analogRead(pin);
 }
 
 return sum/10;
 
}


void loop(){
 
   
//read serial dokola jak blbec a prepinaj set 
  
   
    if(Serial.available()){
      
      byte c = Serial.read();
    
       if (index == 0 && c != 0xc0)
            return;
       
       buffer[writeSet][index++] = c;
       if (index == 14){
         index = 0;
         if (buffer[writeSet][13] != 0x80) 
            return;
        nosignal = 0;
        if(writeSet == 1){
           set = 1;
           writeSet = 0;
        }
        else{
           set = 0;
           writeSet = 1;
        }
       }

    }else{
    
    delay(30);
    nosignal ++;
    if(nosignal > 70){
      nosignal = 70;
      buffer[set][13] = 0x81;
    }else{
      buffer[set][13] = 0x80;
    }
    }
    
    
    
  
 

     
}
