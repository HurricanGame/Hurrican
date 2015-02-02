#ifndef _GEGNER_EISZAPFEN_H_
#define _GEGNER_EISZAPFEN_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerEiszapfen : public GegnerClass
{
public :

    GegnerEiszapfen(int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
