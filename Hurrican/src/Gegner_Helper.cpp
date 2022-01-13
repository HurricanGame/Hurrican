// Datei : Gegner.cpp

// --------------------------------------------------------------------------------------
//
// Gegner für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Gegner_Helper.hpp"
#include "Console.hpp"
#include "DX8Font.hpp"
#include "DX8Sound.hpp"
#include "Gameplay.hpp"
#include "Globals.hpp"
#include "HUD.hpp"
#include "Logdatei.hpp"
#include "Partikelsystem.hpp"
#include "Player.hpp"
#include "Projectiles.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"

#include <cstdint>

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

DirectGraphicsSprite *pGegnerGrafix[MAX_GEGNERGFX];  // Grafiken der Gegner
RECT_struct GegnerRect[MAX_GEGNERGFX];                      // Rechtecke für Kollision
float g_Fahrstuhl_yPos;                              // yPosition des Fahrstuhls
float g_Fahrstuhl_Offset;                            // Scrollposition relativ zum Farhstuhl
float g_Fahrstuhl_Speed;                             // Fahrstuhl Geschwindigkeit

// --------------------------------------------------------------------------------------
// Bestimmte Gegner-Grafik laden
// --------------------------------------------------------------------------------------

struct SpriteData {
  const char* filename;
  uint16_t xs;
  uint16_t ys;
  uint16_t xfs;
  uint16_t yfs;
  uint16_t xfc;
  uint16_t yfc;
};

