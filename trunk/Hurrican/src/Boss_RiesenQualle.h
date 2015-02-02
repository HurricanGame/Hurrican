#ifndef _BOSS_RIESENQUALLE_H_
#define _BOSS_RIESENQUALLE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerRiesenQualle : public GegnerClass
{
public:

    GegnerRiesenQualle(int  Wert1, int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
