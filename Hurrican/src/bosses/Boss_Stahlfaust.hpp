#ifndef _BOSS_STAHLFAUST_HPP_
#define _BOSS_STAHLFAUST_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerStahlfaust : public GegnerClass
{
public:
    float	SmokeDelay;

    GegnerStahlfaust   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
