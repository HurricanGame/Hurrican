#ifndef _BOSS_DRACHE_HPP_
#define _BOSS_DRACHE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerDrache : public GegnerClass {
  private:
    DirectGraphicsSprite Head[2];
    DirectGraphicsSprite Hals;
    DirectGraphicsSprite Tail;
    DirectGraphicsSprite Spitze;
    DirectGraphicsSprite Leg;
    DirectGraphicsSprite Arm;

    int Attack;          // Welche "Unterhandlung"
    int mirrorOffset;    // Zum Zeichnen wenn gespiegelt
    int ShotCount;
    DirectionEnum Position;
    float SmokeCount;    // Für Antriebspartikel
    float TailSinus;     // Für das Schwanzwedeln (nicht das, was DU jetzt denkst... ;)
    float HeadWinkel;    // Kopf Ausrichtung, zeigt immer auf Spieler
    float AnimWinkel;    // Arme und Beine Winkel
    float KieferWinkel;  // Kiefer kann auf und zu gehen
    float HeadX, HeadY, HeadXSpeed, HeadYSpeed;
    float ShotDelay;
    float StartPosY;
    float DrawYOffset;
    float ArrowCount;
    float FlareRot;
    bool HeadLocked;     // Kopf ist fest und dreht nicht zu Spieler
    bool Growl;          // Heute schon gegröhlt? =)

    void ComputeHeadWinkel();

  public:
    GegnerDrache(int Wert1,
                 int Wert2,  // Konstruktor
                 bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;  // Gegner individuell rendern
                             // eigenen kleinen KI
                             // bewegen
};

#endif
