#ifndef _GEGNER_KRABBLERLINKS_HPP_
#define _GEGNER_KRABBLERLINKS_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerKrabblerLinks : public GegnerClass {
  private:
    float ShotDelay;  // Schussverzögerung

  public:
    GegnerKrabblerLinks(int Wert1,
                        int Wert2,  // Konstruktor
                        bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
