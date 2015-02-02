#ifndef _GEGNER_SMALLWESPE_H_
#define _GEGNER_SMALLWESPE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSmallWespe : public GegnerClass
{
public:

    GegnerSmallWespe  (int Wert1, int Wert2,			// Konstruktor
                       bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
