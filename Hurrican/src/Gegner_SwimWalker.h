#ifndef _GEGNER_SWIMWALKER_H
#define _GEGNER_SWIMWALKER_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSwimWalker : public GegnerClass
{
public:

    GegnerSwimWalker(int  Wert1, int Wert2,				// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
