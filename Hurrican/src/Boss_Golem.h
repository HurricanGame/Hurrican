#ifndef _BOSS_GOLEM_H_
#define _BOSS_GOLEM_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

#define WACKELMAX 0.1f
#define ARMX -55.0f
#define ARMY 20.0f

#define ARMSIZEX   193
#define ARMSIZEY   179

#define ARM_HEBEN		0
#define ARM_SENKEN		1
#define ARM_SENKEN2		2
#define ARM_UEBERKOPF	3
#define ARM_WERFEN		4

class GegnerGolem : public GegnerClass
{
private:
    DirectGraphicsSprite	arm[2];
    DirectGraphicsSprite	load;
    float					Wackel;
    float					WackelDir;
    float					rotarm1;
    float					rotarm2;
    float					yoff;
    float					ShotDelay;
    float					count;
    int						state2;
    int						StoneCount;
    int						last;

public:

    GegnerGolem   (int Wert1,	int Wert2,			// Konstruktor
                   bool		  Light);
    float WinkelToPlayer(void);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
    void Wackeln	   (void);
    bool GoToZero(float &Value, float speed = 0.1f);
    // wackeln lassen beim Laufen
};

#endif
