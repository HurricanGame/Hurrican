// Datei : Partikelsystem.cpp

// --------------------------------------------------------------------------------------
//
// Partikelsystem für Hurrican
// für Funken, Rauchwolken, Patronenhülsen usw.
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "Partikelsystem.hpp"
#include "Console.hpp"
#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "DX8Sound.hpp"
#include "Gameplay.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Player.hpp"
#include "Projectiles.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

enum class DrawModeEnum {
  NORMAL,
  ROTATED
};

// --------------------------------------------------------------------------------------
// externe Variablen
// --------------------------------------------------------------------------------------

extern Logdatei Protokoll;
extern DirectGraphicsFont *pFont;
extern PartikelsystemClass PartikelSystem;
extern TileEngineClass TileEngine;
extern ProjectileListClass Projectiles;
extern ConsoleClass Console;
extern LPDIRECT3DDEVICE8 lpD3DDevice;  // Direct3D Device-Objekt

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

DirectGraphicsSprite PartikelGrafix[MAX_PARTIKELGFX];  // Grafiken der Partikel
RECT_struct PartikelRect[MAX_PARTIKELGFX];             // Rechtecke für Level Kollision
int CurrentPartikelTexture;                            // Aktuelle Textur der Partikel
DrawModeEnum DrawMode;                                 // normale oder rotierte Partikel?

// --------------------------------------------------------------------------------------
// PartikelKlasse Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

// DKS - Moved construction duties into CreatePartikel(), since all partikels are
//      created there anyway and constructor was doing unnecessary work.
//      NOTE: The primary reason construction has been eliminated is to support the new
//      pooled memory manager (see DataStructures.h), which stores an array of
//      pre-constructed objects from which it assigns new ones.
#if 0
PartikelClass::PartikelClass()
{
    // Partikelwerte auf Null setzen (zur Sicherheit)
    red = green = blue = 255;
    AnimCount = 0.0f;
    xPos	= 0.0f;
    yPos	= 0.0f;
    xPosOld = 0.0f;
    yPosOld = 0.0f;
    xSpeed	= 0.0f;
    ySpeed	= 0.0f;
    xAcc	= 0.0f;
    yAcc	= 0.0f;
    OwnDraw				= false;
    Rotate				= false;
    Rot					= 0.0f;
    RemoveWhenOffScreen = true;
    m_pParent = nullptr;

    (rand()%2 == 0) ? (RotDir = 1) : (RotDir = -1);
}
#endif  // 0

// --------------------------------------------------------------------------------------
// Partikel vom Typ "Art" erzeugen
// --------------------------------------------------------------------------------------

