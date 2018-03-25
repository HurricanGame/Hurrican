#ifndef _GEGNER_KRABBLERLINKS_HPP_
#define _GEGNER_KRABBLERLINKS_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerKrabblerLinks : public GegnerClass {
  public:
    float ShotDelay;  // Schussverzögerung

    GegnerKrabblerLinks(int Wert1,
                        int Wert2,  // Konstruktor
                        bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
