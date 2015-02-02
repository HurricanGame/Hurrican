#ifndef _GEGNER_KETTENGLIED_H
#define _GEGNER_KETTENGLIED_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerKettenglied : public GegnerClass
{

public:
    GegnerClass *pParent;

    GegnerKettenglied(int  Wert1, int Wert2,			// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Eigene Drawroutine
};

#endif
