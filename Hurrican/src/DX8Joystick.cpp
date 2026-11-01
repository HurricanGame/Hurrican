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

#if SDL_VERSION_ATLEAST(2,0,0)
#  include <SDL_gamecontroller.h>
#endif

constexpr Uint16 rumble[4] = { 20000, 40000, 60000, 30000 };

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

DirectJoystickClass::DirectJoystickClass() :
    lpDIJoystick(nullptr),
#if SDL_VERSION_ATLEAST(2,0,0)
    lpGameController(nullptr),
#endif
    CanForceFeedback(false) {

    Active = false;
    JoystickX = 0;
    JoystickY = 0;
    JoystickPOV = -1;
    NumButtons = 0;
    NumAxis = 0;
    NumHats = 0;

    // hardcoded default button values
    startButton = 7;
    enterButton = 0;
    backButton = 1;
    deleteButton = 4;

    lt = 0;
    rt = 0;

    JoystickButtons.reset();
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
#if SDL_VERSION_ATLEAST(2,0,9)
    SDL_JoystickRumble(lpDIJoystick, rumble[nr], rumble[nr], 100 * nr);
#endif
}

// --------------------------------------------------------------------------------------
// ForceFeedback Effekt "nr" anhalten
// --------------------------------------------------------------------------------------

void DirectJoystickClass::StopForceFeedbackEffect(int nr) {
    if (UseForceFeedback == false || CanForceFeedback == false)
        return;
#if SDL_VERSION_ATLEAST(2,0,9)
    SDL_JoystickRumble(lpDIJoystick, 0, 0, 0);
#endif
}

#if SDL_VERSION_ATLEAST(2,0,0)
#  define SDLJOYINDEX lpDIJoystick
#else
#  define SDLJOYINDEX joy
#endif

    // --------------------------------------------------------------------------------------
    // Joystick initialisieren
    // --------------------------------------------------------------------------------------

