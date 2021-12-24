// Datei : Intro.hpp

// --------------------------------------------------------------------------------------
//
// Klasse für das Hurrican Intro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _INTRO_HPP_
#define _INTRO_HPP_

// --------------------------------------------------------------------------------------
// Inlcudces
// --------------------------------------------------------------------------------------

#include "DX8Font.hpp"
#include "DX8Sprite.hpp"
#include "Globals.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Zustände

enum class IntroStateEnum {
  FADEIN,
  RUN,
  FADEOUT,
  DONE
};

// --------------------------------------------------------------------------------------
// Klassen Deklaration für das Intro
// --------------------------------------------------------------------------------------

// DKS - Entries in a  dynamically-size vector of text that will actually be displayed on-screen, based on font size:
struct IntroEntry {
    std::string text;
    bool should_pause;  // If a line was split, only pause on the second of the two lines
};

class IntroClass {
  private:
    DirectGraphicsSprite Background[6];
    IntroStateEnum Zustand;
    int BildNr;
    int EntriesOff;  // Offset into actual lines displayed on the screen (can vary based on font size)
    int TextOff;
    // DKS - Horizontal text scrolling now depends on an separate counter that doesn't get reset on skipped lines:
    float HorizCounter;
    float Counter;
    // DKS - Added dynamically-size vector of text that will actually be displayed on-screen, based on font size:
    std::vector<IntroEntry> entries;

  public:
    IntroClass();    // Konstruktor
    ~IntroClass();   // Destruktor
    void DoIntro();  // Intro ablaufen lassen
    void EndIntro();

    bool IsDone() const { return Zustand == IntroStateEnum::DONE; }
};

#endif
