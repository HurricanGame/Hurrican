#ifndef _GEGNER_ZITRONE_H_
#define _GEGNER_ZITRONE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerZitrone : public GegnerClass
{
private :
    float	SmokeDelay;
    float	WackelOffset;
    float	KnarreWinkel;
    float	FollowCount;
    float	NewX, NewY;
    float	ShotDelay;

public :

    GegnerZitrone (int  Wert1, int Wert2,					// Konstruktor
                   bool Light);
    void GegnerExplode   (void);							// Gegner explodiert
    void DoKI		     (void);							// Gegner individuell mit seiner  eigenen kleinen KI bewegen
    void DoDraw		     (void);							// Eigene Draw Routine
    void CalcKnarreWinkel(void);
};

#endif

