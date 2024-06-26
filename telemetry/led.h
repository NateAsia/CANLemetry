/**
 * @file led.h
 * @author Nathaniel Asia
 * @brief 
 * @version 0.1
 * @date 2024-06-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef LED_H
#define LED_H

#include <stdint.h>
#include "wiring_private.h"

class LED{
  private:
      uint8_t _pin;
      bool    _state; 
  public:
      LED(){}
      void init(uint8_t);
      void set(bool);
};

void LED::init(uint8_t pin){
    _pin = pin;
    pinMode(_pin, OUTPUT);
}
void LED::set(bool state){
    _state = state;
    digitalWrite(_pin, _state);
}

#endif