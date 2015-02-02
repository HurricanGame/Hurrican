#ifndef _GEGNER_SCHWABBEL_H_
#define _GEGNER_SCHWABBEL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSchwabbel : public GegnerClass
{
private:
    float	shotdelay;

public :

    GegnerSchwabbel(int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
