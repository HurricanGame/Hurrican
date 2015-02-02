#ifndef _GEGNER_SCHLEIMMAUL_H_
#define _GEGNER_SCHLEIMMAUL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSchleimMaul : public GegnerClass
{
private:
    float	SpawnCount;
    float	ShotDelay;
    GegnerClass	*pChildren[10];

public :

    GegnerSchleimMaul(float x, float y, int  Wert1, int Wert2,
                      bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
