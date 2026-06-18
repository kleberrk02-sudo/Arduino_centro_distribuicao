#include <Arduino.h>
#include <Wire.h>
#include "centro.h"

// ====================================================================
// MAPEAMENTO ELÉTRICO COESIVO (Barramento Isolado nas Portas Analógicas)
// ====================================================================
const int pinoMotorA       = A0; // Conectado em 23 PC0/A0 no PICSimLab
const int pinoMotorB       = A1; // Conectado em 24 PC1/A1 no PICSimLab
const int pinoLedVermelho  = A2; // Conectado em 25 PC2/A2 no PICSimLab
const int pinoBuzzer       = A3; // Conectado em 26 PC3/A3 no PICSimLab

// Construtor da Classe
CentroDistribuicao::CentroDistribuicao(LiquidCrystal_I2C *lcd_param) {
    _lcd = lcd_param;
    _totalPosicoes = 100; 
    
    // Preenche as posições como cheias para simulação (exceto o item 50)
    for (int i = 1; i <= 100; i++) {
        if (i == 50) {
            _quantidadeEstoque[i] = 1;  
        } else {
            _quantidadeEstoque[i] = 100; 
        }
    }
}

// Inicialização de todo o Hardware do Módulo
void CentroDistribuicao::init() {
    Wire.begin(); 
    
    // Configura a comunicação de monitoramento com o VTerm
    Serial.begin(9600);
    Serial.println(F("=================================================="));
    Serial.println(F("   SISTEMA SUPERVISORIO - CENTRO DE DISTRIBUICAO  "));
    Serial.println(F("                  Status: ONLINE                  "));
    Serial.println(F("=================================================="));
    
    // Configuração de pinagem física das saídas
    pinMode(pinoMotorA, OUTPUT);
    pinMode(pinoMotorB, OUTPUT);
    pinMode(pinoBuzzer, OUTPUT);
    pinMode(pinoLedVermelho, OUTPUT);
    
    // Garante estado elétrico seguro inicial (tudo desligado)
    digitalWrite(pinoMotorA, LOW);
    digitalWrite(pinoMotorB, LOW);
    digitalWrite(pinoBuzzer, LOW); 
    digitalWrite(pinoLedVermelho, LOW); 

    // Inicialização do chip de saídas usando a constante global
    Wire.beginTransmission(ADDR_LEDS);
    Wire.write(0x00); 
    Wire.endTransmission();
}

// Operação 1: Localizar Item no Galpão
void CentroDistribuicao::localizarItem(int numeroPosicao) {
    if (numeroPosicao < 1 || numeroPosicao > _totalPosicoes) {
        Serial.print(F("[ERRO] Posicao invalida digitada: "));
        Serial.println(numeroPosicao);
        return;
    }

    Serial.print(F("[INFO] Solicitada localizacao do Item "));
    Serial.println(numeroPosicao);

    if (_quantidadeEstoque[numeroPosicao] == 0) {
        Serial.print(F("[ALERTA] Localizacao REJEITADA - Item "));
        Serial.print(numeroPosicao);
        Serial.println(F(" esta ESGOTADO no galpao."));

        if (_lcd != NULL) {
            _lcd->clear();
            _lcd->print("Item Esgotado!");
            _lcd->setCursor(0, 1);
            _lcd->print("Estoque: 0 un");
        }
        
        // Dispara alarme visual/sonoro de erro
        digitalWrite(pinoBuzzer, HIGH);
        digitalWrite(pinoLedVermelho, HIGH);
        delay(800);
        
        digitalWrite(pinoBuzzer, LOW);
        digitalWrite(pinoLedVermelho, HIGH); 
        return;
    }

    digitalWrite(pinoLedVermelho, LOW);

    if (_lcd != NULL) {
        _lcd->clear();
        _lcd->setCursor(0, 0);
        _lcd->print("Localizando...");
        _lcd->setCursor(0, 1);
        _lcd->print("Item "); _lcd->print(numeroPosicao);
        _lcd->print(" Qtde:"); _lcd->print(_quantidadeEstoque[numeroPosicao]);
    }

    Serial.print(F("[SUCESSO] Item "));
    Serial.print(numeroPosicao);
    Serial.print(F(" localizado. Quantidade em estoque: "));
    Serial.println(_quantidadeEstoque[numeroPosicao]);

    // Efeito visual de indicação piscando o LED da vaga correspondente
    for (int i = 0; i < 3; i++) {
        acionarLedPCF(numeroPosicao, true);
        delay(300);
        acionarLedPCF(numeroPosicao, false);
        delay(300);
    }
}

