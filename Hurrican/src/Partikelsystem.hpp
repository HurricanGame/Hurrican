// Datei : Partikelsystem.hpp

// --------------------------------------------------------------------------------------
//
// Partikelsystem für Hurrican
// für Funken, Rauchwolken, Patronenhülsen usw.
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _PARTIKELSYSTEM_HPP_
#define _PARTIKELSYSTEM_HPP_

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "DX8Sprite.hpp"
#include "Player.hpp"

// DKS - For new pooled memory manager and grouped drawlists of particles to render:
#include "DataStructures.hpp"

#include <algorithm>

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Partikel, die normal oder nur mit Alphablending gerendert werden

enum {
  BULLET = 0,                  // Patronenhülse
  EXPLOSION_MEDIUM = 2,        // Mittelgrosse Explosion
  EXPLOSION_MEDIUM2 = 3,       // Mittelgrosse Explosion Nr 2
  EXPLOSION_MEDIUM3 = 4,       // Mittelgrosse Explosion Nr 3
  EXPLOSION_GREEN = 5,         // Grüne mittelgroße Explosion
  EXPLOSION_BIG = 6,           // Grosse Explosion
  EXPLOSION_GIANT = 10,        // Riesen Explosion
  EXPLOSION_ALIEN = 11,        // Riesen Explosion
  BLUE_EXPLOSION = 12,         // Blaue kleine Explosion
  SPLITTER = 13,               // Kleiner animierter Splitter
  PIRANHATEILE = 14,           // Teile eines kaputten Piranhas
  PIRANHATEILE2 = 15,          // Teile eines kaputten Riesen Piranhas
  PIRANHABLUT = 16,            // Blut  eines kaputten Piranhas
  ROCKSPLITTER = 17,           // Brocken eines kaputten Felsblocks
  ROCKSPLITTERSMALL = 18,      // kleine Brocken eines kaputten Felsblocks ;)
  ROCKSPLITTERSMALLBLUE = 19,  // kleine Brocken eines kaputten Stalagtits
  SPIDERSPLITTER = 20,         // kleine Brocken einer kaputten Spinne :P
  SPIDERSPLITTER2 = 21,        // kleine Brocken einer kaputten Spinne :P
  SPIDERGRENADE = 22,          // Spinnen Granate (kein SCHUSS, da er Spieler nicht treffen kann)
  EVILSMOKE = 23,              // Schatten des EvilHurri links
  EVILSMOKE2 = 24,             // Schatten des EvilHurri rechts
  STELZE = 25,                 // Bein eines StelzSacks
  STELZHEAD = 26,              // Kopf eines StelzSacks
  SMOKE2 = 27,                 // Rauch2
  EXPLOSION_GIGA = 28,         // Riesen Explosion

  SMOKE3 = 29,     // wie Rauch2, nur steigt er nach oben
  SMOKE3_RO = 30,  // Rauch3 nach rechts oben
  SMOKE3_R = 31,   // Rauch3 nach rechts
  SMOKE3_RU = 32,  // Rauch3 nach rechts unten
  SMOKE3_U = 33,   // Rauch3 nach unten
  SMOKE3_LU = 34,  // Rauch3 nach links unten
  SMOKE3_L = 35,   // Rauch3 nach links
  SMOKE3_LO = 36,  // Rauch3 nach links oben

  SMOKEBIG = 37,         // Riesen Rauch
  SMOKEBIG2 = 38,        // Riesen Rauch
  SMOKEBIG_OUTTRO = 99,  // Riesen Rauch

  MADEBLUT = 39,        // Blut der Made
  SPAWNDROP = 40,       // Wassertropfen der vom Partikelspawner augespuckt wird
  BLATT = 41,           // Umherwirbelndes Blatt aus der Pflanze
  BLATT2 = 42,          // Umherwirbelndes Blatt aus dem Partikelspawner
  NESTLUFT = 43,        // Umherwirbelndes Wespennest nach Abschuss =)
  FLOATSMOKE = 44,      // Rauch der kleinen Floating Plattform
  THUNDERBOLT = 45,     // Blitz + Donner (Screen flackert kurz auf)
  REGENTROPFEN = 46,    // Regentropfen
  FOG = 47,             // Nebel Partikel
  HALSWIRBEL = 48,      // Halswirbel des MetalHead Bosses
  KAPUTTETURBINE = 49,  // Kaputte Turbine des MetalHead Bosses
  SCHNEEFLOCKE = 50,    // Eine Schneeflocke
  // DKS - NOTE: the game originally had only one type of particle art for when the player
  //            blue up, which is HURRITEILE. I added a HURRITEILE_P2 set of particles further
  //            below colored blue for when player 2 blows up.
  HURRITEILE = 51,     // Teile vom Hurri, wenn er explodiert ist =)
  BOULDER_SMALL = 52,  // Kleiner blauer Boulder

