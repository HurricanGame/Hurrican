// --------------------------------------------------------------------------------------
// Klasse für die Gegner und Klasse für die Linked List der Gegner
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GegnerClass.h"
#include "Gegner_Helper.h"
#include "Player.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerClass::GegnerClass(void)
{	
	Active		 = false;
	TestBlock    = true;
	OwnDraw	     = false;
	DontMove     = false;
	ForceLight	 = false;
	BackGround   = false;
	xPos		 = 0.0f; yPos	= 0.0f;
	xPosOld		 = 0.0f; yPosOld	= 0.0f;
	xSpeed	 	 = 0.0f;	ySpeed	= 0.0f;
	xAcc	 	 = 0.0f;	yAcc	= 0.0f;
	AnimStart 	 = 0;
	AnimPhase    = 0;	AnimCount   = 0.0f;
	AnimEnde     = 0;	AnimSpeed   = 0.0f;
	DamageTaken  = 0;
	LastAction   = -1;
	HitSound     = 0;
	TimeToChange = 50.0f;
	TurnCount    = 0.0f;

	// Ziel zufällig wählen
	pAim = ChooseAim();	
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

GegnerClass::~GegnerClass(void)
{
	for (int p = 0; p < NUMPLAYERS; p++)
		if (pPlayer[p]->AufPlattform == this)
			pPlayer[p]->AufPlattform = NULL;
}

// --------------------------------------------------------------------------------------
// Testen, ob der Gegner die Spieler berührt hat
// --------------------------------------------------------------------------------------

void GegnerClass::TestDamagePlayers(float dam, bool destroy)
{
	for (int i = 0; i < NUMPLAYERS; i++)
	if (SpriteCollision(xPos,  yPos,  GegnerRect[GegnerArt], 
						pPlayer[i]->xpos, pPlayer[i]->ypos, pPlayer[i]->CollideRect))
	{
		pPlayer[i]->DamagePlayer(dam);

		if (destroy == true)
			Energy = 0.0f;
	}
}

// --------------------------------------------------------------------------------------
// Gegner explodiert
// --------------------------------------------------------------------------------------

void GegnerClass::GegnerExplode(void)
{
	for (int p = 0; p < NUMPLAYERS; p++)
		if (pPlayer[p]->AufPlattform == this)
			pPlayer[p]->AufPlattform = NULL;	
}

// --------------------------------------------------------------------------------------
// Gegner nur rendern. Entweder per Standard oder per eigener Drawroutine
// --------------------------------------------------------------------------------------

void GegnerClass::Render(void)
{	
	// Nicht auf Screen? Dann nicht rendern ;)
	if (IsOnScreen() == false)
		return;

	bool mirrored;				// Gegner kuckt in die andere Richtung ?
	int Anim = AnimPhase;
	D3DCOLOR Color;

	mirrored = (BlickRichtung == RECHTS);	

	if (GegnerArt == DIAMANT)		// Diamant
		Color = 0xFFFFFFFF;
	else
	{
		if (ChangeLight == true)
			Color = pTileEngine->LightValue(xPos, yPos, GegnerRect[GegnerArt], ForceLight);
		else
			Color = 0xFFFFFFFF;
	}

	// Gegner rendern (einige bekommen ne Extra Wurst)
	if (Handlung != GEGNER_NOTVISIBLE)
	{
		if (OwnDraw  == false &&
			DontMove == false)
		{
			DirectGraphics.SetColorKeyMode();

			if (pGegnerGrafix[GegnerArt]->itsTexture != NULL)
				pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
													   (float)(yPos-pTileEngine->YOffset), 
													   Anim, Color, mirrored);
		}
		// DAS ist die Extrawurst =)
		else			
			if (OwnDraw == true)
				DoDraw();
	}

	// Gegner getroffen? Dann nochmal in weiss aufleuchten lassen
	if (Handlung != GEGNER_NOTVISIBLE)
	if (DamageTaken > 0.0f)
	{
		int	  Wert;
		float f = DamageTaken;
		
		MYMATH_FTOL(f,Wert);

		Color = D3DCOLOR_RGBA(255, 255, 255, Wert);		

		// neu rendern
		if (Handlung != GEGNER_NOTVISIBLE)
		{		
			DirectGraphics.SetAdditiveMode();

			if (OwnDraw  == false &&
				DontMove == false)
			{
				if (pGegnerGrafix[GegnerArt]->itsTexture != NULL)
					for (int i = 0; i < 2; i++)
						pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset), 
															   (float)(yPos-pTileEngine->YOffset), 
															   Anim, Color, mirrored);
			}

			// Wieder Extrawurst
			else
			if (OwnDraw  == true)
				for (int i = 0; i < 2; i++)
					DoDraw();		

			DirectGraphics.SetColorKeyMode();		
		}

		// Rotwerden langsam ausfaden lassen
		// und ggf anhalten
		//
		if (GegnerArt != POWERBLOCK)
			DamageTaken -= 250 SYNC;
		else
			DamageTaken -= 75 SYNC;

		if (DamageTaken < 0.0f)				
			DamageTaken = 0.0f;
	}

	// Im Debug Mode noch den Abstand zum Spieler anzeigen
	if (DebugMode == true)
	{
		char Buffer[10];
		_itoa_s(PlayerAbstand(), Buffer, 10);
		pMenuFont->DrawText(float(xPos-pTileEngine->XOffset), 
					 	   	float(yPos-pTileEngine->YOffset), Buffer, 0xFFFFFFFF);
	}
}

// --------------------------------------------------------------------------------------
// Gegner auf Grenzen checken usw
// --------------------------------------------------------------------------------------

