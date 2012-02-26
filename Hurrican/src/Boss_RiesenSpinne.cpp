// --------------------------------------------------------------------------------------
// Der Riesen-Spinnen Endboss
//
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_RiesenSpinne.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenSpinne::GegnerRiesenSpinne(int Wert1, int Wert2, bool Light)
{
	Handlung		= GEGNER_NOTVISIBLE;
	BlickRichtung	= LINKS;

	Energy			= 100000;
	LastEnergy		= 100000;
	Value1			= Wert1;
	Value2			= Wert2;
	ChangeLight		= Light;
	Destroyable		= true;
	OwnDraw			= true;	
	TestBlock		= false;

	ShotMode = 0;
	HeadWinkel = 0.0f;
	HeadXOffset = 0.0f;
	WalkCount = 0.0f;
	DamageWackel = 0.0f;
	WalkDir = -5.0f;
	yBody = 0.0f;
	ShotCount = 0;

	// Zusätzliche Grafiken für die Beine
	Head.LoadImage("spiderboss_kopf.png",  127, 92, 127, 92, 1, 1);
	Legs[0].LoadImage("spiderboss_foot.png",  305, 388, 61, 97, 5, 4);
	Legs[1].LoadImage("spiderboss_leg2.png",  320, 228, 32, 114, 10, 2);
	Legs[2].LoadImage("spiderboss_leg1.png",  340, 308, 34, 154, 10, 2);	
}

// --------------------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::StopCurrentAction(void)
{
}

// --------------------------------------------------------------------------------------
// Bein zeichnen
// x,y: Position
// winkel: Drehwinkel, geht von 0 bis 2 PI , wobei 0 = 2 PI = Ausgangsposition
// anim: Grafikausschnitt
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::DrawLeg(float x, float y, float winkel, int anim, int off, D3DCOLOR col)
{
	float xp, yp, yoff, yoff2;

	VERTEX2D	TriangleStrip[4];					// Strip für einen Schuss

	x += 120;

	// Je nach "winkel" die beine anderes positionieren
	if (winkel < PI) 
	{
		xp = x + (float(sin(winkel + PI / 2.0f) * 18.0f));
		yp = y + (float(cos(winkel + PI / 2.0f) * 30.0f));
	}
	else
	{
//		xp = x + (float(sin(winkel + PI / 2.0f) * 20.0f));
		xp = x + (winkel - PI) * 10.0f - 16;
		yp = y;
	}		

	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten

	float tempdamage = (float)sin(DamageWackel * 2 * PI) * 10.0f;

//----- Oberteil des Beins rendern

	yoff = (float)(sin(WalkCount) * 2.0f);
	yoff2 = (float)(sin(winkel - PI - 0.5f) * 5.0f);
	Legs[2].itsRect = Legs[2].itsPreCalcedRects [anim*3 + off];

	l = xp - 0.5f;													// Links
	r = xp+(Legs[2].itsRect.right-Legs[2].itsRect.left-1)+0.5f;		// Rechts
	o = y + 55.0f - 0.5f - (Legs[2].itsRect.bottom - Legs[2].itsRect.top) + yoff2 + yBody;	// Oben
	u = y + 30.0f + 0.5f + yoff + tempdamage + yBody;										// unten

	tl = Legs[2].itsRect.left  /Legs[2].itsXSize;	// Links
	tr = Legs[2].itsRect.right /Legs[2].itsXSize;	// Rechts
	to = Legs[2].itsRect.top   /Legs[2].itsYSize;	// Oben
	tu = Legs[2].itsRect.bottom/Legs[2].itsYSize;	// Unten

	TriangleStrip[0].color = 
	TriangleStrip[1].color = 
	TriangleStrip[2].color = 
	TriangleStrip[3].color = col;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

	TriangleStrip[0].x		= l;		// Links oben
	TriangleStrip[0].y		= o + yoff;
	TriangleStrip[0].tu		= tl;
	TriangleStrip[0].tv		= to;

	TriangleStrip[1].x		= r;		// Rechts oben
	TriangleStrip[1].y		= o + yoff;
	TriangleStrip[1].tu		= tr;
	TriangleStrip[1].tv		= to;

	TriangleStrip[2].x		= l;		// Links unten
	TriangleStrip[2].y		= u;
	TriangleStrip[2].tu		= tl;
	TriangleStrip[2].tv		= tu;

	TriangleStrip[3].x		= r;		// Rechts unten
	TriangleStrip[3].y		= u;
	TriangleStrip[3].tu		= tr;
	TriangleStrip[3].tv		= tu;

	//TriangleStrip[0].x = xPos + 100 + anim*40;
	//TriangleStrip[1].x = xPos + 100 + anim*40 + 34;	

	float winkel2 = -(float)((int)(winkel + 320.0f) % 360);
	float temp = (float)(sin(winkel) * 10.0f);

	switch (anim)
	{
		case 0: 
			TriangleStrip[0].x = temp + x + 40;
			TriangleStrip[1].x = temp + x+32 + 40;
			TriangleStrip[2].x = x + 120;
			TriangleStrip[3].x = x+34 + 120;			
		break;

		case 1: 
			TriangleStrip[0].x = temp + x + 24;
			TriangleStrip[1].x = temp + x+32 + 24;
			TriangleStrip[2].x = x + 40;
			TriangleStrip[3].x = x+34 + 40;
		break;

		case 2: 
			TriangleStrip[0].x = temp + x - 10;
			TriangleStrip[1].x = temp + x+32 - 10;
			TriangleStrip[2].x = x - 40;
			TriangleStrip[3].x = x+34 - 40;
		break;

		case 3: 
			TriangleStrip[0].x = temp + x - 20;
			TriangleStrip[1].x = temp + x+32 - 20;
			TriangleStrip[2].x = x - 120;
			TriangleStrip[3].x = x+34 - 120;
		break;
	}

	lpD3DDevice->SetTexture (0, Legs[2].itsTexture);
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);

