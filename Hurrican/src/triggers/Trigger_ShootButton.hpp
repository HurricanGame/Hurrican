#ifndef _TRIGGER_SHOOTBUTTON_HPP_
#define _TRIGGER_SHOOTBUTTON_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerShootButton : public GegnerClass {
  private:
    GegnerClass *pPlattForm;  // Zeiger auf die Plattform, die der Button vorantreibt
    float SmokeCount;         // Antiebsflamme

  public:
    GegnerShootButton(GegnerClass *Plattform);  // Konstruktor
    void GegnerExplode(void);                   // Gegner explodiert
    void DoKI(void);                            // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw(void);
};

#endif
