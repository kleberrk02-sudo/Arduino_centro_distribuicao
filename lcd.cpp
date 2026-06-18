#include <Arduino.h>
#include "lcd.h"

void initLCD(LiquidCrystal_I2C *lcd_param) {
    if (lcd_param != NULL) {
        lcd_param->init();       // Inicializa o display I2C
        lcd_param->backlight();  // Liga a luz de fundo do display
        lcd_param->clear();      // Limpa a tela
    }
}