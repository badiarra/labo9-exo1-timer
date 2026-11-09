#include "Led.h"

Led::Led(int pin, const char* name){
    _pin = pin;
    _name = name;
}

void Led::init() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Led::on() {
    digitalWrite(_pin, HIGH);
}

void Led::off() {
    digitalWrite(_pin, LOW);
}

 const char* Led::getName(){
    return _name;
 }