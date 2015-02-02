#ifndef _GEGNER_FIRESPIRDER_H_
#define _GEGNER_FIRESPIRDER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFireSpider : public GegnerClass
{
private:
    float	rot;
    float	shotdelay;

public :

    GegnerFireSpider(int  Wert1, int Wert2,			// Konstruktor
                     bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
