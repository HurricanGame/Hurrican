// Datei : Projectiles.hpp

// --------------------------------------------------------------------------------------
//
// Projektile (Schüsse) für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _PROJECTILES_HPP_
#define _PROJECTILES_HPP_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------
#include "DX8Sprite.hpp"
#include "Player.hpp"

// DKS - For new pooled memory manager and grouped drawlists of projectiles to render:
#include "DataStructures.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

// Schüsse, die mit normalem Alpha gerendert werden

enum {
  SPREADSHOT = 0,      // SpreadShot des Spielers
  SPREADSHOT2 = 1,     // SpreadShot des Spielers
  SPREADSHOTBIG = 2,   // Riesen SpreadShot des Spielers
  SPREADSHOTBIG2 = 3,  // Riesen SpreadShot des Spielers

  BOUNCESHOT1 = 4,       // BounceShot des Spielers Stufe gross
  BOUNCESHOT2 = 5,       // BounceShot des Spielers Stufe mittel
  BOUNCESHOT3 = 6,       // BounceShot des Spielers Stufe klein
  BOUNCESHOT2_LONG = 7,  // BounceShot des Spielers Stufe mittel, hält länger =)
  BOUNCESHOT3_LONG = 8,  // BounceShot des Spielers Stufe mittel, hält länger =)

  BOUNCESHOTBIG1 = 9,   // Riesen BounceShot des Spielers Stufe gross
  BOUNCESHOTBIG2 = 10,  // Riesen BounceShot des Spielers Stufe mittel
  BOUNCESHOTBIG3 = 11,  // Riesen BounceShot des Spielers Stufe klein

  LASERSHOT = 12,      //          Laser des Spielers
  LASERSHOT2 = 13,     //          Laser des Spielers
  LASERSHOTBIG = 14,   // Riesen Laser des Spielers
  LASERSHOTBIG2 = 15,  // Riesen Laser des Spielers

  BOMBE = 16,     // Bombe des Spielers als Rad
  BOMBEBIG = 17,  // Bombe des Spielers als Rad mit Super PowerUp

  WALKER_LASER = 18,    // Laserschuss des Walkers links
  WALKER_LASER2 = 228,  // Laserschuss des Walkers rechts
  SPIDER_LASER = 229,   // Laserschuss des Spinnenbosses

  CANONBALL = 19,    // Kugel des GunTowers links
  CANONBALL2 = 230,  // Kugel des GunTowers rechts

  SUCHSCHUSS = 20,           // Kugel des Deckenturms
  STRAIGHTSCHUSS = 253,      // Schuss direkt nach oben
  STRAIGHTSCHUSS2 = 254,     // Schuss direkt nach unten
  SPITTERBOMBESHOTLO = 255,  // Spitterbombenschuss links oben
  SPITTERBOMBESHOTLM = 256,  // Spitterbombenschuss links mitte
  SPITTERBOMBESHOTLU = 257,  // Spitterbombenschuss links unten
  SPITTERBOMBESHOTRO = 258,  // Spitterbombenschuss rechts oben
  SPITTERBOMBESHOTRM = 259,  // Spitterbombenschuss rechts mitte
  SPITTERBOMBESHOTRU = 260,  // Spitterbombenschuss recht unten

  SHIELDSPAWNER = 261,   // Schield Emitter (zerstört auch Gegner)
  SHIELDSPAWNER2 = 262,  // Schield Emitter (zerstört auch Gegner)

  POWERLINE = 21,    // Powerline des Spielers links
  POWERLINE2 = 231,  // Powerline des Spielers rechts

  KRABBLERLASER1 = 22,  // Nach unten
  KRABBLERLASER2 = 23,  // Nach rechts
  KRABBLERLASER3 = 24,  // Nach links

  GRENADE = 25,    // Granate
  SMARTBOMB = 26,  // SmartBomb

