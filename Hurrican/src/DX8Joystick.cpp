// Datei : DX8Joystick.cpp

// --------------------------------------------------------------------------------------
//
// Joystick Klasse
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "DX8Joystick.hpp"
#include "Gameplay.hpp"
#include "Logdatei.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectJoystickClass::DirectJoystickClass() {
    lpDIJoystick = NULL;

    Active = false;
    JoystickX = 0;
    JoystickY = 0;
    JoystickPOV = -1;
    CanForceFeedback = false;
    NumButtons = 0;

    for (int i = 0; i < MAX_JOYSTICKBUTTONS; i++)
        JoystickButtons[i] = false;
}

// --------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------

DirectJoystickClass::~DirectJoystickClass() {}

// --------------------------------------------------------------------------------------
// ForceFeedback Effekt "nr" starten
// --------------------------------------------------------------------------------------

void DirectJoystickClass::ForceFeedbackEffect(int nr) {
    if (UseForceFeedback == false || CanForceFeedback == false)
        return;
}

// --------------------------------------------------------------------------------------
// ForceFeedback Effekt "nr" anhalten
// --------------------------------------------------------------------------------------

void DirectJoystickClass::StopForceFeedbackEffect(int nr) {
    if (UseForceFeedback == false || CanForceFeedback == false)
        return;
}

    // --------------------------------------------------------------------------------------
    // Joystick initialisieren
    // --------------------------------------------------------------------------------------

bool DirectJoystickClass::Init(int joy) {
    lpDIJoystick = SDL_JoystickOpen(joy);

    if (lpDIJoystick == NULL) {
        Protokoll << "\n-> Joystick : Acquire error!" << std::endl;
        return false;
    }

    SDL_JoystickEventState(SDL_IGNORE); /* the joy events will be updated manually */

    Active = true;
    NumButtons = SDL_JoystickNumButtons(lpDIJoystick);
    CanForceFeedback = false;  // Forced false for now until implemented

    // Get joystick's name
    if (strlen(SDL_JoystickName(joy)) < sizeof(JoystickName)) {
        strcpy_s(JoystickName, SDL_JoystickName(joy));
    } else {
        strcpy_s(JoystickName, sizeof(JoystickName) - 1, SDL_JoystickName(joy));  // Truncate to fit
        JoystickName[sizeof(JoystickName) - 1] = '\0';                            // and null-terminate
    }

    Protokoll << "Joystick " << joy << ": Acquire successful!\nButtons: " << NumButtons << " Name: " << JoystickName
              << std::endl;

    return true;
}

    // --------------------------------------------------------------------------------------
    // Joystick updaten
    // --------------------------------------------------------------------------------------
bool DirectJoystickClass::Update() {
    if (lpDIJoystick != NULL) {
        SDL_JoystickUpdate();

        for (int i = 0; i < NumButtons; i++) {
            if (SDL_JoystickGetButton(lpDIJoystick, i) >= 1)
                JoystickButtons[i] = true;
            else
                JoystickButtons[i] = false;
        }

        if (SDL_JoystickNumAxes(lpDIJoystick) > 1) {
            // DKS - Map range of motion from SDL's (+/- 32768) to the original game's (+/- 1000)
            JoystickX = static_cast<int>(SDL_JoystickGetAxis(lpDIJoystick, 0) * (1000.0f / 32767.0f));
            JoystickY = static_cast<int>(SDL_JoystickGetAxis(lpDIJoystick, 1) * (1000.0f / 32767.0f));
        }

        if (SDL_JoystickNumHats(lpDIJoystick) > 0) {
            // DKS: Note - DirectX HAT values are -1 for centered, otherwise are in hundredths
            //      of a degree, from starting at 0 (UP, north) so right is 9000, down is 18000,
            //      left is 27000.
            uint8_t hat_state = SDL_JoystickGetHat(lpDIJoystick, 0);
            switch (hat_state) {
                case SDL_HAT_UP:
                    JoystickPOV = 0;
                    break;
                case SDL_HAT_RIGHTUP:
                    JoystickPOV = 4500;
                    break;
                case SDL_HAT_RIGHT:
                    JoystickPOV = 9000;
                    break;
                case SDL_HAT_RIGHTDOWN:
                    JoystickPOV = 13500;
                    break;
                case SDL_HAT_DOWN:
                    JoystickPOV = 18000;
                    break;
                case SDL_HAT_LEFTDOWN:
                    JoystickPOV = 22500;
                    break;
                case SDL_HAT_LEFT:
                    JoystickPOV = 27000;
                    break;
                case SDL_HAT_LEFTUP:
                    JoystickPOV = 31500;
                    break;
                case SDL_HAT_CENTERED:
                default:
                    JoystickPOV = -1;
                    break;
            }
        }
    }

    return true;
}

// DKS-Added these three for better joystick support, esp in menus
bool DirectJoystickClass::ButtonEnterPressed() {
    return JoystickButtons[0];  // Default is button 0
}

bool DirectJoystickClass::ButtonEscapePressed() {
    return JoystickButtons[1];  // Default is button 1
}

bool DirectJoystickClass::ButtonDeletePressed() {
    return JoystickButtons[4];  // Default is button 4
}
