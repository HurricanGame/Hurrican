#ifndef _BOSS_SKELETOR_HPP_
#define _BOSS_SKELETOR_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerSkeletor : public GegnerClass
{
private:
    float   SmokeDelay;
    int		ShotCount;
    float	ShotDelay;
    float	GunWinkel;
    bool	HasLaughed;
    int		AnimOffset;
    int		LastHandlung;
    bool	DrawNow;

    void	CalcGunWinkel();
    void	Laugh();

public:

    DirectGraphicsSprite	Flamme;

    GegnerSkeletor (int Wert1,	int Wert2,			// Konstruktor
                    bool		  Light);
    void GegnerExplode (void);						// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
