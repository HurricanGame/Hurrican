// Datei : Globals.hpp

// --------------------------------------------------------------------------------------
//
// Header Datei mit global gültigen Makros, Defines usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Mathematics.hpp"
#include "SDL_port.hpp"
#include "Texts.hpp"

#include <type_traits>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Spielzustände

enum class GameStateEnum {
  CRACKTRO,
  VORSPANN,
  INTRO,
  MAINMENU,
  GAMELOOP,
  OUTTRO
};

// --------------------------------------------------------------------------------------
// Definitionen für den Spielablauf
// --------------------------------------------------------------------------------------

constexpr float MAX_ENERGY = 100.0f;        // Maximalwert für die Spielerenergie
constexpr float MAX_ARMOUR = 200.0f;        // Maximalwert für das Rad
constexpr int DIAMONDS_FOR_EXTRALIFE = 50;  // Diamanten die man für 200 Zeiteinheiten

#define RARFILENAME "Hurrican.dat"
#define RARFILEPASSWORD "+/#(%,,/"

#if defined(__AROS__)
#  define TEMP_FILE_PREFIX "T:"
#else
#  define TEMP_FILE_PREFIX
#endif

// --------------------------------------------------------------------------------------
// Functions
// --------------------------------------------------------------------------------------
template<typename T>
inline void clampAngle(T& angle) {
    static_assert(std::is_arithmetic<T>::value, "Arithmetic value required in clampAngle.");

    constexpr T MIN_ANGLE = static_cast<T>(0);
    constexpr T MAX_ANGLE = static_cast<T>(360);

    while (angle < MIN_ANGLE)
      angle += MAX_ANGLE;
    while (angle >= MAX_ANGLE)
      angle -= MAX_ANGLE;
}

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern bool GameRunning;  // Spiel läuft :-)
#ifndef NDEBUG
extern bool DebugMode;  // Debug Mode ein/aus
#endif
extern bool FlameThrower;
extern bool HasCheated;

#endif