SpriteData sprites[] = {
    { 0, 0, 0, 0, 0, 0, 0 }, // EXTRAS
    { 0, 0, 0, 0, 0, 0, 0 }, // ONEUP
    { 0, 0, 0, 0, 0, 0, 0 }, // DIAMANT
    { 0, 0, 0, 0, 0, 0, 0 }, // POWERBLOCK
    { "spitter.png", 110, 23, 22, 23, 5, 1 },
    { "enemy-walker.png", 360, 180, 45, 45, 8, 4 },
    { "enemy-guntower.png", 300, 120, 60, 40, 5, 3 },
    { "spiderbomb.png", 350, 34, 50, 34, 7, 1 },
    { "piranha.png", 225, 232, 45, 29, 5, 8 },
    { "stahlmuecke.png", 170, 70, 34, 35, 5, 2 },
    { "deckenturm.png", 190, 240, 38, 40, 5, 6 },
    { "kugel4.png", 150, 120, 30, 30, 5, 4 },
    { "kugel3.png", 225, 180, 45, 45, 5, 4 },
    { "kugel2.png", 300, 240, 60, 60, 5, 4 },
    { "kugel1.png", 450, 360, 90, 90, 5, 4 },
    { "krabbleroben.png", 250, 225, 50, 75, 5, 3 },
    { "krabblerlinks.png", 225, 250, 75, 50, 3, 5 },
    { "krabblerrechts.png", 225, 250, 75, 50, 3, 5 },
    { "auge.png", 360, 26, 60, 26, 6, 1 },
    { "qualle.png", 240, 180, 40, 60, 6, 3 },
    { "fallingstone.png", 80, 40, 80, 40, 1, 1 },
    { "fallingstone2.png", 160, 40, 80, 40, 2, 1 },
    { "roboraupe.png", 360, 105, 90, 35, 4, 3 },
    { "stalagtit.png", 40, 80, 40, 80, 1, 1 },
    { "raketenwerfer.png", 225, 50, 45, 25, 5, 2 },
    { "swimwalker.png", 260, 135, 65, 45, 4, 3 },
    { "flugsack.png", 320, 210, 80, 70, 4, 3 },
    { "flugsack.png", 320, 210, 80, 70, 4, 3 },
    { "eisstachel.png", 420, 390, 105, 78, 4, 5 },
    { "skiwalker.png", 195, 152, 65, 38, 3, 4 },
    { "flugkanone.png", 260, 225, 65, 45, 4, 5 },
    { "snowbomb.png", 480, 320, 80, 80, 6, 4 },
    { "roboman1.png", 450, 300, 90, 100, 5, 3 },
    { "boulder.png", 300, 165, 60, 55, 5, 3 },
    // DKS - Corrected yfc parameter (Y frame count) from 4 to 3, to match actual
    //      image file:
    { "stelzsack.png", 500, 441, 100, 147, 5, 3 },
    { "fallbombe.png", 138, 59, 23, 59, 6, 1 },
    { "pokenuke.png", 81, 217, 81, 217, 1, 1 },
    { "lavakrabbe.png", 280, 136, 70, 34, 4, 4 },
    { "lavaball.png", 300, 240, 60, 60, 5, 4 },
    { "lavaball.png", 300, 240, 60, 60, 5, 4 },
    { "fettespinne.png", 416, 414, 104, 69, 4, 6 },
    { "eiermann.png", 310, 132, 62, 66, 5, 2 },
    { "wandcannon.png", 141, 31, 47, 32, 3, 1 },
    { "starsmall.png", 80, 80, 40, 40, 2, 2 },
    { "starbig.png", 160, 160, 80, 80, 2, 2 },
    { "made.png", 200, 40, 20, 20, 10, 2 },
    { "drone.png", 280, 240, 70, 80, 4, 3 },
    { "neufisch.png", 360, 292, 90, 73, 4, 4 },
    { "pflanze.png", 440, 450, 110, 90, 4, 5 },
    { "spitterbombe.png", 174, 50, 29, 50, 6, 1 },
    { "nest.png", 43, 60, 43, 60, 1, 1 },
    { "schienenviech.png", 245, 40, 49, 40, 5, 1 },
    // DKS - Corrected dimensions from 300x190 to 300x180, to match actual image file:
    { "wassermine.png", 300, 180, 60, 90, 5, 2 },
    { "fledermaus.png", 500, 174, 50, 58, 10, 3 },
    { "climbspider.png", 250, 105, 50, 35, 5, 3 },
    { "feuerspinne.png", 480, 110, 80, 55, 6, 2 },
    { "ballerdrone.png", 450, 195, 90, 65, 5, 3 },
    { "schwabbel.png", 300, 240, 60, 60, 5, 4 },
    // DKS - Corrected dimensions from 578x600 to 584x600,as image file had
    //      misaligned spritesheet. Image has also been corrected.
    { "bigrocket.png", 584, 600, 73, 300, 8, 2 },
    { "eiszapfen.png", 30, 100, 30, 100, 1, 1 },
    { "wandkrabbe.png", 136, 192, 34, 64, 4, 3 },
    { "jaeger.png", 280, 212, 70, 53, 4, 4 },
    { "siderocket.png", 40, 18, 40, 18, 1, 1 },
    { "fiesedrone.png", 200, 250, 50, 50, 4, 5 },
    { "schleimboller.png", 240, 240, 60, 60, 4, 4 },
    { "schleimrot.png", 240, 240, 60, 60, 4, 4 },
    { "schleimmaul.png", 272, 272, 68, 68, 4, 4 },
    { "fieserwalker.png", 264, 240, 66, 60, 4, 4 },
    { "fieserwalker2.png", 264, 240, 66, 60, 4, 4 },
    { "mittelspinne.png", 321, 344, 107, 86, 3, 4 },
    { "smallwespe.png", 200, 40, 50, 40, 4, 1 },
    { "lavakrabbe.png", 280, 136, 70, 34, 4, 4 },
    { "stampfstein.png", 80, 256, 80, 256, 1, 1 },
    { "lavamann.png", 280, 150, 40, 50, 7, 3 },
    { "lafass.png", 444, 495, 74, 165, 6, 3 },
    { "geschuetz.png", 300, 300, 60, 60, 5, 5 },
    { "fetterakete.png", 250, 34, 25, 34, 10, 1 },
    { "schabe.png", 80, 60, 20, 20, 4, 3 },
    { "minidragon.png", 300, 141, 60, 47, 5, 3 },
    { "schneekoppe.png", 14, 28, 14, 28, 1, 1 },
    { "zitrone.png", 460, 243, 115, 81, 4, 3 },
    { "rotzpott.png", 55, 41, 55, 41, 1, 1 },
    { "stachelbeere.png", 360, 360, 60, 60, 6, 6 },
    { "minirocket.png", 12, 12, 12, 12, 1, 1 },
    { "mutant.png", 500, 450, 100, 90, 5, 5 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { "punisher.png", 1020, 850, 170, 170, 6, 5 },
    { "riesenpiranha.png", 1024, 420, 256, 210, 4, 2 },
    { "riesenqualle.png", 390, 480, 130, 240, 3, 2 },
    { "riesenraupe.png", 540, 290, 180, 58, 3, 5 },
    { "wasp.png", 232, 300, 116, 100, 2, 3 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { "stahlfaust.png", 512, 264, 256, 264, 2, 1 },
    { "pharaokopf.png", 448, 448, 224, 224, 2, 2 },
    { "spiderboss_rumpf.png", 415, 160, 415, 160, 1, 1 },
    { 0, 0, 0, 0, 0, 0, 0 }, // EVILHURRI
    { "ufo.png", 600, 658, 200, 94, 3, 7 },
    { "fahrstuhlboss_rahmen.png", 420, 280, 420, 280, 1, 1 },
    { "flugboss_main.png", 280, 285, 280, 285, 1, 1 },
    // DKS - Corrected dimensions from 323x400 to 232x400, to match actual image file:
    //      NOTE: also see my notes in Boss_Bratklops.cpp regarding this and other sprites. (TODO)
    { "bratklops0000.png", 232, 400, 232, 400, 1, 1 },
    { 0, 0, 0, 0, 0, 0, 0 }, // GEGNERSTURM
    { "metalhead_birne.png", 204, 242, 204, 242, 1, 1 },
    { "stahlfaust_eis.png", 512, 264, 256, 264, 2, 1 },
    { 0, 0, 0, 0, 0, 0, 0 }, // WUXESPINNEN
    { "golem_rumpf.png", 185, 291, 185, 291, 1, 1 },
    { "spinnenmaschine_oben.png", 317, 413, 317, 413, 1, 1 },
    { "drache_body.png", 201, 147, 201, 147, 1, 1 },
    { "snowbomb.png", 480, 320, 80, 80, 6, 4 },
    { "schneekoenig.png", 800, 600, 200, 200, 4, 3 },
    // DKS - Corrected dimensions from 208x157 to 208x156, to match actual image file
    { "bigfish.png", 208, 156, 208, 156, 1, 1 },
    { "skeletor.png", 1000, 400, 125, 200, 8, 2 },
    { "endbossmaschine.png", 285, 480, 285, 480, 1, 1 },
    { 0, 0, 0, 0, 0, 0, 0 }, // PARTIKELSPAWN
    { "fahrstuhl.png", 400, 134, 400, 134, 1, 1 },
    { "fass.png", 48, 56, 48, 56, 1, 1 },
    { "flammenwerfer.png", 120, 120, 60, 60, 2, 2 },
    { 0, 0, 0, 0, 0, 0, 0 }, // FADEMUSIC
    { 0, 0, 0, 0, 0, 0, 0 }, // ENDLEVEL
    { "plattform.png", 128, 64, 128, 32, 1, 2 },
    { "presse.png", 100, 256, 100, 256, 1, 1 },
    { "trenzshrine.png", 120, 180, 120, 180, 1, 1 },
    { "bruecke.png", 10, 6, 10, 6, 1, 1 },
    { "floating.png", 39, 14, 39, 14, 1, 1 },
    { "floating2.png", 110, 18, 110, 18, 1, 1 },
    { "floating3.png", 100, 54, 100, 27, 1, 2 },
    { "surfbrett.png", 52, 35, 52, 35, 1, 1 },
    { "shootplattformbutton.png", 32, 9, 16, 9, 2, 1 },
    { "shootplattform.png", 100, 54, 100, 54, 1, 1 },
    { "glubschi.png", 510, 360, 51, 120, 10, 3 },
    // DKS - Corrected dimensions from 510x350 to 510x360, to match actual image file:
    { "glubschi2.png", 510, 360, 51, 120, 10, 3 },
    { "column.png", 40, 100, 40, 100, 1, 1 },
    { "lift.png", 80, 230, 80, 23, 1, 10 },
    { "column.png", 40, 100, 40, 100, 1, 1 },
    { "schleuse.png", 200, 40, 200, 40, 1, 1 },
    { "schleuse2.png", 40, 200, 40, 200, 1, 1 },
    { "switch.png", 48, 32, 24, 32, 2, 1 },
    { "warning.png", 180, 40, 180, 40, 1, 1 },
    { 0, 0, 0, 0, 0, 0, 0 }, // SOUNDTRIGGER
    { "luefter_gross.png", 224, 223, 224, 223, 1, 1 },
    // DKS - Original dimensions in code here were 453x342, with frame size of 113x114, which not only
    //      did not match what the true dimensions should be, but also caused problems with
    //      resized textures. When using resized textures (new feature), fan image would jiggle.
    //      I resized the image and its frames to have a one-pixel transparent border on X-axis,
    //      giving overall dimension of 456x342 with frame size of 114x114. Resizing issues fixed.
    { "luefter1.png", 456, 342, 114, 114, 4, 3 },
    // DKS - See note above
    { "luefter2.png", 456, 342, 114, 114, 4, 3 },
    { 0, 0, 0, 0, 0, 0, 0 }, // TUTORIALTEXT
    { "lavaflare.png", 120, 120, 120, 120, 1, 1 },
    { 0, 0, 0, 0, 0, 0, 0 }, // SECRET
    { "mushroom_head.png", 135, 61, 135, 61, 1, 1 },
    { "presswurst.png", 185, 357, 185, 357, 1, 1 },
    { "lafass_spawner.png", 74, 165, 74, 165, 1, 1 },
    { "spikelift.png", 460, 80, 460, 40, 1, 2 },
    { "tube.png", 400, 220, 100, 220, 4, 1 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { "rollmops_kette.png", 18, 36, 18, 18, 1, 2 },
    { "skull.png", 256, 123, 32, 41, 8, 3 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 }
};

void LoadGegnerGrafik(int Nr) {
    // Check if graphics is already loaded
    if (pGegnerGrafix[Nr] != nullptr)
        return;

    pGegnerGrafix[Nr] = new DirectGraphicsSprite();

    SpriteData& data = sprites[Nr];
    if (data.filename == nullptr) {
        return;
    }

    pGegnerGrafix[Nr]->LoadImage(data.filename, data.xs, data.ys, data.xfs, data.yfs, data.xfc, data.yfc);

    if (Nr == REITFLUGSACK) {
        // DKS - Flugsack song is now loaded on-demand here instead of globally in Main.cpp:
        SoundManager.LoadSong("flugsack.it", MUSIC::FLUGSACK);
    }
}