  WASSER_SPRITZER2 = 53,  // Wasserspritzer, wenn der Spieler ins Wasser hopst
  WASSER_SPRITZER = 54,   // Wasserspritzer, wenn der Spieler aus dem Wasser hopst
  LAVA_SPRITZER = 55,     // Lavaspritzer beim reinhopsen
  LAVA_SPRITZER2 = 56,    // Lavaspritzer beim raushopsen

  LAVAKRABBE_KOPF = 57,  // Kopf der Lavakrabbe
  LAVAKRABBE_BEIN = 58,  // Bein der Lavakrabbe

  SPIDERPARTS = 59,   // Spinnenstücke auf dem Fliessband
  KETTENTEILE = 60,   // Kettenteile (Arm links)
  KETTENTEILE2 = 61,  // Kettenteile (Arm rechts)
  KETTENTEILE3 = 62,  // Kettenteile (Arm Mitte)
  KETTENTEILE4 = 63,  // Kettenteile (Kettenglied)

  SMOKE = 64,            // Rauch
  DUST = 65,             // Staub
  SCHROTT1 = 66,         // Schrotteil1
  SCHROTT2 = 67,         // Schrotteil1
  BULLET_SKELETOR = 68,  // Patronenhülse vom letzten Endboss
  GLASSPLITTER = 69,     // Glassplitter (von der Tube)

  SCHNEEFLOCKE_END = 70,  // Eine Schneeflocke fürs Outtro

  // Teile vom explodierten Hurrican :)

  HURRITEILE_ARM1 = 80,
  HURRITEILE_ARM2 = 81,
  HURRITEILE_BEIN1 = 82,
  HURRITEILE_BEIN2 = 83,
  HURRITEILE_KOPF = 84,
  HURRITEILE_WAFFE = 85,
  HURRITEILE_TORSO = 86,

  // DKS - Player = 2, sprite is blue, so I added separate particles and particle art for them
  //      that are colored blue, using some unused space between particles 86-100
  HURRITEILE_P2 = 87,  // Teile vom Hurri, wenn er explodiert ist =)
  HURRITEILE_P2_ARM1 = 88,
  HURRITEILE_P2_ARM2 = 89,
  HURRITEILE_P2_BEIN1 = 90,
  HURRITEILE_P2_BEIN2 = 91,
  HURRITEILE_P2_KOPF = 92,
  HURRITEILE_P2_WAFFE = 93,
  HURRITEILE_P2_TORSO = 94,

  //----- Partikel, die mit additivem Alphablending gerendert werden

