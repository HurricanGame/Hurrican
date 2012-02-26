// Datei : Player.cpp

// -------------------------------------------------------------------------------------- 
//
// Funktionen für den Spieler
// Tasteneingaben bearbeiten und Spieler entsprechend animieren
// usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>

#include "Console.h"
#include "GUISystem.h"
#include "Player.h"
#include "DX8Graphics.h"
#include "DX8Input.h"
#include "DX8Sound.h"
#include "DX8Font.h"
#include "Gegner_Helper.h"
#include "Globals.h"
#include "GamePlay.h"
#include "Logdatei.h"
#include "Menu.h"
#include "Main.h"
#include "Outtro.h"
#include "PartikelSystem.h"
#include "Projectiles.h"
#include "Tileengine.h"
#include "Timer.h"

extern long DEMOPress;
extern bool	DEMORecording;
extern bool	DEMOPlaying;

extern bool	JoystickFound;

int Stage;				// Aktuelles Level
int NewStage;			// Neues Level
int	Skill;				// 0 = easy, 1 = medium, 2 = hard, 3 = Hurrican

// --------------------------------------------------------------------------------------
// Player-Klasse
// --------------------------------------------------------------------------------------

DirectGraphicsSprite	SchussFlamme[4];		// Grafiken für die SchussFlamme, für alle 3 Waffen
DirectGraphicsSprite	SchussFlammeFlare;		// Grafiken für das Leuchten der SchussFlamme
DirectGraphicsSprite	PlayerBlitz[2];
DirectGraphicsSprite	PlayerCrouch[2];
DirectGraphicsSprite	PlayerKucken[2];
DirectGraphicsSprite	PlayerIdle[2];
DirectGraphicsSprite	PlayerIdle2[2];
DirectGraphicsSprite	PlayerJump[2];
DirectGraphicsSprite	PlayerJumpUp[2];
DirectGraphicsSprite	PlayerJumpDiagonal[2];
DirectGraphicsSprite	PlayerRun[2];
//DirectGraphicsSprite	PlayerRunDiagonal[2][FRAMES_RUN];
DirectGraphicsSprite	PlayerDiagonal[2];
DirectGraphicsSprite	PlayerOben[2];
DirectGraphicsSprite	PlayerSurf[2];
DirectGraphicsSprite	PlayerPiss[2];				// Grafiken für das Pissen, wenn man lange nix macht =)
DirectGraphicsSprite	PlayerRide[2];				// Grafiken für das Reiten auf dem FlugSack
DirectGraphicsSprite	PlayerRad[2];				// Grafiken für das Rad		
DirectGraphicsSprite	Blitzstrahl[4];			// Grafik   des Blitzes
DirectGraphicsSprite	Blitzflash[4];			// Grafik   des Leuchtens beim Blitz
DirectGraphicsSprite	BlitzTexture;			// Textur auf den Blitzen drauf

// --------------------------------------------------------------------------------------
// Konstruktor : Spieler initialisieren und Grafiken laden
// --------------------------------------------------------------------------------------

PlayerClass::PlayerClass(void)
{
	Stage    = -1;				// Es läuft noch kein Spiel
	NewStage = -1;
	StageClearRunning = false;
	JoystickSchwelle = 500.0f;
	ControlType = JOYMODE_KEYBOARD;
	JoystickMode = JOYMODE_PAD; 
	JoystickIndex = 0;
	GodMode = false;
	WheelMode = false;
	ExplodingTimer = 0.0f;
	BeamCount = 0.0f;
	changecount = 0.0f;
	ZeroMemory(&strahlen, sizeof(strahlen));
}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

PlayerClass::~PlayerClass(void)
{
}

// --------------------------------------------------------------------------------------
// Spieler auf neues Spiel vorbereiten
// --------------------------------------------------------------------------------------

void PlayerClass::InitPlayer(void)
{
	BronsonCounter			= 0.0f;
	Score					= 0;
	Stage					= 1;
	NewStage				= 1;
	Lives					= 3;
	Armour					= MAX_ARMOUR;
	Energy					= MAX_ENERGY;
	yspeed					= 0.0f;
	yadd					= 0.0f;

	// Waffen initialisieren
	PowerLines				= 3;
	Grenades				= 3;
	SmartBombs				= 1;
	SelectedWeapon			= 0;

	CollectedDiamonds		= 0;

	int i = 0;
/*
	for(i=0; i<4; i++)
		CollectedPowerUps[i] = 0;
*/

	for(i=0; i<4; i++)
	{
		CurrentWeaponLevel[i] = 0;
//		NextWeaponLevel	  [i] = 3;
	}

	CurrentWeaponLevel[0] = 1;

	// Blitz
	BlitzLength			  = 3;			// Aktuelle
	CurrentWeaponLevel[3] = 2;			// Länge
	//NextWeaponLevel	  [3] = 3;			// Wieviel PowerUps zum nächsten Level ?

	AutoFireCount   = 0.0f;
	AutoFireExtra   = 0.0f;
	RiesenShotExtra = 0.0f;
	
	// Spielt der Spieler das TutorialLevel? Dann alle Extra-Waffen auf Null setzen
	//
	if (RunningTutorial == true)
	{
		Skill = 0;
		PowerLines = 0;
		Grenades   = 0;
		SmartBombs = 0;
	}

	GodMode = false;
	WheelMode = false;
	InitNewLevel();
	AutoScrollspeed = 0.0f;
}

// --------------------------------------------------------------------------------------
// Spieler auf neues Level vorbereiten
// --------------------------------------------------------------------------------------

void PlayerClass::InitNewLevel()
{
	if (Handlung == TOT)
		return;

	BronsonCounter			= 0.0f;
	Handlung				= STEHEN;
	JumpAdd					= 0.0f;
	AnimPhase				= 0;
	AnimCount				= 0.0f;
	xspeed				    = 0.0f;
	yspeed					= 0.0f;
	xadd					= 0.0f;
	yadd					= 0.0f;
	Blickrichtung			= RECHTS;
	JumpPossible			= true;
	CollideRect.left		= 20;
	CollideRect.top			= 12;
	CollideRect.right		= 50;
	CollideRect.bottom		= 80;
	Energy					= MAX_ENERGY;
	Armour					= MAX_ARMOUR;

	// Waffen initialisieren
	ShotDelay				= 0.0f;
	BlitzWinkel				= 0.0f;
	BlitzCount				= 0.0f;
	BlitzAnim				= 0;
	PowerLinePossible		= true;

	AufPlattform			= NULL;
	InLiquid				= false;
	JumpedOnSurfboard		= false;

	DamageCounter			= 0.0f;
	Shield					= 0.0f;

	GameOverTimer			= 0.0f;

	FlameTime				= 0.0f;

	SecretThisLevel			= 0;
	DiamondsThisLevel		= 0;
	LivesThisLevel			= 0;
	BlocksThisLevel			= 0;

	CanBeDamaged			= true;
	WalkLock				= true;

	FesteAktion = -1;
	DoFesteAktion = false;
	StageClearRunning = false;


	// Zu Beginn des Levels werden alle Listen mit Gegner, Schüssen und Partikeln gelöscht
	pPartikelSystem->ClearAll();
	pGegner->ClearAll();
	pProjectiles->ClearAll();		
}

// --------------------------------------------------------------------------------------
// Spieler explodiert gerade
// --------------------------------------------------------------------------------------

void PlayerClass::runExplode(void)
{
	ExplodingTimer -= 1.0f SYNC;

	// Zuende explodiert?
	//
	if (ExplodingTimer <= 0.0f)
	{
		// Rechteck wieder herstellen
		//
		CollideRect.left		= 20;
		CollideRect.top			= 12;
		CollideRect.right		= 50;
		CollideRect.bottom		= 79;

		// Dann ein Leben abziehen
		Lives--;
		if (Lives < 0)
			Handlung = TOT;		

		// Aktuell eingestellte Waffe um eine Stufe verringern
		//
		if (CurrentWeaponLevel[SelectedWeapon] > 1)
			CurrentWeaponLevel[SelectedWeapon]--;

		// Blitz um eine Stufe verkürzen
		//
		if (CurrentWeaponLevel[3] > 2)
			CurrentWeaponLevel[3]--;

		// Powerlines wieder minimal auf Spieleranzahl setzen
		if (PowerLines < NUMPLAYERS)
			PowerLines = NUMPLAYERS;

		// Granaten wieder minimal auf Spieleranzahl setzen
		if (Grenades < NUMPLAYERS)
			Grenades = NUMPLAYERS;

		// SmartBombs wieder minimal auf 1 setzen
		//if (SmartBombs < 1)
		//	SmartBombs = 1;

		if (pTileEngine->Zustand == ZUSTAND_SCROLLBAR &&
			Handlung != SACKREITEN &&
			Handlung != DREHEN)
		{
			xpos    = JumpxSave;	// Alte Position wieder herstellen, wenn der
			ypos    = JumpySave;	// der Spieler zB einen Abgrund runterfiel
			xposold = JumpxSave;
			yposold = JumpySave;
		}			

		// Spieler hat Game Over ?
		if (pPlayer[0]->Lives < 0 &&
		   (NUMPLAYERS == 1 ||
			pPlayer[1]->Lives < 0))
		{
			pPlayer[0]->Lives = -1;
			pPlayer[1]->Lives = -1;
			pPlayer[0]->GameOverTimer = 50.0f;
			pSoundManager->StopSong(MUSIC_STAGEMUSIC, false);
			pSoundManager->StopSong(MUSIC_FLUGSACK, false);
			pSoundManager->StopSong(MUSIC_BOSS, false);
			pSoundManager->PlaySong(MUSIC_GAMEOVER, false);
			pMenu->AktuellerZustand = 0;
		}

		// oder wird wieder gespanwed
		else
		if (Lives >= 0)
		{
			// Spieler hat kurze Zeit lang Schild
			//if (Shield <= 0.0f)
				pProjectiles->PushProjectile (xpos, ypos, SHIELDSPAWNER, this);
				pProjectiles->PushProjectile (xpos, ypos, SHIELDSPAWNER2, this);

			Shield += 20.0f;			

			// Energie und Rad-Energie wieder auf voll setzen
			Energy = MAX_ENERGY;
			Armour = MAX_ARMOUR;

			// Im Fahrstuhllevel? Dann auf dem Fahrstuhl wieder anfangen
			//
			if (g_Fahrstuhl_yPos > -1.0f)
			{
				xpos = (float)(pTileEngine->XOffset) + 320.0f - 35;
				ypos = (float)(g_Fahrstuhl_yPos) - CollideRect.bottom - 1;
				xposold = xpos;	// Alte Position wieder herstellen, wenn der
				yposold = ypos;	// der Spieler zB einen Abgrund runterfiel
				JumpxSave = xpos;
				JumpySave = ypos;
			}
		}

		if (Lives < -1)
			Lives = -1;

		if (Handlung != SACKREITEN &&
			Handlung != DREHEN &&
			Handlung != TOT)
			Handlung = SPRINGEN;

		yspeed   = 0.0f;
		JumpAdd  = 0.0f;
	}
}

// --------------------------------------------------------------------------------------
// will der Spieler schiessen?
// --------------------------------------------------------------------------------------

void PlayerClass::checkShoot(void)
{
	if (!Aktion[AKTION_SHOOT] &&
		FlameThrower == true &&
		pSoundManager->its_Sounds[SOUND_FLAMETHROWER + SoundOff]->isPlaying)
		pSoundManager->StopWave(SOUND_FLAMETHROWER + SoundOff);

	// Normaler Schuss mit Primärwaffe?
	//
	if(Aktion[AKTION_SHOOT]  &&
	   Handlung != BLITZEN   &&
	   Handlung != BEAMLADEN &&
	   Handlung != DREHEN    &&
	   AutoFireCount > 0.0f  &&
	   ShotDelay <= 0.0f)
	{
		ShotDelay = PLAYER_SHOTDELAY;
		PlayerShoot();

		if (Handlung != LAUFEN	    && Handlung != SPRINGEN && 
			Handlung != SACKREITEN  && Handlung != RADELN   &&
			Handlung != RADELN_FALL &&
			WalkLock == true)
			AnimPhase = 1;		

		// TODO immer Dauerfeuer?
		/*// Nur bei "Easy" Dauerfeuer
		// ansonsten Autofire Count verringern
		//
		if (Skill > 0)
			AutoFireCount -= 1.0f;*/

		if (AutoFireExtra > 0.0f)
			ShotDelay /= 2.25f;

		// Beim Sackreiten den Spieler durch den Rückschlag noch bewegen
		//
		if (Handlung == SACKREITEN)
			if (Blickrichtung == LINKS)
				xadd += 2.0f;
			else
				xadd -= 2.0f;

		// Beim Surfen genauso ;)
		//
		if (Handlung == SURFEN     ||
			Handlung == SURFENJUMP ||
			Handlung == SURFENCROUCH)
				xadd -= 2.0f;
	}

	// Granate abfeuern ?
	//
	if (Aktion[AKTION_GRANATE] == true  && 
		PowerLinePossible == true       && 
		Grenades > 0		&&
		Handlung != RADELN				&&
		Handlung != RADELN_FALL			&&
		Handlung != BLITZEN				&&
		Handlung != DREHEN				&&
		Handlung != BEAMLADEN)
	{
		PlayerGrenadeShoot();
	}

	// Kann man wieder schiessen ?
	//
	if (Aktion[AKTION_SHOOT] == false)
	{
		ShotDelay     = 0.0f;

		if (AutoFireCount < 3.0f)
			AutoFireCount = 3.0f;
	}

	if (ShotDelay > 0.0f)
		ShotDelay -= float(1.0 SYNC);
	else
		ShotDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// Autofire abhandeln
// --------------------------------------------------------------------------------------

void PlayerClass::handleAutoFire (void)
{
	// Eingesammeltes Autofire runterzählen
	//
	if (AutoFireExtra > 0.0f)
	{
		AutoFireExtra -= 0.5f SYNC;
		AutoFireCount  = 3;
	}
	else
		AutoFireExtra = 0.0f;

	// Eingesammeltes RiesenShotExtra runterzählen
	//
	if (RiesenShotExtra > 0.0f)
		RiesenShotExtra -= 0.5f SYNC;
	else
		RiesenShotExtra = 0.0f;
}

// --------------------------------------------------------------------------------------
// Eingabegeräte abfragen und Spieler entsprechend reagieren lassen
// --------------------------------------------------------------------------------------

bool PlayerClass::GetPlayerInput(void)
{
	// tot?
	if (Handlung == TOT)
		return false;

	// explodiert der Spieler noch?
	//
	if (Handlung == EXPLODIEREN)
	{
		runExplode();
		return false;
	}

	// Zuerst alle Aktionen auf false setzen
	//
	for (int i=0; i<MAX_AKTIONEN; i++)
		Aktion[i] = false;

	// und Bewegungsgeschwindigkeit für den nächsten Frame auf 0 setzen,
	// es sei denn, man läuft auf Eis	
	if (pTileEngine->BlockUntenNormal	  (xpos, ypos, xposold, yposold, CollideRect) & BLOCKWERT_EIS) 
	{
		xspeed *= PLAYER_ICESSLOWDOWN;
	}
	else
		xspeed = 0.0f;

	// Aktionen nach Keyboard-Input abfragen falls der
	// Spieler sich gerade bewegen darf =)
	//
//	if (pTileEngine->Zustand != ZUSTAND_SCROLLTOPLAYER)
	{
		// Demo läuft ?
		if (DEMOPlaying == true)
			PlayDemo();

		// oder feste Aktion?
		else
		if (DoFesteAktion == true)
		{
			if (FesteAktion >= 0)
				Aktion[FesteAktion] = true;				
		}
		// oder Spieler spielt ?
		else
		{
			// Keyboard
			//if (ControlType == JOYMODE_KEYBOARD)
			{
				for (int i=0; i<MAX_AKTIONEN; i++)
				if (KeyDown(AktionKeyboard[i]))  
				{
					Aktion[i] = true;
					BronsonCounter = 0.0f;
				}
			}
			//else
			if (ControlType != JOYMODE_KEYBOARD &&
			    JoystickFound == true)
			{
				// Zum laufen die Achse links/rechts auslesen
				if (DirectInput.Joysticks[JoystickIndex].JoystickX >  JoystickSchwelle) Aktion[AKTION_RECHTS] = true;
				if (DirectInput.Joysticks[JoystickIndex].JoystickX < -JoystickSchwelle) Aktion[AKTION_LINKS]  = true;
				if (DirectInput.Joysticks[JoystickIndex].JoystickY >  JoystickSchwelle) Aktion[AKTION_DUCKEN] = true;

				if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 1 &&	DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 3) Aktion[AKTION_RECHTS] = true;
				if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 5 &&	DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 7) Aktion[AKTION_LINKS]  = true;
				if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >  4500 * 3 &&	DirectInput.Joysticks[JoystickIndex].JoystickPOV <  4500 * 5) Aktion[AKTION_DUCKEN] = true;

				// Zum umsehen die Achsen hoch/runter auslesen (nur mit Joypad)
				//
				if (JoystickMode == JOYMODE_PAD)
				{
					if (DirectInput.Joysticks[JoystickIndex].JoystickY >  JoystickSchwelle &&
						AktionJoystick [AKTION_UNTEN] == -1)						
						Aktion[AKTION_UNTEN] = true;

					if (DirectInput.Joysticks[JoystickIndex].JoystickY < -JoystickSchwelle) Aktion[AKTION_OBEN]  = true;

					if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 7 ||	
					   (DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 1 && DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 0)) 
					   Aktion[AKTION_OBEN]  = true;

					//if (DirectInput.Joysticks[JoystickIndex].JoystickPOV >  4500 * 3 &&	DirectInput.Joysticks[JoystickIndex].JoystickPOV <  4500 * 5) Aktion[AKTION_UNTEN] = true;
				}

				// Joystick buttons auslesen
				for (int i = AKTION_LINKS; i <= AKTION_WAFFEN_CYCLE; i++)
					if (DirectInput.Joysticks[JoystickIndex].JoystickButtons[AktionJoystick [i]] == true)
						Aktion[i] = true;

				// Mit Joystick springt man, indem man nach oben drückt
				//
				if (JoystickMode == JOYMODE_STICK)
				{
					// Spieler kann im Joystick Mode nicht mit dem Button springen
					//
					Aktion[AKTION_JUMP] = false;

					if (DirectInput.Joysticks[JoystickIndex].JoystickY < -JoystickSchwelle ||
						DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 4500 * 7 ||	
					   (DirectInput.Joysticks[JoystickIndex].JoystickPOV <= 4500 * 1 && DirectInput.Joysticks[JoystickIndex].JoystickPOV >= 0))
						Aktion[AKTION_JUMP] = true;
				}
			}

			// Bewegungen aufnehmen ?
			if (DEMORecording == true)
				RecordDemo();
		}
	}

	// Spieler läuft ins Exit?
	if (pPlayer[0]->StageClearRunning == true)
			RunPlayerExit();

	// Spieler schiesst?
	//
	checkShoot();

	// Autofire und Riesenshot Extra abgandeln
	//
	handleAutoFire();
	
	// waffe wechseln?
	checkWeaponSwitch();


	for (int i=0; i<MAX_AKTIONEN; i++)
		if (Aktion[i])
		{
			if (Handlung == PISSEN)
			{
				pGUI->HideBoxFast(); 
				BronsonCounter = 0.0f;
				Handlung = STEHEN;
			}			
		}

	// Bronson-Counter erhöhen
	if ((Handlung == STEHEN ||
		 Handlung == PISSEN) &&
		 RunningTutorial == false)
		BronsonCounter += 1.0f SYNC;
	else
	{
		BronsonCounter = 0.0f;

		if (Handlung == PISSEN)
			Handlung = STEHEN;
	}

	// und evtl auch mit Bronson loslegen =)
	//
	if (BronsonCounter > 200.0f)
	{
		if (Handlung != PISSEN)
		{
			AnimCount = 0.0f;
			AnimPhase = 0;
			Handlung = PISSEN;
		}
	}
	
	return true;
}

