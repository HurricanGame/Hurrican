#ifndef _BOSS_SPINNENMASCHINE_HPP_
#define _BOSS_SPINNENMASCHINE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

constexpr float TIME_TILL_OPEN = 10.0f;
constexpr float TIME_TILL_CLOSE = 40.0f;

constexpr float TIME_TILL_HOCH = 30.0f;

enum class DeckelStateEnum {
  ZU,
  OFFEN,
  OEFFNEN,
  SCHLIESSEN
};

class GegnerSpinnenmaschine : public GegnerClass {
  private:
    int DisplayState;     // Was ist auf dem Monitor zu sehen?
    int OldDisplayState;  // Was ist auf dem Monitor zu sehen?
    int DeckelPhase;      // AnimPhase des Deckels
    float DeckelCount;    // Counter für Sinusbewegung des Hochgehenden Deckels
    float DeckelOffset;   // Tatsächlicher Offset (=sin(DeckelCount) * X)
    DeckelStateEnum DeckelStatus;     // Aktueller Status (siehe #defines)
    float SizeCount;
    float OpenCounter;  // Cunter wann der Deckel aufgeht
    float HochCounter;  // Conter wann der Kopf hochfährt
    DeckelStateEnum HochStatus;     // Status des Kopfs (siehe #defines)
    float ShotDelay;

    float SpawnDelay;  // Delay zum Gegner ausspucken

    float SmokeDelay;   // Rauchcounter
    float SmokeDelay2;  // Rauchcounter für Damage-Rauch

    float LightRayCount;  // Counter, wie breit der Lichtkegel ist

    float DeathCount;

    int AnimUnten;  // Aktuelle Animphase für das Unterteil

    bool AktionFertig;  // Aktion fertig (Deckel öffnen, hoch/runter fahren)

    DirectGraphicsSprite Deckel;
    DirectGraphicsSprite Unten[2];
    DirectGraphicsSprite Display;
    DirectGraphicsSprite Strahl;

  public:
    GegnerSpinnenmaschine(int Wert1,
                          int Wert2,  // Konstruktor
                          bool Light);
    void DoDeckel();
    void DoHoch();
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
