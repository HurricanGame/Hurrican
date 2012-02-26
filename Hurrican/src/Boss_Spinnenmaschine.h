#ifndef _BOSS_SPINNENMASCHINE_H_
#define _BOSS_SPINNENMASCHINE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

#define TIME_TILL_OPEN	10.0f
#define TIME_TILL_CLOSE 40.0f

#define TIME_TILL_HOCH  30.0f


#define	ZU			0
#define OFFEN		1
#define OEFFNEN		2
#define SCHLIESSEN	3

class GegnerSpinnenmaschine : public GegnerClass
{
	private:
		int		DisplayState;							// Was ist auf dem Monitor zu sehen?
		int		OldDisplayState;						// Was ist auf dem Monitor zu sehen?
		int		DeckelPhase;							// AnimPhase des Deckels
		float	DeckelCount;							// Counter für Sinusbewegung des Hochgehenden Deckels
		float	DeckelOffset;							// Tatsächlicher Offset (=sin(DeckelCount) * X)
		int		DeckelStatus;							// Aktueller Status (siehe #defines)
		float	SizeCount;									
		float	OpenCounter;							// Cunter wann der Deckel aufgeht
		float	HochCounter;							// Conter wann der Kopf hochfährt
		int		HochStatus;								// Status des Kopfs (siehe #defines)
		float	ShotDelay;

		float	SpawnDelay;								// Delay zum Gegner ausspucken

		float	SmokeDelay;								// Rauchcounter
		float	SmokeDelay2;							// Rauchcounter für Damage-Rauch

		float	LightRayCount;							// Counter, wie breit der Lichtkegel ist

		float	DeathCount;

		int		AnimUnten;								// Aktuelle Animphase für das Unterteil

		bool	AktionFertig;							// Aktion fertig (Deckel öffnen, hoch/runter fahren)

	public:
		DirectGraphicsSprite	Deckel;
		DirectGraphicsSprite	Unten[2];
		DirectGraphicsSprite	Display;
		DirectGraphicsSprite	Strahl;


	GegnerSpinnenmaschine(int Wert1,	int Wert2,				// Konstruktor
							  bool		  Light);	
	void DoDeckel(void);
	void DoHoch(void);
	void GegnerExplode (void);							// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
	void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
