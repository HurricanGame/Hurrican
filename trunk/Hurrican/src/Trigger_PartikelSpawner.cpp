// --------------------------------------------------------------------------------------
// Der Partikel Spawner
//
// Spawnt Partikel mit Abstand Value2
// Value1 gibt die Partikel Art an, wobei
// 0 = Regen, 1 = Schnee, 2 = Rauch, 3 = Funken, 4 = Luftblasen
// 5 = Aufsteigender Rauch, 6 = Aufsteigender Rauch, der ab und zu aussetzt
// 7 = Spritzer für den Wasserfall, 8 = Wassertropfen von der Decke
// 9 = Blitz und Donner, 10 = Scrollspeed
// 11 = Fog, 12 = Funken ohne "bzzzzzt" Sound =)
// 13 = Licht flackert (wie 9, nur in schwarz und mit Sound)
// 14 = Spinnenteile
// 15 = Blätter
// 16 = Staub
// 17 = Maden
// 18 = Lavafunken
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_PartikelSpawner.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPartikelSpawner::GegnerPartikelSpawner(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_STEHEN;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= false;
	DontMove		= true;
	Energy			= 100;
	Aussetzer		= 0.0f;			// für aussetzenden, auftreibenden Rauch
	AnimCount		= float (Value2);
	Active			= true;
	pAim			= pPlayer[0];

	// Tropfen per Zufall anfangen
	//
	if (Value1 == 8)
		AnimCount = float (rand()%Value2);

	if (Value1 == 18)
		AnimCount = 3.0f;

	if (Value1 == 11 ||
		Value1 == 19)
		AnimCount = 0.5f;

	// gleich aktivieren, damit er schon Partikel spuckt, bevor er im Bild ist ;)
	//
	Active			= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPartikelSpawner::DoKI(void)
{
	// Smoke Emitter geben Auftrieb beim Springen bei geradem Rauch nach oben (Value2 == 0)
	if (Value1 == 19 ||
	  ((Value1 == 5  ||		 
	   ( Value1 == 6 && Aussetzer > 6.0f)) && Value2 == 0))
	{
		for (int p = 0; p < NUMPLAYERS; p++)
		if (pPlayer[p]->yspeed != 0.0f &&
			pPlayer[p]->xpos + pPlayer[p]->CollideRect.left < xPos + 40  &&
			xPos < pPlayer[p]->xpos + pPlayer[p]->CollideRect.right   &&
			pPlayer[p]->ypos + pPlayer[p]->CollideRect.top  < yPos + 40	&&
			yPos - 300 < pPlayer[p]->ypos + pPlayer[p]->CollideRect.bottom)
		{
			if (Value1 == 19)
				pPlayer[p]->yspeed -= Value2 SYNC;
			else
				pPlayer[p]->yspeed -= 6.0f SYNC;
		}
	}

	Aussetzer += 0.2f SYNC;
	if (Aussetzer > 10.0f)
		Aussetzer = 0.0f;

	// Wird ein neuer Partikel gespwant ?
	AnimCount -= 1.0f SYNC;
	if (AnimCount < 0)
	{
		AnimCount = float (Value2);

		if (Value1 == 18) AnimCount = 3.0f;
		if (Value1 == 11) AnimCount = 2.0f;

		// Partikel, die auch gespawnt werden, wenn sie nicht im Bild sind (ab bestimmtem Abstand zum Spieler)
		//
		//float pa;
		if (PlayerAbstand() < 1000)
		{
			// Richtigen Partikel Spawnen
			switch (Value1)
			{
				// Regen
				case 0 :
				{
					// Wäre der Partikel überhaupt im Screen?
					float sx, sy;

					sx = xPos-320 + rand()%640;
					sy = (float)(pTileEngine->YOffset) - 16 - rand ()%64;

					if (sx + 20 < pTileEngine->XOffset ||
						sx - 180 > pTileEngine->XOffset + 640.0f)
						break;

					pPartikelSystem->PushPartikel(sx , sy, REGENTROPFEN);
					AnimCount = Value2 / 4.0f;
					if (AnimCount = 0.0f)
						AnimCount = 1.0f;
				} break;

				// Schneeflocke
				case 1 :
				{
					pPartikelSystem->PushPartikel(xPos-320 + rand()%640 , (float)(pTileEngine->YOffset) - 16 - rand ()%64, SCHNEEFLOCKE);
					AnimCount = 0.5f;
				} break;

				// Blubberblasen
				case 4 :
				{
					if (PlayerAbstand() < 800)
					pPartikelSystem->PushPartikel(xPos + 12 , yPos + 26, BUBBLE);
				} break;

				// Aufsteigender Rauch
				case 5 :
				{
					pPartikelSystem->PushPartikel(xPos + 8 + rand ()%10 , yPos + 24 + rand ()%10, SMOKE3 + Value2);
					AnimCount = 0.25f;
				} break;

				// Aufsteigender Rauch, der ab und zu aussetzt
				case 6 :
				{
					if (Aussetzer > 4.0f)
						pPartikelSystem->PushPartikel(xPos + 8 + rand ()%10 , yPos + 24 + rand ()%10, SMOKE3 + Value2);

					AnimCount = 0.25f;
				} break;

				// Wasserdampf vom Wasserfall
				case 7 :
				{
					for (int i = 0; i < 3; i++)
						pPartikelSystem->PushPartikel(xPos - 26 + rand ()%50 , yPos - 26 + rand ()%40, WATERFLUSH);
					
				} break; // Wasserdampf

				// Wassertropfen von der Decke
				case 8 :
				{
					float xdrop = xPos + rand()%40;

					if (xdrop < pTileEngine->XOffset ||
						xdrop > pTileEngine->XOffset + 640.0f ||
						yPos  < pTileEngine->YOffset - 240.0f ||
						yPos  > pTileEngine->YOffset + 480.0f)
					{
					}
					else
						pPartikelSystem->PushPartikel(xdrop, yPos, SPAWNDROP, pPlayer[0]);

					AnimCount -= rand()%5;
				} break;

				// Blitz (und Donner)
				case 9 :
				{
					pPartikelSystem->ThunderAlpha = 128; 
					pPartikelSystem->ThunderColor [0] = 228;
					pPartikelSystem->ThunderColor [1] = 242;
					pPartikelSystem->ThunderColor [2] = 255;
					pSoundManager->PlayWave (20 + rand()%60, 128, 8000 + rand()%4000, SOUND_THUNDER);

					Value2 = rand()%80 + 10;
				} break;

				// Nebel
				case 11 :
				{
					pPartikelSystem->PushPartikel(xPos - Value2 + rand()%(Value2 * 2), yPos - 20 + rand()%40, FOG);
				} break;

				// Licht flackert (wie blitz, nur in schwarz)
				case 13 :
				{
					pPartikelSystem->ThunderAlpha = 160;
					pPartikelSystem->ThunderColor [0] = 0;
					pPartikelSystem->ThunderColor [1] = 0;
					pPartikelSystem->ThunderColor [2] = 0;
					pSoundManager->PlayWave (80, 128, 11025 + rand()%2000, SOUND_FUNKE + rand ()%4);

					Value2 = rand()%20 + 10;
				} break;	

				// Spinnenteil
				case 14 :
				{
					pPartikelSystem->PushPartikel(xPos + 4, yPos + 4, SPIDERPARTS);

					AnimCount = (float)(rand()%5 + 10);
				} break;

				// Blätter
				case 15:
				{
					WinkelUebergabe = 0.0f;

					if (Value2 == 0)
					{
						pPartikelSystem->PushPartikel((float)(pTileEngine->XOffset + 640.0f), 
													  (float)(yPos - 240.0f + rand()%480), BLATT2);
						AnimCount = (rand()%5 + 1) / 5.0f;
					}
					else
					{
						pPartikelSystem->PushPartikel((float)(pTileEngine->XOffset + 640.0f), 
													  (float)(yPos - 240.0f + rand()%480), DUST);
						AnimCount = (rand()%5 + 1) / 2.0f;
					}
					
				} break;

				// Staub
				case 16:
				{					
					pPartikelSystem->PushPartikel((float)(xPos + rand()%200 - 100),
												  (float)(yPos + rand()%200 - 100), DUST);

					AnimCount = (rand()%5 + 1) / 5.0f;
				} break;	
				
				case 18:
				{				
					AnimCount = 3.0f;
					pPartikelSystem->PushPartikel(xPos - (Value2) + rand()%(Value2 * 2),
												  yPos, LAVADUST);	
				} break;

				// Aufsteigender Rauch mit Auftrieb (Value2)
				case 19:
				{
					if (rand()%3 == 0)
						pPartikelSystem->PushPartikel(xPos - 10, yPos - 20, SMOKEBIG);
					else
						pPartikelSystem->PushPartikel(xPos + 8 + rand ()%10 , yPos + 24 + rand ()%10, SMOKE3);

					AnimCount = 0.25f;
				}
			} // switch
		}

		// Partikel, die nur gespwant werden, wenn der Trigger im Bild ist
		// Ist der Trigger im Bild ?
		if (xPos + 40 > pTileEngine->XOffset	   &&
			xPos	  < pTileEngine->XOffset + 640 &&
			yPos + 40 > pTileEngine->YOffset	   &&
			yPos	  < pTileEngine->YOffset + 480)
		{
			// Richtigen Partikel Spawnen
			switch (Value1)
			{
				// Rauch
				case 2 :
				{
					pPartikelSystem->PushPartikel(xPos + rand()%5, yPos+24, SMOKE);
				} break;

				// Funken
				case 3 :
				case 12 :
				{
					if (rand()%4 == 0)
					{
						// mit Sound?
						if (Value1 == 3)					
							pSoundManager->PlayWave3D(int(xPos), int(yPos), 11025 + rand()%1000, SOUND_FUNKE);

						pPartikelSystem->PushPartikel(xPos, yPos-24, LASERFLAME);

						for (int i=0; i<10; i++)
						{
							pPartikelSystem->PushPartikel(xPos+19, yPos, FUNKE);
							pPartikelSystem->PushPartikel(xPos+19, yPos, LASERFUNKE2);
						}
					}
				} break;

				// Scrollspeed setzen und danach Trigger entfernen
				case 10 :
				{
					pPlayer[0]->AutoScrollspeed = float (Value2);
					Energy = 0.0f;
				} break;
				
				// Maden
				case 17:
				{	
					pGegner->PushGegner((float)(xPos + rand()%20 - 10),
										(float)(yPos + 16), MADE, 0, 0, false, false);
				} break;
			} // switch
		}
	}
}

// --------------------------------------------------------------------------------------
// PartikelSpawner explodiert (nicht *g*)
// --------------------------------------------------------------------------------------

void GegnerPartikelSpawner::GegnerExplode(void)
{
}
