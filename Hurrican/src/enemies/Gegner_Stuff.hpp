#ifndef _GEGNER_STUFF_HPP_
#define _GEGNER_STUFF_HPP_

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Gegner/Objekt-Defines

enum Enemy {
  EXTRAS = 0,            // Alle Extras in einer Grafik
  ONEUP = 1,             // Extra-Leben
  DIAMANT = 2,           // Diamant für Punkte und Extralaben
  POWERBLOCK = 3,        // Block mit Extras
  SPITTER = 4,           // Kleiner "Spitter" Gegner
  WALKER = 5,            // Typischer Walker
  GUNTOWER = 6,          // Geschützturm
  SPIDERBOMB = 7,        // Spinnenbombe
  PIRANHA = 8,           // Piranha
  STAHLMUECKE = 9,       // Stahlmücke
  DECKENTURM = 10,       // Geschützturm an der Decke
  KUGELKLEIN = 11,       // Kleine   Stachel-Kugel
  KUGELMEDIUM = 12,      // Mittlere Stachel-Kugel
  KUGELGROSS = 13,       // Große    Stachel-Kugel
  KUGELRIESIG = 14,      // Riesige  Stachel-Kugel
  KRABBLEROBEN = 15,     // Decken Krabbler
  KRABBLERLINKS = 16,    // Wand Krabbler
  KRABBLERRECHTS = 17,   // Wand Krabbler
  AUGE = 18,             // Fliegendes Extra Auge
  QUALLE = 19,           // Die kleine Qualle
  FALLINGROCK = 20,      // Fallender   Felsblock
  BROCKELROCK = 21,      // Bröckelnder Felsblock
  ROBORAUPE = 22,        // Robo Raupe
  STALAGTIT = 23,        // Stalagtit
  RAKETENWERFER = 24,    // Raketenwerfer der auf und zuklappt
  SWIMWALKER = 25,       // Schwimmender Walker
  FLUGSACK = 26,         // Der fliegende Fettsack =)
  REITFLUGSACK = 27,     // Der Fettsack, auf dem der Hurri später reitet
  EISSTACHEL = 28,       // Sich drehender Eisstachel
  SKIWALKER = 29,        // Walker auf Ski
  FLUGKANONE = 30,       // Fliegende Kanone
  SNOWBOMBE = 31,        // SchneeBomben Emitter
  ROBOMAN1 = 32,         // Robo fliegend mit Raketen
  BOULDER = 33,          // Runterfallender blauer Stein
  STELZSACK = 34,        // Runterfallender StelzSack
  FALLBOMBE = 35,        // Fallende Bombe im Fahrstuhl
  POKENUKE = 36,         // Riesen Poke Nuke
  LAVAKRABBE = 37,       // Anfliegende Lava Krabbe
  LAVABALLSPAWNER = 38,  // Lava Ball Spawner, der Lava Bälle erzeugt
  LAVABALL = 39,         // Lava Ball
  FETTESPINNE = 40,      // Die fiese fette Spinne
  EIERMANN = 41,         // Der Spinnen-Eiermann, der Eier legt
  WANDKANNONE = 42,      // Kannone an der Wand (Flugsack Level)
  STARSMALL = 43,        // Kleiner Stern (Flugsack Level)
  STARBIG = 44,          // Großer  Stern (Flugsack Level)
  MADE = 45,             // Made, die der Alienboss spuckt
  DRONE = 46,            // Drone
  NEUFISCH = 47,         // Großer Fisch
  PFLANZE = 48,          // Feuerball spuckende Pflanze
  SPITTERBOMBE = 49,     // Bombe, aus der der Spitter rauskommt
  NEST = 50,             // Wespennest
  SCHIENENVIECH = 51,    // Die Schienendrone
  WASSERMINE = 52,       // Wassermine
  FLEDERMAUS = 53,       // Fledermaus
  CLIMBSPIDER = 54,      // Kletterspinne
  FIRESPIDER = 55,       // Feuerspinne
  BALLERDRONE = 56,      // Die fliegende fette Drone
  SCHWABBEL = 57,        // Der Madenkotzende Schwabbel
  BIGROCKET = 58,        // Die fette Rakete im Flugsack Level
  EISZAPFEN = 59,        // Runterfallender Eiszapfen
  WANDKRABBE = 60,       // An der Wand krabbelnde Krabbe
  JAEGER = 61,           // Abfangjäger im Flugsacklevel
  SIDEROCKET = 62,       // Seitliche Rakete an der Wand
  FIESEDRONE = 63,       // Die kleine fiese Drone im Space Level (und anderswo)
  SCHLEIMBOLLER = 64,    // Der grüne Schleimbollen
  SCHLEIMALIEN = 65,     // Der Schleimbollen im Alien Level
  SCHLEIMMAUL = 66,      // Der Schleimbollen mit Maul dran
  FIESERWALKER = 67,     // Der fiese Walker mit dem Laser
  FIESERWALKER2 = 68,    // Der fiese Walker mit dem Flammenwerfer
  MITTELSPINNE = 69,     // Mittelgroße Spinne beim Wuxe Spinnenangriff
  SMALLWESPE = 70,       // Kleine Wespe
  DECKENKRABBE = 71,     // Krabbe an der Decke
  STAMPFSTEIN = 72,      // Der stampfende Stein
  LAVAMANN = 73,         // Der Lavamann
  LAFASS = 74,           // Das Fass
  GESCHUETZ = 75,        // Deckengeschuetz
  FETTERAKETE = 76,      // Fette Spinnenrakete
  SCHABE = 77,           // Wandschabe
  MINIDRAGON = 78,       // Mini Drache
  SCHNEEKOPPE = 79,      // Schuss des Schneekönigs
  ZITRONE = 80,          // Die Zieh Drone
  ROTZPOTT = 81,         // Rotzi
  STACHELBEERE = 82,     // Die aufklappende Stachelbeere
  MINIROCKET = 83,       // Kleine Rakete der Stachelbeere
  MUTANT = 84,           // Der rumspringende Mutant aus der Röhre

