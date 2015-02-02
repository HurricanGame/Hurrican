#ifndef _GEGNER_QUALLE_H_
#define _GEGNER_QUALLE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerQualle : public GegnerClass
{
private:
    float ShotDelay;

public:

    GegnerQualle	   (int  Wert1, int Wert2,			// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