//----- Unterteil des Beines rendern

	Legs[1].itsRect = Legs[1].itsPreCalcedRects [anim*3 + off];

	l = xp - 0.5f;													// Links
	r = xp+(Legs[1].itsRect.right-Legs[1].itsRect.left-1)+0.5f;		// Rechts
	o = y + 20.0f - 0.5f - (Legs[1].itsRect.bottom - Legs[1].itsRect.top) + yoff2 + yBody;	// Oben
	u = yp + + 20.0f + 0.5f;													// unten

	tl = Legs[1].itsRect.left  /Legs[1].itsXSize;	// Links
	tr = Legs[1].itsRect.right /Legs[1].itsXSize;	// Rechts
	to = Legs[1].itsRect.top   /Legs[1].itsYSize;	// Oben
	tu = Legs[1].itsRect.bottom/Legs[1].itsYSize;	// Unten

	TriangleStrip[0].color = 
	TriangleStrip[1].color = 
	TriangleStrip[2].color = 
	TriangleStrip[3].color = col;
	TriangleStrip[0].z	   = TriangleStrip[1].z		= TriangleStrip[2].z	 = TriangleStrip[3].z	  = 0.0f;

	TriangleStrip[0].x		= l;		// Links oben
	TriangleStrip[0].y		= o;
	TriangleStrip[0].tu		= tl;
	TriangleStrip[0].tv		= to;

	TriangleStrip[1].x		= r;		// Rechts oben
	TriangleStrip[1].y		= o;
	TriangleStrip[1].tu		= tr;
	TriangleStrip[1].tv		= to;

	TriangleStrip[2].x		= l;		// Links unten
	TriangleStrip[2].y		= u;
	TriangleStrip[2].tu		= tl;
	TriangleStrip[2].tv		= tu;

	TriangleStrip[3].x		= r;		// Rechts unten
	TriangleStrip[3].y		= u;
	TriangleStrip[3].tu		= tr;
	TriangleStrip[3].tv		= tu;

	switch (anim)
	{
		case 0: 
			TriangleStrip[0].x = temp + x + 40;
			TriangleStrip[1].x = temp + x+32 + 40;
			TriangleStrip[2].x += 20;
			TriangleStrip[3].x += 20;
		break;

		case 1: 
			TriangleStrip[0].x = temp + x + 20;
			TriangleStrip[1].x = temp + x+32 + 20;
			TriangleStrip[2].x += 20;
			TriangleStrip[3].x += 20;
		break;

		case 2: 
			TriangleStrip[0].x = temp + x - 20;
			TriangleStrip[1].x = temp + x+32 - 20;
		break;

		case 3: 
			TriangleStrip[0].x = temp + x - 40;
			TriangleStrip[1].x = temp + x+32 - 40;
		break;
	}

	lpD3DDevice->SetTexture (0, Legs[1].itsTexture);
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2,&TriangleStrip[0]);
	

