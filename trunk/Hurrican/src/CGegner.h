// Datei : GegnerClass.h

// --------------------------------------------------------------------------------------
//
// Gegnerklasse für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _GegnerClass_h_
#define _GegnerClass_h_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <d3dx8.h>
#include "mmgr.h"
#include "DX8Sprite.h"

// --------------------------------------------------------------------------------------
// Gegner Klasse (Werte für EINEN Gegner)
// --------------------------------------------------------------------------------------

class GegnerClass
{
protected:											// protected wegen Ableitung
    int					AnimStart;					// Erste Animationsphase
    int					AnimPhase, AnimEnde;		// Aktuelle Phase und Endphase
    float				AnimSpeed, AnimCount;		// Anim-Geschwindigkeit und Counter
    int					BlickRichtung;				// Links oder Rechts :)
    bool				ChangeLight;				// Licht des Levels annehmen ?
    int					LastAction;					// Letzter Angriff (damit Bosse nicht immer das Gleiche machen)

public:
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
    int					blocku, blocko,
                        blockl, blockr;				// Block links rechts über und unter dem Gegner

    GegnerClass	 (void);						// Konstruktor
    ~GegnerClass	 (void);						// Destruktor
    virtual void GegnerExplode(void);						// Gegner explodiert
    virtual void DoKI(void) {}								// Gegner individuell bewegen
    bool DoGegner	 (void);						// Gegner auf Grenzen checken und malen
    int  PlayerAbstand(void);						// Abstand Gegner/Spieler zurückliefern
    int  PlayerAbstandHoriz(void);					// Nur Horizontaler Abstand
    int  PlayerAbstandVert (void);					// Nur Vertikaler   Abstand
    void PlattformTest	   (RECT rect);				// Steht der Spieler auf dem Gegner ?
    void SimpleAnimation   (void);					// Einfache Animation (loop)
    void TurnonWall		   (void);					// An der Wand umdrehen?
    void TurnonShot		   (void);					// Umdrehen, wenn angeschoßen
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
                    bool Light);
    void DelSel		(GegnerClass *pTemp);			// Ausgewählten Gegner entfernen
    void ClearAll	(void);							// Alle Gegner löschen
    int  GetNumGegner(void);						// Zahl der Gegner zurückliefern
    void DoAllGegner  (void);						// Alle Gegner der Liste animieren
    void ShowAllGegner(void);						// Alle Gegner der Liste anzeigen
    void DestroyEnemiesonScreen(float x, float y);	// Alle Gegner ausgehend von Stelle x/y verwunden (Granate)
};

#endif
