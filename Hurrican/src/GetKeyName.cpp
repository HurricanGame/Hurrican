// Datei : GetKeyName.cpp

//---------------------------------------------------------------------------------------
//
// Erleichtert das Abfragen von Tasten-Namen mit DInput
// von Jörg Matthias Winterstein
//
// (c) 2002 Poke53280
//
// visit www.poke53280.de
//
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// Include-Dateien
//---------------------------------------------------------------------------------------

#include "GetKeyName.hpp"  // Unser Header =)
#include "DX8Input.hpp"

//---------------------------------------------------------------------------------------
// Funktion		: GetKeyName
// Beschreibung : Liefert einen String mit dem Names des als Parameter übergebenen
//				  DirectInput Keyboard Scan Codes zurück
//---------------------------------------------------------------------------------------

const char *GetKeyName(int DIK_Code) {
// DKS - Now, we use SDL's names - The game originally assumed a German keyboard
#if SDL_VERSION_ATLEAST(2, 0, 0)
    return SDL_GetKeyName(static_cast<SDL_Keycode>(DIK_Code));
#else
    return SDL_GetKeyName(static_cast<SDLKey>(DIK_Code));
#endif  // SDL_VERSION_ATLEAST(2,0,0)
}
