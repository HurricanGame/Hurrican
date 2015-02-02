#ifndef _GEGNER_STARBIG_H_
#define _GEGNER_STARBIG_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerStarBig : public GegnerClass
{
private :
    int ShotCount;
public :

    GegnerStarBig (int  Wert1, int Wert2,			// Konstruktor
                   bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
