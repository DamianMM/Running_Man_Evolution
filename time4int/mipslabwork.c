/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#define TIMER2PERIOD 31250; //(((80*10^3)*100)/256) = 80*10^3 cycles per milisecond, times 100 for 100 miliseconds and divided by scale of 256.

int mytime = 0x5957;
int prime = 1234567;

volatile int* portout = (void *)0xBF886110; //PORTE, output LED.

char textstring[] = "text, more text, and even more text!";

int timeoutcount = 0; //Used to count timeouts.



/* Interrupt Service Routine */
void user_isr( void )
{
  timeoutcount++;

 volatile int button = getbtns();
  switch (button){
    case 1:
      mytime = (mytime & ~(0x00F0)) | getsw()<<4;
      break;
    case 2:
      mytime = (mytime & ~(0x0F00)) | getsw()<<8;
      break;
    case 3:
      mytime = (mytime & ~(0x0FF0)) | getsw()<<4 | getsw() << 8;
      break;
    case 4:
      mytime = (mytime & ~(0xF000)) | getsw()<<12;
      break;
    case 5:
      mytime = (mytime & ~(0xF0F0)) | getsw()<<4 | getsw() <<12;
      break;
    case 6:
      mytime = (mytime & ~(0xFF00)) | getsw()<<8 | getsw() << 12;
      break;
    case 7:
      mytime = (mytime & ~(0xFFF0)) | getsw()<<4 | getsw() <<8 | getsw()<<12;
    break;
  } 

  if(timeoutcount == 10){
    time2string( textstring, mytime );
    display_string( 3, textstring );
    display_update();
    tick( &mytime );
    *portout += 1;
    timeoutcount = 0;
  }

  IFSCLR(0) = 0x100; //Clear timer status flag.
}

/* Lab-specific initialization goes here */
void labinit( void ){ 
  volatile int* portout = (void *)0xBF886110; //PORTE, output LED.
  volatile int * trisout = (void *)0xBF886100; //TRISE, LEDs.
  *trisout = *trisout & ~0xFF; // Masking one byte from trise for output.
  TRISD |= 0xFE0; //Masking to 1 on 7:5 for input.

  T2CON = 0x0;      //Stop timer and clear control register.
  T2CONSET = 0x70;  // Set 1:256 prescale.
  TMR2 = 0x0;       // Clear timer register.
  PR2 = TIMER2PERIOD; // Set timer period.
  IFSCLR(0) = 0x100; //Clear timer status flag.

  IPCSET(2) = 0x10; //Priority set to 4.
  IECSET(0) = 0x100; //Enable timer interrupts.

  enable_interrupts(); //From assembly.
  T2CONSET = 0x8000; //Start timer, <15> to 1.
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  prime = nextprime(prime);
  display_string(0, itoaconv(prime));
  display_update();
}