// --------------------------------------------------------------------------------------
// Funken und Rauch erzeugen, wenn der Spieler angeschlagen ist
// --------------------------------------------------------------------------------------

void PlayerClass::DoStuffWhenDamaged(void)
{
	if (pConsole->Showing == true ||
		Handlung == EXPLODIEREN)
		return;

	static float sparkcount = 0.0f;
	static float smokecount = 0.0f;

	// Noch viel Energie? Dann gleich wieder raus
	//
	if (Energy > MAX_ENERGY / 2)
		return;

	// Funkenzähler runterzählen
	//
	if (sparkcount > 0.0f)
		sparkcount -= 1.0f SYNC;
	else
	{
		sparkcount = (float)(rand()%(int)(Energy / 2 + 2)) + 5;

		// ein Funken "Schadenseffekt" per Zufall einbauen
		//
		int effect = rand()%3;

		switch (effect)
		{
			// Normale Funken
			//
			case 0:
			{
				float x = xpos + (float)(20 + rand()%40);
				float y = ypos + (float)(20 + rand()%40);

				for (int i = 0; i < 5; i++)
					pPartikelSystem->PushPartikel(x + rand()%4, y + rand()%4, FUNKE);

				pPartikelSystem->PushPartikel(x - 20, y - 20, LASERFLAME);
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_FUNKE);
			}
			break;

			// Lange Funken
			//
			case 1:
			{
				float x = xpos + (float)(20 + rand()%40);
				float y = ypos + (float)(20 + rand()%40);

				for (int i = 0; i < 5; i++)
					pPartikelSystem->PushPartikel(x + rand()%4, y + rand()%4, LONGFUNKE);

				pPartikelSystem->PushPartikel(x - 20, y - 20, LASERFLAME);
				pSoundManager->PlayWave(100, 128, 8000 + rand()%4000, SOUND_FUNKE);
			}
			break;

		}
	}	

	// Rauchzähler runterzählen
	//
	if (smokecount > 0.0f)
		smokecount -= 1.0f SYNC;
	else
	{
		smokecount = 0.8f;

		// Rauch per Zufall
		//		
		if (Energy < MAX_ENERGY / 3 + 5.0f)
		if (rand()%2 == 0)
			pPartikelSystem->PushPartikel(xpos + 10 + rand()%30, ypos + 20 + rand()%40, SMOKE2);

		// Rauchsäule
		//
		if (Energy < MAX_ENERGY / 4 + 5.0f)
			pPartikelSystem->PushPartikel(xpos + 26 + Blickrichtung * 4 + rand()%4, ypos + 20 + rand()%4, SMOKE3);

	}



}

// --------------------------------------------------------------------------------------
// Energie testen, ob <= 0. Dann explodieren lassen
// --------------------------------------------------------------------------------------

void PlayerClass::CheckForExplode(void)
{
	if (Energy		  <= 0.0f		 && 
		Handlung	  != EXPLODIEREN &&
		GameOverTimer  == 0.0f)
	{
		if (pSoundManager->its_Sounds[SOUND_ABZUG + SoundOff]->isPlaying == true)
			pSoundManager->StopWave(SOUND_ABZUG + SoundOff);

		if (pSoundManager->its_Sounds[SOUND_BLITZ + SoundOff]->isPlaying == true)
			pSoundManager->StopWave(SOUND_BLITZ + SoundOff);

		if (pSoundManager->its_Sounds[SOUND_BEAMLOAD + SoundOff]->isPlaying == true)
			pSoundManager->StopWave(SOUND_BEAMLOAD + SoundOff);

		DirectInput.Joysticks[JoystickIndex].StopForceFeedbackEffect(FFE_BLITZ);

		// Screen bei evtl. Ruckeln begradigen
		D3DXMATRIX		matRot;				// Rotationsmatrix
		WackelMaximum = 0.0f;
		WackelValue   = 0.0f;
		D3DXMatrixRotationZ  (&matRot, 0.0f);
		lpD3DDevice->SetTransform(D3DTS_WORLD, &matRot);

		CollideRect.left   = 0;
		CollideRect.right  = 0;
		CollideRect.top    = 0;
		CollideRect.bottom = 0;

		// Spieler explodieren lassen und Gegnern dabei Schaden zufügen
		//
		pSoundManager->PlayWave (100, 128, 11025, SOUND_EXPLOSION2);
		pGegner->DamageEnemiesonScreen (xpos + 35, ypos + 40, 400);

		// Piss-Meldung verschwinden lassen
		pGUI->HideBoxFast(); 
		BronsonCounter = 0.0f;

		Shield = 0.0f;

		ShakeScreen (5);

		for (int i = 0; i < 3; i++)
			pPartikelSystem->PushPartikel (xpos + 10 + rand () % 50, 
										   ypos + 10 + rand () % 50, SPLITTER);

		for (int i = 0; i < 10; i++)
			pPartikelSystem->PushPartikel (xpos + 10 + rand () % 50, 
										   ypos + 10 + rand () % 50, SPIDERSPLITTER);


		pPartikelSystem->PushPartikel (xpos + 35 - 90, ypos + 40 - 90, EXPLOSION_GIGA);
		pPartikelSystem->PushPartikel (xpos + 35, ypos + 40, SHOCKEXPLOSION);
		pPartikelSystem->PushPartikel (xpos - 20, ypos - 20, EXPLOSIONFLARE);		

		pPartikelSystem->PushPartikel (xpos + 20, ypos + 10, HURRITEILE_KOPF);
		pPartikelSystem->PushPartikel (xpos + 20, ypos + 10, HURRITEILE_ARM1);
		pPartikelSystem->PushPartikel (xpos + 20, ypos + 10, HURRITEILE_ARM2);
		pPartikelSystem->PushPartikel (xpos + 20, ypos + 10, HURRITEILE_BEIN1);
		pPartikelSystem->PushPartikel (xpos + 20, ypos + 10, HURRITEILE_BEIN2);
		pPartikelSystem->PushPartikel (xpos + 20, ypos + 10, HURRITEILE_WAFFE);
		pPartikelSystem->PushPartikel (xpos + 20, ypos + 10, HURRITEILE_TORSO);

		pPartikelSystem->PushPartikel (xpos - 88, ypos - 88, GRENADEFLARE);

		ExplodingTimer = 30.0f;

		// Punisher verschwinden lassen
		GegnerClass *pTemp;
		GegnerPunisher *pPunisher;

		pTemp = pGegner->pStart;

		while (pTemp != NULL)
		{
			if (pTemp->GegnerArt == PUNISHER)
			{
				pPunisher = (GegnerPunisher*)pTemp;
				pPunisher->Vanish();
			}

			pTemp = pTemp->pNext;
		}

		if (Handlung == SACKREITEN ||
			Handlung == DREHEN)
		{			
			// Von unten wieder hochkommen ?
			if (ypos >= pTileEngine->YOffset + 475.0f)
			{
				ypos = float (pTileEngine->YOffset + 470.0f);
				yadd   = -25.0f;
			}

			ExplodingTimer = 0.0f;
			pSoundManager->PlayWave (100, 128, 11025, SOUND_EXPLOSION2);

			for (int i = 0; i < 15; i++)
				pPartikelSystem->PushPartikel(xpos - 10 + rand()%80,
										 	  ypos - 10 + rand()%80,
											  EXPLOSION_MEDIUM2);

			for (int i = 0; i < 40; i++)
				pPartikelSystem->PushPartikel(xpos + rand()%90,
										 	  ypos + rand()%90,
											  LONGFUNKE);
			runExplode();
		}
		else
		if (Handlung != TOT)
			Handlung = EXPLODIEREN;

		DirectInput.Joysticks[JoystickIndex].ForceFeedbackEffect(FFE_BIGRUMBLE);
		InLiquid = false;
	}
}

// --------------------------------------------------------------------------------------
// Items in Reichweite anziehen
// --------------------------------------------------------------------------------------

void PlayerClass::PullItems(void)
{
	GegnerClass* pTemp;

	pTemp = pGegner->pStart;

	while (pTemp != NULL)
	{
		// Item ist ein Extra? Dann ansaugen
		if (pTemp->GegnerArt == EXTRAS &&
			pTemp->Handlung == GEGNER_STEHEN &&
			pTemp->IsOnScreen())
		{
			float absx, absy, speed;				// Variablen für die Geschwindigkeits-
													// berechnung
			absx = (pTemp->xPos+10)-(xpos+35);		// Differenz der x 
			absy = (pTemp->yPos+10)-(ypos+40);		// und y Strecke
				
			speed = (float)(1.0f/sqrt(absx*absx + absy*absy));	// Länge der Strecke berechnen
			speed = speed * 0.1f * BlitzStart;				// Geschwindigkeit ist 4 fach

			pTemp->xSpeed = -speed * 10.0f * absx SYNC;
			pTemp->yPos -= speed * absy SYNC;
		}

		pTemp = pTemp->pNext;
	}
}

// --------------------------------------------------------------------------------------
// Spieler animieren
// --------------------------------------------------------------------------------------

