// Datei : DX8Joystick.h

// --------------------------------------------------------------------------------------
//
// Joystick Klasse
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _DX8JOYSTICK_HPP_
#define _DX8JOYSTICK_HPP_

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

constexpr int MAX_JOYSTICKBUTTONS = 128;

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------
#if defined(PLATFORM_SDL)
#  include "SDL_port.hpp"
#endif

#include <bitset>
#include <string>

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Joystick Klasse
// --------------------------------------------------------------------------------------

class DirectJoystickClass {
  private:
    LPDIRECTINPUTDEVICE8 lpDIJoystick;  // Joystick Device Interface

    int startButton;
    int enterButton;
    int backButton;
    int deleteButton;

    bool CanForceFeedback;

  public:
    bool Active;
    int JoystickX;                              // Joystick x-Koordinaten
    int JoystickY;                              // Joystick y-Koordinaten
    int JoystickX2;                             // Joystick x-Koordinaten 2. analog Stick
    int JoystickY2;                             // Joystick y-Koordinaten 2. analog Stick
    int JoystickPOV;                            // POV (für coolie hat)
    int JoystickMode;                           // Joypad oder Stickmode
    std::bitset<MAX_JOYSTICKBUTTONS> JoystickButtons;  // Feuerknopf gedrückt?
    std::string JoystickName;                   // Joystick Produktname
    int NumButtons;                             // How many buttons joystick supports

    DirectJoystickClass();
    ~DirectJoystickClass();

    void ForceFeedbackEffect(int nr);
    void StopForceFeedbackEffect(int nr);

    bool Init(int joy);

    void Exit(int joy);

    bool Update();

    // DKS-Added these three for better joystick support, esp in menus
    // Returns true if button(s) serving as Enter key  are pressed (For menus)
    bool ButtonEnterPressed() const;

    // Returns true if button(s) serving as Escape key are pressed (For menus)
    bool ButtonEscapePressed() const;

    // Returns true if button(s) serving as Delete key are pressed (For menus, esp. button mapping menu)
    bool ButtonDeletePressed() const;

    // Returns true if button(s) serving as Start key are pressed (For game)
    bool ButtonStartPressed() const;
};
#endif