void PartikelClass::CreatePartikel(float x, float y, int Art, PlayerClass *pParent) {
// leucht-Partikel nur im Glow-Modus anzeigen
// d.h., im nicht-Glow Modus gleich wieder aussteigen, sobald ein
// solcher Partikel erzeugt werden soll
//
// DKS - Moved this check to PushPartikel() to fix potential memory leak there:
//      (CreatePartikel() function now has void return type, not bool)
#if 0
    if (options_Detail < DETAIL_HIGH &&
            (Art == GRENADEFLARE ||
             Art == SHOTFLARE    ||
             Art == SHOTFLARE2   ||
             Art == EXPLOSIONFLARE))
        return false;
#endif  // 0

    // DKS - Added initializer for colors to their most commonly-set values,
    //      (code de-duplication in rest of function)
    red = green = blue = alpha = 255;

    // DKS - Added initializer for lebensdauer to its most commonly-set value,
    //      (code de-duplication in rest of function)
    Lebensdauer = 255.0f;

    // DKS - Moved parts of class constructor that weren't redundant here, and made
    //      class constructor empty, since all particles are created through this
    //      function anyway:
    xSpeed = 0.0f;
    ySpeed = 0.0f;
    xAcc = 0.0f;
    yAcc = 0.0f;
    OwnDraw = false;
    Rotate = false;
    Rot = 0.0f;
    RemoveWhenOffScreen = true;
    (random(2) == 0) ? (RotDir = 1) : (RotDir = -1);

    m_pParent = pParent;
    xPos = x;
    yPos = y;
    xPosOld = x;
    yPosOld = y;
    PartikelArt = Art;
    AnimPhase = 0;
    AnimCount = 0.0f;
    AnimEnde = 0;  // Standardmässig von einem nicht animierten Partikel ausgehen
    AnimSpeed = 0.0f;
    BounceWalls = false;

    // Zwei Case Anweisungen, einmal für nicht additive und einmal für additive Partikel
    // Wird dadurch im Schnitt hoffentlich doppelt so schnell ;)
    if (Art < ADDITIV_GRENZE)
        switch (Art) {
            case BULLET:  // Leere Patronenhülse
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(40) + 20) / 5) *
                          static_cast<float>(Direction::asInt(Direction::invert(m_pParent->Blickrichtung)));
                ySpeed = -static_cast<float>(random(40) + 20) / 5;
                xAcc = 0.0f;
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;
                // DKS - This was commented out in original source:
                // AnimEnde	= 8;
                AnimSpeed = 0.0f;

                Rotate = true;

            } break;

            case BULLET_SKELETOR:  // Leere Patronenhülse vom Skeletor
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(40) - 20) / 3);
                ySpeed = -static_cast<float>(random(8) + 14);
                xAcc = 0.0f;
                yAcc = 10.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                AnimSpeed = 0.0f;

                Rotate = true;

            } break;

            case GLASSPLITTER: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(40) - 20)) / 2;
                ySpeed = -static_cast<float>(random(10) + 20) / 2;
                xAcc = 0.0f;
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                AnimPhase = random(20);

                Rotate = true;
            } break;

            case EXPLOSION_MEDIUM:  // Mittelgrosse Explosion
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 192;
                AnimEnde = 19;
                AnimSpeed = (random(6) + 3) / 20.0f;

                PartikelSystem.PushPartikel(x - 30, y - 30, EXPLOSIONFLARE);

                Rotate = true;

                if (random(2) == 0)
                    PartikelArt = EXPLOSION_MEDIUM2_ADD;

            } break;

            case EXPLOSION_MEDIUM2:  // Mittelgrosse Explosion Nr 2
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 192;
                AnimEnde = 19;
                AnimSpeed = (random(6) + 3) / 20.0f;

                PartikelSystem.PushPartikel(x - 30, y - 30, EXPLOSIONFLARE);

                Rotate = true;

                if (random(2) == 0)
                    PartikelArt = EXPLOSION_MEDIUM2_ADD;

            } break;

            case EXPLOSION_MEDIUM3:  // Mittelgrosse Explosion Nr 2
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 192;
                AnimEnde = 19;
                AnimSpeed = (random(6) + 3) / 20.0f;

                PartikelSystem.PushPartikel(x - 30, y - 30, EXPLOSIONFLARE);

                if (random(2) == 0)
                    PartikelArt = EXPLOSION_MEDIUM3_ADD;

                Rotate = true;

            } break;

            case EXPLOSION_GIGA:  // Riesig fette Explosion
            {
                x -= 10;
                y -= 10;
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 192;
                AnimEnde = 19;
                AnimSpeed = (random(2) + 5) / 10.0f;

                PartikelSystem.PushPartikel(x + 30, y + 30, EXPLOSIONFLARE);

                for (int i = 0; i < 20; i++) {
                    PartikelSystem.PushPartikel(x + 60 + random(40), y + 60 + random(40), FUNKE);
                    PartikelSystem.PushPartikel(x + 60 + random(40), y + 60 + random(40), LONGFUNKE);
                }

            } break;

            case EXPLOSION_GREEN:  // Grüne Explosion
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 192;
                // DKS - This sprite actually has 25 frames, so increased this to 24:
                // AnimEnde	= 22;
                AnimEnde = 24;
                AnimSpeed = (random(10) + 5) / 15.0f;

            } break;

            case EXPLOSION_ALIEN:  // Lilane Alien Explosion
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 224;
                // DKS - This sprite actually has 25 frames, so increased this to 24:
                // AnimEnde	= 22;
                AnimEnde = 24;
                AnimSpeed = (random(5) + 10) / 80.0f;

                PartikelSystem.PushPartikel(x - 30, y - 30, EXPLOSIONFLARE);

            } break;

            case EXPLOSION_BIG:  // Grosse Explosion
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 224;
                AnimEnde = 19;
                AnimSpeed = (random(5) + 10) / 30.0f;
                PartikelSystem.PushPartikel(x + 32, y + 32, EXPLOSION_KRINGEL);

                if (options_Detail >= DETAIL_MAXIMUM)
                    for (int i = 0; i < 10; i++)
                        PartikelSystem.PushPartikel(x + 32, y + 32, SMOKEBIG2);

                PartikelSystem.PushPartikel(x - 10, y - 10, EXPLOSIONFLARE);
            } break;

            case EXPLOSION_GIANT:  // Riesen Explosion
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 192;
                AnimEnde = 15;
                AnimSpeed = (random(5) + 10) / 15.0f;

            } break;

            case BLUE_EXPLOSION:  // Kleine blaue Explosion
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 224;
                // DKS - Changed value to 11 ( it has 12 frames of animation)
                //      This was glitching after switching to using the sprites' itsPreCalcedRects[]
                // AnimEnde	= 12;
                AnimEnde = 11;
                AnimSpeed = (random(10) + 5) / 15.0f;

            } break;

            case SPLITTER:  // Kleiner animierter Splitter
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 200;
                AnimEnde = 15;
                AnimSpeed = 0.3f;
                xSpeed = (static_cast<float>(random(160) - 80) / 4);
                ySpeed = -((random(20) + 50) / 2.0f);
                yAcc = 3.0f;
                BounceWalls = true;

            } break;

            case PIRANHATEILE:   // Fetzen eines kaputten Piranhas
            case PIRANHATEILE2:  // Fetzen eines kaputten Riesen Piranhas
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = random(5);
                if (random(2) == 0) {
                    xSpeed = -(static_cast<float>(random(20) + 4) / 5);
                    xAcc = 0.1f;
                } else {
                    xSpeed = (static_cast<float>(random(20) + 4) / 5);
                    xAcc = -0.1f;
                }

                yAcc = -static_cast<float>(random(10) + 20) / 100;

            } break;

            case PIRANHABLUT:  // Blut eines kaputten Piranhas
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 192;

                Lebensdauer = 192;
                AnimEnde = 3;
                AnimSpeed = 2.0f;
                ySpeed = -static_cast<float>(random(10) + 20) / 20;
                Rotate = true;
            } break;

            case MADEBLUT:  // Blut der Made
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 192;

                Lebensdauer = 128;
                AnimEnde = 3;
                AnimSpeed = 1.0f;
            } break;

            case SPAWNDROP:  // Wassertropfen der vom Spawner ausgespuckt wird
            {
                red = 230;
                green = 240;
                blue = 255;
                alpha = 128;

                Lebensdauer = 128;
                ySpeed = static_cast<float>(random(10)) + 5;
                yAcc = 4.0f;
                RemoveWhenOffScreen = false;
            } break;

            case BLATT:  // Blatt von kaputter Pflanze
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                ySpeed = -static_cast<float>((random(40)) + 25) / 3.0f;
                yAcc = 5.0f;
                xSpeed = static_cast<float>((random(40)) - 20) / 2.0f;

                Rotate = true;

                AnimPhase = random(2);

            } break;

            case BLATT2:  // Blatt vom Spawner
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                ySpeed = -static_cast<float>((random(20)) - 10) / 10.0f;
                yAcc = -static_cast<float>((random(20)) - 10) / 20.0f;
                xSpeed = -static_cast<float>(random(50) + 30);

                if (WinkelUebergabe != 0.0f)
                    xSpeed *= -1;

                Rotate = true;

                AnimPhase = random(2);

            } break;

            case NESTLUFT:  // Wespennest nach dem Abschuss
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                yAcc = 5.0f;
                xSpeed = static_cast<float>((random(40)) - 20) / 2.0f;
                ySpeed = -static_cast<float>((random(10)) + 20);
                AnimSpeed = 1.5f;

                BounceWalls = true;
                Rotate = true;

                RotDir = 2.0f;

            } break;

            case FOG:  // Nebel in der Luft
            {
                red = 64;
                green = 255;
                blue = 80;
                alpha = 0;

                xSpeed = static_cast<float>(random(30) + 10) / 50.0f;
                ySpeed = -3.0f * (AnimPhase + 1);

                if (random(2) == 0)
                    xSpeed *= -1.0f;

                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = random(2);

                RemoveWhenOffScreen = false;
                AnimCount = 0.0f;
            } break;

            case HALSWIRBEL:  // Halswirbel des MetalHead Bosses
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 0;
                xSpeed = static_cast<float>(random(20) + 20) / 2.0f;
                ySpeed = -static_cast<float>(random(20) + 30);
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                AnimCount = static_cast<float>(random(100));
                OwnDraw = true;
            } break;

            case KAPUTTETURBINE:  // Kaputte Turbine des MetalHead Bosses
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 0;
                xSpeed = 18.0f;
                ySpeed = -30.0f;
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                AnimCount = 0.0f;
                OwnDraw = true;
            } break;

            case ROCKSPLITTER:  // Splitter eines Felsblocks
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(80) - 40) / 4);
                ySpeed = -static_cast<float>(random(30) + 15) / 4;
                xAcc = 0.0f;
                yAcc = 2.8f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                AnimEnde = 7;
                AnimSpeed = (random(10) + 5) / 12.0f;
            } break;

            case ROCKSPLITTERSMALL:  // kleine Splitter eines Felsblocks
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(80) - 40) / 4);
                ySpeed = -static_cast<float>(random(40) + 20) / 3;
                xAcc = 0.0f;
                yAcc = 2.8f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                AnimEnde = 7;
                AnimSpeed = (random(10) + 5) / 8.0f;
            } break;

            case ROCKSPLITTERSMALLBLUE:  // kleine Splitter eines Stalagtits
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(80) - 40) / 4);
                ySpeed = -static_cast<float>(random(40) + 20) / 3;
                xAcc = 0.0f;
                yAcc = 2.8f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                AnimEnde = 7;
                AnimSpeed = (random(10) + 5) / 8.0f;
            } break;

            case SPIDERSPLITTER:  // Splitter der Spinne
            case SPIDERSPLITTER2: {
                if (PartikelArt == SPIDERSPLITTER)
                    PartikelArt += random(2);
                int r = random(128) + 128;
                red = green = blue = r;
                alpha = 255;

                xSpeed = (static_cast<float>(random(80) - 40) / 3.0f);
                ySpeed = -static_cast<float>(random(30) + 15) / 2.0f;
                xAcc = 0.0f;
                yAcc = 3.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                AnimEnde = 15;
                AnimSpeed = (random(10) + 5) / 12.0f;
            } break;

            case SPIDERGRENADE:  // Granate der Spinne
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = -14.0f;
                ySpeed = -15.0f;
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;
                AnimEnde = 3;
                AnimSpeed = (random(10) + 5) / 12.0f;
            } break;

            case EVILSMOKE:  // Schatten des EvilHurri links
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;

                Lebensdauer = 200;
                BounceWalls = false;
            } break;

            case EVILSMOKE2:  // Schatten des EvilHurri rechts
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;

                Lebensdauer = 200;
                BounceWalls = false;
            } break;

            case STELZE:  // Stelze eines Stelzsacks
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                xSpeed = (static_cast<float>(random(40) - 20) / 3.0f);
                ySpeed = -static_cast<float>(random(80) + 40) / 3.0f;
                yAcc = 5.0f;

                Rotate = true;
            } break;

            case STELZHEAD:  // Kopf eines Stelzsacks
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                xSpeed = (static_cast<float>(random(40) - 20) / 5.0f);
                ySpeed = -static_cast<float>(random(80) + 40) / 4.0f;
                yAcc = 4.0f;
                AnimCount = 1.0f;
            } break;

            case SMOKE:  // Rauchwolke
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                ySpeed = -static_cast<float>(random(10)) / 10.0f;
                yAcc = -0.1f;

                // Lebensdauer = 255;      //DKS - now redundant
                Rotate = true;
                Rot = static_cast<float>(random(360));

            } break;

            case SMOKE2:  // Rauch bei zB Lava Bällen
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;

                Lebensdauer = 80;
                xSpeed = (static_cast<float>(random(40) - 20) / 10.0f);
                ySpeed = -static_cast<float>(random(40) - 20) / 6.0f;

                // Grafik hat sich von 20x20 auf 24x24 pixel geändert :P
                xPos -= 2;
                yPos -= 2;

                Rotate = true;

            } break;

            case SMOKE3:     // Aufsteigender Dampf
            case SMOKE3_RO:  // rechts oben
            case SMOKE3_R:   // rechts
            case SMOKE3_RU:  // rechts unten
            case SMOKE3_U:   // unten
            case SMOKE3_LU:  // links unten
            case SMOKE3_L:   // links
            case SMOKE3_LO:  // links oben
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 64;

                Lebensdauer = 128;

                switch (Art) {
                    case SMOKE3: {
                        xSpeed = (static_cast<float>(random(40) - 20) / 15.0f);
                        ySpeed = -static_cast<float>(random(10) + 20) / 2.0f;
                    } break;

                    case SMOKE3_RO:  // rechts oben
                    {
                        xSpeed = static_cast<float>(random(10) + 20) / 3.0f;
                        ySpeed = -xSpeed - (static_cast<float>(random(40) - 20) / 15.0f);
                    } break;

                    case SMOKE3_R:  // rechts
                    {
                        xSpeed = static_cast<float>(random(10) + 20) / 2.0f;
                        ySpeed = -(static_cast<float>(random(40) - 20) / 15.0f);
                    } break;

                    case SMOKE3_RU:  // rechts oben
                    {
                        xSpeed = static_cast<float>(random(10) + 20) / 3.0f;
                        ySpeed = xSpeed - (static_cast<float>(random(40) - 20) / 15.0f);
                    } break;

                    case SMOKE3_U:  // unten
                    {
                        xSpeed = (static_cast<float>(random(40) - 20) / 15.0f);
                        ySpeed = static_cast<float>(random(10) + 20) / 2.0f;
                    } break;

                    case SMOKE3_LU:  // links oben
                    {
                        xSpeed = -static_cast<float>(random(10) + 20) / 3.0f;
                        ySpeed = -xSpeed - (static_cast<float>(random(40) - 20) / 15.0f);
                    } break;

                    case SMOKE3_L:  // links
                    {
                        xSpeed = -static_cast<float>(random(10) + 20) / 2.0f;
                        ySpeed = -(static_cast<float>(random(40) - 20) / 15.0f);
                    } break;

                    case SMOKE3_LO:  // links oben
                    {
                        xSpeed = -static_cast<float>(random(10) + 20) / 3.0f;
                        ySpeed = xSpeed - (static_cast<float>(random(40) - 20) / 15.0f);
                    } break;
                }

                PartikelArt = SMOKE3;
                RemoveWhenOffScreen = false;
                Rotate = true;

            } break;

            case SMOKEBIG_OUTTRO:  // Riesen Rauchwolke
            {
                red = 128;
                green = 128;
                blue = 128;
                alpha = 255;

                Lebensdauer = 90;
                xSpeed = (static_cast<float>(random(40) - 20) / 6.0f);
                ySpeed = -static_cast<float>(random(10) + 10) / 5.0f;

                AnimPhase = random(4);
                Rotate = true;

            } break;

            case SMOKEBIG:  // Riesen Rauchwolke
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 140;

                Lebensdauer = 140;
                xSpeed = (static_cast<float>(random(40) - 20) / 10.0f);
                ySpeed = -static_cast<float>(random(20) + 10) / 10.0f;

                AnimPhase = random(4);
                Rotate = true;

            } break;

            case SMOKEBIG2:  // Riesen Rauchwolke
            {
                red = 128;
                green = 128;
                blue = 128;
                alpha = 200;

                Lebensdauer = 200;
                xSpeed = static_cast<float>(random(6) - 3) * 6.0f;
                ySpeed = static_cast<float>(random(10) - 5) * 2.0f;
                yAcc = -1.0f;

                AnimPhase = random(4);
                Rotate = true;

            } break;

            case FLOATSMOKE:  // Rauch der kleinen floating Plattform
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 144;
                xSpeed = static_cast<float>(random(40) - 20) / 40.0f;
                ySpeed = static_cast<float>(random(10) + 40) / 5.0f;
                Lebensdauer = static_cast<float>(180 + random(10));
                PartikelArt = ROCKETSMOKE;

            } break;

            case DUST: {
                if (random(2) == 0) {
                    red = 255;
                    green = 224;
                    blue = 128;
                    alpha = 255;
                    yAcc = -0.8f;
                } else {
                    red = 255;
                    green = 192;
                    blue = 64;
                    alpha = 255;
                    yAcc = 0.8f;
                }

                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = random(3);

                xSpeed = -5.0f * (AnimPhase + 1);
            } break;

            case SCHROTT1: {
                // DKS - off-by-one error:
                // AnimEnde = 20;
                AnimEnde = 19;
                AnimSpeed = (random(10) + 10) / 20.0f;

                xSpeed = static_cast<float>(random(80) - 40) / 4.0f;
                ySpeed = -static_cast<float>(random(10) + 8);
                xAcc = 0.0f;
                yAcc = 3.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;

                if (random(2) == 0)
                    PartikelArt = SCHROTT2;
            } break;

            case SCHNEEFLOCKE:  // Schneeflocke
            case SCHNEEFLOCKE_END: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 60;
                xSpeed = 0.0f;

                if (random(2) == 0)  // Per Zufall nach links
                    xAcc = 0.2f;
                else  // oder rechts
                    xAcc = -0.2f;

                AnimPhase = random(3);
                ySpeed = static_cast<float>(random(10) + 15) / 10.0f;
                ySpeed *= (AnimPhase + 1);

                if (PartikelArt == SCHNEEFLOCKE_END) {
                    xSpeed /= 2.0f;
                    ySpeed /= 2.0f;
                }

                Lebensdauer = 60;
                RemoveWhenOffScreen = false;
            } break;

            case BOULDER_SMALL:  // kleiner blauer Boulder
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = static_cast<float>(random(80) - 40) / 3.0f;
                ySpeed = -(static_cast<float>(random(80) - 20) / 3.0f);
                xAcc = 0.0f;
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                // DKS - off-by-one error:
                // AnimEnde	= 15;
                AnimEnde = 14;
                AnimSpeed = static_cast<float>(random(10) + 5) / 20.0f;

            } break;

            case LAVAKRABBE_KOPF:  // Kopf der Lava Krabbe
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = static_cast<float>(random(80) - 40) / 3.0f;
                ySpeed = -(static_cast<float>(random(60) + 20) / 2.0f);
                xAcc = 0.0f;
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                Rotate = true;
                Rot = static_cast<float>(random(360));

            } break;

            case LAVAKRABBE_BEIN:  // Bein der Lava Krabbe
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = static_cast<float>(random(80) - 40) / 3.0f;
                ySpeed = -(static_cast<float>(random(60) + 20) / 2.0f);
                xAcc = 0.0f;
                yAcc = 5.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = true;
                Rotate = true;
                Rot = static_cast<float>(random(360));
                AnimPhase = 1;
                PartikelArt = LAVAKRABBE_KOPF;

            } break;

            case SPIDERPARTS:  // Spinnenstücke auf dem Fliessband
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = random(4);
                RemoveWhenOffScreen = false;

            } break;

            case KETTENTEILE: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = 0;

                xSpeed = (static_cast<float>(random(80) - 40) / 3.0f);
                ySpeed = -(static_cast<float>(random(30) + 10) / 2.0f);
                xAcc = 0.0f;
                yAcc = 5.0f;
                BounceWalls = true;
                Rotate = true;
                Rot = static_cast<float>(random(360));

            } break;

            case KETTENTEILE2: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = 1;

                xSpeed = (static_cast<float>(random(80) - 40) / 3.0f);
                ySpeed = -(static_cast<float>(random(30) + 10) / 2.0f);
                xAcc = 0.0f;
                yAcc = 5.0f;
                BounceWalls = true;
                Rotate = true;
                Rot = static_cast<float>(random(360));

                PartikelArt = KETTENTEILE;

            } break;

            case KETTENTEILE3: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = 2;

                xSpeed = (static_cast<float>(random(80) - 40) / 3.0f);
                ySpeed = -(static_cast<float>(random(30) + 10) / 2.0f);
                xAcc = 0.0f;
                yAcc = 5.0f;
                BounceWalls = true;
                Rotate = true;
                Rot = static_cast<float>(random(360));

                PartikelArt = KETTENTEILE;

            } break;

            case KETTENTEILE4: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant

                xSpeed = (static_cast<float>(random(80) - 40) / 3.0f);
                ySpeed = -(static_cast<float>(random(30) + 10) / 2.0f);
                xAcc = 0.0f;
                yAcc = 5.0f;
                BounceWalls = true;
                Rotate = true;
                Rot = static_cast<float>(random(360));

            } break;

            case WASSER_SPRITZER:  // Wasserspritzer, wenn der Spieler aus dem Wasser hopst
            {
                static int count = 0;

                if (count == 0) {
                    red = 255;
                    green = 255;
                    blue = 255;
                    count++;
                } else {
                    count = 0;
                    red = TileEngine.ColR3;
                    green = TileEngine.ColG3;
                    blue = TileEngine.ColB3;
                }

                if (random(2) == 0)
                    Lebensdauer = static_cast<float>(random(128) + 128);
                else
                    Lebensdauer = static_cast<float>(random(32) + 224);

                AnimSpeed = static_cast<float>(random(20) + 30);
                AnimCount = static_cast<float>(random(25) + 5) / 10.0f;

            } break;

            case WASSER_SPRITZER2:  // Wasserspritzer, wenn der Spieler ins Wasser hopst
            {
                static int count = 0;

                if (count == 0) {
                    red = 255;
                    green = 255;
                    blue = 255;
                    count++;
                } else {
                    count = 0;
                    red = TileEngine.ColR3;
                    green = TileEngine.ColG3;
                    blue = TileEngine.ColB3;
                }

                if (random(2) == 0)
                    Lebensdauer = static_cast<float>(random(128) + 128);
                else
                    Lebensdauer = static_cast<float>(random(32) + 224);

                AnimSpeed = static_cast<float>(random(15) + 25);
                AnimCount = static_cast<float>(random(25) + 5) / 8.0f;

            } break;

            case LAVA_SPRITZER:  // LavaSpritzer beim raushopsen
            {
                if (random(3) == 0) {
                    red = 255;
                    green = 80;
                    blue = 32;
                    alpha = 255;
                } else {
                    red = 255;
                    green = 180;
                    blue = 64;
                    alpha = 255;
                }

                // Lebensdauer = 255;      //DKS - now redundant

                AnimSpeed = static_cast<float>(random(20) + 30);
                AnimCount = static_cast<float>(random(25) + 5) / 10.0f;

                PartikelArt = WASSER_SPRITZER;

            } break;

            case LAVA_SPRITZER2:  // LavaSpritzer beim reinhopsen
            {
                if (random(3) == 0) {
                    red = 255;
                    green = 80;
                    blue = 32;
                    alpha = 255;
                } else {
                    red = 255;
                    green = 180;
                    blue = 64;
                    alpha = 255;
                }

                // Lebensdauer = 255;      //DKS - now redundant

                AnimSpeed = static_cast<float>(random(15) + 25);
                AnimCount = static_cast<float>(random(25) + 5) / 8.0f;

                PartikelArt = WASSER_SPRITZER2;

            } break;

            case HURRITEILE_ARM1:
            case HURRITEILE_ARM2:
            case HURRITEILE_BEIN1:
            case HURRITEILE_BEIN2:
            case HURRITEILE_KOPF:
            case HURRITEILE_WAFFE:
            case HURRITEILE_TORSO: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = PartikelArt - HURRITEILE_ARM1;
                xSpeed = (static_cast<float>(random(80) - 40) / 4.0f);
                ySpeed = -15.0f * (AnimPhase + 2) / 6.0f;
                yAcc = 2.0f;
                BounceWalls = true;
                PartikelArt = HURRITEILE;

                Rotate = true;

            } break;

            // DKS - Player 2 sprite is blue, so I added separate particles and particle art for them
            //      that are colored blue, using some unused space between particles 86-100
            case HURRITEILE_P2_ARM1:
            case HURRITEILE_P2_ARM2:
            case HURRITEILE_P2_BEIN1:
            case HURRITEILE_P2_BEIN2:
            case HURRITEILE_P2_KOPF:
            case HURRITEILE_P2_WAFFE:
            case HURRITEILE_P2_TORSO: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = PartikelArt - HURRITEILE_P2_ARM1;
                xSpeed = (static_cast<float>(random(80) - 40) / 4.0f);
                ySpeed = -15.0f * (AnimPhase + 2) / 6.0f;
                yAcc = 2.0f;
                BounceWalls = true;
                PartikelArt = HURRITEILE_P2;

                Rotate = true;

            } break;

            case REGENTROPFEN:  // Regentropfen
            {
                red = 220;
                green = 240;
                blue = 255;
                alpha = 192;
                xSpeed = -24.0f;
                ySpeed = 44.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                RemoveWhenOffScreen = false;
            } break;

            default:
                break;

        }  // switch < Additiv Grenze

    else

        switch (Art) {
            case EVILROUNDSMOKE:  // Rauch des Rundumschusses des evil hurri
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(60) - 30) / 10.0f);
                ySpeed = (static_cast<float>(random(60) - 30) / 10.0f);

                Lebensdauer = 180;
                BounceWalls = false;
            } break;

            case BEAMSMOKE:  // Rauch des Blitzbeams
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(20) - 10) / 20.0f);
                ySpeed = (static_cast<float>(random(20) - 10) / 20.0f);

                AnimCount = 30.0f;
                Lebensdauer = 140;
                BounceWalls = false;

                OwnDraw = true;  // eigene Draw-Routine, da er die Größe ändert
                Rotate = true;
                RotDir = 10.0f;
                Rot = static_cast<float>(random(360));
            } break;

            case BEAMSMOKE2:  // Rauch beim Aufladen des Blitzbeams
            {
                // DKS - converted to float:
                float absx, absy, speed;  // Variablen für die Geschwindigkeits-

                AnimPhase = random(3);

                if (m_pParent != nullptr) {
                    // berechnung
                    absx = m_pParent->BeamX - xPos;  // Differenz der x
                    absy = m_pParent->BeamY - yPos;  // und y Strecke

                    // DKS - converted to float:
                    speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                    speed = speed * static_cast<float>(4 + AnimPhase * 2);           // Geschwindigkeit

                    absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                    absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                    xSpeed = absx;
                    ySpeed = absy;
                } else {
                    xSpeed = static_cast<float>(random(20) - 10) / 2.0f;
                    ySpeed = static_cast<float>(random(20) - 10) / 2.0f;
                }

                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant
            } break;

            case BEAMSMOKE3:  // Rauch des Blitzbeams beim Explodieren
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                AnimPhase = random(3);

                float mul = static_cast<float>(random(100) + 10) / 10.0f;

                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // float arc = static_cast<float>(rand()%360) * PI / 180.0f;
                // xSpeed		= static_cast<float>(sin(arc) * mul);
                // ySpeed		= static_cast<float>(cos(arc) * mul);
                int arc = random(360);
                xSpeed = sin_deg(arc) * mul;
                ySpeed = cos_deg(arc) * mul;

                Lebensdauer = static_cast<float>(random(200) + 55);
            } break;

            case BEAMSMOKE4:  // Rauch des Blitzbeams
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                float mul = static_cast<float>(random(50) + 10) / 10.0f;

                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // float arc = static_cast<float>(rand()%360) * PI / 180.0f;
                // xSpeed		= static_cast<float>(sin(arc) * mul);
                // ySpeed		= static_cast<float>(cos(arc) * mul);
                int arc = random(360);
                xSpeed = sin_deg(arc) * mul;
                ySpeed = cos_deg(arc) * mul;

                Rotate = true;
                RotDir = static_cast<float>(random(10) + 15);
                Lebensdauer = static_cast<float>(random(200) + 55);
            } break;

            case BEAMSMOKE5:  // Druckwelle beim Beam Explodieren
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                if (static_cast<int>(xPos) % 2 == 0)
                    AnimPhase = 20;
                else
                    AnimPhase = 40;

                // Lebensdauer = 255;      //DKS - now redundant
                OwnDraw = true;
            } break;

            case SNOWFLUSH:  // SchneeGestöber und Wasserfall Rauch
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                xSpeed = (static_cast<float>(random(60) - 30) / 20.0f);
                ySpeed = (static_cast<float>(random(60) - 30) / 20.0f);
                yAcc = 0.5f;

                Lebensdauer = 140;
                BounceWalls = false;
                Rotate = true;

            } break;

            case WATERFLUSH:  // Wasserfall Dampfwolken
            {
                static int count = 0;

                if (count == 0) {
                    red = 255;
                    green = 255;
                    blue = 255;
                    count++;
                } else {
                    count = 0;
                    red = TileEngine.ColR3;
                    green = TileEngine.ColG3;
                    blue = TileEngine.ColB3;
                }

                xSpeed = (static_cast<float>(random(80) - 40) / 20);
                ySpeed = (static_cast<float>(random(60) - 10) / 20);
                yAcc = 0.5f;

                Lebensdauer = 75;
                Rotate = true;

            } break;

            case WATERFLUSH2:  // Surf Dampfwolken
            {
                static int count = 0;

                if (count == 0) {
                    red = 255;
                    green = 255;
                    blue = 255;
                    count++;
                } else {
                    count = 0;
                    red = TileEngine.ColR3;
                    green = TileEngine.ColG3;
                    blue = TileEngine.ColB3;
                }

                xSpeed = (static_cast<float>(random(80) - 40) / 20.0f);
                ySpeed = -(static_cast<float>(random(40) + 40) / 20.0f);
                yAcc = 1.0f;

                BounceWalls = true;
                Lebensdauer = 75;
                PartikelArt = WATERFLUSH;
                Rotate = true;

            } break;

            case WATERFLUSH_HIGH:  // Wasserminen Dampfwolken
            {
                static int count = 0;

                if (count == 0) {
                    red = 255;
                    green = 255;
                    blue = 255;
                    count++;
                } else {
                    count = 0;
                    red = TileEngine.ColR3;
                    green = TileEngine.ColG3;
                    blue = TileEngine.ColB3;
                }

                xSpeed = (static_cast<float>(random(80) - 40) / 2.0f);
                ySpeed = -(static_cast<float>(random(100) + 100) / 5.0f);
                yAcc = 4.0f;

                Lebensdauer = 128;
                PartikelArt = SNOWFLUSH;
                Rotate = true;

            } break;

            case WATERFLUSH_HIGH2:  // Wasserspritzer Dampfwolken
            {
                static int count = 0;

                if (count == 0) {
                    red = 255;
                    green = 255;
                    blue = 255;
                    count++;
                } else {
                    count = 0;
                    red = TileEngine.ColR3;
                    green = TileEngine.ColG3;
                    blue = TileEngine.ColB3;
                }

                xSpeed = (static_cast<float>(random(40) - 20) / 4.0f);
                ySpeed = -(static_cast<float>(random(50) + 50) / 3.0f);
                yAcc = 6.0f;

                Lebensdauer = 128;
                PartikelArt = SNOWFLUSH;
                Rotate = true;

            } break;

            case UFOLASERFLARE:  // Flare des Ufo Lasers
            {
                red = 255;
                green = 64;
                blue = 224;
                alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
            } break;

            case FUNKE:  // Roter Funken
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                xSpeed = (static_cast<float>(random(80) - 40) / 5.0f);
                ySpeed = -static_cast<float>(random(40) + 20) / 5.0f;
                yAcc = 5.0f;
                BounceWalls = true;

                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case FUNKE2:  // Grüner Funken
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                xSpeed = (static_cast<float>(random(80) - 40) / 5.0f);
                ySpeed = -static_cast<float>(random(40) + 20) / 5.0f;
                yAcc = 5.0f;
                BounceWalls = true;

                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case LASERFUNKE:  // Laser-Funken blau
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                xSpeed = (static_cast<float>(random(80) - 40) / 2.0f);
                ySpeed = -static_cast<float>(random(40) + 20) / 5.0f;
                yAcc = 5.0f;
                BounceWalls = true;

                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case LASERFUNKE2:  // Laser-Funken rot
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                xSpeed = (static_cast<float>(random(20) - 10) / 2.0f);
                ySpeed = -static_cast<float>(random(40) + 20) / 5.0f;
                yAcc = 5.0f;
                BounceWalls = true;
                // DKS - off-by-one error:
                // AnimEnde	= 3;
                AnimEnde = 2;
                AnimSpeed = 0.75f;

                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case PHARAOSMOKE:  // Roter Rauch für den Pharaokopf schuss
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                xSpeed = (static_cast<float>(random(80) - 40) / 40.0f);
                ySpeed = -static_cast<float>(random(80) - 40) / 40.0f;
                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case ROCKETSMOKE:       // Rauch einer Rakete
            case ROCKETSMOKEBLUE:   // auch in blau =)
            case ROCKETSMOKEGREEN:  // und das selbe nochmal in grün :D
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 150;
                xSpeed = (static_cast<float>(random(40) - 20) * 0.025f);
                ySpeed = -static_cast<float>(random(40) - 20) * 0.025f;
                Lebensdauer = 200;

            } break;

            case FLUGSACKSMOKE:  // Rauch des FlugSacks links
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;
                xSpeed = -(8.0f + (static_cast<float>(random(20)) / 10.0f));
                ySpeed = 15.0f;
                Lebensdauer = static_cast<float>(150 + random(10));

                AnimSpeed = 0.8f;
                // DKS - off-by-one error:
                // AnimEnde  = 16;
                AnimEnde = 15;

                BounceWalls = true;
            } break;

            case FLUGSACKSMOKE2:  // Rauch des FlugSacks rechts
            {
                PartikelArt = FLUGSACKSMOKE;
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;
                xSpeed = 8.0f + (static_cast<float>(random(20)) / 10.0f);
                ySpeed = 15.0f;
                Lebensdauer = static_cast<float>(150 + random(10));

                AnimSpeed = 0.8f;
                // DKS - off-by-one error:
                // AnimEnde  = 16;
                AnimEnde = 15;
                BounceWalls = true;
            } break;

            case ROBOMANSMOKE:  // Rauch des RoboMans
            {
                PartikelArt = FLUGSACKSMOKE;
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;
                ySpeed = 15.0f + (static_cast<float>(random(20)) / 5.0f);
                Lebensdauer = static_cast<float>(150 + random(10));

                AnimSpeed = 0.8f;
                // DKS - off-by-one error:
                // AnimEnde  = 16;
                AnimEnde = 15;
                BounceWalls = true;
            } break;

            case STELZFLARE:  // Flare des Stelzsack smokes
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;
            } break;

            case TEXTSECRET: {
                // Lebensdauer = 255;      //DKS - now redundant
                OwnDraw = true;
                ySpeed = -1.0f;
            } break;

            case KRINGELSECRET:  // Secret Kringel
            {
                red = 255;
                green = 224;
                blue = 32;
                alpha = 255;
                AnimPhase = random(4);
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;

                float mul = static_cast<float>(random(60) + 40) / 8.0f;

                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // float arc = static_cast<float>(rand()%360) * PI / 180.0f;
                // xSpeed		= static_cast<float>(sin(arc) * mul);
                // ySpeed		= static_cast<float>(cos(arc) * mul);
                int arc = random(360);
                xSpeed = sin_deg(arc) * mul;
                ySpeed = cos_deg(arc) * mul;

                xPos += xSpeed;
                yPos += ySpeed;
            } break;

            case KRINGELR:  // Roter Kringel
            {
                red = 255;
                green = 64;
                blue = 0;
                alpha = 255;
                AnimPhase = random(4);
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;
                PartikelArt = KRINGEL;

                // DKS - converted to float:
                // berechnung
                float absx = m_pParent->xpos + 35 - (xPos + 4);  // Differenz der x
                float absy = m_pParent->ypos + 40 - (yPos + 4);  // und y Strecke

                // DKS - converted to float:
                float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                speed = speed * static_cast<float>(8 + AnimPhase);                  // Geschwindigkeit ist 4 fach

                absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                xSpeed = absx;
                ySpeed = absy;
            } break;

            case KRINGELG:  // Grüner Kringel
            {
                red = 64;
                green = 255;
                blue = 0;
                alpha = 255;
                AnimPhase = random(4);
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;
                PartikelArt = KRINGEL;

                // DKS - converted to float:
                // berechnung
                float absx = m_pParent->xpos + 35 - (xPos + 4);  // Differenz der x
                float absy = m_pParent->ypos + 40 - (yPos + 4);  // und y Strecke

                // DKS - converted to float:
                float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                speed = speed * static_cast<float>(8 + AnimPhase);                  // Geschwindigkeit ist 4 fach

                absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                xSpeed = absx;
                ySpeed = absy;
            } break;

            case KRINGELB:  // Blauer Kringel
            {
                red = 24;
                green = 48;
                blue = 255;
                alpha = 255;
                AnimPhase = random(4);
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;
                PartikelArt = KRINGEL;

                // DKS - converted to float:
                // berechnung
                float absx = m_pParent->xpos + 35 - (xPos + 4);  // Differenz der x
                float absy = m_pParent->ypos + 40 - (yPos + 4);  // und y Strecke

                // DKS - converted to float:
                float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                speed = speed * (8 + AnimPhase);                  // Geschwindigkeit ist 4 fach

                absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                xSpeed = absx;
                ySpeed = absy;
            } break;

            case KRINGELHB:  // Hellblauer Kringel
            {
                red = 64;
                green = 192;
                blue = 255;
                alpha = 255;
                AnimPhase = random(4);
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;
                PartikelArt = KRINGEL;

                // DKS - converted to float:
                // berechnung
                float absx = m_pParent->xpos + 35 - (xPos + 4);  // Differenz der x
                float absy = m_pParent->ypos + 40 - (yPos + 4);  // und y Strecke

                // DKS - converted to float:
                float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                speed = speed * (8 + AnimPhase);                  // Geschwindigkeit ist 4 fach

                absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                xSpeed = absx;
                ySpeed = absy;
            } break;

            case LONGFUNKE:  // Langer Roter Funken
            {
                red = 255;
                green = 192;
                blue = 24;
                alpha = 255;
                xSpeed = (static_cast<float>(random(100) - 50) / 2.0f);
                ySpeed = -static_cast<float>(random(40) + 40) / 2.0f;
                yAcc = 5.0f;
                BounceWalls = true;
                OwnDraw = true;

                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case WATERFUNKE:  // Wasserspritzer
            {
                red = 255;
                green = 192;
                blue = 24;
                alpha = 255;

                xSpeed = (static_cast<float>(random(50) - 25) * 0.4f);
                ySpeed = -static_cast<float>(random(30) + 60) * 0.15f;
                yAcc = 5.0f;
                BounceWalls = true;
                OwnDraw = true;

                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case EVILFUNKE:  // Funke des Evil Blitzes
            {
                red = 255;
                green = 255;
                blue = 255;
                alpha = 128;
                ySpeed = 8.0f + (static_cast<float>(random(20)) / 10.0f);
                Lebensdauer = static_cast<float>(150 + random(10));

                BounceWalls = true;
            } break;

            case WASSERTROPFEN:  // Tropfen
            {
                red = 230;
                green = 240;
                blue = 255;
                alpha = 128;

                Lebensdauer = 128;
                ySpeed = -static_cast<float>(random(10) + 2) / 2.0f;
                xSpeed = static_cast<float>(random(20) - 10) / 2.0f;
                yAcc = 4.0f;
            } break;

            case BUBBLE:  // Luftbläschen im Wasser
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                xSpeed = 0.0f;

                if (random(2) == 0)
                    xAcc = 0.1f;
                else
                    xAcc = -0.1f;

                AnimCount = static_cast<float>(random(10) + 10) / 10.0f;

                ySpeed = -static_cast<float>(random(10) + 20) / 10.0f;

                if (WinkelUebergabe == -1.0f) {
                    ySpeed *= -3.0f;
                    yAcc = -1.0f;
                }

                // Lebensdauer = 255;      //DKS - now redundant
                RemoveWhenOffScreen = false;
            } break;

            case LASERFLAME:  // Leuchteffekt für den Krabbler
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant

            } break;

            case LASERFLAMEPHARAO:  // Leuchteffekt für den Pharao
            {
                red = 255;
                green = 64;
                blue = 64;
                alpha = 255;
                Lebensdauer = 192.0f;
                PartikelArt = LASERFLAME;

            } break;

            case SHIELD:  // Schutzschild
            {
                // Schild farbliche an die aktuelle Stärke anpassen
                // Schild voll == hellblau, Schild leer == rot
                float amount = m_pParent->Shield;

                if (amount > 100.0f)
                    amount = 100.0f;

                // DKS - Use new color-setting functions to check range:
                // red   = 255 - static_cast<int>(amount*2.55f);
                // blue  = 	  static_cast<int>(amount*2.55f);
                // if (red < 0)	 red = 0;
                // if (red > 255)	 red = 255;
                // if (blue < 0)    blue = 0;
                // if (blue > 255)  blue = 255;
                SetRed(255 - static_cast<int>(amount * 2.55f));
                SetBlue(amount * 2.55f);
                green = static_cast<int>(blue / 2);

                alpha = 128;
                Lebensdauer = 128.0f;

                // Offset zum Spieler merken
                xSpeed = m_pParent->xpos - xPos;
                ySpeed = m_pParent->ypos - yPos;

            } break;

            case TURBINESMOKE:  // Partikel für die Turbine des Metalhead Bosses
            {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                AnimPhase = random(3);
                // Lebensdauer = 255;      //DKS - now redundant
                BounceWalls = false;

                // DKS - converted to float:
                // berechnung
                float absx = PartikelSystem.xtarget - xPos;  // Differenz der x
                float absy = PartikelSystem.ytarget - yPos;  // und y Strecke

                // DKS - converted to float:
                float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                speed = speed * (8 + AnimPhase);                  // Geschwindigkeit ist 4 fach

                absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                xSpeed = absx;
                ySpeed = absy;
            } break;

            case MINIFLARE:  // Flare vom Lava Ball
            {
                red = 255;
                green = 224;
                blue = 192;
                alpha = 192;
                Lebensdauer = 192;
                BounceWalls = false;

                ySpeed = 2.0f;
                yAcc = 2.0f;
                xSpeed = static_cast<float>(random(10) + 10) / 2.0f;
                ySpeed = -static_cast<float>(random(10) + 10);

                if (random(2) == 0)
                    xSpeed *= -1;
            } break;

            case GRENADEFLARE:  // Aufleuchten bei Granaten Treffer
            {
                red = 255;
                green = 224;
                blue = 192;
                alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
                Rotate = true;
            } break;

            case EXPLOSIONFLARE:  // Aufleuchten bei Explosion
            {
                red = 255;
                green = 224;
                blue = 112;
                alpha = 192;
                Lebensdauer = 192;
            } break;

            case EXPLOSIONFLARE2:  // Aufleuchten bei Explosion Lila
            {
                red = 255;
                green = 128;
                blue = 255;
                alpha = 192;
                Lebensdauer = 192;
            } break;

            case SCHLEIM:  // kleiner Schleimbollen
            case SCHLEIM2: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 224;
                xSpeed = (static_cast<float>(random(80) - 40) / 3.0f);
                ySpeed = -static_cast<float>(random(40) + 40) / 3.0f;
                yAcc = 5.0f;
                BounceWalls = true;
                // DKS - off-by-one error:
                // AnimEnde    = 4;
                AnimEnde = 3;
                AnimSpeed = static_cast<float>((random(2) + 3) / 10.0f);

                // Lebensdauer = 255;      //DKS - now redundant
            } break;

            case SHOCKEXPLOSION: {
                red = 255;
                green = 192;
                blue = 64;
                alpha = 255;
                AnimCount = 4.0f;
                // Lebensdauer = 255;      //DKS - now redundant
                OwnDraw = true;
            } break;

            case SHOTFLARE: {
                red = 255;
                green = 128;
                blue = 32;
                alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
            } break;

            case SHOTFLARE2: {
                red = 32;
                green = 255;
                blue = 32;
                alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
            } break;

            case EXTRACOLLECTED: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant

                OwnDraw = true;
                AnimCount = 1.0f;
                AnimPhase = 1;

            } break;

            case EXPLOSION_KRINGEL:  // Kringel/Druckwelle bei Explosion
            {
                red = 255;
                green = 240;
                blue = 128;
                alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant

                OwnDraw = true;
                AnimCount = 5.0f;
                AnimPhase = 2;

                if (WinkelUebergabe == -1.0f)
                    AnimPhase = 2;

                PartikelArt = EXTRACOLLECTED;

            } break;

            case DIAMANTCOLLECTED: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant

                OwnDraw = true;
                AnimCount = 16.0f;

            } break;

            case LILA: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
            } break;

            case LILA2: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant

                ySpeed = -static_cast<float>(random(30) + 10) / 2.0f;
                xSpeed = static_cast<float>(random(40) - 20) / 2.0f;
                yAcc = 5.0f;
                PartikelArt = LILA;
            } break;

            case LILA3: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant

                xSpeed = -static_cast<float>(random(20) + 120);
                PartikelArt = LILA;
            } break;

            case DRACHE_SMOKE: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;
                // Lebensdauer = 255;      //DKS - now redundant
                xSpeed = -static_cast<float>(random(20) + 120) / 4.0f;
                ySpeed = static_cast<float>(random(20) - 10) / 5.0f;

                if (WinkelUebergabe != 0.0f)
                    xSpeed *= -1;

                Rotate = true;
            } break;

            case FIREBALL_SMOKE: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255;
                alpha = 128;
                // Lebensdauer = 255;      //DKS - now redundant
                xSpeed = static_cast<float>(random(10) - 5) / 10.0f;
                ySpeed = static_cast<float>(random(10) - 5) / 10.0f;

                Rotate = true;
            } break;

            case LASERFLARE: {
                red = 32;
                green = 192;
                blue = 255;
                alpha = 255;
                // Lebensdauer = 255;      //DKS - now redundant
            } break;

            case EXPLOSION_TRACE_END: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 150;
                AnimEnde = 17;
                AnimPhase = 5;
                AnimSpeed = 3.0f;

                // DKS - Use new range-checked SetBlue() function:
                // blue = static_cast<int>(WinkelUebergabe);
                SetBlue(static_cast<int>(WinkelUebergabe));
                OwnDraw = true;

                Rotate = true;

            } break;

            case LAVADUST: {
                red = 255;
                green = 128;
                blue = 64;
                alpha = 255;

                // ???
                if (random(2) == 0)
                    xAcc = -0.4f;
                else
                    xAcc = -0.4f;

                // Lebensdauer = 255;      //DKS - now redundant
                AnimPhase = random(3);

                ySpeed = -3.0f * (AnimPhase + 1);
                RemoveWhenOffScreen = false;
            } break;

            case EXPLOSION_TRACE: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                // Lebensdauer = 255;      //DKS - now redundant

                xPos -= 60;
                yPos -= 60;

                xSpeed = (static_cast<float>(random(80) - 40));
                ySpeed = -static_cast<float>(random(10) + 50);
                yAcc = 8.0f;

            } break;

            case EXPLOSION_REGULAR: {
                // DKS - now redundant:
                // red	= 255; green = 255; blue = 255; alpha = 255;

                Lebensdauer = 128;
                AnimEnde = 17;
                AnimSpeed = 0.75f;

                Rotate = true;
            } break;

            default:
                break;
        }  // switch >= Additiv Grenze

    // return true;  //DKS - Function now has void return type
}

// --------------------------------------------------------------------------------------
// Partikel animieren und bewegen
// --------------------------------------------------------------------------------------

void PartikelClass::Run() {

    // DKS - alpha color was getting set outside its range, so set
    //      a temp value and fix at the end:
    int tmp_alpha = alpha;

    // Screen verlassen oder eh schon weg wegen der Lebensdauer ?
    //
    if (RemoveWhenOffScreen) {
        if (yPos - TileEngine.YOffset > 480 + 20 || yPos - TileEngine.YOffset + PartikelRect[PartikelArt].bottom < 20 ||
            xPos - TileEngine.XOffset > 640 + 20 || xPos - TileEngine.XOffset + PartikelRect[PartikelArt].right < 20)
            Lebensdauer = 0.0f;
    }

    // wird diesen Frame gelöscht? Dann nicht mehr animieren und rendern
    //
    if (Lebensdauer <= 0.0f)
        return;

    // Bewegen (nur wenn sichtbar)
    if (alpha > 0) {
        xSpeed += Timer.sync(xAcc);
        ySpeed += Timer.sync(yAcc);
        xPos += Timer.sync(xSpeed);
        yPos += Timer.sync(ySpeed);
    }

    uint32_t bo, bu, bl, br;

    if (BounceWalls) {
        bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
        bl = TileEngine.BlockLinks(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
        br = TileEngine.BlockRechts(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
        bu = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
    }

    else {
        bo = bl = br = 0;
        bu = TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
    }

    // Auf Kollision mit dem Level Testen
    if (BounceWalls) {
        // Testen,ob der Partikel sich in einer Wand verfangen hat, wenn ja
        // dann verschwinden lassen

        if ((bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && bl & BLOCKWERT_WAND) ||
            (bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && br & BLOCKWERT_WAND))
            Lebensdauer = 0.0f;

        // An der Decke abgeprallt?
        if (ySpeed < 0.0f && bo & BLOCKWERT_WAND)
            ySpeed *= -1.0f;

        // Am Boden abgesprungen?
        if (ySpeed > 0.0f && (bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM)) {
            ySpeed = -ySpeed / 2.0f;

            if (ySpeed > -0.5f) {
                ySpeed = 0.0f;
                xSpeed *= 0.99f;
            }
        }

        if ((xSpeed > 0.0f && br & BLOCKWERT_WAND) || (xSpeed < 0.0f && bl & BLOCKWERT_WAND))
            xSpeed *= -1.0f;
    }

    // Animieren
    if (AnimEnde > 0)  // Soll überhaupt anmiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;  // Dann wieder auf Null setzen
            AnimPhase++;    // Und nächste Animationsphase
        }
    }

    //----- Verschiedene Partikel unterschiedlich abhandeln
    // Zwei Case Anweisungen, einmal für nicht additive und einmal für additive Partikel
    // Wird dadurch im Schnitt hoffentlich doppelt so schnell ;)

    if (PartikelArt < ADDITIV_GRENZE)
        switch (PartikelArt) {
            case GLASSPLITTER: {
                Rot += Timer.sync(xSpeed * 10.0f);

                Lebensdauer -= Timer.sync(20.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case BULLET_SKELETOR:
            case BULLET:  // Patronenhülse
            {
                bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
                bl = TileEngine.BlockLinks(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
                br = TileEngine.BlockRechts(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
                bu = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

                // Testen,ob der Partikel sich in einer Wand verfangen hat, wenn ja
                // dann verschwinden lassen

                if ((bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && bl & BLOCKWERT_WAND) ||
                    (bo & BLOCKWERT_WAND && bu & BLOCKWERT_WAND && br & BLOCKWERT_WAND))
                    Lebensdauer = 0.0f;

                // An der Decke abgeprallt?
                if (ySpeed < 0.0f && bo & BLOCKWERT_WAND)
                    ySpeed *= -1.0f;

                // Am Boden abgesprungen?
                if (ySpeed > 0.0f && (bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM)) {
                    ySpeed = -ySpeed / 2.0f;
                    xSpeed *= 0.5f;

                    if (ySpeed > -0.5f) {
                        yAcc = 0.0f;
                        ySpeed = 0.0f;
                        xSpeed = 0.0f;
                    }
                }

                if ((xSpeed > 0.0f && br & BLOCKWERT_WAND) || (xSpeed < 0.0f && bl & BLOCKWERT_WAND))
                    xSpeed *= -1.0f;

                Rot += Timer.sync(xSpeed * 10.0f);

                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

            } break;

            case EXPLOSION_GREEN:    // Mittlere Grüne Explosion
            case EXPLOSION_ALIEN:    // Mittlere Alien Explosion
            case EXPLOSION_MEDIUM:   // Mittlere Explosion
            case EXPLOSION_MEDIUM2:  // Mittlere Explosion Nr 2
            case EXPLOSION_MEDIUM3:  // Mittlere Explosion Nr 3
            case EXPLOSION_GIGA:     // Riesen fette Explosion
            case EXPLOSION_BIG:      // Grosse Explosion
            case EXPLOSION_GIANT:    // Grosse Explosion
            {
                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                if (AnimPhase > AnimEnde)  // Animation zu Ende	?
                    Lebensdauer = 0;       // Dann Explosion verschwinden lassen

                if (Rotate)
                    Rot += Timer.sync(RotDir * AnimSpeed * 10.0f);

            } break;

            case BLUE_EXPLOSION:  // Kleine blaue Explosion
            {
                Lebensdauer -= Timer.sync(12.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                if (AnimPhase > AnimEnde)  // Animation zu Ende	?
                    Lebensdauer = 0;       // Dann Explosion verschwinden lassen

            } break;

            case SPLITTER:  // Kleiner animierter Splitter
            {
                Lebensdauer -= Timer.sync(7.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                if (AnimPhase > AnimEnde)  // Animation zu Ende	?
                    AnimPhase = 0;         // Dann von vorne beginnen lassen

                if (AnimCount == 0.0f)
                    PartikelSystem.PushPartikel(xPos - 12, yPos - 14, SMOKE);

            } break;

            case HURRITEILE:     // Teile des explodierten Hurris
            case HURRITEILE_P2:  // DKS - Added blue-colored hurrican piece particles for player 2
            {
                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Partikel drehen
                Rot += Timer.sync(static_cast<float>(AnimPhase + 1) * 8.0f);

            } break;

            case LAVAKRABBE_KOPF:  // Teile der explodierten Lavakrabbe
            {
                Lebensdauer -= Timer.sync(10.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Partikel drehen
                Rot += Timer.sync(32.0f);

            } break;

            case KETTENTEILE:
            case KETTENTEILE2:
            case KETTENTEILE3:
            case KETTENTEILE4: {
                Lebensdauer -= Timer.sync(10.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Partikel drehen
                Rot += Timer.sync(40.0f);

            } break;

            case SPIDERPARTS:  // Spinnenstücke auf dem Fliessband
            {
                bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
                bl = TileEngine.BlockLinks(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
                br = TileEngine.BlockRechts(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);
                bu = TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

                // an der Wand abprallen oder aufhören und verschwinden
                if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND) {
                    // abprallen
                    if (ySpeed > 0.0f) {
                        if (bl & BLOCKWERT_WAND)
                            xSpeed = 11.0f;
                        else
                            xSpeed = -11.0f;
                    }

                    // oder verschwinden
                    else
                        Lebensdauer = 0.0f;
                }

                // auf dem Fliessband bewegen
                if (bu & BLOCKWERT_FLIESSBANDL)
                    xSpeed = -11.0f;

                if (bu & BLOCKWERT_FLIESSBANDR)
                    xSpeed = 11.0f;

                // runterfallen?
                if (!(bu & BLOCKWERT_WAND))
                    yAcc = 5.0f;
                else {
                    yAcc = 0.0f;
                    ySpeed = 0.0f;
                }

                // unten aus dem Screen raus? Dann verschwinden lassen
                if (yPos > TileEngine.YOffset + 500)
                    Lebensdauer = 0.0f;

            } break;

            case PIRANHATEILE:   // Teile eines kaputten Piranhas
            case PIRANHATEILE2:  // Teile eines kaputten Riesen Piranhas
            {
                bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

                if (ySpeed < -2.0f)
                    ySpeed = -2.0f;

                if (ySpeed == 0.0f)  // an der Oberfläche langsam ausfaden lassen
                    Lebensdauer -= Timer.sync(7.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);

                if ((xAcc > 0.0f && xSpeed > 0.0f) || (xAcc < 0.0f && xSpeed < 0.0f)) {
                    xAcc = 0.0f;
                    xSpeed = 0.0f;
                }

                // An der Wand Seitwärtsbewegung einstellen
                if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND) {
                    xSpeed = 0.0f;
                    xAcc = 0.0f;
                }

                // An der Decke oder der Oberfläche Aufwärtsbewegung einstellen
                if (!(bo & BLOCKWERT_LIQUID)) {
                    xSpeed = 0.0f;
                    ySpeed = 0.0f;
                    yAcc = 0.0f;
                }

            } break;

            case PIRANHABLUT:  // Blut eines kaputten Piranhas
            {
                bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Nicht mehr im Wasser ? Dann gleich verschwinden lassen
                if (!(bo & BLOCKWERT_LIQUID))
                    Lebensdauer = 0;

                // Animation von vorne beginnen
                if (AnimPhase > AnimEnde)
                    AnimPhase = 0;

                Rot += Timer.sync(5.0f);
            } break;

            case MADEBLUT:  // Blut einer kaputten Made
            {
                Lebensdauer -= Timer.sync(15.0f);  // ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Animation von vorne beginnen
                if (AnimPhase > AnimEnde)
                    AnimPhase = 0;
            } break;

            case SPAWNDROP:  // Wassertropfen aus dem Spawner
            {
                if (ySpeed > 30.0f)
                    yAcc = 0.0f;

                // Im Wasser oder auf dem Boden gelandet ?
                //
                uint32_t blocku = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

                if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM || blocku & BLOCKWERT_LIQUID) {
                    Lebensdauer = 0;

                    for (int i = 0; i < 3; i++)
                        PartikelSystem.PushPartikel(xPos, yPos, WATERFUNKE);

                    // Platschen im Wasser
                    //

                    float xdiff = ((m_pParent->xpos + 45) - xPos);
                    float ydiff = ((m_pParent->ypos + 45) - yPos);

                    // DKS - converted to float:
                    float Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

                    int vol = static_cast<int>((100.0f - Abstand / 6.0f) * 1.5f);
                    if (vol < 0)
                        vol = 0;

                    // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                    if (!SoundManager.WaveIsPlaying(SOUND::DROP))
                        SoundManager.PlayWave(vol, 128, 6000 + random(6000), SOUND::DROP);
                }

            } break;

            case BLATT: {
                if (ySpeed > 6.0f)
                    ySpeed = 6.0f;

                uint32_t bu2 = TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

                if (bu2 & BLOCKWERT_WAND || bu2 & BLOCKWERT_PLATTFORM) {
                    xSpeed = 0.0f;
                    xAcc = 0.0f;

                    Lebensdauer -= Timer.sync(10.0f);
                }

                if (xSpeed > 0.0f)
                    xAcc = -0.5f;
                else
                    xAcc = 0.5f;

                Rot += Timer.sync(RotDir * ySpeed * 8.0f);

                Lebensdauer -= Timer.sync(5.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);

            } break;

            case BLATT2: {
                Rot += Timer.sync(-xSpeed / 10.0f * RotDir * ySpeed * 8.0f);
            } break;

            case NESTLUFT: {
                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // drehen
                if (yAcc != 0.0f)
                    Rot += Timer.sync(RotDir);

                while (Rot > 360.0f)
                    Rot -= 360.0f;

                uint32_t bu2 = TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]);

                if (yAcc != 0.0f && (bu2 & BLOCKWERT_WAND || bu2 & BLOCKWERT_PLATTFORM)) {
                    ySpeed = -ySpeed / 1.5f;

                    if (ySpeed > -1.0f) {
                        xSpeed = 0.0f;
                        ySpeed = 0.0f;
                        yAcc = 0.0f;
                    }
                }

            } break;

            case FOG:  // Nebel (fadet ein und aus)
            {
                // Nebel faden
                //
                AnimCount += Timer.sync(4.0f);

                // Fadet ein ?
                //
                if (AnimCount < 128.0f)
                    tmp_alpha = static_cast<int>(AnimCount);

                // oder aus ?
                else
                    tmp_alpha = 256 - static_cast<int>(AnimCount);

                if (AnimCount >= 256.0f)
                    Lebensdauer = 0;
            } break;

            case ROCKSPLITTER:  // Splitter eines Felsblocks
            case ROCKSPLITTERSMALL:
            case ROCKSPLITTERSMALLBLUE:
            case BOULDER_SMALL: {
                if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
                    AnimPhase = 0;         // Dann wieder von vorne beginnen

                Lebensdauer -= Timer.sync(6.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case WASSER_SPRITZER: {
                Lebensdauer -= Timer.sync(AnimSpeed);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                OwnDraw = true;
            } break;

            case WASSER_SPRITZER2: {
                Lebensdauer -= Timer.sync(AnimSpeed);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                OwnDraw = true;
            } break;

            case SPIDERSPLITTER:
            case SPIDERSPLITTER2: {
                if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
                    AnimPhase = 0;         // Dann wieder von vorne beginnen

                Lebensdauer -= Timer.sync(6.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case SPIDERGRENADE: {
                // DKS - we weren't getting our full animation
                // if (AnimPhase == AnimEnde)		// Animation von zu Ende	?
                if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
                    AnimPhase = 0;         // Dann wieder von vorne beginnen

                // Am Boden aufgetroffen ? Dann Feuerwalze erzeugen
                if (bu & BLOCKWERT_WAND) {
                    Lebensdauer = 0;
                    //				Projectiles.PushProjectile(xPos - 30, yPos, FLAMEWALL);
                }
            } break;

            case EVILSMOKE:
            case EVILSMOKE2:
            case EVILROUNDSMOKE: {
                Lebensdauer -= Timer.sync(25.0f);  // schnell ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case STELZE:  // Stelze eines StelzSacks
            {
                if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
                    AnimPhase = 0;

                Rot += Timer.sync(xSpeed * 2.0f);
            } break;

            case STELZHEAD: {
                AnimCount -= Timer.sync(1.0f);

                if (AnimCount < 0.0f) {
                    AnimCount = 0.5f;
                    PartikelSystem.PushPartikel(xPos - 20 + random(40), yPos - 20 + random(40), EXPLOSION_MEDIUM2);
                }
            } break;

            case SMOKEBIG:  // Riesen Rauch
            {
                Lebensdauer -= Timer.sync(4.5f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += Timer.sync(RotDir * ySpeed * 3.0f);

            } break;

            case SMOKEBIG2:  // Riesen Rauch
            {
                Lebensdauer -= Timer.sync(4.5f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += Timer.sync(RotDir * ySpeed);

                if (ySpeed < -5.0f) {
                    yAcc = 0.0f;
                    ySpeed = -5.0f;
                }

                if (xSpeed > 0.0f)
                    xSpeed -= Timer.sync(3.0f);

                if (xSpeed < 0.0f)
                    xSpeed += Timer.sync(3.0f);

            } break;

            case SMOKEBIG_OUTTRO:  // Riesen Rauch
            {
                Lebensdauer -= Timer.sync(1.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += Timer.sync(RotDir * ySpeed * 1.0f);

            } break;

            case SMOKE:  // Rauchwolke
            {
                Lebensdauer -= Timer.sync(30.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += Timer.sync(RotDir * ySpeed * 5.0f);
            } break;

            case SMOKE2:  // Rauchwolke bei LavaBall zb
            case SMOKE3: {
                Lebensdauer -= Timer.sync(4.5f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += Timer.sync(RotDir * ySpeed * 3.0f);

                // Rauch im Wasser ? Dann zur Blubberblase werden lassen
                // DKS - Fixed out-of-bounds access to Tiles[][] array here when particle rises to top of screen
                //      at the top of a level (level 7, eis.map will crash when firing laser weapon at beginning)
                // ORIGINAL LINE:
                // if (TileEngine.TileAt(static_cast<int>((xPos + 5) / TILESIZE_X), static_cast<int>((yPos + 5) /
                // TILESIZE_Y)).Block & BLOCKWERT_LIQUID)
                // ADDED CHECK AND CONVERTED TO float DIV-BY-RECIPROCAL:
                int const tmp_x = static_cast<int>((xPos + 5.0f) * (1.0f / TILESIZE_X));
                int const tmp_y = static_cast<int>((yPos + 5.0f) * (1.0f / TILESIZE_Y));
                if (tmp_x >= 0 && tmp_x < TileEngine.LEVELSIZE_X && tmp_y >= 0 && tmp_y < TileEngine.LEVELSIZE_Y &&
                    TileEngine.TileAt(tmp_x, tmp_y).Block & BLOCKWERT_LIQUID) {
                    float off = 0;

                    if (PartikelArt == SMOKE2)
                        off = 7;

                    Lebensdauer = 0;
                    PartikelSystem.PushPartikel(xPos + 5, yPos + off, BUBBLE);
                }
            } break;

            case REGENTROPFEN:  // Verschwindet wenn Wand oder Boden getroffen
            {
                // Aus dem Level rausgeflogen ?
                if (yPos > TileEngine.LEVELPIXELSIZE_Y)
                    Lebensdauer = 0;

                // Auf den Boden gekommen oder an die Wand geklatscht ? Dann verschwinden zerplatzen
                if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bu & BLOCKWERT_WAND || bu & BLOCKWERT_LIQUID ||
                    bu & BLOCKWERT_PLATTFORM) {
                    Lebensdauer = 0;

                    // Ein paar Spritzer erzeugen
                    if (options_Detail >= DETAIL_HIGH)
                        for (int i = 0; i < 2; i++)
                            PartikelSystem.PushPartikel(xPos + 8, yPos + 16, WASSERTROPFEN);
                }

            } break;

            case HALSWIRBEL:
            case KAPUTTETURBINE: {
                Lebensdauer -= Timer.sync(5.0f);  // schnell ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                AnimCount += Timer.sync(0.8f);

                while (AnimCount > TWO_PI)
                    AnimCount -= TWO_PI;
            } break;

            case DUST: {
                Lebensdauer -= Timer.sync(1.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Richtung umdrehen ?
                if (ySpeed < -4.0f)
                    yAcc = 0.8f;

                if (ySpeed > 4.0f)
                    yAcc = -0.8f;
            } break;

            case SCHROTT1:
            case SCHROTT2: {
                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // DKS - we weren't getting our full animation
                // if (AnimPhase >= AnimEnde)
                if (AnimPhase > AnimEnde)
                    AnimPhase = 0;
            } break;

            case SCHNEEFLOCKE:  // Verschwindet am unteren Level-Rand
            {
                // Richtung umdrehen ?
                if (xSpeed < -2.0f)
                    xAcc = 0.2f;

                if (xSpeed > 2.0f)
                    xAcc = -0.2f;

                // Aus dem Level rausgeflogen ?
                if (yPos > TileEngine.LEVELPIXELSIZE_Y)
                    Lebensdauer = 0;

                // Auf den Boden gekommen ? Dann langsam ausfaden
                if (bu & BLOCKWERT_WAND || bu & BLOCKWERT_LIQUID || bu & BLOCKWERT_PLATTFORM) {
                    xSpeed = 0.0f;
                    ySpeed = 0.0f;
                    xAcc = 0.0f;
                    yAcc = 0.0f;
                }

                // Ausfaden am Boden
                if (ySpeed == 0.0f) {
                    Lebensdauer -= Timer.sync(10.0f);
                    tmp_alpha = static_cast<int>(Lebensdauer);
                }

            } break;

            case SCHNEEFLOCKE_END:  // Verschwindet am unteren Level-Rand
            {
                // Richtung umdrehen ?
                if (xSpeed < -2.0f)
                    xAcc = 0.1f;

                if (xSpeed > 2.0f)
                    xAcc = -0.1f;

                // Unten aus dem Screen raus? Dann verschwinden lassen
                if (yPos > 480.0f)
                    Lebensdauer = 0.0f;
            } break;

            default:
                break;
        }  // switch < Additiv Grenze

    else

        switch (PartikelArt) {
                // Rauch des Blitzbeams, wird langsam größer

            case BEAMSMOKE: {
                Lebensdauer -= Timer.sync(15.0f);  // langsam
                tmp_alpha = static_cast<int>(Lebensdauer);

                AnimCount += Timer.sync(3.0f);

            } break;

            case BEAMSMOKE3:
            case BEAMSMOKE4: {
                Rot += Timer.sync(RotDir);
                Lebensdauer -= Timer.sync(10.0f);  // langsam
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case BEAMSMOKE5: {
                Lebensdauer -= Timer.sync(AnimPhase);  // langsam
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case BEAMSMOKE2: {
                Lebensdauer -= Timer.sync(20.0f);  // langsam
                tmp_alpha = static_cast<int>(Lebensdauer);

                if (m_pParent != nullptr) {
                    // Bewegungsrichtung anpassen
                    // DKS - converted to float:
                    float absx = m_pParent->BeamX - xPos;  // Differenz der x
                    float absy = m_pParent->BeamY - yPos;  // und y Strecke

                    // DKS - converted to float:
                    float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                    speed = speed * (4 + AnimPhase * 2);                    // Geschwindigkeit

                    absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                    absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                    xSpeed = absx;
                    ySpeed = absy;
                }
            } break;

            case SNOWFLUSH: {
                Lebensdauer -= Timer.sync(8.0f);

                // Im Wasser gelandet ?
                if (TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) & BLOCKWERT_LIQUID)
                    Lebensdauer -= Timer.sync(20.0f);

                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += Timer.sync(RotDir * 10.0f);
            } break;

            case WATERFLUSH: {
                Lebensdauer -= Timer.sync(1.5f);

                // Im Wasser gelandet ?
                if (TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) & BLOCKWERT_LIQUID ||
                    TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) &
                        BLOCKWERT_PLATTFORM ||
                    TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt]) & BLOCKWERT_WAND)
                    Lebensdauer -= Timer.sync(4.0f);

                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += Timer.sync(RotDir * 10.0f);
            } break;

            case UFOLASERFLARE: {
                Lebensdauer -= Timer.sync(20.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case FUNKE:  // RoterFunken
            {
                Lebensdauer -= Timer.sync(14.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case FUNKE2:  // Grüner Funken
            {
                Lebensdauer -= Timer.sync(16.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case LONGFUNKE: {
                Lebensdauer -= Timer.sync(12.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // steckt in der Wand?
                //
                if (yPosOld == yPos)
                    Lebensdauer -= Timer.sync(100.0f);

                if (Lebensdauer < 0.0f)
                    Lebensdauer = 0.0f;

            } break;

            case WATERFUNKE: {
                // In Flüssigkeit? Dann gleich verschwinden
                if (bu & BLOCKWERT_LIQUID)
                    Lebensdauer = 0.0f;

                // langsam ausfaden lassen
                Lebensdauer -= Timer.sync(18.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case LASERFUNKE:  // Laser-Funken
            {
                Lebensdauer -= Timer.sync(16.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case LASERFUNKE2:  // Laser-Funken
            {
                Lebensdauer -= Timer.sync(16.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                if (AnimPhase > AnimEnde)
                    AnimPhase = 0;
            } break;

            case PHARAOSMOKE:  // Rauch des Pharao-Schusses
            {
                Lebensdauer -= Timer.sync(20.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case ROCKETSMOKE:      // Rauch einer Rakete
            case ROCKETSMOKEBLUE:  // in blau
            {
                Lebensdauer -= Timer.sync(50.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case ROCKETSMOKEGREEN:  // in grün
            {
                Lebensdauer -= Timer.sync(20.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case FLUGSACKSMOKE: {
                Lebensdauer -= Timer.sync(30.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case EVILFUNKE: {
                Lebensdauer -= Timer.sync(30.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case WASSERTROPFEN:  // Verschindet an der Wasseroberfläche
            {
                Lebensdauer -= Timer.sync(16.0f);  // langsam ausfaden lassen

                if (Lebensdauer > 255.0f)
                    tmp_alpha = 0;
                else
                    tmp_alpha = static_cast<int>(Lebensdauer);

                if (bu & BLOCKWERT_LIQUID && ySpeed > 0.0f)
                    Lebensdauer = 0;
            } break;

            case BUBBLE:  // Zerplatzt an der Wasseroberfläche
            {
                // Richtung umdrehen ?
                if ((xSpeed < -AnimCount && xAcc < 0.0f) || (xSpeed > AnimCount && xAcc > 0.0f))
                    xAcc *= -1;

                if (ySpeed < 0.0f) {
                    bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, PartikelRect[PartikelArt], false);
                    if (!(bo & BLOCKWERT_LIQUID) || (bo & BLOCKWERT_WAND))

                    {
                        for (int i = 0; i < 3; i++)
                            PartikelSystem.PushPartikel(xPos, yPos, WATERFUNKE);

                        // Und Luftblase verschwinden lassen
                        Lebensdauer = 0;
                    }
                }
            } break;

            case LASERFLAME:  // Wird immer durchsichtiger
            case STELZFLARE: {
                Lebensdauer -= Timer.sync(50.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);

            } break;

            case SHIELD:  // Verschwindet nach Ablauf der Animation
            {
                Lebensdauer -= Timer.sync(20.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);

                // an Spieler anpassen
                xPos = m_pParent->xpos + xSpeed + 37;
                yPos = m_pParent->ypos + ySpeed + 57;

                if (m_pParent->Handlung == PlayerActionEnum::RADELN ||
                        m_pParent->Handlung == PlayerActionEnum::RADELN_FALL)
                    yPos += 30;
            } break;

            case TEXTSECRET: {
                Lebensdauer -= Timer.sync(10.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case KRINGELSECRET: {
                Lebensdauer -= Timer.sync(10.0f) + static_cast<float>(AnimPhase);
                tmp_alpha = static_cast<int>(Lebensdauer);

                xSpeed -= Timer.sync(xSpeed * 0.1f);
                ySpeed -= Timer.sync(ySpeed * 0.1f);
            } break;

            case KRINGEL: {
                Lebensdauer -= Timer.sync(20.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);

                xPos += Timer.sync(m_pParent->xspeed);
                yPos += Timer.sync(m_pParent->yspeed);

                // Richtung neu berechnen
                //
                // DKS - converted to float:
                // berechnung
                float absx = m_pParent->xpos + 35 - (xPos + 4);  // Differenz der x
                float absy = m_pParent->ypos + 40 - (yPos + 4);  // und y Strecke

                // DKS - converted to float:
                float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                speed = speed * (8 + AnimPhase);                  // Geschwindigkeit ist 4 fach

                absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                xSpeed = absx;
                ySpeed = absy;
            } break;

            case TURBINESMOKE:  // Partikel, die in die Turbine des Metalhead Bosses gesaugt werden
            {
                Lebensdauer -= Timer.sync(10.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Richtung neu berechnen
                //
                // DKS - converted to float:
                // berechnung
                float absx = PartikelSystem.xtarget - xPos;  // Differenz der x
                float absy = PartikelSystem.ytarget - yPos;  // und y Strecke

                // DKS - converted to float:
                float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                speed = speed * (15 + AnimPhase * 2);             // Geschwindigkeit ist 4 fach

                absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                xSpeed = absx;
                ySpeed = absy;
            } break;

            case MINIFLARE:  // Flare beim Lava Ball
            {
                Lebensdauer -= Timer.sync(5.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);

                if (xSpeed > 2.0f)
                    xAcc = -1.0f;
                if (xSpeed < -2.0f)
                    xAcc = 1.0f;

                if (ySpeed > 20.0f)
                    ySpeed = 20.0f;

            } break;

            case GRENADEFLARE:  // Flare beim Granaten Treffer
            {
                Lebensdauer -= Timer.sync(20.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);
                Rot += Timer.sync(RotDir * 20.0f);

            } break;

            case EXPLOSIONFLARE:   // Flare bei Explosion
            case EXPLOSIONFLARE2:  // Flare bei Explosion
            {
                Lebensdauer -= Timer.sync(50.0f);
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case SCHLEIM:   // kleiner Schleimbollen
            case SCHLEIM2:  // kleiner Alien Schleimbollen
            {
                Lebensdauer -= Timer.sync(18.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                if (AnimPhase > AnimEnde)
                    AnimPhase = 0;
            } break;

            case SHOCKEXPLOSION:  // Schockwelle bei Spieler Explosion
            {
                Lebensdauer -= Timer.sync(10.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                AnimCount += Timer.sync(130.0f);
            } break;

            case SHOTFLARE:  // Leuchten bei Schuss-Aufprall
            case SHOTFLARE2: {
                Lebensdauer -= Timer.sync(100.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case EXTRACOLLECTED: {
                Lebensdauer -= Timer.sync(100.0f - static_cast<float>(AnimPhase) * 20.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                AnimCount += Timer.sync(static_cast<float>(AnimPhase) * 40.0f);
            } break;

            case DIAMANTCOLLECTED: {
                Lebensdauer -= Timer.sync(120.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                AnimCount += Timer.sync(40.0f);
            } break;

            case DRACHE_SMOKE: {
                Lebensdauer -= Timer.sync(25.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += RotDir * Timer.sync(10.0f);
            } break;

            case FIREBALL_SMOKE: {
                Lebensdauer -= Timer.sync(150.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                Rot += RotDir * Timer.sync(10.0f);
            } break;

            case LILA: {
                Lebensdauer -= Timer.sync(30.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case LASERFLARE: {
                Lebensdauer -= Timer.sync(150.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
            } break;

            case EXPLOSION_TRACE_END: {
                Lebensdauer -= Timer.sync(4.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                if (AnimPhase > AnimEnde)  // Animation zu Ende	?
                    Lebensdauer = 0;       // Dann Explosion verschwinden lassen

                Rot += Timer.sync(RotDir * AnimSpeed * 3.0f);

            } break;

            case LAVADUST: {
                Lebensdauer -= Timer.sync(1.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);

                // Richtung umdrehen ?
                if (xSpeed < -2.0f)
                    xAcc = 0.4f;

                if (xSpeed > 2.0f)
                    xAcc = -0.4f;
            } break;

            case EXPLOSION_MEDIUM2_ADD:
            case EXPLOSION_MEDIUM3_ADD:
            case EXPLOSION_REGULAR: {
                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen
                tmp_alpha = static_cast<int>(Lebensdauer);
                // DKS - off-by-one error:
                // if (AnimPhase >= AnimEnde)		// Animation zu Ende	?
                if (AnimPhase > AnimEnde)  // Animation zu Ende	?
                    Lebensdauer = 0;       // Dann Explosion verschwinden lassen

                Rot += Timer.sync(RotDir * AnimSpeed * 10.0f);

            } break;

            case EXPLOSION_TRACE: {
                Lebensdauer -= Timer.sync(5.0f);  // langsam ausfaden lassen

                // Explosion spawnen
                AnimCount -= Timer.sync(1.0f);
                if (AnimCount < 0.0f) {
                    AnimCount = 0.4f;
                    PartikelSystem.PushPartikel(xPos + 30, yPos + 30, EXPLOSION_REGULAR);
                }
            } break;

        }  // switch >= Additiv Grenze

    // alte Position für Kollisionsabfrage sichern
    xPosOld = xPos;
    yPosOld = yPos;

    // DKS - SetAlpha() below will ensure range 0-255 is ensured
    SetAlpha(tmp_alpha);
}

// --------------------------------------------------------------------------------------
// Partikel anzeigen, und dabei nur die Partikel-Textur setzen, wenn diese nicht
// schon gesetzt ist, denn das bringt Speeeeeeeeeeed ... hoffentlich =)
// --------------------------------------------------------------------------------------

bool PartikelClass::Render() {

    // Partikel rotieren?
    if (Rotate) {
        DrawMode = DrawModeEnum::ROTATED;

        clampAngle(Rot);

        DirectGraphics.SetFilterMode(true);

        int width = PartikelGrafix[PartikelArt].itsXFrameSize;
        int height = PartikelGrafix[PartikelArt].itsYFrameSize;

        // Rotationsmatrix
        glm::mat4x4 matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[static_cast<int>(Rot)], glm::vec3(0.0f, 0.0f, 1.0f));

        float x = static_cast<float>(xPos - TileEngine.XOffset);
        float y = static_cast<float>(yPos - TileEngine.YOffset);

        glm::mat4x4 matTrans, matTrans2;

        // Transformation zum Ursprung
        D3DXMatrixTranslation(&matTrans, -x - (width) / 2, -y - (height) / 2, 0.0f);

        // Transformation wieder zurück
        D3DXMatrixTranslation(&matTrans2, x + (width) / 2, y + (height) / 2, 0.0f);

        // Verschieben und rotieren
        matWorld = matRot * matTrans;

        // und wieder zurück verschieben
        matWorld = matTrans2 * matWorld;
        g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
        load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
    } else

        // Partikel nicht rotieren?
        if (!Rotate && DrawMode != DrawModeEnum::NORMAL) {
        DrawMode = DrawModeEnum::NORMAL;

        DirectGraphics.SetFilterMode(false);

        // Normale Projektions-Matrix wieder herstellen
        matWorld = glm::mat4x4(1.0f);
        g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
        load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
    }

    // Normaler Partikel (aus Grafik) zeichnen
    // DKS - Added a check to ensure we never draw without a valid animation phase, to ensure
    //      we never step outside itsPreCalcedRects boundaries
    // if (OwnDraw == false)
    if (!OwnDraw && AnimPhase <= AnimEnde) {
        float const xts = PartikelGrafix[PartikelArt].itsXTexScale;
        float const yts = PartikelGrafix[PartikelArt].itsYTexScale;

        // DKS - There is no need to compute this, it's already in the sprite's itsPreCalcedRects array:
        // int   xfs, yfs, xfc;
        // xfs = PartikelGrafix[PartikelArt].itsXFrameSize;
        // yfs = PartikelGrafix[PartikelArt].itsYFrameSize;
        // xfc = PartikelGrafix[PartikelArt].itsXFrameCount;
        // RECT_struct Rect;
        //// Ausschnitt berechnen
        // Rect.top	= (AnimPhase/xfc) * yfs;
        // Rect.left	= (AnimPhase%xfc) * xfs;
        // Rect.right  = Rect.left + xfs;
        // Rect.bottom = Rect.top  + yfs;

        RECT_struct &Rect = PartikelGrafix[PartikelArt].itsPreCalcedRects[AnimPhase];

        // Vertice Koordinaten
        float l = -TileEngine.XOffset + xPos - 0.5f;                                 // Links
        float r = -TileEngine.XOffset + xPos + (Rect.right - Rect.left - 1) + 0.5f;  // Rechts
        float o = -TileEngine.YOffset + yPos - 0.5f;                                 // Oben
        float u = -TileEngine.YOffset + yPos + (Rect.bottom - Rect.top - 1) + 0.5f;  // Unten

        // Textur Koordinaten
        float tl = Rect.left * xts;    // Links
        float tr = Rect.right * xts;   // Rechts
        float to = Rect.top * yts;     // Oben
        float tu = Rect.bottom * yts;  // Unten

        // DKS - Altered this code to assign to TriangleStrip directly:
        QUAD2D TriangleStrip;
        TriangleStrip.v1.color = TriangleStrip.v2.color = TriangleStrip.v3.color = TriangleStrip.v4.color =
            D3DCOLOR_RGBA(red, green, blue, alpha);

        TriangleStrip.v1.x = l;  // Links oben
        TriangleStrip.v1.y = o;
        TriangleStrip.v1.tu = tl;
        TriangleStrip.v1.tv = to;

        TriangleStrip.v2.x = r;  // Rechts oben
        TriangleStrip.v2.y = o;
        TriangleStrip.v2.tu = tr;
        TriangleStrip.v2.tv = to;

        TriangleStrip.v3.x = l;  // Links unten
        TriangleStrip.v3.y = u;
        TriangleStrip.v3.tu = tl;
        TriangleStrip.v3.tv = tu;

        TriangleStrip.v4.x = r;  // Rechts unten
        TriangleStrip.v4.y = u;
        TriangleStrip.v4.tu = tr;
        TriangleStrip.v4.tv = tu;

        if (PartikelArt != CurrentPartikelTexture) {
            DirectGraphics.SetTexture(PartikelGrafix[PartikelArt].itsTexIdx);
            CurrentPartikelTexture = PartikelArt;
        }

        // Sprite zeichnen
        // DKS - Altered to match new QUAD2D strips:
        DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip);

    } else if (PartikelArt == TEXTSECRET) {
        D3DCOLOR col = D3DCOLOR_RGBA(255, 224, 64, alpha);
        pMenuFont->DrawTextCenterAlign(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset, TextArray[TEXT::SECRET],
                                       col);

        CurrentPartikelTexture = -1;
    }

    // Langer Funke (Linie)
    // DKS - Added a check to ensure we never draw without a valid animation phase, to ensure
    //      we never step outside itsPreCalcedRects boundaries
    // else if (PartikelArt == EXPLOSION_TRACE_END)
    else if (PartikelArt == EXPLOSION_TRACE_END && AnimPhase <= AnimEnde) {
        D3DCOLOR col = D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(Lebensdauer));
        PartikelGrafix[PartikelArt].itsRect = PartikelGrafix[PartikelArt].itsPreCalcedRects[AnimPhase];
        PartikelGrafix[PartikelArt].RenderSpriteScaledRotated(xPos, yPos, static_cast<float>(blue),
                                                              static_cast<float>(blue), Rot, col);
        CurrentPartikelTexture = EXPLOSION_TRACE_END;
    } else if (PartikelArt == LONGFUNKE || PartikelArt == WATERFUNKE) {
        DirectGraphics.SetTexture(-1);

        D3DXVECTOR2 pos, dir;

        pos.x = xPos - TileEngine.XOffset;
        pos.y = yPos - TileEngine.YOffset;
        dir.x = pos.x - xSpeed;
        dir.y = pos.y - ySpeed;

        if (PartikelArt == LONGFUNKE)
            RenderLine(dir, pos, D3DCOLOR_RGBA(255, 64, 16, static_cast<int>(Lebensdauer / 3.0f)),
                       D3DCOLOR_RGBA(255, 192, 32, static_cast<int>(Lebensdauer)));

        else if (PartikelArt == WATERFUNKE) {
            dir.x = pos.x - xSpeed * 0.5f;
            dir.y = pos.y - ySpeed * 0.5f;

            RenderLine(dir, pos,
                       D3DCOLOR_RGBA(TileEngine.ColR1, TileEngine.ColG1, TileEngine.ColB1,
                                     static_cast<int>(Lebensdauer / 3.0f)),
                       D3DCOLOR_RGBA(TileEngine.ColR2, TileEngine.ColG2, TileEngine.ColB2,
                                     static_cast<int>(Lebensdauer)));
        }

        CurrentPartikelTexture = -1;
    }

    // Halswirbel oder Turbine des Metalhead Bosses (dreht sich, daher extrawurscht)
    else if (PartikelArt == HALSWIRBEL || PartikelArt == KAPUTTETURBINE) {
        DirectGraphics.SetTexture(PartikelGrafix[PartikelArt].itsTexIdx);

        // DKS-new rad/deg macros:
        // PartikelGrafix[PartikelArt].RenderSpriteRotated (float (xPos - TileEngine.XOffset),
        //        float (yPos - TileEngine.YOffset),
        //        AnimCount / PI * 180.0f, D3DCOLOR_RGBA(red, green, blue, alpha));
        PartikelGrafix[PartikelArt].RenderSpriteRotated(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset,
                                                        RadToDeg(AnimCount), D3DCOLOR_RGBA(red, green, blue, alpha));

        CurrentPartikelTexture = PartikelArt;
    }

    // Druckwelle bei der Explosion des Beams
    else if (PartikelArt == BEAMSMOKE5) {
        float a = static_cast<float>(255 - alpha + 64);

        PartikelGrafix[PartikelArt].RenderSpriteScaledRotated(xPos - a / 2.0f - TileEngine.XOffset + 30,
                                                              yPos - a / 2.0f - TileEngine.YOffset + 30, a, a, Rot,
                                                              D3DCOLOR_RGBA(255, 255, 255, alpha));

        CurrentPartikelTexture = PartikelArt;
    }

    // Druckwelle bei der Explosion des Spieler
    else if (PartikelArt == SHOCKEXPLOSION) {
        // int a = 255 - alpha + 64;

        PartikelGrafix[PartikelArt].RenderSpriteScaled(
            xPos - AnimCount / 2.0f - TileEngine.XOffset, yPos - AnimCount / 2.0f - TileEngine.YOffset,
            static_cast<int>(AnimCount), static_cast<int>(AnimCount), D3DCOLOR_RGBA(red, green, blue, alpha));

        CurrentPartikelTexture = PartikelArt;
    }

    // Leuchten beim Extra Einsammeln
    else if (PartikelArt == EXTRACOLLECTED) {
        PartikelGrafix[PartikelArt].RenderSpriteScaled(
            xPos + 16 - AnimCount / 2.0f - TileEngine.XOffset, yPos + 16 - AnimCount / 2.0f - TileEngine.YOffset,
            static_cast<int>(AnimCount), static_cast<int>(AnimCount), D3DCOLOR_RGBA(red, green, blue, alpha));

        CurrentPartikelTexture = PartikelArt;
    }

    // Leuchten beim Diamant Einsammeln
    else if (PartikelArt == DIAMANTCOLLECTED) {
        DirectGraphics.SetAdditiveMode();

        PartikelGrafix[PartikelArt].RenderSpriteScaled(
            xPos + 15 - AnimCount / 2.0f - TileEngine.XOffset, yPos + 15 - AnimCount / 2.0f - TileEngine.YOffset,
            static_cast<int>(AnimCount), static_cast<int>(AnimCount), D3DCOLOR_RGBA(red, green, blue, alpha));

        CurrentPartikelTexture = PartikelArt;
    }

    // Langer Funke/Wasserspritzer
    else if (PartikelArt == WASSER_SPRITZER || PartikelArt == WASSER_SPRITZER2) {

        int h = 100;
        int b;
        if (PartikelArt == WASSER_SPRITZER)
            b = 200 - alpha / 2;
        else
            b = static_cast<int>(AnimCount * 30.0f);

        PartikelGrafix[PartikelArt].RenderSpriteScaled(xPos - TileEngine.XOffset - b / 4,
                                                       yPos - TileEngine.YOffset - h / 2, b, h,
                                                       D3DCOLOR_RGBA(red, green, blue, alpha));

        CurrentPartikelTexture = PartikelArt;
    } else if (PartikelArt == BEAMSMOKE) {
        PartikelGrafix[PartikelArt].RenderSpriteScaledRotated(xPos - TileEngine.XOffset - AnimCount / 2.0f,
                                                              yPos - TileEngine.YOffset - AnimCount / 2.0f, AnimCount,
                                                              AnimCount, Rot, D3DCOLOR_RGBA(red, green, blue, alpha));

        CurrentPartikelTexture = PartikelArt;
    }

    return true;
}

// --------------------------------------------------------------------------------------
// PartikelsystemKlasse Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor : laden der Partikel Grafiken
// --------------------------------------------------------------------------------------

PartikelsystemClass::PartikelsystemClass() {
    pStart = nullptr;
    pEnd = nullptr;
    NumPartikel = 0;
    MAX_PARTIKEL = 2000;
    SetParticleCount();

    ThunderAlpha = 0.0f;
    for (unsigned char &i : ThunderColor)
        i = 0;

    DrawMode = DrawModeEnum::NORMAL;
}

// DKS - PartikelsystemClass is now a static global, instead of dynamically allocated
//      pointer, so moved the loading of sprites from its constructor to this new
//      function:
void PartikelsystemClass::LoadSprites() {
    // Patronenhülse
    PartikelGrafix[BULLET].LoadImage("bullet.png", 8, 8, 8, 8, 1, 1);
    PartikelRect[BULLET].left = 2;
    PartikelRect[BULLET].right = 6;
    PartikelRect[BULLET].top = 2;
    PartikelRect[BULLET].bottom = 6;

    // Patronenhülse vom Skeletor
    PartikelGrafix[BULLET_SKELETOR].LoadImage("skeletor_bullet.png", 15, 6, 15, 6, 1, 1);
    PartikelRect[BULLET_SKELETOR].left = 2;
    PartikelRect[BULLET_SKELETOR].right = 13;
    PartikelRect[BULLET_SKELETOR].top = 1;
    PartikelRect[BULLET_SKELETOR].bottom = 5;

    // Glassplitter
    PartikelGrafix[GLASSPLITTER].LoadImage("glassplitter.png", 100, 80, 20, 20, 5, 4);
    PartikelRect[GLASSPLITTER].left = 8;
    PartikelRect[GLASSPLITTER].right = 12;
    PartikelRect[GLASSPLITTER].top = 8;
    PartikelRect[GLASSPLITTER].bottom = 12;

    // Mittlere Explosion
    PartikelGrafix[EXPLOSION_MEDIUM].LoadImage("explosion-medium.png", 300, 240, 60, 60, 5, 4);
    PartikelRect[EXPLOSION_MEDIUM].left = 0;
    PartikelRect[EXPLOSION_MEDIUM].right = 60;
    PartikelRect[EXPLOSION_MEDIUM].top = 0;
    PartikelRect[EXPLOSION_MEDIUM].bottom = 60;

    // Mittlere Explosion2
    PartikelGrafix[EXPLOSION_MEDIUM2].LoadImage("explosion-medium2.png", 300, 240, 60, 60, 5, 4);
    PartikelRect[EXPLOSION_MEDIUM2].left = 0;
    PartikelRect[EXPLOSION_MEDIUM2].right = 60;
    PartikelRect[EXPLOSION_MEDIUM2].top = 0;
    PartikelRect[EXPLOSION_MEDIUM2].bottom = 60;

    // Mittlere Explosion2
    PartikelGrafix[EXPLOSION_MEDIUM2_ADD].LoadImage("explosion-medium2.png", 300, 240, 60, 60, 5, 4);
    PartikelRect[EXPLOSION_MEDIUM2_ADD].left = 0;
    PartikelRect[EXPLOSION_MEDIUM2_ADD].right = 60;
    PartikelRect[EXPLOSION_MEDIUM2_ADD].top = 0;
    PartikelRect[EXPLOSION_MEDIUM2_ADD].bottom = 60;

    // Mittlere Explosion3
    // DKS - This now uses explosion-medium.png just like EXPLOSION_MEDIUM does, because their
    //      image files were 100% identical. Saves a bit of VRAM with the new intelligent
    //      TexturesystemClass I added, as it shares identical textures between sprites.
    // PartikelGrafix[EXPLOSION_MEDIUM3].LoadImage("explosion-medium3.png", 300, 240, 60, 60, 5, 4);
    PartikelGrafix[EXPLOSION_MEDIUM3].LoadImage("explosion-medium.png", 300, 240, 60, 60, 5, 4);
    PartikelRect[EXPLOSION_MEDIUM3].left = 0;
    PartikelRect[EXPLOSION_MEDIUM3].right = 60;
    PartikelRect[EXPLOSION_MEDIUM3].top = 0;
    PartikelRect[EXPLOSION_MEDIUM3].bottom = 60;

    // Mittlere Explosion3
    // DKS - See note directly above
    // PartikelGrafix[EXPLOSION_MEDIUM3_ADD].LoadImage("explosion-medium3.png", 300, 240, 60, 60, 5, 4);
    PartikelGrafix[EXPLOSION_MEDIUM3_ADD].LoadImage("explosion-medium.png", 300, 240, 60, 60, 5, 4);
    PartikelRect[EXPLOSION_MEDIUM3_ADD].left = 0;
    PartikelRect[EXPLOSION_MEDIUM3_ADD].right = 60;
    PartikelRect[EXPLOSION_MEDIUM3_ADD].top = 0;
    PartikelRect[EXPLOSION_MEDIUM3_ADD].bottom = 60;

    // Grüne Explosion
    PartikelGrafix[EXPLOSION_GREEN].LoadImage("explosion-green.png", 300, 300, 60, 60, 5, 5);
    PartikelRect[EXPLOSION_GREEN].left = 0;
    PartikelRect[EXPLOSION_GREEN].right = 60;
    PartikelRect[EXPLOSION_GREEN].top = 0;
    PartikelRect[EXPLOSION_GREEN].bottom = 60;

    // Alien Explosion
    PartikelGrafix[EXPLOSION_ALIEN].LoadImage("alienexplosion.png", 300, 300, 60, 60, 5, 5);
    PartikelRect[EXPLOSION_ALIEN].left = 0;
    PartikelRect[EXPLOSION_ALIEN].right = 60;
    PartikelRect[EXPLOSION_ALIEN].top = 0;
    PartikelRect[EXPLOSION_ALIEN].bottom = 60;

    // Grosse Explosion
    // DKS - Corrected dimensions from 512x512 to 500x400, to match actual image file:
    //      Also corrected yfc from 5 to 4 here:
    PartikelGrafix[EXPLOSION_BIG].LoadImage("explosion-big.png", 500, 400, 100, 100, 5, 4);
    PartikelRect[EXPLOSION_BIG].left = 0;
    PartikelRect[EXPLOSION_BIG].right = 100;
    PartikelRect[EXPLOSION_BIG].top = 0;
    PartikelRect[EXPLOSION_BIG].bottom = 100;

    // Grosse Explosion
    PartikelGrafix[EXPLOSION_GIANT].LoadImage("explosion-big2.png", 480, 480, 120, 120, 4, 4);
    PartikelRect[EXPLOSION_GIANT].left = 0;
    PartikelRect[EXPLOSION_GIANT].right = 120;
    PartikelRect[EXPLOSION_GIANT].top = 0;
    PartikelRect[EXPLOSION_GIANT].bottom = 120;

    // Explosions Trace
    PartikelGrafix[EXPLOSION_TRACE].LoadImage("explosion-trace.png", 120, 120, 120, 120, 1, 1);
    PartikelRect[EXPLOSION_TRACE].left = 0;
    PartikelRect[EXPLOSION_TRACE].right = 120;
    PartikelRect[EXPLOSION_TRACE].top = 0;
    PartikelRect[EXPLOSION_TRACE].bottom = 120;

    // Kleine blaue Explosion
    PartikelGrafix[BLUE_EXPLOSION].LoadImage("blue-explosion.png", 96, 72, 24, 24, 4, 3);
    PartikelRect[BLUE_EXPLOSION].left = 0;
    PartikelRect[BLUE_EXPLOSION].right = 24;
    PartikelRect[BLUE_EXPLOSION].top = 0;
    PartikelRect[BLUE_EXPLOSION].bottom = 24;

    // Riesen fette Explosion
    PartikelGrafix[EXPLOSION_GIGA].LoadImage("explosion-giga.png", 1000, 800, 200, 200, 5, 4);
    PartikelRect[EXPLOSION_GIGA].left = 0;
    PartikelRect[EXPLOSION_GIGA].right = 200;
    PartikelRect[EXPLOSION_GIGA].top = 0;
    PartikelRect[EXPLOSION_GIGA].bottom = 200;

    // Kleine animierter Splitter
    PartikelGrafix[SPLITTER].LoadImage("splitter.png", 32, 32, 8, 8, 4, 4);
    PartikelRect[SPLITTER].left = 0;
    PartikelRect[SPLITTER].right = 8;
    PartikelRect[SPLITTER].top = 0;
    PartikelRect[SPLITTER].bottom = 8;

    // Fetzen eines kaputten Piranhas
    PartikelGrafix[PIRANHATEILE].LoadImage("piranhateile.png", 100, 16, 20, 16, 5, 1);
    PartikelRect[PIRANHATEILE].left = 0;
    PartikelRect[PIRANHATEILE].right = 20;
    PartikelRect[PIRANHATEILE].top = 0;
    PartikelRect[PIRANHATEILE].bottom = 16;

    // Fetzen eines kaputten Riesen Piranhas
    PartikelGrafix[PIRANHATEILE2].LoadImage("piranhateile2.png", 240, 180, 120, 90, 2, 2);
    PartikelRect[PIRANHATEILE2].left = 0;
    PartikelRect[PIRANHATEILE2].right = 120;
    PartikelRect[PIRANHATEILE2].top = 0;
    PartikelRect[PIRANHATEILE2].bottom = 90;

    // Blut eines kaputten Piranhas
    PartikelGrafix[PIRANHABLUT].LoadImage("piranhablut.png", 160, 40, 40, 40, 4, 1);
    PartikelRect[PIRANHABLUT].left = 0;
    PartikelRect[PIRANHABLUT].right = 40;
    PartikelRect[PIRANHABLUT].top = 0;
    PartikelRect[PIRANHABLUT].bottom = 40;

    // Blut einer kaputten Made
    PartikelGrafix[MADEBLUT].LoadImage("madeblut.png", 48, 12, 12, 12, 4, 1);
    PartikelRect[MADEBLUT].left = 0;
    PartikelRect[MADEBLUT].right = 12;
    PartikelRect[MADEBLUT].top = 0;
    PartikelRect[MADEBLUT].bottom = 12;

    // Wassertropfen der vom Spawner kommt
    PartikelGrafix[SPAWNDROP].LoadImage("tropfen.png", 5, 5, 5, 5, 1, 1);
    PartikelRect[SPAWNDROP].left = 0;
    PartikelRect[SPAWNDROP].right = 5;
    PartikelRect[SPAWNDROP].top = 0;
    PartikelRect[SPAWNDROP].bottom = 5;

    // Blatt
    PartikelGrafix[BLATT].LoadImage("blatt.png", 32, 10, 16, 10, 2, 1);
    PartikelRect[BLATT].left = 0;
    PartikelRect[BLATT].right = 16;
    PartikelRect[BLATT].top = 0;
    PartikelRect[BLATT].bottom = 8;

    // Blatt2
    PartikelGrafix[BLATT2].LoadImage("blatt.png", 32, 10, 16, 10, 2, 1);
    PartikelRect[BLATT2].left = 0;
    PartikelRect[BLATT2].right = 16;
    PartikelRect[BLATT2].top = 0;
    PartikelRect[BLATT2].bottom = 8;

    // Wespennest nach Abschuss
    PartikelGrafix[NESTLUFT].LoadImage("nest.png", 43, 60, 43, 60, 1, 1);
    PartikelRect[NESTLUFT].left = 0;
    PartikelRect[NESTLUFT].right = 43;
    PartikelRect[NESTLUFT].top = 0;
    PartikelRect[NESTLUFT].bottom = 60;

    // Fels-Splitter
    PartikelGrafix[ROCKSPLITTER].LoadImage("rocksplitter.png", 96, 48, 24, 24, 4, 2);
    PartikelRect[ROCKSPLITTER].left = 0;
    PartikelRect[ROCKSPLITTER].right = 24;
    PartikelRect[ROCKSPLITTER].top = 0;
    PartikelRect[ROCKSPLITTER].bottom = 24;

    // kleine Fels-Splitter
    PartikelGrafix[ROCKSPLITTERSMALL].LoadImage("rocksplittersmall.png", 48, 24, 12, 12, 4, 2);
    PartikelRect[ROCKSPLITTERSMALL].left = 0;
    PartikelRect[ROCKSPLITTERSMALL].right = 12;
    PartikelRect[ROCKSPLITTERSMALL].top = 0;
    PartikelRect[ROCKSPLITTERSMALL].bottom = 12;

    // kleine Stalagtit-Splitter
    PartikelGrafix[ROCKSPLITTERSMALLBLUE].LoadImage("rocksplittersmallblue.png", 48, 24, 12, 12, 4, 2);
    PartikelRect[ROCKSPLITTERSMALLBLUE].left = 0;
    PartikelRect[ROCKSPLITTERSMALLBLUE].right = 12;
    PartikelRect[ROCKSPLITTERSMALLBLUE].top = 0;
    PartikelRect[ROCKSPLITTERSMALLBLUE].bottom = 12;

    // Spinnen Splitter
    PartikelGrafix[SPIDERSPLITTER].LoadImage("spidersplitter.png", 64, 64, 16, 16, 4, 4);
    PartikelRect[SPIDERSPLITTER].left = 2;
    PartikelRect[SPIDERSPLITTER].right = 14;
    PartikelRect[SPIDERSPLITTER].top = 2;
    PartikelRect[SPIDERSPLITTER].bottom = 14;

    PartikelGrafix[SPIDERSPLITTER2].LoadImage("spidersplitter2.png", 64, 64, 16, 16, 4, 4);
    PartikelRect[SPIDERSPLITTER2].left = 2;
    PartikelRect[SPIDERSPLITTER2].right = 14;
    PartikelRect[SPIDERSPLITTER2].top = 2;
    PartikelRect[SPIDERSPLITTER2].bottom = 14;

    // Granate der Spinne
    PartikelGrafix[SPIDERGRENADE].LoadImage("spidergrenade.png", 64, 16, 16, 16, 4, 1);
    PartikelRect[SPIDERGRENADE].left = 0;
    PartikelRect[SPIDERGRENADE].right = 16;
    PartikelRect[SPIDERGRENADE].top = 0;
    PartikelRect[SPIDERGRENADE].bottom = 16;

    // Schatten des Evil Hurri
    PartikelGrafix[EVILSMOKE].LoadImage("evil-smoke.png", 70, 80, 70, 80, 1, 1);
    PartikelRect[EVILSMOKE].left = 0;
    PartikelRect[EVILSMOKE].right = 70;
    PartikelRect[EVILSMOKE].top = 0;
    PartikelRect[EVILSMOKE].bottom = 80;
    PartikelGrafix[EVILSMOKE2].LoadImage("evil-smoke2.png", 70, 80, 70, 80, 1, 1);
    PartikelRect[EVILSMOKE2].left = 0;
    PartikelRect[EVILSMOKE2].right = 70;
    PartikelRect[EVILSMOKE2].top = 0;
    PartikelRect[EVILSMOKE2].bottom = 80;

    // Stelze eines StelzSacks
    PartikelGrafix[STELZE].LoadImage("stelze.png", 400, 200, 100, 100, 4, 2);
    PartikelRect[STELZE].left = 30;
    PartikelRect[STELZE].right = 70;
    PartikelRect[STELZE].top = 30;
    PartikelRect[STELZE].bottom = 70;

    // Kopf eines StelzSacks
    PartikelGrafix[STELZHEAD].LoadImage("stelzhead.png", 64, 60, 64, 60, 1, 1);
    PartikelRect[STELZHEAD].left = 0;
    PartikelRect[STELZHEAD].right = 64;
    PartikelRect[STELZHEAD].top = 0;
    PartikelRect[STELZHEAD].bottom = 60;

    // Smoke2
    PartikelGrafix[SMOKE2].LoadImage("smoke2.png", 24, 24, 24, 24, 1, 1);
    PartikelRect[SMOKE2].left = 0;
    PartikelRect[SMOKE2].right = 24;
    PartikelRect[SMOKE2].top = 0;
    PartikelRect[SMOKE2].bottom = 24;

    // Smoke3
    PartikelGrafix[SMOKE3].LoadImage("smoke2.png", 24, 24, 24, 24, 1, 1);
    PartikelRect[SMOKE3].left = 0;
    PartikelRect[SMOKE3].right = 24;
    PartikelRect[SMOKE3].top = 0;
    PartikelRect[SMOKE3].bottom = 24;

    // Riesen Rauch
    PartikelGrafix[SMOKEBIG].LoadImage("smokebig.png", 240, 60, 60, 60, 4, 1);
    PartikelRect[SMOKEBIG].left = 0;
    PartikelRect[SMOKEBIG].right = 60;
    PartikelRect[SMOKEBIG].top = 0;
    PartikelRect[SMOKEBIG].bottom = 60;

    // Riesen Rauch
    PartikelGrafix[SMOKEBIG2].LoadImage("smokebig.png", 240, 60, 60, 60, 4, 1);
    PartikelRect[SMOKEBIG2].left = 0;
    PartikelRect[SMOKEBIG2].right = 60;
    PartikelRect[SMOKEBIG2].top = 0;
    PartikelRect[SMOKEBIG2].bottom = 60;

    // Riesen Rauch
    PartikelGrafix[SMOKEBIG_OUTTRO].LoadImage("smokebig.png", 240, 60, 60, 60, 4, 1);
    PartikelRect[SMOKEBIG_OUTTRO].left = 0;
    PartikelRect[SMOKEBIG_OUTTRO].right = 60;
    PartikelRect[SMOKEBIG_OUTTRO].top = 0;
    PartikelRect[SMOKEBIG_OUTTRO].bottom = 60;

    // Rauch des Blitzbeams
    PartikelGrafix[BEAMSMOKE].LoadImage("beamsmoke.png", 24, 24, 24, 24, 1, 1);
    PartikelRect[BEAMSMOKE].left = 0;
    PartikelRect[BEAMSMOKE].right = 24;
    PartikelRect[BEAMSMOKE].top = 0;
    PartikelRect[BEAMSMOKE].bottom = 24;

    // Rauch des Blitzbeams
    PartikelGrafix[BEAMSMOKE4].LoadImage("beamsmoke.png", 24, 24, 24, 24, 1, 1);
    PartikelRect[BEAMSMOKE4].left = 0;
    PartikelRect[BEAMSMOKE4].right = 24;
    PartikelRect[BEAMSMOKE4].top = 0;
    PartikelRect[BEAMSMOKE4].bottom = 24;

    // Rauch beim Aufladen des Blitzbeams
    PartikelGrafix[BEAMSMOKE2].LoadImage("beamsmoke2.png", 24, 8, 8, 8, 3, 1);
    PartikelRect[BEAMSMOKE2].left = 0;
    PartikelRect[BEAMSMOKE2].right = 8;
    PartikelRect[BEAMSMOKE2].top = 0;
    PartikelRect[BEAMSMOKE2].bottom = 8;

    // Rauch beim Aufladen des Blitzbeams
    PartikelGrafix[BEAMSMOKE3].LoadImage("beamsmoke2.png", 24, 8, 8, 8, 3, 1);
    PartikelRect[BEAMSMOKE3].left = 0;
    PartikelRect[BEAMSMOKE3].right = 8;
    PartikelRect[BEAMSMOKE3].top = 0;
    PartikelRect[BEAMSMOKE3].bottom = 8;

    // Druckwelle bei der Explosion des Beams
    PartikelGrafix[BEAMSMOKE5].LoadImage("beamsmoke5.png", 64, 64, 64, 64, 1, 1);
    PartikelRect[BEAMSMOKE5].left = 0;
    PartikelRect[BEAMSMOKE5].right = 64;
    PartikelRect[BEAMSMOKE5].top = 0;
    PartikelRect[BEAMSMOKE5].bottom = 64;

    // Schneegestöber
    PartikelGrafix[SNOWFLUSH].LoadImage("snowflush.png", 32, 32, 32, 32, 1, 1);
    PartikelRect[SNOWFLUSH].left = 0;
    PartikelRect[SNOWFLUSH].right = 32;
    PartikelRect[SNOWFLUSH].top = 0;
    PartikelRect[SNOWFLUSH].bottom = 32;

    // Wasserfall Dampf
    PartikelGrafix[WATERFLUSH].LoadImage("snowflush.png", 32, 32, 32, 32, 1, 1);
    PartikelRect[WATERFLUSH].left = 0;
    PartikelRect[WATERFLUSH].right = 32;
    PartikelRect[WATERFLUSH].top = 0;
    PartikelRect[WATERFLUSH].bottom = 32;

    // Ufo Laser Flare
    PartikelGrafix[UFOLASERFLARE].LoadImage("ufolaserflare.png", 160, 160, 160, 160, 1, 1);
    PartikelRect[UFOLASERFLARE].left = 0;
    PartikelRect[UFOLASERFLARE].right = 160;
    PartikelRect[UFOLASERFLARE].top = 0;
    PartikelRect[UFOLASERFLARE].bottom = 160;

    // Additive Partikel
    //
    // Roter Funken
    PartikelGrafix[FUNKE].LoadImage("funke.png", 3, 3, 3, 3, 1, 1);
    PartikelRect[FUNKE].left = 0;
    PartikelRect[FUNKE].right = 3;
    PartikelRect[FUNKE].top = 0;
    PartikelRect[FUNKE].bottom = 2;

    // Langer Funken
    PartikelGrafix[LONGFUNKE].LoadImage("funke.png", 3, 3, 3, 3, 1, 1);
    PartikelRect[LONGFUNKE].left = 0;
    PartikelRect[LONGFUNKE].right = 3;
    PartikelRect[LONGFUNKE].top = 0;
    PartikelRect[LONGFUNKE].bottom = 2;

    // Wasser Funken
    PartikelRect[WATERFUNKE].left = 0;
    PartikelRect[WATERFUNKE].right = 3;
    PartikelRect[WATERFUNKE].top = 0;
    PartikelRect[WATERFUNKE].bottom = 2;

    // Grüner Funken
    PartikelGrafix[FUNKE2].LoadImage("funke2.png", 3, 3, 3, 3, 1, 1);
    PartikelRect[FUNKE2].left = 0;
    PartikelRect[FUNKE2].right = 3;
    PartikelRect[FUNKE2].top = 0;
    PartikelRect[FUNKE2].bottom = 2;

    // Laserfunke
    PartikelGrafix[LASERFUNKE].LoadImage("laserfunke.png", 9, 9, 9, 9, 1, 1);
    PartikelRect[LASERFUNKE].left = 0;
    PartikelRect[LASERFUNKE].right = 9;
    PartikelRect[LASERFUNKE].top = 0;
    PartikelRect[LASERFUNKE].bottom = 9;

    // Laserfunke2
    PartikelGrafix[LASERFUNKE2].LoadImage("laserfunke2.png", 27, 9, 9, 9, 3, 1);
    PartikelRect[LASERFUNKE2].left = 0;
    PartikelRect[LASERFUNKE2].right = 9;
    PartikelRect[LASERFUNKE2].top = 0;
    PartikelRect[LASERFUNKE2].bottom = 9;

    // Rauchwolke
    PartikelGrafix[SMOKE].LoadImage("smoke.png", 30, 30, 30, 30, 1, 1);
    PartikelRect[SMOKE].left = 0;
    PartikelRect[SMOKE].right = 30;
    PartikelRect[SMOKE].top = 0;
    PartikelRect[SMOKE].bottom = 30;

    // Rauch für den Pharao Schuss
    PartikelGrafix[PHARAOSMOKE].LoadImage("pharaosmoke.png", 12, 12, 12, 12, 1, 1);
    PartikelRect[PHARAOSMOKE].left = 0;
    PartikelRect[PHARAOSMOKE].right = 12;
    PartikelRect[PHARAOSMOKE].top = 0;
    PartikelRect[PHARAOSMOKE].bottom = 12;

    // Rauch einer Rakete
    PartikelGrafix[ROCKETSMOKE].LoadImage("rocketsmoke.png", 12, 12, 12, 12, 1, 1);
    PartikelRect[ROCKETSMOKE].left = 0;
    PartikelRect[ROCKETSMOKE].right = 12;
    PartikelRect[ROCKETSMOKE].top = 0;
    PartikelRect[ROCKETSMOKE].bottom = 12;

    // Rauch einer Rakete in blau
    PartikelGrafix[ROCKETSMOKEBLUE].LoadImage("rocketsmokeblue.png", 12, 12, 12, 12, 1, 1);
    PartikelRect[ROCKETSMOKEBLUE].left = 0;
    PartikelRect[ROCKETSMOKEBLUE].right = 12;
    PartikelRect[ROCKETSMOKEBLUE].top = 0;
    PartikelRect[ROCKETSMOKEBLUE].bottom = 12;

    // Rauch einer Rakete in grün
    PartikelGrafix[ROCKETSMOKEGREEN].LoadImage("rocketsmokegreen.png", 12, 12, 12, 12, 1, 1);
    PartikelRect[ROCKETSMOKEGREEN].left = 0;
    PartikelRect[ROCKETSMOKEGREEN].right = 12;
    PartikelRect[ROCKETSMOKEGREEN].top = 0;
    PartikelRect[ROCKETSMOKEGREEN].bottom = 12;

    // Rauch des Flugsacks
    PartikelGrafix[FLUGSACKSMOKE].LoadImage("flugsacksmoke.png", 64, 64, 16, 16, 4, 4);
    PartikelRect[FLUGSACKSMOKE].left = 0;
    PartikelRect[FLUGSACKSMOKE].right = 16;
    PartikelRect[FLUGSACKSMOKE].top = 0;
    PartikelRect[FLUGSACKSMOKE].bottom = 16;

    // Funke des Evil Blitzes
    PartikelGrafix[EVILFUNKE].LoadImage("evilfunke.png", 8, 8, 8, 8, 1, 1);
    PartikelRect[EVILFUNKE].left = 0;
    PartikelRect[EVILFUNKE].right = 8;
    PartikelRect[EVILFUNKE].top = 0;
    PartikelRect[EVILFUNKE].bottom = 8;

    // Wassertropfen
    PartikelGrafix[WASSERTROPFEN].LoadImage("tropfen2.png", 3, 3, 3, 3, 1, 1);
    PartikelRect[WASSERTROPFEN].left = 0;
    PartikelRect[WASSERTROPFEN].right = 3;
    PartikelRect[WASSERTROPFEN].top = 0;
    PartikelRect[WASSERTROPFEN].bottom = 3;

    // Säuretropfen
    PartikelGrafix[WASSERTROPFEN2].LoadImage("wassertropfen2.png", 16, 4, 4, 4, 4, 1);
    PartikelRect[WASSERTROPFEN2].left = 0;
    PartikelRect[WASSERTROPFEN2].right = 4;
    PartikelRect[WASSERTROPFEN2].top = 0;
    PartikelRect[WASSERTROPFEN2].bottom = 4;

    // Leuchteffekt für den Krabblerlaser
    PartikelGrafix[LASERFLAME].LoadImage("laserflame.png", 48, 48, 48, 48, 1, 1);
    PartikelRect[LASERFLAME].left = 0;
    PartikelRect[LASERFLAME].right = 48;
    PartikelRect[LASERFLAME].top = 0;
    PartikelRect[LASERFLAME].bottom = 48;

    // Luftblase
    PartikelGrafix[BUBBLE].LoadImage("bubble.png", 6, 6, 6, 6, 1, 1);
    PartikelRect[BUBBLE].left = 0;
    PartikelRect[BUBBLE].right = 6;
    PartikelRect[BUBBLE].top = 0;
    PartikelRect[BUBBLE].bottom = 6;

    // Schneeflocke
    PartikelGrafix[SCHNEEFLOCKE].LoadImage("snow.png", 30, 10, 10, 10, 3, 1);
    PartikelRect[SCHNEEFLOCKE].left = 0;
    PartikelRect[SCHNEEFLOCKE].right = 10;
    PartikelRect[SCHNEEFLOCKE].top = 0;
    PartikelRect[SCHNEEFLOCKE].bottom = 3;

    // Schneeflocke
    PartikelGrafix[SCHNEEFLOCKE_END].LoadImage("snow.png", 30, 10, 10, 10, 3, 1);
    PartikelRect[SCHNEEFLOCKE_END].left = 0;
    PartikelRect[SCHNEEFLOCKE_END].right = 10;
    PartikelRect[SCHNEEFLOCKE_END].top = 0;
    PartikelRect[SCHNEEFLOCKE_END].bottom = 3;

    // HurriTeile
    // DKS - Corrected xfs parameter from 32 to 31, to match actual image file:
    PartikelGrafix[HURRITEILE].LoadImage("p1_hurri-teile.png", 217, 32, 31, 32, 7, 1);
    PartikelRect[HURRITEILE].left = 10;
    PartikelRect[HURRITEILE].right = 22;
    PartikelRect[HURRITEILE].top = 10;
    PartikelRect[HURRITEILE].bottom = 22;

    // HurriTeile for Player 2
    // DKS - Player 2 sprite is blue, so I added separate particles and particle art for them
    //      that are colored blue, using some unused space between particles 86-100
    PartikelGrafix[HURRITEILE_P2].LoadImage("p2_hurri-teile.png", 217, 32, 31, 32, 7, 1);
    PartikelRect[HURRITEILE_P2].left = 10;
    PartikelRect[HURRITEILE_P2].right = 22;
    PartikelRect[HURRITEILE_P2].top = 10;
    PartikelRect[HURRITEILE_P2].bottom = 22;

    // Kleiner blauer Boulder
    PartikelGrafix[BOULDER_SMALL].LoadImage("boulder_small.png", 60, 33, 12, 11, 5, 3);
    PartikelRect[BOULDER_SMALL].left = 0;
    PartikelRect[BOULDER_SMALL].right = 12;
    PartikelRect[BOULDER_SMALL].top = 0;
    PartikelRect[BOULDER_SMALL].bottom = 11;

    // Der Wasserspritzer, wenn der Spieler aus dem Wasser hopst
    PartikelGrafix[WASSER_SPRITZER].LoadImage("wasserspritzer.png", 64, 128, 40, 128, 1, 1);
    PartikelRect[WASSER_SPRITZER].left = 0;
    PartikelRect[WASSER_SPRITZER].right = 40;
    PartikelRect[WASSER_SPRITZER].top = 0;
    PartikelRect[WASSER_SPRITZER].bottom = 128;

    // Der Wasserspritzer, wenn der Spieler ins Wasser hopst
    // DKS - Corrected dimensions from 40x142 to 64x128, to match actual image file:
    //      NOTE: This particle type does not appear to be used, looking at Player.cpp
    PartikelGrafix[WASSER_SPRITZER2].LoadImage("wasserspritzer.png", 64, 128, 40, 142, 1, 1);
    PartikelRect[WASSER_SPRITZER2].left = 0;
    PartikelRect[WASSER_SPRITZER2].right = 64;
    PartikelRect[WASSER_SPRITZER2].top = 0;
    PartikelRect[WASSER_SPRITZER2].bottom = 128;

    // Schrott der Lava Krabbe
    PartikelGrafix[LAVAKRABBE_KOPF].LoadImage("lavakrabbe_teile.png", 80, 40, 40, 40, 2, 1);
    PartikelRect[LAVAKRABBE_KOPF].left = 12;
    PartikelRect[LAVAKRABBE_KOPF].right = 28;
    PartikelRect[LAVAKRABBE_KOPF].top = 12;
    PartikelRect[LAVAKRABBE_KOPF].bottom = 28;

    // Spinnenteile
    PartikelGrafix[SPIDERPARTS].LoadImage("spiderparts.png", 192, 40, 48, 40, 4, 1);
    PartikelRect[SPIDERPARTS].left = 0;
    PartikelRect[SPIDERPARTS].right = 48;
    PartikelRect[SPIDERPARTS].top = 0;
    PartikelRect[SPIDERPARTS].bottom = 36;

    // Kettenteile
    PartikelGrafix[KETTENTEILE].LoadImage("kettenteile.png", 126, 50, 42, 50, 3, 1);
    PartikelRect[KETTENTEILE].left = 10;
    PartikelRect[KETTENTEILE].right = 40;
    PartikelRect[KETTENTEILE].top = 10;
    PartikelRect[KETTENTEILE].bottom = 32;

    // Kettenteile 4
    PartikelGrafix[KETTENTEILE4].LoadImage("kettenteil2.png", 11, 21, 11, 21, 1, 1);
    PartikelRect[KETTENTEILE4].left = 0;
    PartikelRect[KETTENTEILE4].right = 11;
    PartikelRect[KETTENTEILE4].top = 0;
    PartikelRect[KETTENTEILE4].bottom = 21;

    // Regentropfen
    PartikelGrafix[REGENTROPFEN].LoadImage("rain.png", 16, 32, 16, 32, 1, 1);
    PartikelRect[REGENTROPFEN].left = 0;
    PartikelRect[REGENTROPFEN].right = 16;
    PartikelRect[REGENTROPFEN].top = 0;
    PartikelRect[REGENTROPFEN].bottom = 28;

    // Schutzschild
    PartikelGrafix[SHIELD].LoadImage("shield.png", 48, 48, 48, 48, 1, 1);
    PartikelRect[SHIELD].left = 0;
    PartikelRect[SHIELD].right = 48;
    PartikelRect[SHIELD].top = 0;
    PartikelRect[SHIELD].bottom = 48;

    // Rauch des Rundumschusses des evil hurri
    PartikelGrafix[EVILROUNDSMOKE].LoadImage("evilroundsmoke.png", 12, 12, 12, 12, 1, 1);
    PartikelRect[EVILROUNDSMOKE].left = 0;
    PartikelRect[EVILROUNDSMOKE].right = 12;
    PartikelRect[EVILROUNDSMOKE].top = 0;
    PartikelRect[EVILROUNDSMOKE].bottom = 12;

    // Leuchteffekt für den Stelsack laser
    PartikelGrafix[STELZFLARE].LoadImage("giantspiderflare.png", 128, 128, 128, 128, 1, 1);
    PartikelRect[STELZFLARE].left = 0;
    PartikelRect[STELZFLARE].right = 128;
    PartikelRect[STELZFLARE].top = 0;
    PartikelRect[STELZFLARE].bottom = 128;

    // Kringel laden
    PartikelGrafix[KRINGELSECRET].LoadImage("kringel.png", 48, 12, 12, 12, 4, 1);
    PartikelRect[KRINGELSECRET].left = 0;
    PartikelRect[KRINGELSECRET].right = 12;
    PartikelRect[KRINGELSECRET].top = 0;
    PartikelRect[KRINGELSECRET].bottom = 12;

    // Kringel laden
    PartikelGrafix[KRINGEL].LoadImage("kringel.png", 48, 12, 12, 12, 4, 1);
    PartikelRect[KRINGEL].left = 0;
    PartikelRect[KRINGEL].right = 12;
    PartikelRect[KRINGEL].top = 0;
    PartikelRect[KRINGEL].bottom = 12;

    // Fog laden
    PartikelGrafix[FOG].LoadImage("dust.png", 30, 10, 10, 10, 3, 1);
    PartikelRect[FOG].left = 0;
    PartikelRect[FOG].right = 10;
    PartikelRect[FOG].top = 0;
    PartikelRect[FOG].bottom = 10;

    // Partikel für die Turbine
    PartikelGrafix[TURBINESMOKE].LoadImage("turbinesmoke.png", 24, 8, 8, 8, 3, 1);
    PartikelRect[TURBINESMOKE].left = 0;
    PartikelRect[TURBINESMOKE].right = 8;
    PartikelRect[TURBINESMOKE].top = 0;
    PartikelRect[TURBINESMOKE].bottom = 8;

    // MiniFlare
    PartikelGrafix[MINIFLARE].LoadImage("miniflare.png", 32, 32, 32, 32, 1, 1);
    PartikelRect[MINIFLARE].left = 0;
    PartikelRect[MINIFLARE].right = 32;
    PartikelRect[MINIFLARE].top = 0;
    PartikelRect[MINIFLARE].bottom = 32;

    // Leuchten bei Granateneinschlag
    PartikelGrafix[GRENADEFLARE].LoadImage("grenadeflare.png", 256, 256, 256, 256, 1, 1);
    PartikelRect[GRENADEFLARE].left = 0;
    PartikelRect[GRENADEFLARE].right = 256;
    PartikelRect[GRENADEFLARE].top = 0;
    PartikelRect[GRENADEFLARE].bottom = 256;

    // Leuchten bei Explosion
    PartikelGrafix[EXPLOSIONFLARE].LoadImage("lavaflare.png", 120, 120, 120, 120, 1, 1);
    PartikelRect[EXPLOSIONFLARE].left = 0;
    PartikelRect[EXPLOSIONFLARE].right = 120;
    PartikelRect[EXPLOSIONFLARE].top = 0;
    PartikelRect[EXPLOSIONFLARE].bottom = 120;

    // Leuchten bei Explosion2
    PartikelGrafix[EXPLOSIONFLARE2].LoadImage("lavaflare.png", 120, 120, 120, 120, 1, 1);
    PartikelRect[EXPLOSIONFLARE2].left = 0;
    PartikelRect[EXPLOSIONFLARE2].right = 120;
    PartikelRect[EXPLOSIONFLARE2].top = 0;
    PartikelRect[EXPLOSIONFLARE2].bottom = 120;

    // Halswirbel des MetalHead laden
    PartikelGrafix[HALSWIRBEL].LoadImage("metalhead_halsteil.png", 64, 16, 64, 16, 1, 1);
    PartikelRect[HALSWIRBEL].left = 0;
    PartikelRect[HALSWIRBEL].right = 64;
    PartikelRect[HALSWIRBEL].top = 0;
    PartikelRect[HALSWIRBEL].bottom = 64;

    // Kaputte Turbine des MetalHead laden
    PartikelGrafix[KAPUTTETURBINE].LoadImage("metalhead_turbine.png", 98, 49, 98, 49, 1, 1);
    PartikelRect[KAPUTTETURBINE].left = 0;
    PartikelRect[KAPUTTETURBINE].right = 98;
    PartikelRect[KAPUTTETURBINE].top = 0;
    PartikelRect[KAPUTTETURBINE].bottom = 49;

    // Schleim
    PartikelGrafix[SCHLEIM].LoadImage("schleim.png", 32, 8, 8, 8, 4, 1);
    PartikelRect[SCHLEIM].left = 1;
    PartikelRect[SCHLEIM].right = 7;
    PartikelRect[SCHLEIM].top = 1;
    PartikelRect[SCHLEIM].bottom = 7;

    // Schleim2
    PartikelGrafix[SCHLEIM2].LoadImage("schleim2.png", 32, 8, 8, 8, 4, 1);
    PartikelRect[SCHLEIM2].left = 1;
    PartikelRect[SCHLEIM2].right = 7;
    PartikelRect[SCHLEIM2].top = 1;
    PartikelRect[SCHLEIM2].bottom = 7;

    // ShockWelle bei Spieler Explosion
    PartikelGrafix[SHOCKEXPLOSION].LoadImage("shockexplosion.png", 128, 128, 128, 128, 1, 1);
    PartikelRect[SHOCKEXPLOSION].left = 0;
    PartikelRect[SHOCKEXPLOSION].right = 128;
    PartikelRect[SHOCKEXPLOSION].top = 0;
    PartikelRect[SHOCKEXPLOSION].bottom = 128;

    // Leuchten bei Aufprall des Schusses
    PartikelGrafix[SHOTFLARE].LoadImage("shotflare.png", 16, 16, 16, 16, 1, 1);
    PartikelRect[SHOTFLARE].left = 0;
    PartikelRect[SHOTFLARE].right = 16;
    PartikelRect[SHOTFLARE].top = 0;
    PartikelRect[SHOTFLARE].bottom = 16;

    // Leuchten bei Aufprall des Schusses
    PartikelGrafix[SHOTFLARE2].LoadImage("shotflare.png", 16, 16, 16, 16, 1, 1);
    PartikelRect[SHOTFLARE2].left = 0;
    PartikelRect[SHOTFLARE2].right = 16;
    PartikelRect[SHOTFLARE2].top = 0;
    PartikelRect[SHOTFLARE2].bottom = 16;

    // Leuchten beim Einsammeln eines Extras
    PartikelGrafix[EXTRACOLLECTED].LoadImage("extracollected.png", 32, 32, 32, 32, 1, 1);
    PartikelRect[EXTRACOLLECTED].left = 0;
    PartikelRect[EXTRACOLLECTED].right = 32;
    PartikelRect[EXTRACOLLECTED].top = 0;
    PartikelRect[EXTRACOLLECTED].bottom = 32;

    // Leuchten beim Einsammeln eines Diamants
    PartikelGrafix[DIAMANTCOLLECTED].LoadImage("diamantcollected.png", 29, 29, 29, 29, 1, 1);
    PartikelRect[DIAMANTCOLLECTED].left = 0;
    PartikelRect[DIAMANTCOLLECTED].right = 29;
    PartikelRect[DIAMANTCOLLECTED].top = 0;
    PartikelRect[DIAMANTCOLLECTED].bottom = 29;

    // Lila
    PartikelGrafix[LILA].LoadImage("spidershotsmoke.png", 24, 24, 24, 24, 1, 1);
    PartikelRect[LILA].left = 0;
    PartikelRect[LILA].right = 29;
    PartikelRect[LILA].top = 0;
    PartikelRect[LILA].bottom = 29;

    // Rauch des Drachen
    PartikelGrafix[DRACHE_SMOKE].LoadImage("drache_smoke.png", 60, 60, 60, 60, 1, 1);
    PartikelRect[DRACHE_SMOKE].left = 0;
    PartikelRect[DRACHE_SMOKE].right = 60;
    PartikelRect[DRACHE_SMOKE].top = 0;
    PartikelRect[DRACHE_SMOKE].bottom = 60;

    // Rauch des Feuerballs
    PartikelGrafix[FIREBALL_SMOKE].LoadImage("fireball_smoke.png", 24, 24, 24, 24, 1, 1);
    PartikelRect[FIREBALL_SMOKE].left = 0;
    PartikelRect[FIREBALL_SMOKE].right = 24;
    PartikelRect[FIREBALL_SMOKE].top = 0;
    PartikelRect[FIREBALL_SMOKE].bottom = 24;

    // Leuchten beim Einsammeln eines Extras
    PartikelGrafix[LASERFLARE].LoadImage("lavaflare.png", 120, 120, 120, 120, 1, 1);
    PartikelRect[LASERFLARE].left = 0;
    PartikelRect[LASERFLARE].right = 120;
    PartikelRect[LASERFLARE].top = 0;
    PartikelRect[LASERFLARE].bottom = 120;

    // Reguläre Explosion
    PartikelGrafix[EXPLOSION_REGULAR].LoadImage("explosion-regular.png", 380, 304, 76, 76, 5, 4);
    PartikelRect[EXPLOSION_REGULAR].left = 0;
    PartikelRect[EXPLOSION_REGULAR].right = 76;
    PartikelRect[EXPLOSION_REGULAR].top = 0;
    PartikelRect[EXPLOSION_REGULAR].bottom = 76;

    // Reguläre Explosion
    PartikelGrafix[EXPLOSION_TRACE_END].LoadImage("explosion-regular.png", 380, 304, 76, 76, 5, 4);
    PartikelRect[EXPLOSION_TRACE_END].left = 0;
    PartikelRect[EXPLOSION_TRACE_END].right = 76;
    PartikelRect[EXPLOSION_TRACE_END].top = 0;
    PartikelRect[EXPLOSION_TRACE_END].bottom = 76;

    // Staub
    PartikelGrafix[DUST].LoadImage("dust.png", 30, 10, 10, 10, 3, 1);
    PartikelRect[DUST].left = 0;
    PartikelRect[DUST].right = 10;
    PartikelRect[DUST].top = 0;
    PartikelRect[DUST].bottom = 10;

    // Staub
    PartikelGrafix[LAVADUST].LoadImage("dust.png", 30, 10, 10, 10, 3, 1);
    PartikelRect[LAVADUST].left = 0;
    PartikelRect[LAVADUST].right = 10;
    PartikelRect[LAVADUST].top = 0;
    PartikelRect[LAVADUST].bottom = 10;

    // Schrott
    // DKS - Corrected dimensions from 200x200 to 200x160, to match actual image file:
    PartikelGrafix[SCHROTT1].LoadImage("schrott1.png", 200, 160, 40, 40, 5, 4);
    PartikelRect[SCHROTT1].left = 10;
    PartikelRect[SCHROTT1].right = 30;
    PartikelRect[SCHROTT1].top = 10;
    PartikelRect[SCHROTT1].bottom = 30;

    PartikelGrafix[SCHROTT2].LoadImage("schrott2.png", 150, 120, 30, 30, 5, 4);
    PartikelRect[SCHROTT2].left = 5;
    PartikelRect[SCHROTT2].right = 25;
    PartikelRect[SCHROTT2].top = 5;
    PartikelRect[SCHROTT2].bottom = 25;
}

// --------------------------------------------------------------------------------------
// Destruktor : Löschen der ganzen Liste und Freigabe der Partikel-Grafiken
// --------------------------------------------------------------------------------------

PartikelsystemClass::~PartikelsystemClass() {
    // Partikel-Liste komplett leeren
    ClearAll();
}

// --------------------------------------------------------------------------------------
// Partikel "Art" hinzufügen
// --------------------------------------------------------------------------------------

// DKS - Modified PushPartikel to:
//      1.) Fix potential memory leak (called CreatePartikel() but didn't free
//          memory if there were too many particles already.
//      2.) Support the now-singly-linked particle list
//      3.) Support new optional pooled memory manager
//      4.) Not return a value
//      5.) Increment NumPartikel *before* calling CreatePartikel(), which can
//          sometimes push particles of its own and overflow new memory pool.
bool PartikelsystemClass::PushPartikel(float x, float y, int Art, PlayerClass *pParent) {
    if (NumPartikel >= MAX_PARTIKEL)  // Grenze überschritten ?
        return false;

    // DKS - moved this detail option check here (from inside CreatePartikel) to fix potential memory leak below:
    if (options_Detail < DETAIL_HIGH &&
        (Art == GRENADEFLARE || Art == SHOTFLARE || Art == SHOTFLARE2 || Art == EXPLOSIONFLARE))
        return false;

#ifdef USE_NO_MEMPOOLING
    PartikelClass *pNew = new PartikelClass;  // Neuer zu erstellender Partikel
#else
    PartikelClass *pNew = particle_pool.alloc();
#endif

#ifndef NDEBUG
    if (!pNew) {
        Protokoll << "WARNING: could not allocate memory for particle in PushPartikel()" << std::endl;
        return false;
    }
#endif

    // DKS - Moved this here from end of function: calling CreatePartikel() for some
    //      particle types will push even more particles, before NumPartikel has
    //      been updated. When using a fixed memory pool like I've added, we must
    //      be very careful to limit total allocations.
    NumPartikel++;

    pNew->CreatePartikel(x, y, Art, pParent);  // neuen Partikel erzeugen
    pNew->pNext = nullptr;

    if (pStart == nullptr)
        pStart = pNew;

    if (pEnd)
        pEnd->pNext = pNew;

    pEnd = pNew;

    return true;
}

// --------------------------------------------------------------------------------------
// Bestimmten Partikel der Liste löschen
// --------------------------------------------------------------------------------------
// DKS - Added new function DelNode() that supports new singly-linked list.
//      It is now up to the caller to splice the list, this blindly deletes what is passed
//      to it and returns the pointer that was in pPtr->pNext, or NULL if pPtr was NULL.
PartikelClass *PartikelsystemClass::DelNode(PartikelClass *pPtr) {
    PartikelClass *pNext = nullptr;
    if (pPtr != nullptr)  // zu löschender Partikel existiert
    {
        pNext = pPtr->pNext;

        if (pStart == pPtr)
            pStart = pNext;

            // DKS - added support for new, fast pooled mem-manager:
#ifdef USE_NO_MEMPOOLING
        delete (pPtr);  // Speicher freigeben
#else
        particle_pool.free(pPtr);
#endif
        NumPartikel--;  // Partikelzahl verringern
    }
    return pNext;
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste löschen
// --------------------------------------------------------------------------------------
void PartikelsystemClass::ClearAll() {
    if (pStart) {
        PartikelClass *pNext = pStart->pNext;
        while (pNext) {
            pNext = DelNode(pNext);
        }
        DelNode(pStart);
    }
    pStart = pEnd = nullptr;

#ifndef NDEBUG
    if (NumPartikel != 0) {
        Protokoll << "ERROR: poss. mem leak / mem. corruption in linked list of particles" << std::endl;
    }
#endif

#ifndef USE_NO_MEMPOOLING
    particle_pool.reinit();
#endif

    // Just to be safe:
    NumPartikel = 0;
}

// --------------------------------------------------------------------------------------
// Zahl der Partikel zurückliefern
// --------------------------------------------------------------------------------------

int PartikelsystemClass::GetNumPartikel() const {
    return NumPartikel;
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste nur anzeigen
// --------------------------------------------------------------------------------------

void PartikelsystemClass::DrawOnly() {
    //----- Partikel, die normal oder mit Alphablending gerendert werden, durchlaufen

    PartikelClass *pTemp = pStart;  // Anfang der Liste
    CurrentPartikelTexture = -1;    // Aktuelle Textur gibt es noch keine
    DrawMode = DrawModeEnum::NORMAL;
    DirectGraphics.SetColorKeyMode();
    while (pTemp != nullptr)  // Noch nicht alle durch ?
    {
        if (pTemp->PartikelArt < ADDITIV_GRENZE)
            pTemp->Render();

        pTemp = pTemp->pNext;  // Und nächsten Partikel anhandeln
    }

    //----- Partikel, die mit additivem Alphablending gerendert werden, durchlaufen

    pTemp = pStart;
    CurrentPartikelTexture = -1;  // Aktuelle Textur gibt es noch keine
    DirectGraphics.SetAdditiveMode();
    while (pTemp != nullptr)  // Noch nicht alle durch ?
    {
        if (pTemp->PartikelArt >= ADDITIV_GRENZE)
            pTemp->Render();

        pTemp = pTemp->pNext;  // Und nächsten Partikel anhandeln
    }

    DirectGraphics.SetColorKeyMode();

    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste animieren und anzeigen, aber speziale
// --------------------------------------------------------------------------------------
void PartikelsystemClass::DoPartikelSpecial(bool ShowThem) {
    if (Console.Showing) {
        DrawOnly();
        return;
    }

    PartikelClass *pCurr = pStart;
    PartikelClass *pPrev = nullptr;

    CurrentPartikelTexture = -1;  // Aktuelle Textur gibt es noch keine
    DrawMode = DrawModeEnum::NORMAL;

    //----- Partikel, die normal oder mit Alphablending gerendert werden, durchlaufen

    DirectGraphics.SetColorKeyMode();

    while (pCurr != nullptr) {
        if (!ShowThem && pCurr->PartikelArt < ADDITIV_GRENZE) {
            pCurr->Run();
            if (pCurr->Lebensdauer > 0.0f)
                pCurr->Render();
        }

        if (pCurr->Lebensdauer > 0.0f) {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        } else {
            // Particle's time to die..

            // If this is the last node in the list, update the main class's pEnd pointer
            if (pEnd == pCurr)
                pEnd = pPrev;

            pCurr = DelNode(pCurr);
            // pCurr now points to the node after the one deleted

            if (pPrev) {
                // This is not the first node in the list, so
                // splice this node onto the previous one
                pPrev->pNext = pCurr;
            }
        }
    }

    //----- Partikel, die mit additivem Alphablending gerendert werden, durchlaufen

    pCurr = pStart;
    pPrev = nullptr;
    CurrentPartikelTexture = -1;  // Aktuelle Textur gibt es noch keine
    DirectGraphics.SetAdditiveMode();
    while (pCurr != nullptr) {
        if ((ShowThem && (pCurr->PartikelArt == SCHNEEFLOCKE_END || pCurr->PartikelArt == EXPLOSION_TRACE_END)) ||
            (!ShowThem && pCurr->PartikelArt >= ADDITIV_GRENZE)) {
            pCurr->Run();  // Partikel animieren/bewegen

            if (pCurr->Lebensdauer > 0.0f)
                pCurr->Render();
        }

        if (pCurr->Lebensdauer > 0.0f) {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        } else {
            // Particle's time to die..

            // If this is the last node in the list, update the main class's pEnd pointer
            if (pEnd == pCurr)
                pEnd = pPrev;

            pCurr = DelNode(pCurr);
            // pCurr now points to the node after the one deleted

            if (pPrev != nullptr) {
                // This is not the first node in the list, so
                // splice this node onto the previous one
                pPrev->pNext = pCurr;
            }
        }
    }

    DirectGraphics.SetColorKeyMode();

    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
}

// --------------------------------------------------------------------------------------
// Alle Partikel der Liste animieren und anzeigen
// --------------------------------------------------------------------------------------
void PartikelsystemClass::DoPartikel() {
    if (Console.Showing) {
        DrawOnly();
        return;
    }

    PartikelClass *pCurr = pStart;
    PartikelClass *pPrev = nullptr;
    CurrentPartikelTexture = -1;  // Aktuelle Textur gibt es noch keine
    DrawMode = DrawModeEnum::NORMAL;

    //----- Partikel, die normal oder mit Alphablending gerendert werden, durchlaufen

    DirectGraphics.SetColorKeyMode();
    while (pCurr != nullptr) {
        if (pCurr->PartikelArt < ADDITIV_GRENZE) {
            pCurr->Run();  // Partikel animieren/bewegen

            if (pCurr->Lebensdauer > 0.0f) {  // ggf Partikel löschen (bei Lebensdauer == 0)
                // DKS - Only render if Lebensdauer > 0.0f:
                pCurr->Render();
            }
        }

        if (pCurr->Lebensdauer > 0.0f) {  // ggf Partikel löschen (bei Lebensdauer == 0)
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        } else {
            // Particle's time to die..
            // If this is the last node in the list, update the main class's pEnd pointer
            if (pEnd == pCurr) {
                pEnd = pPrev;
            }
            pCurr = DelNode(pCurr);
            // pCurr now points to the node after the one deleted

            if (pPrev) {
                // This is not the first node in the list, so
                // splice this node onto the previous one
                pPrev->pNext = pCurr;
            }
        }
    }

    pCurr = pStart;
    pPrev = nullptr;
    CurrentPartikelTexture = -1;  // Aktuelle Textur gibt es noch keine
    DirectGraphics.SetAdditiveMode();

    while (pCurr != nullptr) {
        if (pCurr->PartikelArt >= ADDITIV_GRENZE) {
            pCurr->Run();
            if (pCurr->Lebensdauer > 0.0f)
                pCurr->Render();
        }

        if (pCurr->Lebensdauer > 0.0f) {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        } else {
            // Particle's time to die..

            // If this is the last node in the list, update the main class's pEnd pointer
            if (pEnd == pCurr) {
                pEnd = pPrev;
            }
            pCurr = DelNode(pCurr);
            // pCurr now points to the node after the one deleted

            if (pPrev != nullptr) {
                // This is not the first node in the list, so
                // splice this node onto the previous one
                pPrev->pNext = pCurr;
            }
        }
    }

    DirectGraphics.SetColorKeyMode();
    // Normale Projektions-Matrix wieder herstellen
    matWorld = glm::mat4x4(1.0f);
    g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
    load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif
}

// --------------------------------------------------------------------------------------
// Blitz und Döner zeigen ;)
// --------------------------------------------------------------------------------------

void PartikelsystemClass::DoThunder() {
    if (ThunderAlpha > 0.0f) {
        D3DCOLOR col = D3DCOLOR_RGBA(ThunderColor[0], ThunderColor[1], ThunderColor[2], static_cast<int>(ThunderAlpha));
        RenderRect(0, 0, 640, 480, col);
        ThunderAlpha -= Timer.sync(40.0f);
    }
}

// --------------------------------------------------------------------------------------
// PowerUp Partikel löschen, falls schon welche vorhanden
// damit kein so bunter bonbon scheiss entsteht, wenn mehrere extras gleichzeitig aufgepowert werden
// --------------------------------------------------------------------------------------
void PartikelsystemClass::ClearPowerUpEffects() {
    PartikelClass *pCurr = pStart;
    PartikelClass *pPrev = nullptr;

    while (pCurr != nullptr) {
        if (pCurr->PartikelArt == KRINGEL) {
            // If this is the last node in the list, update the main class's pEnd pointer
            if (pEnd == pCurr) {
                pEnd = pPrev;
            }
            pCurr = DelNode(pCurr);
            // pCurr now points to the node after the one deleted

            if (pPrev) {
                // This is not the first node in the list, so
                // splice this node onto the previous one
                pPrev->pNext = pCurr;
            }
        } else {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        }
    }
}

// --------------------------------------------------------------------------------------
// Je nach Detailstufe wird ein anderer Wert für MAX_PARTIKEL gesetzt
// --------------------------------------------------------------------------------------

void PartikelsystemClass::SetParticleCount() {
    switch (options_Detail) {
        case DETAIL_LOW:
            MAX_PARTIKEL = 2000;
            break;

        case DETAIL_MEDIUM:
            MAX_PARTIKEL = 3000;
            break;

        case DETAIL_HIGH:
            MAX_PARTIKEL = 4000;
            break;

        case DETAIL_MAXIMUM:
            MAX_PARTIKEL = 5000;
            break;
    }
}
