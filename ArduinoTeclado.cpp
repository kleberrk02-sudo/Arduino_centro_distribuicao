#include "ArduinoTeclado.h"
#include <Keypad.h>

const byte LINHAS = 4;
const byte COLUNAS = 4;

char matrizTeclas[LINHAS][COLUNAS] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

byte pinosLinhas[LINHAS] =
{
    9,8,7,6
};

byte pinosColunas[COLUNAS] =
{
    5,4,3,2
};

Keypad keypadHW(
    makeKeymap(matrizTeclas),
    pinosLinhas,
    pinosColunas,
    LINHAS,
    COLUNAS
);

ArduinoTeclado::ArduinoTeclado()
{
}

char ArduinoTeclado::lerTecla()
{
    return keypadHW.getKey();
}