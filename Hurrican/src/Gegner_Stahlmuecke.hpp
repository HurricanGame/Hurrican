#ifndef _GEGNER_STAHLMUECKE_HPP_
#define _GEGNER_STAHLMUECKE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerStahlmuecke : public GegnerClass
{
public:

    GegnerStahlmuecke  (int Wert1, int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);

};

#endif
