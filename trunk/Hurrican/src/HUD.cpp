// Datei : HUD.cpp

// -------------------------------------------------------------------------------------- 
//
// Funktionen für das Hurrican HUD
// Werte anzeigen usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "HUD.h"
#include "DX8Font.h"
#include "DX8Graphics.h"
#include "Gameplay.h"
#include "GegnerClass.h"
#include "Gegner_Stuff.h"
#include "Player.h"
#include "Tileengine.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Konstruktor : HUD Grafiken laden und Werte initialisieren
// --------------------------------------------------------------------------------------

HUDClass::HUDClass(void)
{
	HUDGfx.LoadImage		("HUD.bmp", 548, 69, 548, 69, 1, 1);
	HUDBlitz.LoadImage		("hudblitz.bmp",6, 18, 6, 18, 1, 1);
	HUDBomb.LoadImage		("hudbomb.bmp", 18, 18, 18, 18, 1, 1);
	HUDSuperShot.LoadImage	("hudsupershot.bmp", 24, 24, 24, 24, 1, 1);
	HUDAutoFire.LoadImage	("hudautofire.bmp",24, 24, 24, 24, 1, 1);
	HUDBall[0].LoadImage	("hudenergy.bmp", 69, 69, 69, 69, 1, 1);
	HUDBall[1].LoadImage	("hudrad.bmp", 69, 69, 69, 69, 1, 1);
	HUDFontBig.LoadImage	("hudfontbig.bmp", 128, 32, 12, 22, 10, 1);
	SelectedWeapon.LoadImage("hudselected.bmp", 68, 17, 17, 17, 4, 1);
	WeaponRahmen.LoadImage	("hudbalken.bmp", 9, 18, 9, 18, 1, 1);
	WeaponPunkt.LoadImage	("hudbalken_full.bmp",7, 2, 7, 2, 1, 1);
	Arrow.LoadImage			("hudarrow.bmp",24, 33, 24, 33, 1, 1);

	BossHUD.LoadImage		("bosshud.bmp", 130, 41, 130, 41, 1, 1);
	BossBar.LoadImage		("bossbar.bmp", 113, 24, 113, 24, 1, 1);

	// HUD Werte initialisieren
	Alpha			= 192;
	red				= 0;
	green			= 255;
	blue			= 0;
	xpos			= (640-548) / 2;
	ypos			= 2;
	BossHUDActive	= 0.0f;
}

// --------------------------------------------------------------------------------------
// Konstruktor : HUD Grafiken freigeben
// --------------------------------------------------------------------------------------

HUDClass::~HUDClass(void)
{	
}

// --------------------------------------------------------------------------------------
// Werte im HUD ändern
// --------------------------------------------------------------------------------------

void HUDClass::UpdateValues(void)
{
}