void PlayerClass::AnimatePlayer(void)
{
	// Pause beim Explodieren
	//
	if (Handlung == EXPLODIEREN ||
		Handlung == TOT)
		return;

	static float	look;		// hoch / runter sehen	

	bu = 0;

	bl = pTileEngine->BlockLinks	  (xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
	br = pTileEngine->BlockRechts	  (xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);

	// auf schrägen laufen/gelandet?
	//
	if (yspeed   >= 0.0f)
		bu = pTileEngine->BlockSlopes     (xpos, ypos, xposold, yposold, CollideRect, yspeed);

	if (Handlung == RADELN)
	{
		if ((Blickrichtung == LINKS   && bu & BLOCKWERT_SCHRAEGE_R) ||
			(Blickrichtung == RECHTS  && bu & BLOCKWERT_SCHRAEGE_L))
		{
			bu = pTileEngine->BlockUnten (xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
			yspeed = -2.0f;
			ypos -= 1.0f;
			
		}
	}

	// normaler Boden
	//
	if (!(bu & BLOCKWERT_SCHRAEGE_L) &&
		!(bu & BLOCKWERT_SCHRAEGE_R))
	{
		bu = pTileEngine->BlockUnten	  (xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
		bu = pTileEngine->BlockUntenNormal(xpos, ypos, xposold, yposold, CollideRect);
	}
	else
	{
		if (Handlung == SPRINGEN)
		{
			Handlung = STEHEN;
			yspeed   = 0.0f;
			JumpAdd  = 0.0f;			
		}
	}

	bo = pTileEngine->BlockOben		  (xpos, ypos, xposold, yposold, CollideRect, true);

	// Spieler unter Wasserfall ? Dann Wassertropfen entstehen lassen
	//
	for (int i = int (xpos + CollideRect.left) + 5; i < int (xpos + CollideRect.right) - 5; i+=2)
	{
		if (pTileEngine->Tiles [int (i / 20)][int (ypos + CollideRect.top + 10) / 20].Block & BLOCKWERT_WASSERFALL)
		{
			if (rand()%50 == 0)	
				pPartikelSystem->PushPartikel (float (i) + rand()%4, ypos + CollideRect.top + rand()%4 + 10, WASSERTROPFEN);

			if (rand()%200 == 0)	
				pPartikelSystem->PushPartikel (float (i) + rand()%4, ypos + CollideRect.top + rand()%4 + 10, WATERFUNKE);

			if (rand()%200 == 0)	
				pPartikelSystem->PushPartikel (float (i) + rand()%4 - 16, ypos + CollideRect.top + rand()%4 + 10 - 16, WATERFLUSH);
		}
	}

	// Alte Position für Kollisionsabfrage sichern
	xposold = xpos;
	yposold = ypos;

	// Auf Fliessband ?
	if ((bo & BLOCKWERT_FLIESSBANDL ||
		 bu & BLOCKWERT_FLIESSBANDL) &&
	  !(bl & BLOCKWERT_WAND)) 
		xpos -= 11.0f SYNC;

	if ((bo & BLOCKWERT_FLIESSBANDR ||
		 bu & BLOCKWERT_FLIESSBANDR) &&
	  !(br & BLOCKWERT_WAND)) 
		xpos += 11.0f SYNC;	

	//---------------------------------------------------------------------------
	// Spieler im normalen Modus, also kein Rad und nicht auf dem FlugSack reiten
	//---------------------------------------------------------------------------

	if (Handlung != RADELN		&&
		Handlung != RADELN_FALL &&
		Handlung != SACKREITEN  &&
		Handlung != SURFEN      &&
		Handlung != SURFENCROUCH&&
		Handlung != SURFENJUMP  &&
		Handlung != DREHEN)
	{
		// Stehen animieren?
		if (Handlung == STEHEN)
		{
			AnimCount += 1.0f SYNC;

			if (AnimCount > 1.0f)
			{
				AnimCount -= 1.0f;
				AnimPhase++;

				if (AnimPhase >= (FRAMES_IDLE * 3) + 44 - 1)
					AnimPhase = 0;
			}
		}

		// Langsam die Rad-Energie wieder auffüllen ?		
		if (Armour < 0.0f)
			Armour = 0.0f;

		Armour += 0.2f SYNC;

		// Oder schneller?
		if (Handlung == STEHEN ||
			Handlung == SCHIESSEN_O)			
			Armour += 1.0f SYNC;

		if (Armour > MAX_ARMOUR)
			Armour = MAX_ARMOUR;

		// Richtung umkehren wenn an die Decke gestossen ?
		if(bo & BLOCKWERT_WAND ||
		  (pTileEngine->Zustand == ZUSTAND_LOCKED &&
		   ypos <= (float)pTileEngine->YOffset))
		{
			if (yspeed < 0.0f)
				yspeed = 0.0f;
			JumpAdd = PLAYER_JUMPADDSPEED;
		}

		// Nach Links laufen/springen oder blitzen
		if (Aktion[AKTION_LINKS]  &&	// Links gedrückt ?
		   !Aktion[AKTION_RECHTS])	 	// und Rechts nicht ?

		if (Handlung == BLITZEN)		// Blitzen und dabei den Blitz bewegen ?
		{
			if (BlitzStart >= PLAYER_BLITZ_START)	// Bewegen schon möglich ?
				BlitzWinkel -= 20 SYNC;
		}
		else
		if (Handlung == BEAMLADEN)		// Rundum bewegen und den Beam aufladen ?
		{
			BlitzWinkel -= 20 SYNC;
		}
		else
		if(!(bl & BLOCKWERT_WAND))	// Keine Wand im Weg ?
		{
			Blickrichtung = LINKS;					// nach links kucken
			if (Handlung == STEHEN)					// Aus dem Stehen heraus 
				//Aktion[AKTION_OBEN]  == false &&	// und nicht nach oben zielen ?
				//Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
				AnimPhase = 0;						// das laufen starten

			if (Handlung != SPRINGEN &&				// Nicht in der Luft 				
				Handlung != DUCKEN)					// und nicht ducken ?
				//Aktion[AKTION_OBEN]  == false &&	// und nicht nach oben zielen ?
				//Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
				Handlung  = LAUFEN;					// Dann laufen wir =)

			if (Handlung != DUCKEN)
			{
				if (InLiquid == false)
				{
					// auf Eis?
					if (bu & BLOCKWERT_EIS)
					{
						xspeed -= PLAYER_ICESPEED SYNC;

						if (xspeed < -PLAYER_MOVESPEED)
							xspeed = -PLAYER_MOVESPEED;
					}
					else

					// oder normal
					xspeed = -PLAYER_MOVESPEED;		
				}
				else
					xspeed = -PLAYER_MOVESPEED*2/3;	// im Wasser langsamer

				if (GegnerDran)
					xspeed *= 0.5f;
			}

			if (Handlung != SPRINGEN &&
				Aktion[AKTION_OBEN]  &&
				WalkLock == true)
			   
				xspeed = 0.0f;
		}

		// Nach Rechts laufen/springen oder blitzen
		if (Aktion[AKTION_RECHTS] &&		// Rechts gedrückt ?
		   !Aktion[AKTION_LINKS])			// und Links nicht ?
		if (Handlung == BLITZEN)			// Blitzen und dabei den Blitz bewegen ?
		{
			if (BlitzStart >= PLAYER_BLITZ_START)	// Bewegen schon möglich ?
				BlitzWinkel += 20 SYNC;
		}
		else
		if (Handlung == BEAMLADEN)		// Rundum bewegen und den Beam aufladen ?
		{
			BlitzWinkel += 20 SYNC;
		}
		else
		if(br | BLOCKWERT_WAND)	// Keine Wand im Weg ?
		{
			Blickrichtung = RECHTS;					// nach rechts kucken
			if (Handlung == STEHEN)					// Aus dem Stehen heraus 
				//Aktion[AKTION_OBEN]  == false &&	// nicht nach oben zielen ?
				//Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
				AnimPhase = 0;						// das laufen starten

			if (Handlung != SPRINGEN &&				// Nicht in der Luft 
				Handlung != DUCKEN )				// und nicht ducken ?
				//Aktion[AKTION_OBEN]  == false &&		// nicht nach oben zielen ?
				//Aktion[AKTION_UNTEN] == false)		// und nicht nach unten zielen ?
				Handlung  = LAUFEN;					// Dann laufen wir =)

			if (Handlung != DUCKEN)
			{
				if (InLiquid == false)
				{
					// auf Eis?
					if (bu & BLOCKWERT_EIS)
					{
						xspeed += PLAYER_ICESPEED SYNC;

						if (xspeed > PLAYER_MOVESPEED)
							xspeed = PLAYER_MOVESPEED;
					}
					else

					// oder normal
					xspeed = PLAYER_MOVESPEED;		
				}
				else
					xspeed = PLAYER_MOVESPEED*2/3;	// im Wasser langsamer

				if (GegnerDran)
					xspeed *= 0.5f;
			}

			if (Handlung != SPRINGEN &&
				Aktion[AKTION_OBEN]  &&
				WalkLock == true)
				xspeed = 0.0f;
		}

		// Wand im Weg ? Dann stehenbleiben
		if((Aktion[AKTION_LINKS]  && (bl & BLOCKWERT_WAND) ||	
		    Aktion[AKTION_RECHTS] && (br & BLOCKWERT_WAND)) &&
			!(bu & BLOCKWERT_SCHRAEGE_L) &&
			!(bu & BLOCKWERT_SCHRAEGE_R))
		   
		{
			if (Handlung == LAUFEN)
			{
				Handlung  = STEHEN;
				AnimPhase = 0;
			}
			xspeed	  = 0.0f;
		}

		// Rechts/Links gedrückt ? stehenbleiben, da man ja nich rückwärts UND vorwärts gehen kann =)
		if(Aktion[AKTION_LINKS] && Aktion[AKTION_RECHTS])
		{
			if ((Handlung != SPRINGEN || JumpPossible == true) &&
				Handlung != BLITZEN	&&
				Handlung != BEAMLADEN)
			{
				AnimPhase = 0;
				Handlung  = STEHEN;
			}
		}

		// Bei keiner Tastatureingabe steht der Spieler ruhig da
		if (!Aktion[AKTION_LINKS]	&& 
			!Aktion[AKTION_RECHTS]	&& 
			Handlung != SPRINGEN	&&
			Handlung != BLITZEN		&&
			Handlung != PISSEN		&&
			Handlung != BEAMLADEN   &&
			ShotDelay <= 0.5f)
		{
			if (Handlung != SPRINGEN)
				Handlung  = STEHEN;						
			//AnimPhase = 0;
		}

		// im Sumpf langsam einsinken
		int busumpf = pTileEngine->BlockUnten(xpos, ypos, xposold, yposold, CollideRect, yspeed >= 0.0f);
		if (busumpf & BLOCKWERT_SUMPF ||
			bl & BLOCKWERT_SUMPF ||
			br & BLOCKWERT_SUMPF)
		{
			if (!Aktion[AKTION_JUMP])
				JumpPossible = true;

			if (!(busumpf &  BLOCKWERT_WAND) &&
				yspeed >= 0.0f)
				ypos += 4.0f SYNC;

			if (Handlung == SPRINGEN &&
				yspeed >= 0.0f)
				Handlung = LAUFEN;				
		}

		// Springen (nur, wenn nicht schon in der Luft)
		if (Aktion[AKTION_JUMP] && 
			JumpPossible == true)
		{
			int save = AnimPhase;				// Für Sumpf

			pSoundManager->StopWave (SOUND_BEAMLOAD + SoundOff);
			JumpPossible = false;
			AnimPhase = 2;						// das springen starten
			Handlung  = SPRINGEN;
			JumpStart = ypos;
			yspeed    = -PLAYER_MAXJUMPSPEED;
			
			JumpAdd	  =  0.0f;

			// Gegner dran festgebissen? Dann kann man nicht so hoch springen
			if (GegnerDran)
			{
				yspeed *= 0.5f;
				JumpAdd = PLAYER_JUMPADDSPEED;
			}

			if (AufPlattform != NULL)
			{
				GegnerMushroom *pTemp;

				if (AufPlattform->GegnerArt == MUSHROOM)
				{
					pTemp = (GegnerMushroom*)(AufPlattform);
					pTemp->PlayerJumps(this);
				}

				AufPlattform = NULL;
			}

			// Blubbersound noch beim Springen im Wasser
			if (InLiquid == true &&
				pSoundManager->its_Sounds[SOUND_DIVE]->isPlaying == false)
				pSoundManager->PlayWave(100, rand()%255, 8000 + rand()%4000, SOUND_DIVE);			

			// Im Sumpf? Dann ein paar Pixel anheben und wieder runterfallen
			if (bu & BLOCKWERT_SUMPF)
			{
				ypos -= 15.0f;

				// noch nicht aus dem Sumpf draussen? Dann gleich wieder runterfallen
				bu = pTileEngine->BlockSlopes     (xpos, ypos, xposold, yposold, CollideRect, yspeed);
				if (bu & BLOCKWERT_SUMPF)
				{
					//yspeed = 0.0f;
					JumpPossible = false;
					AnimPhase = save;
					//Handlung = STEHEN;
				}
			}
		}

		// Ist ein Sprung möglich ? (Wenn der Spieler wieder auf den Boden aufkommt)
		if ((bu & BLOCKWERT_SUMPF)		||
			(bu & BLOCKWERT_WAND)		||
			(bu & BLOCKWERT_PLATTFORM)  ||
			(bu & BLOCKWERT_SCHRAEGE_L)  ||
			(bu & BLOCKWERT_SCHRAEGE_R)  ||
			AufPlattform != NULL)
		{
			if (!(bu & BLOCKWERT_SCHRAEGE_L) &&
				!(bu & BLOCKWERT_SCHRAEGE_R))
			{
				if (yspeed > 0.0f)
				{
					// Beim Blitz weiterblitzen, ansonsten auf "STEHEN" schalten
					//
					if (Handlung != BLITZEN)
					{
						Handlung = STEHEN;
						AnimPhase = 0;
					}
					
					// stehen bleiben
					yspeed = 0.0f;
					pSoundManager->PlayWave(100, 128, 11025, SOUND_LANDEN);
					pPartikelSystem->PushPartikel(xpos+20, ypos+60, SMOKE);
				}				
			}
			else
				bu = pTileEngine->BlockSlopes     (xpos, ypos, xposold, yposold, CollideRect, yspeed);

			if (JumpPossible		== false &&
				Aktion[AKTION_JUMP] == false &&
				yspeed >= 0.0f)
				JumpPossible = true;			
		}	

		// Kein Block unter dem Spieler und kein Lift? Dann fällt er runter
		if (AufPlattform == NULL     &&
			Handlung != SPRINGEN     && 
			yspeed >= 0.0f			 &&
			(!(bu & BLOCKWERT_WAND)) &&
			(!(bu & BLOCKWERT_PLATTFORM)) &&
			(!(bu & BLOCKWERT_SUMPF)) &&
			(!(bl & BLOCKWERT_SCHRAEGE_L)) &&
			(!(bl & BLOCKWERT_SCHRAEGE_R)) &&
			(!(br & BLOCKWERT_SCHRAEGE_L)) &&
			(!(br & BLOCKWERT_SCHRAEGE_R)) &&

			(!(bu & BLOCKWERT_SCHRAEGE_L)) &&
			(!(bu & BLOCKWERT_SCHRAEGE_R))) 
		{
			JumpPossible = false;

			if (Handlung != BLITZEN)
			{
				Handlung  = SPRINGEN;
				AnimPhase = 0;				
				//yspeed    = 0.5f;
			}

			if ((Handlung == BLITZEN &&
				 yspeed == 0.0f))
				 yspeed = 0.5f;

			JumpAdd   = PLAYER_JUMPADDSPEED;
		}

		// Am Boden und nicht im Sumpf oder Damage-Tile? Dann die Position sichern
		if (Handlung != SPRINGEN &&
			AufPlattform == NULL &&
			!(bo & BLOCKWERT_SUMPF) &&
			!(bu & BLOCKWERT_SUMPF) &&
			!(bl & BLOCKWERT_SUMPF) &&
			!(br & BLOCKWERT_SUMPF) &&
			!(bo & BLOCKWERT_SCHADEN) &&
			!(bu & BLOCKWERT_SCHADEN) &&
			!(bl & BLOCKWERT_SCHADEN) &&
			!(br & BLOCKWERT_SCHADEN))
		{
			JumpxSave = xpos;
			JumpySave = ypos;
		}				

		// Vom Ducken aufstehen
		// nur, wenn keine Decke über dem Spieler (wenn er grad mit ner Plattform wo drunter durchfährt z.B.)
		if (Handlung == DUCKEN)			
		{
			float ypos2 = ypos - 48.0f;
			int bo2 = pTileEngine->BlockOben  (xpos, ypos2, xposold, yposold, CollideRect, true);

			if (!(bo  & BLOCKWERT_WAND) &&
				!(bo2 & BLOCKWERT_WAND))
				Handlung = STEHEN;
		}

		CollideRect.top = 12;	// Blockierung wieder wie im Stehen

		// oder gleich wieder geduckt bleiben ?
		if (Aktion[AKTION_DUCKEN] == true  &&
			//Aktion[AKTION_LINKS]  == false &&
			//Aktion[AKTION_RECHTS] == false &&
			JumpPossible == true		&&
			Handlung != BEAMLADEN		&&
			Handlung != BLITZEN)
		{
			Handlung = DUCKEN;
			CollideRect.top = 32;	// Im Ducken liegt die obere Blockgrenze tiefer
		}

		// Blitz schiessen ? (nur, wenn am Boden)
		if (Aktion[AKTION_BLITZ] && JumpPossible == true)
		{
			// Blitz aufladen?
			if (Aktion[AKTION_SHOOT] && Handlung != BEAMLADEN)
			{
				pSoundManager->PlayWave (100, 128, 9000, SOUND_BEAMLOAD + SoundOff);

				DirectInput.Joysticks[JoystickIndex].ForceFeedbackEffect(FFE_BLITZ);

				Handlung   = BEAMLADEN;
				BlitzStart = 5.0f;
				AnimPhase  = 0;
				if (Blickrichtung == LINKS)			// Blitz je nach Blickrichtung neu
					BlitzWinkel = 270;				// geradeaus richten
				else 
					BlitzWinkel = 90;

				if (Aktion[AKTION_OBEN])
				{
					Blickrichtung *= -1;
					BlitzWinkel = 0;
				}
			}

			else

			if (Handlung != BLITZEN && Handlung != BEAMLADEN)			// Blitz wird erst gestartet ?
			{
				BlitzStart = 0.0f;
				BeamCount  = 0.1f;
				AnimPhase = 0;
				Handlung  = BLITZEN;
				changecount = 0.0f;

				// Blitz Startsound ausgeben
				pSoundManager->PlayWave(100, 128, rand()%500+18025, SOUND_BLITZSTART + SoundOff);

				if (Blickrichtung == LINKS)			// Blitz je nach Blickrichtung neu
					BlitzWinkel = 270;				// geradeaus richten
				else 
					BlitzWinkel = 90;
			}
		}

		if (Aktion[AKTION_BLITZ] == false && Handlung == BLITZEN)
		{			
			// Im Fallen? Dann in Sprunganimation wechseln
			//
			if (yspeed > 0.0f)
			{
				Handlung = SPRINGEN;				
			}

			// Im Stehen? Dann je nach aktueller Blickrichtung
			// richtig hinstellen
			//
			else
			{
				if (AnimPhase >= 9 &&
					AnimPhase <= 18)
					Blickrichtung *= -1;

				Handlung = STEHEN;
			}

			AnimPhase = 0;			
		}

		// In das Rad verwandeln ?
		if (JumpPossible			 ==	true &&
			Aktion[AKTION_POWERLINE] == true && 
			Aktion[AKTION_DUCKEN]    == true &&
			Armour > 5.0f)
		{
			Handlung  = RADELN;
			AnimPhase = 0;
			AnimCount = 0.0f;
			CollideRect.left		= 20;	// Neue Abmessungen für das Rad
			CollideRect.top			= 40;
			CollideRect.right		= 50;
			CollideRect.bottom		= 79;
			PowerLinePossible		= false;
		}

		// Geloopten Blitzsound ausgeben
		if (Handlung == BLITZEN)
		{
			// Blitz Sound abspielen, wenn er nicht schon abgespielt wird
			if (BlitzStart >= PLAYER_BLITZ_START &&
				true == pSoundManager->InitSuccessfull &&
			    pSoundManager->its_Sounds[SOUND_BLITZ + SoundOff]->isPlaying == false)
			{
			    pSoundManager->PlayWave(100, 128, 11025, SOUND_BLITZ + SoundOff);
				DirectInput.Joysticks[JoystickIndex].ForceFeedbackEffect(FFE_BLITZ);
			}
		}
		else
		if (true == pSoundManager->InitSuccessfull &&
			pSoundManager->its_Sounds[SOUND_BLITZ + SoundOff]->isPlaying == true)
		{
			pSoundManager->StopWave(SOUND_BLITZ + SoundOff);
			pSoundManager->PlayWave(100, 128, rand()%1000+11025, SOUND_BLITZENDE + SoundOff);
			DirectInput.Joysticks[JoystickIndex].StopForceFeedbackEffect(FFE_BLITZ);
		}

		// Spieler nach BlitzRichtung ausrichten
		if (Handlung == BLITZEN)
		{
			// Verzögerung beim Blitzen
			if (BlitzStart < PLAYER_BLITZ_START)
				BlitzStart += 1.0f SYNC;
			else
			{
				float Winkel;

				Winkel = BlitzWinkel - 270;		// 270° beim nach links kucken = Animphase 0
				if (Winkel < 0.0f)
					Winkel += 360.0f;

				// Je nach Richtung Winkel umdrehen
				if (Blickrichtung == LINKS)
					Winkel = 360.0f-Winkel;
				else
					Winkel = 180 + Winkel;				

				AnimPhase = (int)((Winkel + 0.0f) / 10.0f) % FRAMES_SURROUND;					
			}
		}

		// Beam Laden und richtige Animationsphase setzen
		//
		if (Handlung == BEAMLADEN)
		{
			int Freq = 9000 + (int)(BlitzStart / 20.0f * 2000);

			//PullItems();

			FSOUND_SetFrequency(pSoundManager->its_Sounds[SOUND_BEAMLOAD + SoundOff]->Channel, Freq);

			// Beam aufladen. Je länger der Blitz desto schneller lädt der Beam
			if (BlitzStart < PLAYER_BEAM_MAX)
				BlitzStart += CurrentWeaponLevel[3] * 1.0f SYNC;

			float Winkel;

				Winkel = BlitzWinkel - 270;		// 270° beim nach links kucken = Animphase 0
				if (Winkel < 0.0f)
					Winkel += 360.0f;

				// Je nach Richtung Winkel umdrehen
				if (Blickrichtung == LINKS)
					Winkel = 360.0f-Winkel;
				else
					Winkel = 180 + Winkel;				

				AnimPhase = (int)((Winkel + 0.0f) / 10.0f) % FRAMES_SURROUND;					

			// Beam abfeuern
			if (!Aktion[AKTION_BLITZ] || !Aktion[AKTION_SHOOT])
			{
				Handlung = STEHEN;
				pSoundManager->StopWave(SOUND_BEAMLOAD + SoundOff);
				DirectInput.Joysticks[JoystickIndex].StopForceFeedbackEffect(FFE_BLITZ);

				if (BlitzStart >= 20.0f)
				{
					pProjectiles->PushBlitzBeam(int(BlitzStart), BlitzWinkel, this);
					pSoundManager->PlayWave(100, 128, 7000, SOUND_BLITZENDE + SoundOff);
					pSoundManager->PlayWave(100, 128, 10000 + rand()%2000, SOUND_BLITZENDE + SoundOff);
					pSoundManager->PlayWave(100, 128, 10000, SOUND_EXPLOSION3);
				}

				// je nach Ausrichtung der Waffe in die richtige Richtung kucken
				//
				if (AnimPhase >= 12 &&
					AnimPhase <= 24)
					Blickrichtung *= -1;
			}
		}

		// Piss-Animation
		//
		if (Handlung == PISSEN)
		{
			AnimCount += 1.0f SYNC;

			if (AnimCount > 1.0f)
			{
				AnimCount = 0.0f;
				AnimPhase++;

				if (AnimPhase > 2)
					AnimCount = -1.0f;

				if (AnimPhase == 11)
					AnimCount = -2.0f;
			}

			if (AnimPhase == 16)
				AnimPhase = 3;
		}
	}

	else

	//--------------------
	// Als Rad rumkullern
	//--------------------

	if (Handlung == RADELN ||
		Handlung == RADELN_FALL)

	{
		// Rad-Energie abziehen
		if (WheelMode == false)
			Armour -= float(2.5 SYNC);

		if (Armour < 0.0f)
			Armour = 0.0f;

		// Powerline schiessen ?
		if (Aktion[AKTION_POWERLINE] == true  && 
			PowerLinePossible		 == true  && PowerLines > 0)
		{
			pSoundManager->PlayWave(100, 128, 11025, SOUND_POWERLINE);
			PowerLinePossible = false;			// Taste einrasten
			PowerLines--;						// Powerlines verringern

			for (int i=-1; i<25; i++)			// Powerlines schiessen
			{
				pProjectiles->PushProjectile (xpos+20, float(int(pTileEngine->YOffset/20)*20 + i*20), POWERLINE, this);
				pProjectiles->PushProjectile (xpos+20, float(int(pTileEngine->YOffset/20)*20 + i*20), POWERLINE2, this);
			}
		}

		// SmartBomb abfeuern ?
		if (Aktion[AKTION_SMARTBOMB] == true  && 
			PowerLinePossible == true && SmartBombs > 0)
		{
			pSoundManager->PlayWave(100, 128, 8000, SOUND_POWERLINE);
			PowerLinePossible = false;			// Taste einrasten
			SmartBombs--;						// SmartBombs verringern

			pProjectiles->PushProjectile (xpos+40-32, ypos+55-32, SMARTBOMB, this);
		}

		// Taste losgelassen ? Dann können wir wieder eine Powerline schiessen
		if (Aktion[AKTION_POWERLINE] == false &&
			Aktion[AKTION_SMARTBOMB] == false)
			PowerLinePossible = true;

		// Ja nach Blickrichtung in die richtige Richtung scrollen
		if (Blickrichtung == LINKS && (!(bl & BLOCKWERT_WAND)))
		{
			if (InLiquid == false)
				xspeed = -40.0f;
			else
				xspeed = -30.0f;
		}
		else
		if (Blickrichtung == RECHTS && (!(br & BLOCKWERT_WAND)))
		{
			if (InLiquid == false)
				xspeed = 40.0f;
			else
				xspeed = 30.0f;
		}

		if (Aktion[AKTION_LINKS] == true &&		// Nach Links rollen ?
			(Handlung == RADELN   ||			// Nur wenn man Boden unter den Füßen hat
			 AufPlattform != NULL ||
			 bu & BLOCKWERT_WAND  ||
			 bu & BLOCKWERT_PLATTFORM))				
			Blickrichtung = LINKS;

		if (Aktion[AKTION_RECHTS] == true &&	// Nach Rechts rollen ?
			(Handlung == RADELN   ||			// Nur wenn man Boden unter den Füßen hat
			 AufPlattform != NULL ||
			 bu & BLOCKWERT_WAND  ||
			 bu & BLOCKWERT_PLATTFORM))				
			Blickrichtung = RECHTS;

		if (Handlung != RADELN_FALL &&
			(!(bu & BLOCKWERT_SCHRAEGE_R)) &&
			(!(bu & BLOCKWERT_SCHRAEGE_L)) &&
			(!(bu & BLOCKWERT_WAND)) &&
			(!(bu & BLOCKWERT_PLATTFORM))) 
		{
			Handlung  =	RADELN_FALL;
			yspeed    = 0.5f;
			JumpAdd   = PLAYER_JUMPADDSPEED;
		}

		// Wieder am Boden aufgekommen ?
		if (Handlung == RADELN_FALL &&
		   ((AufPlattform != NULL) ||
		    (bu & BLOCKWERT_WAND) ||
			(bu & BLOCKWERT_PLATTFORM)))
		{
			if (yspeed > 2.0f)
				pPartikelSystem->PushPartikel(xpos+20, ypos+60, SMOKE);

			if (yspeed > 0.0f)
			{				
				yspeed = -yspeed*2/3;				// Abhopfen

				if (yspeed > -5.0f)					// oder ggf wieder
				{									// Normal weiterkullern
					yspeed    = 0.0f;
					JumpAdd   = 0.0f;
					Handlung  = RADELN;
					pTileEngine->BlockUnten(xpos, ypos, xposold, yposold, CollideRect, true);
				}
				else
				{
					AufPlattform = NULL;				
					pSoundManager->PlayWave(100, 128, 11025, SOUND_LANDEN);
				}
			}
		}

		// An die Wand gestossen ? Dann Richtung umkehren
		if (!(bu & BLOCKWERT_SCHRAEGE_R) &&
			!(bu & BLOCKWERT_SCHRAEGE_L))
		{
			if (Blickrichtung == RECHTS && (br & BLOCKWERT_WAND))
				Blickrichtung = LINKS;

			if (Blickrichtung == LINKS && (bl & BLOCKWERT_WAND))
				Blickrichtung = RECHTS;
		}

		// Aufhören zu kullern und zurückverwandeln, wenn man springt oder keine Energie mehr hat
		// funktiomiert nur, wenn über einem keine Wand ist. Dabei wird noch ein Teil mehr gecheckt, als
		// das Rad hoch ist, damit man den Zustand prüft, als wenn der Spieler wieder stehen würde

		float ypos2 = ypos - 20.0f;
		int   bo2 = pTileEngine->BlockOben  (xpos, ypos2, xposold, yposold, CollideRect, true);

		if ((Armour <= 0.0f ||
			 Aktion[AKTION_JUMP])   &&
			!(bo  & BLOCKWERT_WAND) &&
			!(bo2 & BLOCKWERT_WAND))
		{
			AnimPhase = 0;
			
			// Am Boden zurückverwandelt ?
			if (Armour <= 0.0f &&
				((bu & BLOCKWERT_WAND) ||
				 (bu & BLOCKWERT_PLATTFORM)))
				Handlung  = STEHEN;		

			// oder in der Luft bzw durch Sprung
			else
				Handlung  = SPRINGEN;	// Nein, dann in der Luft

			CollideRect.left		= 20;
			CollideRect.top			= 12;
			CollideRect.right		= 50;
			CollideRect.bottom		= 79;
			if (Armour < 0.0f)
				Armour = 0.0f;
		}
	}

	else

	//------------------------
	// Auf dem FlugSack reiten
	//------------------------

	if (Handlung == SACKREITEN ||
		Handlung == DREHEN)
	{
		// Spieler und Level bewegen (wenn nicht beim Endboss bzw wenn Level scrollbar)
		// Nur, wenn alle Spieler auf einem Flugsack sitzen
		//
		bool BeideFrei = true;

		for (int p = 0; p < NUMPLAYERS; p++)
			if (pPlayer[p]->FesteAktion > -1)
				BeideFrei = false;		

		if (FlugsackFliesFree == false &&
			Riding() && 
			BeideFrei == true)
			ypos -= PLAYER_FLUGSACKSPEED SYNC;
		
		JumpySave = ypos;
		JumpxSave = xpos;

		// unten rausgeflogen ? Dann wieder zurücksetzen
		if (ypos > pTileEngine->YOffset + 475.0f)
		{
			pSoundManager->PlayWave (100, 128, 11025, SOUND_EXPLOSION2);
			xpos    = JumpxSave;	// Alte Position wieder herstellen, wenn der
			ypos    = JumpySave;	// der Spieler zB einen Abgrund runterfiel
			xposold = JumpxSave;
			yposold = JumpySave;
			Energy  = 0.0f;
		}

		// An der Decke anstoßen
		if (ypos < pTileEngine->YOffset && 
			BeideFrei)
		{
			if (yadd < 0.0f)
			yadd *= -1.0f;
		}

		// Wände checken
		if (bl & BLOCKWERT_WAND) if (xadd < 0.0f) xadd = -xadd / 2.0f;
		if (br & BLOCKWERT_WAND) if (xadd > 0.0f) xadd = -xadd / 2.0f;
		if (bo & BLOCKWERT_WAND) if (yadd < 0.0f) yadd = -yadd / 2.0f;
		if (bu & BLOCKWERT_WAND) if (yadd > 0.0f) yadd = -yadd / 2.0f;

		// Rauch am Flugsack erzeugen
		SmokeCount -= 1.0f SYNC;
		if (SmokeCount < 0.0f)
		{
			SmokeCount += 0.1f;

			// Beim Reiten
			if (Handlung == SACKREITEN)
			{	
				if (Blickrichtung == LINKS)
					pPartikelSystem->PushPartikel(xpos + 72, ypos + 100, FLUGSACKSMOKE2);
				else
					pPartikelSystem->PushPartikel(xpos - 2,  ypos + 100, FLUGSACKSMOKE);
			}

			// oder beim Drehen
			if (Handlung == DREHEN)
			{	
				if (Blickrichtung == LINKS)
				{
					pPartikelSystem->PushPartikel(xpos + 70 - AnimPhase * 10, ypos + 100, FLUGSACKSMOKE2);
					pPartikelSystem->PushPartikel(xpos + 87 - AnimPhase * 10, ypos + 100, FLUGSACKSMOKE);
				}
				else
				{
					pPartikelSystem->PushPartikel(xpos + 4  + (AnimPhase-10) * 10,  ypos + 100, FLUGSACKSMOKE);
					pPartikelSystem->PushPartikel(xpos - 20 + (AnimPhase-10) * 10,  ypos + 100, FLUGSACKSMOKE2);
				}
			}
		}

		// Umdrehen
		if (Handlung == DREHEN)
		{
			AnimCount += 1.2f SYNC;

			if (AnimCount > 0.6f)
			{
				AnimCount -= 0.6f;
				AnimPhase++;

				// Zuende gedreht ?
				if (AnimPhase == AnimEnde)
				{
					AnimEnde = 0;
					Handlung = SACKREITEN;

					if (Blickrichtung == RECHTS)
					{
						AnimPhase = 0;
						Blickrichtung = LINKS;
					}
					else
					{
						AnimPhase = 10;
						Blickrichtung = RECHTS;
					}
				}
			}
		}

		// Links fliegen
		if (Aktion[AKTION_LINKS] == true && Aktion[AKTION_RECHTS]== false)
		{
			xadd -= 10.0f SYNC;

			// Drehen ?
			if (Aktion[AKTION_SHOOT] == false &&
				Handlung      == SACKREITEN	  &&
				Blickrichtung == RECHTS)
			{
				Handlung  = DREHEN;
				AnimEnde  = 19;
				AnimCount = 0.0f;
				AnimPhase = 10;
			}
		}

		// Rechts fliegen
		if (Aktion[AKTION_RECHTS] == true && Aktion[AKTION_LINKS]== false)
		{
			xadd += 10.0f SYNC;

			// Drehen ?
			if (Aktion[AKTION_SHOOT] == false &&
				Handlung      == SACKREITEN   &&
				Blickrichtung == LINKS)
			{
				Handlung  = DREHEN;
				AnimEnde  = 9;
				AnimCount = 0.0f;
				AnimPhase = 0;
			}
		}

		// Hoch fliegen		
		if ((Aktion[AKTION_OBEN] == true ||
			 Aktion[AKTION_JUMP] == true) &&			
			 Aktion[AKTION_DUCKEN]== false)
				yadd -= 10.0f SYNC;

		// Runter fliegen
		if (Aktion[AKTION_DUCKEN] == true && 
			Aktion[AKTION_OBEN]== false)
			yadd += 10.0f SYNC;

		// Bewegung abbremsen
		if (!Aktion[AKTION_LINKS] && 			
			!Aktion[AKTION_RECHTS])
		{
			if (xadd < 0.0f) 
			{	
				xadd += 4.0f SYNC;
				if (xadd > 0.0f)
					xadd = 0.0f;
			}

			if (xadd > 0.0f) 
			{
				xadd -= 4.0f SYNC;
				if (xadd < 0.0f)
					xadd = 0.0f;
			}
		}
			
		if (!Aktion[AKTION_OBEN] &&
			!Aktion[AKTION_DUCKEN])
		{
			if (yadd < 0.0f) 
			{
				yadd += 4.0f SYNC;
				if (yadd > 0.0f)
					yadd = 0.0f;
			}

			if (yadd > 0.0f) 
			{
				yadd -= 4.0f SYNC;
				if (yadd < 0.0f)
					yadd = 0.0f;
			}
		}

		// Geschwindigkeitsgrenzen checken
		if (xadd >  25.0f) xadd =  25.0f;
		if (xadd < -25.0f) xadd = -25.0f;
		if (yadd >  25.0f) yadd =  25.0f;
		if (yadd < -25.0f) yadd = -25.0f;
		
		xspeed = xadd;
		ypos += yadd SYNC;

		// Wendepunkt erreicht? Dann automatisch abspringen
		float ytemp = ypos - 1;
		bu = pTileEngine->BlockUntenNormal(xpos, ypos, xposold, ytemp, CollideRect);
		if (bu & BLOCKWERT_WENDEPUNKT)
		{
			Handlung = SPRINGEN;
			AnimPhase = 0;
			yspeed = -PLAYER_MAXJUMPSPEED;
			JumpAdd = PLAYER_JUMPADDSPEED;			
			AufPlattform = NULL;

			// abstürzenden Flugsack adden
			pGegner->PushGegner(xpos, ypos + 20, FLUGSACK, 99, 0, false);

			FlugsackFliesFree = true;
			pTileEngine->Zustand = ZUSTAND_SCROLLBAR;

			pSoundManager->FadeSong(MUSIC_FLUGSACK, -2.0f, 0, true);
			pSoundManager->FadeSong(MUSIC_STAGEMUSIC, 2.0f, 100, true);
		}
	}

	//-------------------------
	// Auf dem SurfBrett stehen
	//-------------------------

	if (Handlung == SURFEN     ||
		Handlung == SURFENJUMP ||
		Handlung == SURFENCROUCH)
	{
		static float Antrieb = 0.0f;

		// Beim ersten Draufspringen geht das Board kurz ab
		if (JumpedOnSurfboard == true)
		{
			JumpedOnSurfboard = false;
			xadd = 30.0f;
		}

		// Spieler und Level bewegen (wenn nicht beim Endboss bzw wenn Level scrollbar)
		//
		xpos			     += AutoScrollspeed SYNC;
		pTileEngine->XOffset += AutoScrollspeed SYNC;

		if (Handlung == SURFENCROUCH)
			Handlung = SURFEN;

		// Ducken
		if (Aktion[AKTION_DUCKEN] == true  &&
			Aktion[AKTION_LINKS]  == false &&
			Aktion[AKTION_RECHTS] == false &&
			JumpPossible == true           &&			
			Handlung != BLITZEN)
		{
			Handlung = SURFENCROUCH;
			CollideRect.top = 32;	// Im Ducken liegt die obere Blockgrenze tiefer
		}

		// Springen (nur, wenn nich schon in der Luft)
		if (Aktion[AKTION_JUMP] && JumpPossible == true)
		{
			Handlung = SURFENJUMP;
		}

		// Links surfen
		if (Aktion[AKTION_LINKS] == true && Aktion[AKTION_RECHTS]== false)
			xadd -= 12.0f SYNC;

		// Rechts surfen
		if (Aktion[AKTION_RECHTS] == true && Aktion[AKTION_LINKS]== false)
		{
			xadd += 12.0f SYNC;
			Antrieb -= 1.0f SYNC;

			// Antriebsflamme erzeugen
			//
			if (Antrieb < 0.0f)
			{
				pPartikelSystem->PushPartikel (xpos - 10, ypos + 60 + rand()%6, ROCKETSMOKE);
				Antrieb += 0.1f;
			}
		}

		// Bewegung abbremsen
		if (!Aktion[AKTION_LINKS] && !Aktion[AKTION_RECHTS]) xadd *= 0.99f;

		// Geschwindigkeitsgrenzen checken
		if (xadd >  25.0f) xadd =  25.0f;
		if (xadd < -25.0f) xadd = -25.0f;
		
		xspeed = xadd;
	}	

	//----------------------
	// Powerline schiessen ?
	//----------------------

	if (Aktion[AKTION_POWERLINE] == true  && 
		Aktion[AKTION_DUCKEN]    == false &&
		PowerLinePossible		 == true  && PowerLines > 0)
	{
		pSoundManager->PlayWave(100, 128, 11025, SOUND_POWERLINE);
		PowerLinePossible = false;			// Taste einrasten
		PowerLines--;						// Powerlines verringern

		for (int i=-1; i<25; i++)			// Powerlines schiessen
		{
			pProjectiles->PushProjectile (xpos+20, float(int(pTileEngine->YOffset/20)*20 + i*20), POWERLINE, this);
			pProjectiles->PushProjectile (xpos+20, float(int(pTileEngine->YOffset/20)*20 + i*20), POWERLINE2, this);
		}
	}

	//---------------------
	// SmartBomb abfeuern ?
	//---------------------

	if (Aktion[AKTION_SMARTBOMB] == true  && 
		PowerLinePossible == true && SmartBombs > 0)
	{
		pSoundManager->PlayWave(100, 128, 8000, SOUND_POWERLINE);
		PowerLinePossible = false;			// Taste einrasten
		SmartBombs--;						// SmartBombs verringern

		pProjectiles->PushProjectile (xpos+40-32, ypos+35-32, SMARTBOMB, this);
	}

	//--------------------------------------------------------------------
	// Taste losgelassen ? Dann können wir wieder eine Powerline schiessen
	//--------------------------------------------------------------------

	if (Aktion[AKTION_POWERLINE] == false &&
		Aktion[AKTION_GRANATE]   == false &&
		Aktion[AKTION_SMARTBOMB] == false)
		PowerLinePossible = true;

	// --------------------------------
	// Hoch / runter kucken bzw. zielen
	// --------------------------------

	// nach oben zielen
	if (Aktion[AKTION_OBEN])
	{
		if (Handlung == STEHEN	     ||
			Handlung == SCHIESSEN_O  ||
			Handlung == SCHIESSEN_LO ||
			Handlung == SCHIESSEN_RO ||
			Handlung == LAUFEN)
		{
			if (Aktion[AKTION_LINKS])
			{
				Handlung      = SCHIESSEN_LO;
				Blickrichtung = LINKS;
			}
			else
			if (Aktion[AKTION_RECHTS])
			{
				Handlung      = SCHIESSEN_RO;
				Blickrichtung = RECHTS;
			}
			else
				Handlung = SCHIESSEN_O;
		}
	}

	if ((!Aktion[AKTION_OBEN]	&&
		 !Aktion[AKTION_UNTEN]) ||
		 NUMPLAYERS ==2 ||
		 pTileEngine->Zustand != ZUSTAND_SCROLLBAR)
		look = 0.0f;

	if (pTileEngine->Zustand == ZUSTAND_SCROLLBAR	  && 
		NUMPLAYERS < 2 &&
		Handlung != SACKREITEN &&
		Handlung != DREHEN &&
		Handlung != SURFEN &&
		Handlung != SURFENJUMP &&
		Handlung != SURFENCROUCH)
	{	
		// Spieler steht am Boden ?
		//
		
		if (Handlung != SPRINGEN)
		{
			// Hoch scrollen / nach oben zielen
			if (Aktion[AKTION_OBEN])
			{
				look += 1.0f SYNC;

				if (look > 5.0f)
				{
					if (pTileEngine->YOffset > ypos - 400.0f)
						pTileEngine->YOffset -= 19.0f SYNC;
					else
						pTileEngine->YOffset = ypos - 400.0f;
				}				
			}

			// Runter scrollen bzw. runter zielen
			if (Aktion[AKTION_UNTEN])
			{
				look += 1.0f SYNC;

				if (look > 5.0f ||
					AktionKeyboard[AKTION_UNTEN] != AktionKeyboard[AKTION_DUCKEN])
				{
					if (pTileEngine->YOffset < ypos - 40.0f)
						pTileEngine->YOffset += 19.0f SYNC;
					else
						pTileEngine->YOffset = ypos - 40.0f;
				}
			}
		}

		else
		{
			if (ypos-pTileEngine->YOffset < SCROLL_BORDER_EXTREME_TOP)
				pTileEngine->YOffset = ypos - SCROLL_BORDER_EXTREME_TOP;

			if (ypos-pTileEngine->YOffset > SCROLL_BORDER_EXTREME_BOTTOM &&
				yspeed > 0.0f)
				pTileEngine->YOffset = ypos - SCROLL_BORDER_EXTREME_BOTTOM;
		}

		// extrem Rand trotzdem nochmal checken
		if (Handlung != SACKREITEN &&
			Handlung != DREHEN)
		{	
			if (ypos-pTileEngine->YOffset < 40)  pTileEngine->YOffset = ypos - 40;
			if (ypos-pTileEngine->YOffset > 380) pTileEngine->YOffset = ypos - 380;			
		}
	}

	// Fahrstuhl hat andere Grenzen
	if (Riding() == false &&
		pTileEngine->IsElevatorLevel == true &&
		ypos-pTileEngine->YOffset > 300)
		pTileEngine->YOffset = ypos - 300;

	// -----------------
	// Spieler animieren
	// -----------------

	// Hat der Spieler ein Schutzschild ?

	if (Shield > 0.0f)
		Shield -= 0.4f SYNC;

	//-----------------------------------------------
	// Energie abziehen

	if ((bo & BLOCKWERT_SCHADEN) ||
		(bu & BLOCKWERT_SCHADEN) ||
		(bl & BLOCKWERT_SCHADEN) ||
		(br & BLOCKWERT_SCHADEN))
	{
		switch (Skill) 
		{
			case 0:
				DamagePlayer(10.0f SYNC);
			break;

			case 1:
				DamagePlayer(20.0f SYNC);
			break;

			case 2:
				DamagePlayer(30.0f SYNC);
			break;

			case 3:
				DamagePlayer(40.0f SYNC);
			break;
		}
		

		if ((Handlung == RADELN ||
			 Handlung == RADELN_FALL) &&
			 WheelMode == false)
			Armour -= 5.0f SYNC;
	}


	//-----------------------------------------------
	// Testen, ob sich der Spieler im Wasser befindet
	//
	int middle = pTileEngine->Tiles[(int)(xpos + 35) / TILESIZE_X][(int)(ypos + 40) / TILESIZE_Y].Block;
	int spritzertype = 0;
	if ((bu & BLOCKWERT_LIQUID) ||
		(br & BLOCKWERT_LIQUID) ||
		(bl & BLOCKWERT_LIQUID) ||
		(bo & BLOCKWERT_LIQUID) ||
		middle & BLOCKWERT_LIQUID)
	{
		spritzertype = WASSER_SPRITZER2;

		// Gerade erst in Flüssigkeit gesprungen ?
		if (InLiquid == false)
		{
			if (Handlung == SPRINGEN ||
				Handlung == RADELN_FALL)
			{
				for (int i=0; i<12; i++)
					pPartikelSystem->PushPartikel(xpos+18+rand()%20, ypos + CollideRect.bottom-25, spritzertype);

				WinkelUebergabe = -1.0f;
				for (int i=0; i<15; i++)
					pPartikelSystem->PushPartikel(xpos+10+rand()%40, ypos + CollideRect.bottom + rand()%20, BUBBLE);
			}

			InLiquid = true;
			pSoundManager->PlayWave(100, 128, 10000 + rand()%2050, SOUND_WATERIN);			
		}		
	}
	else
	{
		// Aus dem Wasser heraus gehopst ?
		if (InLiquid == true)
		{
			// Welcher Liquid Type?
			//if (bu & BLOCKWERT_WASSER) spritzertype = WASSER_SPRITZER;
			//if (bu & BLOCKWERT_LAVA)   spritzertype = LAVA_SPRITZER;
			spritzertype = WASSER_SPRITZER2;

			if (Handlung == SPRINGEN ||
				Handlung == RADELN_FALL)
			{
				for (int i=0; i<12; i++)
					pPartikelSystem->PushPartikel(xpos+10+rand()%20, ypos + CollideRect.bottom-25, spritzertype);
			}

			pSoundManager->PlayWave(100, 128, 10000 + rand()%2050, SOUND_WATEROUT);
			InLiquid = false;
		}		
	}

	// Spieler im Wasser 
	if (InLiquid   == true)
	{
		//Blubberblasen entstehen lassen
		if (rand()%500 == 0)
			pPartikelSystem->PushPartikel(xpos+30, ypos+20, BUBBLE);

		// ggf noch Tauchgeräusche abspielen
		if (rand()%500 == 0 &&
			pSoundManager->its_Sounds[SOUND_DIVE]->isPlaying == false)
			pSoundManager->PlayWave(100, rand()%255, 8000 + rand()%4000, SOUND_DIVE);
	}	

	// schräg laufen?
	if ((Handlung == SCHIESSEN_LO ||
		 Handlung == SCHIESSEN_RO) &&
		 WalkLock == false)
	{
		// Nächste Animations-Phase ?
		AnimCount += 1.0f SYNC;
		while (AnimCount > PLAYER_ANIMSPEED)
		{			
			AnimCount = AnimCount - PLAYER_ANIMSPEED;

			AnimPhase++;
					
			if (AnimPhase >= FRAMES_RUN)
				AnimPhase = 0;	// Loop
		}
	}

	// Normal laufen oder radeln?
	if (Handlung == LAUFEN      ||					// Spieler läuft oder
		Handlung == RADELN      ||					// oder kullert
		Handlung == RADELN_FALL ||					// oder fällt als Rad
	   (Handlung == SPRINGEN &&						// springt, sprich, wird animiert ?
	    yspeed >-PLAYER_MAXJUMPSPEED/1.5f))			
	{						
		if (InLiquid == false)
			AnimCount += 1.0f SYNC;					// Dann animieren, je nachdem, ob man im
		else										// Wasser ist oder nicht
			AnimCount += 0.5f SYNC; 				// verschieden schnell animieren

		if (Handlung == RADELN	||
			Handlung == RADELN_FALL)				// Als Rad schneller animieren, also
			AnimCount += 4.0f SYNC;					// einfach nochmal den Wert viermal dazu

		while (AnimCount > PLAYER_ANIMSPEED)
		{
			
			AnimCount = AnimCount - PLAYER_ANIMSPEED;

			switch(Handlung)						// Je nach Handlung anders animieren
			{
				case LAUFEN:						// Laufen animieren 
				{
					AnimPhase++;
					
					if (AnimPhase >= FRAMES_RUN)
						AnimPhase = 0;	// Loop
				} break;

				case SPRINGEN:						// Springen animieren wobei erst animiert
				{									// wird wenn der Spieler wieder am Fallen ist
					if (AnimPhase < FRAMES_JUMP - 1)
						AnimPhase++;						// Kein Loop
				} break;

				case RADELN:
				case RADELN_FALL:
				{
					if (AnimPhase < PLAYERANIM_RAD_ENDE)
						AnimPhase++;
					else
						AnimPhase=0;
				} break;

				default : break;
			}
		}
	}

	// Springen
	if (Handlung == SPRINGEN ||
	   (Handlung == BLITZEN && 
	    yspeed > 0.0f))
	{
		// Sprung-Geschwindigkeit manipulieren
		if (InLiquid == false)
			yspeed += JumpAdd SYNC;
		else
			yspeed += JumpAdd*2/3 SYNC;

		// y-Position manipulieren, wenn oben frei ist
		if (InLiquid == false)
			ypos	  += yspeed SYNC;		
		else
			ypos	  += yspeed*2/3 SYNC;		

		if (yspeed > PLAYER_MAXJUMPSPEED)	// Schnellste "Fall-Geschwindigkeit" erreicht ?
			yspeed = PLAYER_MAXJUMPSPEED;

		if (InLiquid == false)
		{
			if (ypos < JumpStart - PLAYER_MAXJUMP ||	// Höchster Punkt erreicht ? Dann den Fall
				Aktion[AKTION_JUMP] == false)			// dazu addieren
			{
				// Der Wert, wie schnell man wieder runterkommt, wird jetzt aus der Sprunghöhe errechnet, sodass
				// man bei einem kleinen Sprung viel schneller wieder runterkommt
				//
				JumpAdd = PLAYER_JUMPADDSPEED;
				JumpAdd = PLAYER_JUMPADDSPEED + abs (int ((160 - (JumpySave - ypos)) / 10.0f));

				if (JumpAdd > 18.0f)
					JumpAdd = 18.0f;
			}
		}
		else
		{
			if (ypos < JumpStart - (PLAYER_MAXJUMP+50) ||	// Höchster Punkt im Wasser liegt anders
				Aktion[AKTION_JUMP] == false)				// dazu addieren
				JumpAdd = PLAYER_JUMPADDSPEED;
		}
	}

	// Als Rad runterfallen
	if (Handlung == RADELN_FALL)
	{
		// Sprung-Geschwindigkeit manipulieren
		if (InLiquid == false)
			yspeed += JumpAdd SYNC;
		else
			yspeed += JumpAdd*2/3 SYNC;

		// y-Position manipulieren, wenn oben frei ist
		if (InLiquid == false)
			ypos	  += yspeed SYNC;		
		else
			ypos	  += yspeed*2/3 SYNC;		

		if (yspeed > PLAYER_MAXJUMPSPEED)	// Schnellste "Fall-Geschwindigkeit" erreicht ?
			yspeed = PLAYER_MAXJUMPSPEED;

		if(bo & BLOCKWERT_WAND)	// An die Decke gestossen ?
		{
			if (yspeed < 0.0f)					// Richtung umkehren
				yspeed = -yspeed/3;
			JumpAdd = PLAYER_JUMPADDSPEED;
		}
	}

	// Spieler "rutscht" weg, weil Level schräg steht?
	if (bu & BLOCKWERT_WAND ||
		bu & BLOCKWERT_PLATTFORM)
		xspeed += ScreenWinkel * 7.0f SYNC;

	// Level abhängig von der Spieler-Position scrollen
	if (xspeed != 0.0f)
	{
		// nur bewegen, wenn keine Wand im Weg ist
		if ((xspeed < 0.0f && !(bl & BLOCKWERT_WAND)) ||
			(xspeed > 0.0f && !(br & BLOCKWERT_WAND)))
			xpos += xspeed SYNC;								 // Spieler bewegen*/

		// Zwei Spieler Mode? Dann auf Screen beschränken
		if (NUMPLAYERS == 2 &&
			StageClearRunning == false)
		{
			if (xpos < pTileEngine->XOffset)  xpos = (float)pTileEngine->XOffset;
			if (xpos > pTileEngine->XOffset + 570) xpos = (float)pTileEngine->XOffset + 570;
		}
	}

	/*if (Handlung != SPRINGEN	&&
		Handlung != RADELN_FALL &&
		yspeed <= 0.0f)
		pTileEngine->YOffset -= PLAYER_MAXJUMPSPEED SYNC;*/


	if (NUMPLAYERS == 1 &&
		pTileEngine->Zustand == ZUSTAND_SCROLLBAR)
	{
 		if(xpos-pTileEngine->XOffset <  20) pTileEngine->XOffset = xpos - 20;
		if(xpos-pTileEngine->XOffset > 550)	pTileEngine->XOffset = xpos - 550;
	}	
}

// --------------------------------------------------------------------------------------
// Aktionen, wenn der Spieler auf einer Plattform steht
// --------------------------------------------------------------------------------------

void PlayerClass::DoPlattformStuff(void)
{
	if (AufPlattform == NULL)
		return;

	if (AufPlattform->GegnerArt == REITFLUGSACK)
		return;

	if (Handlung == SPRINGEN)
	{
		Handlung = STEHEN;
		yspeed   = 0.0f;
	}

	if (Aktion[AKTION_JUMP] == false)
		JumpPossible = true;			

	// Fahrstuhl über Boden gefahren? Dann bleibt der Spieler daran hängen
	if (bu & BLOCKWERT_WAND ||
		bu & BLOCKWERT_PLATTFORM)
	{
		AufPlattform = NULL;
	}
	else			
	{
		if (AufPlattform->GegnerArt == FAHRSTUHL)
			ypos = g_Fahrstuhl_yPos + GegnerRect[AufPlattform->GegnerArt].top - CollideRect.bottom;
		else
			ypos = AufPlattform->yPos + GegnerRect[AufPlattform->GegnerArt].top - CollideRect.bottom;
	}
}
 
// --------------------------------------------------------------------------------------
// Spieler an der aktuellen Position zeichnen, je nach Zustand
//
//	leuchten : Spieler additiv rendern
//  farbe    : mit Farbe "CurrentColor" rendern
// --------------------------------------------------------------------------------------

bool PlayerClass::DrawPlayer(bool leuchten, bool farbe)
{
	if (Energy <= 0.0f &&
		Handlung == TOT)
		return false;	

	float		xdraw, ydraw;
	D3DCOLOR	Color;

	if (leuchten)
		DirectGraphics.SetAdditiveMode();

	xdraw = (float)((int)(xpos) - (int)(pTileEngine->XOffset));
	ydraw = ypos - pTileEngine->YOffset;

	// Im Wasser? Dann schwabbeln lassen
	if (InLiquid == true &&
		bo & BLOCKWERT_LIQUID &&
		bu & BLOCKWERT_PLATTFORM)
	{
		int yLevel = (int)(ypos + 80.0f) / 20;
		int off = (int)(pTileEngine->SinPos2 + yLevel) % 40;

		xdraw -= pTileEngine->SinList2[off];
	}

	// Schaden genommen ? Dann Spieler blinken lassen
	if (DamageCounter > 0.0f)
		DamageCounter -= 5.0f SYNC;
	else 
		DamageCounter = 0.0f;

	if (DamageCounter == 0.0f &&
		true == pSoundManager->InitSuccessfull &&
		pSoundManager->its_Sounds[SOUND_ABZUG + SoundOff]->isPlaying == true)
		pSoundManager->StopWave(SOUND_ABZUG + SoundOff);

	Color = pTileEngine->LightValue(xpos, ypos, CollideRect, false);

	if (Handlung == BLITZEN)
		Color = Color | 0xFF0000FF;

	if (ShotDelay > 1.0f)
		Color = 0xFFFFFFFF;

	// Spieler nicht normal rendern (zB wenn er blinkt nach dem Waffeneinsammeln)
	// dann entsprechende Farbe setzen
	if (farbe)
		Color = CurrentColor;

//	if (this == pPlayer[1])
//		Color = 0xFFBBFFBB;

	bool blick = false;
	if (Blickrichtung == RECHTS)
		blick = true;	

//----- Spieler anzeigen
	switch (Handlung)
	{

// Spieler kullert (oder fällt) als Rad durch die Gegend
		case RADELN:
		case RADELN_FALL:
		{
			PlayerRad[SoundOff].RenderSprite(xdraw+17, ydraw+45, AnimPhase, Color, blick);
		} break;

// Spieler steht und macht nichts
		case STEHEN :						
		{
			if (Aktion[AKTION_UNTEN] &&
				!Aktion[AKTION_SHOOT] &&
				!Aktion[AKTION_GRANATE])
				PlayerKucken[SoundOff].RenderSprite(xdraw, ydraw, 1, Color, !blick);
			else
			{
				int a;

				if (AnimPhase < FRAMES_IDLE * 3)
				{				
					a = AnimPhase % FRAMES_IDLE;

					if (ShotDelay > 0.25f &&
						FlameThrower == false)
						PlayerIdle[SoundOff].RenderSprite(xdraw, ydraw, FRAMES_IDLE, Color, !blick);
					else
						PlayerIdle[SoundOff].RenderSprite(xdraw, ydraw, a, Color, !blick);
				}
				else
				{
					a = AnimPhase - FRAMES_IDLE * 3;
					PlayerIdle2[SoundOff].RenderSprite(xdraw, ydraw, a, Color, !blick);
				}
			}
				
		} break;

// Spieler zielt nach oben
		case SCHIESSEN_O :						
		{
			if (Aktion[AKTION_SHOOT] ||
				Aktion[AKTION_GRANATE])
			{
				if (ShotDelay > 0.25f &&
					FlameThrower == false)
					PlayerOben[SoundOff].RenderSprite(xdraw, ydraw, 1, Color, !blick);
				else
					PlayerOben[SoundOff].RenderSprite(xdraw, ydraw, 0, Color, !blick);
			}
			else
				PlayerKucken[SoundOff].RenderSprite(xdraw, ydraw, 0, Color, !blick);
		} break;

// Spieler zielt nach links oder rechts oben
		case SCHIESSEN_LO:
		case SCHIESSEN_RO:
		{
			// im Stehen schiessen?
			if (WalkLock == true)
			{
				if (ShotDelay > 0.25f &&
					FlameThrower == false)
					PlayerDiagonal[SoundOff].RenderSprite(xdraw, ydraw, 1, Color, !blick);
				else
					PlayerDiagonal[SoundOff].RenderSprite(xdraw, ydraw, 0, Color, !blick);
			}

/*
			// oder im Laufen?
			else
			{
				if (ShotDelay > 0.25f)
					PlayerRunDiagonal[SoundOff][AnimPhase].RenderSprite(xdraw, ydraw, 0, Color, !blick);
				else
					PlayerRunDiagonal[SoundOff][AnimPhase].RenderSprite(xdraw, ydraw, 0, Color, !blick);				
			}
*/
		} break;

// Spieler zielt nach links oder rechts unten
/*		case SCHIESSEN_LU:
		case SCHIESSEN_RU:
		{
			PlayerBlitz.RenderSprite(xdraw, ydraw, 4, Color, blick);
		} break;*/

// Spieler surft
		case SURFEN :						
		{
			PlayerSurf[SoundOff].RenderSprite(xdraw, ydraw, 0, Color, false);
		} break;

// Spieler springt bei surfen
		case SURFENJUMP :						
		{
			PlayerSurf[SoundOff].RenderSprite(xdraw, ydraw, 0, Color, false);
		} break;

// Spieler surft im Kien
		case SURFENCROUCH :						
		{
			PlayerSurf[SoundOff].RenderSprite(xdraw, ydraw, 1, Color, false);
		} break;


// Spieler springt
		case SPRINGEN :
		{
			if (Aktion[AKTION_OBEN])
			{
				if (Aktion[AKTION_LINKS] ||
					Aktion[AKTION_RECHTS])
					PlayerJumpDiagonal[SoundOff].RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
				else
					PlayerJumpUp[SoundOff].RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
			}
			else
				PlayerJump[SoundOff].RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);

		} break;

// Spieler läuft
		case LAUFEN :
		{
			PlayerRun[SoundOff].RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
		} break;

// Spieler muss brutal pullern ?
		case PISSEN:
		{
			PlayerPiss[SoundOff].RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
		} break;

// Spieler schiesst Blitz oder lädt den Beam
		case BLITZEN :
		case BEAMLADEN:
		{
			PlayerBlitz[SoundOff].RenderSprite(xdraw, ydraw, AnimPhase, Color, !blick);
		} break;

// Spieler duckt sich
		case DUCKEN :						
		{
			if (ShotDelay > 0.25f)
				PlayerCrouch[SoundOff].RenderSprite(xdraw, ydraw, 1, Color, !blick);
			else
				PlayerCrouch[SoundOff].RenderSprite(xdraw, ydraw, 0, Color, !blick);
		} break;

// Spieler reitet auf dem FlugSack
		case SACKREITEN :
		{
			if (Blickrichtung == LINKS)
				PlayerRide[SoundOff].RenderSprite(xdraw, ydraw, 0, Color);
			else
			if (Blickrichtung == RECHTS)
				PlayerRide[SoundOff].RenderSprite(xdraw, ydraw, 10, Color);
		} break;

		case DREHEN :
		{
			PlayerRide[SoundOff].RenderSprite(xdraw, ydraw,    AnimPhase,    Color);
		} break;

		default :
		break;
	}	

	//----- Schuss-Flamme anzeigen
	if (FlameTime > 0.0f  && 
		leuchten == false &&
		farbe    == false)
		
	{
		// Zeit der Darstellung verringern
		//
		FlameTime -= SpeedFaktor;				

		DirectGraphics.SetAdditiveMode();
		CalcFlamePos();

		int FlameOff = SelectedWeapon;

		if (FlameThrower)
			FlameOff = 1;

		if (Blickrichtung == RECHTS)
			SchussFlamme[AustrittAnim].RenderSprite(xpos + AustrittX, ypos + AustrittY, FlameAnim+FlameOff*2, 0xFFFFFFFF);
		else
			SchussFlamme[AustrittAnim].RenderSprite(xpos + AustrittX, ypos + AustrittY, FlameAnim+FlameOff*2, 0xFFFFFFFF, true);

		CalcAustrittsPunkt();

		if (options_Detail >= DETAIL_MEDIUM)
		{
			int FlameOff = SelectedWeapon;
			if (FlameThrower)
				FlameOff = 0;

			switch (FlameOff)
			{
				case 0 : 
					SchussFlammeFlare.RenderSprite(xpos + AustrittX - 70 - (float)pTileEngine->XOffset, 
												   ypos + AustrittY - 70 - (float)pTileEngine->YOffset, 0, 0x88FFCC99);
				break;

				case 1 : 
					SchussFlammeFlare.RenderSprite(xpos + AustrittX - 70 - (float)pTileEngine->XOffset, 
												   ypos + AustrittY - 70 - (float)pTileEngine->YOffset, 0, 0x8899CCFF);
				break;

				case 2 : 
					SchussFlammeFlare.RenderSprite(xpos + AustrittX - 70 - (float)pTileEngine->XOffset, 
												   ypos + AustrittY - 70 - (float)pTileEngine->YOffset, 0, 0x8899FFCC);
				break;
			}
		}

		DirectGraphics.SetColorKeyMode();
	}

	// Blitz anzeigen oder Beam aufladen
	//
	if (Handlung == BLITZEN &&
		AlreadyDrawn == false)
		DoLightning();

	if (Handlung == BEAMLADEN &&
		AlreadyDrawn == false)
		LoadBeam();	

	if (leuchten)
		DirectGraphics.SetColorKeyMode();

	// Funken und so Kram, wenn der Spieler verwundet ist
	//
	if (pConsole->Showing == false)
		DoStuffWhenDamaged();

	CurrentShotTexture = NULL;
	CurrentPartikelTexture = -1;

	AlreadyDrawn = true;

	return true;
}

// --------------------------------------------------------------------------------------
// Spieler mit aktueller Speed bewegen
// --------------------------------------------------------------------------------------

void PlayerClass::MovePlayer(void)
{
	// wenn der Spieler gerade fernegesteuert das Level verlässt dann gleich wieder raus
	if (DoFesteAktion == true ||
		Handlung == TOT)
		return;

	// Ränder überprüfen
	// links raus
	if (xpos < 0.0f)
		xpos = 0.0f;

	// rechts raus
	if (xpos > pTileEngine->LEVELPIXELSIZE_X - 80.0f)
		xpos = float(pTileEngine->LEVELPIXELSIZE_X - 80.0f);

	// unten raus
	if (ypos > pTileEngine->LEVELPIXELSIZE_Y)
		Energy = 0.0f;

	// im 2 Spieler-Mode: Ein Spieler fällt aus dem Screen?
	// Geht nur, wenn beide noch leben
	//
	if (NUMPLAYERS == 2 &&
		pPlayer[0]->Handlung != TOT &&
		pPlayer[1]->Handlung != TOT)
	{
		PlayerClass *pVictim = NULL;
		PlayerClass *pSurvivor = NULL;

		// Spieler 1 fällt unten raus?
		if (pPlayer[0]->ypos > pPlayer[1]->ypos + 480.0f)
		{
			pVictim   = pPlayer[0];
			pSurvivor = pPlayer[1];
		}

		// Spieler 2 fällt unten raus?
		if (pPlayer[1]->ypos > pPlayer[0]->ypos + 480.0f)
		{
			pVictim   = pPlayer[1];
			pSurvivor = pPlayer[0];
		}

		// Einer der Spieler fällt raus
		if (pVictim != NULL)
		{					
			pVictim->xpos = pSurvivor->xpos;
			pVictim->ypos = pSurvivor->ypos - 20.0f;
		}
	}



	// an die Decke
	if (ypos < 0.0f)
	{
		if (yspeed < 0.0f)
			yspeed = -yspeed * 2/3;
		else
			ypos = 0.0f;
	}

	// Ränder für gelockten Screen prüfen
	if (pTileEngine->Zustand != ZUSTAND_SCROLLBAR)
	{
		if (xpos < pTileEngine->XOffset)	 xpos = float(pTileEngine->XOffset);
		if (xpos > pTileEngine->XOffset+580) xpos = float(pTileEngine->XOffset+580);

		// Im Fahrstuhllevel?
		//
		if (g_Fahrstuhl_yPos > -1.0f)
		{
			// ja, dann checken ob Spieler ausserhalb des Screens, und wenn ja, dann
			// lassen wir ihn halt mal einfach so sterben. Das gehört sich ja auch nicht ;)
			//
			if (ypos + CollideRect.bottom < pTileEngine->YOffset ||
				ypos > pTileEngine->YOffset + 480.0f) 
				Energy = 0.0f;
		}
		else
		{
			if (ypos < pTileEngine->YOffset) 
				ypos = float(pTileEngine->YOffset);
		}
		
	}
}

// --------------------------------------------------------------------------------------
// Spieler schiesst mit Primär Waffe
// --------------------------------------------------------------------------------------

void PlayerClass::PlayerShoot(void)
{
	float wadd = 0.0f;
	int   yoff;

	CalcAustrittsPunkt();

	//----- Y-Offset der Patronenhülse ausrechnen

	// surfen
	if (Handlung == SURFEN ||
	    Handlung == SURFENJUMP)
		yoff = -13;
	else if (Handlung == SURFENCROUCH)
		yoff = -1;

	// andere Handlung
	else
	{
		if (Handlung == DUCKEN)
			yoff = 23;
		else 
			yoff = 0;
	}	

	//----- Richung des Schusses ermitteln

	// Hurri zielt nach oben
	//
	if (Handlung == SCHIESSEN_O ||
		(Handlung == SPRINGEN &&
		 Aktion[AKTION_OBEN]  &&
		!Aktion[AKTION_LINKS] &&
		!Aktion[AKTION_RECHTS]))
	{					
		if (Blickrichtung == LINKS)
			wadd = 90.0f;
		else 
			wadd = -90.0f;
	}
	else

	// Hurri zielt nach links oben
	//
	if (Handlung == SCHIESSEN_LO ||
	   (Handlung == SPRINGEN &&
		 Aktion[AKTION_OBEN]  &&
		 Aktion[AKTION_LINKS]))
		wadd = 45.0f;
	else

	// Hurri zielt nach rechts oben
	//
	if (Handlung == SCHIESSEN_RO ||
	   (Handlung == SPRINGEN &&
		 Aktion[AKTION_OBEN]  &&
		 Aktion[AKTION_RECHTS]))
		wadd = -45.0f;

	if (Handlung != RADELN &&						// Normal schiessen ?
		Handlung != RADELN_FALL)
	{

		int tempadd = 0;
		int tempshot = 0;

		if (FlameThrower)
		{
			WinkelUebergabe = 90.0f + wadd;
			pProjectiles->PushProjectile(xpos-tempadd+AustrittX - 29, ypos-tempadd+AustrittY - 33, PLAYERFIRE, this);
			ShotDelay = PLAYER_SHOTDELAY / 5.0f;

			if (pSoundManager->its_Sounds[SOUND_FLAMETHROWER + SoundOff]->isPlaying == false)
				pSoundManager->PlayWave(100, 128, 11025, SOUND_FLAMETHROWER + SoundOff);
		}
		else
		switch (SelectedWeapon)	
		{

//----- Spread Shot

		case 0 :
		{						
			// Normale Schüsse
			//
			if (RiesenShotExtra <= 0.0f)
			{
				tempadd = 8;
				tempshot = SPREADSHOT;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_SPREADSHOT);
			}
			else
			{
				tempadd = 16;
				tempshot = SPREADSHOTBIG;
				pSoundManager->PlayWave(100, 128, 8000, SOUND_SPREADSHOT);
			}

			WinkelUebergabe = 90.0f + wadd;

			if (CurrentWeaponLevel[SelectedWeapon] < 5)
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);

			if (CurrentWeaponLevel[SelectedWeapon] >= 5)
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);

			if (CurrentWeaponLevel[SelectedWeapon] > 1 &&
				CurrentWeaponLevel[SelectedWeapon] < 6)
			{
				WinkelUebergabe = 82.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);

				WinkelUebergabe = 98.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);
			}

			if (CurrentWeaponLevel[SelectedWeapon] >= 6)
			{
				WinkelUebergabe = 82.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);

				WinkelUebergabe = 98.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);
			}

			if (CurrentWeaponLevel[SelectedWeapon] > 2 &&
				CurrentWeaponLevel[SelectedWeapon] < 7)
			{
				WinkelUebergabe = 74.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);	

				WinkelUebergabe = 106.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);	
			}

			if (CurrentWeaponLevel[SelectedWeapon] >= 7)
			{
				WinkelUebergabe = 74.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);

				WinkelUebergabe = 106.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);
			}

			if (CurrentWeaponLevel[SelectedWeapon] > 3 &&
				CurrentWeaponLevel[SelectedWeapon] < 8)
			{
				WinkelUebergabe = 66.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);

				WinkelUebergabe = 114.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);
			}			

			if (CurrentWeaponLevel[SelectedWeapon] >= 8)
			{
				WinkelUebergabe = 66.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);

				WinkelUebergabe = 114.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);
			}			
		} break;

