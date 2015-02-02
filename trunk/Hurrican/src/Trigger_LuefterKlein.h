#ifndef _TRIGGER_LUEFTERKLEIN_H_
#define _TRIGGER_LUEFTERKLEIN_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLuefterKlein : public GegnerClass
{
public :

    GegnerLuefterKlein(int  Wert1, int Wert2,					// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