// --------------------------------------------------------------------------------------
// HUD anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::ShowHUD(void)
{
	char	 Buffer[100];							// Für iota Umwandlung der HUD-Werte
	int		 BlitzOff;								// Für die Balken-Offsets
//	int		 StateOff;								
	D3DCOLOR Color;									// Farbe des Huds
	D3DCOLOR playercol;

	DirectGraphics.SetColorKeyMode();

	Color = D3DCOLOR_RGBA(red, green, blue, Alpha);

	HUDGfx.RenderSprite(xpos, ypos, 0, Color);

	BlitzOff  = int(BLITZ_STAT_HEIGHT  - pPlayer[0]->CurrentWeaponLevel[3]*BLITZ_STAT_HEIGHT/BLITZ_STAT_HEIGHT);

	// Energy
	float off;

	if (NUMPLAYERS == 1)
	{
		off = ((MAX_ENERGY - pPlayer[0]->Energy) * 69.0f / MAX_ENERGY);
		HUDBall[0].SetRect(0, (int)off, 69, 69);
		HUDBall[0].RenderSprite(xpos, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));

		// Radenergie
		off = ((MAX_ARMOUR - pPlayer[0]->Armour) * 69.0f / MAX_ARMOUR );
		HUDBall[1].SetRect(0, (int)off, 69, 69);
		HUDBall[1].RenderSprite(xpos+548-69, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));
	}
	else
	for (int num = 0; num < NUMPLAYERS; num++)
	{
		PlayerClass *pCurrentPlayer;
		
		if (num == 0)
		{
			pCurrentPlayer = pPlayer[0];

			off = ((MAX_ENERGY - pCurrentPlayer->Energy) / MAX_ENERGY * 69);
			HUDBall[0].SetRect(0, (int)off, 34, 69);
			HUDBall[0].RenderSprite(xpos, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));

			// Radenergie
			off = ((MAX_ARMOUR - pCurrentPlayer->Armour) / MAX_ENERGY * 69);
			HUDBall[1].SetRect(34, (int)off, 69, 69);
			HUDBall[1].RenderSprite(xpos + 34, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));			
		}
		else
		{
			pCurrentPlayer = pPlayer[1];

			off = ((MAX_ENERGY - pCurrentPlayer->Energy) / MAX_ENERGY * 69);
			HUDBall[0].SetRect(0, (int)off, 34, 69);
			HUDBall[0].RenderSprite(xpos+548-69, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));			

			// Radenergie
			off = ((MAX_ARMOUR - pCurrentPlayer->Armour) / MAX_ARMOUR * 69);
			HUDBall[1].SetRect(34, (int)off, 69, 69);
			HUDBall[1].RenderSprite(xpos+548-69 + 34, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));
		}		
	}

	HUDBlitz.SetRect (0, BlitzOff, 6, 18);

	// Wenn geblitzt wird, die Blitzwaffe heller anzeigen
	if (pPlayer[0]->Handlung == BLITZEN)
	SelectedWeapon.RenderSprite(xpos + 212 + 3 * 32, 
								ypos + 14, 3, Color);

	D3DCOLOR color;

	 // Blitz-Level anzeigen
	if (pPlayer[0]->Handlung == BLITZEN)
		color = D3DCOLOR_RGBA(0, 255, 0, 192);			
	else
		color = D3DCOLOR_RGBA(0, 255, 0, 64);

	WeaponRahmen.RenderSprite(xpos + 216 + 3*32, ypos +  35, 0, color);
	 for (int j=0; j<pPlayer[0]->CurrentWeaponLevel[3]; j++)
		WeaponPunkt.RenderSprite(xpos + 217 + 3*32, ypos +  50 - j, 0, color);