//----- LaserShot

		case 1 :
		{
			int tempaddx, tempaddy, tempshot;
			float mul1 = 7.0f/8.0f;

			// Normale Schüsse
			//
			if (RiesenShotExtra <= 0.0f)
			{
				tempaddx = 8;
				tempaddy = 24;
				tempshot = LASERSHOT;
				pSoundManager->PlayWave(100, 128, 11025, SOUND_LASERSHOT);
			}
			else
			{
				tempaddx = 12;
				tempaddy = 40;
				tempshot = LASERSHOTBIG;
				pSoundManager->PlayWave(100, 128, 8000, SOUND_LASERSHOT);
			}

			WinkelUebergabe = 90.0f + wadd;

			if (CurrentWeaponLevel[SelectedWeapon] == 4 ||
				CurrentWeaponLevel[SelectedWeapon] == 6)
				pProjectiles->PushProjectile(xpos-tempaddx+AustrittX - 4, ypos-tempaddy+AustrittY, tempshot + 1, this);

			if (CurrentWeaponLevel[SelectedWeapon] == 1 ||
				CurrentWeaponLevel[SelectedWeapon] == 3 ||
				CurrentWeaponLevel[SelectedWeapon] == 5)
				pProjectiles->PushProjectile(xpos-tempaddx+AustrittX, ypos-tempaddy+AustrittY, tempshot, this);

			if (CurrentWeaponLevel[SelectedWeapon] == 2)
			{
				if (wadd == 0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX, ypos-tempaddy+AustrittY-tempaddx*mul1, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX, ypos-tempaddy+AustrittY+tempaddx*mul1, tempshot, this);
				}

				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+tempaddx*mul1+AustrittX - 2, ypos-tempaddy+AustrittY-tempaddx*mul1, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx-tempaddx*mul1+AustrittX + 2, ypos-tempaddy+AustrittY+tempaddx*mul1, tempshot, this);
				}

				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx-tempaddx*mul1+AustrittX+6, ypos-tempaddy+AustrittY-tempaddx*mul1, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx+tempaddx*mul1+AustrittX+2, ypos-tempaddy+AustrittY+tempaddx*mul1, tempshot, this);
				}

				if (wadd ==  90.0f ||
					wadd == -90.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*mul1, ypos-tempaddy+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX+tempaddx*mul1, ypos-tempaddy+AustrittY, tempshot, this);
				}
			}

			if (CurrentWeaponLevel[SelectedWeapon] >= 7)
			{
				if (wadd == 0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX, ypos-tempaddy+AustrittY-tempaddx*mul1, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX, ypos-tempaddy+AustrittY+tempaddx*mul1, tempshot + 1, this);
				}

				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+tempaddx*mul1*0.75f+AustrittX, ypos-tempaddy+AustrittY-tempaddx*mul1*0.75f, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx-tempaddx*mul1*0.75f+AustrittX, ypos-tempaddy+AustrittY+tempaddx*mul1*0.75f, tempshot + 1, this);
				}

				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx-tempaddx*mul1*0.75f+AustrittX, ypos-tempaddy+AustrittY-tempaddx*mul1*0.75f, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+tempaddx*mul1*0.75f+AustrittX, ypos-tempaddy+AustrittY+tempaddx*mul1*0.75f, tempshot + 1, this);
				}

				if (wadd ==  90.0f ||
					wadd == -90.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*mul1, ypos-tempaddy+AustrittY, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX+tempaddx*mul1, ypos-tempaddy+AustrittY, tempshot + 1, this);
				}
			}

			if (CurrentWeaponLevel[SelectedWeapon] >= 3 &&
				CurrentWeaponLevel[SelectedWeapon] <= 4)
			{	
				if (wadd == 0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung, ypos-tempaddy-tempaddx*mul1*2.3f+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung, ypos-tempaddy+tempaddx*mul1*2.3f+AustrittY, tempshot, this);
				}

				if (wadd == 90.0f ||
					wadd ==-90.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx-tempaddx*mul1*2.3f+AustrittX, ypos-tempaddy/2+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx+tempaddx*mul1*2.3f+AustrittX, ypos-tempaddy/2+AustrittY, tempshot, this);
				}

				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos+tempaddx*mul1*2.3f+AustrittX, ypos-tempaddy+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos+tempaddx-tempaddx*mul1*2.3f+AustrittX, ypos-tempaddy+tempaddx*3+AustrittY, tempshot, this);
				}

				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx*mul1*4+AustrittX, ypos-tempaddy+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx*2+tempaddx*mul1*2.3f+AustrittX, ypos-tempaddy+tempaddx*2+AustrittY, tempshot, this);
				}
			}			

			if (CurrentWeaponLevel[SelectedWeapon] == 5 ||
				CurrentWeaponLevel[SelectedWeapon] == 6)
			{	
				if (wadd == 0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung*2, ypos-tempaddy+AustrittY-tempaddx*mul1*2, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung*2, ypos-tempaddy+AustrittY+tempaddx*mul1*2, tempshot + 1, this);
				}

				if (wadd == 90.0f ||
					wadd ==-90.0f)
				{
					pProjectiles->PushProjectile(xpos+tempaddx*mul1*2-tempaddx+AustrittX - 4, ypos-tempaddy+AustrittY+tempaddx, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx*mul1*2-tempaddx+AustrittX - 4, ypos-tempaddy+AustrittY+tempaddx, tempshot + 1, this);
				}

				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos+tempaddx+AustrittX-4, ypos-tempaddy+AustrittY, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-4, ypos-tempaddy+tempaddx*2+AustrittY, tempshot + 1, this);
				}

				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx*3+AustrittX-6, ypos-tempaddy+AustrittY, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-4, ypos-tempaddy+tempaddx*2+AustrittY, tempshot + 1, this);
				}
			}	


			if (CurrentWeaponLevel[SelectedWeapon] == 7)
			{
				if (wadd == 0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung*2, ypos-tempaddy+AustrittY-tempaddx*mul1*3, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung*2, ypos-tempaddy+AustrittY+tempaddx*mul1*3, tempshot, this);
				}

				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos+tempaddx*2+AustrittX+6, ypos-tempaddy+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx*2+AustrittX+4, ypos-tempaddy+tempaddx*5/2+AustrittY, tempshot, this);
				}

				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx*4+AustrittX+2, ypos-tempaddy+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos+AustrittX+4, ypos-tempaddy+tempaddx*5/2+AustrittY, tempshot, this);
				}

				if (wadd ==  90.0f ||
					wadd == -90.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx-tempaddy*mul1+AustrittX+3, ypos-tempaddy/2+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempaddx+tempaddy*mul1+AustrittX+4, ypos-tempaddy/2+AustrittY, tempshot, this);
				}
			}

			if (CurrentWeaponLevel[SelectedWeapon] == 8)
			{
				if (wadd == 0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung*2, ypos-tempaddy+AustrittY-tempaddx*3, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+AustrittX-tempaddx*Blickrichtung*2, ypos-tempaddy+AustrittY+tempaddx*3, tempshot + 1, this);
				}

				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos+tempaddx*3+AustrittX, ypos-tempaddy+AustrittY, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx*2+AustrittX, ypos-tempaddy+tempaddx*3+AustrittY, tempshot + 1, this);
				}

				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx*4.5f+AustrittX, ypos-tempaddy+AustrittY, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos+AustrittX, ypos-tempaddy+tempaddx*3+AustrittY, tempshot + 1, this);
				}

				if (wadd ==  90.0f ||
					wadd == -90.0f)
				{
					pProjectiles->PushProjectile(xpos-tempaddx-tempaddy*mul1+AustrittX, ypos-tempaddy/2+AustrittY, tempshot + 1, this);
					pProjectiles->PushProjectile(xpos-tempaddx+tempaddy*mul1+AustrittX, ypos-tempaddy/2+AustrittY, tempshot + 1, this);
				}
			}

		} break;

