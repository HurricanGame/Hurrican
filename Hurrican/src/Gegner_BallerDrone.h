#ifndef _GEGNER_BALLERDRONE_H_
#define _GEGNER_BALLERDRONE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerBallerdrone : public GegnerClass
{
private:
    float	shotdelay;

public :

    GegnerBallerdrone(int  Wert1, int Wert2,			// Konstruktor
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
