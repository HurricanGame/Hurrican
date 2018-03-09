#ifndef _GEGNER_QUALLE_HPP_
#define _GEGNER_QUALLE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerQualle : public GegnerClass {
  private:
    float ShotDelay;

  public:
    GegnerQualle(int Wert1,
                 int Wert2,  // Konstruktor
                 bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