//----- BounceShot

		case 2 :
		{
			int tempadd, tempshot;

			// Normale Schüsse
			//
			if (RiesenShotExtra <= 0.0f)
			{
				tempadd = 12;
				tempshot = BOUNCESHOT1;

				pSoundManager->PlayWave(100, 128, 11025, SOUND_BOUNCESHOT);
			}
			else
			{
				tempadd = 24;
				tempshot = BOUNCESHOTBIG1;
				pSoundManager->PlayWave(100, 128, 8000, SOUND_BOUNCESHOT);
			}

			WinkelUebergabe = 90.0f + wadd;

			//if (CurrentWeaponLevel[SelectedWeapon] <= 5)
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot, this);

			/*
			if (CurrentWeaponLevel[SelectedWeapon] == 5)
			{
				if (wadd == 0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX + 4, ypos-tempadd+AustrittY-11, tempshot+1, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX + 4, ypos-tempadd+AustrittY+19, tempshot+1, this);
				}
				else
				if (wadd ==  90.0f ||
					wadd == -90.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX-11, ypos-tempadd+AustrittY+4, tempshot+1, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX+19, ypos-tempadd+AustrittY+4, tempshot+1, this);
				}
				else
				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX-7, ypos-tempadd+AustrittY+17, tempshot+1, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX+17, ypos-tempadd+AustrittY-7, tempshot+1, this);
				}
				else
				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX-7, ypos-tempadd+AustrittY-7, tempshot+1, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX+17, ypos-tempadd+AustrittY+15, tempshot+1, this);
				}
			}

			if (CurrentWeaponLevel[SelectedWeapon] > 5)
			{
				if (wadd ==  0.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY-10, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY+10, tempshot, this);
				}
				else
				if (wadd ==  90.0f ||
					wadd == -90.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX-10, ypos-tempadd+AustrittY, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX+10, ypos-tempadd+AustrittY, tempshot, this);
				}

				if (wadd == -45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX-7, ypos-tempadd+AustrittY-7, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX+7, ypos-tempadd+AustrittY+7, tempshot, this);
				}
				else
				if (wadd == 45.0f)
				{
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX+7, ypos-tempadd+AustrittY-7, tempshot, this);
					pProjectiles->PushProjectile(xpos-tempadd+AustrittX-7, ypos-tempadd+AustrittY+7, tempshot, this);
				}
			}

			if (CurrentWeaponLevel[SelectedWeapon] == 7)
			{
				WinkelUebergabe = 60.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+2, this);

				WinkelUebergabe = 120.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+2, this);
			}	

			if (CurrentWeaponLevel[SelectedWeapon] == 8)
			{
				WinkelUebergabe = 60.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);

				WinkelUebergabe = 120.0f + wadd;
				pProjectiles->PushProjectile(xpos-tempadd+AustrittX, ypos-tempadd+AustrittY, tempshot+1, this);
			}	
			*/

		} break;

		default : break;
		} // switch				

		if (!FlameThrower)
		{
			// Patronenhülse auswerfen bei SpreadShot
			if (SelectedWeapon == 0)
				pPartikelSystem->PushPartikel(xpos+30, ypos+24+yoff, BULLET, this);
			else

			// Rauchwolke bei Laser
			if (SelectedWeapon == 1)
				pPartikelSystem->PushPartikel(xpos+30, ypos+24+yoff, SMOKE3);
			else

			// Grüne Funken beim Bounce
			if (SelectedWeapon == 2)
				for (int i = 0; i < 2; i++)
					pPartikelSystem->PushPartikel(xpos+30 + rand ()%4, ypos+28+yoff + rand ()%4, FUNKE2);
		}

		// Schussflamme
		FlameTime = 1.0f;
		FlameAnim = rand()%2;
	}

	else					// oder Bombe legen
	{
		// Riesen Bombe?
		if (RiesenShotExtra > 0.0f)
			pProjectiles->PushProjectile(xpos+26, ypos+46, BOMBEBIG, this);

		// oder normal
		else
			pProjectiles->PushProjectile(xpos+30, ypos+56, BOMBE, this);
	}	
}

