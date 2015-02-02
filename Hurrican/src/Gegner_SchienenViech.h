#ifndef _GEGNER_SCHIENENVIECH_H_
#define _GEGNER_SCHIENENVIECH_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSchienenViech : public GegnerClass
{
public :

    GegnerSchienenViech(int  Wert1, int Wert2,			// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit KI bewegen
    void DoDraw		   (void);							// eigene Drawfunktion
};

#endif
