#ifndef _GEGNER_KUGELGROSS_H_
#define _GEGNER_KUGELGROSS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerKugelGross : public GegnerClass
{
public:

    GegnerKugelGross (int  Wert1, int Wert2,			// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
