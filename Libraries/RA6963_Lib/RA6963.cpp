#include "RA6963.h"

#include <stdio.h>
//#include <string.h>
//#include <inttypes.h>

// No RD,FS pins (hardwired)
RA6963 :: RA6963(const UB8 wr, const UB8 cd, const UB8 enable, const UB8 reset,
                 const UB8 d0, const UB8 d1, const UB8 d2, const UB8 d3,
                 const UB8 d4, const UB8 d5, const UB8 d6, const UB8 d7)
{
   //Wire pin RD (Data Read) to GND and pin FS (Font Select) to GND or VDD.
   setPins(wr, 0, enable, cd, 0, reset, d0, d1, d2, d3, d4, d5, d6, d7);
}
// All pins used as I/O
RA6963 :: RA6963(const UB8 wr, const UB8 rd, const UB8 enable, 
                 const UB8 cd, const UB8 fs, const UB8 reset,
                 const UB8 d0, const UB8 d1, const UB8 d2, const UB8 d3,
                 const UB8 d4, const UB8 d5, const UB8 d6, const UB8 d7)
{
   setPins(wr, rd, enable, cd, fs, reset, d0, d1, d2, d3, d4, d5, d6, d7);
}
void RA6963 :: setPins(const UB8 wr, const UB8 rd, const UB8 enable, 
                       const UB8 cd, const UB8 fs, const UB8 reset,
                       const UB8 d0, const UB8 d1, const UB8 d2, const UB8 d3,
                       const UB8 d4, const UB8 d5, const UB8 d6, const UB8 d7)
{
  wr_pin = wr;
  rd_pin = rd;
  ce_pin = enable;
  cd_pin = cd;
  fs_pin = fs;
  rst_pin = reset;

  data_pins[0] = d0;
  data_pins[1] = d1;
  data_pins[2] = d2;
  data_pins[3] = d3;
  data_pins[4] = d4;
  data_pins[5] = d5;
  data_pins[6] = d6;
  data_pins[7] = d7;

  pinMode(wr_pin, OUTPUT);
  if(rd_pin!=0) {
     pinMode(rd_pin, OUTPUT);
     digitalWrite(rd_pin, HIGH);   //disable read (default)
  }
  pinMode(ce_pin, OUTPUT);
  pinMode(cd_pin, OUTPUT);
  if(fs_pin!=0) {
     pinMode(fs_pin, OUTPUT);
     digitalWrite(fs_pin, LOW);   //select font table 1 (default)
  }
  pinMode(rst_pin, OUTPUT);

  for (uint8_t i = 0; i < 8; i++)
  {
    pinMode(data_pins[i], OUTPUT);
  }
}

void RA6963 :: writeEnable(void)
{
  digitalWrite(wr_pin, LOW);   //write to RA6963
  delayMicroseconds(2);
  digitalWrite(wr_pin, HIGH);  //disable write
  delayMicroseconds(1);
  digitalWrite(ce_pin, HIGH);  //disable chip
  delayMicroseconds(1);
}



void RA6963 :: Write_command(uint8_t value)
{
  digitalWrite(cd_pin, HIGH);   //select command
  delayMicroseconds(2);
  digitalWrite(ce_pin, LOW);   //chip enable
  delayMicroseconds(1);

  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(data_pins[i], (value >> i) & 0x01);
  }
  delayMicroseconds(1);

  writeEnable();
}

void RA6963 :: Write_data(uint8_t value)
{
  digitalWrite(cd_pin, LOW);   //select data
  delayMicroseconds(2);
  digitalWrite(ce_pin, LOW);   //chip enable
  delayMicroseconds(1);

  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(data_pins[i], (value >> i) & 0x01);
  }
  delayMicroseconds(1);

  writeEnable();
}

void RA6963 :: RESET(void)
{
  digitalWrite(rst_pin, HIGH);
  delay(1);
  digitalWrite(rst_pin, LOW);
  delay(100);
  digitalWrite(rst_pin, HIGH);
  delay(10);
  digitalWrite(ce_pin, HIGH);
}

void RA6963 :: init(void)
{
  uint8_t LOW_BYTE1;

  RESET();

  /*write text home address=0000h */
  Write_data(0x00);
  Write_data(0x00);
  Write_command(0x40);

  Write_data(LOW_BYTE);
  Write_data(HI_BYTE);
  Write_command(0x42);

  /*write text area address*/
  LOW_BYTE1 = MaxColumn / 8;
  Write_data(LOW_BYTE1);
  Write_data(0x00);
  Write_command(0x41);

  /*write graphic area address*/
  LOW_BYTE1 = MaxColumn / 8;
  Write_data(LOW_BYTE1);
  Write_data(0x00);
  Write_command(0x43);

  /*set display mode Display mode set (Graphic only enable)*/
  Write_command(0x80);
  /*Graphic display enable*/
  Write_command(0x98);

}

//################################################################


