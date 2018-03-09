#ifndef _GEGNER_BALLERDRONE_HPP_
#define _GEGNER_BALLERDRONE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerBallerdrone : public GegnerClass {
  private:
    float shotdelay;

  public:
    GegnerBallerdrone(int Wert1,
                      int Wert2,  // Konstruktor
                      bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw(void);
};

#endif
