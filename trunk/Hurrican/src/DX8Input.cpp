// Datei : DX8Input.cpp

// -------------------------------------------------------------------------------------- 
//
// DirectInput Klasse
// zum Erfassen von Eingaben durch Tastaur, Maus und Joystick/Joypad
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "DX8Input.h"
#include "Logdatei.h"
#include "Main.h"
#include "Globals.h"
#include "Gameplay.h"

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

char					TastaturPuffer[256];	// Tastaturpuffer für Keyboardabfrage

// --------------------------------------------------------------------------------------
// Joystick mit ForceFeedback Enumeration Callback-Funktion
// --------------------------------------------------------------------------------------

bool CALLBACK EnumForceFeedbackDevices(LPDIDEVICEINSTANCE lpddi, LPVOID pv)
{
	GUID *pguidDevice = NULL;

	if (pv)
	{
		pguidDevice = (GUID*)pv;
		*pguidDevice = lpddi->guidInstance;
	}
	
	DirectInput.Joysticks[DirectInput.JoysticksFound].CanForceFeedback = true;
	DirectInput.Joysticks[DirectInput.JoysticksFound].guidJoystickDevice = DirectInput.guidJoystickDevice;
	strcpy_s(DirectInput.Joysticks[DirectInput.JoysticksFound].JoystickName, strlen((char*)lpddi->tszProductName) + 1, (char*)lpddi->tszProductName);	

	// Counter erhöhen
	DirectInput.JoysticksFound++;

	// Genug Joysticks? Dann halte die EnumDevices() Funktion an
	if (DirectInput.JoysticksFound < MAX_JOYSTICKS)
		return DIENUM_CONTINUE;
	else
		return DIENUM_STOP;
}

// --------------------------------------------------------------------------------------
// Joystick Enumeration Callback-Funktion
// --------------------------------------------------------------------------------------

bool CALLBACK EnumJoystickDevices(LPDIDEVICEINSTANCE lpddi, LPVOID pv)
{
	GUID *pguidDevice = NULL;

	if (pv)
	{
		pguidDevice = (GUID*)pv;
		*pguidDevice = lpddi->guidInstance;
	}
	
	DirectInput.Joysticks[DirectInput.JoysticksFound].CanForceFeedback = false;
	strcpy_s(DirectInput.Joysticks[DirectInput.JoysticksFound].JoystickName, strlen((char*)lpddi->tszProductName) + 1, (char*)lpddi->tszProductName);	

	// Counter erhöhen
	DirectInput.JoysticksFound++;

	// Genug Joysticks? Dann halte die EnumDevices() Funktion an
	if (DirectInput.JoysticksFound < MAX_JOYSTICKS)
		return DIENUM_CONTINUE;
	else
		return DIENUM_STOP;
}