bool DirectJoystickClass::Init(int joy) {
#if SDL_VERSION_ATLEAST(2,0,0)
    if (SDL_IsGameController(joy)) {
        lpGameController = SDL_GameControllerOpen(joy);
        if (lpGameController != nullptr) {
            lpDIJoystick = SDL_GameControllerGetJoystick(lpGameController);
        }
    }
#endif

    if (lpDIJoystick == nullptr) {
        lpDIJoystick = SDL_JoystickOpen(joy);
    }

    if (lpDIJoystick == nullptr) {
        Protokoll << "\n-> Joystick : Acquire error!" << std::endl;
        return false;
    }

    SDL_JoystickEventState(SDL_IGNORE); /* the joy events will be updated manually */

    Active = true;
    NumButtons = SDL_JoystickNumButtons(lpDIJoystick);
    NumAxis = SDL_JoystickNumAxes(lpDIJoystick);
    NumHats = SDL_JoystickNumHats(lpDIJoystick);

    // Get joystick's name
    JoystickName = SDL_JoystickName(SDLJOYINDEX);

    Protokoll << "Joystick " << joy << ": Acquire successful!"
        << " \nName: " << JoystickName
        << " \nAxis: " << NumAxis
        << " \nHats: " << NumHats
        << " \nButtons: " << NumButtons
        << std::endl;

#if SDL_VERSION_ATLEAST(2,0,18)
    CanForceFeedback = (SDL_JoystickHasRumble(lpDIJoystick) == SDL_TRUE);
#elif SDL_VERSION_ATLEAST(2,0,9)
    CanForceFeedback = (SDL_JoystickRumble(lpDIJoystick, 32768, 32768, 100) == 0);
    if (CanForceFeedback)
        SDL_JoystickRumble(lpDIJoystick, 0, 0, 0);
#endif
    Protokoll << "Force-feedback: " << std::boolalpha << CanForceFeedback << std::endl;

    TotalButtons = NumButtons;

#if SDL_VERSION_ATLEAST(2,0,0)
    if (lpGameController != nullptr) {
        Protokoll << "It's a Game Controller, mapping standard buttons..." << std::endl;

        SDL_GameControllerButtonBind bind;

        // try mapping START button
        bind = SDL_GameControllerGetBindForButton(lpGameController, SDL_CONTROLLER_BUTTON_START);
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
            startButton = bind.value.button;
            Protokoll << "Start function mapped to button START (" << startButton << ")" << std::endl;
        } /*else {
            // fallback to X button
            bind = SDL_GameControllerGetBindForButton(lpGameController, SDL_CONTROLLER_BUTTON_X);
            if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
                startButton = bind.value.button;
                Protokoll << "Start function mapped to button X (" << startButton << ")" << std::endl;
            }
        }*/
        // try mapping A button for enter function
        bind = SDL_GameControllerGetBindForButton(lpGameController, SDL_CONTROLLER_BUTTON_A);
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
            enterButton = bind.value.button;
            Protokoll << "Enter function mapped to button A (" << enterButton << ")" << std::endl;
        }
        // try mapping BACK button
        bind = SDL_GameControllerGetBindForButton(lpGameController, SDL_CONTROLLER_BUTTON_BACK);
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
            backButton = bind.value.button;
            Protokoll << "Back function mapped to button BACK (" << backButton << ")" << std::endl;
        } else {
            // fallback to B button
            bind = SDL_GameControllerGetBindForButton(lpGameController, SDL_CONTROLLER_BUTTON_B);
            if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
                backButton = bind.value.button;
                Protokoll << "Back function mapped to button B (" << backButton << ")" << std::endl;
            }
        }
        // try mapping LEFT SHOULDER button for delete function
        bind = SDL_GameControllerGetBindForButton(lpGameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
            deleteButton = bind.value.button;
            Protokoll << "Delete function mapped to button LB (" << deleteButton << ")" << std::endl;
        } else {
            // fallback to Y button
            bind = SDL_GameControllerGetBindForButton(lpGameController, SDL_CONTROLLER_BUTTON_Y);
            if (bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) {
                deleteButton = bind.value.button;
                Protokoll << "Delete function mapped to button Y (" << deleteButton << ")" << std::endl;
            }
        }

        // try mapping LEFT SHOULDER trigger
        bind = SDL_GameControllerGetBindForAxis(lpGameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS) {
            lt = bind.value.axis;
            Protokoll << "LT mapped to button (" << lt << ")" << std::endl;
            TotalButtons++;
        }
        // try mapping RIGHT SHOULDER trigger
        bind = SDL_GameControllerGetBindForAxis(lpGameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        if (bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS) {
            rt = bind.value.axis;
            Protokoll << "RT mapped to button (" << rt << ")" << std::endl;
            TotalButtons++;
        }
    }
#endif

    return true;
}

void DirectJoystickClass::Exit(int joy) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
    if (lpGameController != nullptr) {
        SDL_GameControllerClose(lpGameController);
        lpGameController = nullptr;
        lpDIJoystick = nullptr;
    }
#endif
    if (lpDIJoystick != nullptr) {
        SDL_JoystickClose(lpDIJoystick);
        lpDIJoystick = nullptr;
    }
}

    // --------------------------------------------------------------------------------------
    // Joystick updaten
    // --------------------------------------------------------------------------------------
