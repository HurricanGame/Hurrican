#ifndef _GEGNER_FLUGKANONE_HPP_
#define _GEGNER_FLUGKANONE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFlugKanone : public GegnerClass
{
private:
    float ShotDelay;

public:

    GegnerFlugKanone(int  Wert1, int Wert2,				// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
