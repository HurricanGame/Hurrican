#ifndef _BOSS_RIESENRAUPE_H_
#define _BOSS_RIESENRAUPE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerRiesenRaupe : public GegnerClass
{
private:
    int	ShotCount;

public:

    GegnerRiesenRaupe(int  Wert1, int Wert2,			// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
