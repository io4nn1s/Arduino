#include "FT3267.h"


// interrupt flag for touch detection
  volatile bool newTouch = false;

  void touch_ISR(){
    newTouch = true;
  }

bool FT3267 :: touched()
{
   #ifdef POLLING  // see FT3267.h for details
      volatile uint8_t reading = 1;
      reading &= digitalRead(ctp_int);
      delayMicroseconds(10);
      reading &= digitalRead(ctp_int);
      delayMicroseconds(10);
      reading &= digitalRead(ctp_int);
      delayMicroseconds(10);
      if(reading>0)
         return false;         //if INT is HIGH, then no touch
      else
         return true;

         
   #else //use ISR
      if (newTouch == true){
         newTouch = false;
         return true;
      } else {
         return false;
      }
   #endif
}

void I2C_start(uint8_t reg_address)
{
   Wire.beginTransmission(FT_I2C_address); //send I2C address
   Wire.write(reg_address);                //select register address to read/write
   Wire.endTransmission(FT_I2C_address);   //end transmission
}

FT3267 :: FT3267(const uint8_t INT_pin, const uint8_t RESET_pin)
{
   ctp_int = INT_pin;
   ctp_reset = RESET_pin;
}

void FT3267 :: init(void)
{
   // setup pins on the MCU
   pinMode(ctp_reset, OUTPUT);
   pinMode(ctp_int, INPUT_PULLUP);

   #ifndef POLLING   //if POLLING is disabled (using MCU interrupt)
      #ifdef digitalPinToInterrupt
         attachInterrupt(digitalPinToInterrupt(ctp_int),touch_ISR,FALLING);
      #else
         attachInterrupt(ctp_int, touch_ISR, FALLING);
      #endif
   #endif

   //reset cap touch IC
   digitalWrite(ctp_reset, HIGH);
   delay(1);
   digitalWrite(ctp_reset, LOW);
   delay(30);
   digitalWrite(ctp_reset, HIGH);
   
   Wire.begin();   //Start I2C
   
   #ifdef CTP_DEBUG
      Serial.begin(9600);
      Serial.println("FT3267 initialized");
   #endif
}


void FT3267 :: getRegisterInfo(unsigned char *buff, uint8_t start_addr, uint8_t nbytes) 
{
    I2C_start(start_addr);
   
    Wire.requestFrom(FT_I2C_address, nbytes); 
    int register_number = 0;
    
    while(Wire.available())  {     // get all register bytes when available
       buff[register_number++] = Wire.read();
    }
}

void FT3267 :: getTouches(int X[2], int Y[2])
{
   unsigned char Register[13];    //for registers 00h - 0Ch
   
   getRegisterInfo(Register, 0, 13);
                                                                    // Touch 1 coordinates
   X[0] = (Register[FT_TOUCH1_XH]&0x0F)<<8 | Register[FT_TOUCH1_XL];     // get X1
   Y[0] = (Register[FT_TOUCH1_YH]&0x0F)<<8 | Register[FT_TOUCH1_YL];     // get Y1
   if(Register[FT_TP_STATUS]>1)  {                                  // if more than 1 touch
      X[1] = (Register[FT_TOUCH2_XH]&0x0F)<<8 | Register[FT_TOUCH2_XL];  // get X2
      Y[1] = (Register[FT_TOUCH2_YH]&0x0F)<<8 | Register[FT_TOUCH2_YL];  // get Y2
   }  
   else  {
      X[1] = 0;
      Y[1] = 0;
   }
}

void FT3267 :: printInfo(void)   // Experimental function
{
    byte buff[4];

    I2C_start(0xA0);
    
    Wire.requestFrom(FT_I2C_address, 3); 
    int num = 0;

    while(Wire.available())
    {
      buff[num++] = Wire.read();
      //Serial.print("reg[");Serial.print(num);Serial.print("] = 0x");Serial.println(buff[num],HEX);
      //num++;
    }
    Serial.println("************************");
    Serial.println("FocalTech CTP IC");
    Serial.print("Library version: ");  //Note: high/low bit may not be same as major/minor version
    Serial.print(buff[FT_TOUCH_LIB_VERSION_H&0x0F]);
    Serial.print(".");
    Serial.print(buff[FT_TOUCH_LIB_VERSION_L&0x0F]);
    Serial.println("\n************************");
//*/
}

  
