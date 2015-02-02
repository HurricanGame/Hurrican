#ifndef _BOSS_FLUGBOSS_H_
#define _BOSS_FLUGBOSS_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFlugBoss : public GegnerClass
{
private:
    DirectGraphicsSprite SchieneL;
    DirectGraphicsSprite SchieneR;

    DirectGraphicsSprite Kanone_big;
    DirectGraphicsSprite Kanone_medium;
    DirectGraphicsSprite Kanone_small;
    DirectGraphicsSprite Geschuetz;

    DirectGraphicsSprite Auge;
    DirectGraphicsSprite Hitzone[2];

    float SchienePos;
    float tempSpeed;
    float yKanone[4];

    float xKanone;
    float Winkel [2];

    float xHit;
    float yHit;
    float dHit;

    float alphaAuge;
    float alphaSpeed;
    float alphaDir;

    float ShotDelay;
    float ShotCount;

public:

    GegnerFlugBoss (int Wert1,	int Wert2,			// Konstruktor
                    bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
