#ifndef _TRIGGER_LIGHTFLARE_HPP_
#define _TRIGGER_LIGHTFLARE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

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