/*
	 // Balken anzeigen, wieviele PowerUps noch zum nächsten Waffenlevel benötigt werden
	 for (i=0; i<4; i++)
	 {
		 StateOff = pPlayer->CollectedPowerUps[i] * 24/pPlayer->NextWeaponLevel[i];

		 WeaponState.SetRect(0, 0, StateOff, 8);
		 WeaponState.RenderSprite(xpos + 208 + i*32,  ypos + 42, Color);
	 }
*/

	 // Anzahl verbleibender Powerlines anzeigen
	 for (int p = 0; p < NUMPLAYERS; p++)
	 {		 
		 int off;

		if (NUMPLAYERS == 1)
		{
			playercol = Color;
			off = 0;
		}
		else
		{
			off = -6;
			if (p == 0)
				playercol = 0xFFFF4400;
			else
				playercol = 0xFF2266FF;
		}

		int xoff;

		if (NUMPLAYERS == 1)
			xoff = 0;
		else
		{
			if (p == 0)
				xoff = -25;
			else
				xoff = 15;
		}

		// AutoFire anzeigen, wenn vorhanden
		if (pPlayer[p]->AutoFireExtra > 0.0f)
		{
			HUDAutoFire.RenderSprite(xpos + 263 + xoff, ypos + 61, 0, Color);
			_itoa_s(int(pPlayer[p]->AutoFireExtra), Buffer, 10);

			if (pPlayer[p]->AutoFireExtra< 10)
			{
				pDefaultFont->DrawText(xpos + 270 + xoff, ypos+86, Buffer, D3DCOLOR_RGBA(0, 0, 0, Alpha));
				pDefaultFont->DrawText(xpos + 269 + xoff, ypos+85, Buffer, playercol);
			}
			else
			if (pPlayer[p]->AutoFireExtra< 100)
			{
				pDefaultFont->DrawText(xpos + 271 + xoff, ypos+86, Buffer, D3DCOLOR_RGBA(0, 0, 0, Alpha));
				pDefaultFont->DrawText(xpos + 270 + xoff, ypos+85, Buffer, playercol);
			}
			else
			{
				pDefaultFont->DrawText(xpos + 269 + xoff, ypos+86, Buffer, D3DCOLOR_RGBA(0, 0, 0, Alpha));
				pDefaultFont->DrawText(xpos + 268 + xoff, ypos+85, Buffer, playercol);
			}
		}

		// Supershot anzeigen, wenn vorhanden (bei vorhandenem Autofire wird der Supershot tiefer angezeigt)
		if (pPlayer[p]->RiesenShotExtra > 0.0f)
		{
			int off = 0;

			if (pPlayer[p]->AutoFireExtra > 0.0f)
				off += 37;

			HUDSuperShot.RenderSprite(xpos + 264 + xoff, ypos + 61 + off, 0, Color);
			_itoa_s(int(pPlayer[p]->RiesenShotExtra), Buffer, 10);

			if (pPlayer[p]->RiesenShotExtra< 10)
			{
				pDefaultFont->DrawText(xpos + 270 + xoff, ypos+87+ off, Buffer, D3DCOLOR_RGBA(0, 0, 0, Alpha));
				pDefaultFont->DrawText(xpos + 269 + xoff, ypos+86+ off, Buffer, playercol);
			}
			else
			if (pPlayer[p]->RiesenShotExtra< 100)
			{
				pDefaultFont->DrawText(xpos + 271 + xoff, ypos+87+ off, Buffer, D3DCOLOR_RGBA(0, 0, 0, Alpha));
				pDefaultFont->DrawText(xpos + 270 + xoff, ypos+86+ off, Buffer, playercol);
			}
			else
			{
				pDefaultFont->DrawText(xpos + 269 + xoff, ypos+87+ off, Buffer, D3DCOLOR_RGBA(0, 0, 0, Alpha));
				pDefaultFont->DrawText(xpos + 268 + xoff, ypos+86+ off, Buffer, playercol);
			}
		}

		_itoa_s(pPlayer[p]->PowerLines, Buffer, 10);

		if (pPlayer[p]->PowerLines < 10)
			pDefaultFont->DrawText(xpos + 353 + off + p * 10, ypos+40, Buffer, playercol);
		else
			pDefaultFont->DrawText(xpos + 349 + off + p * 10, ypos+40, Buffer, playercol);

		// Anzahl verbleibender Granaten anzeigen
		_itoa_s(pPlayer[p]->Grenades, Buffer, 10);

		if (pPlayer[p]->Grenades < 10)
			pDefaultFont->DrawText(xpos + 376 + off + p * 10, ypos+40, Buffer, playercol);
		else
			pDefaultFont->DrawText(xpos + 372 + off + p * 10, ypos+40, Buffer, playercol);

		// Befindet sich die Smartbombe im Besitz des Spielers ?
		if (pPlayer[p]->SmartBombs > 0)
		{
			if (NUMPLAYERS == 1)
				HUDBomb.RenderSprite(xpos + 391, ypos + 19, 0, playercol);
			else
			{
				if (p == 0)
				{
					HUDBomb.SetRect(0, 0, 9, 18);
					HUDBomb.RenderSprite(xpos + 391, ypos + 19, playercol);
				}
				else
				{
					HUDBomb.SetRect(10, 0, 18, 18);
					HUDBomb.RenderSprite(xpos + 391 + 9, ypos + 19, playercol);
				}
			}
		}	

		// Gewählte Waffe heller darstellen
		if (pPlayer[p]->Handlung != BLITZEN)
		{
			if (NUMPLAYERS == 1)
				SelectedWeapon.RenderSprite(xpos + 212 + pPlayer[p]->SelectedWeapon * 32, 
											ypos + 14, pPlayer[p]->SelectedWeapon, playercol);
			else
			{
				// Anderer Spieler hat eine andere Waffe? Dann ganz rendern
				if (pPlayer[p]->SelectedWeapon != pPlayer[1-p]->SelectedWeapon)
					SelectedWeapon.RenderSprite(xpos + 212 + pPlayer[p]->SelectedWeapon * 32, 
												ypos + 14, pPlayer[p]->SelectedWeapon, playercol);
				// ansonsten halbieren
				else
				{
					SelectedWeapon.itsRect = SelectedWeapon.itsPreCalcedRects[pPlayer[p]->SelectedWeapon];

					if (p == 0)
						SelectedWeapon.itsRect.right -= 8;
					else
						SelectedWeapon.itsRect.left += 8;

					SelectedWeapon.RenderSprite(xpos + 212 + p * 8 + pPlayer[p]->SelectedWeapon * 32, 
												ypos + 14, playercol);
				}
			}
		}

		// PowerLevel der Waffen darstellen
		for (unsigned int i=0; i<3; i++)	 
		{
			if (NUMPLAYERS == 2 &&
				pPlayer[p]->SelectedWeapon != pPlayer[1-p]->SelectedWeapon &&
				pPlayer[1-p]->SelectedWeapon == (int)i)
				continue;

			if (i == (unsigned int)pPlayer[p]->SelectedWeapon)
			{
				if (NUMPLAYERS == 1)
					playercol = D3DCOLOR_RGBA(0, 255, 0, 224);			
				else
				{
					if (p == 0)
						playercol = 0xFFFF4400;
					else
						playercol = 0xFF2266FF;
				}
			}
			else
			{
				if (NUMPLAYERS == 1 ||
					pPlayer[p]->SelectedWeapon != (int)i)
					playercol = D3DCOLOR_RGBA(0, 255, 0, 64);
				else
				{
					if (p == 0)
						playercol = 0x40FF4400;
					else
						playercol = 0x402266FF;
				}
			}

			if (NUMPLAYERS == 1 ||
				pPlayer[p]->SelectedWeapon != pPlayer[1-p]->SelectedWeapon)
				WeaponRahmen.RenderSprite(xpos + 216 + i*32, ypos +  35, 0, playercol);
			else
			{
				WeaponRahmen.itsRect.left = 0;
				WeaponRahmen.itsRect.right = 9;
				WeaponRahmen.itsRect.top = 0;
				WeaponRahmen.itsRect.bottom = 18;

				if (p == 0)
					WeaponRahmen.itsRect.right = 5;					
				else
					WeaponRahmen.itsRect.left  = 5;

				WeaponRahmen.RenderSprite((float)(xpos + p * 5 + 216 + i*32), 
										  (float)(ypos +  35), playercol);

			}

			// Waffen Punkte
			for (int j=0; j<pPlayer[p]->CurrentWeaponLevel[i]; j++)
			{
				if (NUMPLAYERS == 1 ||
					(pPlayer[p]->SelectedWeapon == (int)i &&
					 pPlayer[p]->SelectedWeapon != pPlayer[1-p]->SelectedWeapon))

					WeaponPunkt.RenderSprite(xpos + 217 + i*32, ypos +  50 - j * 2 , 0, playercol);
				else
				{
					WeaponPunkt.itsRect.left = 0;
					WeaponPunkt.itsRect.right = 7;
					WeaponPunkt.itsRect.top = 0;
					WeaponPunkt.itsRect.bottom = 2;

					if (p == 0)
						WeaponPunkt.itsRect.right = 4;
					else
						WeaponPunkt.itsRect.left = 4;

					WeaponPunkt.RenderSprite(xpos + p * 4 + 217 + i*32, ypos +  50 - j * 2 , playercol);
				}
			}
		 }
	 }

	 // Anzahl verbleibender Leben anzeigen
	 int LivesToShow;
	 
	 LivesToShow = max(0, pPlayer[0]->Lives);

	 _itoa_s(LivesToShow, Buffer, 10);

	for(unsigned int i=0; i<strlen(Buffer); i++)
	{
		char	z = Buffer[i]-48;

		HUDFontBig.SetRect(z*12, 0, z*12+12, 25);
		HUDFontBig.RenderSprite(xpos - strlen(Buffer)*13+ i*13 + 48, ypos + 22, Color);

	}

	 // Punkte anzeigen
	if (HasCheated)
		pPlayer[0]->Score = 0;

	_itoa_s(pPlayer[0]->Score, Buffer, 10);

	int len = strlen(Buffer);
	for(int i=0; i<len; i++)
	{
		char	z = Buffer[i]-48;

		HUDFontBig.SetRect(z*12, 0, z*12+12, 25);
		HUDFontBig.RenderSprite(xpos - strlen(Buffer)*13 + i*13 + 195, ypos + 22, Color);
	}

	// Diamanten anzeigen
	if (NUMPLAYERS == 1)
		_itoa_s(pPlayer[0]->CollectedDiamonds, Buffer, 10);

	// oder Leben Spieler 2
	else
	{
		LivesToShow = max(0, pPlayer[1]->Lives);
		_itoa_s(LivesToShow, Buffer, 10);
	}

	len = strlen(Buffer);
	for(int i=0; i<len; i++)
	{
		char	z = Buffer[i]-48;

		HUDFontBig.SetRect(z*12, 0, z*12+12, 25);
		HUDFontBig.RenderSprite(xpos - strlen(Buffer)*13 + i*13 + 527, ypos + 22, Color);
	}

	if (NUMPLAYERS == 2)
	{
		_itoa_s(pPlayer[0]->CollectedDiamonds, Buffer, 10);
		pGegnerGrafix[DIAMANT]->RenderSprite(xpos + 555, ypos + 15, 0, Color);
		pDefaultFont->DrawTextCenterAlign(xpos + 566, ypos + 50, Buffer, Color);
	}

	// Verbleibende Zeit anzeigen
	_itoa_s(int(pTileEngine->Timelimit), Buffer, 10);

	for(unsigned int i=0; i<strlen(Buffer); i++)
	{
		char	z = Buffer[i]-48;

		HUDFontBig.SetRect(z*12, 0, z*12+12, 25);
		HUDFontBig.RenderSprite(xpos - strlen(Buffer)*13 + i*13 + 467, ypos + 22, Color);

		// Zeit wird knapp? Dann Rote Zahlen rendern
		if (BossHUDActive <= 0.0f &&
			pTileEngine->Timelimit < 10.0f &&
			pTileEngine->Timelimit >  0.0f)
		{
			DirectGraphics.SetAdditiveMode();

			float size = (float)((int)(pTileEngine->Timelimit + 1) - pTileEngine->Timelimit) * 255.0f;

			float xoff = 0.0f;
			if ((int)(pTileEngine->Timelimit) == 1)
				xoff = -size / 2.4f;

			HUDFontBig.RenderSpriteScaled(320 - size / 2.0f + xoff, 
										  240 - size / 2.0f, 
										  12 + (int)size, 
										  24 + (int)size, D3DCOLOR_RGBA(255, 0, 0, 255 - (int)(size)));

			DirectGraphics.SetColorKeyMode();
		}
	}
}

