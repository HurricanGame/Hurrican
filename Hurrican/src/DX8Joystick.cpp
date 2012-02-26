// Datei : DX8Joystick.cpp

// -------------------------------------------------------------------------------------- 
//
// Joystick Klasse
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "DX8Joystick.h"
#include "Logdatei.h"
#include "Gameplay.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectJoystickClass::DirectJoystickClass(void)
{
	lpDIJoystick = NULL;

	Active = false;
	JoystickX = 0;
	JoystickY = 0;
	JoystickPOV = -1;
	CanForceFeedback = false;

	for(int i = 0; i<MAX_JOYSTICKBUTTONS; i++)
		JoystickButtons[i] = false;
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

DirectJoystickClass::~DirectJoystickClass(void)
{	
}

// --------------------------------------------------------------------------------------
// ForceFeedback Effekt "nr" starten
// --------------------------------------------------------------------------------------

void DirectJoystickClass::ForceFeedbackEffect(int nr)
{
	if (UseForceFeedback == false ||
		CanForceFeedback == false)
		return;

	switch (nr)
	{
		case 0:
 			pFFE_SmallVib->Start(1,0);
		break;

		case 1:
			pFFE_BigVib->Start(1,0);
		break;

		case 2:
			pFFE_MaxVib->Start(1,0);
		break;

		case 3:
			pFFE_Blitz->Start(1,0);
		break;
	}
}

// --------------------------------------------------------------------------------------
// ForceFeedback Effekt "nr" anhalten
// --------------------------------------------------------------------------------------

void DirectJoystickClass::StopForceFeedbackEffect(int nr)
{
	if (UseForceFeedback == false ||
		CanForceFeedback == false)
		return;

	switch (nr)
	{
		case 0:
 			pFFE_SmallVib->Stop();
		break;

		case 1:
			pFFE_BigVib->Stop();
		break;

		case 2:
			pFFE_MaxVib->Stop();
		break;

		case 3:
			pFFE_Blitz->Stop();
		break;
	}
}

// --------------------------------------------------------------------------------------
// Joystick initialisieren
// --------------------------------------------------------------------------------------

bool DirectJoystickClass::Init(HWND hwnd, LPDIRECTINPUT8 lpDI)
{
   HRESULT             dirval;           // Rückgabewert
   DIPROPRANGE         diprg;            // Joystick Eigenschaften

	// Joystick für enumerierte GUID erstellen
	dirval = lpDI->CreateDevice(guidJoystickDevice,&lpDIJoystick, NULL);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : CreateDevice error!\n", false);
		return false;
	}
	Protokoll.WriteText("Joystick : CreateDevice successfull!\n", false);

	// Datenformat für Joystick festlegen
	dirval = lpDIJoystick->SetDataFormat(&c_dfDIJoystick2);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : SetDataFormat error!\n", false);
		return false;
    }
	Protokoll.WriteText("Joystick : SetDataFormat successfull!\n", false);

	// Zusammenarbeit mit Windows regeln
	dirval = lpDIJoystick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : SetCooperativeLevel error!\n", false);
		return false;
	}
	Protokoll.WriteText("Joystick : SetCooperativeLevel	successfull!\n", false);

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
		Protokoll.WriteText("\n-> Joystick : SetPropertyX error!\n", false);
		return false;
	}
	Protokoll.WriteText("Joystick : SetPropertyX sucessfull!\n", false);

	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = DIJOFS_Y;                    // y-Achse
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMin = -1000;                             // Wertebereich
	diprg.lMax = +1000;                             // von, bis

	dirval = lpDIJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (dirval != DI_OK) 
	{
		Protokoll.WriteText("\n-> Joystick : SetPropertyY error!\n", false);
		return false;
	}
	Protokoll.WriteText("Joystick : SetPropertyY successfull!\n", false);

	// Joystick Objekt aktivieren
	if (lpDIJoystick) 
	{
		dirval = lpDIJoystick->Acquire();
		if (dirval != DI_OK) 
		{
			Protokoll.WriteText("\n-> Joystick : Acquire error!\n", false);
			return false;
		}  
		Protokoll.WriteText("Joystick : Acquire successfull!\n", false);
	}
   else 
	   return false;

   Active = true;

   return true;	
}

// --------------------------------------------------------------------------------------
// Joystick updaten
// --------------------------------------------------------------------------------------

bool DirectJoystickClass::Update(void)
{
	HRESULT		hresult;
	DIJOYSTATE2	js;

	hresult = lpDIJoystick->Poll();
	hresult = lpDIJoystick->GetDeviceState (sizeof (DIJOYSTATE2), &js);

	// Joystick auf einmal weg?
	//
	if (hresult == DIERR_INPUTLOST)
	{
		// Versuchen, erneut zu holen
		//
		lpDIJoystick->Acquire();
		hresult = lpDIJoystick->GetDeviceState (sizeof (DIJOYSTATE2), &js);

		// immernoch wech? Dann gibts halt kein Joystick mehr
		//
		if (hresult != DI_OK) 
		{
			Protokoll.WriteText("\n-> Joystick : Re-Acquire Fehler !\n", false);
			return false;
		}

		// Ansonsten State holen
		//
		else 
		{
			hresult = lpDIJoystick->GetDeviceState (sizeof (DIJOYSTATE2), &js);

			// Wieder fehler? Dann wars das auch mit dem Joystick
			//
			if (hresult == DIERR_INPUTLOST) 
			{
				Protokoll.WriteText("\n-> Joystick : Re-Acquire Fehler !\n", false);
				return false;
			}
		}
	}

	for (int i = 0; i < MAX_JOYSTICKBUTTONS; i++)
	{
		if (js.rgbButtons [i] & 0x80)
			JoystickButtons[i] = true;
		else
			JoystickButtons[i] = false;
	}

	JoystickX = js.lX;
	JoystickY = js.lY;
	JoystickPOV = js.rgdwPOV[0];

	return true;
}
