/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdio.h>
#include <stdlib.h>
#include "mipslab.h"  /* Declatations for these labs */

#define TIMER2PERIOD 31250; //(((80*10^3)*100)/256) = 80*10^3 cycles per milisecond, times 100 for 100 miliseconds and divided by scale of 256.
#define JUMPHEIGHT 18;

int mytime = 0x5957;
int prime = 1234567;
int generate_counter = 0;
int jumping = 0;
char game_over[] = "GAME OVER";
int delay_time = 100;

volatile int* portout = (void *)0xBF886110; //PORTE, output LED.

//char textstring[] = "text, more text, and even more text!";

//int timeoutcount = 0; //Used to count timeouts.



/* Interrupt Service Routine */
void user_isr( void )
{
  /*
 volatile int button = getbtns();
  switch (button){
    case 1:
      runner_jump();
      break;*/
    /*case 2:
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
    break; */
  //} 

 /* if(timeoutcount == 10){
    time2string( textstring, mytime );
    display_string( 3, textstring );
    display_update();
    tick( &mytime );
    *portout += 1;
    timeoutcount = 0;
  } */

  //IFSCLR(0) = 0x100; //Clear timer status flag. */
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

void generate_obstacle(){
  ground[3][127] = 0;
  ground[3][95] = 0;
  ground[3][63] = 255;
  ground[3][31] = 255;
}

void scroll_page_once(int page){
      if(page < 3){
        ground[page][31] = ground[page+1][0];
        ground[page][63] = ground[page+1][32];
        ground[page][95] = ground[page+1][64];
        ground[page][127] = ground[page+1][96];
        generate_counter++;
      }  

      int column;
      for(column = 0; column<31; column++){
        ground[page][96+column] = ground[page][96+column+1];
        ground[page][64+column] = ground[page][64+column+1];
        ground[page][32+column] = ground[page][32+column+1];
        ground[page][0+column] = ground[page][0+column+1];
      }

      if(page == 3){
        if(generate_counter < 128){
          ground[page][127] = 127;
          ground[page][95] = 255;
          ground[page][63] = 255;
          ground[page][31] = 255;
      } else{
          generate_obstacle();
          generate_counter = 0;
        }
      } 
}

int render_runner(){
  int n;
  for(n = 0; n < 128; n++){
      if(runner[n] < 255 & ground[0][n] == runner[n]) return 0;
      else if(runner[n] > ground[0][n] & ground[0][n] + runner[n] < 255) return 0;
      else ground[0][n] = ground[0][n] + runner[n] - 255;
  } return 1;
}

void derender_runner(){
  int n;
  for(n = 0; n < 128; n++){
    ground[0][n] = ground[0][n]+ 255 - runner[n];
  }
}

void render_screen(){
  display_image(0, ground[0]);
  display_image(32, ground[1]);
  display_image(64, ground[2]);
  display_image(96, ground[3]);
}

void scroll_screen_once(){
    scroll_page_once(0);
    scroll_page_once(1);
    scroll_page_once(2);
    scroll_page_once(3);
}


void runner_jump(){
  int index;
  for(index = 0; index < 128; index++){
    if(pixel_jump(index) == 0) runner[index-32] = (runner[index-32] - 128);
  }
}

int pixel_jump(int index){
  if(runner[index] < 255){
    int indx = runner[index] + 1;
    if(indx >> 1 == runner[index]>>1){
      runner[index] = (indx>>1) + 128;
      return 0;
    } else{
      runner[index] = (runner[index]>>1) + 128;
      return 1;
    } 
  } else{
    runner[index] = 255;
    return 1;
  }
}


void runner_fall(){
  int index;
  for(index = 127; index >= 0; index--){
    if(pixel_fall(index) == 0) runner[index+32] = runner[index+32] - 1;
  }
}

int pixel_fall(int index){
  if(runner[index] < 128){
      runner[index] = (runner[index]<<1) + 1;
      return 0;
  }else{
      runner[index] = (runner[index]<<1) + 1;
      return 1;
  }
}

void render_routine(){
  scroll_screen_once();
  int render = render_runner();
  if(render){
      render_screen();
    }
    else if(~render){
      while(1)display_string(5, game_over);
      display_update();
    }
    derender_runner();
    delay(delay_time);
}

void jump_routine(){
  int routine;
  jumping == 1;
  
  for(routine = 0; routine <= 11; routine++){
    runner_jump();
    runner_jump();
    render_routine();
  }
  for(routine = 11; routine >= 0; routine--){
    runner_fall();
    runner_fall();
    render_routine();
  }

  jumping == 0;
}


/* This function is called repetitively from the main program */
void labwork( void )
{ 

  volatile int button = getbtns();
  switch (button){
    case 1:
        if(delay_time > 10) delay_time -= 10;
        break;

    case 2:
        delay_time += 10;
        break;
    
    case 4:
      if(jumping == 0) jump_routine();
      break;
  } 
  render_routine();
}

