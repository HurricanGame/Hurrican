#ifndef _GEGNER_MUTANT_H
#define _GEGNER_MUTANT_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerMutant : public GegnerClass
{
private :
    float zRot;
    float JumpDelay;
public :

    GegnerMutant(int  Wert1, int Wert2,					// Konstruktor
                 bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
