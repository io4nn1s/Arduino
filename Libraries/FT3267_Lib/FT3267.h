/**************************************************************************************
 *                                 FT3267 Library
 * IC:              FT3267
 * MCU:             uC32, UNO32, Arduino, etc.
 * Description:     This is a basic library for Arduino compatible microcontrollers
 *                  for driving the FT3267 capacitive touch controller. 
 *
 * Copyright (C) 2022  Midas Displays
 * 
 * Author(s):       I. Antonakis 
 * Date:            12/11/2021
 * Release:         v 1.0
 *                  NOTE: Current version relies on hardware I2C pins and Wire.h lib.
 *************************************************************************************/
#ifndef FT3267_H
#define FT3267_H

#include <Arduino.h>
#include <Wire.h>    //i2c library

//Uncomment below for Serial Debugging
//#define CTP_DEBUG

//Uncomment below to use poll (read directly) the INT pin from the CTP.
//Otherwise you need to use a pin on the MCU that supports interrupts,
//because this program will use an interrupt routine by default.
//#define POLLING  //does not work well!

#define FT_I2C_address  0x38

//FocalTech IC Register Addresses (counting from 0x00)
#define FT_DEVICE_MODE  0x00
#define FT_GEST_ID      0x01
#define FT_TP_STATUS    0x02
#define FT_TOUCH1_XH    0x03
#define FT_TOUCH1_XL    0x04
#define FT_TOUCH1_YH    0x05
#define FT_TOUCH1_YL    0x06
//registers 07h and 08h are reserved
#define FT_TOUCH2_XH    0x09
#define FT_TOUCH2_XL    0x0A
#define FT_TOUCH2_YH    0x0B
#define FT_TOUCH2_YL    0x0C

#define FT_NUMBER_OF_TOTAL_REGISTERS 0xFE
#define FT5xx6_NUMBER_OF_TOTAL_REGISTERS 0xFE
#define FT_TOUCH_LIB_VERSION_H 0xA1
#define FT_TOUCH_LIB_VERSION_L 0xA2

class FT3267
{
   private:
      uint8_t ctp_int, ctp_reset;

   public:
      FT3267(const uint8_t INT_pin, const uint8_t RESET_pin);

      void init(void);
      bool touched();
      void getRegisterInfo(unsigned char *buff, uint8_t start_addr, uint8_t nbytes);
      void getTouches(int X[2], int Y[2]);
      void printInfo(void);

};




#endif
