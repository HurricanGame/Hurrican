#ifndef _TRIGGER_LUEFTERKLEIN2_HPP_
#define _TRIGGER_LUEFTERKLEIN2_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerLuefterKlein2 : public GegnerClass
{
public :

    GegnerLuefterKlein2(int  Wert1, int Wert2,					// Konstruktor
                        bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