  ADDITIV_GRENZE = 100,
  FUNKE = 100,   // Roter Funke
  FUNKE2 = 101,  // Grüner Funke
  LASERFUNKE = 103,
  LASERFUNKE2 = 104,
  WASSERTROPFEN = 106,          // Spritzer beim ins Wasser eintreten
  WASSERTROPFEN2 = 107,         // Spritzer beim in die Säure eintreten
  BUBBLE = 108,                 // Luftbläschen unter Wasser
  LASERFLAME = 111,             // Leuchteffekt für den Krabblerlaser
  LASERFLAMEPHARAO = 112,       // Leuchteffekt für die Pharaokopf Augen
  PHARAOSMOKE = 113,            // Leuchteffekt für den Pharaokopf Laser
  SHIELD = 114,                 // Schutzschild-Stern
  ROCKETSMOKE = 115,            // Rauch einer Rakete
  ROCKETSMOKEBLUE = 116,        // Blauer Rauch
  ROCKETSMOKEGREEN = 117,       // Grüner Rauch
  FLUGSACKSMOKE = 118,          // Rauch des Flug Sacks links
  FLUGSACKSMOKE2 = 119,         // Rauch des Flug Sacks rechts
  EVILFUNKE = 120,              // Funke des Evil Blitzes
  EVILROUNDSMOKE = 121,         // Rauch des Rundumschusses des Evil Hurri
  BEAMSMOKE = 122,              // Rauch des Blitzbeams
  BEAMSMOKE2 = 123,             // Rauch beim Aufladen des Blitzbeams
  BEAMSMOKE3 = 124,             // Rauch beim Explodieren des Blitzbeams
  BEAMSMOKE4 = 125,             // Rauch beim Explodieren des Blitzbeams
  BEAMSMOKE5 = 126,             // Kringel beim Explodieren des Blitzbeams
  SNOWFLUSH = 127,              // Schneegestöber
  WATERFLUSH = 128,             // Wasserfall Rauch
  WATERFLUSH2 = 129,            // Wasserfall Rauch
  WATERFLUSH_HIGH = 130,        // Wasserfall Rauch
  WATERFLUSH_HIGH2 = 131,       // Wasserspritzer Rauch
  UFOLASERFLARE = 132,          // Flare des Ufo Lasers
  ROBOMANSMOKE = 133,           // Rauch des Flug Sacks rechts
  STELZFLARE = 134,             // Flare des Stelzsack Lasers
  LONGFUNKE = 135,              // Langer Funke, der eine Spur zieht (Linie)
  WATERFUNKE = 136,             // Wasserspritzer, der eine Spur zieht
  KRINGEL = 137,                // Die Kringelpartikel, die eine neue Waffenstufe angeben
  KRINGELR = 138,               // rot
  KRINGELB = 139,               // blau
  KRINGELG = 140,               // grün
  KRINGELHB = 141,              // hellblau
  TURBINESMOKE = 142,           // Partikel, die in die Turbine des Metalhead Bosses gesaugt werden
  MINIFLARE = 143,              // Flare vom Lava Ball, der runtersegelt
  GRENADEFLARE = 144,           // Aufleuchten bei Granaten-Treffer
  EXPLOSIONFLARE = 145,         // Aufleuchten bei Granaten-Treffer
  EXPLOSIONFLARE2 = 146,        // Aufleuchten bei Lilaschuss-Treffer
  SCHLEIM = 147,                // Schleimtropfen
  SCHLEIM2 = 148,               // Schleim aus SchleimAlien mit Maul
  SHOCKEXPLOSION = 149,         // SchockWelle bei Spieler Explosion
  SHOTFLARE = 150,              // Leuchten bei Schuss-Explosion gelb
  SHOTFLARE2 = 151,             // Leuchten bei Schuss-Explosion grün
  EXTRACOLLECTED = 152,         // Leuchten beim Einsammeln eines Extras
  LASERFLARE = 153,             // Flare beim Auftreffen des Lasers
  EXPLOSION_MEDIUM2_ADD = 154,  // Die coolste Explosion
  EXPLOSION_MEDIUM3_ADD = 155,  // Die coolste Explosion
  EXPLOSION_TRACE = 156,        // Eine Bahn von Explosionen ziehen
  EXPLOSION_REGULAR = 157,      // Kleinere Explosion in Gelb, die immer gleich abläuft
  DIAMANTCOLLECTED = 158,       // Leuchten bei Diamant eingesammelt
  LILA = 159,                   // Lila leuchten
  EXPLOSION_KRINGEL = 160,      // Kringel/Druckwelle bei Explosion
  DRACHE_SMOKE = 161,           // Antrieb des Drachen
  FIREBALL_SMOKE = 162,         // Rauch des Feuerballs des Minidrachens
  EXPLOSION_TRACE_END = 163,    // In der Größe veränderbar
  LAVADUST = 164,               // Lavafunken
  KRINGELSECRET = 165,          // Die Kringelpartikel beim Secret
  TEXTSECRET = 166,             // Text "Secret"

  // Partikel ohne Grafik
  LILA2 = 167,  // Lila leuchten fällt runter
  LILA3 = 168,  // Lila leuchten fliegt links

  MAX_PARTIKELGFX = 169,  // Maximale Anzahl verschiedener Grafiken    
};

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Partikel Klasse (Werte für EIN Partikel)
// --------------------------------------------------------------------------------------

class PartikelClass {
  private:
    // DKS - Changed from int to uint8_t:
    uint8_t red, green, blue, alpha;  // Farbwerte des Partikels

    // DKS - Never used, disabled:
    // D3DCOLOR			Color;						// Farbe des Partikels

    float xSpeed, ySpeed;  // Geschwindigkeit des Partikels
    float xAcc, yAcc;      // Beschleunigung des Partikels

    // DKS - Changed from int to int16_t:
    uint16_t AnimPhase, AnimEnde;  // Aktuelle Phase und Endphase

    float AnimSpeed, AnimCount;  // Anim-Geschwindigkeit und Counter
    bool Rotate;                 // evtl rotieren?
    float Rot;                   // Rotation
    float RotDir;                // Richtung
    bool BounceWalls;            // an Wänden abprallen ?
    bool OwnDraw;                // Extrawurscht beim rendern?
    bool RemoveWhenOffScreen;    // verschiwnden lassen, wenn ausserhalb des Screeens?

  public:
    float xPos, yPos;        // Position des Partikels
    float xPosOld, yPosOld;  // Alte Position