  PUNISHER = 99,         // Der Punisher, der kommt, wenn der Spieler keine Zeit mehr hat

  // Viertel Bosse
  RIESENPIRANHA = 100,    // Der riesige grüne Piranha
  RIESENQUALLE = 101,     // Die riesige fette Qualle
  RIESENRAUPE = 102,      // Die riesige Robo Raupe
  RIESENWASP = 103,       // Die riesige Robo Wespe

  // Ziwschen und Endbosse
  STAHLFAUST = 120,       // Riesige  Stahlfaust
  PHARAOKOPF = 121,       // Riesiger Pharao Kopf
  RIESENSPINNE = 122,     // Die fiese ecklige Riesenspinne
  EVILHURRI = 123,        // Böser Hurrican
  UFO = 124,              // Das Ufo
  FAHRSTUHLBOSS = 125,    // Boss im Fahrstuhl
  FLUGBOSS = 126,         // Boss im Flugsack Level
  BRATKLOPS = 127,        // Boss im Alien Level
  GEGNERSTURM = 128,      // Ansturm von vielen fiesen Gegnern
  METALHEAD = 129,        // Roboter Metall Kopf
  EISFAUST = 130,         // Stahlfaust Boss im Eis
  WUXESPINNEN = 131,      // Spinnenansturm vom Wuxe
  GOLEM = 132,            // Golem Boss im Lava Level
  SPINNENMASCHINE = 133,  // Spinnenmaschine in der Spiderfactory
  DRACHE = 134,           // Der Drache im Turmlevel
  ROLLMOPS = 135,         // Der Rollmops im Eislevel
  SCHNEEKOENIG = 136,     // Der Rollmops im Eislevel
  BIGFISH = 137,          // Der riesen Piranha
  SKELETOR = 138,         // Skeletor
  THEWALL = 139,          // Die Endbosswand

  // Trigger und andere Objekte
  PARTIKELSPAWN = 140,   // Partikel-Erzeuger (Regen, Schnee etc)
  FAHRSTUHL = 141,       // Fahrstuhl
  FASS = 142,            // Fass explodiert bei Schuss in Splitter
  FEUERSPUCKER = 143,    // Erzeugt die Feuerfalle
  FADEMUSIC = 144,       // Music Fade Trigger
  ENDLEVEL = 145,        // Level Ende Position
  PLATTFORM = 146,       // Fliegende Plattform
  PRESSE = 147,          // Stahlpresse
  SHRINE = 148,          // Schrein mit Manfred drin =)
  BRUECKE = 149,         // Hängebrücke
  FLOATING = 150,        // kleine schwebe Plattform
  FLOATING2 = 151,       // mittlere schwebe Plattform
  FLOATING3 = 152,       // große schwebe Plattform

