#ifndef _BOSS_SCHNEEKOENIG_HPP_
#define _BOSS_SCHNEEKOENIG_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchneeKoenig : public GegnerClass {
  private:
    int ShotCount;
    int SpezialAktion;
    int Action;
    float ySave;
    float LastEnergy;
    float SmokeDelay;  // Rauchcounter
    float yOffset;
    float KnarreWinkel;
    float KnarreY;
    float ShotDelay;
    float WackelAnim;
    float WackelCount;
    float GunSlide;
    bool DrawNow;

    DirectGraphicsSprite Knarre;
    DirectGraphicsSprite Laser;

    float WinkelToPlayer();
    void RenderLaser();

  public:
    GegnerSchneeKoenig(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
    void NextAction(int NewAction = -1);
};

#endif
