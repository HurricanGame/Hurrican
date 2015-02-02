#ifndef _GEGNER_LAVAKRABBE_H_
#define _GEGNER_LAVAKRABBE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

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