// --------------------------------------------------------------------------------------
// Klassenfunktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectInput Klasse
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectInputClass::DirectInputClass(void)
{
	lpDI		 = NULL;
	lpDIKeyboard = NULL;
	lpDIMaus	 = NULL;

	// Zu Beginn alle Eingabegeräte zurücksetzen
	MausX		 = 0;
	MausY		 = 0;
	for(int i=0; i<MAX_MOUSEBUTTONS; i++)
		MausButtons[i] = false;

	JoysticksFound = 0;
	UseForceFeedback = false;
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

DirectInputClass::~DirectInputClass(void)
{
}

// --------------------------------------------------------------------------------------
// DirectInput initialisieren
// Keyboard und Maus initialisieren und Joystick, falls vorhanden
// --------------------------------------------------------------------------------------

bool DirectInputClass::Init(HWND hwnd, HINSTANCE hinst)
{
	HRESULT hresult;

	Protokoll.WriteText("\n--> DirectInput8 init <--\n", false);
	Protokoll.WriteText(  "-------------------------\n\n", false);

	hresult = DirectInput8Create(hinst, DIRECTINPUT_VERSION, 
                                 IID_IDirectInput8, (LPVOID*)&lpDI, NULL); 

	// DirectInput Haupt-Device erstellen
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> DirectInput8Create error!\n", true);
		return false;
	}
	Protokoll.WriteText("DirectInput8Create	successfull!\n", false);

//----- Keyboard

	// Keyboard Device erstellen
	hresult = lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKeyboard, NULL);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Keyboard : CreateDevice error!\n", true);
		return false;
	}
	Protokoll.WriteText("Keyboard : CreateDevice successfull!\n", false);

	// Datenformat für Keyboard festlegen
	hresult = lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Keyboard : SetDataFormat error!\n", true);
		return false;
	}
	Protokoll.WriteText("Keyboard : SetDataFormat successfull!\n", false);

	// Keyboard Cooperativelevel setzen
	if (CommandLineParams.RunWindowMode == true)
		hresult = lpDIKeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	else
		hresult = lpDIKeyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Keyboard : SetCooperativeLevel error!\n", true);
		return false;
	}
	Protokoll.WriteText("Keyboard : SetCooperativeLevel	successfull!\n", false);

	// Keyboard akquirieren
	if (lpDIKeyboard) 
	{
		hresult = lpDIKeyboard->Acquire();
		if(hresult != DI_OK)
		{
			Protokoll.WriteText("\n-> Keyboard : Acquire error!\n", true);
			return false;
		}
		Protokoll.WriteText("Keyboard : Acquire	successfull!\n", false);
	}
  
	// angeschlossenen Joystick mit ForceFeedback enumerieren
	hresult = lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, 
							   (LPDIENUMDEVICESCALLBACK)EnumForceFeedbackDevices, 
                               &guidJoystickDevice, 0/*DIEDFL_ATTACHEDONLY*/);
	if(hresult != DI_OK) 
	{ 
		hresult = lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, 
							   (LPDIENUMDEVICESCALLBACK)EnumJoystickDevices, 
                               &guidJoystickDevice, 0/*DIEDFL_ATTACHEDONLY*/);
	}
	else
		Protokoll.WriteText("Joystick : EnumDevices successfull!\n", false);	

	// Gefundene Joysticks initialisieren
	if (JoysticksFound == 0)
		Protokoll.WriteText("No joystick found, skipping init!\n", false);
	else
	for (int i = 0; i < JoysticksFound; i++)
	{
		char Buf[255];

		if(Joysticks[i].Init(hwnd, lpDI) == false)
		
		{
			strcpy_s(Buf, strlen("Error initialising Joystick ") + 1, "Error initialising Joystick ");
			strcat_s(Buf, strlen(Joysticks[i].JoystickName) + 1, Joysticks[i].JoystickName);
			strcat_s(Buf, 5, " !\n");
			Protokoll.WriteText(Buf, false);
		}
		else
		{
			JoystickFound = true;

			strcpy_s(Buf, strlen("Joystick found : ") + 1, "Joystick found : ");
			strcat_s(Buf, strlen(Joysticks[i].JoystickName) + 1, Joysticks[i].JoystickName);
			strcat_s(Buf, 3, "\n");
			Protokoll.WriteText (Buf, false);

			if (Joysticks[i].CanForceFeedback)
			{
				Protokoll.WriteText ("Initializing ForceFeedback Effects\n", false);

				// Vibrations-Effekte erstellen
				// Kurzes, schwaches Vibrieren (ShorVib)
				DICONSTANTFORCE		 diConstantForce;
				DIRAMPFORCE			 diRampForce;
				DIEFFECT			 diEffect;
				DWORD				 dwAxes[2] = {DIJOFS_X, DIJOFS_Y};
				LONG				 lDirection[2] = {180*DI_DEGREES,0};

				diConstantForce.lMagnitude = DI_FFNOMINALMAX;			

				diEffect.dwSize = sizeof(DIEFFECT);
				diEffect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_CARTESIAN;
				diEffect.dwDuration = DI_SECONDS/4;
				diEffect.dwSamplePeriod = 0;
				diEffect.dwGain = DI_FFNOMINALMAX;
				diEffect.dwTriggerButton = DIEB_NOTRIGGER;
				diEffect.dwTriggerRepeatInterval = 0;
				diEffect.cAxes = 2;
				diEffect.rgdwAxes = dwAxes;
				diEffect.rglDirection = lDirection;
				diEffect.lpEnvelope = NULL;
				diEffect.cbTypeSpecificParams = sizeof (DICONSTANTFORCE);
				diEffect.lpvTypeSpecificParams = &diConstantForce;
				diEffect.dwStartDelay = 0;

				hresult = Joysticks[i].lpDIJoystick->CreateEffect (GUID_ConstantForce , &diEffect, &Joysticks[i].pFFE_SmallVib, NULL);
				if (hresult != DI_OK)
				{
					Protokoll.WriteText ("Error initializing Effect! Not using ForceFeedback\n", false);
					Joysticks[i].CanForceFeedback = false;
				}

				// Kurzes, starkes Vibrieren (BigVib)
				diEffect.dwFlags = DIEFF_OBJECTOFFSETS |DIEFF_POLAR;
				diEffect.dwGain = DI_FFNOMINALMAX/2;

				hresult = Joysticks[i].lpDIJoystick->CreateEffect (GUID_ConstantForce , &diEffect, &Joysticks[i].pFFE_BigVib, NULL);
				if (hresult != DI_OK)
				{
					Protokoll.WriteText ("Error initializing Effect! Not using ForceFeedback\n", false);
					Joysticks[i].CanForceFeedback = false;
				}

				// Kurzes, abklingendes Vibrieren (MaxVib)
				diEffect.dwGain = DI_FFNOMINALMAX;
				diEffect.dwDuration = DI_SECONDS;
				diRampForce.lStart = -10000;
				diRampForce.lEnd   = 0;
				diEffect.cbTypeSpecificParams = sizeof (DIRAMPFORCE);
				diEffect.lpvTypeSpecificParams = &diRampForce;

				hresult = Joysticks[i].lpDIJoystick->CreateEffect (GUID_RampForce, &diEffect, &Joysticks[i].pFFE_MaxVib, NULL);
				if (hresult != DI_OK)
				{
					Protokoll.WriteText ("Error initializing Effect! Not using ForceFeedback\n", false);
					Joysticks[i].CanForceFeedback = false;
				}

				// Blitz Effekt			
				diEffect.dwTriggerRepeatInterval = INFINITE;
				diEffect.dwGain = DI_FFNOMINALMAX;
				diEffect.dwDuration = DI_SECONDS | INFINITE;
				diConstantForce.lMagnitude = DI_FFNOMINALMAX;
				diEffect.dwGain = DI_FFNOMINALMAX/2;
				diEffect.cbTypeSpecificParams = sizeof (DICONSTANTFORCE);
				diEffect.lpvTypeSpecificParams = &diConstantForce;

				hresult = Joysticks[i].lpDIJoystick->CreateEffect (GUID_ConstantForce, &diEffect, &Joysticks[i].pFFE_Blitz, NULL);
				if (hresult != DI_OK)
				{
					Protokoll.WriteText ("Error initializing Effect! Not using ForceFeedback\n", false);
					Joysticks[i].CanForceFeedback = false;
				}
			}
		}
	}
	
	Protokoll.WriteText("\n-> DirectInput8 init successfull!\n\n", false);
	return true;
}

