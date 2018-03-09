#ifndef _BOSS_ROLLMOPS_HPP_
#define _BOSS_ROLLMOPS_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Kettenglied.hpp"
#include "Gegner_Stuff.hpp"

#define NUM_KETTENGLIEDER	20

class GegnerRollmops : public GegnerClass
{
private:

    DirectGraphicsSprite	Rollen;
    DirectGraphicsSprite	Aufklappen;
    DirectGraphicsSprite	Gun;

    GegnerKettenglied		*pKettenTeile[NUM_KETTENGLIEDER];

    float	GunWinkel;
    float	SmokeCount;									// Für Antriebspartikel
    float	ShotDelay;
    float	ShotCount;
    float	HookX, HookY;
    float	Schwung, SchwungDir;
    float	Delay;
    float	HitsToTake;

    void	NeueAktion(void);
    void	GunAusfahren(bool Auf);
    void	Losrollen(void);
    void	Abhopsen(float mul = -0.6f);
    void    CalcGunWinkel(void);
    void	RoundShot(bool single);

public:

    GegnerRollmops	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern														// eigenen kleinen KI bewegen
};

#endif
