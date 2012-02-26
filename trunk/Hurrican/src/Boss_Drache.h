#ifndef _BOSS_DRACHE_H_
#define _BOSS_DRACHE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerDrache : public GegnerClass
{
	private:

		DirectGraphicsSprite	Head[2];
		DirectGraphicsSprite	Hals;
		DirectGraphicsSprite	Tail;
		DirectGraphicsSprite	Spitze;
		DirectGraphicsSprite	Leg;
		DirectGraphicsSprite	Arm;
		
		bool	HeadLocked;				// Kopf ist fest und dreht nicht zu Spieler
		float	SmokeCount;				// Für Antriebspartikel
		float	TailSinus;				// Für das Schwanzwedeln (nicht das, was DU jetzt denkst... ;)
		float	HeadWinkel;				// Kopf Ausrichtung, zeigt immer auf Spieler
		float	AnimWinkel;				// Arme und Beine Winkel
		float	KieferWinkel;			// Kiefer kann auf und zu gehen
		bool	Growl;					// Heute schon gegröhlt? =)
		int		Attack;					// Welche "Unterhandlung"
		int		mirrorOffset;			// Zum Zeichnen wenn gespiegelt
		int		Position;
		float	HeadX, HeadY, HeadXSpeed, HeadYSpeed;
		int		ShotCount;
		float	ShotDelay;
		float	StartPosY;
		float	DrawYOffset;
		float	ArrowCount;
		float	FlareRot;

	void ComputeHeadWinkel(void);

	public:

	GegnerDrache	   (int Wert1,	int Wert2,			// Konstruktor
						bool		  Light);	
	void GegnerExplode (void);							// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
	void DoDraw		   (void);							// Gegner individuell rendern														// eigenen kleinen KI bewegen
};

#endif