// --------------------------------------------------------------------------------------
// DirectInput beenden
// --------------------------------------------------------------------------------------

void DirectInputClass::Exit(void)
{
	if(lpDIKeyboard != NULL)			// Keyboard freigeben
	{
		lpDIKeyboard->Unacquire();
		lpDIKeyboard->Release();
		lpDIKeyboard = NULL;
	}

	/*if(lpDIMaus != NULL)				// Maus freigeben
	{
		lpDIMaus->Unacquire();
		lpDIMaus->Release();
		lpDIMaus = NULL;
	}*/

	for (int i = 0; i < JoysticksFound; i++)
	{
		if(Joysticks[i].lpDIJoystick != NULL)			// Joystick freigeben
		{
			Joysticks[i].lpDIJoystick->Unacquire();
			Joysticks[i].lpDIJoystick->Release();
			Joysticks[i].lpDIJoystick = NULL;
		}
	}
	
	SafeRelease(lpDI);					// DirectInput Hauptobjekt freigeben
	Protokoll.WriteText("-> DirectInput8 erfolgreich beendet !\n", false);
}

// --------------------------------------------------------------------------------------
// Tasten abfragen und im Puffer speichern
// --------------------------------------------------------------------------------------

bool DirectInputClass::UpdateTastatur(void)
{
	HRESULT hresult;

	hresult = lpDIKeyboard->GetDeviceState(sizeof(TastaturPuffer), (LPVOID)&TastaturPuffer); 

	if(hresult == DIERR_INPUTLOST)				// Keyboard verloren gegangen ?
	{
		if(lpDIKeyboard)						// Dann nochmal versuchen
		{
			lpDIKeyboard->Acquire();
			hresult = lpDIKeyboard->GetDeviceState(sizeof(TastaturPuffer), 
												   (LPVOID)&TastaturPuffer); 
		}
	}

	return true;
}

