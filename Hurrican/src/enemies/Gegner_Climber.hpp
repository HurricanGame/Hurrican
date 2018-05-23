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
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