  SURFBRETT = 153,       // Surfbrett Gegner =)
  SHOOTBUTTON = 154,     // Schwebe Plattform Button (wird im Editor nicht gesetzt, nur vom Game)
  SHOOTPLATTFORM = 155,  // Schwebe Plattform (durch Button aufzulösen)
  GLUBSCHI = 156,        // Glubschauge, das dem Hurri hinterherkuckt
  GLUBSCHI2 = 157,       // Glubschauge von der Decke
  COLUMN = 158,          // Die Säule, die umfällt, und das ganze Level schräg stellt
  LIFT = 159,            // Lift mit Countdown, der explodiert
  COLUMN2 = 160,         // Die Säule, die umfällt, und das ganze Level wieder aufstellt
  SCHLEUSEH = 161,       // Horizontale Schleuse
  SCHLEUSEV = 162,       // Vertikale Schleuse
  SWITCH = 163,          // Schalter (zum Türen öffnen)
  WARNING = 164,         // Warnungs Schild
  SOUNDTRIGGER = 165,    // Sound Trigger
  LUEFTER_GROSS = 166,   // Großer Luefter
  LUEFTER_KLEIN = 167,   // Kleiner Leufter = 1,
  LUEFTER_KLEIN2 = 168,  // Kleiner Leufter = 2,
  TUTORIALTEXT = 169,    // Textbox für Tutorial
  LIGHTFLARE = 170,      // Wie der Name schon sagt ;)
  SECRET = 171,          // Secret Area im Level
  MUSHROOM = 172,        // Pilzkopf-Trampolin =)
  PRESSWURST = 173,      // Die Spinnen Presswurst
  LAFASSSPAWNER = 174,   // Der Trigger für das Fass, das an der Decke entlang fährt
  SPIKELIFT = 175,       // Lift mit Stacheln
  TUBE = 176,            // Glasröhre

  // Objekte, die nicht im Editor auftauchen
  KETTENGLIED = 190,     // Kettenglied z.B. vom Rollmops
  SKULL = 191            // Schädel vom Skeletor
};

//----- Handlungen der Gegner

namespace GEGNER {

enum EnemyAction {
  STEHEN = 0,
  LAUFEN = 1,
  LAUFEN2 = 2,
  LAUFEN3 = 3,
  LAUFEN4 = 4,
  SCHIESSEN = 5,
  SPRINGEN = 6,
  FALLEN = 7,
  WATSCHELN = 8,      // Für Walker
  DREHEN = 9,         // Für Geschützturm
  DREHEN2 = 10,       // Für Stelzsack
  VERFOLGEN = 11,     // Für Stahlmücke
  UNVERWUNDBAR = 12,  // Für Deckenturm
  OEFFNEN = 13,       // Für Deckenturm nochmal
  SCHLIESSEN = 14,    // Für Deckenturm (einmal noch :P )
  NOTVISIBLE = 99,
};

// Zwischenbosse

enum {
  INIT = 30,            // Warten bis der Screen zentriert wurde
  EINFLIEGEN = 31,      // Gegner erscheint am Screen
  CRUSHEN = 32,         // Stahlfaust/Pharao zerquetscht den Hurri
  CRUSHEN2 = 33,        // Stahlfaust/Pharao zerquetscht den Hurri
  CRUSHENERHOLEN = 34,  // Stahlfaust fliegt nach dem Crushen hoch
  SPECIAL = 35,         // Special Move  (Pharao Steine rieseln lassen etc)
  SPECIAL2 = 36,        // Special Move2 (Bratklops Fettboller usw)
  SPECIAL3 = 37,        // Special Move3 (Bratklops Laser usw)
  ABSENKEN = 38,        // Spinne senkt sich ab
  ABSENKENZWEI = 39,    // Spinne senkt sich ab und schiesst fetten laser
  AUFRICHTEN = 40,      // Spinne richtet sich wieder auf
  AUFRICHTENZWEI = 41,  // Spinne richtet sich wieder auf und schiesst fetten laser =)
  AUSSPUCKEN = 42,      // Spinne spuckt kleine Spinnen Bomben aus
  AUSSPUCKENZWEI = 43,
  BOMBARDIEREN = 44,  // Spinne wirft Granaten

  INIT2 = 45,
  INIT3 = 46,
  INIT4 = 47,

  EINFAHREN = 48,
  AUSFAHREN = 49,
  EINFLIEGEN2 = 50,
  WARTEN = 51,

  LAUFEN_LINKS = 52,
  LAUFEN_RECHTS = 53,

  LAUFEN_LINKS2 = 54,
  LAUFEN_RECHTS2 = 55,

  AUSWAHL = 56,

  EXPLODIEREN = 80,  // Endboss fliegt in die Luft
};

} // namespace

//----- Sonstiges

constexpr int MAX_GEGNER = 2048;    // Maximale Zahl Gegner pro Level
constexpr int MAX_GEGNERGFX = 200;  // Maximale Grafiken der Gegner

//----- Externals

extern GegnerListClass Gegner;
extern DirectGraphicsSprite *pGegnerGrafix[MAX_GEGNERGFX];  // Grafiken der Gegner
extern RECT_struct GegnerRect[MAX_GEGNERGFX];                      // Rechtecke für Kollision
extern float g_Fahrstuhl_yPos;                              // yPosition des Fahrstuhls
extern float g_Fahrstuhl_Offset;                            // Scrollposition relativ zum Farhstuhl
extern float g_Fahrstuhl_Speed;                             // Fahrstuhl Geschwindigkeit

#endif