  FEUERFALLE2 = 232,         // Flamme des Wand-Flammenwerfers rechts
  FEUERFALLE3 = 233,         // Flamme des Wand-Flammenwerfers unten
  FEUERFALLE4 = 234,         // Flamme des Wand-Flammenwerfers links
  FEUERFALLE_LAVAMANN = 27,  // Flamme des LavaMannes

  SPIDERFIRE = 235,  // Flamme der Feuer-Spinne (wie Suchschuss)
  WALKERFIRE = 263,  // Flamme des Feuer-Walkers

  ROCKET = 28,            // Normale Rakete des Gegners
  ROCKETSPIDER = 29,      // Rakete des Spinnen Endbosses
  ROCKETWERFER = 30,      // Rakete des Raketenwerfers
  ARCSHOT = 31,           // Schuss der im Bogen fliegt
  TORPEDO = 32,           // Torpedo der SchwimmWalker
  EVILSHOT = 33,          // Schuss des Evil Hurris rechts
  EVILSHOT2 = 34,         // Schuss des Evil Hurris links
  SNOWBOMB = 35,          // SchneeBombe
  SNOWBOMBSMALL = 36,     // SchneeBombe klein
  FETTESPINNESHOT = 37,   // Schuss der fetten Spinne rechts
  FETTESPINNESHOT2 = 38,  // Schuss der fetten Spinne links
  EIERBOMBE = 39,         // Eierbombe, die der Eiermann legt
  BRATKLOPSSHOT = 40,     // Schuss des Bratklops Endbosses
  DRONEBULLET = 41,       // Schuss der Drone rechts
  DRONEBULLET2 = 251,     // Schuss der Drone links
  GOLEMSAEULE = 42,       // Feuersäule des Golems
  SCHLEIMSHOT = 43,       // Suchschuss des Alienmauls
  LAFASSSHOT = 44,        // Das Fass, wenn es runterfällt
  FLUGLASER = 45,
  EISZAPFENSHOT = 46,    // Eiszapfen als Geschoss
  BLUEBOMB = 47,         // Blaue Bombe (= Ex Schneekoppe)
  SKELETORGRANATE = 48,  // Granate vom Skeletor
  DIAMONDSHOT = 49,      // Diamant der auf den Punisher fiegt
  UFOLASER = 50,         // Laser des Ufos

  ARCSHOTLEFT = 236,   // Schuss der im Bogen fliegt nach links
  ARCSHOTRIGHT = 237,  // Schuss der im Bogen fliegt nach rechts
  EVILROUND1 = 238,    // Rundschuss EvilHurri
  EVILROUND2 = 239,    // Rundschuss EvilHurri
  EVILROUND3 = 240,    // Rundschuss EvilHurri
  EVILROUND4 = 241,    // Rundschuss EvilHurri
  EVILROUND5 = 242,    // Rundschuss EvilHurri
  EVILROUND6 = 243,    // Rundschuss EvilHurri
  EVILROUND7 = 244,    // Rundschuss EvilHurri
  EVILROUND8 = 245,    // Rundschuss EvilHurri
  EVILROUND9 = 246,    // Rundschuss EvilHurri
  EVILROUNDA = 247,    // Rundschuss EvilHurri
  EVILROUNDB = 248,    // Rundschuss EvilHurri
  EVILROUNDC = 249,    // Rundschuss EvilHurri
  ROBOROCKET = 250,    // Rakete des RoboMans

// Schüsse, die additiv gerendert werden

