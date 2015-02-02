#ifndef _GEGNER_SCHLEIMALIEN_H_
#define _GEGNER_SCHLEIMALIEN_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSchleimAlien : public GegnerClass
{
private:
    float		Size;

public:

    GegnerSchleimAlien(int  Wert1, int Wert2,
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