bool DirectJoystickClass::Update() {
    if (lpDIJoystick != nullptr) {
        SDL_JoystickUpdate();

        for (int i = 0; i < NumButtons; i++) {
            JoystickButtons[i] = SDL_JoystickGetButton(lpDIJoystick, i) > 0;
        }

        {
            int i = NumButtons;
            if (lt) {
                JoystickButtons[i] = SDL_JoystickGetAxis(lpDIJoystick, lt) > 0;
                i++;
            }
            if (rt) {
                JoystickButtons[i] = SDL_JoystickGetAxis(lpDIJoystick, rt) > 0;
                i++;
            }
        }

#if SDL_VERSION_ATLEAST(2, 0, 0)
        if (lpGameController != nullptr) {
            // Standardized analog sticks
            int16_t axis_x = SDL_GameControllerGetAxis(lpGameController, SDL_CONTROLLER_AXIS_LEFTX);
            int16_t axis_y = SDL_GameControllerGetAxis(lpGameController, SDL_CONTROLLER_AXIS_LEFTY);
            JoystickX = static_cast<int>(axis_x * (1000.0f / 32767.0f));
            JoystickY = static_cast<int>(axis_y * (1000.0f / 32767.0f));

            int16_t axis_x2 = SDL_GameControllerGetAxis(lpGameController, SDL_CONTROLLER_AXIS_RIGHTX);
            int16_t axis_y2 = SDL_GameControllerGetAxis(lpGameController, SDL_CONTROLLER_AXIS_RIGHTY);
            JoystickX2 = static_cast<int>(axis_x2 * (1000.0f / 32767.0f));
            JoystickY2 = static_cast<int>(axis_y2 * (1000.0f / 32767.0f));

            // Standardized D-Pad
            bool d_up = SDL_GameControllerGetButton(lpGameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
            bool d_down = SDL_GameControllerGetButton(lpGameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            bool d_left = SDL_GameControllerGetButton(lpGameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            bool d_right = SDL_GameControllerGetButton(lpGameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

            if (d_up && d_right)            JoystickPOV = 4500;
            else if (d_down && d_right)     JoystickPOV = 13500;
            else if (d_down && d_left)      JoystickPOV = 22500;
            else if (d_up && d_left)        JoystickPOV = 31500;
            else if (d_up)                  JoystickPOV = 0;
            else if (d_right)               JoystickPOV = 9000;
            else if (d_down)                JoystickPOV = 18000;
            else if (d_left)                JoystickPOV = 27000;
            else if (NumHats > 0) {
                uint8_t hat_state = SDL_JoystickGetHat(lpDIJoystick, 0);
                switch (hat_state) {
                    case SDL_HAT_UP:        JoystickPOV = 0; break;
                    case SDL_HAT_RIGHTUP:   JoystickPOV = 4500; break;
                    case SDL_HAT_RIGHT:     JoystickPOV = 9000; break;
                    case SDL_HAT_RIGHTDOWN: JoystickPOV = 13500; break;
                    case SDL_HAT_DOWN:      JoystickPOV = 18000; break;
                    case SDL_HAT_LEFTDOWN:  JoystickPOV = 22500; break;
                    case SDL_HAT_LEFT:      JoystickPOV = 27000; break;
                    case SDL_HAT_LEFTUP:    JoystickPOV = 31500; break;
                    case SDL_HAT_CENTERED:
                    default:                JoystickPOV = -1; break;
                }
            } else {
                JoystickPOV = -1;
            }
        } else
#endif
        {
            if (NumAxis > 1) {
                // DKS - Map range of motion from SDL's (+/- 32768) to the original game's (+/- 1000)
                JoystickX = static_cast<int>(SDL_JoystickGetAxis(lpDIJoystick, 0) * (1000.0f / 32767.0f));
                JoystickY = static_cast<int>(SDL_JoystickGetAxis(lpDIJoystick, 1) * (1000.0f / 32767.0f));
            }

            if (NumHats > 0) {
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
    }

    return true;
}

// DKS-Added these three for better joystick support, esp in menus
bool DirectJoystickClass::ButtonEnterPressed() const {
    return JoystickButtons[enterButton];
}

bool DirectJoystickClass::ButtonEscapePressed() const {
    return JoystickButtons[backButton];
}

bool DirectJoystickClass::ButtonDeletePressed() const {
    return JoystickButtons[deleteButton];
}

bool DirectJoystickClass::ButtonStartPressed() const {
    return JoystickButtons[startButton];
}
