#ifndef _BOSS_FAHRSTUHLBOSS_HPP_
#define _BOSS_FAHRSTUHLBOSS_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFahrstuhlBoss : public GegnerClass {
  private:
    DirectGraphicsSprite Head;   // Grafiken für den "Kopf" des Gegners
    DirectGraphicsSprite Canon;  // Grafiken für die zwei Kanonen
    int Shots;       // Wieviele Schüsse abgeben
    float ShotDelay;             // Schussverzögerung
    float TempY;                 // yPosition relativ zum Fahrstuhl
    float x1, y1, x2, y2, x3;    // Koordinaten für Kopf und Kanonen
    float dx1, dx2;              // Speed für Kopf und Kanonen
    float CanonAnim;             // AnimationsPhase der Kanone

    bool Activated;  // Boss kommt schon rein

  public:
    GegnerFahrstuhlBoss(int Wert1,
                        int Wert2,  // Konstruktor
                        bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