//----- Fuß rendern
	Legs[0].RenderSpriteRotated(xp, yp, 0, anim*3 + off, col);	
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::DoDraw(void)
{
	D3DCOLOR Color = 0xFFFFFFFF;	

	DirectGraphics.SetFilterMode(true);

	// Beine hinter dem Körper
	for (int i = 0; i < 4; i++)
	{
		int a;
		float w;

		if (i == 0) a = 0;
		if (i == 1) a = 1;
		if (i == 2) a = 3;
		if (i == 3) a = 2;

		w = LegsAnim[a] + PI;

		if (w > 2*PI) 
			w -= 2*PI;

		DrawLeg((float)(xPos-pTileEngine->XOffset - 70 + a * 110), 
				(float)(yPos-pTileEngine->YOffset + 130), 
				w, a, 10, 0xFF888888);
	}

	float tempwinkel, tempdamage;

	tempdamage = (float)sin(DamageWackel * 2 * PI) * 10.0f;
	tempwinkel = HeadWinkel - (float)sin(DamageWackel) * 12.0f;

	while (tempwinkel > 360.0f)
		tempwinkel -= 360.0f;

	// Rumpf
	pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
										   (float)(yPos-pTileEngine->YOffset) + (float)(sin(WalkCount) * 2.0f) + tempdamage + yBody, 0, Color);	

	// Kopf	
	Head.RenderSpriteRotatedOffset((float)(xPos-pTileEngine->XOffset - 90 + 120) + (float)(sin(HeadXOffset) * 10.0f), 
								   (float)(yPos-pTileEngine->YOffset + 60) + (float)(sin(WalkCount) * 2.0f) + tempdamage + yBody, 
								   tempwinkel, 127, 0,
								   Color);	

	// Beine vor dem Körper
	for (int i = 0; i < 4; i++)
	{
		int a;

		if (i == 0) a = 0;
		if (i == 1) a = 1;
		if (i == 2) a = 3;
		if (i == 3) a = 2;

		DrawLeg((float)(xPos-pTileEngine->XOffset - 70 + a * 110), 
				(float)(yPos-pTileEngine->YOffset + 130), 
				LegsAnim[a], a, 0, 0xFFFFFFFF);	
	}

	// Evtl. Lila Leuchten vor kopf
	if (Handlung == GEGNER_LAUFEN_LINKS &&
		Energy < 75000.0f)
	{

		int a = (int)(255.0f - ShotDelay / 15.0f * 255.0f);

		DirectGraphics.SetAdditiveMode();

		LavaFlare.RenderSprite((float)(xPos-pTileEngine->XOffset) - 10.0f + (float)(sin(HeadXOffset) * 10.0f), 
							   (float)(yPos-pTileEngine->YOffset) + 50.0f + (float)(sin(WalkCount) * 2.0f) + tempdamage + yBody, D3DCOLOR_RGBA(255, 0, 255, a));

		LavaFlare.RenderSpriteScaled((float)(xPos-pTileEngine->XOffset) + 30.0f + (float)(sin(HeadXOffset) * 10.0f), 
								     (float)(yPos-pTileEngine->YOffset) + 90.0f + (float)(sin(WalkCount) * 2.0f) + tempdamage + yBody, 40, 40, D3DCOLOR_RGBA(255, 255, 255, a));
	}

	// Evtl. Lila Leuchten vor kopf
	if (Handlung == GEGNER_SPECIAL2)
	{
		int a = (int)(ShotDelay);

		DirectGraphics.SetAdditiveMode();

		LavaFlare.RenderSpriteScaled((float)(xPos-pTileEngine->XOffset) - 20.0f + (float)(sin(HeadXOffset) * 10.0f), 
								     (float)(yPos-pTileEngine->YOffset) + 60.0f + (float)(sin(WalkCount) * 2.0f) + tempdamage + yBody, 100, 100, D3DCOLOR_RGBA(255, 255, 255, a));

		LavaFlare.RenderSpriteScaled((float)(xPos-pTileEngine->XOffset) - 90.0f + (float)(sin(HeadXOffset) * 10.0f), 
								     (float)(yPos-pTileEngine->YOffset) - 10.0f + (float)(sin(WalkCount) * 2.0f) + tempdamage + yBody, 240, 240, D3DCOLOR_RGBA(255, 255, 255, a));
	}

	DirectGraphics.SetFilterMode(false);
	DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Neue Handlung, wenn sie links steht
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::RandomHandlung(void)
{
	ShotDelay = 4.0f;					
	xSpeed = 0.0f;

	static int last = - 1;
		   int j = -1;

	// Per Zufall neue Aktion, die eben noch nicht dran war
	do
	{
		j = rand()%6;

	} while (j == last);

	// letzte Aktion merken
	last = j;

	switch (j)
	{
		case 0:
		{
			ShotCount = 10 + rand()%10;
			Handlung = GEGNER_SCHIESSEN;
		} break;

		case 1:
		{						
			Handlung = GEGNER_ABSENKEN;
		} break;

		case 2:
		{
			ShotCount = 20 + rand()%10;
			Handlung = GEGNER_SPECIAL;
		} break;

		case 3:
		{
			Handlung = GEGNER_ABSENKENZWEI;					
		} break;

		case 4:
		{		
			Handlung = GEGNER_LAUFEN_LINKS;
			ShotDelay = 1.0f;
			ShotMode = rand()%2;
		} break;
	
		case 5:
		{
			Handlung = GEGNER_SPECIAL2;		
			pProjectiles->PushProjectile((float) Value1, yPos + yBody + 110.0f, SPIDERLASER);
			pSoundManager->PlayWave (100, 128, 11025, SOUND_BEAMLOAD2);
			AnimCount = 140;
			ShotDelay = 0.0f;
		} break;
	}
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::DoKI(void)
{
	// Energie anzeigen
	if (Handlung != GEGNER_NOTVISIBLE && Handlung != GEGNER_EXPLODIEREN)
		pHUD->ShowBossHUD(100000, Energy);

	// Levelausschnitt auf die RiesenSpinne zentrieren, sobald diese sichtbar wird
	if (Active == true && pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
		pTileEngine->ScrollLevel((float)Value1, 
								 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf die Spinne zentrieren

		pSoundManager->FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
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
			}
		} break;

		case GEGNER_EINFLIEGEN:		// Spinne läuft ins Level ein
		{			
			yPos -= 55;
			Energy = 100000;
			DamageTaken = 0.0f;

//			if (xPos <= Value1 + 460)			// Weit genug eingelaufen ?
			{
				pSoundManager->PlayWave(100, 128, 11025, SOUND_SPIDERSCREAM);
				Handlung = GEGNER_LAUFEN_LINKS;
				ShotDelay = 1.0f;
				ShotMode = rand()%2;
			}
		} break;

		// Spinne läuft nach links oder rechts
		case GEGNER_CRUSHEN:
		case GEGNER_LAUFEN_LINKS:
		case GEGNER_LAUFEN_RECHTS:
		case GEGNER_EXPLODIEREN:
		{
			// Kopf gerade richten
			if (HeadWinkel > 0.0f)
				HeadWinkel -= 1.0f SYNC;

			if (HeadWinkel < 0.0f)
				HeadWinkel += 1.0f SYNC;	

			xSpeed = WalkDir;
			WalkCount -= WalkDir / 10.0f SYNC;

			if (WalkCount > 2*PI) WalkCount -= 2*PI;
			if (WalkCount < 0.0f) WalkCount += 2*PI;

			if (Handlung == GEGNER_CRUSHEN)
			{				
				if (HeadWinkel < 45.0f)
					HeadWinkel += 1.0f SYNC;

				WalkDir -= 10.0f SYNC;

				if (WalkDir < -35.0f)
					WalkDir = -35.0f;				

				if (xPos < Value1 - 50)
				{
					Handlung = GEGNER_LAUFEN_RECHTS;
					ShotDelay = 15.0f;
				}

				// Spieler wegschieben?

				RECT coll;				

				coll.left = 50;					coll.right  = 120;
				coll.top  = 40 + int(yBody);	coll.bottom = 140 + int(yBody);
				Wegschieben(coll, 50.0f);
			}

			if (Handlung == GEGNER_LAUFEN_LINKS)
			{								
				WalkDir -= 1.0f SYNC;

				if (WalkDir < -8.0f)
					WalkDir = -8.0f;				

				if (xPos < Value1 + 100)
				{
					Handlung = GEGNER_LAUFEN_RECHTS;
					ShotDelay = 15.0f;
				}

				// evtl schiessen
				if (Energy < 75000.0f)
				{
					ShotDelay -= 1.0f SYNC;

					if (DamageWackel > 0.0f)
						ShotDelay = 15.0f;

					if (ShotDelay < 0.0f)
					{
						ShotDelay = 12.0f;

						pSoundManager->PlayWave(100, 128, 8000 + rand()%1000, SOUND_LILA);
						pPartikelSystem->PushPartikel(xPos + 30.0f - 50.0f, yPos + yBody + 85.0f - 35.0f, EXPLOSIONFLARE2);

						if (ShotMode == 0)
							pProjectiles->PushProjectile(xPos + 30.0f, yPos + yBody + 85.0f, SPIDERSLOW);
						else
						{
							for (int i = 0; i < 5; i++)
							{
								WinkelUebergabe = 212 - i * 15.0f;
								pProjectiles->PushProjectile(xPos, yPos + yBody + 85.0f, SPIDERSHOTLANGSAM);
							}

						}
						
					}
				}
			}

			if (Handlung == GEGNER_LAUFEN_RECHTS ||
				Handlung == GEGNER_EXPLODIEREN)
			{
				WalkDir += 1.0f SYNC;

				// Körper hoch, wenn er grade unten ist
				if (yBody > 0.0f)
				{
					yBody -= 1.0f SYNC;

					if (yBody < 0.0f)
						yBody -= 0.0f;
				}

				if (Handlung == GEGNER_LAUFEN_RECHTS)
				{
					// Spinnen hinten rauslassen							
					ShotDelay -= 1.0f SYNC;
					if (ShotDelay < 0.0f)
					{
						ShotDelay = 15.0f;

						pGegner->PushGegner(xPos + 400, yPos + 100.0f + yBody, SPIDERBOMB, 0, 0, false);
					}
				}

				// Nach crushen kurz schneller laufen
				if (xPos < Value1 - 550)
					WalkDir = 30.0f;

				else
				{
					if (Handlung == GEGNER_LAUFEN_RECHTS)
					if (WalkDir > 8.0f)
						WalkDir = 8.0f;

					if (Handlung == GEGNER_EXPLODIEREN)
					if (WalkDir > 4.0f)
						WalkDir = 4.0f;					
				}

				if (Handlung == GEGNER_LAUFEN_RECHTS)
				if (xPos > Value1 + 400)
					RandomHandlung();				

			}

			if (Handlung == GEGNER_EXPLODIEREN)
			{
				Energy = 100.0f;

				AnimCount -= 1.0f SYNC;

				if (AnimCount < 0.0f)
				{
					AnimCount = 0.4f;

					pPartikelSystem->PushPartikel(xPos + rand()%400, yPos + rand()%100, EXPLOSION_MEDIUM + rand()%2);
					pPartikelSystem->PushPartikel(xPos + rand()%400, yPos + rand()%100, SMOKEBIG);

					if (rand()%5 == 0)
						pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_EXPLOSION3 + rand()%2);
				}

				if (xPos > Value1 + 640)
					Energy = 0.0f;
			}

			// AnimStufen für die einzelnen Beine setzen
			for (int i = 0; i < 4; i++)
			{
				LegsAnim[i] = WalkCount + (PI / 3.0f) * i;

				while (LegsAnim[i] > 2*PI)
					LegsAnim[i] -= 2*PI;
			}

		} break;

		// Raketen abschiessen
		case GEGNER_SCHIESSEN:
		{
			ShotDelay -= 1.0f SYNC;

			if (ShotDelay < 0.0f)
			{
				ShotDelay = 4.0f;

				// Rakete abschiessen
				pSoundManager->PlayWave(100, 128, 10000 + rand()%2000, SOUND_ROCKET);
				pGegner->PushGegner(xPos + 35.0f, yPos + 95.0f, FETTERAKETE, rand()%60 - 130, 0, false);

				// Kopf zurückschnellen lassen
				HeadXOffset = 2 * PI;

				// Shusscounter runterzählen
				ShotCount--;
				if (ShotCount == 0 ||
					pAim->xpos > xPos - 50)
				{
					Handlung  = GEGNER_LAUFEN_LINKS;
					ShotMode  = rand()%2;
					ShotDelay = 5.0f;
				}
				//RandomHandlung();
			}						
		} break;

		// Kopf bewegen und Schüsse abgeben
		case GEGNER_SPECIAL:
		{
			static float headdir = 1.0f;

			HeadWinkel += headdir SYNC;

			if (HeadWinkel >  15.0f) headdir = -1.0f;
			if (HeadWinkel < -10.0f) headdir = 1.0f;

			ShotDelay -= 1.0f SYNC;

			if (ShotDelay < 0.0f)
			{
				ShotDelay = 3.5f;

				// Böller abschiessen
				pSoundManager->PlayWave(100, 128, 10000 + rand()%2000, SOUND_LILA);
				WinkelUebergabe = 180 - HeadWinkel * 1.5f;
				pProjectiles->PushProjectile(xPos - (float)cos(HeadWinkel * PI / 180.0f) * 10.0f, 
											 yPos + (float)sin(HeadWinkel * PI / 180.0f) * 140.0f + 90.0f + yBody, SPIDERSHOT);

				// Kopf zurückschnellen lassen
				HeadXOffset = 2 * PI;

				// Shusscounter runterzählen
				ShotCount--;
				if (ShotCount == 0)
				{
					Handlung = GEGNER_LAUFEN_LINKS;
					ShotMode = rand()%2;
					ShotDelay = 5.0f;
				}
					//RandomHandlung();
			}			
		} break;

		case GEGNER_SPECIAL2:
		{
			// Flare einfaden
			if (ShotDelay < 255.0f)
				ShotDelay += 5.8f SYNC;
			else
				ShotDelay = 255.0f;

			// Counter runterzählen
			AnimCount -= 2.5f SYNC;
			if (AnimCount < 0.0f)
			{
				Handlung = GEGNER_LAUFEN_LINKS;
				ShotMode = rand()%2;
				ShotDelay = 5.0f;
			}
		} break;

		case GEGNER_ABSENKEN:
		{
			yBody += 1.0f SYNC;			

			// ganz unten?
			if (yBody > 50.0f)
			{
				Handlung = GEGNER_AUFRICHTEN;
			}

			// schiessen
			ShotDelay -= 1.0f SYNC;

			if (ShotDelay < 0.0f)
			{
				ShotDelay = 10.0f;

				// Böller abschiessen
				pSoundManager->PlayWave(100, 128, 10000 + rand()%2000, SOUND_LILA);
				pProjectiles->PushProjectile(xPos + 20.0f, yPos + 80.0f + yBody, SPIDERSHOT2);
				pProjectiles->PushProjectile(xPos + 20.0f, yPos + 100.0f + yBody, SPIDERSHOT2);

				// Kopf zurückschnellen lassen
				HeadXOffset = 3 * PI;
			}
		} break;

		case GEGNER_ABSENKENZWEI:
		{
			yBody += 8.0f SYNC;			

			// ganz unten? Dann nach links losrennen
			if (yBody > 50.0f)
			{
				Handlung = GEGNER_CRUSHEN;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_SPIDERSCREAM);
			}
		} break;

		case GEGNER_AUFRICHTEN:
		{
			yBody -= 1.0f SYNC;			

			if (yBody <= 0.0f)
			{
				yBody = 0.0f;
				Handlung = GEGNER_LAUFEN_LINKS;
				ShotMode = rand()%2;
				ShotDelay = 5.0f;
				//RandomHandlung();
			}

			// schiessen
			ShotDelay -= 1.0f SYNC;

			if (ShotDelay < 0.0f)
			{
				ShotDelay = 10.0f;

				// Böller abschiessen
				pSoundManager->PlayWave(100, 128, 10000 + rand()%2000, SOUND_LILA);
				WinkelUebergabe = PlayerAbstandHoriz() / 10.0f;
				pProjectiles->PushProjectile(xPos + 20.0f, yPos + 80.0f + yBody, SPIDERSHOT2);
				pProjectiles->PushProjectile(xPos + 20.0f, yPos + 100.0f + yBody, SPIDERSHOT2);

				// Kopf zurückschnellen lassen
				HeadXOffset = 3 * PI;
			}
		} break;
		

		default : break;
	} // switch	

	// Kopf zurückgeschnellt?
	if (HeadXOffset > PI)
		HeadXOffset -= 0.8f SYNC;
	else
		HeadXOffset = PI;

	// Spinne hat Schaden genommen?
	if (DamageTaken > 0.0f)
	{
		// War es ein Fass? Dann den Kopf heben
		if (LastEnergy - Energy > 5000)
		{
			if (Handlung != GEGNER_SPECIAL2)
				DamageWackel = PI;

			pSoundManager->PlayWave(100, 128, 15000, SOUND_SPIDERSCREAM);

			if (Handlung == GEGNER_SCHIESSEN ||
				Handlung == GEGNER_SPECIAL   ||
				Handlung == GEGNER_ABSENKEN  ||
				Handlung == GEGNER_AUFRICHTEN)
				Handlung = GEGNER_LAUFEN_LINKS;
		}
		else
			DamageTaken = 0.0f;

		LastEnergy = Energy;
	}

	// Spinne wackelt gerade mit dem Kopf wegen Schaden?
	if (DamageWackel > 0.0f)
	{
		DamageWackel -= 0.2f SYNC;
	}
	else
		DamageWackel = 0.0f;

	GegnerRect[RIESENSPINNE].left = 60;					GegnerRect[RIESENSPINNE].right  = 360;
	GegnerRect[RIESENSPINNE].top  = 40 + int(yBody);	GegnerRect[RIESENSPINNE].bottom = 140 + int(yBody);

	// Testen, ob der Spieler den Boss berührt hat
	TestDamagePlayers((float) 4.0 SYNC);

	// Hat die Spinnen keine Energie mehr ? Dann explodiert sie
	if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
	{
		Energy = 100.0f;
		Handlung  = GEGNER_EXPLODIEREN;
		AnimCount = 1.0f;
		pProjectiles->ClearType(SPIDERLASER);

		// Endboss-Musik ausfaden und abschalten
		pSoundManager->FadeSong(MUSIC_BOSS, -2.0f, 0, false);
	}
}

// --------------------------------------------------------------------------------------
// RiesenSpinne explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenSpinne::GegnerExplode(void)
{
	pPlayer[0]->Score += 15000;
	pSoundManager->PlayWave(100, 128, 11025, SOUND_EXPLOSION2);	
	ShakeScreen(5.0f);

	ScrolltoPlayeAfterBoss();
}
