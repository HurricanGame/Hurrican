// Datei : Gegner.h

// -------------------------------------------------------------------------------------- 
//
// Gegner für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _GEGNERCLASS_H_
#define _GEGNERCLASS_H_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <d3dx8.h>
#include "DX8Sprite.h"


class PlayerClass;

// --------------------------------------------------------------------------------------
// Gegner Klasse (Werte für EINEN Gegner)
// --------------------------------------------------------------------------------------

class GegnerClass
{
	protected:											// protected wegen Ableitung		
		int					AnimStart;					// Erste Animationsphase				
		int					BlickRichtung;				// Links oder Rechts :)
		bool				ChangeLight;				// Licht des Levels annehmen ?				
		float				TurnCount;


	public:
		bool				AlreadyDrawn;
		int					HitSound;					// Metall = 0, Organisch = 1
		int					LastAction;					// Letzter Angriff (damit Bosse nicht immer das Gleiche machen)
		float				AnimSpeed, AnimCount;		// Anim-Geschwindigkeit und Counter		
		int					AnimPhase, AnimEnde;		// Aktuelle Phase und Endphase		
		bool				Active;						// Gegner überhaupt aktiv ?
		int					Handlung;					// Aktuelle "Handlung" des Gegners
		bool				Destroyable;				// Ist er kaputtbar ? :)
		bool				TestBlock;					// Auf Block mit der Wand testen ?
		bool				OwnDraw;					// Hat eigene Draw-Routine in der KI Funktion?
		bool				DontMove;					// Muss der Gegner bewegt werden?
		bool				ForceLight;					// Gegner wird auf jeden Fall von Level "beleuchtet"?
		bool				BackGround;					// Gegner zuerst rendern, da zu Background-Grafik gehört? (z.B. Lüfter)
		int					GegnerArt;					// Art des Gegners (siehe Defines)
		float				xPos,yPos;					// Position des Gegners
		float				xPosOld,yPosOld;			// Position des Gegners
		float				xSpeed,ySpeed;				// Geschwindigkeit des Gegners
		float				xAcc, yAcc;					// Beschleunigung des Gegners
		int					Value1, Value2;				// Trigger-Werte
		float				DamageTaken;				// Wie lange rot nach Treffer
		float				Energy;						// Energie des Gegners
		float				TimeToChange;				// pAim wechseln :)		
		int					blocku, blocko,
							blockl, blockr;				// Block links rechts über und unter dem Gegner
		PlayerClass			*pAim;						// Player to attack

		 GegnerClass	 (void);						// Konstruktor
		~GegnerClass	 (void);						// Destruktor
virtual void GegnerExplode(void);						// Gegner explodiert
virtual void DoKI	(void)	{}							// Gegner individuell bewegen
virtual void DoDraw (void)	{}							// Gegner individuell rendern (nur bei manchen)
	   	bool Run	(void);								// Gegner bewegen, checken etc
		void Render (void);								// nur rendern
		int  PlayerAbstand(bool both = false);			// Abstand Gegner/Spieler zurückliefern
		int  PlayerAbstandHoriz(PlayerClass *pTarget = NULL);		// Nur Horizontaler Abstand
		int  PlayerAbstandVert (PlayerClass *pTarget = NULL);		// Nur Vertikaler   Abstand
		void TestDamagePlayers (float dam, bool detroy = false);// Spieler verletzt?
		void PlattformTest	   (RECT rect);				// Steht der Spieler auf dem Gegner ?
		void Wegschieben	   (RECT rect, float dam);	// Gegner schiebt Spieler beiseite
		void SimpleAnimation   (bool backward = false);	// Einfache Animation (loop)
		void TurnonWall		   (void);					// An der Wand umdrehen?
		bool TurnonShot		   (void);					// Umdrehen, wenn angeschoßen
		bool IsOnScreen		   (void);					// Gegner grade sichtbar?
		GegnerClass		*pNext;							// Zeiger auf den nächsten   Gegner
		GegnerClass		*pPrev;							// Zeiger auf den vorherigen Gegner
};

// --------------------------------------------------------------------------------------
// Gegner Klasse, Linked List, die alle Gegner beinhaltet
// --------------------------------------------------------------------------------------

class GegnerListClass
{
	private:
		int					NumGegner;				// aktuelle Zahl der Gegner

	public:
		GegnerClass			*pStart;				// Erstes  Element der Liste
		GegnerClass			*pEnd;					// Letztes Element der Liste

		GegnerListClass(void);						// Konstruktor
	   ~GegnerListClass(void);						// Destruktor

	    bool PushGegner(float x, float y, int Art,	// Gegner "Art" hinzufügen
						int Value1,
						int Value2,
						bool Light, bool atEnd = true);
		void DelSel		(GegnerClass *pTemp);			// Ausgewählten Gegner entfernen
		void ClearAll	(void);							// Alle Gegner löschen
	    int  GetNumGegner(void);						// Zahl der Gegner zurückliefern
		void RunAll	  (void);							// Alle Gegner der Liste animieren
		void RenderAll(void);							// Alle Gegner der Liste rendern
		void DamageEnemiesonScreen(float x, float y,	// Alle Gegner ausgehend von Stelle x/y verwunden (Granate)
								   int MaxDamage);
};

#endif