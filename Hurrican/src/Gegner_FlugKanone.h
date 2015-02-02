#ifndef _GEGNER_FLUGKANONE_H_
#define _GEGNER_FLUGKANONE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFlugKanone : public GegnerClass
{
private:
    float ShotDelay;

public:

    GegnerFlugKanone(int  Wert1, int Wert2,				// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
