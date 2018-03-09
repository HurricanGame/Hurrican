#ifndef _GEGNER_SCHLEIMBOLLER_HPP_
#define _GEGNER_SCHLEIMBOLLER_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchleimBoller : public GegnerClass {
  private:
    int Size;

  public:
    GegnerSchleimBoller(int Wert1,
                        int Wert2,  // Konstruktor
                        bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw(void);         // Gegner individuell rendern
};

#endif
