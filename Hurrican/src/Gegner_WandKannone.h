#ifndef _GEGNER_WANDKANNONE_H_
#define _GEGNER_WANDKANNONE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerWandKannone : public GegnerClass
{
public :

    GegnerWandKannone (int  Wert1, int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
