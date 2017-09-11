#ifndef _GEGNER_STALAKTIT_HPP_
#define _GEGNER_STALAKTIT_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerStalagtit : public GegnerClass
{
public:

    GegnerStalagtit(int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
