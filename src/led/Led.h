#ifndef LED_H
#define LED_H
#include <Arduino.h>

class Led 
{
  public:
    Led(int pin, const char* name);
    void on();
    void off();
  
    void init();
    const char* getName();
  private:
    int _pin;
    const char* _name; //Une constante
};

#endif