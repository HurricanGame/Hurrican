#ifndef _GEGNER_SCHABE_HPP_
#define _GEGNER_SCHABE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchabe : public GegnerClass {
  private:
    float winkel;
    float ShotDelay;
    bool mirror;

  public:
    GegnerSchabe(int Wert1, int Wert2, bool Light);  // Konstruktor
    void GegnerExplode(void);                        // Gegner explodiert
    void DoKI(void);                                 // Gegner individuell mit seiner
    void DoDraw(void);                               // Gegner individuell rendern
    // eigenen kleinen KI bewegen
};

#endif
