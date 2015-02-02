#ifndef _BOSS_SKULL_H_
#define _BOSS_SKULL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSkull : public GegnerClass
{
private:
    float   mxPos, myPos;		// Koordinaten der Bossmaschine
    float   SmokeDelay;
    float	ShotDelay;
    int		ShotCount;
    float	Disappear;
    GegnerClass *pMachine;
public:

    GegnerSkull (int Wert1,	int Wert2,			// Konstruktor
                 bool		  Light);
    void GegnerExplode (void);						// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
