#ifndef _GEGNER_EISZAPFEN_HPP_
#define _GEGNER_EISZAPFEN_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerEiszapfen : public GegnerClass {
  public:
    GegnerEiszapfen(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
