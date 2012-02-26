// Datei : Console.cpp

// -------------------------------------------------------------------------------------- 
//
// Konsolen-Klasse für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "Console.h"
#include "DX8Graphics.h"
#include "GetKeyName.h"
#include "Gameplay.h"
#include "Globals.h"
#include "Logdatei.h"
#include "Projectiles.h"
#include "Player.h"
#include "Tileengine.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Konsolen-Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

ConsoleClass::ConsoleClass()
{
	ConsoleGFX.LoadImage("console.bmp", 512, 256, 512, 256, 1, 1);	// Grafik laden

	its_Alpha		= 0.0f;
	Fade			= 0.0f;
	Activate		= true;
	Active			= false;
	Showing			= false;
	RenderBinary    = false;

	// Text in der Konsole leeren
	for (int i = 0; i < MAX_LINES; i++)
		strcpy_s(Text[i], 1, "");

	// Text in Konsole schreiben
	for (int i = 0; i < 9; i++)
		CONSOLE_PRINT(TextArray[TEXT_CONSOLE1+i]);
	
	for (int i = 0; i < 10; i++)
		CONSOLE_PRINT(" ");

	// Lock-Puffer leeren
	for (int i = 0; i < 256; i++)
		Pressed[i] = false;

} // Konstruktor

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

ConsoleClass::~ConsoleClass()
{
	ConsoleGFX.~DirectGraphicsSprite();	// Grafik freigeben
} // Destruktor

// --------------------------------------------------------------------------------------
// Konsole anzeigen
// --------------------------------------------------------------------------------------

void ConsoleClass::ShowConsole(void)
{
	D3DCOLOR	Color;
	int			a;
	float		yoffset;
	static float cursorcount = 0.0f;

	//a = int (its_Alpha * 8 / 9);
	a = int (its_Alpha);

	yoffset = - 255.0f + its_Alpha - 1.0f;

	Color = D3DCOLOR_RGBA(255, 255, 255, a);
	ConsoleGFX.RenderSpriteScaled(0, yoffset, 640, 256, Color);

	// Text anzeigen
	for (int i=0; i<MAX_LINES; i++)
	{
		a = int (its_Alpha * 8 / 9);
		Color = D3DCOLOR_RGBA(128, 128, 255, a);

		if (i<MAX_LINES)
			pDefaultFont->DrawText(26, yoffset + float(10 + i * 10), Text[i], Color);
		else
			pDefaultFont->DrawText(20, yoffset + float(10 + i * 10), Text[i], Color);
	}

	char Temp[105];

	strcpy_s (Temp, 2, ">");
	strcat_s (Temp, strlen(Buffer) + 1, Buffer);

	// Cursor animieren
	cursorcount += 0.3f SYNC;

	switch (int (cursorcount))
	{
		case 0 : 
			strcat_s (Temp, 2, " ");
		break;

		case 1 : 
			strcat_s (Temp, 2, "_");
		break;

		default :
		break;
	}

	if (cursorcount >= 2.0f)
		cursorcount = 0.0f;
	
	Color = D3DCOLOR_RGBA(255, 255, 255, a);
	pDefaultFont->DrawText(20, yoffset +  10 + (MAX_LINES) * 10, Temp, Color);
} // ShowConsole

// --------------------------------------------------------------------------------------
// Cheat eingegeben?
// --------------------------------------------------------------------------------------

bool ConsoleClass::CONSOLE_CHEAT(char *cheat)
{
	bool result = false;	

	// TODO FIX
//	result = CONSOLE_COMMAND(convertText(cheat));

	if (result)
	{
		pSoundManager->PlayWave(100, 128, 15000, SOUND_MESSAGE);
		pPlayer[0]->Score = 0;
		HasCheated = true;
	}

	return result;
}

// --------------------------------------------------------------------------------------
// Text checken, wenn Enter gedrückt wurde
// --------------------------------------------------------------------------------------

