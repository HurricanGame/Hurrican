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

#if defined(PLATFORM_DIRECTX)
#include <dinput.h>								// Wegen der Key-Code Defines
#endif
#include "DX8Input.h"
#include "GetKeyName.h"							// Unser Header =)

//---------------------------------------------------------------------------------------
// Funktion		: GetKeyName
// Beschreibung : Liefert einen String mit dem Names des als Parameter übergebenen
//				  DirectInput Keyboard Scan Codes zurück
//---------------------------------------------------------------------------------------

const char *GetKeyName(int DIK_Code)
{
#if defined(PLATFORM_SDL)
    //DKS - Now, we use SDL's names - The game originally assumed a German keyboard
#if SDL_VERSION_ATLEAST(2,0,0)
    return SDL_GetKeyName((SDL_Keycode)DIK_Code);
#else
    return SDL_GetKeyName((SDLKey)DIK_Code);
#endif //SDL_VERSION_ATLEAST(2,0,0)
#else

    const char *Text;									// Hier wird der Name der Taste gespeichert

    switch (DIK_Code)							// Name der Taste ermitteln
    {
    case DIK_ESCAPE          :
        Text = "Escape";
        break;
    case DIK_1               :
        Text = "1";
        break;
    case DIK_2               :
        Text = "2";
        break;
    case DIK_3               :
        Text = "3";
        break;
    case DIK_4               :
        Text = "4";
        break;
    case DIK_5               :
        Text = "5";
        break;
    case DIK_6               :
        Text = "6";
        break;
    case DIK_7               :
        Text = "7";
        break;
    case DIK_8               :
        Text = "8";
        break;
    case DIK_9               :
        Text = "9";
        break;
    case DIK_0               :
        Text = "0";
        break;
    case DIK_MINUS           :
        Text = "ß";
        break;
    case DIK_EQUALS          :
        Text = "´";
        break;
    case DIK_BACK            :
        Text = "Backspace";
        break;
    case DIK_TAB             :
        Text = "Tab";
        break;
    case DIK_Q               :
        Text = "Q";
        break;
    case DIK_W               :
        Text = "W";
        break;
    case DIK_E               :
        Text = "E";
        break;
    case DIK_R               :
        Text = "R";
        break;
    case DIK_T               :
        Text = "T";
        break;
    case DIK_Z               :
        Text = "Y";
        break;
    case DIK_U               :
        Text = "U";
        break;
    case DIK_I               :
        Text = "I";
        break;
    case DIK_O               :
        Text = "O";
        break;
    case DIK_P               :
        Text = "P";
        break;
    case DIK_LBRACKET        :
        Text = "UE";
        break;
    case DIK_RBRACKET        :
        Text = "+";
        break;
    case DIK_RETURN          :
        Text = "Return";
        break;
    case DIK_LCONTROL        :
        Text = "Left ctrl";
        break;
    case DIK_A               :
        Text = "A";
        break;
    case DIK_S               :
        Text = "S";
        break;
    case DIK_D               :
        Text = "D";
        break;
    case DIK_F               :
        Text = "F";
        break;
    case DIK_G               :
        Text = "G";
        break;
    case DIK_H               :
        Text = "H";
        break;
    case DIK_J               :
        Text = "J";
        break;
    case DIK_K               :
        Text = "K";
        break;
    case DIK_L               :
        Text = "L";
        break;
    case DIK_SEMICOLON       :
        Text = "OE";
        break;
    case DIK_APOSTROPHE      :
        Text = "AE";
        break;
    case DIK_GRAVE           :
        Text = "^";
        break;
    case DIK_LSHIFT          :
        Text = "Left shift";
        break;
    case DIK_BACKSLASH       :
        Text = "#";
        break;
    case DIK_Y               :
        Text = "Z";
        break;
    case DIK_X               :
        Text = "X";
        break;
    case DIK_C               :
        Text = "C";
        break;
    case DIK_V               :
        Text = "V";
        break;
    case DIK_B               :
        Text = "B";
        break;
    case DIK_N               :
        Text = "N";
        break;
    case DIK_M               :
        Text = "M";
        break;
    case DIK_COMMA           :
        Text = ",";
        break;
    case DIK_PERIOD          :
        Text = ".";
        break;
    case DIK_SLASH           :
        Text = "-";
        break;
    case DIK_RSHIFT          :
        Text = "Right shift";
        break;
    case DIK_MULTIPLY        :
        Text = "*";
        break;
    case DIK_LMENU           :
        Text = "Left alt";
        break;
    case DIK_SPACE           :
        Text = "Space";
        break;
    case DIK_CAPITAL         :
        Text = "Caps Lock";
        break;
    case DIK_F1              :
        Text = "F1";
        break;
    case DIK_F2              :
        Text = "F2";
        break;
    case DIK_F3              :
        Text = "F3";
        break;
    case DIK_F4              :
        Text = "F4";
        break;
    case DIK_F5              :
        Text = "F5";
        break;
    case DIK_F6              :
        Text = "F6";
        break;
    case DIK_F7              :
        Text = "F7";
        break;
    case DIK_F8              :
        Text = "F8";
        break;
    case DIK_F9              :
        Text = "F9";
        break;
    case DIK_F10             :
        Text = "F10";
        break;
    case DIK_NUMLOCK         :
        Text = "Num Lock";
        break;
    case DIK_SCROLL          :
        Text = "Scroll Lock";
        break;
    case DIK_NUMPAD7         :
        Text = "Numpad 7";
        break;
    case DIK_NUMPAD8         :
        Text = "Numpad 8";
        break;
    case DIK_NUMPAD9         :
        Text = "Numpad 9";
        break;
    case DIK_SUBTRACT        :
        Text = "Numpad -";
        break;
    case DIK_NUMPAD4         :
        Text = "Numpad 4";
        break;
    case DIK_NUMPAD5         :
        Text = "Numpad 5";
        break;
    case DIK_NUMPAD6         :
        Text = "Numpad 6";
        break;
    case DIK_ADD             :
        Text = "Numpad +";
        break;
    case DIK_NUMPAD1         :
        Text = "Numpad 1";
        break;
    case DIK_NUMPAD2         :
        Text = "Numpad 2";
        break;
    case DIK_NUMPAD3         :
        Text = "Numpad 3";
        break;
    case DIK_NUMPAD0         :
        Text = "Numpad 0";
        break;
    case DIK_DECIMAL         :
        Text = "Numpad .";
        break;
    case DIK_OEM_102         :
        Text = "<";
        break;
    case DIK_F11             :
        Text = "F11";
        break;
    case DIK_F12             :
        Text = "F12";
        break;
    case DIK_F13             :
        Text = "F13";
        break;
    case DIK_F14             :
        Text = "F14";
        break;
    case DIK_F15             :
        Text = "F15";
        break;
#if defined(PLATFORM_DIRECTX)
    case DIK_KANA            :
        Text = "Kana";
        break;
    case DIK_ABNT_C1         :
        Text = "?";
        break;
    case DIK_CONVERT         :
        Text = "Convert";
        break;
    case DIK_NOCONVERT       :
        Text = "No Convert";
        break;
    case DIK_YEN             :
        Text = "Yen";
        break;
    case DIK_ABNT_C2         :
        Text = "Numpad .";
        break;
    case DIK_NUMPADEQUALS    :
        Text = "Numpad =";
        break;
    case DIK_PREVTRACK       :
        Text = "Previous Track";
        break;
    case DIK_AT              :
        Text = "At";
        break;
    case DIK_COLON           :
        Text = "Colon";
        break;
    case DIK_UNDERLINE       :
        Text = "_";
        break;
    case DIK_KANJI           :
        Text = "Kanji";
        break;
    case DIK_STOP            :
        Text = "Stop";
        break;
    case DIK_AX              :
        Text = "AX";
        break;
    case DIK_UNLABELED       :
        Text = "Unlabeled";
        break;
    case DIK_NEXTTRACK       :
        Text = "Next Track";
        break;
    case DIK_NUMPADENTER     :
        Text = "Numpad Enter";
        break;
#endif
    case DIK_RCONTROL        :
        Text = "Right ctrl";
        break;
#if defined(PLATFORM_DIRECTX)
    case DIK_MUTE            :
        Text = "Mute";
        break;
    case DIK_CALCULATOR      :
        Text = "Calculator";
        break;
    case DIK_PLAYPAUSE       :
        Text = "Play / Pause";
        break;
    case DIK_MEDIASTOP       :
        Text = "Media Stop";
        break;
    case DIK_VOLUMEDOWN      :
        Text = "Volume -";
        break;
    case DIK_VOLUMEUP        :
        Text = "Volume +";
        break;
    case DIK_WEBHOME         :
        Text = "Web Home";
        break;
    case DIK_NUMPADCOMMA     :
        Text = "Numpad ,";
        break;
    case DIK_DIVIDE          :
        Text = "Numpad /";
        break;
    case DIK_SYSRQ           :
        Text = "PrintScreen";
        break;
#endif
    case DIK_RMENU           :
        Text = "Right alt";
        break;
#if defined(PLATFORM_DIRECTX)
    case DIK_PAUSE           :
        Text = "Pause";
        break;
#endif
    case DIK_HOME            :
        Text = "Home";
        break;
    case DIK_UP              :
        Text = "Cursor up";
        break;
    case DIK_PRIOR           :
        Text = "Page up";
        break;
    case DIK_LEFT            :
        Text = "Cursor left";
        break;
    case DIK_RIGHT           :
        Text = "Cursor right";
        break;
    case DIK_END             :
        Text = "End";
        break;
    case DIK_DOWN            :
        Text = "Cursor down";
        break;
    case DIK_NEXT            :
        Text = "Page down";
        break;
    case DIK_INSERT          :
        Text = "Insert";
        break;
    case DIK_DELETE          :
        Text = "Delete";
        break;
    case DIK_LWIN            :
        Text = "left windows";
        break;
    case DIK_RWIN            :
        Text = "right windows";
        break;
    case DIK_APPS            :
        Text = "AppMenu";
        break;
#if defined(PLATFORM_DIRECTX)
    case DIK_POWER           :
        Text = "System Power";
        break;
    case DIK_SLEEP           :
        Text = "System Sleep";
        break;
    case DIK_WAKE            :
        Text = "System Wake";
        break;
    case DIK_WEBSEARCH       :
        Text = "Web Search";
        break;
    case DIK_WEBFAVORITES    :
        Text = "Web Favorites";
        break;
    case DIK_WEBREFRESH      :
        Text = "Web Refresh";
        break;
    case DIK_WEBSTOP         :
        Text = "Web Stop";
        break;
    case DIK_WEBFORWARD      :
        Text = "Web Forward";
        break;
    case DIK_WEBBACK         :
        Text = "Web Back";
        break;
    case DIK_MYCOMPUTER      :
        Text = "My Computer";
        break;
    case DIK_MAIL            :
        Text = "Mail";
        break;
    case DIK_MEDIASELECT     :
        Text = "Media Select";
        break;
#endif
    default :
        Text = "Nicht definiert";
    } // switch

#if defined(PLATFORM_DIRECTX)
    DIPROPSTRING str;
    str.diph.dwSize = sizeof(DIPROPSTRING);
    str.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    str.diph.dwObj = DIK_Code;
    str.diph.dwHow = DIPH_BYOFFSET;
    DirectInput.lpDIKeyboard->GetProperty(DIPROP_KEYNAME, (LPDIPROPHEADER) &str);
#endif

    return Text;								// Und den Name der Taste zurückliefern
#endif // PLATFORM_SDL
}