  FLAMME = 100,             // Flammen-Splitter des Fasses
  PHARAOLASER = 101,        // Schuss des Pharao Kopfes
  LASERSPIDER = 102,        // Laser  des Spinnen Endbosses
  FLAMEWALL = 103,          // Aufsteigende Feuerwand
  EVILBLITZ = 104,          // Blitz des EvilHurris
  EVILBLITZ2 = 105,         // Blitz des EvilHurris der runterkommt
  BLITZBEAM = 106,          // Blitzbeam des Spielers
  STELZLASER = 108,         // Laser des Stelzsacks
  STELZLASER2 = 109,        // Laser des Stelzsacks
  PFLANZESHOT = 110,        // Schuss der Pflanze Rechts
  SUCHSCHUSS2 = 111,        // Blauer Schuss der Riesenwespe
  GOLEMSCHUSS = 112,        // Schuss des Golems
  SPIDERSHOT = 113,         // Spinnenschuss
  SPIDERSHOTLANGSAM = 114,  // Spinnenschuss
  SPIDERSHOT2 = 115,        // Spinnenschuss
  SPIDERSHOT3 = 116,        // Spinnenschuss
  SPIDERSLOW = 117,         // Langsamer Spinnenschuss
  SPIDERLASER = 118,        // Fetter Spinnenlaser
  FEUERFALLE = 119,         // Flamme des Wand-Flammenwerfers oben
  FIREBALL = 120,           // Feuerball des Minidrachens
  FIREBALL_BIG = 121,       // Feuerball des Minidrachens
  ROTZSHOT = 122,           // Rotzi ballert die Dinger im Bogen
  SKELETOR_SHOT = 123,      // Schuss vom Skeletor
  FIREBALL_BOMB = 124,      // Feuerball des Minidrachens
  PLAYERFIRE = 125,         // Flamme des Spielers :)
  ELEKTROSCHUSS = 126,      // Eletrkoschuss in der Röhre
  ELEKTROPAMPE = 127,       // Elektropampe
  TURRIEXTRAWURST = 128,    // Schuss des Golems
};

constexpr int MAX_SHOTS = 1024;  // Maximale Anzahl an Schüssen

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Schuss Klasse (Werte für EINEN Schuss)
// --------------------------------------------------------------------------------------

class ProjectileClass {
    friend class ProjectileListClass;

  private:
    bool HasGlow;                // Leuchten?
    bool CheckBlock;             // überhaupt Kollision mit Wand checken?

    float xSpeed, ySpeed;        // Geschwindigkeit des Partikels
    float xAcc, yAcc;            // Beschleunigung des Partikels
    int AnimPhase, AnimEnde;     // Aktuelle Phase und Endphase
    float AnimSpeed, AnimCount;  // Anim-Geschwindigkeit und Counter
    bool BounceWalls;            // an Wänden abprallen ?
    bool ExplodeOnImpact;        // Schüsse, die durch Objekte durchgehen
    int Damage;                  // Wieviel Schaden verursacht der Schuss
    bool DamagePlayer;           // Wer bekommt Schaden ? Spieler oder Gegner
    float Winkel;                // nur für SpreadShots
    float Counter;               // Spezialcounter für Extra-Aktionen
    bool OwnDraw;

  public:
    int ShotArt;                 // Art des Schusses (siehe Defines)
    float xPos, yPos;            // Position des Partikels
    float xPosOld, yPosOld;      // alte X-Position

    // DKS - Constructor is now empty, destructor explicitly so.
    //      Duties of constructor have now been moved to CreateShot()
    //      and PushBlitzBeam() functions, as constructor was setting
    //      values redundantly and those functions are the only
    //      two that ultimately create projectiles.
    //      NOTE: The primary reason construction has been eliminated is to support the new
    //      pooled memory manager (see DataStructures.h), which stores an array of
    //      pre-constructed objects from which it assigns new ones.
    ProjectileClass() {}   // Konstruktor
    ~ProjectileClass() {}  // Destruktor

    void CreateShot(float x, float y, int Art, PlayerClass *pTemp);  // Bestimmten Schuss erzeugen
    void Run();                                                  // Schuss animieren und bewegen
    void Render();                                               // Schuss rendern
    void CheckCollision();                                       // Kollision checken
    void ExplodeShot();                                          // Schuss explodiert und erzeugt Partikel
    ProjectileClass *pNext;                                          // Zeiger auf den nächsten   Schuss
    // DKS - Made a singly-linked list, there's no need or benefit for a doubly-linked list here.
    // ProjectileClass		*pPrev;						// Zeiger auf den vorherigen Schuss
    PlayerClass *pParent;
};

