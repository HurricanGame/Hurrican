// Datei : Intro.h

// -------------------------------------------------------------------------------------- 
//
// Klasse f�r das Hurrican Intro
//
// (c) 2006 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __INTRO_H_
#define __INTRO_H_

// --------------------------------------------------------------------------------------
// Inlcudces
// --------------------------------------------------------------------------------------

#include "Globals.h"
#include "DX8Font.h"
#include "DX8Sprite.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Zust�nde

#define INTRO_FADEIN			0
#define INTRO_RUN				1
#define INTRO_FADEOUT			2
#define INTRO_DONE				3

// --------------------------------------------------------------------------------------
// Klassen Deklaration f�r das Intro
// --------------------------------------------------------------------------------------

class IntroClass
{
	private:
		DirectGraphicsSprite	Background[6];		
		int						BildNr;
		int						TextOff;
		float					Counter;

	public:
		int						Zustand;
			IntroClass(void);							// Konstruktor
		   ~IntroClass(void);							// Destruktor
	   void DoIntro   (void);							// Intro ablaufen lassen
	   void EndIntro  (void);

};

#endif