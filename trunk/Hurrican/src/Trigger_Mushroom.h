#ifndef _TRIGGER_MUSHROOM_H_
#define _TRIGGER_MUSHROOM_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerMushroom : public GegnerClass
{
private:
    float	ySize;
    float	yCount;
    float	count;

public:

    GegnerMushroom  (int Wert1,	int Wert2,		// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
    void PlayerJumps   (PlayerClass *pPlayer);
};

#endif
