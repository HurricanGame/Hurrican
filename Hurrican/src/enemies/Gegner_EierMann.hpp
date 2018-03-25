#ifndef _GEGNER_EIERMANN_HPP_
#define _GEGNER_EIERMANN_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerEierMann : public GegnerClass {
  public:
    GegnerEierMann(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