// --------------------------------------------------------------------------------------
// Maus abfragen
// true  = gepuffert
// false = ungepuffert
// --------------------------------------------------------------------------------------

bool DirectInputClass::UpdateMaus(bool gepuffert)
{
	return true;

   HRESULT      hresult;
   DIMOUSESTATE ms;
   bool         fertig = false;

	// Gepufferte Abfrage : Langsam, bearbeitet aber ALLE Maus-Aktionen
	if (gepuffert == true) 
	{
		while (!fertig) 
		{
			DIDEVICEOBJECTDATA od;  // Speichert Mauszustand
			DWORD dwElemente = 1;   // Elemente die abgefragt werden sollen

			hresult = lpDIMaus->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElemente, 0);
			if (hresult == DIERR_INPUTLOST) 
			{
				hresult = lpDIMaus->Acquire();
				if (hresult != DI_OK) 
				{
					Protokoll.WriteText("\n-> Maus : Re-Acquire Fehler !\n", false);
					return false;
				}
				else 
				{
					hresult = lpDIMaus->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), 
													  &od, &dwElemente, 0);
					if (hresult == DIERR_INPUTLOST) 
					{
						Protokoll.WriteText("\n-> Maus : GetData Fehler !\n", false);
						return false;
					}
				} 
            }
        else 
		if (hresult != DI_OK)		// anderer Fehler ?
		{			
			fertig = true;
            return false;
		}
         
		if (dwElemente == 0)			// Keine Elemente wurden verändert
			fertig = true;
         
		switch (od.dwOfs)				// Feld 'dwOfs' enthält Maus-Aktion:
		{		
			case DIMOFS_X:				// Horizontale Bewegung
				MausX += od.dwData;
				fertig = true;
				break;
            
            case DIMOFS_Y:				// Vertikale Bewegung
				MausY += od.dwData;
				fertig = true;
				break;
         
            case DIMOFS_BUTTON0:		// Knopf 0 gedrückt
				if (od.dwData & 0x80)	// Knopf gedrück
				{
					MausButtons[0] = true;
					fertig = TRUE;
				}
                else					// Knopf losgelassen
				{
					MausButtons[0] = false;
					fertig = TRUE;
                }
				break;

				case DIMOFS_BUTTON1:	// Knopf 1 gedrückt
				if (od.dwData & 0x80)	// Knopf gedrück
				{
					MausButtons[1] = true;
					fertig = TRUE;
				}
                else					// Knopf losgelassen
				{
					MausButtons[1] = false;
					fertig = TRUE;
                }
				break;

				case DIMOFS_BUTTON2:	// Knopf 2 gedrückt
				if (od.dwData & 0x80)	// Knopf gedrück
				{
					MausButtons[2] = true;
					fertig = TRUE;
				}
                else					// Knopf losgelassen
				{
					MausButtons[2] = false;
					fertig = TRUE;
                }
				break;            
			};
         }
      }

	// Ungepuffert: Schnell, verpasst aber ab und zu Maus-Aktionen:
	else 
	{
		hresult = lpDIMaus->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&ms);
		if (hresult == DIERR_INPUTLOST) 
		{
			hresult = lpDIMaus->Acquire();
			if (hresult != DI_OK) 
			{
				Protokoll.WriteText("\n-> Maus : Re-Acquire Fehler !\n", false);
				return false;
            }
			else 
			{
				hresult = lpDIMaus->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&ms);
				if (hresult == DIERR_INPUTLOST) 
				{
					Protokoll.WriteText("\n-> Maus : GetDeviceState Fehler !\n", false);
					return false;
				}
			}
		}
		else									// anderer Fehler
		if (hresult != DI_OK) 
		{
			return false;
        }

		// Mauskoordinaten angleichen
		MausX += ms.lX;
		MausY += ms.lY;
      
		// Buttons prüfen
		if (ms.rgbButtons[0] & 0x80)
			MausButtons[0] = true;
		else
			MausButtons[0] = false;
		if (ms.rgbButtons[1] & 0x80)
			MausButtons[1] = true;
		else
			MausButtons[1] = false;
		if (ms.rgbButtons[2] & 0x80)
			MausButtons[2] = true;
		else
			MausButtons[2] = false;
      }

	// Die Maus nicht verschwinden lassen
	if (MausX < 0)				MausX = 0;
	if (MausY < 0)				MausY = 0;
	if (MausX > SCREENWIDTH)	MausX = SCREENWIDTH;
	if (MausY > SCREENHEIGHT)	MausY = SCREENHEIGHT;

	return true;
}

// --------------------------------------------------------------------------------------
// Keyboard wieder akquirieren (wenn Fenster gewechselt zB)
// --------------------------------------------------------------------------------------

void DirectInputClass::AcquireKeyboard(void)
{
	if(lpDIKeyboard)
		lpDIKeyboard->Acquire();
}

// --------------------------------------------------------------------------------------
// Alle Joysticks updaten
// --------------------------------------------------------------------------------------

void DirectInputClass::UpdateJoysticks(void)
{
	for (int i = 0; i < JoysticksFound; i++)
		Joysticks[i].Update();
}

// --------------------------------------------------------------------------------------
// Checken ob keine Taste mehr gedrückt ist
// --------------------------------------------------------------------------------------

bool DirectInputClass::AreAllKeysReleased()
{
	for (int i = 0; i < 255; i++)
		if (TastaturPuffer[i] != NULL)
			return false;

	return true;
}

// --------------------------------------------------------------------------------------
// Checken ob irgendeine Taste gedrückt ist
// --------------------------------------------------------------------------------------

bool DirectInputClass::AnyKeyDown(void)
{
	for (int i = 0; i < 256; i++)
		if (KeyDown(i))
			return true;

	return false;
}

// --------------------------------------------------------------------------------------
// Checken ob irgendein Button gedrückt ist
// --------------------------------------------------------------------------------------

bool DirectInputClass::AnyButtonDown(void)
{
	if (JoystickFound == false)
		return false;

	for (int i = 0; i < JoysticksFound; i++)
		for (int j = 0; j < MAX_JOYSTICKBUTTONS; j++)
			if (Joysticks[i].JoystickButtons[j])
				return true;

	return false;
}

