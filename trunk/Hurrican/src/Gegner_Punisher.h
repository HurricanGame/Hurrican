#ifndef _GEGNER_PUNISHER_H_
#define _GEGNER_PUNISHER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerPunisher : public GegnerClass
{
private :
    float	alpha;
    float	ShotDelay;
public :

    GegnerPunisher(int  Wert1, int Wert2,				// Konstruktor
                   bool Light);
    void FollowPlayer  (void);							// Spieler folgen
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
    void Vanish		   (void);
};

#endif