// --------------------------------------------------------------------------------------
// Spieler Granate
// --------------------------------------------------------------------------------------

void PlayerClass::PlayerGrenadeShoot (void)
{
	CalcAustrittsPunkt();

	//----- Richung des Schusses ermitteln

	float wadd = 0.0f;

	// Hurri zielt nach oben
	//
	if (Handlung == SCHIESSEN_O ||
		(Handlung == SPRINGEN &&
		 Aktion[AKTION_OBEN]  &&
		!Aktion[AKTION_LINKS] &&
		!Aktion[AKTION_RECHTS]))
	{					
		if (Blickrichtung == LINKS)
			wadd = 90.0f;
		else 
			wadd = -90.0f;
	}
	else

	// Hurri zielt nach links oben
	//
	if (Handlung == SCHIESSEN_LO ||
	   (Handlung == SPRINGEN &&
		 Aktion[AKTION_OBEN]  &&
		 Aktion[AKTION_LINKS]))
		wadd = 45.0f;
	else

	// Hurri zielt nach rechts oben
	//
	if (Handlung == SCHIESSEN_RO ||
	   (Handlung == SPRINGEN &&
		 Aktion[AKTION_OBEN]  &&
		 Aktion[AKTION_RECHTS]))
		wadd = -45.0f;

	WinkelUebergabe = 90.0f + wadd;
	pProjectiles->PushProjectile(xpos+AustrittX-4, ypos+AustrittY-4, GRENADE, this);

	pSoundManager->PlayWave(100, 128, 11025, SOUND_GRANATE);
	PowerLinePossible = false;			// Taste einrasten
	Grenades--;							// Granaten verringern

	// Schussflamme
	FlameTime = 1.0f;
	FlameAnim = rand()%2;
}

