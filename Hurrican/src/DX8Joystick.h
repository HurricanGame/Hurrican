// Datei : DX8Joystick.h

// -------------------------------------------------------------------------------------- 
//
// Joystick Klasse
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __DX8Joystick_h__
#define __DX8Joystick_h__

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_JOYSTICKBUTTONS		128

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <dinput.h>

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Joystick Klasse
// --------------------------------------------------------------------------------------

class DirectJoystickClass
{
	public:
		LPDIRECTINPUTDEVICE8	lpDIJoystick;			// Joystick Device Interface		
		GUID					guidJoystickDevice;		// GUID des Joystick Devices
		LPDIRECTINPUTEFFECT		pFFE_SmallVib;			// Kurzes, schwaches Vibrieren
		LPDIRECTINPUTEFFECT		pFFE_BigVib;			// Kurzes, starkes Vibrieren
		LPDIRECTINPUTEFFECT		pFFE_MaxVib;			// Kurzes, heftiges Vibrieren
		LPDIRECTINPUTEFFECT		pFFE_Blitz;				// Blitz Effekt			

		bool  CanForceFeedback;
		bool  Active;
		int   JoystickX;								// Joystick x-Koordinaten
		int   JoystickY;								// Joystick y-Koordinaten
		int   JoystickX2;								// Joystick x-Koordinaten 2. analog Stick
		int   JoystickY2;								// Joystick y-Koordinaten 2. analog Stick
		int   JoystickPOV;								// POV (für coolie hat)
		int   JoystickMode;								// Joypad oder Stickmode		
		bool  JoystickButtons[MAX_JOYSTICKBUTTONS];		// Feuerknopf gedrückt?
		char  JoystickName[50];							// Joystick Produktname

		 DirectJoystickClass(void);							
		~DirectJoystickClass(void);							

	void ForceFeedbackEffect	(int nr);		
	void StopForceFeedbackEffect(int nr);		

	bool Init(HWND hwnd, LPDIRECTINPUT8 lpDI);
	bool Update(void);
};

#endif