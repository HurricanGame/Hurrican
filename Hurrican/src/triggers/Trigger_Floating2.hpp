#ifndef _TRIGGER_FLOATING2_HPP_
#define _TRIGGER_FLOATING2_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFloating2 : public GegnerClass {
  private:
    bool playeron;

  public:
    GegnerFloating2(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
