
#ifndef _REMOCON_HEADER
#define _REMOCON_HEADER
  	
  	#include "RH_RF69.h"
  	#include <Wire.h>
	#include <STM32ADC.h>
    
    #ifdef _CONTROLLER
    ////////////////////////Libraries//////////////
  
	    #include <Adafruit_GFX.h>
	    #include <Adafruit_SSD1306_STM32.h>
	  
	    ///////////////////////Pin Names///////////////
	    #define OLED_RESET  -1
	    
	    #define pinLStick_X    PB0
	    #define pinLStick_Y    PB1
	    #define pinRStick_X    PA6
	    #define pinRStick_Y    PA7
	    #define pinLeftButton    PB11
	    #define pinRightButton     PB10
	    
	    #define pinLeftSlider  PA3
	    #define pinCenterSlider PA4
	    #define pinRightSlider PA5
	    
	    #define pinUp PA9
	    #define pinDown PB8
	    #define pinLeft PA10
	    #define pinRight PB5
	    #define pinA PC14
	    #define pinB PC13
	    
	    #define pinRightPot PA2
	    #define pinLeftPot  PA1
	    #define pinConector PA0
    
	   	#include "Remocon.cpp"
    
    
    #endif //CONTROLLER
    #ifdef _RECEIVER

    #endif //RECEIVER
#endif // _REMOCON_HEADER
