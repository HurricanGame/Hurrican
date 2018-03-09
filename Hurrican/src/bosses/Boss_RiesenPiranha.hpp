#ifndef _BOSS_RIESENPIRANHA_HPP_
#define _BOSS_RIESENPIRANHA_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerRiesenPiranha : public GegnerClass
{
public:

    float				ShotDelay;						// Schussverzögerung

    GegnerRiesenPiranha(int  Wert1, int Wert2,			// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
