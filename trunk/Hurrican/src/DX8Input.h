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

#if defined(ANDROID)
#include <vector>
#endif
#if defined(PLATFORM_DIRECTX)
#include <dinput.h>
#endif
#include "DX8Joystick.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_MOUSEBUTTONS		4
#define DINPUT_BUFFERSIZE		32
#define MAX_KEYS              256
#if defined(PLATFORM_DIRECTX)
#define KeyDown(Taste) (TastaturPuffer[Taste] & 0x80)
#elif defined(PLATFORM_SDL)
#if SDL_VERSION_ATLEAST(2,0,0)
#define KeyDown(Taste) (TastaturPuffer[SDL_GetScancodeFromKey(Taste)] > 0)
#else
#define KeyDown(Taste) (TastaturPuffer[Taste] > 0)
#endif
#endif

// ForceFeedback Effekte
//
#define	FFE_SMALLRUMBLE 0
#define	FFE_MEDIUMRUMBLE 1
#define	FFE_BIGRUMBLE 2
#define	FFE_BLITZ 3

#define MAX_JOYSTICKS 4
//DKS - Fixed ambiguity: The game had ControlType and JoystickMode both using this #define, but
//      that was confusing and mixed up in the code:  Now, JoystickMode uses this define and
//      ControlType uses the one following it:
#define JOYMODE_JOYPAD		1       // NOTE: DO NOT change the values 1 & 2 here, Texts.cpp counts on these values.
#define JOYMODE_JOYSTICK	2       // There originally was a 0 but it was removed for reasons in comment above.

#define CONTROLTYPE_KEYBOARD    0
#define CONTROLTYPE_JOY         1

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
#if defined(GCW)
    int                     InternalJoystickIndex;  // If the system has built-in joystick controls, 
#endif //GCW
                                                    // this is their index, -1 otherwise.
public:

#if defined(PLATFORM_DIRECTX)
    LPDIRECTINPUTDEVICE8	lpDIKeyboard;			// Keyboard Device Interface
    LPDIRECTINPUTDEVICE8	lpDIMaus;				// Maus Device Interface
    GUID					guidJoystickDevice;		// GUID des Joystick Devices
#endif

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
    char* MapButtonToString(int joy_idx, int button);   // Returns char string "1" for button 0, etc..
                                                        // But when used on GCW Zero's internal controls
                                                        // (or other special cases) will return the custom
                                                        // string for the devices's buttons ("X", "Select", etc)

#if defined(GCW)
    // If the system has built-in joystick controls, this is their index. 0 is default.
    // On embedded systems like GCW Zero, this is set to the device's internal controls 
    // so that a player is never 'locked out' and also so the game is not dependent on
    // the system always setting the internal controls to device index 0. Likely useful on other devices, too. -DKS
    int  GetInternalJoystickIndex(void) { return InternalJoystickIndex; }

    // On GCW Zero, Main menu button is permanently mapped to START (button 5):
    int  GetInternalJoystickMainMenuButton(void) { return 5; }

    bool InternalJoystickMainMenuButtonDown(void);  
#endif //GCW

#if defined(ANDROID)
#define BOX_RECT_TOTAL 12
    uint16_t Width;
    uint16_t Height;
    uint16_t TouchDeviceCount;
    uint16_t TouchdpadX;
    uint16_t TouchdpadY;
    uint16_t TouchdpadRadius;
    void InitTouchBoxes( int w, int h );
    void UpdateTouchscreen( void );
    bool CheckRectCollision( SDL_Rect* boxA, SDL_Rect* boxB );
    std::vector<SDL_Rect> TouchBoxes;
    std::vector<SDL_Keycode> TouchBoxMaps;
#endif
    int NumberOfKeys;
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern DirectInputClass		DirectInput;
#if defined(PLATFORM_DIRECTX)
extern char					TastaturPuffer[256];				// Tastaturpuffer des Keyboards
#elif defined(PLATFORM_SDL)
extern char*				TastaturPuffer;
#endif
extern bool					UseForceFeedback;					// ForceFeedback Fähigkeit nutzen ?

#endif

