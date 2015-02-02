#ifndef _GEGNER_LAVAMANN_H
#define _GEGNER_LAVAMANN_H

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLavamann : public GegnerClass
{
private:

    float	FlameCount;

public:

    GegnerLavamann	   (int Wert1, int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif

