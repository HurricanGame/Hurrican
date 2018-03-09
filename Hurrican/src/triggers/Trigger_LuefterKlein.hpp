#ifndef _TRIGGER_LUEFTERKLEIN_HPP_
#define _TRIGGER_LUEFTERKLEIN_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerLuefterKlein : public GegnerClass
{
public :

    GegnerLuefterKlein(int  Wert1, int Wert2,					// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
