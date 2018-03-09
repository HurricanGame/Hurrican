#ifndef _GEGNER_SPIDERBOMB_HPP_
#define _GEGNER_SPIDERBOMB_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSpiderBomb : public GegnerClass
{
public:

    GegnerSpiderBomb   (int Wert1, int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
