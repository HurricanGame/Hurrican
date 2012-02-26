// Datei : DX8Input.h

// -------------------------------------------------------------------------------------- 
//
// DirectInput Klasse
// zum Erfassen von Eingaben durch Tastaur, Maus und Joystick/Joypad
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __DX8Input_h__
#define __DX8Input_h__

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <dinput.h>
#include "DX8Joystick.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_MOUSEBUTTONS		4
#define DINPUT_BUFFERSIZE		32
#define KeyDown(Taste) (TastaturPuffer[Taste] & 0x80)

// ForceFeedback Effekte
//
#define	FFE_SMALLRUMBLE 0
#define	FFE_MEDIUMRUMBLE 1
#define	FFE_BIGRUMBLE 2
#define	FFE_BLITZ 3

#define MAX_JOYSTICKS 4
#define JOYMODE_KEYBOARD 0
#define JOYMODE_PAD		 1	
#define JOYMODE_STICK	 2

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// DirectInput Klasse
// --------------------------------------------------------------------------------------

class DirectInputClass
{
	private:
		LPDIRECTINPUT8			lpDI;					// DirectInput Hauptinterface				

	public:

		LPDIRECTINPUTDEVICE8	lpDIKeyboard;			// Keyboard Device Interface
		LPDIRECTINPUTDEVICE8	lpDIMaus;				// Maus Device Interface				
		GUID					guidJoystickDevice;		// GUID des Joystick Devices

		int						JoysticksFound;
		DirectJoystickClass		Joysticks[MAX_JOYSTICKS];

		int						MausX;					// Koordinaten der
		int						MausY;					// der Maus und ihre Buttons
		bool					MausButtons[MAX_MOUSEBUTTONS];

		DirectInputClass(void);							// Konstruktor
	   ~DirectInputClass(void);							// Desktruktor
		bool Init(HWND hwnd, HINSTANCE hinst);			// DirectInput initialisieren
		void Exit(void);								// DirectInput beenden
		bool UpdateTastatur(void);						// Tasten abfragen und im Puffer speichern
		bool UpdateMaus	   (bool gepuffert);			// Maus abfragen gepuffert/ungepuffert		
		void AcquireKeyboard(void);						// Keyboard akquirierens
		bool AreAllKeysReleased();						// Checken ob keine Taste mehr gedrückt ist
		bool AnyKeyDown(void);
		bool AnyButtonDown(void);
		void UpdateJoysticks(void);
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern DirectInputClass		DirectInput;
extern char					TastaturPuffer[256];				// Tastaturpuffer des Keyboards
extern bool					UseForceFeedback;					// ForceFeedback Fähigkeit nutzen ?

#endif