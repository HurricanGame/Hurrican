#ifndef _BOSS_SCHNEEKOENIG_HPP_
#define _BOSS_SCHNEEKOENIG_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"


class GegnerSchneeKoenig : public GegnerClass
{
private:
    float ySave;
    float LastEnergy;
    float SmokeDelay;								// Rauchcounter
    float yOffset;
    float KnarreWinkel;
    float KnarreY;
    float ShotDelay;
    int	  ShotCount;
    int	  SpezialAktion;
    int	  Action;
    float WackelAnim;
    float GunSlide;
    float WackelCount;
    bool  DrawNow;

    DirectGraphicsSprite	Knarre;
    DirectGraphicsSprite	Laser;

    float	WinkelToPlayer(void);
    void	RenderLaser(void);

public:

    GegnerSchneeKoenig(int  Wert1,	int Wert2,				// Konstruktor
                       bool	Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
    void NextAction    (int NewAction = -1);
};

#endif
