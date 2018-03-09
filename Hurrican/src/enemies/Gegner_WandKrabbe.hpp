#ifndef _GEGNER_WANDKRABBE_HPP_
#define _GEGNER_WANDKRABBE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerWandKrabbe : public GegnerClass {
  private:
    float rot;
    int lastPlayerAction;

  public:
    GegnerWandKrabbe(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw(void);         // Gegner individuell rendern
};

#endif
