#ifndef _GEGNER_SCHNEEKOPPE_H_
#define _GEGNER_SCHNEEKOPPE_H_

#include "Gegner_Stuff.h"
#include "GegnerClass.h"

class GegnerSchneekoppe : public GegnerClass
{
private:
    bool	isHit;

public:

    GegnerSchneekoppe		   (int  Wert1, int Wert2,			// Konstruktor
                                bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// eigene Draw Funktion
};

#endif