void ConsoleClass::CheckCommands(void)
{
	// Hilfe
	//
#ifdef _DEBUG
	// TODO FIX
/*
		if (CONSOLE_COMMAND("help"))
		{
			CONSOLE_PRINT(" ");
			CONSOLE_PRINT("Console Commands");
			CONSOLE_PRINT("-------------------------");
			CONSOLE_PRINT(" ");
			CONSOLE_PRINT("clear - clear console");
			CONSOLE_PRINT("levelinfo - show level information");
			CONSOLE_PRINT("maxfps x - set maximum framerate to 'x' (0 = no maximum framerate used)");
			CONSOLE_PRINT("setspeed x - Set Game-speed (default = 10)");
			CONSOLE_PRINT("loadmap 'name' - load level 'name'");
			CONSOLE_PRINT("minimap - save minimap to disc");
			CONSOLE_PRINT("quit - Quit game immediately");
		} else*/
	
#endif

	// Konsole löschen
	if (CONSOLE_COMMAND("clear"))
	{
		for (int i=0; i<MAX_LINES; i++)
			strcpy_s(Text[i], "");

		strcpy_s(Buffer, "");
	} else

	// Levelinfo anzeigen
	if (CONSOLE_COMMAND("levelinfo"))
	{
		char StringBuffer[100];
		char temp[50];

		CONSOLE_PRINT(" ");
		CONSOLE_PRINT("Levelinfo");
		CONSOLE_PRINT("-------------");
		CONSOLE_PRINT(" ");

		CONSOLE_PRINT(pTileEngine->Beschreibung);

		_itoa_s((int)(pTileEngine->XOffset), temp, 10);
		strcpy_s(StringBuffer, "Level Offset X : ");
		strcat_s(StringBuffer, temp);
		CONSOLE_PRINT(StringBuffer);

		_itoa_s((int)(pTileEngine->YOffset), temp, 10);
		strcpy_s(StringBuffer, "Level Offset Y : ");
		strcat_s(StringBuffer, temp);
		CONSOLE_PRINT(StringBuffer);

		_itoa_s((int)(pTileEngine->LEVELSIZE_X), temp, 10);
		strcpy_s(StringBuffer, "Level Size X : ");
		strcat_s(StringBuffer, temp);
		CONSOLE_PRINT(StringBuffer);

		_itoa_s((int)(pTileEngine->LEVELSIZE_Y), temp, 10);
		strcpy_s(StringBuffer, "Level Size Y : ");
		strcat_s(StringBuffer, temp);
		CONSOLE_PRINT(StringBuffer);
	} else

	// Spiel sofort verlassen
	if (CONSOLE_COMMAND("quit"))
	{
		CONSOLE_PRINT("Shutting down ...");
		GameRunning = false;
	} else
		

	// Volle Extrawaffen
	if (CONSOLE_CHEAT(Cheats[CHEAT_EXTRAS]))
	{
		for (int p = 0; p < NUMPLAYERS; p++)
		{
			pPlayer[p]->PowerLines = 999;
			pPlayer[p]->Grenades   = 999;
			pPlayer[p]->SmartBombs = 999;
		}

		CONSOLE_PRINT("-> Let's rock!");
	} else

	// Volle Zeit
	if (CONSOLE_CHEAT(Cheats[CHEAT_ZEIT]))
	{
		pTileEngine->Timelimit = 999;

		CONSOLE_PRINT("-> knowing that there's no rhyme...");
	} else

	// Volle Waffen
	if (CONSOLE_CHEAT(Cheats[CHEAT_WAFFEN]))
	{
		for (int p = 0; p < NUMPLAYERS; p++)
		{
			pPlayer[p]->CurrentWeaponLevel[3] = 16;

			for (int w = 0; w <= 2; w++)
				pPlayer[p]->CurrentWeaponLevel[w] = 8;
		}

		CONSOLE_PRINT("-> Blast Off and Strike The Evil Bydo Empire!");
	} else

	// Schild
		if (CONSOLE_CHEAT(Cheats[CHEAT_SCHILD]))
	{
		for (int i = 0; i < NUMPLAYERS; i++)
		{
			/*
			if (pPlayer[i]->Shield <= 0.0f)
			{
				pProjectiles->PushProjectile (pPlayer[i]->xpos, pPlayer[i]->ypos, SHIELDSPAWNER, pPlayer[i]);
				pProjectiles->PushProjectile (pPlayer[i]->xpos, pPlayer[i]->ypos, SHIELDSPAWNER2, pPlayer[i]);
			}
		
			// Schild setzen
			pPlayer[i]->Shield = 500.0f;*/

			pPlayer[i]->Lives = 99;

		}		

		strcpy_s(Buffer, "-> Live long and prosper!");
	} else

	// Dauerfeuer
	if (CONSOLE_CHEAT(Cheats[CHEAT_SUPERSHOT]))
	{
		for (int i = 0; i < NUMPLAYERS; i++)
			pPlayer[i]->RiesenShotExtra += 500;

		strcpy_s(Buffer, "-> Supershot");
	} else

	// Dauerfeuer
	if (CONSOLE_CHEAT(Cheats[CHEAT_AUTOFIRE]))
	{
		for (int i = 0; i < NUMPLAYERS; i++)
			pPlayer[i]->AutoFireExtra += 500;

		strcpy_s(Buffer, "-> Autofire");
	} else


	// Flammenwerfer Mode 
	if (CONSOLE_CHEAT(Cheats[CHEAT_FLAMER]))
	{
		FlameThrower = !FlameThrower;

		if (FlameThrower)
		{
			CONSOLE_PRINT("-> Flamethrower on");
		}
		else
		{
			CONSOLE_PRINT("-> Flamethrower off");
		}
	} //else

	// GodMode 
	if (CONSOLE_CHEAT(Cheats[CHEAT_GOD]))
	{
		if (pPlayer[0]->GodMode == false)
		{
			pPlayer[0]->GodMode = true;
			pPlayer[1]->GodMode = true;
			CONSOLE_PRINT("-> Godmode on");
		}
		else
		{
			pPlayer[0]->GodMode = false;
			pPlayer[1]->GodMode = false;
			CONSOLE_PRINT("-> Godmode off");
		}
	} //else

	// GodMode 
	if (CONSOLE_CHEAT(Cheats[CHEAT_RAD]))
	{
		if (pPlayer[0]->WheelMode == false)
		{
			pPlayer[0]->WheelMode = true;
			pPlayer[1]->WheelMode = true;
			CONSOLE_PRINT("-> WheelMode on");
		}
		else
		{
			pPlayer[0]->WheelMode = false;
			pPlayer[1]->WheelMode = false;
			CONSOLE_PRINT("-> WheelMode off");
		}
	} //else
	
	// max FPS setzen
	if (strncmp(Buffer, "maxfps ", 7) == 0)
	{

		// Bis zu der Zahl vorgehen
		int index1 = 7;
		int g_test = 0;

		while(Buffer[index1] !='\0')
		{
	        if(Buffer[index1]>=48 && Buffer[index1]<=58)
			{
				g_test*=10;
				g_test+=Buffer[index1]-48;
			}
			index1++;
		}

		// Meldung ausgeben
		char dummy[50];

		_itoa_s(g_test, dummy, 10);
		strcpy_s(Buffer, "Setting maximum Framerate to ");
		strcat_s(Buffer, dummy);
		strcat_s(Buffer, " ...");
		CONSOLE_PRINT(Buffer);

		// und FPS Setzen
		pTimer->SetMaxFPS(g_test);
	}

//#ifdef _DEBUG
	// Speed setzen
	if (strncmp(Buffer, "setspeed ", 9) == 0)
	{
		// Bis zu der Zahl vorgehen
		int index1 = 9;
		int g_test = 0;

		while(Buffer[index1] !='\0')
		{
	        if(Buffer[index1]>=48 && Buffer[index1]<=58)
			{
				g_test*=10;
				g_test+=Buffer[index1]-48;
			}
			index1++;
		}

		// Meldung ausgeben
		char dummy[50];

		_itoa_s(g_test, dummy, 10);
		strcpy_s(Buffer, "Setting Speed to ");
		strcat_s(Buffer, dummy);
		strcat_s(Buffer, " ...");
		CONSOLE_PRINT(Buffer);

		// und Speed Setzen
		pTimer->SetMoveSpeed(float(atoi(dummy)));
	}
//#endif


	// Stage laden
	if (strncmp(Buffer, "loadmap ", 8) == 0)
	{
		char dummy[255], dummy2[255];

		strcpy_s(dummy, _strrev (Buffer));
		strncpy_s (dummy2, dummy, strlen (dummy) - 8);
		dummy2[strlen (dummy) - 8] = 0;
		strcpy_s(dummy, _strrev (dummy2));

		// Meldung ausgeben
		strcpy_s(Buffer, "Loading Level ");
		strcat_s(Buffer, dummy);
		strcat_s(Buffer, " ...");
		CONSOLE_PRINT(Buffer);

		// und Level laden
		FILE *Datei = NULL;
		char name[100];

		strcpy_s(name, "data/");
		strcat_s(name, dummy);

		// .map anhängen
		if (dummy2[strlen (dummy2) - 1] != 'p' ||
		    dummy2[strlen (dummy2) - 2] != 'a' ||
		    dummy2[strlen (dummy2) - 3] != 'm') 
			strcat_s(name, ".map");

		fopen_s(&Datei, name, "rb");

		// Datei gefunden? Dann laden
		if (Datei)
		{
			for (int i = 0; i < NUMPLAYERS; i++)
				pPlayer[i]->InitNewLevel();

			fclose(Datei);

			CONSOLE_PRINT("found.");
			pTileEngine->LoadLevel(dummy);
			Protokoll.WriteText(dummy, false);
		}

		// Ansonsten Fehlermeldung
		else
		{
			sprintf_s(dummy2,  "Map %s not found !", name);
			CONSOLE_PRINT(dummy2);
		}
	}

	// Minimap anzeigen und Screenshot machen
	if (strncmp(Buffer, "minimap", 7) == 0)
	{
		// Darstellung beenden
		lpD3DDevice->EndScene();						
		DirectGraphics.ShowBackBuffer();		

		// Mit dem Darstellen beginnen
		lpD3DDevice->BeginScene();			

		// Screen schwarz färben
		RenderRect(0, 0, 640, 480, 0xFF000000);

		// Mini Map anzeigen
		for (int i=0; i < pTileEngine->LEVELSIZE_X; i++)
		 for (int j=0; j < pTileEngine->LEVELSIZE_Y; j++)
		 {
			 if (pTileEngine->Tiles[i][j].Block & BLOCKWERT_WAND)		  RenderRect((float)i, (float)j, 1, 1, 0xFFFFFFFF);
			 if (pTileEngine->Tiles[i][j].Block & BLOCKWERT_DESTRUCTIBLE) RenderRect((float)i, (float)j, 1, 1, 0xFFFFFF00);
			 if (pTileEngine->Tiles[i][j].Block & BLOCKWERT_PLATTFORM)	  RenderRect((float)i, (float)j, 1, 1, 0xFF888888);
			 if (pTileEngine->Tiles[i][j].Block & BLOCKWERT_WASSER)		  RenderRect((float)i, (float)j, 1, 1, 0xFF0000FF);
		 }

		char buf[100], buf2[100];

		strcpy_s(buf, "Level ");
		_itoa_s(Stage, buf2, 10);
		strcat_s(buf, buf2);

		pDefaultFont->DrawText(3, 458, buf, 0xFF00FF00);
		pDefaultFont->DrawText(3, 470, pTileEngine->Beschreibung, 0xFF00FF00);
	
		lpD3DDevice->EndScene();						// Darstellung beenden
		DirectGraphics.ShowBackBuffer();		

		// Screenshot machen
		DirectGraphics.TakeScreenshot("MiniMap", 640, 480);
		CONSOLE_PRINT("Mini Map saved !");
	}

	// Demo aufnehmen
	if (CONSOLE_COMMAND("record demo"))
	{
		NewDemo ("Demo.dem");
		CONSOLE_PRINT("Recording demo");
		this->Activate = false;
		this->Fade   = -25.0f;
		this->Active = false;
		this->Showing= false;
	} //else

	// Demo anhalten
	if (CONSOLE_COMMAND("stop demo"))
	{
		EndDemo ();
		CONSOLE_PRINT("Demo stopped");
	} //else

	// Demo laden und abspielen
	if (CONSOLE_COMMAND("load demo"))
	{
		if (LoadDemo ("Demo.dem") == false)
		{
			CONSOLE_PRINT("Error loading demo");
		}
		else
		{
			CONSOLE_PRINT("Playing demo");
			this->Activate = false;
			this->Fade   = -25.0f;
			this->Active = false;
			this->Showing= false;
		}
	} //else

	// 0 und 1 Render Mode anschalten
	if (CONSOLE_COMMAND("binary"))
	{
		if (this->RenderBinary == false)
		{
			CONSOLE_PRINT("Rendering Binary");
			this->RenderBinary = true;
		}
		else
		{
			CONSOLE_PRINT("Rendering Normal");
			this->RenderBinary = false;
		}
	} //else


#ifdef _DEBUG
	// In Level warpen
	if (strncmp(Buffer, "goto ", 5) == 0)
	{
		// Bis zu der Zahl vorgehen
		int index1 = 5;
		int g_test = 0;

		while(Buffer[index1] !='\0')
		{
	        if(Buffer[index1]>=48 && Buffer[index1]<=58)
			{
				g_test*=10;
				g_test+=Buffer[index1]-48;
			}
			index1++;
		}

		// Level > 0? Dann dorthin springen
		//
		if (g_test >= MAX_LEVELS)
		{
			CONSOLE_PRINT("Level does not exist");
		}
		else
		if (g_test > 0)
		{
			this->Activate = false;
			this->Fade   = -25.0f;
			this->Active = false;		
			this->Showing= false;

			InitNewGame ();			

			for (int p = 0; p < NUMPLAYERS; p++)
			{
				for (int i=0; i<MAX_AKTIONEN; i++)
					pPlayer[p]->Aktion[i] = false;

				Stage    = g_test;
				NewStage = g_test;
			}

			InitNewGameLevel (Stage);
		}
	} //else	
#endif

	if (CONSOLE_COMMAND("light cool"))
	{
		pTileEngine->ComputeCoolLight();
	}

	if (CONSOLE_COMMAND("light shit"))
	{
		pTileEngine->ComputeShitLight();
	}

	if (CONSOLE_COMMAND("lamp"))
	{
		if (pTileEngine->bDrawShadow == false)
		{
			pTileEngine->bDrawShadow = true;
			ShadowAlpha = 255.0f;
		}
		else
			pTileEngine->bDrawShadow = false;
	}
	
	//strcpy_s(Buffer, "Error : Unknown Command !");

} // CheckCommands

