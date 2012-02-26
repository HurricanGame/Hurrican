// --------------------------------------------------------------------------------------
// Der Lava Golem
//
// wirft Steinbroken und lässt die Höhle schaukeln
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_Golem.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerGolem::GegnerGolem(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= LINKS;
	Energy			= 8000;
	Value1			= Wert1;
	Value2			= Wert2;
	AnimPhase		= 0;
	ChangeLight		= Light;
	Destroyable		= true;
	OwnDraw			= true;

	Wackel			= 0.0f;
	WackelDir		= 1.0f;
	yoff			= 0.0f;

	StoneCount		= 0;

	rotarm1 = 0.0f;
	rotarm2 = 0.0f;

	last = 0;

	arm[0].LoadImage("golemarm_hinten.png", 224, 241, 224, 241, 1, 1);
	arm[1].LoadImage("golemarm_vorne.png", 205, 266, 205, 266, 1, 1);	
}

// --------------------------------------------------------------------------------------
// Winkel zum Spieler
// --------------------------------------------------------------------------------------

float GegnerGolem::WinkelToPlayer(void)
{
	float xdiv, ydiv;
	float w;

	ydiv = (pAim->ypos + 40) - (yPos + yoff + 50);
	if (ydiv == 0.0f)
		ydiv  = 0.00001f;

	xdiv = (pAim->xpos + 35) - (xPos + 50);
	w = 90.0f + (float)atan(ydiv / xdiv) * 180.0f / PI;

	return w;
}

// --------------------------------------------------------------------------------------
// Golem beim laufen wackeln lassen
// --------------------------------------------------------------------------------------

