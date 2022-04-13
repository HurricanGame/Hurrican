#ifndef _TRIGGER_GLUBSCHI_HPP_
#define _TRIGGER_GLUBSCHI_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerGlubschi : public GegnerClass {
  private:
    float rotspeed;
    float rot;

  public:
    GegnerGlubschi(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw() override;
};

#endif
