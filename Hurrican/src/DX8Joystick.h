// Datei : DX8Joystick.h

// --------------------------------------------------------------------------------------
//
// Joystick Klasse
//
// (c) 2002 J�rg M. Winterstein
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

#if defined(PLATFORM_DIRECTX)
#include <dinput.h>
#endif
#if defined(PLATFORM_SDL)
#include "SDL_port.h"
#endif

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
#if defined(PLATFORM_DIRECTX)
    GUID					guidJoystickDevice;		// GUID des Joystick Devices
    LPDIRECTINPUTEFFECT		pFFE_SmallVib;			// Kurzes, schwaches Vibrieren
    LPDIRECTINPUTEFFECT		pFFE_BigVib;			// Kurzes, starkes Vibrieren
    LPDIRECTINPUTEFFECT		pFFE_MaxVib;			// Kurzes, heftiges Vibrieren
    LPDIRECTINPUTEFFECT		pFFE_Blitz;				// Blitz Effekt
#endif

    bool  CanForceFeedback;
    bool  Active;
    int   JoystickX;								// Joystick x-Koordinaten
    int   JoystickY;								// Joystick y-Koordinaten
    int   JoystickX2;								// Joystick x-Koordinaten 2. analog Stick
    int   JoystickY2;								// Joystick y-Koordinaten 2. analog Stick
    int   JoystickPOV;								// POV (f�r coolie hat)
    int   JoystickMode;								// Joypad oder Stickmode
    bool  JoystickButtons[MAX_JOYSTICKBUTTONS];		// Feuerknopf gedr�ckt?
    char  JoystickName[50];							// Joystick Produktname

    DirectJoystickClass(void);
    ~DirectJoystickClass(void);

    void ForceFeedbackEffect	(int nr);
    void StopForceFeedbackEffect(int nr);

#if defined(PLATFORM_DIRECTX)
    bool Init(HWND hwnd, LPDIRECTINPUT8 lpDI);
#elif defined(PLATFORM_SDL)
    bool Init(int joy);
#endif

    bool Update(void);
};

#endif

