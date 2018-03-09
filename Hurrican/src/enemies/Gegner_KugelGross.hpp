#ifndef _GEGNER_KUGELGROSS_HPP_
#define _GEGNER_KUGELGROSS_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerKugelGross : public GegnerClass
{
public:

    GegnerKugelGross (int  Wert1, int Wert2,			// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
