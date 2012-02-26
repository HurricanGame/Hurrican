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

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_MOUSEBUTTONS		4
#define MAX_JOYSTICKBUTTONS		4
#define DINPUT_BUFFERSIZE		32
#define KeyDown(Taste) (TastaturPuffer[Taste] & 0x80)

// --------------------------------------------------------------------------------------
// Joystick-Enumerations Callbackfunktion
// --------------------------------------------------------------------------------------

bool CALLBACK EnumJoystickDevices(LPDIDEVICEINSTANCE lpddi, LPVOID pv);

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
		LPDIRECTINPUTDEVICE8	lpDIKeyboard;			// Keyboard Device Interface
		LPDIRECTINPUTDEVICE8	lpDIMaus;				// Maus Device Interface
		LPDIRECTINPUTDEVICE8	lpDIJoystick;			// Joystick Device Interface
		GUID					guidJoystickDevice;		// GUID des Joystick Devices
		int  JoystickX;									// Joystick x-Koordinaten
		int  JoystickY;									// Joystick y-Koordinaten
		bool JoystickButtons[MAX_JOYSTICKBUTTONS];		// Feuerknopf gedrückt?

	public:
		int						MausX;					// Koordinaten der
		int						MausY;					// der Maus und ihre Buttons
		bool					MausButtons[MAX_MOUSEBUTTONS];

		DirectInputClass(void);							// Konstruktor
	   ~DirectInputClass(void);							// Desktruktor
		bool Init(HWND hwnd, HINSTANCE hinst);			// DirectInput initialisieren
		bool InitJoystick(HWND hwnd);					// Joystick extra initialisieren
		void Exit(void);								// DirectInput beenden
		bool TastaturUpdate(void);						// Tasten abfragen und im Puffer speichern
		bool MausUpdate(bool gepuffert);				// Maus abfragen gepuffert/ungepuffert
		void AcquireKeyboard(void);						// Keyboard neu ansprechen
		void AcquireMaus(void);							// Maus neu ansprechen
};

#endif