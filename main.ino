#include "ArduinoHardware.h"
#include "ArduinoTeclado.h"

#include "Estoque.h"
#include "CentroDistribuicao.h"

ArduinoHardware hw;
ArduinoTeclado teclado;
Estoque estoque;

CentroDistribuicao cd(
    &hw,
    &estoque
);

char entrada[4];
uint8_t indice = 0;

bool telaPrincipalExibida = false;

void limparEntrada()
{
    indice = 0;
    entrada[0] = '\0';
    
    telaPrincipalExibida = false; 
}

void setup()
{
    
    hw.init(); 

    hw.lcdMensagem(
        "Centro de",
        "Distribuicao"
    );

    hw.esperarMs(4000);

    limparEntrada();
}

void loop()
{
    
    if(indice == 0 && !telaPrincipalExibida)
    {
        cd.atualizarTelaPrincipal();
        telaPrincipalExibida = true;
    }

    char tecla = teclado.lerTecla();

    if(tecla == 0)
    {
        delay(10); 
        return;
    }

    if(tecla >= '0' && tecla <= '9')
    {
        if(indice < 3)
        {
            entrada[indice++] = tecla;
            entrada[indice] = '\0';

            hw.lcdMensagem(
                "Digitar Posicao",
                entrada
            );
        }
        return;
    }

    if(tecla == '*')
    {
        limparEntrada();

        hw.lcdMensagem(
            "Cancelado",
            ""
        );

        hw.esperarMs(1000);
        return;
    }

    if(tecla == '#')
    {
        if(indice > 0)
        {
            int posicao = atoi(entrada);
            cd.localizarItem(posicao);
        }

        limparEntrada();
        return;
    }

    if(tecla == 'A')
    {
        if(indice > 0)
        {
            int posicao = atoi(entrada);
            cd.separarPedido(posicao);
        }

        limparEntrada();
        return;
    }
}
