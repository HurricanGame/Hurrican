#ifndef _BOSS_METALHEAD_H_
#define _BOSS_METALHEAD_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerMetalHead : public GegnerClass
{
	// Struct für einen Halswirbel
	//
	#define MAXWIRBEL 25

	// Aktionen/Angriffe (SK = StahlKopp)
	//
	#define SK_PAUSE	0		// Kurze Pause
	#define SK_AUSGANG	1		// Ausgansposition links oben
	#define SK_AUSHOLEN	2		// Ausholen zum Headbangen
	#define SK_BANGEN	3		// Headbangen, sprich, mit dem Kopf nach rechts schlagen
	#define SK_CHASING	4		// Spieler verfolgen
	#define SK_TURBINE	5		// Auf Turbinen-Ansaugen vorbereiten
	#define SK_WACKELN	6		// Kopf wackelt auf und ab und schlägt dabei gegen die Decke
	#define SK_SPUCKEN	7		// Kopf spuckt aus der Turbine Feuerbälle
	
	struct wirbel
	{	
		float x, y;
		float w;
	};

	private:
		bool	Turbine_dran;			// Ist die Turbine noch dran
		float	KieferSpeed;	
		float	KieferPos;
		float	TurbineOff;
		float	Eye_Alpha;
		float   SmokeCount;
		float	Eye_Winkel;
		int		ShotCount;
		float	ShotDelay;
		float	GunWinkel;
		int		Akt;					// Aktueller Angriff (siehe Defines)
		int		ShotArt;
		wirbel	Hals[MAXWIRBEL];

		float	NewX, NewY;
		float   MoveSpeed;				// Wie schnell bewegt sich der Kopf zum neuen Zielpunkt ?
		float	SinCount;

		// Für die Bewegung mit den xto und yto Koordinaten müssen wir uns am Anfang merken, ob
		// wir uns jetzt rechts oder links bzw hoch oder runter bewegen, um beim Check, ob der 
		// MoveTo Punkt erreicht wurde, die richtige Abfrage setzen zu können.
		//
		bool	Moving;

		void	DoMove(void);
		void	WinkelToPlayer(void);
		
	public:

	DirectGraphicsSprite	Kiefer;
	DirectGraphicsSprite	Kiefer2;
	DirectGraphicsSprite	Turbine;
	DirectGraphicsSprite	Turbine2;
	DirectGraphicsSprite	HalsGfx;
	DirectGraphicsSprite	Flare;	

	GegnerMetalHead (int Wert1,	int Wert2,			// Konstruktor
							  bool		  Light);	
	void GegnerExplode (void);						// Gegner explodiert
	void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
	void DoDraw		   (void);							// Gegner individuell rendern
	void MoveToNewPoint(float x, float y, float s,	// neuen MoveTo Punkt festlegen mit Pos, Speed und Aktion
						int Akt);	 
};

#endif
