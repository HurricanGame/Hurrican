#ifndef _BOSS_EVILHURRI_H_
#define _BOSS_EVILHURRI_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerEvilHurri : public GegnerClass
{
public:

    float					BlinkDirection;				// Animationsphase der Beine
    float					BlinkWert;					// Y-Offset des Körpers
    float					ActionDelay;				// Warten

    GegnerEvilHurri   (int Wert1,	int Wert2,			// Konstruktor
                       bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
