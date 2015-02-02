#ifndef _TRIGGER_LIGHTFLARE_H_
#define _TRIGGER_LIGHTFLARE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLightFlare : public GegnerClass
{
private:
    int		 r, g, b;
    float	 rot;
    float	 rotspeed;
    float	 Alpha;

public:

    GegnerLightFlare  (int Wert1,	int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
