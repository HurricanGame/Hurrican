#ifndef _GEGNER_AUGE_HPP_
#define _GEGNER_AUGE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerAuge : public GegnerClass {
  private:
    float smokedelay;

  public:
    GegnerAuge(int Wert1,
               int Wert2,  // Konstruktor
               bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
