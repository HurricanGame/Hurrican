#ifndef _BOSS_UFO_HPP_
#define _BOSS_UFO_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerUfo : public GegnerClass
{
public:

    float			ShotDelay;						// Schussverzögerung
    float			ActionDelay;					// Wie lange wird die aktuelle Aktion durchgeführt
    float			SmokeDelay;

    GegnerUfo		   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    void DoDraw		   (void);
    // eigenen kleinen KI bewegen
};

#endif
