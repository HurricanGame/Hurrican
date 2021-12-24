#ifndef _BOSS_THEWALL_HPP_
#define _BOSS_THEWALL_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerTheWall : public GegnerClass {
  private:
    float ShotDelay;
    float SmokeDelay;  // Rauchcounter
    float KringelWinkel;
    float HeartBeat;
    int ShotCount;
    int HeartAnim;
    float DoorOffset;
    float DoorCount;
    int CountOpen;
    float OldEnergy;
    float SkullEnergy;
    float VorneX, VorneY;
    float Laughing;
    float dummy;
    float SkullShotDelay;

    void ReturnSkull();

    DirectGraphicsSprite Vorne;
    DirectGraphicsSprite Tuer;
    DirectGraphicsSprite Rahmen;
    DirectGraphicsSprite Kringel[2];
    DirectGraphicsSprite Brain;

  public:
    GegnerTheWall(int Wert1, int Wert2, bool Light);

    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
    void NeueAktion();
};

#endif
