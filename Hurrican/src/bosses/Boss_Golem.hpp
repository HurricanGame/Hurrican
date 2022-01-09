#ifndef _BOSS_GOLEM_HPP_
#define _BOSS_GOLEM_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

enum class ArmState {
  HEBEN,
  SENKEN,
  SENKEN2,
  UEBERKOPF,
  WERFEN
};

class GegnerGolem : public GegnerClass {
  private:
    DirectGraphicsSprite arm[2];
    DirectGraphicsSprite load;
    float Wackel;
    float WackelDir;
    float rotarm1;
    float rotarm2;
    float yoff;
    float ShotDelay;
    float count;
    ArmState state2;
    int StoneCount;
    int last;

  public:
    GegnerGolem(int Wert1,
                int Wert2,  // Konstruktor
                bool Light);
    float WinkelToPlayer();
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Gegner individuell rendern
    void Wackeln();
    bool GoToZero(float &Value, float speed = 0.1f);
    // wackeln lassen beim Laufen
};

#endif
