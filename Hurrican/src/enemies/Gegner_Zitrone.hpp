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
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner  eigenen kleinen KI bewegen
    void DoDraw(void);         // Eigene Draw Routine
    void CalcKnarreWinkel(void);
};

#endif
