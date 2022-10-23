//===============================================================
//    Display    : 128*64 LCD (works up to 240*128 resolution)
//    Controller : RA6963  (font 8*8)
//    Author     : Ioannis
//    Date       : 06/07/2021
//================================================================
#ifndef RA6963_h
#define RA6963_h

#include <Arduino.h>

//=================================
//  type define
//=================================
typedef unsigned char         BOOL;          /* Boolean */
typedef unsigned char         UB8;           /* 8-bit data */
typedef unsigned short int    UW16;          /*16-bit data */
//typedef unsigned long         UW32;          /*32-bit data */
//typedef signed char           SB8;           /* 8-bit data */
//typedef signed short int      SW16;          /*16-bit data */
//typedef signed long           SW32;          /*32-bit data */

//Macros
#define   MaxColumn    128            //  Max RAM = 240*128
#define   MaxRow       64             //
#define   MaxLine    (MaxColumn/8)    // num of bytes per line
#define   graphic_home_address  ((MaxColumn /8) *(MaxRow/8))
#define   LOW_BYTE   (graphic_home_address % 0x0100)
#define   HI_BYTE    (graphic_home_address / 0x0100)


class RA6963
{
   private :
      uint8_t wr_pin, rd_pin, ce_pin, cd_pin, rst_pin, fs_pin;
      uint8_t data_pins[8];
      
   public :
      RA6963(const UB8 wr, const UB8 cd, const UB8 enable, const UB8 reset,
             const UB8 d0, const UB8 d1, const UB8 d2, const UB8 d3,
             const UB8 d4, const UB8 d5, const UB8 d6, const UB8 d7);

      RA6963(const UB8 wr, const UB8 rd, const UB8 enable, 
             const UB8 cd, const UB8 fs, const UB8 reset,
             const UB8 d0, const UB8 d1, const UB8 d2, const UB8 d3,
             const UB8 d4, const UB8 d5, const UB8 d6, const UB8 d7);

      void setPins(const UB8 wr, const UB8 rd, const UB8 enable, 
                   const UB8 cd, const UB8 fs, const UB8 reset,
                   const UB8 d0, const UB8 d1, const UB8 d2, const UB8 d3,
                   const UB8 d4, const UB8 d5, const UB8 d6, const UB8 d7);

      void writeEnable(void);
      void Write_command(uint8_t value);
      void Write_data(uint8_t value);
      void RESET(void);
      void init(void);

      void Show_picture(const UB8 *img_data);
      void Vertical_line(void);
      void Horizontal_line(void);
      void Cross_Dot();
      void home_address();
      void full_on();
      void full_off();
      void home_address(UW16);
      void Character_pattern();
      void stp_sc();
      void Vertical_line1();
      void Horizontal_line1();
      void address1(UB8 y,UB8 x);
      void address(UB8 y);

      ~RA6963()   //destructor (optional)
      {}
};

#endif
