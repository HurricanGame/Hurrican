#ifndef _GEGNER_BROCKELROCK_H
#define _GEGNER_BROCKELROCK_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerBrockelRock : public GegnerClass
{
public:

    GegnerBrockelRock(int  Wert1, int Wert2,			// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
