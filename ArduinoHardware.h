#ifndef ARDUINOHARDWARE_H
#define ARDUINOHARDWARE_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "IHardware.h"

class ArduinoHardware : public IHardware
{
private:

    LiquidCrystal_I2C lcd;

    byte estadoLeds;

public:

    ArduinoHardware();

    void init() override;

    void motorLigar() override;
    void motorDesligar() override;

    void buzzer(bool estado) override;

    void ledErro(bool estado) override;

    void ledPosicao(
        int posicao,
        bool estado
    ) override;

    void lcdMensagem(
        const char* linha1,
        const char* linha2
    ) override;

    void esperarMs(
        unsigned long tempo
    ) override;
};

#endif