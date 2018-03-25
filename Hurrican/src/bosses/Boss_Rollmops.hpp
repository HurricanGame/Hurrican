#ifndef _BOSS_ROLLMOPS_HPP_
#define _BOSS_ROLLMOPS_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Kettenglied.hpp"
#include "enemies/Gegner_Stuff.hpp"

#define NUM_KETTENGLIEDER 20

class GegnerRollmops : public GegnerClass {
  private:
    DirectGraphicsSprite Rollen;
    DirectGraphicsSprite Aufklappen;
    DirectGraphicsSprite Gun;

    GegnerKettenglied *pKettenTeile[NUM_KETTENGLIEDER];

    float GunWinkel;
    float SmokeCount;  // Für Antriebspartikel
    float ShotDelay;
    float ShotCount;
    float HookX, HookY;
    float Schwung, SchwungDir;
    float Delay;
    float HitsToTake;

    void NeueAktion();
    void GunAusfahren(bool Auf);
    void Losrollen();
    void Abhopsen(float mul = -0.6f);
    void CalcGunWinkel();
    void RoundShot(bool single);

  public:
    GegnerRollmops(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw(
        void);  // Gegner individuell rendern														// eigenen kleinen KI
                // bewegen
};

#endif
