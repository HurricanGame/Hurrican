#ifndef _GEGNER_ONEUP_H_
#define _GEGNER_ONEUP_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerOneUp : public GegnerClass
{
public:

    GegnerOneUp			(int Wert1, int Wert2,			// Konstruktor
                         bool			Light);
    void GegnerExplode  (void);							// Gegner explodiert
    void DoKI			(void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif

