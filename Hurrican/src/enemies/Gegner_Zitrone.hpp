#ifndef _GEGNER_ZITRONE_HPP_
#define _GEGNER_ZITRONE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerZitrone : public GegnerClass {
  private:
    float SmokeDelay;
    float WackelOffset;
    float KnarreWinkel;
    float FollowCount;
    float NewX, NewY;
    float ShotDelay;

  public:
    GegnerZitrone(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner  eigenen kleinen KI bewegen
    void DoDraw() override;         // Eigene Draw Routine
    void CalcKnarreWinkel();
};

#endif