// --------------------------------------------------------------------------------------
// Den alten, billigen Blitz rendern
// --------------------------------------------------------------------------------------

void PlayerClass::DrawNormalLightning(int DrawLength)
{
	//----- Strahl des Blitzes anzeigen

	float l,  r,  o,  u;					// Vertice Koordinaten
	float tl, tr, to, tu;					// Textur Koordinaten	
	float x, y;
	
	x = (float)(xpos - pTileEngine->XOffset+60);		// Position errechnen
	y = (float)(ypos - pTileEngine->YOffset+36);

	if (Blickrichtung == LINKS)
		x -= 56;

	l = x-0.5f;						// Links
	o = y-DrawLength*32-0.5f;		// Oben
	r = x+31+0.5f;					// Rechts
	u = y+31+0.5f;					// Unten
		
	tl = 0.0f;
	tr = 1.0f;
	to = 0.0f;
	tu = 1.0f * DrawLength;
	if (tu == 0.0f)
		tu =  1.0f;

	TriangleStrip[0].z	 = TriangleStrip[1].z		= TriangleStrip[2].z	   = TriangleStrip[3].z	  = 0.0f;

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

	TriangleStrip[0].color = TriangleStrip[1].color = TriangleStrip[2].color = TriangleStrip[3].color = 0xFFFFFFFF;


	lpD3DDevice->SetTexture (0, Blitzstrahl[BlitzAnim].itsTexture);		// Textur setzen

	// Blitz rotieren lassen
	D3DXMATRIX	matRot, matTrans, matTrans2;

	D3DXMatrixRotationZ  (&matRot, DegreetoRad[int(BlitzWinkel)]);// Rotationsmatrix
	D3DXMatrixTranslation(&matTrans,  -x-16, -y-56, 0.0f);		// Transformation zum Ursprung

	if (Blickrichtung == RECHTS)
		D3DXMatrixTranslation(&matTrans2,  x-23,  y,    0.0f);		// Transformation wieder zurück
	else
		D3DXMatrixTranslation(&matTrans2,  x+31,  y,    0.0f);		// Transformation wieder zurück

	D3DXMatrixMultiply	 (&matWorld, &matTrans, &matRot);		// Verschieben und rotieren
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);	// und wieder zurück
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	DirectGraphics.SetFilterMode (true);

	// Blitzstrahl zeichnen
	v1.color = v2.color = v3.color = v4.color = 0xFFFFFFFF;
	DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2, &TriangleStrip[0]);

	// nochmal rendern bei supershot?
	if (RiesenShotExtra > 0.0f)
		DirectGraphics.RendertoBuffer (D3DPT_TRIANGLESTRIP, 2, &TriangleStrip[0]);

	DirectGraphics.SetFilterMode (false);

	// Normale Projektions-Matrix wieder herstellen, wenn der Screen nicht gerade gewackelt wird
	if (WackelMaximum <= 0.0f)
	{
		D3DXMatrixRotationZ (&matWorld, 0.0f);
		lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	}
	else
	{
		int Winkel = int(WackelValue+500)-500;			// +500 und -500 damit er von -1.0 bis +1.0 
														// nich stehen bleibt, weil -0.99 bis +0.99
														// auf 0 gerundet wird

		// Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
		if (Winkel > 360) Winkel -= 360;
		if (Winkel < 0)	  Winkel += 360;
		D3DXMatrixRotationZ  (&matRot, DegreetoRad[Winkel]);

		if (WackelMaximum <= 0.0f)					// Wackeln zuende ?
			WackelMaximum = 0.0f;					// Dann aufhören damit

		// rotierte Matrix setzen
		lpD3DDevice->SetTransform(D3DTS_WORLD, &matRot);
	}
		
	DirectGraphics.SetColorKeyMode();
}

// --------------------------------------------------------------------------------------
// Den neuen, mega geilen Blitz rendern
// bestehend aus mehreren, zusammengesetzten vertice-strichen
// Drawlength gibt an, wie lang der Blitz ist
// --------------------------------------------------------------------------------------

void PlayerClass::DrawCoolLightning(int DrawLength, float mul)
{
	int r = 0;
	int zufall = 0;
	int ystrahl = 0;
	int xstrahl = 0;
	float xstart = 0.0f;
	float ystart = 0.0f;
	int yoff = 0;
	int size = 0;
	int maxintersections = 0;					// anzahl zwischenschritte im blitz	
	D3DCOLOR col;	

	ystrahl = 0;
	xstrahl = 0;	

	xstart = (float)(18);
	ystart = (float)(4);

	maxintersections = DrawLength + 2;

	// Blitze neu zusammensetzen?
	if (pConsole->Showing == false)
		changecount -= 1.0f SYNC;

	if (changecount <= 0.0f)
	{
		changecount = (float)(rand()%10 + 1) / 10.0f;		

		for (int n = 0; n < 12; n ++)
		{
			// Farbe festlegen, hell oder dunkel oder fast weiss
			zufall = rand()%6;
			if (zufall < 3)
			{
				r = rand()%64;
				col = D3DCOLOR_RGBA(r/2, r, rand()%32 + 224, rand()%64 + 192);
			}
			else if (zufall < 5)
			{
				r = rand()%32;
				col = D3DCOLOR_RGBA(r/2, r, rand()%24 + 64, rand()%24 + 192);
			}
			else
			{
				r = rand()%128+128;
				col = D3DCOLOR_RGBA(r, r, 255, rand()%16 + 128);
			}

			// Startpunkt auf 0 setzen
			yoff = 0;
			size = rand()%4 + 1;

			for (int i = 0; i < maxintersections * 2; i += 2)
			{
				// zwei neue Punkte zwischen letztem Punkt und Endpunkt per Zufall setzen			
				xstrahl = (int)((xpos - pTileEngine->XOffset + xstart) + (rand()%32 - 16) * mul);
				ystrahl = (int)(ypos - pTileEngine->YOffset + ystart - yoff);

				// Am End- und Austrittspunkt gebündelt
				if (i == 0)				
				{
					xstrahl = (int)(xpos - pTileEngine->XOffset + xstart) + rand()%6 - 2;
					ystrahl = (int)(ypos - pTileEngine->YOffset + ystart);
				}	
				
				if (i >= (maxintersections - 1) * 2)
				{
					xstrahl = (int)(xpos - pTileEngine->XOffset + xstart) + rand()%6 - 2;
					ystrahl = (int)(ypos - pTileEngine->YOffset + ystart - (DrawLength + 1)*32);
				}
			
				// Position setzen
				strahlen[n][i + 0].x = (float)(xstrahl - size + xstart);
				strahlen[n][i + 1].x = (float)(xstrahl + size + xstart);
				strahlen[n][i + 0].y = (float)(ystrahl + ystart);
				strahlen[n][i + 1].y = (float)(ystrahl + ystart);
				strahlen[n][i + 0].z = 0.0f;
				strahlen[n][i + 1].z = 0.0f;

				// texturkoordinaten setzen
				strahlen[n][i + 0].tu = 0.0f;
				strahlen[n][i + 0].tv = 0.0f;
				strahlen[n][i + 1].tu = 1.0f;
				strahlen[n][i + 1].tv = 0.0f;

				// Farbe setzen
				strahlen[n][i + 0].color = col;
				strahlen[n][i + 1].color = col;		
				
				yoff += rand()%21+24;
			}	
		}
	}

	// Strahlen rendern
	lpD3DDevice->SetTexture (0, BlitzTexture.itsTexture);

	for (int n = 0; n < 12; n ++)
	{
		DirectGraphics.RendertoBuffer(D3DPT_TRIANGLESTRIP,
									 (maxintersections - 1) * 2 - 1,
									  &strahlen[n][0]);
	}

	return;
}

// --------------------------------------------------------------------------------------
// Blitz animieren und zeichnen
//
// Ist die Blitztaste schon lange genug gedrückt worden, wird der ganze Blitz angezeigt
// Andernfalls nur der Blitzursprung beim Spieler
// --------------------------------------------------------------------------------------

