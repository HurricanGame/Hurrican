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
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw(void);
};

#endif
