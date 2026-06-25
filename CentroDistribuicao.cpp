#include "CentroDistribuicao.h"
#include <stdio.h>  
#include <Arduino.h> 

CentroDistribuicao::CentroDistribuicao(
    IHardware* hardware,
    Estoque* est)
{
    hw = hardware;
    estoque = est;
}

void CentroDistribuicao::init()
{
    hw->init();
}

void CentroDistribuicao::atualizarTelaPrincipal()
{
    
    Serial.println("----------");
    hw->lcdMensagem(
        "Dig. Item 1-100",
        "#=local A=sep"
    );
}

void CentroDistribuicao::localizarItem(int posicao)
{
    if(!estoque->existe(posicao))
    {
        Serial.println("----------");
        Serial.print(F("[ERRO] Posicao invalida digitada: "));
        Serial.println(posicao);
        
        hw->lcdMensagem(
            "Posicao",
            "Invalida"
        );

        hw->esperarMs(1500);
        return;
    }

    // Registra a solicitação no Terminal Virtual antes da busca
    Serial.println("----------");
    Serial.print(F("[INFO] Solicitada localizacao do Item "));
    Serial.println(posicao);

    int qtd = estoque->consultar(posicao);

    if(qtd == 0)
    {
        Serial.print(F("[ALERTA] Localizacao REJEITADA - Item "));
        Serial.print(posicao);
        Serial.println(F(" esta ESGOTADO no galpao."));

        hw->lcdMensagem(
            "Item Esgotado",
            "Saldo: 0"
        );

        hw->buzzer(true);
        hw->ledErro(true);

        hw->esperarMs(800);
        hw->buzzer(false);
        return;
    }

    hw->ledErro(false);

    // 1. Primeiro avisa que está buscando na tela
    hw->lcdMensagem(
        "Localizando",
        "Item..."
    );

    // Logs de processamento visual no terminal
    Serial.print(F("[SUCESSO] Item "));
    Serial.print(posicao);
    Serial.println(F(" localizado. Processando busca visual..."));

    // 2. Pisca os LEDs físicos simulando a localização na bancada
    for(int i = 0; i < 3; i++)
    {
        hw->ledPosicao(posicao, true);
        hw->esperarMs(300);
        hw->ledPosicao(posicao, false);
        hw->esperarMs(300);
    }

    // 3. Formata e exibe o resultado do estoque por último no LCD e no Monitor
    char linha1[17];
    char linha2[17];

    sprintf(linha1, "Posicao: %d", posicao);
    sprintf(linha2, "Estoque: %d uni", qtd);

    hw->lcdMensagem(linha1, linha2);
    
    Serial.print(F("Quantidade em estoque: "));
    Serial.println(qtd);
    
    // 4. Segura a quantidade na tela por 3 segundos para leitura
    hw->esperarMs(3000); 
}

void CentroDistribuicao::separarPedido(int posicao)
{
    if(!estoque->existe(posicao))
    {
        Serial.println("----------");
        Serial.print(F("[ERRO] Tentativa de separacao em posicao invalida: "));
        Serial.println(posicao);

        hw->lcdMensagem(
            "Posicao",
            "Invalida"
        );

        hw->esperarMs(1500);
        return;
    }

    Serial.println("----------");
    Serial.print(F("[INFO] Iniciando processo de separacao do Item "));
    Serial.println(posicao);

    if(!estoque->retirar(posicao))
    {
        Serial.print(F("[ALERTA CRITICO] Esteira BLOQUEADA. Item "));
        Serial.print(posicao);
        Serial.println(F(" sem saldo para atendimento!"));

        hw->lcdMensagem(
            "Sem Estoque",
            ""
        );

        for(int i = 0; i < 5; i++)
        {
            hw->buzzer(true);
            hw->ledErro(true);

            hw->esperarMs(200);

            hw->buzzer(false);
            hw->ledErro(false);

            hw->esperarMs(200);
        }

        hw->ledErro(true);
        return;
    }

    hw->ledErro(false);

    // Alinha perfeitamente com a saída do terminal original
    hw->lcdMensagem(
        "Separando",
        "Pedido"
    );

    Serial.println(F("[ESTEIRA] Motor de transporte ativado... (3 segundos)"));

    hw->ledPosicao(posicao, true);

    hw->motorLigar();
    hw->esperarMs(3000);
    hw->motorDesligar();

    hw->ledPosicao(posicao, false);

    Serial.println(F("[ESTEIRA] Item transportado. Motor desligado."));

    // Computa a resposta de finalização e exibe o saldo restante no monitor
    Serial.print(F("[ESTOQUE] Baixa computada no Item "));
    Serial.print(posicao);
    Serial.print(F(". Novo saldo gerencial: "));
    Serial.println(estoque->consultar(posicao));

    Serial.println("----------");
    hw->lcdMensagem(
        "Pedido",
        "Concluido"
    );

    hw->esperarMs(2000);
}
