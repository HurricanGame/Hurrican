// Datei : Gegner.hpp

// --------------------------------------------------------------------------------------
//
// Gegner für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _GEGNERCLASS_HPP_
#define _GEGNERCLASS_HPP_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_SDL)
#include "SDL_port.hpp"
#else
#include <d3dx8.h>
#endif
#include "DX8Sprite.hpp"

#include <list>
#include <memory>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

enum class DirectionEnum {
    RECHTS = 1,
    LINKS  = -1
};

namespace Direction {

constexpr auto asInt(DirectionEnum dir) noexcept {
    return static_cast<typename std::underlying_type<DirectionEnum>::type>(dir);
}

constexpr auto invert(DirectionEnum dir) noexcept {
    return static_cast<DirectionEnum>(static_cast<typename std::underlying_type<DirectionEnum>::type>(dir) * -1);
}

}

class PlayerClass;

// --------------------------------------------------------------------------------------
// Gegner Klasse (Werte für EINEN Gegner)
// --------------------------------------------------------------------------------------

// DKS - TODO: Reduce size of member vars where appropriate
class GegnerClass {
  protected:            // protected wegen Ableitung
    int AnimStart;      // Erste Animationsphase
    DirectionEnum BlickRichtung;  // Links oder Rechts :)
    float TurnCount;
    bool ChangeLight;   // Licht des Levels annehmen ?

  public:
    PlayerClass *pAim;                        // Player to attack
    int HitSound;                             // Metall = 0, Organisch = 1
    int LastAction;                           // Letzter Angriff (damit Bosse nicht immer das Gleiche machen)
    int AnimPhase, AnimEnde;                  // Aktuelle Phase und Endphase
    int Handlung;                             // Aktuelle "Handlung" des Gegners
    int GegnerArt;                            // Art des Gegners (siehe Defines)
    int Value1, Value2;                       // Trigger-Werte
    uint32_t blocku, blocko, blockl, blockr;  // Block links rechts über und unter dem Gegner
    float AnimSpeed, AnimCount;               // Anim-Geschwindigkeit und Counter
    float xPos, yPos;                         // Position des Gegners
    float xPosOld, yPosOld;                   // Position des Gegners
    float xSpeed, ySpeed;                     // Geschwindigkeit des Gegners
    float xAcc, yAcc;                         // Beschleunigung des Gegners
    float DamageTaken;                        // Wie lange rot nach Treffer
    float Energy;                             // Energie des Gegners
    float TimeToChange;                       // pAim wechseln :)
    bool AlreadyDrawn;
    bool Active;                              // Gegner überhaupt aktiv ?
    bool Destroyable;                         // Ist er kaputtbar ? :)
    bool TestBlock;                           // Auf Block mit der Wand testen ?
    bool OwnDraw;                             // Hat eigene Draw-Routine in der KI Funktion?
    bool DontMove;                            // Muss der Gegner bewegt werden?
    bool ForceLight;                          // Gegner wird auf jeden Fall von Level "beleuchtet"?
    bool BackGround;                          // Gegner zuerst rendern, da zu Background-Grafik gehört? (z.B. Lüfter)

    GegnerClass();                                           // Konstruktor
    virtual ~GegnerClass();                                  // Destruktor
    virtual void GegnerExplode();                            // Gegner explodiert
    virtual void DoKI() {}                                   // Gegner individuell bewegen
    virtual void DoDraw() {}                                 // Gegner individuell rendern (nur bei manchen)
    bool Run();                                              // Gegner bewegen, checken etc
    void Render();                                           // nur rendern
    int PlayerAbstand(bool both = false) const;                     // Abstand Gegner/Spieler zurückliefern
    int PlayerAbstandHoriz(PlayerClass *pTarget = nullptr) const;   // Nur Horizontaler Abstand
    int PlayerAbstandVert(PlayerClass *pTarget = nullptr) const;    // Nur Vertikaler   Abstand
    void TestDamagePlayers(float dam, bool detroy = false);  // Spieler verletzt?
    void PlattformTest(RECT_struct rect);                           // Steht der Spieler auf dem Gegner ?
    void Wegschieben(RECT_struct rect, float dam);                  // Gegner schiebt Spieler beiseite
    void SimpleAnimation(bool backward = false);             // Einfache Animation (loop)
    void TurnonWall();                                       // An der Wand umdrehen?
    bool TurnonShot();                                       // Umdrehen, wenn angeschoßen
    bool IsOnScreen() const;                                 // Gegner grade sichtbar?
};

// --------------------------------------------------------------------------------------
// Gegner Klasse, Linked List, die alle Gegner beinhaltet
// --------------------------------------------------------------------------------------

class GegnerListClass {
  public:
    // DKS - Moved these three here, they used to be dynamic globals in Gegner_Helper.cpp:
    //      (Moved so they would always get destructed in a place we had control over)
    DirectGraphicsSprite DroneFlame;  // Flamme der Drone
    DirectGraphicsSprite DroneGun;    // Flamme der Zitrone

    std::list<std::unique_ptr<GegnerClass>> enemies;

    // DKS - GegnerListClass is now a static global, instead of dynamically allocated
    //      pointer, so moved the loading of sprites from its constructor to this new
    //      function:
    void LoadSprites();

    GegnerClass* PushGegner(
                    float x,
                    float y,
                    int Art,  // Gegner "Art" hinzufügen
                    int Value1,
                    int Value2,
                    bool Light,
                    bool atEnd = true);
    void ClearAll();                  // Alle Gegner löschen
    int GetNumGegner() const;         // Zahl der Gegner zurückliefern
    void RunAll();                    // Alle Gegner der Liste animieren
    void RenderAll();                 // Alle Gegner der Liste rendern
    void DamageEnemiesonScreen(float x,
                               float y,  // Alle Gegner ausgehend von Stelle x/y verwunden (Granate)
                               int MaxDamage);
};

#endif