void GegnerGolem::Wackeln(void)
{	
	// yoff bewegen
	if (Wackel > 0.0f)
		yoff = Wackel * 8.0f;
	else
		yoff = -Wackel * 8.0f;

	// grenze überschritten?
	if ((WackelDir > 0.0f && Wackel >  WACKELMAX) ||
		(WackelDir < 0.0f && Wackel < -WACKELMAX))
	{
		WackelDir *= -1.0f;
		Wackel = WACKELMAX * -WackelDir;

		if (Handlung == GEGNER_EINFLIEGEN ||
			Handlung == GEGNER_LAUFEN     ||
			Handlung == GEGNER_LAUFEN2    ||
			Handlung == GEGNER_LAUFEN3	  ||
			Handlung == GEGNER_LAUFEN4)
		{
			ShakeScreen(4.0f);
			pSoundManager->PlayWave(100, 128, 15000 + rand()%2000, SOUND_DOORSTOP);
		}
	} 
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerGolem::DoDraw(void)
{
	D3DCOLOR Color, Color2;
	float wackel = 0.0f;

	if (Handlung == GEGNER_EXPLODIEREN)
		wackel = (float)(int(ShotDelay) % 2);

	DirectGraphics.SetFilterMode (true);

	int	Wert = 255-(int(DamageTaken));
	Color = D3DCOLOR_RGBA(255, Wert, Wert, 255);

	Wert /= 2;
	Color2 = D3DCOLOR_RGBA(128, Wert, Wert, 255);
		
	// Rotationsmatrizen erstellen
	RECT itsRect;
	D3DXMATRIX	matRotBody, matRotArm, matTransBody, matTransArm, matTrans2Body, matTrans2Arm;

	// Körper	
	int offx = 0;
	int offy = 80;
	float x = (float)(xPos - pTileEngine->XOffset);
	float y = (float)(yPos - pTileEngine->YOffset);

	itsRect = GegnerRect[GegnerArt];
	D3DXMatrixRotationZ  (&matRotBody,  Wackel);	
	D3DXMatrixTranslation(&matTransBody,-x-(itsRect.right  - itsRect.left)/2.0f-offx,
									    -y-(itsRect.bottom - itsRect.top )/2.0f-offy, 0.0f);

	// Transformation wieder zurück
	D3DXMatrixTranslation(&matTrans2Body,x+(itsRect.right  - itsRect.left)/2.0f+offx,
										 y+(itsRect.bottom - itsRect.top )/2.0f+offy, 0.0f);

	D3DXMatrixRotationZ  (&matRotArm,  rotarm2 - 0.5f);	

	// arm bewegen
	offx = 0;
	offy = 80;
	x = (float)(xPos + ARMX - pTileEngine->XOffset);
	y = (float)(yPos + ARMY - pTileEngine->YOffset);
	D3DXMatrixTranslation(&matTransArm,-x-205, -y-10, 0.0f);

	// Transformation wieder zurück
	D3DXMatrixTranslation(&matTrans2Arm,x+120, y+80, 0.0f);

	D3DXMatrixIdentity	 (&matWorld);
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTransArm);		// rotieren (Körper)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRotArm);		// rotieren (Arm)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2Arm);		// und wieder zurück verschieben	
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTransBody);		// rotieren (Körper)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRotBody);		// rotieren (Arm)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2Body);		// und wieder zurück verschieben
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Arm hinten zeichnen
    arm[0].RenderSprite((float)(xPos - pTileEngine->XOffset) + wackel,
		  		        (float)(yPos - pTileEngine->YOffset) + yoff, Color);

    // Körper zeichnen
	D3DXMatrixIdentity	 (&matWorld);
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTransBody);		// rotieren (Körper)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRotBody);		// rotieren (Arm)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2Body);		// und wieder zurück verschieben
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);	

    pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos - pTileEngine->XOffset) + wackel,
										   (float)(yPos - pTileEngine->YOffset) + yoff, Color);

	// Arm
	offx = 0;
	offy = 0;
	x = (float)(xPos + ARMX - pTileEngine->XOffset);
	y = (float)(yPos + ARMY - pTileEngine->YOffset);

	itsRect.left = 0;
	itsRect.top = 0;
	itsRect.right = 193;
	itsRect.bottom = 179;

	D3DXMatrixRotationZ  (&matRotArm,  rotarm1 - 0.3f);	

	// arm bewegen
	D3DXMatrixTranslation(&matTransArm,-x-180, -y - 50, 0.0f);

	// Transformation wieder zurück
	D3DXMatrixTranslation(&matTrans2Arm,x+175, y+95, 0.0f);

	D3DXMatrixIdentity	 (&matWorld);
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTransArm);		// rotieren (Körper)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRotArm);		// rotieren (Arm)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2Arm);		// und wieder zurück verschieben	
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTransBody);		// rotieren (Körper)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matRotBody);		// rotieren (Arm)
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2Body);		// und wieder zurück verschieben
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // Arm zeichnen
    arm[1].RenderSprite((float)(xPos - pTileEngine->XOffset) + wackel,
		  		     (float)(yPos - pTileEngine->YOffset) + yoff, Color);	

	// Normale Projektions-Matrix wieder herstellen
	D3DXMatrixRotationZ (&matWorld, 0.0f);
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);


	// Blockrect setzen
	GegnerRect[GOLEM].left = 42; GegnerRect[GOLEM].right  = 138 + (int)yoff;
	GegnerRect[GOLEM].top  = 29; GegnerRect[GOLEM].bottom = 300;	

	DirectGraphics.SetFilterMode (false);
	DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Wert gegen Null gehen lassen mit speed
// --------------------------------------------------------------------------------------
bool GegnerGolem::GoToZero(float &Value, float speed)
{
	bool fertig = true;

	if (Value > 0.0f)
	{
		fertig = false;
		Value -= speed SYNC;
		if (Value < 0.0f)
			Value = 0.0f;
	}

	if (Value < 0.0f)
	{
		fertig = false;
		Value += speed SYNC;
		if (Value > 0.0f)
			Value = 0.0f;
	}

	return fertig;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerGolem::DoKI(void)
{
	// Energie anzeigen
	if (Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(8000, Energy);

	// Levelausschnitt auf den Golem zentrieren, sobald dieser sichtbar wird
	if (Active == true && pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		// verstecken
		xPos += 400;

		// hinscrollen
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);

		 // Ausfaden und pausieren
		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);
	}

	// Zwischenboss blinkt nicht so lange wie die restlichen Gegner
	if (DamageTaken > 0.0f)
		DamageTaken -= 100 SYNC;				// Rotwerden langsam ausfaden lassen
	else
		DamageTaken = 0.0f;						// oder ganz anhalten

	// Hat der Golem keine Energie mehr ? Dann explodiert er
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		Destroyable = false;
		Handlung  = GEGNER_EXPLODIEREN;
		xSpeed    = 0.0f;
		ySpeed    = 0.0f;
		xAcc      = 0.0f;
		yAcc      = 0.0f;		
		AnimCount = 1.0f;
		ShotDelay = 30.0f;
		
		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}	

