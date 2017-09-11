#ifndef _GEGNER_LAVAKRABBE_HPP_
#define _GEGNER_LAVAKRABBE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerLavaKrabbe : public GegnerClass
{
public:
    float			zPos;							// zPos da die Krabbe ja aus dem Hintergrund kommt

    GegnerLavaKrabbe (int  Wert1, int Wert2,			// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif

