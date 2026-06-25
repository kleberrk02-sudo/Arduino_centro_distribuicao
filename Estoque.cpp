#include "Estoque.h"

Estoque::Estoque()
{
    for(int i = 1; i <= 100; i++)
    {
        quantidade[i] = 100;
    }

    quantidade[50] = 1;
}

bool Estoque::existe(int posicao)
{
    return posicao >= 1 &&
           posicao <= 100;
}

int Estoque::consultar(int posicao)
{
    if(!existe(posicao))
        return 0;

    return quantidade[posicao];
}

bool Estoque::retirar(int posicao)
{
    if(!existe(posicao))
        return false;

    if(quantidade[posicao] == 0)
        return false;

    quantidade[posicao]--;

    return true;
}
