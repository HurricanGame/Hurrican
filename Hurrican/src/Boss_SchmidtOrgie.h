#ifndef _BOSS_SCHMIDTORGIE_H_
#define _BOSS_SCHMIDTORGIE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSchmidtOrgie : public GegnerClass
{
private:
    float	ActionCounter;

public:

    GegnerSchmidtOrgie (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner 														// eigenen kleinen KI bewegen
};

#endif
