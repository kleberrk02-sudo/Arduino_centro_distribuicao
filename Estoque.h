#ifndef ESTOQUE_H
#define ESTOQUE_H

class Estoque
{
private:

    int quantidade[101];

public:

    Estoque();

    int consultar(int posicao);

    bool retirar(int posicao);

    bool existe(int posicao);
};

#endif
