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

#define SPREADSHOT 0      // SpreadShot des Spielers
#define SPREADSHOT2 1     // SpreadShot des Spielers
#define SPREADSHOTBIG 2   // Riesen SpreadShot des Spielers
#define SPREADSHOTBIG2 3  // Riesen SpreadShot des Spielers

#define BOUNCESHOT1 4       // BounceShot des Spielers Stufe gross
#define BOUNCESHOT2 5       // BounceShot des Spielers Stufe mittel
#define BOUNCESHOT3 6       // BounceShot des Spielers Stufe klein
#define BOUNCESHOT2_LONG 7  // BounceShot des Spielers Stufe mittel, hält länger =)
#define BOUNCESHOT3_LONG 8  // BounceShot des Spielers Stufe mittel, hält länger =)

#define BOUNCESHOTBIG1 9   // Riesen BounceShot des Spielers Stufe gross
#define BOUNCESHOTBIG2 10  // Riesen BounceShot des Spielers Stufe mittel
#define BOUNCESHOTBIG3 11  // Riesen BounceShot des Spielers Stufe klein

#define LASERSHOT 12      //	      Laser des Spielers
#define LASERSHOT2 13     //	      Laser des Spielers
#define LASERSHOTBIG 14   // Riesen Laser des Spielers
#define LASERSHOTBIG2 15  // Riesen Laser des Spielers

#define BOMBE 16     // Bombe des Spielers als Rad
#define BOMBEBIG 17  // Bombe des Spielers als Rad mit Super PowerUp

#define WALKER_LASER 18    // Laserschuss des Walkers links
#define WALKER_LASER2 228  // Laserschuss des Walkers rechts
#define SPIDER_LASER 229   // Laserschuss des Spinnenbosses

#define CANONBALL 19    // Kugel des GunTowers links
#define CANONBALL2 230  // Kugel des GunTowers rechts

#define SUCHSCHUSS 20           // Kugel des Deckenturms
#define STRAIGHTSCHUSS 253      // Schuss direkt nach oben
#define STRAIGHTSCHUSS2 254     // Schuss direkt nach unten
#define SPITTERBOMBESHOTLO 255  // Spitterbombenschuss links oben
#define SPITTERBOMBESHOTLM 256  // Spitterbombenschuss links mitte
#define SPITTERBOMBESHOTLU 257  // Spitterbombenschuss links unten
#define SPITTERBOMBESHOTRO 258  // Spitterbombenschuss rechts oben
#define SPITTERBOMBESHOTRM 259  // Spitterbombenschuss rechts mitte
#define SPITTERBOMBESHOTRU 260  // Spitterbombenschuss recht unten

#define SHIELDSPAWNER 261   // Schield Emitter (zerstört auch Gegner)
#define SHIELDSPAWNER2 262  // Schield Emitter (zerstört auch Gegner)

#define POWERLINE 21    // Powerline des Spielers links
#define POWERLINE2 231  // Powerline des Spielers rechts

#define KRABBLERLASER1 22  // Nach unten
#define KRABBLERLASER2 23  // Nach rechts
#define KRABBLERLASER3 24  // Nach links

#define GRENADE 25    // Granate
#define SMARTBOMB 26  // SmartBomb

#define FEUERFALLE2 232         // Flamme des Wand-Flammenwerfers rechts
#define FEUERFALLE3 233         // Flamme des Wand-Flammenwerfers unten
#define FEUERFALLE4 234         // Flamme des Wand-Flammenwerfers links
#define FEUERFALLE_LAVAMANN 27  // Flamme des LavaMannes

#define SPIDERFIRE 235  // Flamme der Feuer-Spinne (wie Suchschuss)
#define WALKERFIRE 263  // Flamme des Feuer-Walkers

#define ROCKET 28            // Normale Rakete des Gegners
#define ROCKETSPIDER 29      // Rakete des Spinnen Endbosses
#define ROCKETWERFER 30      // Rakete des Raketenwerfers
#define ARCSHOT 31           // Schuss der im Bogen fliegt
#define TORPEDO 32           // Torpedo der SchwimmWalker
#define EVILSHOT 33          // Schuss des Evil Hurris rechts
#define EVILSHOT2 34         // Schuss des Evil Hurris links
#define SNOWBOMB 35          // SchneeBombe
#define SNOWBOMBSMALL 36     // SchneeBombe klein
#define FETTESPINNESHOT 37   // Schuss der fetten Spinne rechts
#define FETTESPINNESHOT2 38  // Schuss der fetten Spinne links
#define EIERBOMBE 39         // Eierbombe, die der Eiermann legt
#define BRATKLOPSSHOT 40     // Schuss des Bratklops Endbosses
#define DRONEBULLET 41       // Schuss der Drone rechts
#define DRONEBULLET2 251     // Schuss der Drone links
#define GOLEMSAEULE 42       // Feuersäule des Golems
#define SCHLEIMSHOT 43       // Suchschuss des Alienmauls
#define LAFASSSHOT 44        // Das Fass, wenn es runterfällt
#define FLUGLASER 45
#define EISZAPFENSHOT 46    // Eiszapfen als Geschoss
#define BLUEBOMB 47         // Blaue Bombe (= Ex Schneekoppe)
#define SKELETORGRANATE 48  // Granate vom Skeletor
#define DIAMONDSHOT 49      // Diamant der auf den Punisher fiegt
#define UFOLASER 50         // Laser des Ufos

