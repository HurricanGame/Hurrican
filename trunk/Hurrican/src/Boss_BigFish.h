#ifndef _BOSS_BIGFISH_H_
#define _BOSS_BIGFISH_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerBigFish : public GegnerClass
{
private:
    bool					Moving;				// Bewegt sich der Fisch gerade?
    float					MaulWinkel;
    float					SinOff;
    int						ShotCount;
    float					NewX, NewY;
    float					rotz;
    float					KugelCount;
    DirectGraphicsSprite	Maul;
    DirectGraphicsSprite	FlosseKlein;
    DirectGraphicsSprite	FlosseGross;
    DirectGraphicsSprite	FlosseOben;
    DirectGraphicsSprite	FlosseUnten;

public:

    GegnerBigFish	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void NewAction	   (void);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner bewegen
    void DoDraw		   (void);							// Gegner individuell rendern														// eigenen kleinen KI bewegen
    void MoveTo		   (float x, float y);
    void DoMove		   (void);

};

#endif
