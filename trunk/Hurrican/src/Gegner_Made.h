#ifndef _GEGNER_MADE_H_
#define _GEGNER_MADE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerMade : public GegnerClass
{
private :
    float movesin;
public :

    GegnerMade (int  Wert1, int Wert2,					// Konstruktor
                bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner KI bewegen
    void DoDraw		   (void);							// eigene draw routine
};

#endif
