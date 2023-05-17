#ifndef _BOSS_BIGFISH_HPP_
#define _BOSS_BIGFISH_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerBigFish : public GegnerClass {
  private:
    int ShotCount;
    float MaulWinkel;
    float SinOff;
    float NewX, NewY;
    float rotz;
    float KugelCount;
    bool Moving;  // Bewegt sich der Fisch gerade?
    DirectGraphicsSprite Maul;
    DirectGraphicsSprite FlosseKlein;
    DirectGraphicsSprite FlosseGross;
    DirectGraphicsSprite FlosseOben;
    DirectGraphicsSprite FlosseUnten;

  public:
    GegnerBigFish(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void NewAction();
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner bewegen
    void DoDraw() override;  // Gegner individuell rendern
    void MoveTo(float x, float y);
    void DoMove();
};

#endif
