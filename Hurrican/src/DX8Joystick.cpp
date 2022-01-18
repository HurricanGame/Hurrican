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
    CanForceFeedback(false) {

    Active = false;
    JoystickX = 0;
    JoystickY = 0;
    JoystickPOV = -1;;
    NumButtons = 0;

    // hardcoded default button values
    startButton = 7;
    enterButton = 0;
    backButton = 1;
    deleteButton = 4;

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
    lpDIJoystick = SDL_JoystickOpen(joy);

    if (lpDIJoystick == nullptr) {
        Protokoll << "\n-> Joystick : Acquire error!" << std::endl;
        return false;
    }

    SDL_JoystickEventState(SDL_IGNORE); /* the joy events will be updated manually */

    Active = true;
    NumButtons = SDL_JoystickNumButtons(lpDIJoystick);

    // Get joystick's name
    JoystickName = SDL_JoystickName(SDLJOYINDEX);

    Protokoll << "Joystick " << joy << ": Acquire successful!\nButtons: " << NumButtons
        << " \nName: " << JoystickName << std::endl;

#if SDL_VERSION_ATLEAST(2,0,18)
    CanForceFeedback = (SDL_JoystickHasRumble(lpDIJoystick) == SDL_TRUE);
#elif SDL_VERSION_ATLEAST(2,0,9)
    CanForceFeedback = (SDL_JoystickRumble(lpDIJoystick, 32768, 32768, 100) == 0);
    if (CanForceFeedback)
        SDL_JoystickRumble(lpDIJoystick, 0, 0, 0);
#endif
    Protokoll << "Force-feedback: " << std::boolalpha << CanForceFeedback << std::endl;

#if SDL_VERSION_ATLEAST(2,0,0)
    if (SDL_IsGameController(joy)) {
        Protokoll << "It's a Game Controller, mapping standard buttons..." << std::endl;

        SDL_GameController* controller = SDL_GameControllerOpen(joy);
        SDL_GameControllerButtonBind bind;
        
        bind = SDL_GameControllerGetBindForButton(controller, SDL_CONTROLLER_BUTTON_START);
        if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            startButton = bind.value.button;
            Protokoll << "Start function mapped to button START (" << startButton << ")" << std::endl;
        }
        bind = SDL_GameControllerGetBindForButton(controller, SDL_CONTROLLER_BUTTON_A);
        if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            enterButton = bind.value.button;
            Protokoll << "Enter function mapped to button A (" << enterButton << ")" << std::endl;
        }
        bind = SDL_GameControllerGetBindForButton(controller, SDL_CONTROLLER_BUTTON_B);
        if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            backButton = bind.value.button;
            Protokoll << "Back function mapped to button B (" << backButton << ")" << std::endl;
        }
        bind = SDL_GameControllerGetBindForButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        if (bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
            deleteButton = bind.value.button;
            Protokoll << "Delete function mapped to button LB (" << deleteButton << ")" << std::endl;
        }
        SDL_GameControllerClose(controller);
    }
#endif

    return true;
}

void DirectJoystickClass::Exit(int joy) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        if (lpDIJoystick != nullptr)
#else
        if (SDL_JoystickOpened(joy))
#endif
        {
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