// --------------------------------------------------------------------------------------
// Eine Zeile hochscrollen
// --------------------------------------------------------------------------------------

void ConsoleClass::ScrollUp(void)
{
	// Zeilen eins hochscrollen
	for (int i = 0; i < MAX_LINES; i++)
		strcpy_s(Text[i], Text[i+1]);

	// Und unterste Zeile mit der letzten eingabe füllen
	strcpy_s(Text[MAX_LINES], Buffer);
	strcpy_s(Buffer, "\0");
} // ScrollUp

// --------------------------------------------------------------------------------------
// Checken ob was eingegeben wurde
// --------------------------------------------------------------------------------------

void ConsoleClass::CheckInput(void)
{
	for (int i = 0; i < 256; i++)
	{
		if (KeyDown(i) && 
			Pressed[i] == false)
		{
			pSoundManager->PlayWave(100, 128, 15000, SOUND_CLICK);

			Pressed[i] = true;

			// Zeichen anhängen
			if (strlen(GetKeyName(i)) <= 1 &&
					   strlen(Buffer) < MAX_CHARS)
			{
				char buf[50];				
				strcpy_s(buf, GetKeyName(i));

				// unwandeln in kleinbuchstaben
				if (!KeyDown(DIK_LSHIFT)) 
				{
					char c;
					for (int i=0; i < 50; i++)
					{
						c = buf[i];
						if (c >= 65 &&
							c <= 90)
							c += 32;
						buf[i] = c;
					}
				}
				
				strcat_s(Buffer, buf);
			}

			// Space anhängen
			if (i == DIK_SPACE)
				strcat_s(Buffer, " ");

			// Backspace
			if (i == DIK_BACKSPACE && strlen(Buffer) > 0) 
			{
				char Temp[MAX_CHARS];

				strcpy_s(Temp, Buffer);
				strcpy_s(Buffer, "");
				strncat_s(Buffer, Temp, strlen(Temp) - 1);
			}
	
			// Neue Zeile
			if (i == DIK_RETURN)
			{
				if (strlen(Buffer) > 0)
				{					
					CheckCommands();					
					ScrollUp();
				}
			}
		}

		// Gedrückte Tasten locken
		if (!KeyDown(i)) 
			Pressed[i] = false;					
	}
} // CheckInput

