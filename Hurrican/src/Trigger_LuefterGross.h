#ifndef _TRIGGER_LUEFTERGROSS_H_
#define _TRIGGER_LUEFTERGROSS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLuefterGross : public GegnerClass
{
public :

    GegnerLuefterGross(int  Wert1, int Wert2,					// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
