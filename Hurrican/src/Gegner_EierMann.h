#ifndef _GEGNER_EIERMANN_H_
#define _GEGNER_EIERMANN_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerEierMann : public GegnerClass
{
public :

    GegnerEierMann (int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