bool GegnerClass::Run(void)
{	
	// Ist der Gegner überhaupt schon aktiviert worden, also im Screen gewesen ?
	// Oder ist es ein Eisstachel, der sich immer bewegt 
	if (Active    == true ||
		GegnerArt == EISSTACHEL)
	{
		// "KI" kann man es zwar nicht nennen, aber wir führen es trotzdem aus ;)
		DoKI();

		// Muss der Gegner garnicht bewegt oder angezeigt werden, weil es zB ein Trigger ist ?
		//
		if (DontMove == true)
			return false;

		// Animationsphasen checken
		if (AnimStart < 0 || AnimPhase < 0) 
		{
			AnimStart = 0;
		}

		// pAim wechseln?
		if (TimeToChange > 0.0f)
			TimeToChange -= 1.0f SYNC;

		// Feststellen welche Blockarten um den Gegner herum existieren
		if (TestBlock == true)
		{
			blockl = pTileEngine->BlockLinks	   (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
			blockr = pTileEngine->BlockRechts	   (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
			blocko = pTileEngine->BlockOben		   (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
			blocku = pTileEngine->BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
		}
		else
			blockl = blockr = blocko = blocku = 0;

		// Geschwindigkeit anpassen
		xSpeed += xAcc   SYNC;
		ySpeed += yAcc   SYNC;

		// Bewegen (nur, wenn keine Wand im Weg ist)
		//
		if ((xSpeed < 0.0f && !(blockl & BLOCKWERT_WAND)) ||
			(xSpeed > 0.0f && !(blockr & BLOCKWERT_WAND)))
			xPos += xSpeed SYNC;

		if ((ySpeed < 0.0f && !(blocko & BLOCKWERT_WAND)) ||
			(ySpeed > 0.0f && !(blocku & BLOCKWERT_WAND)))
			yPos += ySpeed SYNC;

		// Level verlassen ?
		//
		if (GegnerArt != FAHRSTUHL &&
			(xPos + GegnerRect[GegnerArt].right  < 0.0f ||
			 yPos + GegnerRect[GegnerArt].bottom < 0.0f ||
			 xPos > pTileEngine->LEVELPIXELSIZE_X ||
			 yPos > pTileEngine->LEVELPIXELSIZE_Y))
			Energy = 0.0f;		

		// Gegner Energie abziehen, wenn der Spieler ein Rad ist
		for (int p = 0; p < NUMPLAYERS; p++)
		{
			if ((pPlayer[p]->Handlung == RADELN ||
				 pPlayer[p]->Handlung == RADELN_FALL) &&
				 Destroyable == true &&
				 SpriteCollision(xPos,   yPos, GegnerRect[GegnerArt],
								 pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect) == true)
			{
				Energy	  -= 10.0f SYNC;	// Energie abziehen

				// blinken lassen, wenn noch nicht blinkt
				if (DamageTaken <= 0.0f)
					DamageTaken = 255;
			
				if (pPlayer[p]->WheelMode == false)
					pPlayer[p]->Armour -= float(3.0 SYNC);	// Spieler verliert Rad Energie

				// Hit Sound
				if (true == pSoundManager->InitSuccessfull &&
					!(pSoundManager->its_Sounds[SOUND_HIT + HitSound]->isPlaying))
					pSoundManager->PlayWave(100, 128, 22050, SOUND_HIT + HitSound);
			}
		}
	}
	else

	// Taucht der Gegner im Screen auf und wird dadurch aktiviert ?
	// Dann auch in Richtung Spieler blicken lassen
	//
	if (yPos-pTileEngine->YOffset < 480 &&
		yPos-pTileEngine->YOffset > 0 - GegnerRect[GegnerArt].bottom   &&
		xPos-pTileEngine->XOffset < 640 &&
		xPos-pTileEngine->XOffset > 0 - GegnerRect[GegnerArt].right)
	{
		Active = true;

		if (pPlayer[0]->xpos + 35 < xPos + GegnerRect[GegnerArt].left + (GegnerRect[GegnerArt].right - GegnerRect[GegnerArt].left) / 2)
			BlickRichtung = LINKS;
		else
			BlickRichtung = RECHTS;

		if (GegnerArt != EXTRAS)
		{
			xSpeed = xSpeed * BlickRichtung;
			xAcc   = xAcc * BlickRichtung; 
		}

		if (TestBlock == true)
		{
			blockl = pTileEngine->BlockLinks	   (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
			blockr = pTileEngine->BlockRechts	   (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
			blocko = pTileEngine->BlockOben		   (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
			blocku = pTileEngine->BlockUntenNormal (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);
		}
		else
			blockl = blockr = blocko = blocku = 0;
	}

	// Positionen sichern
	//
	xPosOld = xPos;
	yPosOld = yPos;

	// Aim ist tot? Dann neues suchen
	//
	if (pAim->Handlung == TOT)
		pAim = ChooseAim();

	return true;
}

// --------------------------------------------------------------------------------------
// Abstand zwischen Gegner und Spieler berechnen und zurückliefern
// --------------------------------------------------------------------------------------

int GegnerClass::PlayerAbstand(bool both)
{
	float xdiff, ydiff, Abstand;

	Abstand = 99999.9f;

	if (both)
	{
		for (int p = 0; p < NUMPLAYERS; p++)
		{
			xdiff = (pPlayer[p]->xpos + 35) - (xPos + GegnerRect[GegnerArt].right/2);
			ydiff = (pPlayer[p]->ypos + 40) - (yPos + GegnerRect[GegnerArt].bottom/2);

			Abstand = min(Abstand, float(sqrt((xdiff * xdiff) + (ydiff * ydiff))));
		}

	}
	else
	{
		xdiff = (pAim->xpos + 35) - (xPos + GegnerRect[GegnerArt].right/2);
		ydiff = (pAim->ypos + 40) - (yPos + GegnerRect[GegnerArt].bottom/2);

		Abstand = float(sqrt((xdiff * xdiff) + (ydiff * ydiff)));
	}

	int a;

	MYMATH_FTOL(Abstand, a);

	return a;
}

// --------------------------------------------------------------------------------------
// Horizontaler Abstand zwischen Gegner und Spieler berechnen und zurückliefern
// --------------------------------------------------------------------------------------

int GegnerClass::PlayerAbstandHoriz(PlayerClass *pTarget)
{
	float Abstand;

	if (pTarget == NULL)
		pTarget = pAim;

	Abstand = (pTarget->xpos + pTarget->CollideRect.left + (pTarget->CollideRect.right - pTarget->CollideRect.left)/2)
			- (xPos + GegnerRect[GegnerArt].left + (GegnerRect[GegnerArt].right - GegnerRect[GegnerArt].left)/2);

	int a;

	MYMATH_FTOL(Abstand, a);

	return abs (a);
}

// --------------------------------------------------------------------------------------
// Vertikaler Abstand zwischen Gegner und Spieler berechnen und zurückliefern
// --------------------------------------------------------------------------------------

int GegnerClass::PlayerAbstandVert(PlayerClass *pTarget)
{
	float Abstand;

	if (pTarget == NULL)
		pTarget = pAim;

	Abstand = (pTarget->ypos + pTarget->CollideRect.top  + (pTarget->CollideRect.bottom - pTarget->CollideRect.top)/2) 
			- (yPos + GegnerRect[GegnerArt].bottom/2);

	int a;

	MYMATH_FTOL(Abstand, a);

	return abs (a);
}

// --------------------------------------------------------------------------------------
// Feststellen ob der Spieler auf einem Gegner steht (der als Plattform in Frage kommt)
// --------------------------------------------------------------------------------------

void GegnerClass::PlattformTest(RECT rect)
{
	// Spieler steht auf dem Gegner
	//
	for (int p = 0; p < NUMPLAYERS; p++)
	if (pPlayer[p]->AufPlattform == this)
	{		
		// so bewegen wie die Plattform selber, wenn keine Wand im Weg
		//
		float x = pPlayer[p]->xpos;
		float y = pPlayer[p]->ypos;

		int br = pTileEngine->BlockRechts(x, y, x, y, pPlayer[p]->CollideRect);
		int bl = pTileEngine->BlockLinks (x, y, x, y, pPlayer[p]->CollideRect);

		if ((xSpeed > 0.0f && !(br & BLOCKWERT_WAND)) ||
			(xSpeed < 0.0f && !(bl & BLOCKWERT_WAND)))
			pPlayer[p]->xpos += xSpeed SYNC;

		pPlayer[p]->ypos  = yPos - pPlayer[p]->CollideRect.bottom + GegnerRect[GegnerArt].top + ySpeed SYNC;

		// Runtergefallen ?
		//
		if (pPlayer[p]->xpos + pPlayer[p]->CollideRect.left  > xPos + rect.right ||
			pPlayer[p]->xpos + pPlayer[p]->CollideRect.right < xPos + rect.left)
			pPlayer[p]->AufPlattform = NULL;
	}

	else
	if (pPlayer[p]->AufPlattform == NULL &&
		pPlayer[p]->Handlung != SACKREITEN &&
		pPlayer[p]->Handlung != DREHEN)
	{
		// Feststellen ob der Hurri auf die Plattform gesprungen ist
		//
		int   laenge;

		laenge = abs (int (pPlayer[p]->ypos - pPlayer[p]->yposold)) + 2;

		// TODO
		// eingestellt, weil man beim pharao boss am anfang nicht draufsteht, wenn er rauskommt
		// bringt das nachteile, wenn man das blockunten nicht abgfragt? bitte testen ;)
		if (pPlayer[p]->yspeed >= 0.0f)//       &&
			//!(pTileEngine->BlockUntenNormal(pPlayer->xpos, pPlayer->ypos, pPlayer->xposold, pPlayer->yposold, pPlayer->CollideRect) & BLOCKWERT_WAND) &&
			//!(pTileEngine->BlockUntenNormal(pPlayer->xpos, pPlayer->ypos, pPlayer->xposold, pPlayer->yposold, pPlayer->CollideRect) & BLOCKWERT_PLATTFORM))

		for (int i=0; i < laenge; i++)
		if (pPlayer[p]->yposold+pPlayer[p]->CollideRect.bottom+i+1 >= yPos + rect.top      &&
			pPlayer[p]->yposold+pPlayer[p]->CollideRect.bottom+i+1 <= yPos + rect.top + 10 &&
			pPlayer[p]->xpos + pPlayer[p]->CollideRect.left  <= xPos + rect.right &&
			pPlayer[p]->xpos + pPlayer[p]->CollideRect.right >= xPos + rect.left)
			pPlayer[p]->AufPlattform = this;
	}
}

// --------------------------------------------------------------------------------------
// Gegner schiebt Spieler beiseite
// --------------------------------------------------------------------------------------

void GegnerClass::Wegschieben(RECT rect, float dam)
{
	// Testen, ob der Spieler den Gegner berührt hat
	//
	for (int i = 0; i < NUMPLAYERS; i++)
	if (SpriteCollision(xPos, yPos, rect,
						pPlayer[i]->xpos, 
						pPlayer[i]->ypos, 
						pPlayer[i]->CollideRect) == true &&
		pPlayer[i]->AufPlattform != this)
	{
		// Spieler als Rad ? Dann abprallen
		if (pPlayer[i]->Handlung == RADELN ||
			pPlayer[i]->Handlung == RADELN_FALL)
		{
			if (pPlayer[i]->xpos < xPos) pPlayer[i]->Blickrichtung = -1;
			if (pPlayer[i]->xpos > xPos) pPlayer[i]->Blickrichtung =  1;
		}

		// Sonst Energie abziehen
		else
		{
			if (dam > 0.0f)
				pPlayer[i]->DamagePlayer(dam SYNC);

			// Spieler wegschieben
			if   (pPlayer[i]->xpos + 35 < xPos + rect.left + (rect.right - rect.left) / 2 &&
				!(pTileEngine->BlockLinks(pPlayer[i]->xpos, pPlayer[i]->ypos,
										  pPlayer[i]->xpos, pPlayer[i]->ypos,
										  pPlayer[i]->CollideRect) & BLOCKWERT_WAND))
				pPlayer[i]->xpos -= (PLAYER_MOVESPEED + 1) SYNC;

			if   (pPlayer[i]->xpos + 35 >= xPos + rect.left + (rect.right - rect.left) / 2 &&
				!(pTileEngine->BlockRechts(pPlayer[i]->xpos, pPlayer[i]->ypos,
										   pPlayer[i]->xpos, pPlayer[i]->ypos,
										   pPlayer[i]->CollideRect) & BLOCKWERT_WAND))				
				pPlayer[i]->xpos += (PLAYER_MOVESPEED + 1) SYNC;
		}
	}
}

// --------------------------------------------------------------------------------------
// Einfache Animation im Loop
// --------------------------------------------------------------------------------------

void GegnerClass::SimpleAnimation (bool backward)
{
	// Animieren
	if (AnimEnde > 0)						// Soll überhaupt animiert werden ?
	{
		AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
		if (AnimCount > AnimSpeed)			// Grenze überschritten ?
		{
			AnimCount = 0;					// Dann wieder auf Null setzen

			if (backward)
			{
				AnimPhase--;					// Und nächste Animationsphase
				if (AnimPhase <= AnimStart)		// Animation von zu Ende	?
					AnimPhase = AnimEnde;		// Dann wieder von vorne beginnen
			}
			else
			{
				AnimPhase++;					// Und nächste Animationsphase
				if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
					AnimPhase = AnimStart;		// Dann wieder von vorne beginnen
			}
		}
	} // animieren
} // SimpleAnmation

// --------------------------------------------------------------------------------------
// An der Wand umdrehen
// --------------------------------------------------------------------------------------

void GegnerClass::TurnonWall (void)
{
	if (((blockl & BLOCKWERT_WAND ||
		  blockl & BLOCKWERT_GEGNERWAND)  &&
		  xSpeed < 0.0f) ||
		((blockr & BLOCKWERT_WAND ||
		 blockr & BLOCKWERT_GEGNERWAND)  &&
		 xSpeed > 0.0f))
	{
		 xSpeed		   *= -1;
		 BlickRichtung *= -1;
	}

} // TurnonWall

// --------------------------------------------------------------------------------------
// Umdrehen, wenn angeschoßen
// --------------------------------------------------------------------------------------

bool GegnerClass::TurnonShot (void)
{
	if (TurnCount > 0.0f)
		TurnCount -= 1.0f SYNC;

	if (DamageTaken > 0 &&
		TurnCount <= 0.0f)
	{
		TurnCount = 20.0f;
		 if ((pPlayer[0]->xpos < xPos &&
			  BlickRichtung == RECHTS) ||
			 (pPlayer[0]->xpos > xPos &&
			  BlickRichtung == LINKS))
		 {
			 BlickRichtung *= -1;
			 xSpeed *= -1;
		 }

		 return true;
	}

	return false;

} // TurnonShot

// --------------------------------------------------------------------------------------
// Gegner grade sichtbar?
// --------------------------------------------------------------------------------------

bool GegnerClass::IsOnScreen(void)
{
	int off;
	int xsize, ysize;

	off = min(GegnerRect[GegnerArt].left, 0);

	xsize = pGegnerGrafix[GegnerArt]->itsXFrameSize;
	ysize = pGegnerGrafix[GegnerArt]->itsYFrameSize;

	if (GegnerArt == DRACHE ||
		GegnerArt == THEWALL)
	{
		xsize = 400;
		off = -400;
	}

		
	if (xPos + xsize < pTileEngine->XOffset ||
		xPos + off > pTileEngine->XOffset + 640.0f ||
		yPos + ysize < pTileEngine->YOffset ||
		yPos > pTileEngine->YOffset + 480.0f)
		return false;

	return true;
}

// --------------------------------------------------------------------------------------
// GegnerListClass Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor : Initialisierung der GegnerDaten
// --------------------------------------------------------------------------------------

GegnerListClass::GegnerListClass(void)
{
	pStart		= NULL;
	pEnd		= NULL;
	NumGegner	= 0;

	// Zu Beginn kein Gegner geladen (werden dann jeweils fürs Level geladen)
	for(int i=0; i<MAX_GEGNERGFX; i++)
		pGegnerGrafix[i] = NULL;

	// Gegner, die in jedem Level vorkommen, laden
	//
	pGegnerGrafix[POWERBLOCK]	= new DirectGraphicsSprite();
	pGegnerGrafix[ONEUP]		= new DirectGraphicsSprite();
	pGegnerGrafix[DIAMANT]		= new DirectGraphicsSprite();
	pGegnerGrafix[EXTRAS]		= new DirectGraphicsSprite();
	pGegnerGrafix[PUNISHER]		= new DirectGraphicsSprite();

	pGegnerGrafix[POWERBLOCK]->LoadImage("powerblock.png", 440, 40, 40, 40, 11, 1);
	pGegnerGrafix[ONEUP]->LoadImage		("oneup.png", 200, 160, 40, 40, 5, 4);
	pGegnerGrafix[DIAMANT]->LoadImage	("diamant.bmp", 261, 29, 29, 29, 9, 1);
	pGegnerGrafix[EXTRAS]->LoadImage	("extras.png", 312, 24, 24, 24, 13, 1); 
	pGegnerGrafix[PUNISHER]->LoadImage	("punisher.png", 1020, 850, 170, 170, 6, 5); 
	LavaFlare.LoadImage ("lavaflare.bmp", 120, 120, 120, 120, 1, 1);
	
	// Gegner-Rects festlegen
	// BonusBlock
	GegnerRect[POWERBLOCK].left = 0;	GegnerRect[POWERBLOCK].right  = 40;
	GegnerRect[POWERBLOCK].top  = 0;	GegnerRect[POWERBLOCK].bottom = 40;

	// Flamme der Drone laden
	//
	if (pDroneFlame == NULL)
	{
		pDroneFlame = new DirectGraphicsSprite();
		pDroneFlame->LoadImage("droneflame.bmp", 164, 46, 82, 46, 2, 1);
	}

	// Knarre der Zitrone laden
	//
	if (pDroneGun == NULL)
	{
		pDroneGun = new DirectGraphicsSprite();
		pDroneGun->LoadImage("Zitronestiel.png", 15, 63, 15, 63, 1, 1);
	}

	// Extra-Leben
	GegnerRect[ONEUP].left = 0;	GegnerRect[ONEUP].right  = 40;
	GegnerRect[ONEUP].top  = 0;	GegnerRect[ONEUP].bottom = 36;

	// Diamant
	GegnerRect[DIAMANT].left = 0;	GegnerRect[DIAMANT].right  = 29;
	GegnerRect[DIAMANT].top  = 0;	GegnerRect[DIAMANT].bottom = 29;

	// Extras laden
	GegnerRect[EXTRAS].left = 0;	GegnerRect[EXTRAS].right  = 24;
	GegnerRect[EXTRAS].top  = 0;	GegnerRect[EXTRAS].bottom = 20;

	// Spitter
	GegnerRect[SPITTER].left = 0;	GegnerRect[SPITTER].right  = 22;
	GegnerRect[SPITTER].top  = 0;	GegnerRect[SPITTER].bottom = 23;

	// Walker
	GegnerRect[WALKER].left = 0;	GegnerRect[WALKER].right  = 45;
	GegnerRect[WALKER].top  = 10;	GegnerRect[WALKER].bottom = 45;

	// Geschützturm
	GegnerRect[GUNTOWER].left = 0;	GegnerRect[GUNTOWER].right  = 60;
	GegnerRect[GUNTOWER].top  = 0;	GegnerRect[GUNTOWER].bottom = 40;

	// Spinnenbombe
	GegnerRect[SPIDERBOMB].left = 0;	GegnerRect[SPIDERBOMB].right  = 50;
	GegnerRect[SPIDERBOMB].top  = 0;	GegnerRect[SPIDERBOMB].bottom = 34;

	// Piranha
	GegnerRect[PIRANHA].left = 0;	GegnerRect[PIRANHA].right  = 45;
	GegnerRect[PIRANHA].top  = 0;	GegnerRect[PIRANHA].bottom = 29;

	// Stahlmuecke
	GegnerRect[STAHLMUECKE].left = 0;	GegnerRect[STAHLMUECKE].right  = 34;
	GegnerRect[STAHLMUECKE].top  = 0;	GegnerRect[STAHLMUECKE].bottom = 35;

	// Deckenturm
	GegnerRect[DECKENTURM].left = 0;	GegnerRect[DECKENTURM].right  = 38;
	GegnerRect[DECKENTURM].top  = 0;	GegnerRect[DECKENTURM].bottom = 40;

	// Deckenkrabbe
	GegnerRect[DECKENKRABBE].left = 20;	GegnerRect[DECKENKRABBE].right  = 50;
	GegnerRect[DECKENKRABBE].top  = 5;	GegnerRect[DECKENKRABBE].bottom = 34;

	// Stampfstein
	GegnerRect[STAMPFSTEIN].left = 2;	GegnerRect[STAMPFSTEIN].right  = 78;
	GegnerRect[STAMPFSTEIN].top  = 165;	GegnerRect[STAMPFSTEIN].bottom = 244;

	// Lavamann
	GegnerRect[LAVAMANN].left = 5;	GegnerRect[LAVAMANN].right  = 35;
	GegnerRect[LAVAMANN].top  = 5;	GegnerRect[LAVAMANN].bottom = 50;

	// Lavamann
	GegnerRect[PUNISHER].left = 40;	GegnerRect[PUNISHER].right  = 130;
	GegnerRect[PUNISHER].top  = 20;	GegnerRect[PUNISHER].bottom = 120;

	// Stachel-Kugel klein
	GegnerRect[KUGELKLEIN].left = 4;	GegnerRect[KUGELKLEIN].right  = 26;
	GegnerRect[KUGELKLEIN].top  = 4;	GegnerRect[KUGELKLEIN].bottom = 26;

	// Stachel-Kugel medium
	GegnerRect[KUGELMEDIUM].left = 8;	GegnerRect[KUGELMEDIUM].right  = 37;
	GegnerRect[KUGELMEDIUM].top  = 8;	GegnerRect[KUGELMEDIUM].bottom = 37;

	// Stachel-Kugel groß
	GegnerRect[KUGELGROSS].left = 12;	GegnerRect[KUGELGROSS].right  = 48;
	GegnerRect[KUGELGROSS].top  = 12;	GegnerRect[KUGELGROSS].bottom = 48;

	// Stachel-Kugel riesig
	GegnerRect[KUGELRIESIG].left = 16;	GegnerRect[KUGELRIESIG].right  = 74;
	GegnerRect[KUGELRIESIG].top  = 16;	GegnerRect[KUGELRIESIG].bottom = 74;

	// Deckenkrabbler
	GegnerRect[KRABBLEROBEN].left = 0;	GegnerRect[KRABBLEROBEN].right  = 50;
	GegnerRect[KRABBLEROBEN].top  = 0;	GegnerRect[KRABBLEROBEN].bottom = 75;

	// Wandkrabbler links
	GegnerRect[KRABBLERLINKS].left = 0;	GegnerRect[KRABBLERLINKS].right  = 40;
	GegnerRect[KRABBLERLINKS].top  = 10;GegnerRect[KRABBLERLINKS].bottom = 50;

	// Wandkrabbler rechts
	GegnerRect[KRABBLERRECHTS].left = 0;	GegnerRect[KRABBLERRECHTS].right  = 75;
	GegnerRect[KRABBLERRECHTS].top  = 0;	GegnerRect[KRABBLERRECHTS].bottom = 50;

	// Augen Extra
	GegnerRect[AUGE].left = 8;	GegnerRect[AUGE].right  = 52;
	GegnerRect[AUGE].top  = 0;	GegnerRect[AUGE].bottom = 26;

	// Qualle
	GegnerRect[QUALLE].left = 4;	GegnerRect[QUALLE].right  = 36;
	GegnerRect[QUALLE].top  = 8;	GegnerRect[QUALLE].bottom = 52;

	// Fallender Felsblock
	GegnerRect[FALLINGROCK].left = 0;	GegnerRect[FALLINGROCK].right  = 80;
	GegnerRect[FALLINGROCK].top  = 0;	GegnerRect[FALLINGROCK].bottom = 40;

	// Bröckelnder Felsblock
	GegnerRect[BROCKELROCK].left = 0;	GegnerRect[BROCKELROCK].right  = 80;
	GegnerRect[BROCKELROCK].top  = 0;	GegnerRect[BROCKELROCK].bottom = 40;

	// Roboraupe
	GegnerRect[ROBORAUPE].left = 20;	GegnerRect[ROBORAUPE].right  = 70;
	GegnerRect[ROBORAUPE].top  = 10;	GegnerRect[ROBORAUPE].bottom = 35;

	// Stalagtit
	GegnerRect[STALAGTIT].left = 0;	GegnerRect[STALAGTIT].right  = 40;
	GegnerRect[STALAGTIT].top  = 0;	GegnerRect[STALAGTIT].bottom = 80;

	// Raketenwerfer
	GegnerRect[RAKETENWERFER].left = 0;	GegnerRect[RAKETENWERFER].right  = 45;
	GegnerRect[RAKETENWERFER].top  = 0;	GegnerRect[RAKETENWERFER].bottom = 25;

	// SwimWalker
	GegnerRect[SWIMWALKER].left = 5;	GegnerRect[SWIMWALKER].right  = 60;
	GegnerRect[SWIMWALKER].top  = 5;	GegnerRect[SWIMWALKER].bottom = 40;

	// Flugsack
	GegnerRect[FLUGSACK].left = 5;	GegnerRect[FLUGSACK].right  = 75;
	GegnerRect[FLUGSACK].top  = 5;	GegnerRect[FLUGSACK].bottom = 65;

	// ReitFlugsack
	GegnerRect[REITFLUGSACK].left = 5;	GegnerRect[REITFLUGSACK].right  = 75;
	GegnerRect[REITFLUGSACK].top  = 5;	GegnerRect[REITFLUGSACK].bottom = 65;

	// EisStachel
	GegnerRect[EISSTACHEL].left = 20;	GegnerRect[EISSTACHEL].right  = 85;
	GegnerRect[EISSTACHEL].top  = 10;	GegnerRect[EISSTACHEL].bottom = 70;

	// SkiWalker
	GegnerRect[SKIWALKER].left = 0;		GegnerRect[SKIWALKER].right  = 65;
	GegnerRect[SKIWALKER].top  = 0;		GegnerRect[SKIWALKER].bottom = 38;

	// FlugKanone
	GegnerRect[FLUGKANONE].left = 5;	GegnerRect[FLUGKANONE].right  = 60;
	GegnerRect[FLUGKANONE].top  = 4;	GegnerRect[FLUGKANONE].bottom = 40;

	// SnowBombe
	GegnerRect[SNOWBOMBE].left = 25;	GegnerRect[SNOWBOMBE].right  = 55;
	GegnerRect[SNOWBOMBE].top  = 15;	GegnerRect[SNOWBOMBE].bottom = 65;

	// Roboman1
	GegnerRect[ROBOMAN1].left = 10;	GegnerRect[ROBOMAN1].right  = 80;
	GegnerRect[ROBOMAN1].top  = 0;	GegnerRect[ROBOMAN1].bottom = 100;

	// Boulder
	GegnerRect[BOULDER].left = 00;	GegnerRect[BOULDER].right  = 60;
	GegnerRect[BOULDER].top  = 5;	GegnerRect[BOULDER].bottom = 50;

	// StelzSack
	GegnerRect[STELZSACK].left = 20;	GegnerRect[STELZSACK].right  = 80;
	GegnerRect[STELZSACK].top  = 5;  	GegnerRect[STELZSACK].bottom = 60;

	// FallBome
	GegnerRect[FALLBOMBE].left = 0;	GegnerRect[FALLBOMBE].right  = 23;
	GegnerRect[FALLBOMBE].top  = 0;	GegnerRect[FALLBOMBE].bottom = 59;

	// PokeNuke
	GegnerRect[POKENUKE].left = 0;	GegnerRect[POKENUKE].right  = 81;
	GegnerRect[POKENUKE].top  = 0;	GegnerRect[POKENUKE].bottom = 217;

	// LavaKrabbe
	GegnerRect[LAVAKRABBE].left = 0;	GegnerRect[LAVAKRABBE].right  = 70;
	GegnerRect[LAVAKRABBE].top  = 0;	GegnerRect[LAVAKRABBE].bottom = 34;

	// LavaBallSpawner
	GegnerRect[LAVABALLSPAWNER].left = 0;	GegnerRect[LAVABALLSPAWNER].right  = 60;
	GegnerRect[LAVABALLSPAWNER].top  = 0;	GegnerRect[LAVABALLSPAWNER].bottom = 100;

	// LavaBall
	GegnerRect[LAVABALL].left = 15;	GegnerRect[LAVABALL].right  = 45;
	GegnerRect[LAVABALL].top  = 15;	GegnerRect[LAVABALL].bottom = 45;

	// Fette Spinne
	GegnerRect[FETTESPINNE].left = 4;	GegnerRect[FETTESPINNE].right  = 100;
	GegnerRect[FETTESPINNE].top  = 12;	GegnerRect[FETTESPINNE].bottom = 69;

	// Eiermann
	GegnerRect[EIERMANN].left = 10;	GegnerRect[EIERMANN].right  = 50;
	GegnerRect[EIERMANN].top  = 10;	GegnerRect[EIERMANN].bottom = 66;

	// Wand Kannone
	GegnerRect[WANDKANNONE].left = 0;	GegnerRect[WANDKANNONE].right  = 41;
	GegnerRect[WANDKANNONE].top  = 4;	GegnerRect[WANDKANNONE].bottom = 28;

	// Kleiner Stern
	GegnerRect[STARSMALL].left = 2;	GegnerRect[STARSMALL].right  = 38;
	GegnerRect[STARSMALL].top  = 2;	GegnerRect[STARSMALL].bottom = 38;

	// Großer Stern
	GegnerRect[STARBIG].left = 10;	GegnerRect[STARBIG].right  = 70;
	GegnerRect[STARBIG].top  = 10;	GegnerRect[STARBIG].bottom = 70;

	// Made
	GegnerRect[MADE].left = 2;	GegnerRect[MADE].right  = 18;
	GegnerRect[MADE].top  = 2;	GegnerRect[MADE].bottom = 10;

	// Drone
	GegnerRect[DRONE].left = 5;	GegnerRect[DRONE].right  = 65;
	GegnerRect[DRONE].top  = 5;	GegnerRect[DRONE].bottom = 75;

	// NeuFisch
	GegnerRect[NEUFISCH].left = 5;	GegnerRect[NEUFISCH].right  = 85;
	GegnerRect[NEUFISCH].top  = 5;	GegnerRect[NEUFISCH].bottom = 68;

	// Feuerpflanze
	GegnerRect[PFLANZE].left = 15;	GegnerRect[PFLANZE].right  = 95;
	GegnerRect[PFLANZE].top  = 20;	GegnerRect[PFLANZE].bottom = 90;

	// Sitterbombe
	GegnerRect[SPITTERBOMBE].left = 2;	GegnerRect[SPITTERBOMBE].right  = 27;
	GegnerRect[SPITTERBOMBE].top  = 0;	GegnerRect[SPITTERBOMBE].bottom = 50;

	// Wespennest
	GegnerRect[NEST].left = 8;	GegnerRect[NEST].right  = 52;
	GegnerRect[NEST].top  = 8;	GegnerRect[NEST].bottom = 60;

	// SchienenViech
	GegnerRect[SCHIENENVIECH].left = 0;	GegnerRect[SCHIENENVIECH].right  = 49;
	GegnerRect[SCHIENENVIECH].top  = 0;	GegnerRect[SCHIENENVIECH].bottom = 40;

	// WasserMine
	GegnerRect[WASSERMINE].left = 15;	GegnerRect[WASSERMINE].right  = 45;
	GegnerRect[WASSERMINE].top  = 10;	GegnerRect[WASSERMINE].bottom = 46;

	// Fledermaus
	GegnerRect[FLEDERMAUS].left = 4;	GegnerRect[FLEDERMAUS].right  = 46;
	GegnerRect[FLEDERMAUS].top  = 5;	GegnerRect[FLEDERMAUS].bottom = 45;

	// Kletter Spinner
	GegnerRect[CLIMBSPIDER].left = 5;	GegnerRect[CLIMBSPIDER].right  = 45;
	GegnerRect[CLIMBSPIDER].top  = 5;	GegnerRect[CLIMBSPIDER].bottom = 30;

	// Feuerspuckende Kletter Spinner
	GegnerRect[FIRESPIDER].left = 5;	GegnerRect[FIRESPIDER].right  = 75;
	GegnerRect[FIRESPIDER].top  = 5;	GegnerRect[FIRESPIDER].bottom = 50;

	// Die fette fliegende Ballerdrone
	GegnerRect[BALLERDRONE].left = 5;	GegnerRect[BALLERDRONE].right  = 85;
	GegnerRect[BALLERDRONE].top  = 10;	GegnerRect[BALLERDRONE].bottom = 60;

	// Der Madenkotzende Schwabbel
	GegnerRect[SCHWABBEL].left = 5;		GegnerRect[SCHWABBEL].right  = 55;
	GegnerRect[SCHWABBEL].top  = 20;	GegnerRect[SCHWABBEL].bottom = 60;

	// Die fette Rakete im Flugsack Level
	GegnerRect[BIGROCKET].left = 20;	GegnerRect[BIGROCKET].right  = 53;
	GegnerRect[BIGROCKET].top  = 10;	GegnerRect[BIGROCKET].bottom = 290;

	// Der runterfallende Eiszapfen
	GegnerRect[EISZAPFEN].left = 0;	GegnerRect[EISZAPFEN].right  = 30;
	GegnerRect[EISZAPFEN].top  = 0;	GegnerRect[EISZAPFEN].bottom = 80;

	// Die Wandkrabbe
	GegnerRect[WANDKRABBE].left = 0;	GegnerRect[WANDKRABBE].right  = 32;
	GegnerRect[WANDKRABBE].top  = 0;	GegnerRect[WANDKRABBE].bottom = 70;

	// Der Abfangjaeger
	GegnerRect[JAEGER].left = 5;	GegnerRect[JAEGER].right  = 65;
	GegnerRect[JAEGER].top  = 5;	GegnerRect[JAEGER].bottom = 48;

	// Die Rakete an der Wand
	GegnerRect[SIDEROCKET].left = 2;	GegnerRect[SIDEROCKET].right  = 38;
	GegnerRect[SIDEROCKET].top  = 2;	GegnerRect[SIDEROCKET].bottom = 16;

	// Die kleine fiese Drone
	GegnerRect[FIESEDRONE].left = 4;	GegnerRect[FIESEDRONE].right  = 46;
	GegnerRect[FIESEDRONE].top  = 4;	GegnerRect[FIESEDRONE].bottom = 46;

	// Der Schleimboller
	GegnerRect[SCHLEIMBOLLER].left = 15;	GegnerRect[SCHLEIMBOLLER].right  = 45;
	GegnerRect[SCHLEIMBOLLER].top  = 15;	GegnerRect[SCHLEIMBOLLER].bottom = 55;

	// Der Alien Schleimboller
	GegnerRect[SCHLEIMALIEN].left = 10;	GegnerRect[SCHLEIMALIEN].right  = 50;
	GegnerRect[SCHLEIMALIEN].top  = 10;	GegnerRect[SCHLEIMALIEN].bottom = 50;

	// Der Schleimboller mit Maul
	GegnerRect[SCHLEIMMAUL].left = 10;	GegnerRect[SCHLEIMMAUL].right  = 58;
	GegnerRect[SCHLEIMMAUL].top  = 10;	GegnerRect[SCHLEIMMAUL].bottom = 58;

	// Der fiese Walker mit Laser
	GegnerRect[FIESERWALKER].left = 5;	GegnerRect[FIESERWALKER].right  = 59;
	GegnerRect[FIESERWALKER].top  = 8;	GegnerRect[FIESERWALKER].bottom = 55;

	// Der fiese Walker mit Flammenwerfer
	GegnerRect[FIESERWALKER2].left = 5;	GegnerRect[FIESERWALKER2].right  = 59;
	GegnerRect[FIESERWALKER2].top  = 8;	GegnerRect[FIESERWALKER2].bottom = 55;

	// Die mittelgroße Spinne 
	GegnerRect[MITTELSPINNE].left = 10;	GegnerRect[MITTELSPINNE].right  = 97;
	GegnerRect[MITTELSPINNE].top  = 10;	GegnerRect[MITTELSPINNE].bottom = 76;

	// Die Wespe
	GegnerRect[SMALLWESPE].left = 0;	GegnerRect[SMALLWESPE].right  = 50;
	GegnerRect[SMALLWESPE].top  = 15;	GegnerRect[SMALLWESPE].bottom = 40;

	// Riesen Piranha
	GegnerRect[RIESENPIRANHA].left = 24;	GegnerRect[RIESENPIRANHA].right  = 256-24;
	GegnerRect[RIESENPIRANHA].top  = 24;	GegnerRect[RIESENPIRANHA].bottom = 210-24;

	// Riesen Qualle
	GegnerRect[RIESENQUALLE].left = 20;	GegnerRect[RIESENQUALLE].right  = 110;
	GegnerRect[RIESENQUALLE].top  = 4;	GegnerRect[RIESENQUALLE].bottom = 200;

	// Riesen Raupe
	GegnerRect[RIESENRAUPE].left = 30;	GegnerRect[RIESENRAUPE].right  = 150;
	GegnerRect[RIESENRAUPE].top  = 20;	GegnerRect[RIESENRAUPE].bottom = 58;

	// Riesen Wespe
	GegnerRect[RIESENWASP].left = 10;	GegnerRect[RIESENWASP]
		.right  = 106;
	GegnerRect[RIESENWASP].top  = 50;	GegnerRect[RIESENWASP].bottom = 100;

	// Stahlfaust
	GegnerRect[STAHLFAUST].left = 35;	GegnerRect[STAHLFAUST].right  = 235;
	GegnerRect[STAHLFAUST].top  = 60;	GegnerRect[STAHLFAUST].bottom = 256;

	// Pharaokopf
	GegnerRect[PHARAOKOPF].left = 0;	GegnerRect[PHARAOKOPF].right  = 224;
	GegnerRect[PHARAOKOPF].top  = 0;	GegnerRect[PHARAOKOPF].bottom = 224;

	// Riesenspinne
	GegnerRect[RIESENSPINNE].left = 60;	GegnerRect[RIESENSPINNE].right  = 360;
	GegnerRect[RIESENSPINNE].top  = 40;	GegnerRect[RIESENSPINNE].bottom = 90;

	// EvilHurri
	GegnerRect[EVILHURRI].left = 10;	GegnerRect[EVILHURRI].right  = 60;
	GegnerRect[EVILHURRI].top  = 20;	GegnerRect[EVILHURRI].bottom = 80;

	// Ufo
	GegnerRect[UFO].left = 10;	GegnerRect[UFO].right  = 190;
	GegnerRect[UFO].top  = 32;	GegnerRect[UFO].bottom = 96;

	// Fahrstuhlboss
	GegnerRect[FAHRSTUHLBOSS].left = 0;	GegnerRect[FAHRSTUHLBOSS].right  = 34;
	GegnerRect[FAHRSTUHLBOSS].top  = 0;	GegnerRect[FAHRSTUHLBOSS].bottom = 34;

	// FlugBoss
	GegnerRect[FLUGBOSS].left = 175; GegnerRect[FLUGBOSS].right  = 175 + 29;
	GegnerRect[FLUGBOSS].top  = 182; GegnerRect[FLUGBOSS].bottom = 182 + 30;

	// Bratklops
	GegnerRect[BRATKLOPS].left = 146; GegnerRect[BRATKLOPS].right  = 146 + 20;
	GegnerRect[BRATKLOPS].top  = 186; GegnerRect[BRATKLOPS].bottom = 186 + 60;

	// MetalHead
	GegnerRect[METALHEAD].left = 20;	   GegnerRect[METALHEAD].right  = 130;
	GegnerRect[METALHEAD].top  = 20 + 113; GegnerRect[METALHEAD].bottom = 130 + 82;	

	// Endboss
	GegnerRect[SKELETOR].left = 40;	GegnerRect[SKELETOR].right  = 85;
	GegnerRect[SKELETOR].top  = 20; GegnerRect[SKELETOR].bottom = 200;

	// Schädel vom Boss
	GegnerRect[SKULL].left = 4;	GegnerRect[SKULL].right  = 28;
	GegnerRect[SKULL].top  = 4;	GegnerRect[SKULL].bottom = 37;

	// Eis Stahlfaust
	GegnerRect[EISFAUST].left = 35;	GegnerRect[EISFAUST].right  = 235;
	GegnerRect[EISFAUST].top  = 60;	GegnerRect[EISFAUST].bottom = 256;

	// PartikelSpawner Trigger
	GegnerRect[PARTIKELSPAWN].left = 0;	GegnerRect[PARTIKELSPAWN].right  = 40;
	GegnerRect[PARTIKELSPAWN].top  = 0;	GegnerRect[PARTIKELSPAWN].bottom = 40;

	// T.R.Schmidts Gegner Sturm
	GegnerRect[GEGNERSTURM].left = 0;	GegnerRect[GEGNERSTURM].right  = 40;
	GegnerRect[GEGNERSTURM].top  = 0;	GegnerRect[GEGNERSTURM].bottom = 40;

	// Fahrstuhl
	GegnerRect[FAHRSTUHL].left = 0;	GegnerRect[FAHRSTUHL].right  = 400;
	GegnerRect[FAHRSTUHL].top  = 0;	GegnerRect[FAHRSTUHL].bottom = 150;

	// Fass
	GegnerRect[FASS].left = 0;	GegnerRect[FASS].right  = 48;
	GegnerRect[FASS].top  = 0;	GegnerRect[FASS].bottom = 56;

	// Flammenwerfer an der Wand bzw im Boden oder an der Decke
	GegnerRect[FEUERSPUCKER].left = 0;	GegnerRect[FEUERSPUCKER].right  = 60;
	GegnerRect[FEUERSPUCKER].top  = 0;	GegnerRect[FEUERSPUCKER].bottom = 60;

	// Fade Music Trigger
	GegnerRect[FADEMUSIC].left = 0;	GegnerRect[FADEMUSIC].right  = 40;
	GegnerRect[FADEMUSIC].top  = 0;	GegnerRect[FADEMUSIC].bottom = 40;

	// Level Ende Trigger
	GegnerRect[ENDLEVEL].left = 0;	GegnerRect[ENDLEVEL].right  = 120;
	GegnerRect[ENDLEVEL].top  = 0;	GegnerRect[ENDLEVEL].bottom = 120;

	// Plattform
	GegnerRect[PLATTFORM].left = 0;	GegnerRect[PLATTFORM].right  = 128;
	GegnerRect[PLATTFORM].top  = 0;	GegnerRect[PLATTFORM].bottom = 32;

	// Presse
	GegnerRect[PRESSE].left = 0;    GegnerRect[PRESSE].right  = 100;
	GegnerRect[PRESSE].top  = 236;	GegnerRect[PRESSE].bottom = 256;

	// Shrine
	GegnerRect[SHRINE].left = 20;   GegnerRect[SHRINE].right  = 100;
	GegnerRect[SHRINE].top  = 80;	GegnerRect[SHRINE].bottom = 180;

	// Bruecke
	GegnerRect[BRUECKE].left = 0;   GegnerRect[BRUECKE].right  = 10;
	GegnerRect[BRUECKE].top  = 0;	GegnerRect[BRUECKE].bottom = 10;

	// Floating
	GegnerRect[FLOATING].left = 0;  GegnerRect[FLOATING].right  = 39;
	GegnerRect[FLOATING].top  = 0;	GegnerRect[FLOATING].bottom = 14;

	// Floating2
	GegnerRect[FLOATING2].left = 0; GegnerRect[FLOATING2].right  = 110;
	GegnerRect[FLOATING2].top  = 0;	GegnerRect[FLOATING2].bottom = 18;

	// Floating3
	GegnerRect[FLOATING3].left = 0; GegnerRect[FLOATING3].right  = 100;
	GegnerRect[FLOATING3].top  = 0;	GegnerRect[FLOATING3].bottom = 27;

	// Surfbrett
	GegnerRect[SURFBRETT].left = 0;  GegnerRect[SURFBRETT].right  = 52;
	GegnerRect[SURFBRETT].top  = 12; GegnerRect[SURFBRETT].bottom = 35;

	// ShootButton
	GegnerRect[SHOOTBUTTON].left = 0; GegnerRect[SHOOTBUTTON].right  = 16;
	GegnerRect[SHOOTBUTTON].top  = 0; GegnerRect[SHOOTBUTTON].bottom = 9;

	// ShootPlattform
	GegnerRect[SHOOTPLATTFORM].left = 0; GegnerRect[SHOOTPLATTFORM].right  = 100;
	GegnerRect[SHOOTPLATTFORM].top  = 0; GegnerRect[SHOOTPLATTFORM].bottom = 20;

	// Glubschauge
	GegnerRect[GLUBSCHI].left = 0; GegnerRect[GLUBSCHI].right  = 51;
	GegnerRect[GLUBSCHI].top  = 0; GegnerRect[GLUBSCHI].bottom = 120;

	// Glubschauge von der Decke
	GegnerRect[GLUBSCHI2].left = 0; GegnerRect[GLUBSCHI2].right  = 51;
	GegnerRect[GLUBSCHI2].top  = 0; GegnerRect[GLUBSCHI2].bottom = 120;

	// Column
	GegnerRect[COLUMN].left = 0; GegnerRect[COLUMN].right  = 40;
	GegnerRect[COLUMN].top  = 0; GegnerRect[COLUMN].bottom = 100;

	// Column2
	GegnerRect[COLUMN2].left = 0; GegnerRect[COLUMN2].right  = 40;
	GegnerRect[COLUMN2].top  = 0; GegnerRect[COLUMN2].bottom = 100;

	// Lift
	GegnerRect[LIFT].left = 0; GegnerRect[LIFT].right  = 80;
	GegnerRect[LIFT].top  = 0; GegnerRect[LIFT].bottom = 23;

	// Schleuse horizontal
	GegnerRect[SCHLEUSEH].left = 0; GegnerRect[SCHLEUSEH].right  = 200;
	GegnerRect[SCHLEUSEH].top  = 0; GegnerRect[SCHLEUSEH].bottom = 40;

	// Schleuse vertikal
	GegnerRect[SCHLEUSEV].left = 0; GegnerRect[SCHLEUSEV].right  = 40;
	GegnerRect[SCHLEUSEV].top  = 0; GegnerRect[SCHLEUSEV].bottom = 200;

	// Schalter
	GegnerRect[SWITCH].left = 0; GegnerRect[SWITCH].right  = 24;
	GegnerRect[SWITCH].top  = 0; GegnerRect[SWITCH].bottom = 32;

	// Warning Schild
	GegnerRect[WARNING].left = 0; GegnerRect[WARNING].right  = 180;
	GegnerRect[WARNING].top  = 0; GegnerRect[WARNING].bottom = 40;

	// Luefter Groß
	GegnerRect[LUEFTER_GROSS].left = 0; GegnerRect[LUEFTER_GROSS].right  = 224;
	GegnerRect[LUEFTER_GROSS].top  = 0; GegnerRect[LUEFTER_GROSS].bottom = 223;

	// Luefter Klein 1
	GegnerRect[LUEFTER_KLEIN].left = 0; GegnerRect[LUEFTER_KLEIN].right  = 112;
	GegnerRect[LUEFTER_KLEIN].top  = 0; GegnerRect[LUEFTER_KLEIN].bottom = 113;

	// Luefter Klein 2
	GegnerRect[LUEFTER_KLEIN2].left = 0; GegnerRect[LUEFTER_KLEIN2].right  = 112;
	GegnerRect[LUEFTER_KLEIN2].top  = 0; GegnerRect[LUEFTER_KLEIN2].bottom = 113;

	// LightFlare
	GegnerRect[LIGHTFLARE].left = 0; GegnerRect[LIGHTFLARE].right  = 120;
	GegnerRect[LIGHTFLARE].top  = 0; GegnerRect[LIGHTFLARE].bottom = 120;

	// Secret
	GegnerRect[SECRET].left = 0; GegnerRect[SECRET].right  = 40;
	GegnerRect[SECRET].top  = 0; GegnerRect[SECRET].bottom = 40;

	// Pilzkopf zum draufhopsem
	GegnerRect[MUSHROOM].left = 30; GegnerRect[MUSHROOM].right  = 110;
	GegnerRect[MUSHROOM].top  = 20; GegnerRect[MUSHROOM].bottom = 40;

	// Presswurst
	GegnerRect[PRESSWURST].left = 28; GegnerRect[PRESSWURST].right  = 185 - 28;
	GegnerRect[PRESSWURST].top  = 251; GegnerRect[PRESSWURST].bottom = 357;

	// Golem Boss
	GegnerRect[GOLEM].left = 42; GegnerRect[GOLEM].right  = 138;
	GegnerRect[GOLEM].top  = 29; GegnerRect[GOLEM].bottom = 300;

	// Spinnenmaschine
	GegnerRect[SPINNENMASCHINE].left = 0;	GegnerRect[SPINNENMASCHINE].right  = 400;
	GegnerRect[SPINNENMASCHINE].top  = 0;	GegnerRect[SPINNENMASCHINE].bottom = 300;

	// TheWall
	GegnerRect[THEWALL].left = -1000;	GegnerRect[THEWALL].right  = 285;
	GegnerRect[THEWALL].top  = 0;	GegnerRect[THEWALL].bottom = 480;

	// LaFass
	GegnerRect[LAFASS].left = 10;	GegnerRect[LAFASS].right  = 54;
	GegnerRect[LAFASS].top  = 50;	GegnerRect[LAFASS].bottom = 155;

	// Geschuetz
	GegnerRect[GESCHUETZ].left = 10;	GegnerRect[GESCHUETZ].right  = 50;
	GegnerRect[GESCHUETZ].top  = 0;		GegnerRect[GESCHUETZ].bottom = 40;

	// Fette Rakete
	GegnerRect[FETTERAKETE].left = 0;	GegnerRect[FETTERAKETE].right  = 25;
	GegnerRect[FETTERAKETE].top  = 4;	GegnerRect[FETTERAKETE].bottom = 30;

	// Minirocket
	GegnerRect[MINIROCKET].left = 0;	GegnerRect[MINIROCKET].right  = 12;
	GegnerRect[MINIROCKET].top  = 0;	GegnerRect[MINIROCKET].bottom = 12;

	// Fette Schabe
	GegnerRect[SCHABE].left = 0;	GegnerRect[SCHABE].right  = 20;
	GegnerRect[SCHABE].top  = 0;	GegnerRect[SCHABE].bottom = 20;

	// Mini Drache
	GegnerRect[MINIDRAGON].left = 0;	GegnerRect[MINIDRAGON].right  = 60;
	GegnerRect[MINIDRAGON].top  = 10;	GegnerRect[MINIDRAGON].bottom = 37;

	// Spikelift
	GegnerRect[SPIKELIFT].left = 0;		GegnerRect[SPIKELIFT].right  = 460;
	GegnerRect[SPIKELIFT].top  = 40;	GegnerRect[SPIKELIFT].bottom = 80;

	// Drache
	GegnerRect[DRACHE].left = 40;	GegnerRect[DRACHE].right  = 201;
	GegnerRect[DRACHE].top  = 0;	GegnerRect[DRACHE].bottom = 135;

	// Rollmops
	GegnerRect[ROLLMOPS].left = 20;	GegnerRect[ROLLMOPS].right  = 80;
	GegnerRect[ROLLMOPS].top  = 20;	GegnerRect[ROLLMOPS].bottom = 80;

	// SchneeKönig
	GegnerRect[SCHNEEKOENIG].left = 60;	GegnerRect[SCHNEEKOENIG].right  = 145;
	GegnerRect[SCHNEEKOENIG].top  = 50;	GegnerRect[SCHNEEKOENIG].bottom = 120;

	// SchneeKoppe
	GegnerRect[SCHNEEKOPPE].left = 0;	GegnerRect[SCHNEEKOPPE].right  = 9;
	GegnerRect[SCHNEEKOPPE].top  = 8;	GegnerRect[SCHNEEKOPPE].bottom = 20;

	// SchneeKoppe
	GegnerRect[ZITRONE].left = 10;	GegnerRect[ZITRONE].right  = 105;
	GegnerRect[ZITRONE].top  = 10;	GegnerRect[ZITRONE].bottom = 71;

	// Rotzpott
	GegnerRect[ROTZPOTT].left = 8;	GegnerRect[ROTZPOTT].right  = 47;
	GegnerRect[ROTZPOTT].top  = 8;	GegnerRect[ROTZPOTT].bottom = 41;

	// RiesenPiranha
	GegnerRect[BIGFISH].left = 0;	GegnerRect[BIGFISH].right  = 17;
	GegnerRect[BIGFISH].top  = 48;	GegnerRect[BIGFISH].bottom = 70;

	// Kettenglied
	GegnerRect[KETTENGLIED].left = 2;	GegnerRect[KETTENGLIED].right  = 16;
	GegnerRect[KETTENGLIED].top  = 2;	GegnerRect[KETTENGLIED].bottom = 16;

	// Stachelbeere
	GegnerRect[STACHELBEERE].left = 10;	GegnerRect[STACHELBEERE].right  = 50;
	GegnerRect[STACHELBEERE].top  = 5;	GegnerRect[STACHELBEERE].bottom = 50;

	// Tube
	GegnerRect[TUBE].left = 0;	GegnerRect[TUBE].right  = 102;
	GegnerRect[TUBE].top  = 0;	GegnerRect[TUBE].bottom = 238;

	// Mutant
	GegnerRect[MUTANT].left = 10;	GegnerRect[MUTANT].right  = 90;
	GegnerRect[MUTANT].top  = 60;	GegnerRect[MUTANT].bottom = 85;
}

// --------------------------------------------------------------------------------------
// Destruktor : Löschen der ganzen Liste und Freigabe der Gegner-Grafiken
// --------------------------------------------------------------------------------------

GegnerListClass::~GegnerListClass(void)
{
	// Gegner-Liste komplett leeren
	ClearAll();

	// Gegner Grafiken freigeben
	for(int i=0; i<MAX_GEGNERGFX; i++)
		if(pGegnerGrafix[i] != NULL)
		{
			delete(pGegnerGrafix[i]);
			pGegnerGrafix[i] = NULL;
		}

	delete(pDroneFlame);
	pDroneFlame = NULL;

	delete(pDroneGun);
	pDroneGun = NULL;
}

// --------------------------------------------------------------------------------------
// Gegner "Art" hinzufügen
// --------------------------------------------------------------------------------------

bool GegnerListClass::PushGegner(float x, float y, int Art, int Value1, int Value2, bool Light, bool atEnd)
{
	GegnerClass *pNew;							// Das wird der neue Gegner;

	if(NumGegner >= MAX_GEGNER)					// Grenze überschritten ?
		return false;

	// Unterscheiden, was für ein Gegner erstellt wird
	switch(Art)
	{
		case POWERBLOCK:
		{
			pNew = new GegnerPowerBlock(Value1, Value2, Light);
		} break;

		case ONEUP:
		{
			pNew = new GegnerOneUp(Value1, Value2, Light);
		} break;

		case DIAMANT:
		{
			pNew = new GegnerDiamant(Value1, Value2, Light);
		} break;

		case EXTRAS:
		{
			pNew = new GegnerExtras(Value1, Value2, Light);
		} break;

		case SPITTER:
		{
			pNew = new GegnerSpitter(Value1, Value2, Light);
		} break;

		case WALKER:
		{
			pNew = new GegnerWalker(Value1, Value2, Light);
		} break;

		case GUNTOWER:
		{
			pNew = new GegnerGunTower(Value1, Value2, Light);
		} break;

		case SPIDERBOMB:
		{
			pNew = new GegnerSpiderBomb(Value1, Value2, Light);
		} break;

		case PIRANHA:
		{
			pNew = new GegnerPiranha(Value1, Value2, Light);
		} break;

		case STAHLMUECKE:
		{
			pNew = new GegnerStahlmuecke(Value1, Value2, Light);
		} break;

		case DECKENTURM:
		{
			pNew = new GegnerDeckenturm(Value1, Value2, Light);
		} break;

		case DECKENKRABBE:
		{
			pNew = new GegnerDeckenKrabbe(Value1, Value2, Light);
		} break;

		case KUGELKLEIN:
		{
			pNew = new GegnerKugelKlein(Value1, Value2, Light);
		} break;

		case KUGELMEDIUM:
		{
			pNew = new GegnerKugelMedium(Value1, Value2, Light);
		} break;

		case KUGELGROSS:
		{
			pNew = new GegnerKugelGross(Value1, Value2, Light);
		} break;

		case KUGELRIESIG:
		{
			pNew = new GegnerKugelRiesig(Value1, Value2, Light);
		} break;

		case KRABBLEROBEN:
		{
			pNew = new GegnerKrabblerOben(Value1, Value2, Light);
		} break;

		case KRABBLERLINKS:
		{
			pNew = new GegnerKrabblerLinks(Value1, Value2, Light);
		} break;

		case KRABBLERRECHTS:
		{
			pNew = new GegnerKrabblerRechts(Value1, Value2, Light);
		} break;

		case AUGE:
		{
			if (Value2 != 1)
				pNew = new GegnerAuge(Value1, Value2, Light);
			else
			{
				pNew = new GegnerExtras(Value1, Value2, Light);
				Art = EXTRAS;
			}
		} break;

		case QUALLE:
		{
			pNew = new GegnerQualle(Value1, Value2, Light);
		} break;

		case FALLINGROCK:
		{
			pNew = new GegnerFallingRock(Value1, Value2, Light);
		} break;

		case BROCKELROCK:
		{
			pNew = new GegnerBrockelRock(Value1, Value2, Light);
		} break;

		case ROBORAUPE:
		{
			pNew = new GegnerRoboRaupe(Value1, Value2, Light);
		} break;

		case STALAGTIT:
		{
			pNew = new GegnerStalagtit(Value1, Value2, Light);
		} break;

		case RAKETENWERFER:
		{
			pNew = new GegnerRaketenwerfer(Value1, Value2, Light);
		} break;

		case SWIMWALKER:
		{
			pNew = new GegnerSwimWalker(Value1, Value2, Light);
		} break;

		case FLUGSACK:
		{
			pNew = new GegnerFlugsack(Value1, Value2, Light);
		} break;

		case REITFLUGSACK:
		{
			pNew = new GegnerReitFlugsack(Value1, Value2, Light);
		} break;

		case EISSTACHEL:
		{
			pNew = new GegnerEisStachel(Value1, Value2, Light);
		} break;

		case SKIWALKER:
		{
			pNew = new GegnerSkiWalker(Value1, Value2, Light);
		} break;

		case FLUGKANONE:
		{
			pNew = new GegnerFlugKanone(Value1, Value2, Light);
		} break;

		case SNOWBOMBE:
		{
			pNew = new GegnerSnowBomb(Value1, Value2, Light);
		} break;

		case ROBOMAN1:
		{
			pNew = new GegnerRoboMan1(Value1, Value2, Light);
		} break;

		case BOULDER:
		{
			pNew = new GegnerBlueBoulder(Value1, Value2, Light);
		} break;

		case STELZSACK:
		{
			pNew = new GegnerStelzSack(Value1, Value2, Light);
		} break;

		case FALLBOMBE:
		{
			pNew = new GegnerFallBombe(Value1, Value2, Light);
		} break;

		case POKENUKE:
		{
			pNew = new GegnerPokeNuke(Value1, Value2, Light);
		} break;

		case LAVAKRABBE:
		{
			pNew = new GegnerLavaKrabbe(Value1, Value2, Light);
		} break;

		case LAVABALL:
		{
			pNew = new GegnerLavaBall(Value1, Value2, Light);
		} break;

		case FETTESPINNE:
		{
			pNew = new GegnerFetteSpinne(Value1, Value2, Light);
		} break;

		case EIERMANN:
		{
			pNew = new GegnerEierMann(Value1, Value2, Light);
		} break;

		case WANDKANNONE:
		{
			pNew = new GegnerWandKannone(Value1, Value2, Light);
		} break;

		case STARSMALL:
		{
			pNew = new GegnerStarSmall(Value1, Value2, Light);
		} break;

		case STARBIG:
		{
			pNew = new GegnerStarBig(Value1, Value2, Light);
		} break;

		case MADE:
		{
			pNew = new GegnerMade(Value1, Value2, Light);
		} break;

		case DRONE:
		{
			pNew = new GegnerDrone(Value1, Value2, Light);
		} break;

		case NEUFISCH:
		{
			pNew = new GegnerNeuFisch(Value1, Value2, Light);
		} break;

		case PFLANZE:
		{
			pNew = new GegnerPflanze(Value1, Value2, Light);
		} break;

		case SPITTERBOMBE:
		{
			pNew = new GegnerSpitterbombe(Value1, Value2, Light);
		} break;

		case NEST:
		{
			pNew = new GegnerNest(Value1, Value2, Light);
		} break;

		case SCHIENENVIECH:
		{
			pNew = new GegnerSchienenViech(Value1, Value2, Light);
		} break;

		case WASSERMINE:
		{
			pNew = new GegnerWasserMine(int (y), Value2, Light);
		} break;

		case FLEDERMAUS:
		{
			pNew = new GegnerFledermaus(int (y), Value2, Light);
		} break;

		case CLIMBSPIDER:
		{
			pNew = new GegnerClimber(Value1, Value2, Light);
		} break;

		case FIRESPIDER:
		{
			pNew = new GegnerFireSpider(Value1, Value2, Light);
		} break;

		case BALLERDRONE:
		{
			pNew = new GegnerBallerdrone(Value1, Value2, Light);
		} break;

		case SCHWABBEL:
		{
			pNew = new GegnerSchwabbel(Value1, Value2, Light);
		} break;

		case BIGROCKET:
		{
			pNew = new GegnerBigRocket(Value1, Value2, Light);
		} break;

		case EISZAPFEN:
		{
			pNew = new GegnerEiszapfen(Value1, Value2, Light);
		} break;

		case WANDKRABBE:
		{
			pNew = new GegnerWandKrabbe(Value1, Value2, Light);
		} break;

		case JAEGER:
		{
			pNew = new GegnerJaeger(Value1, Value2, Light);
		} break;

		case SIDEROCKET:
		{
			pNew = new GegnerSideRocket(Value1, Value2, Light);
		} break;

		case FIESEDRONE:
		{
			pNew = new GegnerFieseDrone(Value1, Value2, Light);
		} break;

		case SCHLEIMBOLLER:
		{
			pNew = new GegnerSchleimBoller(Value1, Value2, Light);
		} break;

		case SCHLEIMALIEN:
		{
			pNew = new GegnerSchleimAlien(Value1, Value2, Light);
		} break;

		case SCHLEIMMAUL:
		{
			pNew = new GegnerSchleimMaul(x, y, Value1, Value2, Light);
		} break;

		case FIESERWALKER:
		{
			pNew = new GegnerFieserWalker(Value1, Value2, Light);
		} break;

		case FIESERWALKER2:
		{
			pNew = new GegnerFieserFireWalker(Value1, Value2, Light);
		} break;

		case MITTELSPINNE:
		{
			pNew = new GegnerMittelSpinne(Value1, Value2, Light);
		} break;

		case SMALLWESPE:
		{
			pNew = new GegnerSmallWespe(Value1, Value2, Light);
		} break;

		case STAMPFSTEIN:
		{
			pNew = new GegnerStampfstein(Value1, Value2, Light);
		} break;

		case LAVAMANN:
		{
			pNew = new GegnerLavamann(Value1, Value2, Light);
		} break;

		case PUNISHER:
		{
			pNew = new GegnerPunisher(Value1, Value2, Light);
		} break;

		case LAVABALLSPAWNER:
		{
			pNew = new GegnerLavaBallSpawner(Value1, Value2, Light);
		} break;

		case RIESENPIRANHA:
		{
			pNew = new GegnerRiesenPiranha(Value1, Value2, Light);
		} break;	

		case RIESENQUALLE:
		{
			pNew = new GegnerRiesenQualle(Value1, Value2, Light);
		} break;	

		case RIESENRAUPE:
		{
			pNew = new GegnerRiesenRaupe(Value1, Value2, Light);
		} break;	

		case RIESENWASP:
		{
			pNew = new GegnerRiesenWasp(Value1, Value2, Light);
		} break;	

		case PARTIKELSPAWN:
		{
			pNew = new GegnerPartikelSpawner(Value1, Value2, Light);
		} break;

		case GEGNERSTURM:
		{
			pNew = new GegnerSchmidtOrgie(Value1, Value2, Light);
		} break;

		case METALHEAD:
		{
			pNew = new GegnerMetalHead(Value1, Value2, Light);
		} break;

		case SKELETOR:
		{
			pNew = new GegnerSkeletor(Value1, Value2, Light);
		} break;

		case SKULL:
		{
			pNew = new GegnerSkull(Value1, Value2, Light);
		} break;

		case STAHLFAUST:
		{
			pNew = new GegnerStahlfaust(Value1, Value2, Light);
		} break;

		case EISFAUST:
		{
			pNew = new GegnerEisFaust(Value1, Value2, Light);
		} break;

		case WUXESPINNEN:
		{
			pNew = new GegnerWuxeSpinnen(Value1, Value2, Light);
		} break;

		case PHARAOKOPF:
		{
			pNew = new GegnerPharaoKopf(Value1, Value2, Light);
		} break;

		case RIESENSPINNE:
		{
			pNew = new GegnerRiesenSpinne(Value1, Value2, Light);
		} break;

		case EVILHURRI:
		{
			pNew = new GegnerEvilHurri(Value1, Value2, Light);
		} break;

		case UFO:
		{
			pNew = new GegnerUfo(Value1, Value2, Light);
		} break;

		case FAHRSTUHLBOSS:
		{
			pNew = new GegnerFahrstuhlBoss(Value1, Value2, Light);
		} break;

		case FLUGBOSS:
		{
			pNew = new GegnerFlugBoss(Value1, Value2, Light);
		} break;

		case BRATKLOPS:
		{
			pNew = new GegnerBratklops(Value1, Value2, Light);
		} break;

		case FAHRSTUHL:
		{
			pNew = new GegnerFahrstuhl(Value1, Value2, Light);
		} break;

		case FASS:
		{
			pNew = new GegnerFass(Value1, Value2, Light);
		} break;

		case FEUERSPUCKER:
		{
			pNew = new GegnerFeuerspucker(Value1, Value2, Light);
		} break;

		case FADEMUSIC:
		{
			pNew = new GegnerFadeMusic(Value1, Value2, Light);
		} break;

		case ENDLEVEL:
		{
			pNew = new GegnerEndLevel(Value1, Value2, Light);
		} break;

		case PLATTFORM:
		{
			pNew = new GegnerPlattform(Value1, Value2, Light);
		} break;

		case PRESSE:
		{
			pNew = new GegnerPresse(Value1, Value2, Light);
		} break;

		case SHRINE:
		{
			pNew = new GegnerShrine(Value1, Value2, Light);
		} break;

		case BRUECKE:
		{
			pNew = new GegnerBruecke(int (y), Value2, Light);
		} break;

		case FLOATING:
		{
			pNew = new GegnerFloating(Value1, Value2, Light);
		} break;

		case FLOATING2:
		{
			pNew = new GegnerFloating2(Value1, Value2, Light);
		} break;

		case FLOATING3:
		{
			pNew = new GegnerFloating3(Value1, Value2, Light);
		} break;

		case SURFBRETT:
		{
			pNew = new GegnerSurfBrett(int (y), Value2, Light);
		} break;

		case SHOOTBUTTON:
		{
			pNew = new GegnerShootButton(pEnd);
		} break;

		case SHOOTPLATTFORM:
		{
			pNew = new GegnerShootPlattform(int (x), int (y), Light);
		} break;

		case GLUBSCHI:
		{
			pNew = new GegnerGlubschi(Value1, Value2, Light);
		} break;

		case GLUBSCHI2:
		{
			pNew = new GegnerGlubschi2(Value1, Value2, Light);
		} break;

		case COLUMN:
		{
			pNew = new GegnerColumn(Value1, Value2, Light);
		} break;

		case COLUMN2:
		{
			pNew = new GegnerColumn2(Value1, Value2, Light);
		} break;

		case LIFT:
		{
			pNew = new GegnerLift(Value1, Value2, Light);
		} break;

		case SCHLEUSEH:
		{
			pNew = new GegnerSchleuseH(Value1, Value2, Light);
		} break;

		case SCHLEUSEV:
		{
			pNew = new GegnerSchleuseV(Value1, Value2, Light);
		} break;

		case SWITCH:
		{
			pNew = new GegnerSwitch(Value1, Value2, Light);
		} break;

		case WARNING:
		{
			pNew = new GegnerWarning(Value1, Value2, Light);
		} break;

		case SOUNDTRIGGER:
		{
			pNew = new GegnerSoundTrigger(Value1, Value2, Light);
		} break;

		case LUEFTER_GROSS:
		{
			pNew = new GegnerLuefterGross(Value1, Value2, Light);
		} break;		

		case LUEFTER_KLEIN:
		{
			pNew = new GegnerLuefterKlein(Value1, Value2, Light);
		} break;

		case LUEFTER_KLEIN2:
		{
			pNew = new GegnerLuefterKlein2(Value1, Value2, Light);
		} break;

		case TUTORIALTEXT:
		{
			pNew = new GegnerTutorialText(Value1, Value2, Light);
		} break;

		case LIGHTFLARE:
		{
			pNew = new GegnerLightFlare(Value1, Value2, Light);
		} break;

		case SECRET:
		{
			pNew = new GegnerSecret(Value1, Value2, Light);
		} break;

		case MUSHROOM:
		{
			pNew = new GegnerMushroom(Value1, Value2, Light);
		} break;

		case PRESSWURST:
		{
			pNew = new GegnerPresswurst(Value1, Value2, Light);
		} break;

		case GOLEM:
		{
			pNew = new GegnerGolem(Value1, Value2, Light);
		} break;

		case SPINNENMASCHINE:
		{
			pNew = new GegnerSpinnenmaschine(Value1, Value2, Light);
		} break;

		case THEWALL:
		{
			pNew = new GegnerTheWall(Value1, Value2, Light);
		} break;

		case LAFASS:
		{
			pNew = new GegnerLaFass(Value1, Value2, Light);
		} break;

		case FETTERAKETE:
		{
			pNew = new GegnerFetteRakete(Value1, Value2, Light);
		} break;

		case MINIROCKET:
		{
			pNew = new GegnerMiniRocket(Value1, Value2, Light);
		} break;

		case GESCHUETZ:
		{
			pNew = new GegnerGeschuetz(Value1, Value2, Light);
		} break;

		case LAFASSSPAWNER:
		{
			pNew = new GegnerLaFassSpawner(Value1, Value2, Light);
		} break;

		case SCHABE:
		{
			pNew = new GegnerSchabe(Value1, Value2, Light);
		} break;

		case MINIDRAGON:
		{
			pNew = new GegnerMiniDragon(Value1, Value2, Light);
		} break;

		case SPIKELIFT:
		{
			pNew = new GegnerSpikelift(Value1, Value2, Light);
		} break;

		case DRACHE:
		{
			pNew = new GegnerDrache(Value1, Value2, Light);
		} break;

		case ROLLMOPS:
		{
			pNew = new GegnerRollmops(Value1, Value2, Light);
		} break;

		case SCHNEEKOENIG:
		{
			pNew = new GegnerSchneeKoenig(Value1, Value2, Light);
		} break;

		case SCHNEEKOPPE:
		{
			pNew = new GegnerSchneekoppe(Value1, Value2, Light);
		} break;

		case ZITRONE:
		{
			pNew = new GegnerZitrone(Value1, Value2, Light);
		} break;

		case ROTZPOTT:
		{
			pNew = new GegnerRotzpott(Value1, Value2, Light);
		} break;

		case BIGFISH:
		{
			pNew = new GegnerBigFish(Value1, Value2, Light);
		} break;

		case KETTENGLIED:
		{
			pNew = new GegnerKettenglied(Value1, Value2, Light);
		} break;

		case STACHELBEERE:
		{
			pNew = new GegnerStachelbeere(Value1, Value2, Light);
		} break;

		case TUBE:
		{
			pNew = new GegnerTube(Value1, Value2, Light);
		} break;

		case MUTANT:
		{
			pNew = new GegnerMutant(Value1, Value2, Light);
		} break;

		default : break;
	}

	// Werte festlegen
	pNew->GegnerArt = Art;
	pNew->xPos		= x;
	pNew->yPos		= y;
	pNew->xPosOld   = x;
	pNew->yPosOld   = y;

	if(pStart==NULL)							// Liste leer ?
	{
		pStart = pNew;							// Ja, dann neuer Gegner gleich der erste
		pEnd   = pNew;							// und letzte Gegner

		pStart->pNext=NULL;						// Next/Previous gibts nich, da wir
		pStart->pPrev=NULL;						// nur 1 Gegner haben
	} 
	else										// Liste ist NICHT leer
	{
		// Gegner am Ende einfügen?
		if (atEnd == true)
		{
			pEnd->pNext = pNew;						// Letzter Gegner zeigt auf den neuen
			pNew->pPrev = pEnd;						// Letzter Gegner ist nicht mehr der letzte
		
			pNew->pNext = NULL;						// Nach dem neuen Gegner kommt keiner mehr
			pEnd		= pNew;						// da er jetzt der letzte in der Liste ist
		}

		// Gegner am Anfag einfügen?
		else
		{
			pStart->pPrev = pNew;					// Erster Gegner zeigt auf den neuen
			pNew->pNext = pStart;					// Erster Gegner ist nicht mehr der erste
		
			pNew->pPrev = NULL;						// Vor dem neuen Gegner kommt keiner mehr
			pStart		= pNew;						// da er jetzt der erste in der Liste ist
		}
	}

	NumGegner++;								// Gegneranzahl erhöhen

	// Bei der ShootPlattform noch den Button anfügen
	//
	if (Art == SHOOTPLATTFORM)
	{
		// Button anfügen	
		pGegner->PushGegner (pNew->xPos + 42, pNew->yPos - 9, SHOOTBUTTON, 0, 0, Light);
	}

	return true;
}

// --------------------------------------------------------------------------------------
// Bestimmten Gegner der Liste löschen
// --------------------------------------------------------------------------------------

void GegnerListClass::DelSel(GegnerClass *pTemp)
{
	GegnerClass  *pN;
    GegnerClass  *pP;

	if(pTemp!=NULL)						// zu löschender Gegner existiert 
	{
		pN = pTemp->pNext;			
		pP = pTemp->pPrev;
  
		if(pP == NULL)					// Wird der erste Gegner gelöscht ?
			pStart = pN;				// Dann wird dessen Nächster zum Ersten
		else
			pP->pNext = pN;	   	        // ansonsten normal eins aufrücken

		if(pN == NULL)					// Wird der letzte Gegner gelöscht ?
			pEnd = pP;					// Dann wir der letzte Gegner zum ersten
		else
		pN->pPrev = pP;
  
		delete (pTemp);					// Speicher freigeben
		pTemp = NULL;

		NumGegner--;					// Gegnerzahl verringern
	}
}

// --------------------------------------------------------------------------------------
// Alle Gegner der Liste löschen
// --------------------------------------------------------------------------------------

void GegnerListClass::ClearAll(void)
{
    GegnerClass *pTemp    = pStart;					// Zeiger auf den ersten   Gegner
	GegnerClass *pNaechst;							// Zeiger auf den nächsten Gegner (falls
													// der eine gelöscht wird)
    while (pTemp != NULL)							// Ende der Liste erreicht ?
    {
		pNaechst = pTemp->pNext;					// Zeiger auf das nächste Element
		DelSel(pTemp);								// Das aktuelle löschen
        pTemp = pNaechst;							// und das nächste bearbeiten
    }

	pStart = NULL;
	pEnd   = NULL;
}

// --------------------------------------------------------------------------------------
// Zahl der Gegner zurückliefern
// --------------------------------------------------------------------------------------

int GegnerListClass::GetNumGegner(void)
{
	return NumGegner;
}

// --------------------------------------------------------------------------------------
// Alle Gegner der Liste animieren
// --------------------------------------------------------------------------------------

void GegnerListClass::RenderAll(void)
{
	GegnerClass *pTemp = pStart;			// Anfang der Liste

	// Zuerst die "Gegner" rendern, die als Background fungieren
	// z.B. der große Lüfter, damit diese nicht die anderen Gegner verdecken können
	//
	while (pTemp != NULL)					// noch nicht alle durch ?		   
	{
		if (pTemp->BackGround == false)		// kein Background? Dann nächsten
		{
			pTemp = pTemp->pNext;
			continue;
		}
		
		if (pTemp->Active == true)			// aktueller Gegner aktiv ?
		{
			// dann Gegner rendern
			pTemp->AlreadyDrawn = false;
			pTemp->Render();
		}

		pTemp = pTemp->pNext;				// Nächsten durchgehen
	}

	// Danach alle anderen "richtigen" Gegner rendern
	//

	pTemp = pGegner->pStart;

	while (pTemp != NULL)					// noch nicht alle durch ?		   
	{
		if (pTemp->BackGround == true)		// Background? Dann nächsten
		{
			pTemp = pTemp->pNext;
			continue;
		}		
		
		if (pTemp->Active == true)			// aktueller Gegner aktiv ?
		{			
			// dann Gegner rendern
			pTemp->AlreadyDrawn = false;
			pTemp->Render();
		}

		pTemp = pTemp->pNext;				// Und nächsten Gegner rendern
	}
}

// --------------------------------------------------------------------------------------
// Alle Gegner der Liste durchgehen
// --------------------------------------------------------------------------------------

void GegnerListClass::RunAll(void)
{
	GegnerClass *pTemp = pStart;			// Anfang der Liste
	GegnerClass *pNext = NULL;

	// Alle Einträge der Gegnerliste durchgehen
	//
	while (pTemp != NULL)					// noch nicht alle durch ?		   
	{
		pNext = pTemp->pNext;

		pTemp->Run();

		// ggf Gegner löschen (bei Energy <= 0)
		if (pTemp->Energy <= 0.0f)		 	
		{
			pTemp->GegnerExplode();			// Jeder Gegner explodiert anders
			DelSel(pTemp);					// Gegner aus der Liste löschen
		}
			

		pTemp = pNext;
	}	
}

// --------------------------------------------------------------------------------------
// Alle Gegner auf dem Screen (ausser Endbosse) zerstören (Granate)
// --------------------------------------------------------------------------------------

void GegnerListClass::DamageEnemiesonScreen(float x, float y, int MaxDamage)
{
	// Gegner durchgehen und die auf dem Screen löschen
	GegnerClass *pTemp = pStart;			// Anfang der Liste
	GegnerClass *pNext = NULL;				// Nächster Gegner in der Liste

	while (pTemp != NULL)					// Noch nicht alle durch ?
	{
		float ax = x - pTemp->xPos;
		float ay = y - pTemp->yPos;
		float dx = float (sqrt ((ax * ax) + (ay * ay)));

		pNext = pTemp->pNext;				// Nächsten sichern

		// Stampfstein? Fällt runter bei Wackeln
		if (pTemp->Active	   == true		  &&			
			pTemp->GegnerArt   == STAMPFSTEIN &&
			pTemp->Handlung    == GEGNER_STEHEN &&
			dx < 300						  &&
			pTemp->xPos + GegnerRect[pTemp->GegnerArt].right  > pTileEngine->XOffset		  &&
			pTemp->xPos + GegnerRect[pTemp->GegnerArt].left   < pTileEngine->XOffset + 640	  &&
			pTemp->yPos + GegnerRect[pTemp->GegnerArt].bottom > pTileEngine->YOffset		  &&
			pTemp->yPos + GegnerRect[pTemp->GegnerArt].top    < pTileEngine->YOffset + 480)
		{
			pTemp->Handlung = GEGNER_FALLEN;
			pTemp->ySpeed   = 20.0f;
			pTemp->yAcc     = 15.0f;

			if (pSoundManager->its_Sounds[SOUND_STONEFALL]->isPlaying == false)
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_STONEFALL);				
		}
		
		// Gegner in der Nähe? Dann Energie abziehen
		if (pTemp->Active	   == true		    &&
			dx < 300							&&
			pTemp->Destroyable == true		    &&
			pTemp->GegnerArt   != POWERBLOCK	&&
			pTemp->GegnerArt < RIESENPIRANHA    &&
			pTemp->xPos + GegnerRect[pTemp->GegnerArt].right  > pTileEngine->XOffset		  &&
			pTemp->xPos + GegnerRect[pTemp->GegnerArt].left   < pTileEngine->XOffset + 640	  &&
			pTemp->yPos + GegnerRect[pTemp->GegnerArt].bottom > pTileEngine->YOffset		  &&
			pTemp->yPos + GegnerRect[pTemp->GegnerArt].top    < pTileEngine->YOffset + 480)
		{
			int amount = (int)(MaxDamage - dx);

			if (amount < 0)
				amount = 0;

			pTemp->Energy -= amount;
		}

		pTemp = pNext;						// Und nächsten Gegner anhandeln
	}
}
