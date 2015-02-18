// Datei : Intro.h

// --------------------------------------------------------------------------------------
//
// Klasse für das Hurrican Intro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef __INTRO_H_
#define __INTRO_H_

// --------------------------------------------------------------------------------------
// Inlcudces
// --------------------------------------------------------------------------------------

#include "Globals.h"
#include "DX8Font.h"
#include "DX8Sprite.h"
#include "Timer.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Zustände

#define INTRO_FADEIN			0
#define INTRO_RUN				1
#define INTRO_FADEOUT			2
#define INTRO_DONE				3

// --------------------------------------------------------------------------------------
// Klassen Deklaration für das Intro
// --------------------------------------------------------------------------------------

//DKS - Entries in a  dynamically-size vector of text that will actually be displayed on-screen, based on font size:
struct IntroEntry {
    std::string text;
    bool should_pause;  // If a line was split, only pause on the second of the two lines
};

class IntroClass
{
private:
    DirectGraphicsSprite	Background[6];
    int						BildNr;
    int                     EntriesOff;  // Offset into actual lines displayed on the screen (can vary based on font size)
    int						TextOff;
    //DKS - Horizontal text scrolling now depends on an separate counter that doesn't get reset on skipped lines:
    float					HorizCounter;
    float                   Counter;
    //DKS - Added dynamically-size vector of text that will actually be displayed on-screen, based on font size:
    std::vector<IntroEntry> entries;        

public:
    int						Zustand;
    IntroClass(void);							// Konstruktor
    ~IntroClass(void);							// Destruktor
    void DoIntro   (void);							// Intro ablaufen lassen
    void EndIntro  (void);

};

#endif