// Operação 2: Despacho e Separação de Pedidos (Acionamento da Esteira)
void CentroDistribuicao::separacaoPedidos(int numeroPosicao) {
    if (numeroPosicao < 1 || numeroPosicao > _totalPosicoes) {
        Serial.print(F("[ERRO] Tentativa de separacao em posicao invalida: "));
        Serial.println(numeroPosicao);
        return;
    }

    Serial.print(F("[INFO] Iniciando processo de separacao do Item "));
    Serial.println(numeroPosicao);

    if (_quantidadeEstoque[numeroPosicao] == 0) {
        Serial.print(F("[ALERTA CRITICO] Esteira BLOQUEADA. Item "));
        Serial.print(numeroPosicao);
        Serial.println(F(" sem saldo para atendimento!"));

        if (_lcd != NULL) {
            _lcd->clear();
            _lcd->print("Erro: Item "); _lcd->print(numeroPosicao);
            _lcd->setCursor(0, 1);
            _lcd->print("Sem estoque!");
        }
        
        // Ciclo intermitente de erro crítico (Pisca LED e Alarme)
        for(int k = 0; k < 5; k++) {
            digitalWrite(pinoBuzzer, HIGH);
            digitalWrite(pinoLedVermelho, HIGH);
            delay(200);
            digitalWrite(pinoBuzzer, LOW);
            digitalWrite(pinoLedVermelho, LOW);
            delay(200);
        }
        
        digitalWrite(pinoLedVermelho, HIGH); // Deixa o LED de falha retido
        return;
    }

    digitalWrite(pinoLedVermelho, LOW);

    if (_lcd != NULL) {
        _lcd->clear();
        _lcd->setCursor(0, 0);
        _lcd->print("Separando Item "); _lcd->print(numeroPosicao);
        _lcd->setCursor(0, 1);
        _lcd->print("Esteira Ativa...");
    }

    Serial.println(F("[ESTEIRA] Motor de transporte ativado... (3 segundos)"));
    
    // Liga a sinalização local e liga o motor direcional
    acionarLedPCF(numeroPosicao, true);
    digitalWrite(pinoMotorA, HIGH); 
    digitalWrite(pinoMotorB, LOW);  

    delay(3000); // Executa o ciclo de movimentação física

    // Desligamento seguro do motor
    digitalWrite(pinoMotorA, LOW);
    digitalWrite(pinoMotorB, LOW);
    
    Serial.println(F("[ESTEIRA] Item transportado. Motor desligado."));

    // Atualiza a tabela interna de inventário
    _quantidadeEstoque[numeroPosicao]--;
    acionarLedPCF(numeroPosicao, false);
    
    Serial.print(F("[ESTOQUE] Baixa computada no Item "));
    Serial.print(numeroPosicao);
    Serial.print(F(". Novo saldo gerencial: "));
    Serial.println(_quantidadeEstoque[numeroPosicao]);
    Serial.println(F("--------------------------------------------------"));

    if (_lcd != NULL) {
        _lcd->clear();
        _lcd->print("Retirado 1 un!");
        _lcd->setCursor(0, 1);
        _lcd->print("Saldo Atual: "); _lcd->print(_quantidadeEstoque[numeroPosicao]);
    }
    delay(2000);
}

// Operação 3: Exibição do Menu do Sistema
void CentroDistribuicao::atualizacaoEstoque() {
    if (_lcd != NULL) {
        _lcd->setCursor(0, 0);
        _lcd->print("Dig. Item 1-100");
        _lcd->setCursor(0, 1);
        _lcd->print("#=loc. A=Separa"); 
    }
}

// ====================================================================
// COMUNICAÇÃO DE BAIXO NÍVEL COM OS EXPANSÓRIOS I2C (PCF8574)
// ====================================================================

void CentroDistribuicao::acionarLedPCF(int posicao, bool estado) {
    int pinoBit = (posicao - 1) % 8; 

    // Aplica a máscara de bits diretamente no buffer de memória
    if (estado) {
        estadoLedsEspelho[0] |= (1 << pinoBit);  
    } else {
        estadoLedsEspelho[0] &= ~(1 << pinoBit); 
    }

    // Transmite o buffer modificado exclusivamente para o chip de LEDs
    Wire.beginTransmission(ADDR_LEDS); 
    Wire.write(estadoLedsEspelho[0]);
    Wire.endTransmission();
} 

bool CentroDistribuicao::lerSensorPCF(int posicao) {
    // Exemplo estruturado usando o segundo endereço para expansão futura
    Wire.requestFrom(ADDR_SENSORES, 1);
    if (Wire.available()) {
        byte leituraBytes = Wire.read();
        // (Aqui entraria a filtragem do bit físico lido do sensor)
    }
    
    return _quantidadeEstoque[posicao] > 0; 
}