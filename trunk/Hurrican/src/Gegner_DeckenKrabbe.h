#ifndef _GEGNER_DECKENKRABBE_H_
#define _GEGNER_DECKENKRABBE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerDeckenKrabbe : public GegnerClass
{
public:
    float			zRot;							// Rotation
    float			zSpeed;							// Rotationsspeed

    GegnerDeckenKrabbe (int  Wert1, int Wert2,			// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
