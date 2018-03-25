#ifndef _GEGNER_CLIMBER_HPP_
#define _GEGNER_CLIMBER_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerClimber : public GegnerClass {
  private:
    float rot;
    float shotdelay;
    float rotspeed;

  public:
    GegnerClimber(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Gegner individuell rendern
};

#endif
