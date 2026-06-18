#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>           
#include "lcd.h"
#include "centro.h"

// Inicializa o display I2C no endereço 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);
CentroDistribuicao cd(&lcd);

// ==========================================
// CONFIGURAÇÃO DO TECLADO MATRICIAL 4X4
// ==========================================
const byte LINHAS = 4;
const byte COLUNAS = 4;

char matrizTeclas[LINHAS][COLUNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Definição dos pinos conectados nas Linhas e Colunas do teclado no PICSimLab
byte pinosLinhas[LINHAS]   = {9, 8, 7, 6};
byte pinosColunas[COLUNAS] = {5, 4, 3, 2}; 

Keypad teclado = Keypad(makeKeymap(matrizTeclas), pinosLinhas, pinosColunas, LINHAS, COLUNAS);

// Variável para armazenar o número digitado pelo usuário
String entradaUsuario = ""; 

void setup()
{
    initLCD(&lcd);
    lcd.init();         // Inicializa o display LCD
    lcd.backlight();    // Garante que a luz de fundo do display está ligada
    cd.init();          // Inicializa o hardware do Centro de Distribuição

    // ================================================================
    //  INICIALIZAÇÃO (Display 16x2)
    // ================================================================
    lcd.clear();
    
    
    lcd.setCursor(3, 0);
    lcd.print("Centro de");
    
    
    lcd.setCursor(2, 1);
    lcd.print("Distribuicao");
    
    delay(4000);        
}

void loop()
{
    // Só atualiza o estoque na tela se nenhuma tecla tiver sido digitada ainda
    if (entradaUsuario.length() == 0) {
        cd.atualizacaoEstoque();
    }

    // Lê a tecla pressionada no simulador
    char tecla = teclado.getKey();

    if (tecla != NO_KEY) {
        
        // Se for um número (de '0' a '9'), adiciona ao nosso texto de entrada
        if (tecla >= '0' && tecla <= '9') {
            if (entradaUsuario.length() < 3) { // Limita a 3 dígitos (já que vai até 100)
                entradaUsuario += tecla;
                
                // Mostra na tela o progresso da digitação sem ser atropelado
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Digitar Posicao:");
                lcd.setCursor(0, 1);
                lcd.print(entradaUsuario);
            }
        }
        
        // Se pressionar '*', limpa o que foi digitado (Cancelamento)
        else if (tecla == '*') {
            entradaUsuario = "";
            lcd.clear();
            lcd.print("Cancelado!");
            delay(1000);
        }
        
        // Se pressionar '#', CONFIRMA e executa a Localização do Item!
        else if (tecla == '#') {
            if (entradaUsuario.length() > 0) {
                int posicaoEscolhida = entradaUsuario.toInt(); // Converte o texto para número inteiro
                
                // Valida se a posição está dentro do escopo do galpão (1 a 100)
                if (posicaoEscolhida >= 1 && posicaoEscolhida <= 100) {
                    // Executa a rotina (pisca o LED equivalente no chip 0x20)
                    cd.localizarItem(posicaoEscolhida);
                } else {
                    lcd.clear();
                    lcd.print("Posicao Invalida!");
                    delay(1500);
                }
                
                // Reseta a variável para permitir que a tela principal de estoque volte a aparecer
                entradaUsuario = "";
            }
        }
        
        // Se pressionar 'A', CONFIRMA e executa a Separação de Pedidos!
        else if (tecla == 'A') {
            if (entradaUsuario.length() > 0) {
                int posicaoEscolhida = entradaUsuario.toInt();
                
                // Parêntese fechado e escopo delimitado corretamente
                if (posicaoEscolhida >= 1 && posicaoEscolhida <= 100) {
                    // Executa a separação (acende o LED fixo)
                    cd.separacaoPedidos(posicaoEscolhida);
                } else {
                    lcd.clear();
                    lcd.print("Posicao Invalida!");
                    delay(1500);
                }
                
                // Reseta a variável após a operação
                entradaUsuario = "";
            }
        }
    }
}