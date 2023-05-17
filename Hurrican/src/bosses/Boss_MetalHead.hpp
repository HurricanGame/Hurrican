#ifndef _BOSS_METALHEAD_HPP_
#define _BOSS_METALHEAD_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerMetalHead : public GegnerClass {
// Struct für einen Halswirbel
//
#define MAXWIRBEL 25

// Aktionen/Angriffe (SK = StahlKopp)
//
#define SK_PAUSE 0     // Short Pause
#define SK_AUSGANG 1   // Starting position at the top left
#define SK_AUSHOLEN 2  // Going out for headbanging
#define SK_BANGEN 3    // Headbanging, in other words, banging head to the right
#define SK_CHASING 4   // Track player
#define SK_TURBINE 5   // Prepare for turbine suction
#define SK_WACKELN 6   // Head wobbles up and down and hits the ceiling
#define SK_SPUCKEN 7   // Head spits fireballs from the turbine

    struct wirbel {
        float x, y;
        float w;
    };

  private:
    int ShotCount;
    int Akt;  // Aktueller Angriff (siehe Defines)
    int ShotArt;
    float KieferSpeed;
    float KieferPos;
    float TurbineOff;
    float Eye_Alpha;
    float Eye_Winkel;
    float SmokeCount;
    float ShotDelay;
    float GunWinkel;
    float NewX, NewY;
    float MoveSpeed;  // Wie schnell bewegt sich der Kopf zum neuen Zielpunkt ?
    float SinCount;

    wirbel Hals[MAXWIRBEL];

    // Für die Bewegung mit den xto und yto Koordinaten müssen wir uns am Anfang merken, ob
    // wir uns jetzt rechts oder links bzw hoch oder runter bewegen, um beim Check, ob der
    // MoveTo Punkt erreicht wurde, die richtige Abfrage setzen zu können.
    //
    bool Moving;
    bool Turbine_dran;  // Ist die Turbine noch dran

    DirectGraphicsSprite Kiefer;
    DirectGraphicsSprite Kiefer2;
    DirectGraphicsSprite Turbine;
    DirectGraphicsSprite Turbine2;
    DirectGraphicsSprite HalsGfx;
    DirectGraphicsSprite Flare;

    void DoMove();
    void WinkelToPlayer();

  public:
    GegnerMetalHead(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
    void MoveToNewPoint(float x,
                        float y,
                        float s,  // neuen MoveTo Punkt festlegen mit Pos, Speed und Aktion
                        int Akt);
};

#endif
