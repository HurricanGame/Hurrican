#ifndef _BOSS_BRATKLOPS_HPP_
#define _BOSS_BRATKLOPS_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerBratklops : public GegnerClass {
  private:
    float ShotDelay;
    float ActionDelay;
    float FlareDelay;
    int Shots;

  public:
    DirectGraphicsSprite *pGfx[6];  // Animationen sind einzeln
    DirectGraphicsSprite *pLaser;   // Lasergrafik
    DirectGraphicsSprite *pFlare;   // Laserflare

    GegnerBratklops(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
