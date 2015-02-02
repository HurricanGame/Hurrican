#ifndef _GEGNER_MINIROCKET_H_
#define _GEGNER_MINIROCKET_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerMiniRocket : public GegnerClass
{
private:
    float	SmokeDelay;
    float	FlareSin;
    float	rot;

public :

    GegnerMiniRocket(int  Wert1, int Wert2,				// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    void DoDraw		   (void);							// Gegner individuell rendern
    // eigenen kleinen KI bewegen
};

#endif