// Je nach Handlung richtig verhalten
	switch (Handlung)
	{
		case GEGNER_NOTVISIBLE:			// Warten bis der Screen zentriert wurde
		{
			if (pTileEngine->Zustand == ZUSTAND_LOCKED)
			{
				// Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
				if (FMUSIC_IsPlaying(pSoundManager->its_Songs[MUSIC_BOSS]->SongData) == false)
					pSoundManager->PlaySong(MUSIC_BOSS, false);

				// Und Boss erscheinen lassen				
				Handlung = GEGNER_EINFLIEGEN;
				pSoundManager->PlayWave(100, 128, 7000, SOUND_MUTANT);
			}
		} break;

		// Gegner läuft am Anfang langsam nach links
		case GEGNER_EINFLIEGEN:
		{			
			Energy = 8000;
			DamageTaken = 0.0f;
			xPos -= float(10.0 SYNC);					// Faust nach unten bewegen
			Wackel += WackelDir * 0.025f SYNC;
			rotarm1 -= WackelDir * 0.2f SYNC;
			rotarm2 += WackelDir * 0.2f SYNC;
			Wackeln();
			if (xPos <= pTileEngine->ScrolltoX + 380)	// Weit genug oben ?
			{
				xPos = (float)(pTileEngine->ScrolltoX + 380);
				Handlung = GEGNER_SPECIAL;				
			}
		} break;

		// Gegner läuft am Anfang langsam nach links
		case GEGNER_LAUFEN:
		{			
			xPos -= float(10.0 SYNC);
			Wackel += WackelDir * 0.025f SYNC;
			rotarm1 -= WackelDir * 0.2f SYNC;
			rotarm2 += WackelDir * 0.2f SYNC;
			Wackeln();
			if (xPos <= pTileEngine->ScrolltoX + 380)	// Weit genug in Mitte  ?
			{
				xPos = (float)(pTileEngine->ScrolltoX + 380);
				Handlung = GEGNER_SPECIAL;				
			}
		} break;

		// Gegner läuft nach rechts
		case GEGNER_LAUFEN2:
		{
			xPos += float(10.0 SYNC);
			Wackel += WackelDir * 0.025f SYNC;
			rotarm1 -= WackelDir * 0.2f SYNC;
			rotarm2 += WackelDir * 0.2f SYNC;
			Wackeln();
			if (xPos >= pTileEngine->ScrolltoX + 540)	// Weit genug rechts ?
			{
				// zurücklaufen?
				xPos = (float)(pTileEngine->ScrolltoX + 540);
				Handlung = GEGNER_SPECIAL;				
			}
		} break;

		// Gegner läuft ganz nach links zur mauer
		case GEGNER_LAUFEN3:
		{
			xPos -= float(10.0 SYNC);
			Wackel += WackelDir * 0.025f SYNC;
			rotarm1 -= WackelDir * 0.2f SYNC;
			rotarm2 += WackelDir * 0.2f SYNC;
			Wackeln();
			if (xPos <= pTileEngine->ScrolltoX + 100)	// Weit genug links ?
			{
				xPos = (float)(pTileEngine->ScrolltoX + 100);
				Handlung = GEGNER_SPECIAL2;
				last = GEGNER_SPECIAL2;
				state2 = ARM_SENKEN;
				StoneCount = 3 + rand()%2;
			}
		} break;

		// von links zur mitte laufen
		case GEGNER_LAUFEN4:
		{
			xPos += float(10.0 SYNC);
			Wackel += WackelDir * 0.025f SYNC;
			rotarm1 -= WackelDir * 0.2f SYNC;
			rotarm2 += WackelDir * 0.2f SYNC;
			Wackeln();
			if (xPos >= pTileEngine->ScrolltoX + 380)	// Weit genug in Mitte  ?
			{
				xPos = (float)(pTileEngine->ScrolltoX + 380);
				Handlung = GEGNER_SPECIAL;				
			}
		} break;

		// gegen die wand bollern und lavabälle spawnen
		case GEGNER_SPECIAL2 :
		{
			switch (state2)
			{
				case ARM_SENKEN:
				{
					if (Wackel > -0.2f)
						Wackel -= 0.03f SYNC;
					else
						Wackel = -0.2f;

					rotarm1 -= 0.25f SYNC;
					rotarm2 += 0.25f SYNC;

					if (rotarm2 > 1.0f)
					{
						state2 = ARM_HEBEN;
						ShakeScreen(3.0f);
						pSoundManager->PlayWave(75, 128, 15000 + rand()%2000, SOUND_DOORSTOP);

						pGegner->PushGegner((float)(pTileEngine->XOffset + 100.0f + rand()%540),
											(float)(pTileEngine->YOffset + 480), LAVABALL, 50 + rand()%10, 0, false);
					}
				}
				break;

				case ARM_HEBEN:
				{
					if (Wackel > -0.2f)
						Wackel -= 0.03f SYNC;
					else
						Wackel = -0.2f;

					rotarm1 += 0.25f SYNC;
					rotarm2 -= 0.25f SYNC;

					if (rotarm1 > 1.0f)
					{
						state2 = ARM_SENKEN;
						ShakeScreen(3.0f);
						pSoundManager->PlayWave(75, 128, 15000 + rand()%2000, SOUND_DOORSTOP);

						pGegner->PushGegner((float)(pAim->xpos - 20.0f + rand()%70),
											(float)(pTileEngine->YOffset + 480), LAVABALL, 50 + rand()%10, 0, false);

						StoneCount--;

						if (StoneCount <= 0)
							Handlung = GEGNER_SPECIAL;
					}
				}
				break;
			}
		} break;

		// Ausgangsposition für Aktionen herstellen
		case GEGNER_SPECIAL:		
		{	
			bool fertig[3] = {true, true, true};

			// zuende wackeln
			Wackeln();

			fertig[0] = GoToZero(Wackel);
			fertig[1] = GoToZero(rotarm1);
			fertig[2] = GoToZero(rotarm2);
			
			if (fertig[0] && 
				fertig[1] && 
				fertig[2])
			{
				int j = 0;

				// Golem steht gerade rechts? 
				if (xPos > pTileEngine->ScrolltoX + 400)
				{		
					// Dann Steine werfen
					if (rand()%2 == 0)
					{
						Handlung = GEGNER_CRUSHEN;
						last = GEGNER_CRUSHEN;
						state2 = ARM_SENKEN;
						StoneCount = rand()%2 + 2;
					}

					// oder auf die lava dreschen
					else
					{
						Handlung = GEGNER_BOMBARDIEREN;
						state2 = ARM_HEBEN;
						count = float (rand()%3 + 2);
					}
				}

				// Golem steht gerade in der Mitte?
				else				
				if (xPos > pTileEngine->ScrolltoX + 120)
				{	
					// Weiterlaufen?
					if (rand()%3 > 0)
					{
						// Die Aktion ausführen, die eben nicht dran war					
						if (last == GEGNER_SPECIAL2)
							Handlung = GEGNER_LAUFEN2;
						else
							Handlung = GEGNER_LAUFEN3;
					}

					// oder Suschuss schiessen
					else
					{
						Handlung = GEGNER_AUSSPUCKEN;
						ShotDelay = 80.0f;
						count = 4.0f + rand()%4;
						Wackel = 0.0f;
					}										
				}

				// Golem steht schon links? Dann zurück zur Mitte
				else
				if (xPos <= pTileEngine->ScrolltoX + 120)
				{
					Handlung = GEGNER_LAUFEN4;
				}
			}

			
		} break;

		// Grüne Rotzbobel schiessen
		case GEGNER_AUSSPUCKEN:
		{
			if (ShotDelay > 10.0f)
			{
				Wackel += 0.03f SYNC;
				rotarm1 -= 0.054f SYNC;
				rotarm2 -= 0.054f SYNC;
			}
			else
			{
				Wackel -= 0.2f SYNC;
				rotarm1 += 0.4f SYNC;
				rotarm2 += 0.4f SYNC;
			}

			ShotDelay -= 10.0f SYNC;

			if (ShotDelay < 0.0f)
			{	
				// Suchschuss erzeugen
				WinkelUebergabe = WinkelToPlayer() + 180.0f;
				pProjectiles->PushProjectile(xPos + 35.0f, yPos + yoff + 35.0f, FIREBALL_BIG, pAim);

				pPartikelSystem->PushPartikel(xPos - 60.0f, yPos + yoff - 70.0f, GRENADEFLARE);

				// Sound ausgeben
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_FIREBALL);
				
				ShotDelay = 80.0f;

				// Schusszahl verringern
				count -= 1.0f;

				// kein schuss mehr? dann neue aktion
				if (count <= 0.0f)
				{					
					Handlung = GEGNER_WARTEN;
					ShotDelay = 10.0f;
				}
				
				Wackel = 0.0f;
				rotarm1 = 0.0f;
				rotarm2 = 0.0f;				
			}
		} break;

		// nach dem Schiessen kurz warten bis zur nächsten Aktion
		case GEGNER_WARTEN:
		{
			ShotDelay -= 1.0f SYNC;

			if (ShotDelay <= 0.0f)
				Handlung = GEGNER_SPECIAL;
		} break;

		// stein raufholen und werfen
		case GEGNER_CRUSHEN :
		{
			switch (state2)
			{
				// absenken und stein holen
				case ARM_SENKEN:
				{
					Wackel -= 0.05f SYNC;
					rotarm1 += 0.1f SYNC;
					yoff += 5.0f SYNC;

					if (Wackel < -0.5f)
						state2 = ARM_HEBEN;
				} break;

				// wieder aufrichten
				case ARM_HEBEN:
				{
					yoff -= 5.0f SYNC;
					Wackel += 0.05f SYNC;
					rotarm1 -= 0.1f SYNC;

					if (Wackel >= 0.0f)
					{
						state2 = ARM_UEBERKOPF;
						Wackel = 0.0f;
						rotarm1 = 0.0f;
						yoff = 0.0f;
					}
				} break;

				// Stein nach hinten nehmen
				case ARM_UEBERKOPF:
				{
					//yoff += 5.0f SYNC;
					Wackel += 0.04f SYNC;
					rotarm1 += 0.4f SYNC;

					if (Wackel >= 0.35f)
					{
						state2 = ARM_WERFEN;												
						pSoundManager->PlayWave(100, 128, 6000 + rand()%2000, SOUND_MUTANT);
					}
				} break;

				// Stein werfen
				case ARM_WERFEN:
				{					
					if (Wackel > 0.0f)
						Wackel -= 0.1f SYNC;
					else
						Wackel = 0.0f;

					rotarm1 -= 0.6f SYNC;

					if (rotarm1 <= 2.0f)
					{
						pGegner->PushGegner(xPos - 40, yPos + 30, BOULDER, -(rand()%80 + 10), -(rand()%20 + 10), true);
						pGegner->PushGegner(xPos - 40, yPos + 30, BOULDER, -(rand()%40 + 10), -(rand()%10 + 10), true);
						pSoundManager->PlayWave(100, 128, 14000 + rand()%4000, SOUND_STONEFALL);
						state2 = ARM_SENKEN2;
					}
				} break;

				// wieder in wurf ausgangsposition
				case ARM_SENKEN2:
				{
					bool fertig[3] = {true, true, true};

					// zuende wackeln
					Wackeln();

					fertig[0] = GoToZero(Wackel, 0.5f);
					fertig[1] = GoToZero(rotarm1, 0.5f);
					fertig[2] = GoToZero(rotarm2, 0.5f);
					
					if (fertig[0] && 
						fertig[1] && 
						fertig[2])
					{
						StoneCount--;

						if (StoneCount == 0)
							Handlung = GEGNER_LAUFEN;
						else
							state2 = ARM_SENKEN;
					}
				}
				break;
			}
		} break;


		// Feuer spucken
		case GEGNER_SCHIESSEN :
		{			
			count += 1.0f SYNC;

			// vorbeugen
			if (count < 10.0f)
			{
				Wackel += 1.0f SYNC;
			}

			// schiessen
			else
			{
				// einmalig Sound abspielen
				if (ShotDelay == 3.0f)
					pSoundManager->PlayWave (100, 128, 11025, SOUND_FEUERFALLE);

				Wackel = 10.0f;
				ShotDelay -= 1.0f SYNC;
			}

			if (ShotDelay <= 0.0f)
			{
				ShotDelay = 0.5f;
				pProjectiles->PushProjectile  (xPos - 10, yPos + 80, WALKERFIRE, pAim);
			}

			if (count > 30.0f)
				Handlung = GEGNER_SPECIAL;

		} break;

		// 
		case GEGNER_BOMBARDIEREN:
		{
			// arm heben
			switch (state2)
			{
				// schnell heben
				case ARM_HEBEN:
				{
					rotarm1 += 0.3f SYNC;
					rotarm2 += 0.2f SYNC;
					
					if (rotarm1 > 3.0f)
					{
						rotarm1 = 3.0f;
						state2 = ARM_SENKEN;
					}
				}
				break;

				// schnell absenken
				case ARM_SENKEN:
				{
					rotarm1 -= 0.8f SYNC;
					rotarm2 -= 0.7f SYNC;
					
					// Lava berührt?
					if (rotarm1 < 0.7f)
					{
						rotarm1 = 0.7f;
						rotarm2 = 0.7f;
						state2 = ARM_SENKEN2;

						// Spritzer
						for (int i=0; i<48; i++)
							pPartikelSystem->PushPartikel(xPos-80+rand()%50, yPos + 245, LAVA_SPRITZER2);

						pSoundManager->PlayWave3D((int)xPos + 30, (int)yPos + 30, 10000 + rand()%2050, SOUND_WATERIN);

						pProjectiles->PushProjectile(xPos - 90, yPos + 300, GOLEMSAEULE);
					}

				}
				break;

				// langsam wieder ein stück hoch federn
				case ARM_SENKEN2:
				{
					rotarm1 += 0.25f SYNC;
					rotarm2 += 0.25f SYNC;
					
					if (rotarm1 > 1.5f)
					{
						rotarm1 = 1.5f;
						rotarm2 = 1.5f;

						count -= 1.0f;

						if (count < 0.0f)
							Handlung = GEGNER_SPECIAL;
						else
							state2 = ARM_HEBEN;
					}
				}
				break;
			}
		} break;

		case GEGNER_EXPLODIEREN:
		{			
			Energy = 100.0f;
				
			AnimCount -= SpeedFaktor;
			if (AnimCount <= 0.0f)
			{
				AnimCount = 0.5f;

				// brodeln lassen
				pPartikelSystem->PushPartikel(xPos + rand()%160, yPos + rand()%300 + yoff, EXPLOSION_MEDIUM2);
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION1);

				if (rand()%2 == 0)
					pPartikelSystem->PushPartikel(xPos + rand()%160, yPos + rand()%300 + yoff, SMOKEBIG);				
			}

			ShotDelay -= 1.0f SYNC;

			if (ShotDelay <= 0.0f)
				Energy = 0.0f;
		} break;

		default : break;
	} // switch

	// Hat der Golem den Hurri berührt ?
	if (Handlung != GEGNER_EXPLODIEREN)
		TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Golem explodiert
// --------------------------------------------------------------------------------------

void GegnerGolem::GegnerExplode(void)
{
	pSoundManager->PlayWave(100, 128, 11025, SOUND_EXPLOSION2);

	for (int p = 0; p < NUMPLAYERS; p++)
		DirectInput.Joysticks[pPlayer[p]->JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);

	int i;

	for (i = 0; i < 100; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%200 - 50, yPos + yoff + rand()%300, ROCKSPLITTER);

	for (i = 0; i < 80; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%200 - 50, yPos + yoff + rand()%300, SMOKEBIG);

	for (i = 0; i < 40; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%200 - 50, yPos + yoff + rand()%300, EXPLOSION_MEDIUM2);	

	for (i = 0; i < 10; i++)
		pPartikelSystem->PushPartikel(xPos + rand()%200 - 50, yPos + yoff + rand()%300, EXPLOSION_BIG);	

	ShakeScreen(4);

	pPlayer[0]->Score += 8000;

	// Plattform spawnen, damit der Spieler weiterkommt =)
	pGegner->PushGegner(6760, 2500, PLATTFORM, 6, 0, false);

	ScrolltoPlayeAfterBoss();
}
