#ifndef _GEGNER_JAEGER_H_
#define _GEGNER_JAEGER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerJaeger : public GegnerClass
{
private :
    float smokeDelay;

public :

    GegnerJaeger(int  Wert1, int Wert2,					// Konstruktor
                 bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