//==================================
// 8*8 Character pattern test (Text)
//==================================
void RA6963 ::  Character_pattern()
{
  UB8 x, y, k = 0;
  Write_command(0x94);        //Text display only
  Write_data(0x00);
  Write_data(0x00);
  Write_command(0x24);
  Write_command(0xB0);
  for (y = 0; y < MaxRow / 8; y++)
  {
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(k++);
      if (k == 0x80)
        k = 0x00;
    }
  }
  Write_command(0xB2); //set auto write reset
}
//==================================
// show picture
//==================================
void RA6963 :: Show_picture(const UB8 *img_data)
{
  UB8 x, y;
  Write_command(0x98);
  home_address();
  Write_command(0xB0);

  for (y = 0; y < MaxRow; y++)
  {
    for (x = 0; x < MaxColumn / 8; x++)
    {
      //NOTE: For Atmel AVR MCU's use the following macro:
      //    pgm_read_byte_near(data_in_PROGMEM);
      // in order to access data in the Program Memory.
      // Here we access an image array via a pointer.
      // For more info visit:
      // https://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html
      
      Write_data(pgm_read_byte_near(img_data));
      img_data++;
    }
  }
  Write_command(0xB2); //set auto write reset
}

//============================================
void RA6963 :: address1(UB8 y, UB8 x)
{
  UW16 ADDRESS;
  UB8  LOW_B, HI_B;
  Write_command(0xB2); //set auto write reset
  ADDRESS = graphic_home_address+(y * MaxLine)+x;
  LOW_B = ADDRESS % 0x0100;
  HI_B  = ADDRESS / 0x0100;
  Write_data(LOW_B);
  Write_data(HI_B);
  Write_command(0x24);
  Write_command(0xB0);
}
//==================================
// full on
//==================================
void RA6963 :: full_off()
{
  UB8 x, y;
  Write_command(0x98);
  home_address();
  Write_command(0xB0);
  for (y = 0; y < MaxRow; y++)
  {
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(0x00);
    }
  }
  Write_command(0xB2); //set auto write reset
}

//==================================
//        VERTICAL LINE-1
//==================================
void RA6963 :: Vertical_line1()
{
  UB8 x1, x, y, dd = 0x80;
  full_off();
  Write_command(0x98);
  home_address();
  Write_command(0xB0);
  for (x1 = 0; x1 < MaxColumn / 8; x1++)
  { dd = 0x80;
    for (x = 0; x < 8; x++)
    {
      for (y = 0; y < MaxRow; y++)
      {
        address1(y, x1);
        Write_data(dd);
      }
      dd = dd >> 1;
      delay(2);
    }

    for (y = 0; y < MaxRow; y++)
    {
      address1(y, x1);
      Write_data(0x00);

    }
  }
  Write_command(0xB2); //set auto write reset

}

//==================================
//        VERTICAL LINE
//==================================
void RA6963 :: Vertical_line(void)
{
  UB8 x, y;
  Write_command(0x98);
  home_address();
  Write_command(0xB0);
  for (y = 0; y < MaxRow; y++)
  {
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(0xaa);
    }
  }
  Write_command(0xB2); //set auto write reset

}
//========================================
void RA6963 :: address(UB8 y)
{
  UW16 ADDRESS;
  UB8  LOW_B, HI_B;
  Write_command(0xB2); //set auto write reset
  ADDRESS = graphic_home_address+(y * MaxLine);
  LOW_B = ADDRESS % 0x0100;
  HI_B  = ADDRESS / 0x0100;
  Write_data(LOW_B);
  Write_data(HI_B);
  Write_command(0x24);
  Write_command(0xB0);
}

//==================================
//   Horizontal line moving down
//==================================
void RA6963 :: Horizontal_line1()
{
  UB8 x, y;
  full_off();
  Write_command(0x98);
  home_address();
  Write_command(0xB0);
  for (y = 0; y < MaxRow; y++)
  {
    for (x = 0; x < MaxColumn/8; x++)
    {
      Write_data(0xff); //Two lines (column/8/2 = column/4)
    }
    delay(100);
    address(y);
    for (x = 0; x < MaxColumn/8; x++)
    {
      Write_data(0x00);
    }
    delay(1);
  }
  Write_command(0xB2); //set auto write reset
}

//==================================
//      CROSS DOT
//==================================
void RA6963 :: Cross_Dot()
{
  UB8 x, y;
  Write_command(0x98);
  home_address();
  Write_command(0xB0);
  for (y = 0; y < MaxRow / 2; y++)
  {
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(0xaa);
    }
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(0x55);
    }
  }
  Write_command(0xB2); //set auto write reset

}
//==================================
//   HORIZONTAL LINES
//==================================
void RA6963 :: Horizontal_line(void)
{
  UB8 x, y;
  Write_command(0x98);
  home_address();
  Write_command(0xB0);
  for (y = 0; y < MaxRow / 2; y++)
  {
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(0xff);
    }
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(0x00);
    }
  }
  Write_command(0xB2); //set auto write reset
}

//========================================
// full on
//========================================
void RA6963 :: full_on()
{
  UB8 x, y;
  Write_command(0x98);
  home_address();
  Write_command(0xB0);
  for (y = 0; y < MaxRow; y++)
  {
    for (x = 0; x < MaxColumn / 8; x++)
    {
      Write_data(0xff);
    }
  }
  Write_command(0xB2); //set auto write reset
}
//========================================
void RA6963 :: home_address()
{
  Write_data(LOW_BYTE);
  Write_data(HI_BYTE);
  Write_command(0x24);
}

//========================================
// Custom Function
//========================================
/* Create your own function like below:
void RA6963 :: func()
{ }
 Remember to declare it in the RA6963.h file.
*/
