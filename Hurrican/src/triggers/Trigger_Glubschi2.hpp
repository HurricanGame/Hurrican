#ifndef _TRIGGER_GLUBSCHI2_HPP_
#define _TRIGGER_GLUBSCHI2_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerGlubschi2 : public GegnerClass {
  private:
    float rotspeed;
    float rot;

  public:
    GegnerGlubschi2(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw(void);
};

#endif
