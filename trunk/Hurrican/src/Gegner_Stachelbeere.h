#ifndef _GEGNER_STACHELBEERE_H_
#define _GEGNER_STACHELBEERE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerStachelbeere : public GegnerClass
{
private:
    float	RollCount;
    int		ShotCount;

public:

    GegnerStachelbeere  (int  Wert1, int Wert2,			// Konstruktor
                         bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    void DoDraw		   (void);
    // eigenen kleinen KI bewegen
};

#endif
