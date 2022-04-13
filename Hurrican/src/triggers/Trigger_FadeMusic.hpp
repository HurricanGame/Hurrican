#ifndef _TRIGGER_FADEMUSIC_HPP_
#define _TRIGGER_FADEMUSIC_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFadeMusic : public GegnerClass {
  public:
    GegnerFadeMusic(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
