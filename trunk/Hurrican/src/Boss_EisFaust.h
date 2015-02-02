#ifndef _BOSS_EISFAUST_H_
#define _BOSS_EISFAUST_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerEisFaust : public GegnerClass
{
public:

    GegnerEisFaust   (int Wert1,	int Wert2,			// Konstruktor
                      bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