#define ARCSHOTLEFT 236   // Schuss der im Bogen fliegt nach links
#define ARCSHOTRIGHT 237  // Schuss der im Bogen fliegt nach rechts
#define EVILROUND1 238    // Rundschuss EvilHurri
#define EVILROUND2 239    // Rundschuss EvilHurri
#define EVILROUND3 240    // Rundschuss EvilHurri
#define EVILROUND4 241    // Rundschuss EvilHurri
#define EVILROUND5 242    // Rundschuss EvilHurri
#define EVILROUND6 243    // Rundschuss EvilHurri
#define EVILROUND7 244    // Rundschuss EvilHurri
#define EVILROUND8 245    // Rundschuss EvilHurri
#define EVILROUND9 246    // Rundschuss EvilHurri
#define EVILROUNDA 247    // Rundschuss EvilHurri
#define EVILROUNDB 248    // Rundschuss EvilHurri
#define EVILROUNDC 249    // Rundschuss EvilHurri
#define ROBOROCKET 250    // Rakete des RoboMans

// Schüsse, die additiv gerendert werden

#define FLAMME 100             // Flammen-Splitter des Fasses
#define PHARAOLASER 101        // Schuss des Pharao Kopfes
#define LASERSPIDER 102        // Laser  des Spinnen Endbosses
#define FLAMEWALL 103          // Aufsteigende Feuerwand
#define EVILBLITZ 104          // Blitz des EvilHurris
#define EVILBLITZ2 105         // Blitz des EvilHurris der runterkommt
#define BLITZBEAM 106          // Blitzbeam des Spielers
#define STELZLASER 108         // Laser des Stelzsacks
#define STELZLASER2 109        // Laser des Stelzsacks
#define PFLANZESHOT 110        // Schuss der Pflanze Rechts
#define SUCHSCHUSS2 111        // Blauer Schuss der Riesenwespe
#define GOLEMSCHUSS 112        // Schuss des Golems
#define SPIDERSHOT 113         // Spinnenschuss
#define SPIDERSHOTLANGSAM 114  // Spinnenschuss
#define SPIDERSHOT2 115        // Spinnenschuss
#define SPIDERSHOT3 116        // Spinnenschuss
#define SPIDERSLOW 117         // Langsamer Spinnenschuss
#define SPIDERLASER 118        // Fetter Spinnenlaser
#define FEUERFALLE 119         // Flamme des Wand-Flammenwerfers oben
#define FIREBALL 120           // Feuerball des Minidrachens
#define FIREBALL_BIG 121       // Feuerball des Minidrachens
#define ROTZSHOT 122           // Rotzi ballert die Dinger im Bogen
#define SKELETOR_SHOT 123      // Schuss vom Skeletor
#define FIREBALL_BOMB 124      // Feuerball des Minidrachens
#define PLAYERFIRE 125         // Flamme des Spielers :)
#define ELEKTROSCHUSS 126      // Eletrkoschuss in der Röhre
#define ELEKTROPAMPE 127       // Elektropampe
#define TURRIEXTRAWURST 128    // Schuss des Golems

#define MAX_SHOTS 1024  // Maximale Anzahl an Schüssen

// DKS - This was unnecessarily high. Changed to the correct figure:
//#define MAX_SHOTGFX				300						// Maximale Anzahl verschiedener Grafiken
#define MAX_SHOTGFX 264  // Maximale Anzahl verschiedener Grafiken

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Schuss Klasse (Werte für EINEN Schuss)
// --------------------------------------------------------------------------------------

class ProjectileClass {
  public:
    float xSpeed, ySpeed;        // Geschwindigkeit des Partikels
    float xAcc, yAcc;            // Beschleunigung des Partikels
    int AnimPhase, AnimEnde;     // Aktuelle Phase und Endphase
    float AnimSpeed, AnimCount;  // Anim-Geschwindigkeit und Counter
    bool BounceWalls;            // an Wänden abprallen ?
    bool CheckBlock;             // überhaupt Kollision mit Wand checken?
    bool HasGlow;                // Leuchten?
    bool ExplodeOnImpact;        // Schüsse, die durch Objekte durchgehen
    int ShotArt;                 // Art des Schusses (siehe Defines)
    float xPos, yPos;            // Position des Partikels
    float xPosOld, yPosOld;      // alte X-Position
    int Damage;                  // Wieviel Schaden verursacht der Schuss
    bool DamagePlayer;           // Wer bekommt Schaden ? Spieler oder Gegner
    float Winkel;                // nur für SpreadShots
    float Counter;               // Spezialcounter für Extra-Aktionen
    bool OwnDraw;

    // DKS - Constructor is now empty, destructor explicitlu so.
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
    int GetNumProjectiles();  // Zahl der Schüsse zurückliefern
    void DoProjectiles();     // Alle Schüsse der Liste animieren
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern ProjectileListClass Projectiles;
extern int CurrentShotTexture;
extern float WinkelUebergabe;

#endif
