#ifndef _GEGNER_MINIDRAGON_H_
#define _GEGNER_MINIDRAGON_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerMiniDragon : public GegnerClass
{
private:
    float	NewX, NewY;
    float	OldX, OldY;
    int		Segments;
    float	ShotDelay;

public :

    GegnerMiniDragon(int  Wert1, int Wert2,			// Konstruktor
                     bool Light);
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
    void GegnerExplode (void);							// Gegner explodiert
};

#endif
