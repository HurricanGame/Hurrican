#ifndef _GEGNER_WANDKRABBE_H_
#define _GEGNER_WANDKRABBE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerWandKrabbe : public GegnerClass
{
private :
    float rot;
    int   lastPlayerAction;

public :

    GegnerWandKrabbe(int  Wert1, int Wert2,				// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