// --------------------------------------------------------------------------------------
// Boss HUD anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::RenderBossHUD (void)
{
	// Boss HUD anzeigen
	if (BossHUDActive > 0.0f)
	{
		int a = int(BossHUDActive * 0.75);
		D3DCOLOR Color = D3DCOLOR_RGBA(red, green, blue, a);

		BossHUDActive -= 20.0f SYNC;			// Ausfaden lassen

		BossHUD.RenderSprite((640 - 130) / 2,	  ypos + 434,      Color);
		BossBar.RenderSprite((640 - 130) / 2 + 9, ypos + 434 + 13, Color);
	}
	else
		BossHUDActive = 0.0f;
}

// --------------------------------------------------------------------------------------
// Boss HUD mit Boss Energie anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::ShowBossHUD(float max, float act)
{
	// Boss HUD einfaden
	BossHUDActive += 40.0f SYNC;

	if (BossHUDActive > Alpha)
		BossHUDActive = float (Alpha);

	// Balken länge berechnen
	int off = int((act-100)*113/(max-100));

	if (off < 0)
		off = 0;

	BossBar.SetRect(0, 0, off, 24);
}

// --------------------------------------------------------------------------------------
// Alles machen, was da HUD betrifft, sprich, werte ändern und dann anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::DoHUD(void)
{
	// Hud Werte aktualisieren
	//
	for (int p = 0; p < NUMPLAYERS; p++)
		pPlayer[p]->BlitzLength = pPlayer[p]->CurrentWeaponLevel[3]+1;
	
	UpdateValues();

	// Einheitsmatrix setzen, da das HUD nie rotiert gerendert wird
	//
	D3DXMATRIX matView;
	D3DXMatrixIdentity	 (&matView);
	lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	// Hud anzeigen
	//
	ShowHUD();

	// Tutorial Pfeil anzeigen?
	//
	if (bShowArrow == true)
		RenderArrow();

	// und evtl wieder rotierten Screen setzen
	//
	SetScreenShake();	
}

// --------------------------------------------------------------------------------------
// Pfeil in diesem Frame anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::ShowArrow (float x, float y)
{
	bShowArrow = true;
	ArrowX = x;
	ArrowY = y;
}

// --------------------------------------------------------------------------------------
// Pfeil rendern
// --------------------------------------------------------------------------------------

void HUDClass::RenderArrow ()
{
	static float alpha    = 0.0f;
	static float alphadir = 50.0f;

	alpha += alphadir SYNC;

	if ((alphadir > 0.0f && alpha > 255.0f) ||
		(alphadir < 0.0f && alpha <   0.0f))
	{
		alphadir *= -1.0f;

		if (alpha <   0.0f) alpha =   0.0f;
		if (alpha > 255.0f) alpha = 255.0f;
	}

	D3DCOLOR Color = D3DCOLOR_RGBA (0, 255, 0, int (alpha));

	DirectGraphics.SetColorKeyMode();
	Arrow.RenderSprite (ArrowX, ArrowY, 0, Color);	
}