    // DKS - Changed from int to int16_t:
    int16_t PartikelArt;  // Art des Partikels (siehe Defines)

    float Lebensdauer;  // Wie lange existiert das Partikel ?

    // DKS - Moved construction duties into CreatePartikel(), since all partikels are
    //      created there anyway and constructor was doing unnecessary work.
    //      NOTE: The primary reason construction has been eliminated is to support the new
    //      pooled memory manager (see DataStructures.h), which stores an array of
    //      pre-constructed objects from which it assigns new ones.
    // PartikelClass();							// Konstruktor
    PartikelClass(){};   // Konstruktor
    ~PartikelClass(){};  // Destruktor
    void CreatePartikel(float x,
                        float y,
                        int Art,  // Bestimmten Partikel erzeugen
                        PlayerClass *pParent = nullptr);
    void Run();     // Partikel animieren und bewegen
    bool Render();  // Partikel anzeigen

    // DKS: Found that colors were getting set unsafely in Run() and elsewhere, and I
    //      also converted the r,g,b,a member vars to uint8_t; these ensure safety:
    void SetRed(int value) {
        red = static_cast<uint8_t>(std::clamp(value, 0, 255));
    }

    void SetGreen(int value) {
        green = static_cast<uint8_t>(std::clamp(value, 0, 255));
    }

    void SetBlue(int value) {
        blue = static_cast<uint8_t>(std::clamp(value, 0, 255));
    }

    void SetAlpha(int value) {
        alpha = static_cast<uint8_t>(std::clamp(value, 0, 255));
    }

    PartikelClass *pNext;  // Zeiger auf den nächsten   Partikel
    // DKS - Particle list is now singly-linked, disabled *pPrev:
    // PartikelClass		*pPrev;						// Zeiger auf den vorherigen Partikel

    PlayerClass *m_pParent;
};

// --------------------------------------------------------------------------------------
// Partikelsystem Klasse, Linked List, die alle Partikel beinhaltet
// --------------------------------------------------------------------------------------

class PartikelsystemClass {
    friend class PartikelClass;

  private:
    PartikelClass *pStart;  // Erstes  Element der Liste
    PartikelClass *pEnd;    // Letztes Element der Liste

    int NumPartikel;   // aktuelle Zahl der Partikel
    int MAX_PARTIKEL;  // was wohl
    int CurrentPartikelTexture;     // Aktuelle Textur der Partikel
    float xtarget, ytarget;         // Zielpunkt, auf den sich bestimmte Partikel richten
    float ThunderAlpha;             // Alpha für Blitz
    unsigned char ThunderColor[3];  // Farbe des Blitzes (r, g, b)

    // DKS - New, simple pooled memory manager decreases alloc/dealloc overhead: (see DataStructures.h)
#ifndef USE_NO_MEMPOOLING
    MemPool<PartikelClass, 5000> particle_pool;
#endif

  public:

    PartikelsystemClass();   // Konstruktor
    ~PartikelsystemClass();  // Destruktor

    // DKS - PartikelsystemClass is now a static global, instead of dynamically allocated
    //      pointer, so moved the loading of sprites from its constructor to this new
    //      function:
    void LoadSprites();

    bool PushPartikel(float x, float y, int Art,
                      PlayerClass *pParent = nullptr);  // Partikel "Art" hinzufügen

    // DKS - Converted particle linked-list to be singly-linked so this DelNode()
    //      is a new function that replaces the old DelSel().
    //      It is now up to the caller to splice the list, this blindly deletes what is passed
    //      to it and returns the pointer that was in pPtr->pNext, or NULL if pPtr was NULL
    // void DelSel		(PartikelClass *pTemp);			// Ausgewähltes Objekt entfernen
    PartikelClass *DelNode(PartikelClass *pPtr);

    PartikelClass *GetPStart() const { return pStart; }
    void ClearAll();                    // Alle Objekte löschen
    int GetNumPartikel() const;         // Zahl der Partikel zurückliefern
    void DoPartikel();                  // Alle Partikel der Liste animieren/anzeigen
    void DoPartikelSpecial(bool ShowThem);  // Alle Partikel der Liste animieren/anzeigen
    void DrawOnly();                    // Alle Partikel der Liste nur anzeigen
    void DoThunder();
    void ClearPowerUpEffects();
    void SetParticleCount();  // Je nach Detailstufe Maximum setzen
    void SetTarget(float x, float y);
    void SetThunderColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void ResetPartikelTexture() { CurrentPartikelTexture = -1; }
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern PartikelsystemClass PartikelSystem;
extern DirectGraphicsSprite PartikelGrafix[MAX_PARTIKELGFX];  // Grafiken der Partikel

#endif
