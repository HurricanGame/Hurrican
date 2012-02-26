// Datei : Intro.cpp

// -------------------------------------------------------------------------------------- 
//
// Klasse für das Hurrican Intro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include "Intro.h"
#include "FMOD.h"
#include "DX8Sound.h"
#include "gameplay.h"

// --------------------------------------------------------------------------------------
// Konstruktor, lädt die Grafiken
// --------------------------------------------------------------------------------------

IntroClass::IntroClass(void)
{
	Background[0].LoadImage("intro1.bmp", 640, 480, 640, 480, 1, 1);
	Background[1].LoadImage("intro2.bmp", 640, 480, 640, 480, 1, 1);
	Background[2].LoadImage("intro3.bmp", 640, 480, 640, 480, 1, 1);
	Background[3].LoadImage("intro4.bmp", 640, 480, 640, 480, 1, 1);
	Background[4].LoadImage("intro5.bmp", 640, 480, 640, 480, 1, 1);
	Background[5].LoadImage("intro6.bmp", 640, 480, 640, 480, 1, 1);

	Zustand = INTRO_FADEIN;	

	TextOff = 0;
	BildNr = 0;
	Counter = 0.0f;
	pSoundManager->LoadSong("intro.it", MUSIC_INTRO);

	while (DirectInput.AreAllKeysReleased() == false)
		DirectInput.UpdateTastatur();

}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

IntroClass::~IntroClass(void)
{
	FMUSIC_StopAllSongs();	
}

// --------------------------------------------------------------------------------------
// Intro beenden
// --------------------------------------------------------------------------------------

void IntroClass::EndIntro(void)
{
	if (Zustand != INTRO_FADEOUT)
	{
		if (Zustand != INTRO_FADEIN)
			Counter = 255.0f;

		Zustand = INTRO_FADEOUT;		
		pSoundManager->FadeSong(MUSIC_INTRO, -1.5f, 0, false);
	}
}

// --------------------------------------------------------------------------------------
// Intro ablaufen lassen
// --------------------------------------------------------------------------------------

void IntroClass::DoIntro(void)
{
	// Hintergrund rendern
	DirectGraphics.SetColorKeyMode();

	int a = (TextOff - 1) / 4;
	if (a < 0) a = 0;
	if (a > 5) a = 5;

	Background[a].RenderSprite(0, 0, 0, 0xFFFFFFFF);

	// nächstes Bild überblenden?
	if ((TextOff - 1) % 4 == 3 &&
		a < 5)
	{
		D3DCOLOR fadecol = D3DCOLOR_RGBA(255, 255, 255, (int)(Counter / 1300.0f * 255.0f));
		Background[a+1].RenderSprite(0, 0, 0, fadecol);
	}

	// Balken unten
	RenderRect4(0, 360, 640, 60, 0x00000000, 0x00000000, 
								 0xFF000000, 0xFF000000);
	RenderRect(0, 420, 640, 60, 0xFF000000);

	// Intro laufen lassen
	switch(Zustand)
	{
		case INTRO_FADEIN :						// Text scrollen
		{
			// Mucke spielen
			if (pSoundManager->InitSuccessfull &&
				FMUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_INTRO]->SongData) == false)
				pSoundManager->PlaySong(MUSIC_INTRO, false);				

			// und einfaden
			Counter += 10.0f SYNC;

			if (Counter > 255.0f)
			{
				Counter = 0.0f;
				Zustand = INTRO_RUN;
			}
			else
			{
				D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255-int(Counter));
				RenderRect(0, 0, 640, 480, col);			
			}
		} break;

		case INTRO_FADEOUT :
		{
			// und ausfaden
			Counter -= 5.0f SYNC;

			if (Counter < 0.0f)
			{
				Counter = 0.0f;
				Zustand = INTRO_DONE;
			}

			D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255-int(Counter));
			RenderRect(0, 0, 640, 480, col);			
			
		} break;

		// Scroller
		case INTRO_RUN:
		{
			Counter += 25.5f SYNC;

			if (Counter > 1300.0f)
			{
				Counter = 0.0f;
				TextOff++;

				if (TextOff > 23)
				{
					EndIntro();
					break;
				}
			}
			
			// Text rendern	
			float xr = Counter;
			if (xr > 700.0f)
				xr = 700.0f;

			DirectGraphics.SetAdditiveMode();

			for (int t = 0; t <= 23; t++)
			{
				D3DCOLOR col;

				col = D3DCOLOR_RGBA(0, 255, 0, 255 - (TextOff - t) * 25);

				if (t <= TextOff &&
					t - TextOff > -10)
				{
					int off = TEXT_INTRO1 + t;

					if (off == TEXT_INTRO1 + 22 &&
						NUMPLAYERS == 2)
						off++;
					else
					if (off >= TEXT_INTRO1 + 23)
						off++;

					pDefaultFont->DrawText(10, (float)(465 + t * 12) - TextOff * 12, TextArray[off], col);

					// Teil des Textes verdecken
					if (t == TextOff)
					{
						DirectGraphics.SetColorKeyMode();
						RenderRect(xr, 464, 640.0f, 17, 0xFF000000);
					}
				}
			}	

			//grüne Rechtecke rendern
			if (TextOff <= 28)
			{
				DirectGraphics.SetAdditiveMode();
				{										
					int l = pDefaultFont->StringLength(TextArray[TEXT_INTRO1 + TextOff], 0) + 28;
					if (xr < l)
					{
						for (int i = 0; i < 5; i++)
						{
							D3DCOLOR col;

							switch(i)
							{
								case 0: col = D3DCOLOR_RGBA(255, 255, 255, 255); break;
								case 1: col = D3DCOLOR_RGBA(0, 255, 0, 255); break;
								case 2: col = D3DCOLOR_RGBA(0, 180, 0, 255); break;
								case 3: col = D3DCOLOR_RGBA(0, 90, 0, 255); break;
								case 4: col = D3DCOLOR_RGBA(0, 50, 0, 255); break;
							}

							RenderRect(xr - i * 12 - 12, 464, 12, 12, col);
						}
					}
					else
					{
						xr = (float)l;

						// blinken lassen
						if ((int)(Counter / 100.0f) % 2 == 0)
							RenderRect(xr - 12, 464, 12, 12, 0xFFFFFFFF);
					}
				}
			}
		}
		break;

		default : break;

	} // switch	
}