#ifndef _GEGNER_NEUFISCH_H_
#define _GEGNER_NEUFISCH_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerNeuFisch : public GegnerClass
{
private:
    PlayerClass*	pFest;
    float			MoveSpeed;
public :

    GegnerNeuFisch (int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif

