#ifndef _GEGNER_SCHNEEKOPPE_HPP_
#define _GEGNER_SCHNEEKOPPE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchneekoppe : public GegnerClass {
  private:
    bool isHit;

  public:
    GegnerSchneekoppe(int Wert1,
                      int Wert2,  // Konstruktor
                      bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw(void);         // eigene Draw Funktion
};

#endif
