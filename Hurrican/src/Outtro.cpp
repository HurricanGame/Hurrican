// Datei : Outtro.cpp

// -------------------------------------------------------------------------------------- 
//
// Klasse für das Hurrican Outtro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include "Console.h"
#include "Outtro.h"
#include "FMOD.h"
#include "DX8Sound.h"
#include "Main.h"
#include "Menu.h"
#include "gameplay.h"
#include "PartikelSystem.h"
#include "Projectiles.h"
#include "TileEngine.h"

// --------------------------------------------------------------------------------------
// Konstruktor, lädt die Grafiken
// --------------------------------------------------------------------------------------

OuttroClass::OuttroClass(void)
{
	Background.LoadImage("endscreen_background.bmp", 640, 480, 640, 480, 1, 1);
	Tower.LoadImage("endscreen_turm.png", 199, 315, 199, 315, 1, 1);
	Henry.LoadImage("endscreen_henry.png", 237, 88, 237, 88, 1, 1);
	Reiter[0].LoadImage("endritt.png", 760, 520, 190, 260, 4, 2);
	Reiter[1].LoadImage("endritt.png", 760, 520, 190, 260, 4, 2);
	ConvertPlayerTexture(&Reiter[1]);

	Zustand		 = OUTTRO_FADEIN;

	Counter = 0.0f;
	SmokeDelay = 0.0f;
	TowerOffset = 0.0f;
	finished = false;
	PlayerSmoke = 0.0f;
	Snow = 0.0f;

	pPartikelSystem->ClearAll();
	pSoundManager->LoadSong("outtro.it", MUSIC_OUTTRO);
	pSoundManager->StopAllSongs(false);
	FMUSIC_StopAllSongs();	
	pTileEngine->ClearLevel();

	// Ein paar Schneepartikel adden
	for (int i = 0; i < 100; i++)
		pPartikelSystem->PushPartikel((float)(rand()%640), (float)(rand()%480), SCHNEEFLOCKE_END);
}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

OuttroClass::~OuttroClass(void)
{
	FMUSIC_StopAllSongs();	
}

// --------------------------------------------------------------------------------------
// Wete für die beiden Spieler setzen
// --------------------------------------------------------------------------------------

void OuttroClass::InitPlayerPos(void)
{
	px[0] = 330.0f;
	py[0] = 240.0f;
	xs[0] = 10.0f;
	ys[0] = 2.0f;

	px[1] = 340.0f;
	py[1] = 260.0f;
	xs[1] = 12.0f;
	ys[1] = -3.0f;
}

// --------------------------------------------------------------------------------------
// Outtro ablaufen lassen
// --------------------------------------------------------------------------------------

