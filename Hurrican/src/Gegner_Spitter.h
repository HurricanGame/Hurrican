#ifndef _GEGNER_SPITTER_H
#define _GEGNER_SPITTER_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSpitter : public GegnerClass
{
public:

    GegnerSpitter		(int Wert1, int Wert2,			// Konstruktor
                         bool			Light);
    void GegnerExplode  (void);							// Gegner explodiert
    void DoKI			(void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
