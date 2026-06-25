#ifndef IHARDWARE_H
#define IHARDWARE_H

class IHardware
{
public:

    virtual void init() = 0;

    virtual void motorLigar() = 0;
    virtual void motorDesligar() = 0;

    virtual void buzzer(bool estado) = 0;

    virtual void ledErro(bool estado) = 0;

    virtual void ledPosicao(
        int posicao,
        bool estado
    ) = 0;

    virtual void lcdMensagem(
        const char* linha1,
        const char* linha2
    ) = 0;

    virtual void esperarMs(
        unsigned long tempo
    ) = 0;

    virtual ~IHardware() {}
};

#endif
