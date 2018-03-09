#ifndef _GEGNER_BIGROCKET_HPP_
#define _GEGNER_BIGROCKET_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerBigRocket : public GegnerClass {
  private:
    float smokedelay;

  public:
    GegnerBigRocket(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw(void);         // Gegner individuell rendern
};

#endif
