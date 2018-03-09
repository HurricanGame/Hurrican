#ifndef _GEGNER_MADE_HPP_
#define _GEGNER_MADE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerMade : public GegnerClass {
  private:
    float movesin;

  public:
    GegnerMade(int Wert1,
               int Wert2,  // Konstruktor
               bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner KI bewegen
    void DoDraw(void);         // eigene draw routine
};

#endif