bool PlayerClass::DoLightning(void)
{
	int			DrawLength;			// Länge des Blitze mit berücksichtigten Wänden im Weg
	GegnerClass  *pEnemy;			// Für die Blitz/Gegner Kollision	

	DrawLength = BlitzLength-1;		// Vom Maximum ausgehen (das wird später "gekürzt")

	if (BlitzWinkel < 0)			// Einmal im Kreis rumgedreht ? Dann wieder
		BlitzWinkel += 360;			// von vorne beginnen mit der Rotation
	if (BlitzWinkel > 360)			// und zwar im und gegen den Uhrzeigersinn
		BlitzWinkel -= 360;

	// Ende des Blitzes beim Spieler leuchten lassen, falls er ihn grade noch auflädt
 
	float x, y;
	
	x = (float)(xpos - pTileEngine->XOffset+35);		// Position errechnen
	y = (float)(ypos - pTileEngine->YOffset+35);

	//if (Blickrichtung == LINKS)
	//	x -= 56;

	DirectGraphics.SetAdditiveMode();

//----- Blitz animieren

	if (pConsole->Showing == false)
		BlitzCount += SpeedFaktor;				// Counter erhöhen

	if (BlitzCount > PLAYER_BLITZ_SPEED)	// Animationsgrenze überschritten ?
	{
		BlitzCount = 0.0f;					// Dann Counter wieder auf Null setzen und
		BlitzAnim++;						// Nächste Animationsphase
		if (BlitzAnim >= 4)					// Animation von vorne beginnen ?
			BlitzAnim = 0;
	}

//----- Wird der Blitz schon angezeigt ?
//----- wenn nicht, dann das Aufladen anzeigen

	float xstart = float(xpos+20);
	float ystart = float(ypos+21);

	xstart += float(36*cos(PI * (BlitzWinkel-90) / 180));
	ystart += float(36*sin(PI * (BlitzWinkel-90) / 180));

	if (BlitzStart < PLAYER_BLITZ_START)
	{
		D3DCOLOR Color, Color2;

		if (BlitzStart < PLAYER_BLITZ_START)
		{
			Color  = D3DCOLOR_RGBA(255, 255, 255, int(BlitzStart * 25));
			Color2 = D3DCOLOR_RGBA(255, 255, 255, int(BlitzStart * 25) / 5);
		}
		else
		{
			Color  = D3DCOLOR_RGBA(255, 255, 255, 255);
			Color2 = D3DCOLOR_RGBA(255, 255, 255, 48);
		}
		
		Blitzflash[BlitzAnim].RenderSprite(float(xstart-18-pTileEngine->XOffset), 
										   float(ystart-18-pTileEngine->YOffset), Color);	

		// noch glow um die blitzenden?
		if (options_Detail >= DETAIL_HIGH)
			Blitzflash[3-BlitzAnim].RenderSpriteScaled(float(xstart-58-pTileEngine->XOffset),
										               float(ystart-58-pTileEngine->YOffset), 144, 144, 0, Color2);

		DirectGraphics.SetColorKeyMode();	
		return true;
	}

//----- Blitz anzeigen - zuerst wird festgestellt, wie lang der Blitz ist,
//		d.h., ob er eine Wand getroffen hat	

	// Anfang des Blitzes leuchten lassen
	Blitzflash[BlitzAnim].RenderSprite(float(xstart-18-pTileEngine->XOffset), 
									   float(ystart-18-pTileEngine->YOffset), 0xFFFFFFFF);	

	// noch glow um die blitzenden?
	if (options_Detail >= DETAIL_HIGH)
	{
		Blitzflash[3-BlitzAnim].RenderSpriteScaled(float(xstart-58-pTileEngine->XOffset), 
									               float(ystart-58-pTileEngine->YOffset), 144, 144, 0, 0x30FFFFFF);
	}

	// Startpunkt der Kollisionsabfrage auch schon mit ein wenig Abstand zum Spieler
	xstart -= float(20*cos(PI * (BlitzWinkel-90) / 180));
	ystart -= float(20*sin(PI * (BlitzWinkel-90) / 180));

	RECT	Rect;							// Rechteck für die Kollisionserkennung
											// ein Blitz-Stück wird grob durch
	Rect.left   = 0;						// ein 32x32 Rechteck abgeschätzt
	Rect.top    = 0;
	Rect.right  = 31;
	Rect.bottom = 31;

	float xs, ys;

	// Rechtecke für die Kollisionsabfrage rotieren lassen
	for (int i=0; i<BlitzLength+1; i++)
	{
		// Zum anzeigen der Rects, die geprüft werden
		if (DebugMode == true)
			RenderRect(float(xstart-pTileEngine->XOffset), 
					   float(ystart-pTileEngine->YOffset), 
					   31, 31, 0x80FFFFFF);		

		xs = xstart;
		ys = ystart;

		// Blitz-LightMap rendern
		if (options_Detail >= DETAIL_MEDIUM)
		pTileEngine->DrawLightmap(LIGHTMAP_BLITZ, xs + 16, ys + 16, 255);

		// Blitz auf Kollision mit den Gegnern prüfen
		pEnemy = pGegner->pStart;			// Anfang der Gegnerliste
		while (pEnemy != NULL)				// Noch nicht alle durch ?
		{
			if (pEnemy->Active == true &&		// Ist der Gegner überhaupt aktiv ?
				pEnemy->Destroyable == true)	// und zerstörbar ?
				
			{
				// Überschneiden sich die Rechtecke ?
				if (xstart + 31 > pEnemy->xPos &&
					xstart < pEnemy->xPos + GegnerRect[pEnemy->GegnerArt].right &&
					ystart + 31 > pEnemy->yPos &&
					ystart < pEnemy->yPos + GegnerRect[pEnemy->GegnerArt].bottom)
				{
					// Funken sprühen
					if (BlitzCount == 0.0f && BlitzAnim%2 == 0)
					pPartikelSystem->PushPartikel(xs+12, ys+12, LASERFUNKE);

					// Gegner blinken lassen
					pEnemy->DamageTaken = 255;

					// Energy abziehen
					pEnemy->Energy = pEnemy->Energy - BLITZ_POWER SYNC;

					// Hit Sound
					if (!(pSoundManager->its_Sounds[SOUND_HIT + pEnemy->HitSound]->isPlaying))
						  pSoundManager->PlayWave(100, 128, 21000 + rand()%1000, SOUND_HIT + pEnemy->HitSound);

					// PlattForm ShootButton getroffen ? Dann Blitz kürzen
					//
					if (pEnemy->GegnerArt == SHOOTBUTTON)
					{
						if (BlitzCount == 0.0f && BlitzAnim%2 == 0)
						pPartikelSystem->PushPartikel(xs+12, ys+12, LASERFUNKE);	// Funken sprühen
						DrawLength = i-1;											// Blitz "kürzen"
						i = BlitzLength + 2;
						break;														// Und Schleife verlassen
					}
				}	
			}

			pEnemy = pEnemy->pNext;			// Nächsten Gegner testen
		}

		// Zerstörbare Wände ?
		pTileEngine->CheckDestroyableWalls(xs,  ys, 0, 0, Rect);
		pTileEngine->BlockDestroyRechts(xs, ys, xs, ys, Rect);
		pTileEngine->BlockDestroyLinks (xs, ys, xs, ys, Rect);
		pTileEngine->BlockDestroyOben  (xs, ys, xs, ys, Rect);
		pTileEngine->BlockDestroyUnten (xs, ys, xs, ys, Rect);

		if (pTileEngine->BlockLinks (xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER &&	// Im Wasser blitzen ?
			pTileEngine->BlockRechts(xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER &&
			pTileEngine->BlockOben  (xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER &&
			pTileEngine->BlockUnten (xs, ys, xs, ys, Rect) & BLOCKWERT_WASSER &&
			rand()%80 == 0)
			pPartikelSystem->PushPartikel(xs+rand()%32, ys+rand()%32, BUBBLE);		// Dann blubbern


		if (pTileEngine->BlockLinks (xs, ys, xs, ys, Rect) & BLOCKWERT_WAND  ||	// Eine Wand getroffen ?
			pTileEngine->BlockRechts(xs, ys, xs, ys, Rect) & BLOCKWERT_WAND  ||
			pTileEngine->BlockOben  (xs, ys, xs, ys, Rect) & BLOCKWERT_WAND  ||
			pTileEngine->BlockUnten (xs, ys, xs, ys, Rect) & BLOCKWERT_WAND)
		{
			if (BlitzCount == 0.0f && BlitzAnim%2 == 0)
			pPartikelSystem->PushPartikel(xs+12, ys+12, LASERFUNKE);	// Funken sprühen
			xstart += float(32*cos(PI * (BlitzWinkel-90) / 180));
			ystart += float(32*sin(PI * (BlitzWinkel-90) / 180));
			DrawLength = i-1;										// Blitz "kürzen"
			break;													// und Schleife verlassen
		}

		xstart += float(32*cos(PI * (BlitzWinkel-90) / 180));
		ystart += float(32*sin(PI * (BlitzWinkel-90) / 180));
	}

	// Position für das Ende des Blitzes wieder ein wenig zurückverschieben
	xstart -= float(16*cos(PI * (BlitzWinkel-90) / 180));
	ystart -= float(16*sin(PI * (BlitzWinkel-90) / 180));

	// Ende des Blitzes leuchten lassen
	Blitzflash[BlitzAnim].RenderSprite(float(xstart-18-pTileEngine->XOffset), 
									   float(ystart-18-pTileEngine->YOffset), 0xFFFFFFFF);

	// noch glow um die blitzenden?
	if (options_Detail >= DETAIL_HIGH)
	{
		Blitzflash[3-BlitzAnim].RenderSpriteScaled(float(xstart-58-pTileEngine->XOffset), 
									               float(ystart-58-pTileEngine->YOffset), 144, 144, 0, 0x30FFFFFF);
	}

	// Blitz rotieren lassen
	D3DXMATRIX	matRot, matTrans, matTrans2;

	D3DXMatrixRotationZ  (&matRot, DegreetoRad[int(BlitzWinkel)]);// Rotationsmatrix
	D3DXMatrixTranslation(&matTrans, -x, -y, 0.0f);		// Transformation zum Ursprung
	if (Blickrichtung == RECHTS)
		D3DXMatrixTranslation(&matTrans2, x, y, 0.0f);		// Transformation wieder zurück
	else
		D3DXMatrixTranslation(&matTrans2, x, y, 0.0f);		// Transformation wieder zurück
	D3DXMatrixMultiply	 (&matWorld, &matTrans, &matRot);		// Verschieben und rotieren
	D3DXMatrixMultiply	 (&matWorld, &matWorld, &matTrans2);	// und wieder zurück
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	DirectGraphics.SetFilterMode (true);
	if (DrawLength < 0)
		DrawLength = 0;

	if (options_Detail >= DETAIL_HIGH)
		DrawCoolLightning(DrawLength);
	else
		DrawNormalLightning(DrawLength);

	// nochmal rendern bei supershot?
	if (RiesenShotExtra > 0.0f)
	{
		if (options_Detail >= DETAIL_HIGH)
			DrawCoolLightning(DrawLength, 1.2f);
		else
			DrawNormalLightning(DrawLength);
	}

	DirectGraphics.SetFilterMode (false);

	// Normale Projektions-Matrix wieder herstellen, wenn der Screen nicht gerade gewackelt wird
	if (WackelMaximum <= 0.0f)
	{
		D3DXMatrixRotationZ (&matWorld, 0.0f);
		lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	}
	else
	{
		int Winkel = int(WackelValue+500)-500;			// +500 und -500 damit er von -1.0 bis +1.0 
														// nich stehen bleibt, weil -0.99 bis +0.99
														// auf 0 gerundet wird

		// Winkel angleichen, damit er immer zwischen 0° und 360° bleibt
		if (Winkel > 360) Winkel -= 360;
		if (Winkel < 0)	  Winkel += 360;
		D3DXMatrixRotationZ  (&matRot, DegreetoRad[Winkel]);

		if (WackelMaximum <= 0.0f)					// Wackeln zuende ?
			WackelMaximum = 0.0f;					// Dann aufhören damit

		// rotierte Matrix setzen
		lpD3DDevice->SetTransform(D3DTS_WORLD, &matRot);
	}
		

	DirectGraphics.SetColorKeyMode();

	return true;
}

// --------------------------------------------------------------------------------------
// BeamBlitz aufladen
//
// Der Beam lädt sich langsam auf, je nach Blitzlänge unterschiedlich schnell
// --------------------------------------------------------------------------------------

bool PlayerClass::LoadBeam (void)
{
	if (BlitzWinkel < 0)			// Einmal im Kreis rumgedreht ? Dann wieder
		BlitzWinkel += 360;			// von vorne beginnen mit der Rotation
	if (BlitzWinkel > 360)			// und zwar im und gegen den Uhrzeigersinn
		BlitzWinkel -= 360;

	//----- Blitz animieren

	BlitzCount += SpeedFaktor;				// Counter erhöhen
	if (BlitzCount > PLAYER_BLITZ_SPEED)	// Animationsgrenze überschritten ?
	{
		BlitzCount = 0.0f;					// Dann Counter wieder auf Null setzen und
		BlitzAnim++;						// Nächste Animationsphase
		if (BlitzAnim >= 4)					// Animation von vorne beginnen ?
			BlitzAnim = 0;
	}

	// Ende des Blitzes beim Spieler leuchten lassen, falls er ihn grade noch auflädt
	//
	float x, y;
	
	x = (float)(xpos - pTileEngine->XOffset+60);		// Position errechnen
	y = (float)(ypos - pTileEngine->YOffset+36);

	if (Blickrichtung == LINKS)
		x -= 56;

	DirectGraphics.SetAdditiveMode();

//----- Wird der Blitz schon angezeigt ?
//----- wenn nicht, dann das Aufladen anzeigen

	D3DCOLOR Color;
	int		 a;

	a = int(BlitzStart * 255 / 160);

	if (BlitzStart < PLAYER_BEAM_MAX)
		Color = D3DCOLOR_RGBA(255, 255, 255, a);
	else
		Color = D3DCOLOR_RGBA(255, 255, 255, 255);

	float	xstart = float(xpos+20);
	float	ystart = float(ypos+21);

	xstart += float(28*cos(PI * (BlitzWinkel-90) / 180));
	ystart += float(28*sin(PI * (BlitzWinkel-90) / 180));

	// Ende des Blitzes leuchten lassen
	Blitzflash[BlitzAnim].RenderSprite(float(xstart-18-pTileEngine->XOffset), 
									   float(ystart-18-pTileEngine->YOffset), Color);

	BeamX = xstart+12;
	BeamY = ystart+12;

	// Auflade Partikel erzeugen?
	//
	if (BlitzStart < PLAYER_BEAM_MAX)
	{
		BeamCount -= float (1.0 SYNC);

		if (BeamCount < 0.0f)
		{
			BeamCount = 0.1f;

			int j = rand ()%360;
			pPartikelSystem->PushPartikel (float (BeamX + sin ((float)j) * 50),
										   float (BeamY + cos ((float)j) * 50), BEAMSMOKE2, this);
		}
	}

	DirectGraphics.SetColorKeyMode();

	return true;
}

// --------------------------------------------------------------------------------------
// Spieler erleidet Schaden
// ist Override == true dann auch mit Shield oder als Rad
// --------------------------------------------------------------------------------------

void PlayerClass::DamagePlayer (float Ammount,
							    bool Override)
{
	if (GodMode		 == true  ||
		CanBeDamaged == false ||
		(Shield > 0.0f && Override == false))
		return;

	// Skill Level berücksichtigen
	if (Skill == 0) Ammount *= 0.3f;
	if (Skill == 1) Ammount *= 0.6f;
	if (Skill == 2) Ammount *= 1.0f;
	if (Skill == 3) Ammount *= 2.2f;

	// Sound starten
	if (DamageCounter == 0.0f   &&
		Handlung != RADELN		&&
		Handlung != RADELN_FALL &&
		pSoundManager->its_Sounds[SOUND_ABZUG + SoundOff]->isPlaying == false)
		pSoundManager->PlayWave(100, 128, 11025, SOUND_ABZUG + SoundOff);

	// Nur verwunden, wenn kein Rad, denn als Rad ist man unverwundbar
	if ((Handlung != RADELN &&
		 Handlung != RADELN_FALL) ||
		 Override == true)
	{
		Energy -= Ammount;
		if (Energy < 0.0f)
			Energy = 0.0f;

		DamageCounter = 5.0f;

		DrawPlayer(true, false);
		WasDamaged = true;		
	}
}

// --------------------------------------------------------------------------------------
// Level zentrieren, damit der Spieler wieder im Mittelpunkt steht
// --------------------------------------------------------------------------------------

void PlayerClass::CenterLevel(void)
{
	pTileEngine->XOffset = xpos - 300;
	pTileEngine->YOffset = ypos - 280;
}

// --------------------------------------------------------------------------------------
// Waffenlevels für die nächste Stufe berechnen
// --------------------------------------------------------------------------------------

void PlayerClass::CalcWeaponLevels(void)
{
/*
	for (int i = 0; i < 3; i++)
		pPlayer->NextWeaponLevel[i] = 3 + (pPlayer->CurrentWeaponLevel[i] - 1) * 2;
	
	pPlayer->NextWeaponLevel[3] = 3;
*/
}

// --------------------------------------------------------------------------------------
// Position der Schussflamme errechnen
// --------------------------------------------------------------------------------------

void PlayerClass::CalcFlamePos (void)
{
	float xoff, yoff;

	// auf dem FlugSack reiten
	if (Handlung == SACKREITEN)
	{
		yoff = 1;
		if (Blickrichtung == RECHTS)
			xoff =  80;
		else
			xoff = -35;
	}

	// Surfen
	else if (Handlung == SURFEN ||
			 Handlung == SURFENJUMP)
	{
		xoff = 52;
		yoff = -14;
	}

	else if (Handlung == SURFENCROUCH)
	{
		xoff = 51;
		yoff = -2;
	}

	// alle anderen Handlugen
	else
	{
		if (Handlung == DUCKEN)
			yoff = 23;
		else 
			yoff = 0;

		if (Blickrichtung == RECHTS)
			xoff =  66;
		else
			xoff = -33;
	}

	// X-Offset richtig berechnen
	xoff -= (int)(pTileEngine->XOffset);
	yoff -= (int)(pTileEngine->YOffset);

	// Und dann je nach Blickrichtung die Flamme und den Flare dazu setzen
	if (Blickrichtung == RECHTS)
	{
		if (Handlung == STEHEN   ||
			Handlung == SACKREITEN ||
			Handlung == DUCKEN)				
		{
			AustrittAnim = 0;
			AustrittX = xoff - 10;
			AustrittY = yoff + 20;				
		}
		else

		// im Sprung?
		if (Handlung == SPRINGEN)
		{
			// dabei hochzielen?
			if (Aktion[AKTION_OBEN])
			{
				// nur noch?
				if (!(Aktion[AKTION_LINKS] ||
					  Aktion[AKTION_RECHTS]))
				{
					AustrittAnim = 2;
					AustrittX = xoff - 47;
					AustrittY = yoff - 34;
				}

				// oder schräg?
				else
				{
					AustrittAnim = 1;
					AustrittX = xoff - 21;
					AustrittY = yoff - 12;
				}								    
			}

			// normal springen
			else
			{				
				AustrittAnim = 0;
				AustrittX = xoff - 10;
				AustrittY = yoff + 15;
			}
		}

		else
		if (Handlung == LAUFEN)
		{
			AustrittAnim = 0;
			AustrittX = xoff - 2;
			AustrittY = yoff + 19;
		}

		else
		if (Handlung == SCHIESSEN_RO)
		{
			//im Stehen?
			if (WalkLock == true)
			{
				AustrittAnim = 1;
				AustrittX = xoff - 25;
				AustrittY = yoff - 8;
			}

			// oder Laufen?
			else
			{
				AustrittAnim = 1;
				AustrittX = xoff - 21;
				AustrittY = yoff - 12;
			}
		}

		else
		if (Handlung == SCHIESSEN_O)
		{
			AustrittAnim = 2;
			AustrittX = xoff - 49;
			AustrittY = yoff - 33;
		}
	}
	else
	{
		if (Handlung == STEHEN   ||
			Handlung == SACKREITEN   ||
			Handlung == DUCKEN)				
		{
			AustrittAnim = 0;
			AustrittX = xoff + 10;
			AustrittY = yoff + 20;				
		}
		else

		// im Sprung?
		if (Handlung == SPRINGEN)
		{
			// dabei hochzielen?
			if (Aktion[AKTION_OBEN])
			{
				// nur noch?
				if (!(Aktion[AKTION_LINKS] ||
					  Aktion[AKTION_RECHTS]))
				{
					AustrittAnim = 2;
					AustrittX = xoff + 62;
					AustrittY = yoff - 34;
				}

				// oder schräg?
				else
				{
					AustrittAnim = 1;
					AustrittX = xoff + 21;
					AustrittY = yoff - 12;
				}								    
			}

			// normal springen
			else
			{				
				AustrittAnim = 0;
				AustrittX = xoff + 6;
				AustrittY = yoff + 15;
			}
		}

		else
		if (Handlung == LAUFEN)
		{
			AustrittAnim = 0;
			AustrittX = xoff;
			AustrittY = yoff + 19;
		}

		else
		if (Handlung == SCHIESSEN_LO)
		{
			//im Stehen?
			if (WalkLock == true)
			{
				AustrittAnim = 1;
				AustrittX = xoff + 21;
				AustrittY = yoff - 8;
			}

			// oder im Laufen
			else
			{
				AustrittAnim = 1;
				AustrittX = xoff + 8;
				AustrittY = yoff - 10;
			}
		}
		else
		if (Handlung == SCHIESSEN_O)
		{
			AustrittAnim = 2;
			AustrittX = xoff + 62;
			AustrittY = yoff - 33;
		}
	}
}

// --------------------------------------------------------------------------------------
// Austrittspunkt des Schusses errechnen
// --------------------------------------------------------------------------------------

void PlayerClass::CalcAustrittsPunkt(void)
{		
	float xver = 0.0f;	// Verschiebung des Austrittspunktes, wenn man vor einer Wandsteht

	//----- Genauen Pixel am Anfang derFlamme finden
	//
	CalcFlamePos();
	AustrittX += (float)pTileEngine->XOffset;
	AustrittY += (float)pTileEngine->YOffset;
	switch (AustrittAnim)
	{
		// gerade flamme
		case 0:
		{
			if (Blickrichtung == LINKS)
				AustrittX += 34;
			else
				AustrittX += 4;

			AustrittY += 24 / 2 - 1;
		}
		break;

		// schräg oben
		case 1:
		{
			if (Blickrichtung == LINKS)				
				AustrittX += 32;
			else
				AustrittX += 8;

			AustrittY += 26;
		}
		break;

		// oben
		case 2:
		{
			AustrittX += 24 / 2;
			AustrittY += 38;
		}
		break;

		case 3:
		{
			AustrittX += 40 / 2;
			AustrittY += 36 / 2;
		}
		break;
	}
}

// --------------------------------------------------------------------------------------
// Spieler aus dem Level rausgelaufen?
// --------------------------------------------------------------------------------------

bool PlayerClass::CheckLevelExit(void)
{
	// Spieler aus Level draussen?
	if (xpos + pPlayer[0]->CollideRect.right  < pTileEngine->XOffset	    ||
		xpos + pPlayer[0]->CollideRect.left   > pTileEngine->XOffset + 640 ||
		ypos + pPlayer[0]->CollideRect.bottom < pTileEngine->YOffset	    ||
		ypos + pPlayer[0]->CollideRect.top    > pTileEngine->YOffset + 480)
		return true;

	return false;
}

// --------------------------------------------------------------------------------------
// Spieler im Ausgang
// --------------------------------------------------------------------------------------

void PlayerClass::PlayerInExit(void)
{
	StageClearRunning = false;

	// Summary Screen zeigen, wenn nicht im Tutorial Mode
	if (RunningTutorial == false)
		SummaryScreen();

	Stage = NewStage;		// Anpassen dass wir im neuen Level sind

	SpielZustand = MAINMENU;
	pConsole->Hide();

	// Tutorial Level zuende? Dann normal zurück ins Hauptmenu
	//

	if (Stage == TUTORIAL_END)
	{
		NewStage = -1;
		Stage    = -1;
		pMenu->AktuellerZustand = MENUZUSTAND_MAINMENU;
		pGUI->HideBoxFast();
		pSoundManager->StopSong(MUSIC_STAGECLEAR, false);
	}

	// Alle Levels durch? Dann Outtro starten
	//

	else
	if (Stage > MAX_LEVELS)
		StartOuttro();

	// Ansonsten Savegame Menu
	//

	else
	if (RunningTutorial == false)
	{
		pMenu->LoadSavegames();					// Slots mit aktuellen Savegames füllen
		pMenu->AktuellerPunkt = 0;
		pMenu->AktuellerZustand = MENUZUSTAND_SAVEGAME;
	}
	else
	{
		for (int p = 0; p < NUMPLAYERS; p++)
			pPlayer[p]->GodMode = false;

		Grenades = 3;
		PowerLines = 3;
		SmartBombs = 1;

		RunningTutorial = false;	
		InitNewGameLevel(NewStage);// Neues level laden
	}

	RunningTutorial = false;	
}

// --------------------------------------------------------------------------------------
// Spieler läuft grad ins Exit
// --------------------------------------------------------------------------------------

void PlayerClass::RunPlayerExit(void)
{
	// Musik zuende ?
	if (FMUSIC_IsFinished(pSoundManager->its_Songs[MUSIC_STAGECLEAR]->SongData))
		pSoundManager->StopSong(MUSIC_STAGECLEAR, false);

	if (CheckLevelExit() == true)
		PlayerInExit();
}

// --------------------------------------------------------------------------------------
// Alle Spieler auf einem Flugsack?
// --------------------------------------------------------------------------------------

bool PlayerClass::Riding(void)
{
	for (int p = 0; p < NUMPLAYERS; p++)
		if (pPlayer[p]->Handlung != SACKREITEN &&
			pPlayer[p]->Handlung != DREHEN &&
			pPlayer[p]->Handlung != TOT)
			return false;

	return true;
}

// --------------------------------------------------------------------------------------
// Level bei Flugsack Passage scrollen
// --------------------------------------------------------------------------------------

void PlayerClass::ScrollFlugsack(void)
{
	bool BeideFrei = true;

	for (int p = 0; p < NUMPLAYERS; p++)
		if (pPlayer[p]->FesteAktion > -1)
			BeideFrei = false;		
	if (!FlugsackFliesFree &&
		Riding() && 
		BeideFrei == true)
		pTileEngine->YOffset -= (float)(PLAYER_FLUGSACKSPEED SYNC);	
}

// --------------------------------------------------------------------------------------
// Waffen durchcyclen
// --------------------------------------------------------------------------------------

void PlayerClass::checkWeaponSwitch(void)
{
	if (!Aktion[AKTION_WAFFEN_CYCLE])
		weaponswitchlock = false;

	if (Aktion[AKTION_WAFFEN_CYCLE] &&
		!weaponswitchlock)
	{
		weaponswitchlock = true;

		do
		{
			SelectedWeapon++;
		} while (CurrentWeaponLevel[SelectedWeapon] == 0);

		if (SelectedWeapon >= 3)
			SelectedWeapon = 0;
	}
}
