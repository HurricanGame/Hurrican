#ifndef _GEGNER_POKENUKE_H_
#define _GEGNER_POKENUKE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerPokeNuke : public GegnerClass
{
private:
    float	SmokeDelay;

public:

    GegnerPokeNuke (int  Wert1, int Wert2,			// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
