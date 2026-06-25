#include <Arduino.h>
#include "ArduinoHardware.h"

constexpr byte ADDR_LCD  = 0x27;
constexpr byte ADDR_LEDS = 0x20;

constexpr int PIN_MOTOR_A = A0;
constexpr int PIN_MOTOR_B = A1;
constexpr int PIN_LED_ERRO = A2;
constexpr int PIN_BUZZER = A3;

ArduinoHardware::ArduinoHardware()
:
lcd(ADDR_LCD,16,2)
{
    estadoLeds = 0;
}

void ArduinoHardware::init()
{
    Wire.begin();

    lcd.init();
    lcd.backlight();
    lcd.clear();

    pinMode(PIN_MOTOR_A, OUTPUT);
    pinMode(PIN_MOTOR_B, OUTPUT);

    pinMode(PIN_LED_ERRO, OUTPUT);

    pinMode(PIN_BUZZER, OUTPUT);

    digitalWrite(PIN_MOTOR_A, LOW);
    digitalWrite(PIN_MOTOR_B, LOW);

    digitalWrite(PIN_LED_ERRO, LOW);
    digitalWrite(PIN_BUZZER, LOW);

    Wire.beginTransmission(
        ADDR_LEDS
    );

    Wire.write(0x00);

    Wire.endTransmission();

    Serial.begin(9600);

    Serial.println();
    Serial.println(
        "Centro de Distribuicao"
    );
}

void ArduinoHardware::motorLigar()
{
    digitalWrite(
        PIN_MOTOR_A,
        HIGH
    );

    digitalWrite(
        PIN_MOTOR_B,
        LOW
    );
}

void ArduinoHardware::motorDesligar()
{
    digitalWrite(
        PIN_MOTOR_A,
        LOW
    );

    digitalWrite(
        PIN_MOTOR_B,
        LOW
    );
}

void ArduinoHardware::buzzer(
    bool estado)
{
    digitalWrite(
        PIN_BUZZER,
        estado
    );
}

void ArduinoHardware::ledErro(
    bool estado)
{
    digitalWrite(
        PIN_LED_ERRO,
        estado
    );
}

void ArduinoHardware::ledPosicao(
    int posicao,
    bool estado)
{
    int bitLed =
        (posicao - 1) % 8;

    if(estado)
    {
        estadoLeds |=
            (1 << bitLed);
    }
    else
    {
        estadoLeds &=
            ~(1 << bitLed);
    }

    Wire.beginTransmission(
        ADDR_LEDS
    );

    Wire.write(
        estadoLeds
    );

    Wire.endTransmission();
}

void ArduinoHardware::lcdMensagem(
    const char* linha1,
    const char* linha2)
{
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print(linha1);

    lcd.setCursor(0,1);
    lcd.print(linha2);

    Serial.println("----------------");
    Serial.println(linha1);
    Serial.println(linha2);
}

void ArduinoHardware::esperarMs(
    unsigned long tempo)
{
    delay(tempo);
}