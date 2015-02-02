#ifndef _GEGNER_ROTZPOTT_H_
#define _GEGNER_ROTZPOTT_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerRotzpott : public GegnerClass
{
private:
    float	GunWinkel;
    float	ShotDelay;
    float	NewWinkel;

public :

    GegnerRotzpott (int  Wert1, int Wert2,				// Konstruktor
                    bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Eigene Drawroutine
    void CalcKnarreWinkel(void);

};

#endif
