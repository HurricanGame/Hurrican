// Datei : CCutScene.h

// -------------------------------------------------------------------------------------- 
//
// Klasse f�r die CutScenes zwischen zwei Stages
//
// (c) 2004 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _CutScene_h_
#define _CutScene_h_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "DX8Input.h"
#include "DX8Font.h"
#include "DX8Sprite.h"
#include "Globals.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Funktionen f�r die CutScenes
// --------------------------------------------------------------------------------------

void PlayCutScene (int nr);

// --------------------------------------------------------------------------------------
// CutScene-Klasse
// --------------------------------------------------------------------------------------

class CCutScene
{
	protected:		
		float					m_fFadeAlpha;				// momentaner Alpha Wert f�rs Fading
		float					m_fYScroll;					// y Pos des Scrolltextes

	public:
		int						m_iFading;					// -1 = fade in, 1 = fadeout, 0 = no fade
		bool					m_bIsDone;					// CutScene beendet/abgebrochen

			     CCutScene	 (void);						// CutScene initialisieren
	virtual	    ~CCutScene	 (void);						// CutScene freigeben
	
	virtual void RunCutScene (void) {}						// CutSCene laufen lassen, bis sie zuende ist oder
															// abgebrochen wurde
}; // CCutScene



// --------------------------------------------------------------------------------------
// Die einzelnen CutScenes sind alles von der Hauptklasse "CCutScene" abgeleitete
// Unterklassen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// CutScene nach Stage 1
// --------------------------------------------------------------------------------------

class CCutScene1 : public CCutScene
{
	private:
		DirectGraphicsSprite *BackGround;

	public:
		 CCutScene1			(void);
		~CCutScene1			(void);

		void RunCutScene	(void);

}; // CCutScene1

#endif // _CutScene_h_