// --------------------------------------------------------------------------------------
// Projectile Klasse, Linked List, die alle Schüsse beinhaltet
// --------------------------------------------------------------------------------------

class ProjectileListClass {
  private:
    int NumProjectiles;  // aktuelle Zahl der Schüsse

    // DKS - New, very simple pooled memory manager decreases alloc/dealloc overhead: (see DataStructures.h)
#ifndef USE_NO_MEMPOOLING
    MemPool<ProjectileClass, MAX_SHOTS> projectile_pool;
#endif

  public:
    ProjectileClass *pStart;  // Erstes  Element der Liste
    ProjectileClass *pEnd;    // Letztes Element der Liste

    // DKS - All of these 5 sprites are no longer globals, I moved them here cleaning up big messes
    //      and fixing ambiguous orders of calls to destructors.
    DirectGraphicsSprite LaserSmoke;       // Leuchten des Lasers
    DirectGraphicsSprite LaserSmokeBig;    // Leuchten des Riesen Lasers
    DirectGraphicsSprite SpreadShotSmoke;  // Leuchten des Riesen Spreadshots
    DirectGraphicsSprite PowerlineSmoke;   // Leuchten der Powerline
    // DKS - Moved this here from being a global in a Gegner file (cleaning up big mess)
    DirectGraphicsSprite LavaFlare;  // Leuchten des lava Balles

    // DKS - Moved these five here from being globals in Player.cpp (same reasoning as above notes)
    DirectGraphicsSprite Blitzstrahl[4];  // Grafik   des Blitzes
    DirectGraphicsSprite Blitzflash[4];   // Grafik   des Leuchtens beim Blitz
    DirectGraphicsSprite BlitzTexture;    // Textur auf den Blitzen drauf
    // DKS - SchussFlamme here had 4 elements originally, but only three were used, so reduced its size to 3:
    DirectGraphicsSprite SchussFlamme[3];    // Grafiken für die SchussFlamme, für alle 3 Waffen
    DirectGraphicsSprite SchussFlammeFlare;  // Grafiken für das Leuchten der SchussFlamme

    ProjectileListClass();   // Konstruktor
    ~ProjectileListClass();  // Destruktor

    // DKS - ProjectileListClass is now a static global, instead of dynamically allocated
    //      pointer, so moved the loading of sprites from its constructor to this new
    //      function:
    void LoadSprites();

    bool PushProjectile(float x, float y, int Art, PlayerClass *pTemp = nullptr);  // Schuss "Art" hinzufügen
    bool PushBlitzBeam(int Size,
                       float Richtung,
                       PlayerClass *pSource);  // BlitzBeam hinzufügen (in verschiedenen Größen und Richtungen möglich)

    // DKS - Converted projectile linked-list to be singly-linked:
    // DelNode() is new and takes the place of DelSel(), but operates a bit differently.
    // It is now up to the caller to splice the list: DelNode() blindly deletes the node
    // passed to it and returns the pointer that was in pPtr->pNext, or NULL if pPtr was NULL.
    // void DelSel		(ProjectileClass *pTemp);		// Ausgewähltes Objekt entfernen
    ProjectileClass *DelNode(ProjectileClass *pPtr);

    void ClearAll();          // Alle Objekte löschen
    void ClearType(int type);     // Alle Objekte eines Typs löschen
    int GetNumProjectiles() const; // Zahl der Schüsse zurückliefern
    void DoProjectiles();     // Alle Schüsse der Liste animieren
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern ProjectileListClass Projectiles;
extern int CurrentShotTexture;
extern float WinkelUebergabe;

#endif
