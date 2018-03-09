#ifndef _GEGNER_SCHWABBEL_HPP_
#define _GEGNER_SCHWABBEL_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchwabbel : public GegnerClass
{
private:
    float	shotdelay;

public :

    GegnerSchwabbel(int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
