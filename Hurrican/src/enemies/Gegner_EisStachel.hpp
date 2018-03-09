#ifndef _GEGNER_EISSTACHEL_HPP_
#define _GEGNER_EISSTACHEL_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerEisStachel : public GegnerClass {
  public:
    GegnerEisStachel(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
