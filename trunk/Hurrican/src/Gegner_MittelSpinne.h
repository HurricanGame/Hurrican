#ifndef _GEGNER_MITTELSPINNE_H_
#define _GEGNER_MITTELSPINNE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerMittelSpinne : public GegnerClass
{
private :
    float	rot;
    float	WalkDelay;
    float	shotdelay;
    float	yStart;

public :

    GegnerMittelSpinne(int  Wert1, int Wert2, bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
