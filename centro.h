#ifndef CENTRO_H
#define CENTRO_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// ====================================================================
// DEFINIÇÃO CENTRALIZADA DOS ENDEREÇOS I2C (Padrão Portátil)
// ====================================================================
constexpr byte ADDR_LEDS     = 0x20; // Expansor PCF8574 focado em SAÍDAS (LEDs das vagas)
constexpr byte ADDR_SENSORES = 0x21; // Expansor PCF8574 focado em ENTRADAS (Sensores/Botões)

class CentroDistribuicao {
public:
    // Construtor: Configura o display LCD e aplica o estoque inicial
    CentroDistribuicao(LiquidCrystal_I2C *lcd_param);
    
        void init();
    
    // Processamento de comandos e controle do pátio
    void localizarItem(int numeroPosicao);
    void separacaoPedidos(int numeroPosicao);
    void atualizacaoEstoque();
    
    // Métodos de comunicação direta com o barramento I2C dos PCFs
    void acionarLedPCF(int posicao, bool estado);
    bool lerSensorPCF(int posicao);

private:
    LiquidCrystal_I2C *_lcd;
    int _totalPosicoes;
    int _quantidadeEstoque[101];    // Suporta de 1 a 100 posições
    byte estadoLedsEspelho[13];     // Buffer de RAM para espelhar o estado dos LEDs
};

#endif