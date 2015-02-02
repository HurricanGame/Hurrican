#ifndef _GEGNER_AUGE_H_
#define _GEGNER_AUGE_H_

#include "Gegner_Stuff.h"
#include "GegnerClass.h"

class GegnerAuge : public GegnerClass
{
private:
    float	smokedelay;

public:

    GegnerAuge		   (int  Wert1, int Wert2,			// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif

