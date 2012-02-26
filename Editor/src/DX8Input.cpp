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
#include "Globals.h"

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern Logdatei			Protokoll;					// Protokoll Datei

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

char					TastaturPuffer[256];	// Tastaturpuffer für Keyboardabfrage
bool					Joystick;				// Joystick gefunden ?
char					JoystickName[50];		// Joystick Produktname

// --------------------------------------------------------------------------------------
// Joystick Enumeration Callback-Funktion
// --------------------------------------------------------------------------------------

bool CALLBACK EnumJoystickDevices(LPDIDEVICEINSTANCE lpddi, LPVOID pv)
{
	GUID *pguidDevice = NULL;
	// Speichere Instance-GUID des gefundenen Device (Joystick)
	// in 'pguidDevice' = 'pv' = 'guidDevice'
	if(pv) 
	{
		// lokaler Pointer zeigt auf gleiche Adresse wie globaler
		pguidDevice = (GUID *)pv;
		// Wert des Pointers wird gefundene GUID
		*pguidDevice = lpddi->guidInstance;
	}

	strcpy(JoystickName, (char*)lpddi->tszProductName);
	Joystick = true;   // Wir haben einen!!!

	// Halte die EnumDevices() Funktion an
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
	lpDIJoystick = NULL;

	// Zu Beginn alle Eingabegeräte zurücksetzen
	MausX		 = 0;
	MausY		 = 0;
	for(int i=0; i<MAX_MOUSEBUTTONS; i++)
		MausButtons[i] = false;

	Joystick	 = false;
	JoystickX	 = 0;
	JoystickY	 = 0;
	for(i=0; i<MAX_JOYSTICKBUTTONS; i++)
		JoystickButtons[i] = false;
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

	Protokoll.WriteText("\n--> DirectInput8 Initialisierung gestartet <--\n", false);
	Protokoll.WriteText(  "----------------------------------------------\n\n", false);

	hresult = DirectInput8Create(hinst, DIRECTINPUT_VERSION, 
                                 IID_IDirectInput8, (LPVOID*)&lpDI, NULL); 

	// DirectInput Haupt-Device erstellen
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> DirectInput8Create Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("DirectInput8Create				: erfolgreich !\n", false);

//----- Keyboard

	// Keyboard Device erstellen
	hresult = lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKeyboard, NULL);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Keyboard : CreateDevice Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Keyboard : CreateDevice				: erfolgreich !\n", false);

	// Datenformat für Keyboard festlegen
	hresult = lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Keyboard : SetDataFormat Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Keyboard : SetDataFormat			: erfolgreich !\n", false);

	// Keyboard Cooperativelevel setzen
	hresult = lpDIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Keyboard : SetCooperativeLevel Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Keyboard : SetCooperativeLevel			: erfolgreich !\n", false);

	// Keyboard akquirieren
	if (lpDIKeyboard) 
	{
		hresult = lpDIKeyboard->Acquire();
		if(hresult != DI_OK)
		{
			Protokoll.WriteText("\n-> Keyboard : Acquire Fehler !\n", true);
			return false;
		}
		Protokoll.WriteText("Keyboard : Acquire				: erfolgreich !\n", false);
	}
   
//----- Maus

	// Mouse Device erstellen
	hresult = lpDI->CreateDevice(GUID_SysMouse, &lpDIMaus, NULL);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Maus : CreateDevice Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Maus : CreateDevice				: erfolgreich !\n", false);

	// Datenformat für die Maus festlegen
	hresult = lpDIMaus->SetDataFormat(&c_dfDIMouse);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Maus : SetDataFormat Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Maus : SetDataFormat				: erfolgreich !\n", false);

	// Maus Cooperativelevel setzen
	hresult = lpDIMaus->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Maus : SetCooperativeLevel Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Maus : SetCooperativeLevel			: erfolgreich !\n", false);

	// Puffer für die Maus erzeugen
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize		= sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj		= 0;
	dipdw.diph.dwHow		= DIPH_DEVICE;
	dipdw.dwData			= DINPUT_BUFFERSIZE;

	// neue Properties setzen
	hresult = lpDIMaus->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if(hresult != DI_OK)
	{
		Protokoll.WriteText("\n-> Maus : SetProperty Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Maus : SetProperty				: erfolgreich !\n", false);

	// Maus akquirieren
	if (lpDIMaus) 
	{
		hresult = lpDIMaus->Acquire();
		if(hresult != DI_OK)
		{
			Protokoll.WriteText("\n-> Maus : Acquire Fehler !\n", true);
			return false;
		}
		Protokoll.WriteText("Maus : Acquire					: erfolgreich !\n", false);
	}

//----- Joystick

	// Ersten angeschlossenen Joysticks enumerieren
	hresult = lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, 
							   (LPDIENUMDEVICESCALLBACK)EnumJoystickDevices, 
                               &guidJoystickDevice, DIEDFL_ATTACHEDONLY);
	if(hresult != DI_OK) 
	{ 
		Protokoll.WriteText("\n-> Joystick : EnumDevices Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Joystick : EnumDevices				: erfolgreich !\n", false);

	if(Joystick)
	{
		if(InitJoystick(hwnd) == false)
		{
			Protokoll.WriteText("Joystick : Fehler bei der Joystick Initialisierung !\n", false);
			Joystick = false;
		}
	}
	else
		Protokoll.WriteText("Joystick : kein Joystick gefunden, überspringe Initialisierung !\n", false);

	Protokoll.WriteText("\n-> DirectInput8 Initialisierung erfolgreich beendet !\n\n", false);
	return true;
}

// --------------------------------------------------------------------------------------
// Joystick extra initialisieren
// --------------------------------------------------------------------------------------

bool DirectInputClass::InitJoystick(HWND hwnd)
{
   HRESULT             dirval;           // Rückgabewert
   DIPROPRANGE         diprg;            // Joystick Eigenschaften

	// Joystick für enumerierte GUID erstellen
	dirval = lpDI->CreateDevice(guidJoystickDevice,&lpDIJoystick, NULL);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : CreateDevice Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Joystick : CreateDevice			: erfolgreich !\n", false);

	// Datenformat für Joystick festlegen
	dirval = lpDIJoystick->SetDataFormat(&c_dfDIJoystick);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : SetDataFormat Fehler !\n", true);
		return false;
    }
	Protokoll.WriteText("Joystick : SetDataFormat			: erfolgreich !\n", false);

	// Zusammenarbeit mit Windows regeln
	dirval = lpDIJoystick->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : SetCooperativeLevel Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Joystick : SetCooperativeLevel		: erfolgreich !\n", false);

	// Joystick Objekt Eigenschaften festlegen
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = DIJOFS_X;                    // x-Achse
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMin = -1000;                             // Wertebereich
	diprg.lMax = +1000;                             // von, bis

	dirval = lpDIJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : SetPropertyX Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Joystick : SetPropertyX			: erfolgreich !\n", false);

	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = DIJOFS_Y;                    // y-Achse
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMin = -1000;                             // Wertebereich
	diprg.lMax = +1000;                             // von, bis

	dirval = lpDIJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : SetPropertyY Fehler !\n", true);
		return false;
	}
	Protokoll.WriteText("Joystick : SetPropertyY			: erfolgreich !\n", false);

	// Joystick Objekt aktivieren
	if (lpDIJoystick) 
	{
		dirval = lpDIJoystick->Acquire();
		if (dirval != DI_OK) 
		{
			Protokoll.WriteText("\n-> Joystick : Acquire Fehler !\n", true);
			return false;
		}  
		Protokoll.WriteText("Joystick : Acquire				: erfolgreich !\n", false);
	}
   else 
	   return false;

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

	if(lpDIMaus != NULL)				// Maus freigeben
	{
		lpDIMaus->Unacquire();
		lpDIMaus->Release();
		lpDIMaus = NULL;
	}

	if(lpDIJoystick != NULL)			// Joystick freigeben
	{
		lpDIJoystick->Unacquire();
		lpDIJoystick->Release();
		lpDIJoystick = NULL;
	}
	
	SafeRelease(lpDI);					// DirectInput Hauptobjekt freigeben
	Protokoll.WriteText("-> DirectInput8 erfolgreich beendet !\n", false);
}

// --------------------------------------------------------------------------------------
// Tasten abfragen und im Puffer speichern
// --------------------------------------------------------------------------------------

bool DirectInputClass::TastaturUpdate(void)
{
	HRESULT hresult;

	hresult = lpDIKeyboard->GetDeviceState(sizeof(TastaturPuffer), (LPVOID)&TastaturPuffer); 
	if(hresult != DI_OK)
	{
		if(hresult == DIERR_INPUTLOST)				// Keyboard verloren gegangen ?
		{
			if(lpDIKeyboard)						// Dann nochmal versuchen
			{
				lpDIKeyboard->Acquire();
				hresult = lpDIKeyboard->GetDeviceState(sizeof(TastaturPuffer), 
												       (LPVOID)&TastaturPuffer); 
				if(hresult != DI_OK)
				{
					Protokoll.WriteText("\n-> Keyboard : GetDeviceState Fehler !\n", false);
					return false;
				}
			}
		}
		else										// anderer Fehler ?
		{
			Protokoll.WriteText("\n-> Keyboard : GetDeviceState Fehler !\n", false);
			return false;
		}
	}

	return true;
}

// --------------------------------------------------------------------------------------
// Maus abfragen
// true  = gepuffert
// false = ungepuffert
// --------------------------------------------------------------------------------------

bool DirectInputClass::MausUpdate(bool gepuffert)
{
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
// Maus wieder akquirieren (wenn Fenster gewechselt zB)
// --------------------------------------------------------------------------------------

void DirectInputClass::AcquireMaus(void)
{
	if(lpDIMaus)
		lpDIMaus->Acquire();
}