void OuttroClass::DoOuttro(void)
{
	// Hintergrund rendern
	DirectGraphics.SetColorKeyMode();
	DirectGraphics.SetFilterMode(false);
	Background.RenderSprite(0, 0, 0, 0xFFFFFFFF);

	// Turm rendern
	float off = 0.0f;

	if (Zustand != OUTTRO_SCROLLER)
		off = (float)(sin(Counter / 2.0f)) / 2.0f;

	Tower.RenderSpriteRotated(299 + off, 
							  61 + TowerOffset, 
							  TowerOffset / 10.0f, 0xFFFFFFFF);
	
	// Partikel 
	pPartikelSystem->DoPartikelSpecial(false);

	// Den Henry, die alte Maske, rendern
	DirectGraphics.SetFilterMode(false);
	Henry.RenderSprite(276, 291, 0xFFFFFFFF);

	// Und nochmal den unteren Teil vom Background drüber
	Background.SetRect(0, 360, 640, 480);
	Background.RenderSprite(0, 360, 0xFFFFFFFF);

	// Auspuff Partikel =)
	pPartikelSystem->DoPartikelSpecial(true);

	// Spieler, der rausfliegt
	if (Zustand == OUTTRO_PLAYER_FLEES)
	{
		DirectGraphics.SetFilterMode(true);

		D3DCOLOR col1;

		if (Counter < 12.8f)
			col1 = D3DCOLOR_RGBA(255, 255, 255, (int)(Counter * 20.0f));
		else
			col1 = 0xFFFFFFFF;

		int a;
			
		// Spieler 2
		a = (int)((Counter - 15.0f) / 10);
		if (a > 8) a = 7;		

		if (NUMPLAYERS == 2)
		if (a >= 0)
		{
			Reiter[1].itsRect = Reiter[1].itsPreCalcedRects[a];		

			Reiter[1].itsRect = Reiter[1].itsPreCalcedRects[a];
			Reiter[1].RenderSpriteScaledRotated(px[1] - (190 / 150.0f * Counter / 2.0f), 
											 py[1] - (260 / 150.0f * Counter / 2.0f),
											 190 / 180.0f * Counter,
											 260 / 180.0f * Counter,
											 0, col1);

			// Schatten
			Reiter[1].itsRect.bottom = Reiter[1].itsPreCalcedRects[a].top;
			Reiter[1].itsRect.top    = Reiter[1].itsPreCalcedRects[a].bottom;
			Reiter[1].RenderSpriteScaledRotated(px[1] - (190 / 150.0f * Counter / 2.0f), 
											 230.0f + py[1] / 2.0f + Counter * 2.0f,
											 190 / 180.0f * Counter,
											 260 / 180.0f * Counter / 3.0f,
											 0, 0x88000000);

		}

		// Spieler 1
		a = (int)(Counter / 10);		
		Reiter[0].itsRect = Reiter[0].itsPreCalcedRects[a];
		
		Reiter[0].RenderSpriteScaledRotated(px[0] - (190 / 100.0f * Counter / 2.0f), 
										 py[0] - (260 / 100.0f * Counter / 2.0f),
										 190 / 100.0f * Counter,
										 260 / 100.0f * Counter,
										 0, col1);

		// Schatten
		Reiter[0].itsRect.bottom = Reiter[0].itsPreCalcedRects[a].top;
		Reiter[0].itsRect.top    = Reiter[0].itsPreCalcedRects[a].bottom;
		Reiter[0].RenderSpriteScaledRotated(px[0] - (190 / 100.0f * Counter / 2.0f), 
										 230.0f + py[0] / 2.0f + Counter * 2.0f,
										 190 / 180.0f * Counter,
										 260 / 180.0f * Counter / 3.0f,
										 0, 0x88000000);

		DirectGraphics.SetFilterMode(false);
	}

	if (pConsole->Showing)
		return;

	// Schnee
	Snow -= 1.0f SYNC;

	if (Snow <= 0.0f)
	{
		Snow = 1.0f;

		pPartikelSystem->PushPartikel((float)(rand()%640), -8, SCHNEEFLOCKE_END);
	}

	switch(Zustand)
	{
		case OUTTRO_FADEIN :						// Text scrollen
		{
			if (pSoundManager->its_Sounds[SOUND_TAKEOFF]->isPlaying == false)
			{
				pSoundManager->PlayWave(100, 128, 5000, SOUND_TAKEOFF);
				pSoundManager->PlayWave(50, 128, 8000, SOUND_WIND);
			}

			Counter += 10.0f SYNC;

			if (Counter > 255.0f)
			{
				Counter = 255.0f;
				Zustand = OUTTRO_TOWER_EXPLODE;				
			}

			D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255-int(Counter));
			RenderRect(0, 0, 640, 480, col);

			SmokeDelay -= 1.0f SYNC;

			// Explosionen und Qualm
			if (SmokeDelay < 0.0f)
			{
				SmokeDelay = 1.5f;
				pPartikelSystem->PushPartikel((float)(280 + rand()%180), 
											  (float)(320 + rand()%10), SMOKEBIG_OUTTRO);

				pPartikelSystem->PushPartikel((float)(280 + rand()%180), 
											  (float)(150 + TowerOffset + rand()%100), EXPLOSION_MEDIUM2);
			}
			
		} break;

		// Explosionen
		case OUTTRO_TOWER_EXPLODE:
		case OUTTRO_PLAYER_FLEES:
		{
			SmokeDelay -= 1.0f SYNC;
			TowerOffset += 0.5f SYNC;

			// Explosionen und Qualm
			if (SmokeDelay < 0.0f)
			{
				SmokeDelay = 0.5f;

				pPartikelSystem->PushPartikel((float)(280 + rand()%180), 
											  (float)(320 + rand()%10), SMOKEBIG_OUTTRO);

				pPartikelSystem->PushPartikel((float)(280 + rand()%180), 
											  (float)(150 + TowerOffset + rand()%100), EXPLOSION_MEDIUM2);								
			}

			if (Zustand == OUTTRO_TOWER_EXPLODE)
			{
				if (TowerOffset >= 20.0f)
				{
					Counter = 0.0f;
					Zustand = OUTTRO_PLAYER_FLEES;
					pSoundManager->PlayWave(100, 128, 10000, SOUND_TAKEOFF);
					InitPlayerPos();
				}
			}

			if (Zustand == OUTTRO_PLAYER_FLEES)
			{				
				PlayerSmoke -= 1.0f SYNC;

				if (PlayerSmoke < 0.0f)
				{
					WinkelUebergabe = (Counter) / 2.0f;
					if (WinkelUebergabe >= 1.0f)
					{
						pPartikelSystem->PushPartikel(px[0] + Counter / 2.0f, py[0] + Counter / 1.5f, EXPLOSION_TRACE_END);
						pPartikelSystem->PushPartikel(px[0] - Counter / 2.0f, py[0] + Counter / 1.5f, EXPLOSION_TRACE_END);
					}

					if (NUMPLAYERS == 2)
					{
						WinkelUebergabe = (Counter - 5.0f) / 2.0f;
						if (WinkelUebergabe >= 1.0f)
						{
							pPartikelSystem->PushPartikel(px[1], py[1] + Counter / 5.0f, EXPLOSION_TRACE_END);
							pPartikelSystem->PushPartikel(px[1] - Counter / 2.0f, py[1] + Counter / 5.0f, EXPLOSION_TRACE_END);
						}
					}

					PlayerSmoke = 0.2f;
				}

				if (Counter < 40.0f)
				{
					xs[0] -= 0.5f SYNC;
					xs[1] -= 0.4f SYNC;
				}
				else
				if (Counter < 50.0f)
				{
					xs[0] -= 2.0f SYNC;
					ys[0] -= 1.0f SYNC;

					xs[1] -= 1.5f SYNC;
					ys[1] += 1.0f SYNC;
				}
				else
				{
					xs[0] -= 4.0f SYNC;
					ys[0] -= 2.0f SYNC;

					xs[1] -= 3.0f SYNC;
					ys[1] += 2.0f SYNC;
				}

				px[0] += xs[0] SYNC;
				py[0] += ys[0] SYNC;
				px[1] += xs[1] SYNC;
				py[1] += ys[1] SYNC;

				if (Counter < 20.0f)
					Counter += 0.6f SYNC;
				else
					Counter += 1.2f SYNC;

				// Spieler sind weg? Dann nur noch scroller anzeigen und fertig =)
				if (Counter > 80.0f)
				{
					Zustand = OUTTRO_SCROLLER;
					TextOff = 0;
					Counter = 0.0f;

					pSoundManager->StopWave(SOUND_WIND);
					pSoundManager->PlayWave(100, 128, 6000, SOUND_DOORSTOP);
					pSoundManager->PlayWave(100, 128, 8000, SOUND_EXPLOSION2);
					pSoundManager->PlaySong(MUSIC_OUTTRO, false);
				}
			}			
		} break;

		case OUTTRO_SCROLLER:
		{
			char text[255];

			// Explosionen und Qualm
			SmokeDelay -= 1.0f SYNC;
			if (SmokeDelay < 0.0f)
			{
				SmokeDelay = (float)(rand()%10) + 5.0f;

				for (int i = 0; i < 10; i++)
					pPartikelSystem->PushPartikel((float)(280 + rand()%180), 
												  (float)(340 + rand()%10), SMOKEBIG_OUTTRO);

				pPartikelSystem->PushPartikel((float)(280 + rand()%180), 
											  (float)(150 + TowerOffset + rand()%100), EXPLOSION_MEDIUM2);								
			}

			Counter += 1.5f SYNC;

			while (Counter >= 20.0f)
			{
				Counter -= 20.0f;
				TextOff++;
				
				if (TEXT_OUTTRO1 + TextOff > TEXT_SEPERATOR_MARIO + CreditsCount - 30)
					TextOff = 0;
			}

			for (int i = 0; i < 30; i++)
			{
				int AnzahlCredits = CreditsCount - 65;
				int off = TextOff + i - 27;
				if (off > 0)
				{
					if (TEXT_OUTTRO1 + off < TEXT_SEPERATOR_MARIO)
						sprintf_s(text, "%s", TextArray[TEXT_OUTTRO1 + off]); 
					else
					if (TEXT_OUTTRO1 + off <= TEXT_SEPERATOR_MARIO + AnzahlCredits)
						sprintf_s(text, "%s", Credits[off + 25]); 
					else
					if (TEXT_OUTTRO1 + off <= TEXT_SEPERATOR_MARIO + AnzahlCredits + 10)
						sprintf_s(text, "%s", TextArray[TEXT_OUTTRO1 + off - AnzahlCredits]); 
					else
						sprintf_s(text, "");

					// rendern
					if (strcmp(text, "") != 0)
						pDefaultFont->DrawTextCenterAlign(320, i * 20.0f - Counter, text, 0xFFEEFFFF, 0);
				}
			}
		}
		break;

		default : break;

	} // switch	
}