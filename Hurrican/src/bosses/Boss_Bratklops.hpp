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

    D3DXMATRIX matWorld;

  public:
    DirectGraphicsSprite *pGfx[7];  // Animationen sind einzeln
    DirectGraphicsSprite *pLaser;   // Lasergrafik
    DirectGraphicsSprite *pFlare;   // Laserflare

    GegnerBratklops(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Gegner individuell rendern
};

#endif
