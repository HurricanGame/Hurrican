#ifndef _TRIGGER_SHRINE_HPP_
#define _TRIGGER_SHRINE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerShrine : public GegnerClass {
  public:
    GegnerShrine(int Wert1,
                 int Wert2,  // Konstruktor
                 bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
