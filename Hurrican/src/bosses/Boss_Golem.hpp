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
    ArmState state2;
    int StoneCount;
    int last;
    float Wackel;
    float WackelDir;
    float rotarm1;
    float rotarm2;
    float yoff;
    float ShotDelay;
    float count;

  public:
    GegnerGolem(int Wert1,
                int Wert2,  // Konstruktor
                bool Light);
    float WinkelToPlayer();
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
    void Wackeln();
    bool GoToZero(float &Value, float speed = 0.1f);
    // wackeln lassen beim Laufen
};

#endif