// --------------------------------------------------------------------------------------
// Konsole zeigen
// --------------------------------------------------------------------------------------

void ConsoleClass::Open(void)
{
	Fade   = 25.0f;
	Active = true;
	Showing = true;
	strcpy_s(Buffer, "");	
}

// --------------------------------------------------------------------------------------
// Konsole verstecken
// --------------------------------------------------------------------------------------

void ConsoleClass::Hide(void)
{
	Fade   = -25.0f;
	Active = false;	
}

// --------------------------------------------------------------------------------------
// Konsole ablaufen lassen
// --------------------------------------------------------------------------------------

bool ConsoleClass::DoConsole(void)
{
	// Konsole ist aktivierbar
	if (!KeyDown(DIK_TAB))
		Activate = true;

	// Konsole wird aktiviert oder deaktiviert ?
	if (KeyDown(DIK_TAB) && Activate == true)
	{
		Activate = false;

		if (Active == true)
			Hide();
		else

		if (Active == false)
			Open();
	}

	// Konsole fadet gerade ?
	if (Fade != 0)	
		its_Alpha += 2.0f * Fade SYNC;

	// Überlauf verhindern
	if (its_Alpha > 255.0f)
	{
		its_Alpha = 255.0f;
		Fade	  =   0.0f;
	}

	// Konsole ist ausgefadet ?
	if (its_Alpha < 0.0f)
	{
		its_Alpha = 0.0f;
		Fade	  = 0.0f;
	}

	// Konsole ausgefadet? Dann aussteigen
	if (Active == false && 
		its_Alpha <= 0.0f) 
	{
		Showing = false;
		return false;
	}

	// Ansonsten Eingabe checken
	CheckInput();

	// Einheitsmatrix setzen, da Konsole nicht rotiert wird
	//
	D3DXMATRIX matView;
	D3DXMatrixIdentity	 (&matView);
	lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	// Und anzeigen
	ShowConsole();

	SetScreenShake ();

	return true;
} // DoCOnsole
