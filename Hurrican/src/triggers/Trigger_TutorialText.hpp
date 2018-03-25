#ifndef _TRIGGER_TUTORIALTEXT_HPP_
#define _TRIGGER_TUTORIALTEXT_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerTutorialText : public GegnerClass {
  public:
    GegnerTutorialText(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
