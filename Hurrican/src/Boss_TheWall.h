#ifndef _BOSS_THEWALL_H_
#define _BOSS_THEWALL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"


class GegnerTheWall : public GegnerClass
{
private:
    float	ShotDelay;
    float	SmokeDelay;								// Rauchcounter
    float	KringelWinkel;
    float	HeartBeat;
    int		ShotCount;
    int		HeartAnim;
    float	DoorOffset;
    float	DoorCount;
    int		CountOpen;
    float	OldEnergy;
    float	SkullEnergy;
    float	VorneX, VorneY;
    float	Laughing;
    float	dummy;
    float	SkullShotDelay;

    void ReturnSkull(void);

public:
    DirectGraphicsSprite	Vorne;
    DirectGraphicsSprite	Tuer;
    DirectGraphicsSprite	Rahmen;
    DirectGraphicsSprite	Kringel[2];
    DirectGraphicsSprite	Brain;


    GegnerTheWall(int Wert1, int Wert2, bool Light);

    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
    void NeueAktion	   (void);
};

#endif
