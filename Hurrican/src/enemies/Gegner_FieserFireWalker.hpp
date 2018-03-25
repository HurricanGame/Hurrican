#ifndef _GEGNER_FIESERFIREWALKER_HPP_
#define _GEGNER_FIESERFIREWALKER_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFieserFireWalker : public GegnerClass {
  private:
    float ShotDelay;
    float ShotDelay2;

  public:
    GegnerFieserFireWalker(int Wert1, int Wert2, bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Gegner individuell rendern
};

#endif
