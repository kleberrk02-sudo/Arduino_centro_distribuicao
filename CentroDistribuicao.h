#ifndef CENTRODISTRIBUICAO_H
#define CENTRODISTRIBUICAO_H

#include "IHardware.h"
#include "Estoque.h"

class CentroDistribuicao
{
private:

    IHardware* hw;
    Estoque* estoque;

public:

    CentroDistribuicao(
        IHardware* hardware,
        Estoque* est
    );

    void init();

    void atualizarTelaPrincipal();

    void localizarItem(int posicao);

    void separarPedido(int posicao);
};

#endif
