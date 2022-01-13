// Datei : Projectiles.cpp

// --------------------------------------------------------------------------------------
//
// Projektile (Schüsse) für Hurrican
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include <cmath>

#include "Console.hpp"
#include "DX8Font.hpp"
#include "DX8Sound.hpp"
#include "Gameplay.hpp"
#include "Gegner_Helper.hpp"
#include "Globals.hpp"
#include "Logdatei.hpp"
#include "Partikelsystem.hpp"
#include "Player.hpp"
#include "Projectiles.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

// DKS - This was unnecessarily high. Changed to the correct figure:
//constexpr int MAX_SHOTGFX = 300;  // Maximale Anzahl verschiedener Grafiken
constexpr int MAX_SHOTGFX = 264;  // Maximale Anzahl verschiedener Grafiken

// --------------------------------------------------------------------------------------
// Variablen
// --------------------------------------------------------------------------------------

DirectGraphicsSprite ProjectileGrafix[MAX_SHOTGFX];  // Grafiken der Schüsse
RECT_struct ShotRect[MAX_SHOTGFX];                          // Rechtecke für Level Kollision
int CurrentShotTexture;                              // Aktuelle Textur der Schüsse
uint32_t bo, bu, bl, br;                             // Blockwerte um den aktuellen Schuss herum

float WinkelUebergabe;  // Extra "Parameter" für PushProjectile

// --------------------------------------------------------------------------------------
// ProjectileKlasse Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

// DKS - Moved constructor assignments to CreateShot(), since there's no need to have
//      a constructor when CreateShot was always called immediately after allocating a shot.
//      Constructor is now empty inline in header. The original constructor was performing
//      redundant or unnecessary assignments.
//      Note that PushBlitzBeam() is an an exception and has been modified to account
//      for this change.
#if 0
ProjectileClass::ProjectileClass()
{
    xPos	= 0.0f;
    yPos	= 0.0f;
    xSpeed	= 0.0f;
    ySpeed	= 0.0f;
    xAcc	= 0.0f;
    yAcc	= 0.0f;
    Counter = 0.0f;
    Winkel  = -10000.0f;					// wird von den wenigsten Schüssen verwendet
}
#endif  // 0

// --------------------------------------------------------------------------------------
// Schuss vom Typ "Art" erzeugen
// --------------------------------------------------------------------------------------

void ProjectileClass::CreateShot(float x, float y, int Art, PlayerClass *pTemp) {
    xPos = x;
    yPos = y;
    xPosOld = x;
    yPosOld = y;
    ShotArt = Art;
    AnimPhase = 0;
    AnimCount = 0.0f;
    AnimEnde = 0;  // Standardmässig von einem nicht animierten Schuss ausgehen
    AnimSpeed = 0.0f;
    BounceWalls = false;
    CheckBlock = true;
    HasGlow = false;
    ExplodeOnImpact = true;
    OwnDraw = false;

    // DKS - Moved the class's constructor assignments here, since there's no need to
    //      have a constructor when CreateShot() is always called immediately after
    //      allocating a new shot. PushBlitzBeam() is an exception and has been
    //      modified accordingly.
    xSpeed = 0.0f;
    ySpeed = 0.0f;
    xAcc = 0.0f;
    yAcc = 0.0f;
    Counter = 0.0f;
    Winkel = -10000.0f;  // wird von den wenigsten Schüssen verwendet

    // Winkel zwischen 0 und 360
    while (WinkelUebergabe > 360.0f)
        WinkelUebergabe -= 360.0f;
    while (WinkelUebergabe < 0.0f)
        WinkelUebergabe += 360.0f;

    pParent = pTemp;

    switch (Art) {
        case DIAMONDSHOT:  // Diamant, der auf den Punisher fliegt
        {
            // DKS - Fixed uninitialized variable:
            // GegnerClass *pAim, *pTemp2;
            GegnerClass *pAim = nullptr;
            GegnerClass *pTemp2 = Gegner.pStart;

            while (pTemp2 != nullptr) {
                if (pTemp2->GegnerArt == PUNISHER) {
                    pAim = pTemp2;
                    pTemp2 = nullptr;
                } else
                    pTemp2 = pTemp2->pNext;
            }

            if (pAim == nullptr)
                return;

            // DKS - Converted to float:

            float absx = pAim->xPos + 85 - xPos;  // Differenz der x
            float absy = pAim->yPos + 40 - yPos;  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 85;                                     // Geschwindigkeit ist 4 fach

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

            xSpeed = absx;
            ySpeed = absy;
            // DKS - off-by-one error:
            // AnimEnde = 9;
            AnimEnde = 8;
            AnimSpeed = 0.75f;

            Damage = 1000;
            DamagePlayer = false;
        } break;

        case SKELETOR_SHOT:  // Schuss vom Skeletor
        {
            Damage = 4;
            DamagePlayer = true;
            Winkel = WinkelUebergabe;

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 100);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 100);
            xSpeed = cos_deg(w) * 100.0f;
            ySpeed = sin_deg(w) * 100.0f;

        } break;

        case SPREADSHOT:  // SpreadShot des Spielers geradeaus
        {
            Damage = 6 - (static_cast<int>(pParent->CurrentWeaponLevel[pParent->SelectedWeapon] * 0.75f));
            DamagePlayer = false;
            Winkel = WinkelUebergabe;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 50);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 50);
            xSpeed = cos_deg(w) * 50.0f;
            ySpeed = sin_deg(w) * 50.0f;

            if (w == -90)
                xSpeed = 0.0f;

        } break;

        case SPREADSHOT2:  // SpreadShot des Spielers geradeaus
        {
            xPos -= 2.0f;
            yPos -= 2.0f;
            Damage = 6;
            DamagePlayer = false;
            Winkel = WinkelUebergabe;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 50);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 50);
            xSpeed = cos_deg(w) * 50.0f;
            ySpeed = sin_deg(w) * 50.0f;

            if (w == -90)
                xSpeed = 0.0f;

        } break;

        case SPREADSHOTBIG:  // SpreadShot des Spielers geradeaus
        {
            Damage = 12;
            DamagePlayer = false;
            Winkel = WinkelUebergabe;
            HasGlow = true;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 50);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 50);
            xSpeed = cos_deg(w) * 50.0f;
            ySpeed = sin_deg(w) * 50.0f;

            if (w == -90)
                xSpeed = 0.0f;

        } break;

        case SPREADSHOTBIG2:  // SpreadShot des Spielers geradeaus
        {
            xPos -= 4;
            yPos -= 4;
            Damage = 16;
            DamagePlayer = false;
            Winkel = WinkelUebergabe;
            HasGlow = true;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 50);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 50);
            xSpeed = cos_deg(w) * 50.0f;
            ySpeed = sin_deg(w) * 50.0f;

            if (w == -90)
                xSpeed = 0.0f;

        } break;

        case LASERSHOT:  // Laser des Spielers
        {
            Damage = 8;
            DamagePlayer = false;
            Winkel = WinkelUebergabe;
            HasGlow = true;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 60);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 60);
            xSpeed = cos_deg(w) * 60.0f;
            ySpeed = sin_deg(w) * 60.0f;

            if (w == -90)
                xSpeed = 0.0f;

            ExplodeOnImpact = false;
        } break;

        case LASERSHOT2:  // Laser des Spielers
        {
            Damage = 10;
            DamagePlayer = false;
            Winkel = WinkelUebergabe;
            HasGlow = true;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 60);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 60);
            xSpeed = cos_deg(w) * 60.0f;
            ySpeed = sin_deg(w) * 60.0f;

            if (w == -90)
                xSpeed = 0.0f;

            // DKS - off-by-one error:
            // AnimEnde = 10;
            AnimEnde = 9;
            AnimSpeed = 0.25f;

            ExplodeOnImpact = false;
        } break;

        case LASERSHOTBIG:  // Riesen Laser des Spielers
        {
            Damage = 12;
            DamagePlayer = false;
            Winkel = WinkelUebergabe;
            HasGlow = true;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 60);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 60);
            xSpeed = cos_deg(w) * 60.0f;
            ySpeed = sin_deg(w) * 60.0f;

            if (w == -90)
                xSpeed = 0.0f;

            ExplodeOnImpact = false;
        } break;

        case LASERSHOTBIG2:  // Riesen Laser des Spielers
        {
            Damage = 16;
            DamagePlayer = false;
            Winkel = WinkelUebergabe;
            HasGlow = true;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 60);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 60);
            xSpeed = cos_deg(w) * 60.0f;
            ySpeed = sin_deg(w) * 60.0f;

            if (w == -90)
                xSpeed = 0.0f;

            // DKS - off-by-one error:
            // AnimEnde = 10;
            AnimEnde = 9;
            AnimSpeed = 0.25f;

            ExplodeOnImpact = false;
        } break;

        case BOUNCESHOT1:  // BounceShot in gross (teilt sich in 2 mittlere Schüsse auf)
        {
            Winkel = WinkelUebergabe;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 65);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 65);
            xSpeed = cos_deg(w) * 65.0f;
            ySpeed = sin_deg(w) * 65.0f;

            if (w == -90)
                xSpeed = 0.01f;

            DamagePlayer = false;
            Damage = 8;

            if (pParent->CurrentWeaponLevel[2] == 2)
                BounceWalls = true;

            OwnDraw = true;
        } break;

        case BOUNCESHOT2:  // BounceShot Mittel
        {
            Winkel = WinkelUebergabe;

            // je nach Waffenlevel öfters abspringen
            Damage = pParent->CurrentWeaponLevel[pParent->SelectedWeapon] * 2 - 3;

            if (Damage > 8)
                Damage = 8;

            if (pParent != nullptr) {
                if (pParent->Blickrichtung == DirectionEnum::LINKS)
                    Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);
            }

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 65);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 65);
            xSpeed = cos_deg(w) * 65.0f;
            ySpeed = sin_deg(w) * 65.0f;

            if (w == -90)
                xSpeed = 0.01f;

            DamagePlayer = false;

            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] < 5)
                BounceWalls = true;
        } break;

        case BOUNCESHOT3:  // BounceShot Klein
        {
            Winkel = WinkelUebergabe;

            if (pParent != nullptr && pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 65);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 65);
            xSpeed = cos_deg(w) * 65.0f;
            ySpeed = sin_deg(w) * 65.0f;

            if (w == -90)
                xSpeed = 0.01f;

            DamagePlayer = false;

            // je nach Waffenlevel öfters abspringen
            Damage = pParent->CurrentWeaponLevel[pParent->SelectedWeapon] - 3;

            if (Damage > 6)
                Damage = 6;

            BounceWalls = true;
        } break;

        case BOUNCESHOTBIG1:  // Riesen BounceShot in Gross
        {
            Winkel = WinkelUebergabe;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 65);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 65);
            xSpeed = cos_deg(w) * 65.0f;
            ySpeed = sin_deg(w) * 65.0f;

            if (w == -90)
                xSpeed = 0.01f;

            DamagePlayer = false;
            Damage = 20;

            if (pParent->CurrentWeaponLevel[2] == 2)
                BounceWalls = true;
        } break;

        case BOUNCESHOTBIG2:  // Riesen BounceShot Mittel
        {
            Winkel = WinkelUebergabe;

            if (pParent != nullptr && pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 65);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 65);
            xSpeed = cos_deg(w) * 65.0f;
            ySpeed = sin_deg(w) * 65.0f;

            if (w == -90)
                xSpeed = 0.01f;

            DamagePlayer = false;
            Damage = 14;

            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] < 5)
                BounceWalls = true;
        } break;

        case BOUNCESHOTBIG3:  // Riesen BounceShot Klein
        {
            Winkel = WinkelUebergabe;

            if (pParent != nullptr && pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 65);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 65);
            xSpeed = cos_deg(w) * 65.0f;
            ySpeed = sin_deg(w) * 65.0f;

            if (w == -90)
                xSpeed = 0.01f;

            DamagePlayer = false;
            Damage = 12;
            BounceWalls = true;
        } break;

        case WALKER_LASER:  // Laser des Walkers
        {
            xSpeed = -30.0f;
            ySpeed = 0.0f;
            Damage = 2;
            DamagePlayer = true;
            HasGlow = true;
            ExplodeOnImpact = false;
        } break;

        case WALKER_LASER2:  // Laser des Walkers
        {
            xSpeed = 30.0f;
            ySpeed = 0.0f;
            Damage = 2;
            DamagePlayer = true;
            HasGlow = true;
            ExplodeOnImpact = false;
            ShotArt = WALKER_LASER;
        } break;

        case SPIDER_LASER:  // Laser der Spinne
        {
            xSpeed = -60.0f;
            ySpeed = 0.0f;
            Damage = 10;
            DamagePlayer = true;
            ShotArt = WALKER_LASER;
        } break;

        case CANONBALL:  // Gechützturm Kugel links
        {
            xSpeed = -30.0f;
            ySpeed = 0.0f;
            Damage = 8;
            DamagePlayer = true;
        } break;

        case CANONBALL2:  // Gechützturm Kugel rechts
        {
            ShotArt = CANONBALL;
            xSpeed = 30.0f;
            ySpeed = 0.0f;
            Damage = 8;
            DamagePlayer = true;
        } break;

        case BOMBE:  // Spieler Bombe
        {
            // DKS - off-by-one error:
            // AnimEnde	 = 16;
            AnimEnde = 15;
            AnimSpeed = 0.5f;
            Damage = 30;
            DamagePlayer = false;
        } break;

        case BOMBEBIG:  // Fette Spieler Bombe
        {
            // DKS - off-by-one error:
            // AnimEnde	 = 16;
            AnimEnde = 15;
            AnimSpeed = 0.5f;
            Damage = 90;
            DamagePlayer = false;
        } break;

        case SUCHSCHUSS:  // Kugel des Deckenturms, der direkt auf den Spieler schiesst
        {
            PlayerClass *pAim;

            if (pParent == nullptr)
                pAim = ChooseAim();
            else
                pAim = pParent;

            // DKS - Converted to float:

            float absx = pAim->xpos + 35 - xPos;  // Differenz der x
            float absy = pAim->ypos + 40 - yPos;  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 20;                                     // Geschwindigkeit ist 4 fach

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

            xSpeed = absx;
            ySpeed = absy;

            Damage = 8;
            DamagePlayer = true;
        } break;

        case STRAIGHTSCHUSS:  // Schuss direkt nach oben (Spitter und Schienenviech)
        {
            ySpeed = -20.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case STRAIGHTSCHUSS2:  // Schuss direkt nach unten (Schienenviech)
        {
            ySpeed = 20.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case SPITTERBOMBESHOTLO: {
            ySpeed = -8.0f;
            xSpeed = -20.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case SPITTERBOMBESHOTLM: {
            xSpeed = -25.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case SPITTERBOMBESHOTLU: {
            ySpeed = 8.0f;
            xSpeed = -20.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case SPITTERBOMBESHOTRO: {
            ySpeed = -8.0f;
            xSpeed = 20.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case SPITTERBOMBESHOTRM: {
            xSpeed = 25.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case SPITTERBOMBESHOTRU: {
            ySpeed = 8.0f;
            xSpeed = 20.0f;
            Damage = 8;
            DamagePlayer = true;
            ShotArt = SUCHSCHUSS;
        } break;

        case SHIELDSPAWNER:  // Schutzschild Emitter
        {
            Damage = 10;
            DamagePlayer = false;
            ySpeed = 0.0f;
            xSpeed = 1.0f;
            // DKS - off-by-one error:
            // AnimEnde = 10;
            AnimEnde = 9;
            AnimSpeed = 1.0f;
            ExplodeOnImpact = false;
        } break;

        case SHIELDSPAWNER2:  // Schutzschild Emitter
        {
            Damage = 10;
            DamagePlayer = false;
            ySpeed = static_cast<float>(M_PI);
            xSpeed = 1.0f;
            ShotArt = SHIELDSPAWNER;
            // DKS - off-by-one error:
            // AnimEnde = 10;
            AnimEnde = 9;
            AnimSpeed = 1.0f;
            ExplodeOnImpact = false;
        } break;

        case SUCHSCHUSS2:  // Blauer Schuss der Riesenwepse, genau auf den Spieler
        {
            PlayerClass *pAim = ChooseAim();

            // DKS - Converted to float:

            float absx = pAim->xpos + 35 - xPos - 20;  // Differenz der x
            float absy = pAim->ypos + 40 - yPos - 20;  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 20;                                     // Geschwindigkeit setzen

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

            xSpeed = absx;
            ySpeed = absy;

            Damage = 12;
            DamagePlayer = true;

            AnimEnde = 3;
            AnimSpeed = 0.5f;
            HasGlow = true;
        } break;

        case TURRIEXTRAWURST: {
            Counter = WinkelUebergabe;

            ySpeed = -16.0f;
            yAcc = 1.0f;

            Damage = 50;
            DamagePlayer = true;

            AnimEnde = 24;
            AnimSpeed = 0.5f;
        } break;

        case GOLEMSCHUSS:  // Golem Suchschuss
        {
            Winkel = WinkelUebergabe;

            xSpeed = -WinkelUebergabe;
            ySpeed = -40;
            yAcc = 10.0f;
            Counter = -ySpeed;

            if (static_cast<int>(xSpeed) % 2 == 0) {
                ySpeed *= -1.0f;
                yAcc *= -1.0f;
            }

            Damage = 50;
            DamagePlayer = true;

            AnimEnde = 24;
            AnimSpeed = 0.5f;
        } break;

        case SPIDERSLOW:  // langsamer Spinnenschuss
        {
            PlayerClass *pAim = ChooseAim();

            // DKS - Converted to float:

            float absx = pAim->xpos + 35 - xPos - 20;  // Differenz der x
            float absy = pAim->ypos + 40 - yPos - 20;  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 20;                               // Geschwindigkeit setzen

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

            xSpeed = absx;
            ySpeed = absy;

            Damage = 30;
            DamagePlayer = true;

            AnimEnde = 24;
            AnimSpeed = 0.5f;
        } break;

        case SPIDERLASER:  // Feter Spinnenlaser
        {
            Damage = 10;
            DamagePlayer = true;

            AnimCount = 0.0f;
            AnimSpeed = 0.0f;
            AnimPhase = 1;
            ExplodeOnImpact = false;
            OwnDraw = true;

            ShotRect[SPIDERLASER].left = 0;
            ShotRect[SPIDERLASER].right = 0;
            ShotRect[SPIDERLASER].top = 0;
            ShotRect[SPIDERLASER].bottom = 0;
        } break;

        case SPIDERSHOT:  // Schuss der Spinne
        {
            Winkel = WinkelUebergabe;

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = -float (sin (w * PI / 180.0f) * 50);
            // ySpeed	  = float (cos (w * PI / 180.0f) * 50);
            xSpeed = -sin_deg(w) * 50.0f;
            ySpeed = cos_deg(w) * 50.0f;

            Damage = 10;
            DamagePlayer = true;
            ExplodeOnImpact = false;
        } break;

        case SPIDERSHOTLANGSAM:  // Schuss der Spinne
        {
            Winkel = WinkelUebergabe;

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = -float (sin (w * PI / 180.0f) * 25);
            // ySpeed	  = float (cos (w * PI / 180.0f) * 25);
            xSpeed = -sin_deg(w) * 25.0f;
            ySpeed = cos_deg(w) * 25.0f;

            Damage = 10;
            DamagePlayer = true;

            ShotArt = SPIDERSHOT;
        } break;

        case SPIDERSHOT2:  // Schuss der Spinne
        {
            xSpeed = -20.0f;
            ySpeed = -static_cast<float>(random(12) + 5);
            yAcc = static_cast<float>(random(10) + 5) / 5.0f;

            if (random(2) == 0) {
                ySpeed *= -1.0f;
                yAcc *= -1.0f;
            }

            Damage = 10;
            DamagePlayer = true;
            BounceWalls = true;
            AnimCount = 1.0f;
        } break;

        case SPIDERSHOT3:  // Schuss der Spinne
        {
            ySpeed = -40.0f;
            Damage = 10;
            DamagePlayer = true;
            AnimCount = 1.0f;
            ShotArt = SPIDERSHOT2;
        } break;

        case PHARAOLASER:  // Laser des Pharao Kopfes, der direkt auf den Spieler schiesst
        {
            PlayerClass *pAim = ChooseAim();

            // DKS - Converted to float:

            float absx = pAim->xpos + 35 - (xPos + 24);  // Differenz der x
            float absy = pAim->ypos + 40 - (yPos + 24);  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 40;

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;

            xSpeed = absx;
            ySpeed = absy;

            Damage = 40;
            DamagePlayer = true;

            AnimEnde = 3;
            AnimSpeed = 0.5f;
        } break;

        // DKS - NOTE: this projectile was never actually used in the game (commented out in Partikelsystem.cpp)
        case FLAMEWALL:  // Aufsteigende Feuerwand die aus der Spinne Granate entsteht
        {
            xSpeed = 40.0f;
            ySpeed = -20.0f;
            yAcc = 2.0f;

            Damage = 40;
            DamagePlayer = true;

            AnimEnde = 6;
            AnimSpeed = 1.0f;

            SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION1);
            CheckBlock = false;
            ExplodeOnImpact = false;
        } break;

        case POWERLINE:  // Powerline links
        {
            xSpeed = -80.0f;
            ySpeed = 0.0f;
            Damage = 20;
            DamagePlayer = false;
            HasGlow = true;
            ExplodeOnImpact = false;
        } break;

        case POWERLINE2:  // Powerline rechts
        {
            ShotArt = POWERLINE;
            xSpeed = 80.0f;
            ySpeed = 0.0f;
            AnimPhase = 1;
            Damage = 12;
            DamagePlayer = false;
            HasGlow = true;
            ExplodeOnImpact = false;
        } break;

        case FLAMME:  // Splitter-Flamme aus dem Fass
        {
            ShotArt = FLAMME;
            xSpeed = static_cast<float>(random(120) - 60) / 3.0f;
            ySpeed = -static_cast<float>(random(60) + 30) / 2.0f;
            yAcc = 4.0f;
            Damage = 50;
            AnimEnde = 7;
            AnimSpeed = static_cast<float>(random(20) + 20) / 100.0f;
            DamagePlayer = false;
        } break;

        case FEUERFALLE_LAVAMANN:  // Flamme des Lavamannes
        {
            AnimEnde = 29;
            AnimPhase = 6 + random(3);
            AnimSpeed = (random(3) + 10) / 20.0f;
            Damage = 1;
            ySpeed = 1.0f;
            DamagePlayer = true;
            xSpeed = static_cast<float>(random(20) - 10) / 8.0f;
            CheckBlock = false;
            ExplodeOnImpact = false;
            OwnDraw = true;

        } break;

        case FEUERFALLE:  // Flamme des Wandflammenwerfers oben
        {
            AnimEnde = 29;
            AnimSpeed = (random(3) + 10) / 20.0f;
            Damage = 1;
            DamagePlayer = true;
            ySpeed = -20.0f;
            xSpeed = static_cast<float>(random(20) - 10) / 8.0f;
            yPos -= 40;
            xPos += 5;
            ExplodeOnImpact = false;
            Winkel = static_cast<float>(random(360));
            OwnDraw = true;

        } break;

        case FIREBALL:  // Feuerball des Minidrachens
        {
            Damage = 15;
            DamagePlayer = true;
            AnimCount = 0.1f;

            Winkel = WinkelUebergabe;
            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 50);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 50);
            xSpeed = cos_deg(w) * 50.0f;
            ySpeed = sin_deg(w) * 50.0f;
        } break;

        case FIREBALL_BIG:  // Feuerball des Minidrachens
        {
            AnimCount = 0.1f;
            Damage = 30;
            DamagePlayer = true;

            Winkel = WinkelUebergabe;
            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 50);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 50);
            xSpeed = cos_deg(w) * 50.0f;
            ySpeed = sin_deg(w) * 50.0f;
        } break;

        case FIREBALL_BOMB:  // Feuerbombe des Metallkopfs
        {
            Damage = 30;
            DamagePlayer = true;

            float absx = pParent->xpos + 35 - (xPos + 50);
            float absy = pParent->ypos + 40 - (yPos + 180);

            xSpeed = absx / 10.0f;
            Damage = 10;
            ySpeed = absy / 10.0f;
            yAcc = 3.0f;

            Winkel = static_cast<float>(random(360));
        } break;

        case ROTZSHOT:  // Shuss des Rotzpotts
        {
            Damage = 12;
            DamagePlayer = true;

            Winkel = WinkelUebergabe;
            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 40);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 30);
            xSpeed = cos_deg(w) * 40.0f;
            ySpeed = sin_deg(w) * 30.0f;

            yAcc = 5.0f;
            AnimEnde = 24;
            AnimSpeed = 0.2f;

            Counter = 0.2f;
        } break;

        case GOLEMSAEULE:  // Flammensäule des Golems
        {
            Damage = 1;
            DamagePlayer = true;
            xSpeed = -static_cast<float>(random(10) + 5);
            CheckBlock = false;
            AnimCount = 0.0f;

        } break;

        case FEUERFALLE2:  // Flamme des Wandflammenwerfers rechts
        {
            ShotArt = FEUERFALLE;
            AnimEnde = 29;
            AnimSpeed = (random(3) + 10) / 20.0f;
            Damage = 1;
            DamagePlayer = true;
            ySpeed = (static_cast<float>(random(20) - 10) / 8.0f) - 1.0f;
            xSpeed = 25.0f;
            yPos -= 7 + (static_cast<float>(random(20)) / 10.0f);
            xPos += 33;
            ExplodeOnImpact = false;
            Winkel = static_cast<float>(random(360));
            OwnDraw = true;

        } break;

        case FEUERFALLE3:  // Flamme des Wandflammenwerfers unten
        {
            ShotArt = FEUERFALLE;
            AnimEnde = 29;
            AnimSpeed = (random(3) + 10) / 20.0f;
            Damage = 1;
            DamagePlayer = true;
            ySpeed = 20.0f;
            xSpeed = static_cast<float>(random(20) - 10) / 8.0f;
            yPos += 30;
            ExplodeOnImpact = false;
            Winkel = static_cast<float>(random(360));
            OwnDraw = true;

        } break;

        case FEUERFALLE4:  // Flamme des Wandflammenwerfers links
        {
            ShotArt = FEUERFALLE;
            AnimEnde = 29;
            AnimSpeed = (random(3) + 10) / 20.0f;
            Damage = 1;
            DamagePlayer = true;
            ySpeed = (static_cast<float>(random(20) - 10) / 8.0f) - 1.0f;
            xSpeed = -25.0f;
            yPos -= 15 + (static_cast<float>(random(20)) / 10.0f);
            xPos -= 28;
            ExplodeOnImpact = false;
            Winkel = static_cast<float>(random(360));
            OwnDraw = true;

        } break;

        case SPIDERFIRE:  // Flamme der Feuer Spinne (wie Suchschuss)
        {
            AnimEnde = 29;
            AnimSpeed = (random(3) + 10) / 30.0f;
            Damage = 1;
            DamagePlayer = true;

            // DKS - Converted to float:

            float absx = pParent->xpos + 35 - xPos;  // Differenz der x
            float absy = pParent->ypos + 40 - yPos;  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 20;                                    // Geschwindigkeit

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;
            xSpeed = absx;
            ySpeed = absy;

            Damage = 4;
            CheckBlock = false;
            ExplodeOnImpact = false;
            Winkel = static_cast<float>(random(360));
            OwnDraw = true;
        } break;

        case WALKERFIRE:  // Flamme des Feuer Walkers
        {
            AnimEnde = 29;
            AnimSpeed = (random(3) + 10) / 30.0f;
            Damage = 1;
            DamagePlayer = true;

            // DKS - Converted to float:

            float absx = pParent->xpos + 35 - xPos;  // Differenz der x
            float absy = pParent->ypos + 5 - yPos;   // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 22;                                     // Geschwindigkeit ist 4 fach

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

            xSpeed = absx;
            ySpeed = std::clamp(absy, -5.0f, 5.0f);

            if (xSpeed > 0.0f)
                if (xSpeed < 5.0f)
                    xSpeed = 5.0f;

            if (xSpeed < 0.0f)
                if (xSpeed > -5.0f)
                    xSpeed = -5.0f;

            Damage = 4;
            CheckBlock = false;

            Winkel = static_cast<float>(random(360));
            OwnDraw = true;
            ExplodeOnImpact = false;

            ShotArt = SPIDERFIRE;
        } break;

        case ELEKTROSCHUSS: {
            Counter = 1000;
            DamagePlayer = true;
            Damage = 1000;
            ExplodeOnImpact = false;
            ySpeed = 25.0f;
            xSpeed = -3.0f;
            HasGlow = true;
        } break;

        case ELEKTROPAMPE: {
            Counter = 1500;
            AnimEnde = 4;
            AnimSpeed = 0.5f;
            DamagePlayer = true;

            Damage = 20;
            ExplodeOnImpact = false;
        } break;

        case PLAYERFIRE:  // Flamme des Spielers
        {
            Counter = 255;
            AnimEnde = 29;
            AnimSpeed = (random(3) + 10) / 40.0f;
            DamagePlayer = false;

            Damage = 20;
            CheckBlock = true;

            Winkel = WinkelUebergabe - 5 + random(10);
            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 35);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 35);
            xSpeed = cos_deg(w) * 35.0f;
            ySpeed = sin_deg(w) * 35.0f;

            Winkel = static_cast<float>(random(360));

            if (w == -90)
                xSpeed = 0.0f;

            ExplodeOnImpact = false;
            OwnDraw = true;
        } break;

        case KRABBLERLASER1:  // Laser des Deckenkrabblers
        {
            Damage = 8;
            DamagePlayer = true;
            ySpeed = 30.0f;
            ExplodeOnImpact = false;
        } break;

        case KRABBLERLASER2:  // Laser des linken WandKrabblers
        {
            Damage = 8;
            DamagePlayer = true;
            xSpeed = 30.0f;
            ExplodeOnImpact = false;
        } break;

        case KRABBLERLASER3:  // Laser des rechten WandKrabblers
        {
            Damage = 8;
            DamagePlayer = true;
            xSpeed = -30.0f;
            ExplodeOnImpact = false;
        } break;

        case GRENADE:  // Granate
        {
            Winkel = WinkelUebergabe;

            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                Winkel = static_cast<float>((static_cast<int>(Winkel) + 180) % 360);

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 60);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 60) - 6.0f;
            xSpeed = cos_deg(w) * 60.0f;
            ySpeed = sin_deg(w) * 60.0f - 6.0f;

            yPos -= 2;
            yAcc = 5.0f;
            BounceWalls = false;
            Damage = 300;
            DamagePlayer = false;
        } break;

        case ROCKET:  // Normale Rakete
        {
            xSpeed = -50.0f;
            Damage = 100;
            DamagePlayer = true;
        } break;

        case ROCKETSPIDER:  // Rakete der Riesenspinne
        {
            AnimPhase = 0;
            AnimCount = 5.0f;
            ySpeed = -60.0f;
            Damage = 40;
            DamagePlayer = true;
        } break;

        case ROCKETWERFER:  // Rakete des Raketenwerfers
        {
            AnimPhase = 0;

            if (pParent->xpos > x)
                AnimPhase = 1;

            if (pParent->ypos + 60 < y)
                ySpeed = -20.0f;
            else
                ySpeed = 20.0f;

            xSpeed = 0.0f;
            Damage = 40;
            DamagePlayer = true;

        } break;

        case ROBOROCKET:  // Rakete des RoboMans
        {
            AnimPhase = 0;
            xSpeed = -30.0f;

            if (pParent->xpos > x) {
                AnimPhase = 1;
                xSpeed = 30.0f;
            }

            Damage = 40;
            DamagePlayer = true;

            ShotArt = ROCKETWERFER;

        } break;

        case ARCSHOT:  // Schuss der im Bogen fliegt
        {
            ySpeed = -30.0f;
            yAcc = 4.0f;
            Damage = 10;
            DamagePlayer = true;
        } break;

        case EVILROUND1:  // Rundum Schuss des Evil Hurri
        {
            xSpeed = -50.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND2:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = -45.0f;
            ySpeed = -8.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND3:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = -40.0f;
            ySpeed = -16.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND4:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = -30.0f;
            ySpeed = -24.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND5:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = -20.0f;
            ySpeed = -32.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND6:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = -10.0f;
            ySpeed = -40.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND7:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = 10.0f;
            ySpeed = -40.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND8:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = 20.0f;
            ySpeed = -32.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUND9:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = 30.0f;
            ySpeed = -24.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUNDA:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = 40.0f;
            ySpeed = -16.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUNDB:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = 45.0f;
            yAcc = 8.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILROUNDC:  // Rundum Schuss des Evil Hurri
        {
            ShotArt = EVILROUND1;
            xSpeed = 50.0f;
            yAcc = 5.0f;
            Damage = 20;
            DamagePlayer = true;
        } break;

        case SNOWBOMB:  // SchneeBombe
        {
            xSpeed = -8.0f;
            ySpeed = 26.0f;
            Damage = 40;
            DamagePlayer = true;
            AnimEnde = 19;
            AnimSpeed = 0.7f;
        } break;

        case FETTESPINNESHOT:  // Schuss der fetten Spinne rechts
        {
            xSpeed = 30.0f;
            Damage = 20;
            DamagePlayer = true;
            AnimEnde = 3;
            AnimSpeed = 0.5f;
            HasGlow = true;
        } break;

        case FETTESPINNESHOT2:  // Schuss der fetten Spinne links
        {
            xSpeed = -30.0f;
            Damage = 20;
            DamagePlayer = true;
            AnimEnde = 3;
            AnimSpeed = 0.5f;
            HasGlow = true;
        } break;

        case EIERBOMBE:  // Eierbome, die der Eiermann legt, und die auf den Hurri zurollt
        {
            if (pParent->xpos + 40 < xPos + 20)
                xSpeed = -18.0f;
            else
                xSpeed = 18.0f;

            Damage = 40;
            DamagePlayer = true;
            AnimSpeed = 0.5f;

            // Linksrum oder rechtsrum rollen lassen
            if (xSpeed < 0.0f) {
                AnimPhase = 18;
                AnimEnde = 19;
            } else {
                AnimPhase = 0;
                AnimEnde = 19;
            }

        } break;

        case SNOWBOMBSMALL:  // SchneeBombe
        {
            xSpeed = (random(80) - 40) / 5.0f;
            ySpeed = -(random(40) + 80) / 5.0f;
            yAcc = 5.0f;
            Damage = 15;
            DamagePlayer = true;
            AnimEnde = 19;
            AnimSpeed = 0.7f;
        } break;

        case TORPEDO:  // Torpedo Schuss
        {
            if (pParent->xpos > x) {
                AnimPhase = 1;
                xSpeed = 20.0f;
            } else {
                AnimPhase = 0;
                xSpeed = -20.0f;
            }

            AnimCount = 2.0f;

            Damage = 20;
            DamagePlayer = true;
        } break;

        case EVILSHOT:  // Schuss des Evil Hurri rechts
        {
            xSpeed = 65.0f;
            Damage = 10;
            DamagePlayer = true;
        } break;

        case EVILSHOT2:  // Schuss des Evil Hurri links
        {
            xSpeed = -65.0f;
            Damage = 10;
            DamagePlayer = true;
        } break;

        case EVILBLITZ:  // Blitz des bösen Hurris hoch
        {
            AnimEnde = 1;
            AnimSpeed = 0.7f;
            Damage = 50;
            ySpeed = -20.0f;
            yAcc = -1.0f;
            DamagePlayer = true;
            ExplodeOnImpact = false;
        } break;

        case EVILBLITZ2:  // Blitz des bösen Hurris runter
        {
            Damage = 100;
            AnimSpeed = 0.5f;
            AnimEnde = 1;
            ySpeed = 30;
            DamagePlayer = true;
            ExplodeOnImpact = false;
        } break;

        case UFOLASER:  // Laser des Ufos
        {
            HasGlow = true;
            AnimEnde = 9;
            AnimSpeed = 0.25f;
            Damage = 20;
            ySpeed = 20.0f;
            yAcc = 10.0f;
            DamagePlayer = true;
            ExplodeOnImpact = false;
        } break;

        case STELZLASER:  // Laser des Stelzsacks links
        {
            AnimPhase = 1;
            xSpeed = -30.0f;
            Damage = 8;
            ySpeed = 15.0f + g_Fahrstuhl_Speed;
            DamagePlayer = true;
            ExplodeOnImpact = false;
        } break;

        case STELZLASER2:  // Laser des Stelzsacks rechts
        {
            ShotArt = STELZLASER;

            AnimPhase = 0;
            xSpeed = 30.0f;
            Damage = 8;
            ySpeed = 15.0f + g_Fahrstuhl_Speed;
            DamagePlayer = true;
        } break;

        case PFLANZESHOT:  // Schuss der Pflanze
        {
            float absx = pParent->xpos + 35 - (xPos + 20);
            float absy = pParent->ypos + 40 - (yPos + 20) - 160;

            xSpeed = absx / 10.0f;
            Damage = 10;
            ySpeed = absy / 10.0f;
            yAcc = 3.25f;
            DamagePlayer = true;
            AnimEnde = 3;
            AnimSpeed = 0.25f;
            HasGlow = true;
        } break;

        case BRATKLOPSSHOT:  // Suchschuss des Bratklopses
        {
            // DKS - Converted to float:

            float absx = pParent->xpos + 35 - (xPos + 30);  // Differenz der x
            float absy = pParent->ypos + 40 - (yPos + 30);  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 20;                                     // Geschwindigkeit ist 4 fach

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

            xSpeed = absx;
            ySpeed = absy;

            Damage = 40;
            DamagePlayer = true;
            OwnDraw = true;
        } break;

        case DRONEBULLET:  // Schuss der Drone
        {
            xSpeed = 40.0f;
            ySpeed = 9.0f;
            Damage = 6;
            DamagePlayer = true;
            AnimEnde = 3;
            AnimSpeed = 0.2f;
        } break;

        case DRONEBULLET2:  // Schuss der Drone
        {
            xSpeed = -40.0f;
            ySpeed = 9.0f;
            Damage = 6;
            DamagePlayer = true;
            AnimEnde = 3;
            AnimSpeed = 0.2f;
            ShotArt = DRONEBULLET;
        } break;

        case SCHLEIMSHOT:  // Schuss der Drone
        {
            // DKS - Converted to float:

            float absx = pParent->xpos + 35 - xPos;  // Differenz der x
            float absy = pParent->ypos + 40 - yPos;  // und y Strecke

            // DKS - Converted to float:
            float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
            speed = speed * 20;                                     // Geschwindigkeit ist 4 fach

            absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
            absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

            xSpeed = absx;
            ySpeed = absy;

            Damage = 8;
            DamagePlayer = true;
        } break;

        case LAFASSSHOT:  // Das runterfallende Fass
        {
            xSpeed = WinkelUebergabe;
            ;
            ySpeed = 0.0f;
            yAcc = 5.0f;

            AnimEnde = 14;
            AnimSpeed = 1.0f;

            Damage = 8000;
            DamagePlayer = false;
        } break;

        case FLUGLASER:  // Laser der FlugKanone
        {
            Damage = 15;
            DamagePlayer = true;
            Winkel = WinkelUebergabe;

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 40);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 40);
            xSpeed = cos_deg(w) * 40.0f;
            ySpeed = sin_deg(w) * 40.0f;
        } break;

        case EISZAPFENSHOT:  // Eiszapfengeschoss
        {
            Damage = 12;
            DamagePlayer = true;
            Winkel = WinkelUebergabe;

            int w = static_cast<int>(Winkel) - 90;
            w = w % 360;

            // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
            // xSpeed	  = float (cos (w * PI / 180.0f) * 40);
            // ySpeed	  = float (sin (w * PI / 180.0f) * 40);
            xSpeed = cos_deg(w) * 40.0f;
            ySpeed = sin_deg(w) * 40.0f;
        } break;

        case ARCSHOTLEFT:  // Schuss der im Bogen fliegt links
        {
            ShotArt = ARCSHOT;
            ySpeed = -30.0f;
            xSpeed = -8.0f * WinkelUebergabe;
            yAcc = 4.0f;
            Damage = 10;
            DamagePlayer = true;
        } break;

        case ARCSHOTRIGHT:  // Schuss der im Bogen fliegt rechts
        {
            ShotArt = ARCSHOT;
            ySpeed = -30.0f;
            xSpeed = 8.0f * WinkelUebergabe;
            yAcc = 4.0f;
            Damage = 10;
            DamagePlayer = true;
        } break;

        case SMARTBOMB:  // Smartbomb
        {
            AnimCount = 32.0f;
            DamagePlayer = false;
            CheckBlock = false;
            ExplodeOnImpact = false;
            Damage = static_cast<int>(255.0f - AnimCount) * 2;
            OwnDraw = true;
        } break;

        case BLUEBOMB: {
            AnimSpeed = 0.5f;
            DamagePlayer = true;
            Damage = 20;
            AnimEnde = 19;
            ySpeed = 10.0f;
            yAcc = 10.0f;
        } break;

        case SKELETORGRANATE: {
            AnimEnde = 9;
            AnimSpeed = 0.5f;
            Damage = 40;
            Counter = 1.0f;
            DamagePlayer = false;

            if (WinkelUebergabe > 180.0f)
                WinkelUebergabe = -(360.0f - WinkelUebergabe);

            xSpeed = WinkelUebergabe * 1.8f;
            ySpeed = -18.0f;

            yAcc = 6.2f;
        } break;

        default:
            break;
    }
}

// --------------------------------------------------------------------------------------
// Schüsse auf Kollision mit den Gegnern oder Spielern testen
// --------------------------------------------------------------------------------------

void ProjectileClass::CheckCollision() {
    GegnerClass *pEnemy = Gegner.pStart;  // Anfang der Gegnerliste

    while (pEnemy != nullptr)  // Noch nicht alle durch ?
    {
        if (pEnemy->Active &&                                                // Ist der Gegner überhaupt aktiv ?
            pEnemy->Destroyable &&                                           // und kann man ihn zerstören
            pEnemy->Energy > 0.0f && ShotArt != BOMBE &&                     // Und der Schuss keine Bombe ?
            !(pEnemy->GegnerArt == POWERBLOCK && ShotArt == SHIELDSPAWNER))  // und kein powerblock + shield
        {
            // Beim BlitzBeam die größe vom Rect dynamisch anpassen
            if (ShotArt == BLITZBEAM) {
                // DKS - Fixed ending commas on next two lines, changed to semicolons. No idea why they were commas.
                ShotRect[BLITZBEAM].left = Damage / 4;
                ShotRect[BLITZBEAM].top = Damage / 4;
                ShotRect[BLITZBEAM].right = ShotRect[BLITZBEAM].left + (Damage / 2);
                ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top + (Damage / 2);
            }

            // Überschneiden sich die Rechtecke ?
            if (SpriteCollision(xPos, yPos, ShotRect[ShotArt], pEnemy->xPos, pEnemy->yPos,
                                GegnerRect[pEnemy->GegnerArt]) &&
                (!DamagePlayer || (ShotArt == FEUERFALLE && pEnemy->GegnerArt != LAVAMANN) ||
                 pEnemy->GegnerArt == SCHLEUSEH || pEnemy->GegnerArt == SCHLEUSEV)) {
                // blinken lassen, wenn noch nicht blinkt
                if (pEnemy->DamageTaken <= 0.0f)
                    pEnemy->DamageTaken = 255.0f;

                float BossZiehtWenigerAb = 1.0f;

                if ((ShotArt == GRENADE || ShotArt == POWERLINE || ShotArt == SMARTBOMB) &&
                    pEnemy->GegnerArt >= STAHLFAUST && pEnemy->GegnerArt < PARTIKELSPAWN)
                    BossZiehtWenigerAb = 0.7f;

                // Im 2 Spieler Modus sind die Gegner fieser
                if (NUMPLAYERS == 2)
                    BossZiehtWenigerAb *= 0.75f;

                // Schuss explodiert beim Auftreffen?
                if (ExplodeOnImpact) {
                    // Dann Energy abziehen
                    pEnemy->Energy -= Damage * BossZiehtWenigerAb;

                    // Powerline? Dann alles mit Energy < 40 eliminieren :)
                    if (ShotArt == POWERLINE && pEnemy->Energy < 40)
                        pEnemy->Energy = 0;

                    // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                    if (!SoundManager.WaveIsPlaying(SOUND::HIT + pEnemy->HitSound))
                        SoundManager.PlayWave(50, 128, 21000 + random(1000), SOUND::HIT + pEnemy->HitSound);

                    Damage = 0;     // und verschwinden lassen
                    ExplodeShot();  // aber explodieren tut er auch =)
                } else {
                    // War es ein Laser oder ne Powerline oder ähnliches ?
                    pEnemy->Energy -= Timer.sync(BossZiehtWenigerAb * Damage);  // Dann fliegt er

                    // Hit-Sound
                    // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                    if (!SoundManager.WaveIsPlaying(SOUND::HIT + pEnemy->HitSound))
                        SoundManager.PlayWave(50, 128, 21000, SOUND::HIT + pEnemy->HitSound);
                }  // nach dem Energy abziehen weiter

                if (pEnemy->GegnerArt == SCHNEEKOPPE) {
                    if (ShotArt != SMARTBOMB)
                        Damage = 0;

                    if (pEnemy->LastAction == -1 && (ShotArt == SMARTBOMB || fabs(xSpeed) >= fabs(ySpeed))) {
                        pEnemy->xSpeed *= -0.75f;
                        pEnemy->LastAction = 0;

                        if (ySpeed > 0.0f)
                            pEnemy->ySpeed += 8.0f;
                        if (ySpeed < 0.0f)
                            pEnemy->ySpeed -= 8.0f;
                    }
                }

                if (ShotArt != SMARTBOMB && (pEnemy->GegnerArt == SKULL || pEnemy->GegnerArt == PUNISHER)) {
                    float mul;

                    if (ShotArt != DIAMONDSHOT)
                        mul = 0.05f;
                    else
                        mul = 0.2f;

                    pEnemy->xSpeed += xSpeed * mul;
                    pEnemy->ySpeed += ySpeed * mul;

                    if (ShotArt != PLAYERFIRE) {
                        Damage = 0;
                        ExplodeShot();
                    }
                }

                // evtl pAim wechseln
                if (pEnemy->TimeToChange <= 0.0f) {
                    pEnemy->TimeToChange = 30.0f;

                    if (random(2) == 0 && pParent != nullptr)
                        pEnemy->pAim = pParent;
                }
            }
        }
        pEnemy = pEnemy->pNext;  // Nächsten Gegner testen
    }
}

// --------------------------------------------------------------------------------------
// Projektil rendern
// --------------------------------------------------------------------------------------

void ProjectileClass::Render() {

    // Schuss rendern
    //
    if (Damage <= 0)
        return;

    if (!OwnDraw) {

        float const xts = ProjectileGrafix[ShotArt].itsXTexScale;
        float const yts = ProjectileGrafix[ShotArt].itsYTexScale;

        // DKS - There is no need to compute this, it's already in the sprite's itsPreCalcedRects array:
        // int xfs, yfs, xfc
        // xfs = ProjectileGrafix[ShotArt].itsXFrameSize;
        // yfs = ProjectileGrafix[ShotArt].itsYFrameSize;
        // xfc = ProjectileGrafix[ShotArt].itsXFrameCount;
        // RECT_struct Rect;
        //// Ausschnitt berechnen
        // Rect.top	= (AnimPhase/xfc) * yfs;
        // Rect.left	= (AnimPhase%xfc) * xfs;
        // Rect.right  = Rect.left + xfs;
        // Rect.bottom = Rect.top  + yfs;

        RECT_struct &Rect = ProjectileGrafix[ShotArt].itsPreCalcedRects[AnimPhase];

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

        D3DCOLOR Color = 0xFFFFFFFF;

        QUAD2D TriangleStrip;  // DKS - Added local declaration

        // DKS - Altered this code to assign to TriangleStrip directly:
        TriangleStrip.v1.color = TriangleStrip.v2.color = TriangleStrip.v3.color = TriangleStrip.v4.color = Color;

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

        // Schuss gedreht rendern?
        //
        if (Winkel > -10000.0f) {
            Winkel = static_cast<float>(static_cast<int>(Winkel) % 360);

            // Rotationsmatrix
            glm::mat4x4 matRot = glm::rotate(glm::mat4x4(1.0f), DegreetoRad[static_cast<int>(Winkel)], glm::vec3(0.0f, 0.0f, 1.0f));

            float x = -TileEngine.XOffset + xPos;
            float y = -TileEngine.YOffset + yPos;
            float mx = ProjectileGrafix[ShotArt].itsXFrameSize / 2.0f;
            float my = ProjectileGrafix[ShotArt].itsYFrameSize / 2.0f;

            glm::mat4x4 matTrans, matTrans2;

            // Transformation zum Ursprung
            D3DXMatrixTranslation(&matTrans, -x - mx, -y - my, 0.0f);

            // Transformation wieder zurück
            D3DXMatrixTranslation(&matTrans2, x + mx, y + my, 0.0f);

            matWorld = matTrans * matWorld;   // Verschieben
            matWorld = matRot * matWorld;     // rotieren
            matWorld = matTrans2 * matWorld;  // und wieder zurück verschieben
            g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
            load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

            DirectGraphics.SetFilterMode(true);
        }

        if (ShotArt != CurrentShotTexture) {
            DirectGraphics.SetTexture(ProjectileGrafix[ShotArt].itsTexIdx);
            CurrentShotTexture = ShotArt;
        }

        // Sprite zeichnen
        // DKS - Altered to match new QUAD2D strips:
        DirectGraphics.RendertoBuffer(GL_TRIANGLE_STRIP, 2, &TriangleStrip);

        if (Winkel > -10000.0f) {
            // Normale Projektions-Matrix wieder herstellen
            matWorld = glm::mat4x4(1.0f);
            g_matModelView = matWorld * g_matView;
#if defined(USE_GL1)
            load_matrix(GL_MODELVIEW, glm::value_ptr(g_matModelView));
#endif

            DirectGraphics.SetFilterMode(false);
        }

#ifndef NDEBUG
        if (DebugMode == true)
            RenderRect(static_cast<float>(xPos - TileEngine.XOffset + ShotRect[ShotArt].left),
                       static_cast<float>(yPos - TileEngine.YOffset + ShotRect[ShotArt].top),
                       static_cast<float>(ShotRect[ShotArt].right - ShotRect[ShotArt].left),
                       static_cast<float>(ShotRect[ShotArt].bottom - ShotRect[ShotArt].top), 0x88FFFFFF);
#endif  //NDEBUG

    }

    // Elektropampe rendern
    else if (ShotArt == BOUNCESHOT1) {
        int size = (pParent->CurrentWeaponLevel[2] * 2 + 12);
        if (size < 20)
            size = 20;
        ProjectileGrafix[ShotArt].RenderSpriteScaled(xPos - TileEngine.XOffset + 12.0f - size / 2.0f,
                                                     yPos - TileEngine.YOffset + 12.0f - size / 2.0f, size, size,
                                                     0xFFFFFFFF);
    } else if (ShotArt == ELEKTROPAMPE) {
        DirectGraphics.SetAdditiveMode();

        D3DCOLOR Color;
        if (Counter > 255.0f)
            Color = D3DCOLOR_RGBA(255, 255, 255, 255);
        else
            Color = D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(Counter));

        ProjectileGrafix[ShotArt].itsRect = ProjectileGrafix[ShotArt].itsPreCalcedRects[AnimPhase];
        ProjectileGrafix[ShotArt].RenderSprite(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset, Color);

        DirectGraphics.SetColorKeyMode();
    } else if (ShotArt == FEUERFALLE || ShotArt == SPIDERFIRE || ShotArt == PLAYERFIRE) {
        DirectGraphics.SetAdditiveMode();

        D3DCOLOR Color;
        if (ShotArt == PLAYERFIRE)
            Color = D3DCOLOR_RGBA(255, 128, 80, static_cast<int>(Counter));
        else
            Color = D3DCOLOR_RGBA(255, 192, 128, 192 - (AnimPhase * 6));

        ProjectileGrafix[ShotArt].itsRect = ProjectileGrafix[ShotArt].itsPreCalcedRects[AnimPhase];

        ProjectileGrafix[ShotArt].RenderSpriteScaledRotated(
            46.0f + (xPos - TileEngine.XOffset) - static_cast<float>(AnimPhase + 30),
            54.0f + (yPos - TileEngine.YOffset) - static_cast<float>(AnimPhase + 30),
            static_cast<int>(AnimPhase * 2.2f + 20.0f),
            static_cast<int>(AnimPhase * 2.2f + 20.0f), Winkel, Color);
        DirectGraphics.SetColorKeyMode();
    }

    // Feuer des Lavamanns
    //
    else if (ShotArt == FEUERFALLE_LAVAMANN) {
        DirectGraphics.SetAdditiveMode();
        D3DCOLOR Color = D3DCOLOR_RGBA(255, 255, 255, 192 - (AnimPhase * 6));

        int a = 30 - AnimPhase;

        ProjectileGrafix[ShotArt].RenderSpriteScaled(46.0f + (xPos - TileEngine.XOffset) - static_cast<float>(a + 30),
                                                     54.0f + (yPos - TileEngine.YOffset) - static_cast<float>(a + 30),
                                                     static_cast<int>(a * 2.2f + 20.0f),
                                                     static_cast<int>(a * 2.2f + 20.0f), AnimPhase, Color);
        DirectGraphics.SetColorKeyMode();
    }

    // Druckwellen Smartbomb anzeigen
    else if (ShotArt == SMARTBOMB) {
        DirectGraphics.SetAdditiveMode();
        ShotRect[ShotArt].bottom = (255 - Damage / 2) * 2;
        ShotRect[ShotArt].right = (255 - Damage / 2) * 2;

        D3DCOLOR Color = D3DCOLOR_RGBA(255, 255, 255, Damage / 2);
        ProjectileGrafix[ShotArt].RenderSpriteScaled(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset,
                                                     (255 - Damage / 2) * 2, (255 - Damage / 2) * 2, Color);

        Color = D3DCOLOR_RGBA(255, 255, 255, Damage / 4);
        ProjectileGrafix[ShotArt].RenderSpriteScaled(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset,
                                                     (255 - Damage / 2) * 2, (255 - Damage / 2) * 2, Color);

        DirectGraphics.SetColorKeyMode();
    } else if (ShotArt == BLITZBEAM) {
        DirectGraphics.SetAdditiveMode();
        D3DCOLOR Color = 0xFFFFFFFF;

        ProjectileGrafix[ShotArt].RenderSpriteScaled(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset, Damage,
                                                     Damage, AnimPhase, Color);

        DirectGraphics.SetColorKeyMode();
    }

    // Fetter Spinnenlaser
    else if (ShotArt == SPIDERLASER) {
        static float off = 0.0f;

        DirectGraphics.SetAdditiveMode();
        ShotRect[ShotArt].bottom = 0;
        ShotRect[ShotArt].right = 0;

        D3DCOLOR col;
        float size;
        int length;

        // Größe festlegen
        if (AnimCount < 45.0f) {
            length = 480;
            size = 10.0f;
            col = D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(AnimCount / 45.0f * 255.0f));

            ShotRect[SPIDERLASER].left = 0;
            ShotRect[SPIDERLASER].right = 0;
            ShotRect[SPIDERLASER].top = 0;
            ShotRect[SPIDERLASER].bottom = 0;
        } else {
            length = 560;
            size = (7.0f) * 30.0f + sin(off) * 10.0f;
            off += Timer.sync(5.0f);
            col = 0xFFFFFFFF;

            ShotRect[SPIDERLASER].left = 0;
            ShotRect[SPIDERLASER].right = 500;
            ShotRect[SPIDERLASER].top = -100;
            ShotRect[SPIDERLASER].bottom = 100;
            Damage = 500;
        }

        ProjectileGrafix[ShotArt].RenderSpriteScaled(
            xPos - TileEngine.XOffset - 10.0f,
            yPos - TileEngine.YOffset - size / 2.0f,
            length, static_cast<int>(size), 0, col);
        DirectGraphics.SetColorKeyMode();
    } else if (ShotArt == BRATKLOPSSHOT) {
        DirectGraphics.SetAdditiveMode();

        ProjectileGrafix[ShotArt].RenderSpriteRotated(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset,
                                                      (xPos + yPos) / 2.0f, 0x88FFFFFF);

        ProjectileGrafix[ShotArt].RenderSpriteRotated(xPos - TileEngine.XOffset, yPos - TileEngine.YOffset,
                                                      (xPos + yPos) / 2.0f, 0x88FFFFFF);

        DirectGraphics.SetColorKeyMode();
    }

    CurrentShotTexture = ShotArt;

    // Leuchten bei bestimmten Schussarten anzeigen ?
    if (HasGlow && options_Detail >= DETAIL_HIGH) {
        DirectGraphics.SetAdditiveMode();
        CurrentShotTexture = -1;  // Erst mal einstellen dass wir jetzt die falsche Textur gesetzt haben =)

        if (ShotArt == UFOLASER) {
            PartikelGrafix[UFOLASERFLARE].RenderSpriteRotated(xPos - TileEngine.XOffset - 63.0f,
                                                              yPos - TileEngine.YOffset - 30.0f, yPos * 0.5f, 0xFFFF22EE);
        } else if (ShotArt == ELEKTROSCHUSS) {
            Projectiles.LavaFlare.RenderSpriteScaled(xPos - 28.0f - TileEngine.XOffset, yPos - 28.0f - TileEngine.YOffset,
                                                     100, 100, 0xFFFF22BB);
        } else if (ShotArt == LASERSHOT || ShotArt == LASERSHOT2) {
            switch (static_cast<int>(Winkel)) {
                case 0:
                    Projectiles.LaserSmoke.RenderSpriteRotated(
                        xPos - TileEngine.XOffset - 60.0f + (ShotArt - LASERSHOT) * 5, yPos - TileEngine.YOffset - 40.0f,
                        Winkel, 0xBB1188FF);
                    break;

                case 45:
                    Projectiles.LaserSmoke.RenderSpriteRotated(xPos - TileEngine.XOffset - 60.0f,
                                                               yPos - TileEngine.YOffset - 40.0f, Winkel, 0xBB1188FF);
                    break;

                case 90:
                    Projectiles.LaserSmoke.RenderSpriteRotated(xPos - TileEngine.XOffset - 53.0f,
                                                               yPos - TileEngine.YOffset - 40.0f, Winkel, 0xBB1188FF);
                    break;

                case 270:
                    Projectiles.LaserSmoke.RenderSpriteRotated(xPos - TileEngine.XOffset - 52.0f,
                                                               yPos - TileEngine.YOffset - 40.0f, Winkel, 0xBB1188FF);
                    break;

                case 315:
                    Projectiles.LaserSmoke.RenderSpriteRotated(xPos - TileEngine.XOffset - 60.0f,
                                                               yPos - TileEngine.YOffset - 40.0f, Winkel, 0xBB1188FF);
                    break;

                default:
                    break;
            }
        }

        else if (ShotArt == LASERSHOTBIG || ShotArt == LASERSHOTBIG2) {
            switch (static_cast<int>(Winkel)) {
                case 0:
                    Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos - TileEngine.XOffset - 120.0f,
                                                                  yPos - TileEngine.YOffset - 80.0f, Winkel, 0xBB1188FF);
                    break;

                case 45:
                    Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos - TileEngine.XOffset - 120.0f,
                                                                  yPos - TileEngine.YOffset - 85.0f, Winkel, 0xBB1188FF);
                    break;

                case 90:
                    Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos - TileEngine.XOffset - 120.0f,
                                                                  yPos - TileEngine.YOffset - 85.0f, Winkel, 0xBB1188FF);
                    break;

                case 270:
                    Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos - TileEngine.XOffset - 105.0f,
                                                                  yPos - TileEngine.YOffset - 85.0f, Winkel, 0xBB1188FF);
                    break;

                case 315:
                    Projectiles.LaserSmokeBig.RenderSpriteRotated(xPos - TileEngine.XOffset - 120,
                                                                  yPos - TileEngine.YOffset - 85, Winkel, 0xBB1188FF);
                    break;

                default:
                    break;
            }
        }

        else if (ShotArt == SPREADSHOTBIG || ShotArt == SPREADSHOTBIG2) {
            Projectiles.SpreadShotSmoke.RenderSprite(xPos - 16.0f - TileEngine.XOffset, yPos - 18.0f - TileEngine.YOffset, 0,
                                                     0xAAFFFFFF);
        }

        else if (ShotArt == PFLANZESHOT) {
            Projectiles.SpreadShotSmoke.RenderSprite(xPos - 24.0f - TileEngine.XOffset, yPos - 24.0f - TileEngine.YOffset, 0,
                                                     0xAAFFFFFF);
        }

        else if (ShotArt == SUCHSCHUSS2) {
            Projectiles.LavaFlare.RenderSpriteScaled(xPos - 24.0f - TileEngine.XOffset, yPos - 24.0f - TileEngine.YOffset, 64,
                                                     64, 0xCC0088FF);
        }

        else if (ShotArt == WALKER_LASER)
            Projectiles.LavaFlare.RenderSprite(xPos - 45.0f - TileEngine.XOffset, yPos - 60.0f - TileEngine.YOffset, 0,
                                               0xAAFF3300);

        else if (ShotArt == FETTESPINNESHOT || ShotArt == FETTESPINNESHOT2)  // Laser der fetten Spinne
        {
            Projectiles.SpreadShotSmoke.RenderSprite(xPos - 12.0f - TileEngine.XOffset, yPos - 7.0f - TileEngine.YOffset, 0,
                                                     0xFFFF0000);
        }

        if (ShotArt == POWERLINE)  // Powerline leuchten lassen ?
        {
            if (xSpeed < 0.0f)
                Projectiles.PowerlineSmoke.RenderSprite(xPos - 10.0f - TileEngine.XOffset, yPos - TileEngine.YOffset, 0,
                                                        0xFFFFFFFF);
            else
                Projectiles.PowerlineSmoke.RenderSprite(xPos - 28.0f - TileEngine.XOffset, yPos - TileEngine.YOffset, 1,
                                                        0xFFFFFFFF);
        }

        DirectGraphics.SetColorKeyMode();
    }  // Leuchten bei bestimmten Schussarten anzeigen

    // Animieren
    if (ShotArt != EIERBOMBE && AnimEnde > 0)  // Soll überhaupt animiert werden ?
    {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = AnimCount - AnimSpeed;  // Dann wieder auf Null setzen
            AnimPhase++;                        // Und nächste Animationsphase

            if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
                AnimPhase = 0;         // Dann wieder von vorne beginnen
        }
    }
}

// --------------------------------------------------------------------------------------
// Projektil animieren und bewegen
// --------------------------------------------------------------------------------------

void ProjectileClass::Run() {
    bo = bu = bl = br = 0;

    if (AnimEnde > 0)            // Soll überhaupt anmiert werden ?
        AnimCount += Timer.sync(1.0f);  // Animationscounter weiterzählen

    // Bewegen
    xSpeed += Timer.sync(xAcc);
    ySpeed += Timer.sync(yAcc);
    xPos += Timer.sync(xSpeed);
    yPos += Timer.sync(ySpeed);

    // Screen verlassen ?
    if (ShotArt != FLAMME && ShotArt != ROCKETSPIDER && ShotArt != EVILBLITZ && ShotArt != EVILBLITZ2 &&
        ShotArt != SNOWBOMB && ShotArt != EIERBOMBE &&
        (yPos - TileEngine.YOffset > 480 + 100 || yPos - TileEngine.YOffset + ShotRect[ShotArt].bottom < 0 - 100 ||
         xPos - TileEngine.XOffset > 640 + 100 || xPos - TileEngine.XOffset + ShotRect[ShotArt].right < 0 - 100))
        Damage = 0;

    // Level verlassen ?
    if ((ShotArt == FLAMME || ShotArt == SNOWBOMB || ShotArt == EIERBOMBE || ShotArt == EVILBLITZ ||
         ShotArt == EVILBLITZ2) &&
        (yPos > TileEngine.LEVELPIXELSIZE_Y || xPos > TileEngine.LEVELPIXELSIZE_X || yPos < 0 || xPos < 0))
        Damage = 0;

    if (Damage == 0)
        return;

    // Testen ob ein zerstörbares Leveltile von einem Schuss des Spielers
    // getroffen wurde und wenn ja, dann dieses und den Schuss zerstören
    //
    if (!DamagePlayer && ShotArt != BOMBE && ShotArt != BOMBEBIG && ShotArt != SHIELDSPAWNER)
        if (TileEngine.BlockDestroyRechts(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) ||
            TileEngine.BlockDestroyLinks(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) ||
            TileEngine.BlockDestroyOben(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) ||
            TileEngine.BlockDestroyUnten(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt])) {
            // Alle Schüsse ausser dem Beam (und dem Schutzschild, welches auch ein Schuss ist) exlodieren
            // an der zerschiessbaren Wand
            //
            if (ShotArt != BLITZBEAM && ShotArt != SHIELDSPAWNER) {
                ExplodeShot();
                Damage = 0;
            } else
                TileEngine.CheckDestroyableWalls(xPos, yPos, xSpeed, ySpeed, ShotRect[ShotArt]);
        }

    if (CheckBlock) {
        bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        bu = TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        bl = TileEngine.BlockLinks(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
        br = TileEngine.BlockRechts(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
    }

    // Auf Kollision mit dem Level Testen (nur bei Bounce-fähigen Schüssen)
    if (BounceWalls) {
        int DoDamage = 0;

        if ((ySpeed < 0.0f && bo & BLOCKWERT_WAND) || (ySpeed > 0.0f && bu & BLOCKWERT_WAND)) {
            ySpeed *= -1.0f;
            DoDamage = 1;
        }

        if ((xSpeed > 0.0f && br & BLOCKWERT_WAND) || (xSpeed < 0.0f && bl & BLOCKWERT_WAND)) {
            xSpeed *= -1.0f;
            DoDamage = 1;
        }

        if (ShotArt == BOUNCESHOT2 || ShotArt == BOUNCESHOT3 || ShotArt == BOUNCESHOTBIG2 ||
            ShotArt == BOUNCESHOTBIG3) {
            Damage -= DoDamage;

            if (Damage <= 0) {
                SoundManager.StopWave(SOUND::BOUNCESHOT);
                SoundManager.PlayWave(100, 128, 11025, SOUND::BOUNCESHOT);

                ExplodeShot();
            }
        }
    }

    // Verschiedene Schüsse unterschiedlich behandeln
    switch (ShotArt) {
        case SKELETOR_SHOT:
        case SPREADSHOT:   // Der Streuschuss (explodiert an der Wand)
        case SPREADSHOT2:  // Der Streuschuss (explodiert an der Wand)
        case SPREADSHOTBIG:
        case SPREADSHOTBIG2: {
            if ((TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND &&
                 ySpeed > 0.0f) ||
                bo & BLOCKWERT_WAND || bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND) {
                Damage = 0;

                SoundManager.StopWave(SOUND::SPREADHIT);
                SoundManager.PlayWave(50, 128, random(2000) + 11025, SOUND::SPREADHIT);
                ExplodeShot();
            }
        } break;

        case LASERSHOT:   // Der Laser (explodiert nur an der Wand, nicht am Gegner)
        case LASERSHOT2:  // Der Laser (explodiert nur an der Wand, nicht am Gegner)
        case LASERSHOTBIG:
        case LASERSHOTBIG2: {
            if (AnimPhase > AnimEnde)
                AnimPhase = 0;

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case BOUNCESHOT1:  // Der Bounce explodiert an der Wand
        case BOUNCESHOTBIG1: {
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bu & BLOCKWERT_WAND || bo & BLOCKWERT_WAND) {
                if (BounceWalls) {
                    Damage -= 3;

                    if (xSpeed == 0.0f || ySpeed == 0.0f)
                        ySpeed = xSpeed;

                    if (Damage <= 0)
                        ExplodeShot();
                } else {
                    ExplodeShot();
                    Damage = 0;
                }
            }
        } break;

        case BOUNCESHOT2:  // Der mittlere Bounce explodiert nur, wenn er sich nochmal aufteilen würde
        case BOUNCESHOTBIG2: {
            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 3 &&
                (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bu & BLOCKWERT_WAND || bo & BLOCKWERT_WAND)) {
                if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 5) {
                    ExplodeShot();
                    Damage = 0;
                }
            }

        } break;

        case POWERLINE:  // Die Powerline (explodiert an der Wand)
        {
            if ((xSpeed < 0.0f && bl & BLOCKWERT_WAND) || (xSpeed > 0.0f && br & BLOCKWERT_WAND)) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case FLAMME:  // Flammen-Splitter explodiert auch an der Wand
        {
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_PLATTFORM) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case CANONBALL:     // Kannonenkugel des Geschützturms
        case SUCHSCHUSS:    // Kannonenkugel des Deckenturms
        case SUCHSCHUSS2:   // Blauer Schuss der Riesenwespe
        case WALKER_LASER:  // Walkerlaser
        case SPIDER_LASER:  // Spinnenlaser
        case SPIDERSLOW: {
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case FIREBALL: {
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount < 0.0f) {
                AnimCount = 0.1f;
                PartikelSystem.PushPartikel(xPos, yPos + 10 + random(5), FIREBALL_SMOKE);
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bu & BLOCKWERT_WAND || bo & BLOCKWERT_WAND) {
                ExplodeShot();
                Damage = 0;
            }
        } break;

        case FIREBALL_BIG: {
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount < 0.0f) {
                AnimCount = 0.1f;

                for (int i = 0; i < 4; i++)
                    PartikelSystem.PushPartikel(xPos + 10 + random(20), yPos + 10 + random(20), FIREBALL_SMOKE);
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bu & BLOCKWERT_WAND || bo & BLOCKWERT_WAND) {
                ExplodeShot();
                Damage = 0;
            }
        } break;

        case FIREBALL_BOMB: {
            Winkel += Timer.sync(100.0f);
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount < 0.0f) {
                AnimCount = 1.0f;

                PartikelSystem.PushPartikel(xPos, yPos, SMOKEBIG);

                for (int i = 0; i < 5; i++)
                    PartikelSystem.PushPartikel(xPos + random(50), yPos + random(50), FUNKE);
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bu & BLOCKWERT_WAND || bo & BLOCKWERT_WAND) {
                ExplodeShot();
                Damage = 0;
            }
        } break;

        case SPIDERLASER: {
            AnimCount += Timer.sync(2.5f);

            // Partikel fliegen
            if (AnimCount > 45.0f)
                AnimSpeed -= Timer.sync(1.0f);

            if (AnimSpeed < 0.0f) {
                AnimSpeed = 0.25f;
                PartikelSystem.PushPartikel(xPos + 460, yPos + (random(170) - 90), LILA3);

                if (AnimPhase > 0) {
                    AnimPhase = 0;
                    SoundManager.PlayWave(100, 128, 12000, SOUND::SPIDERLASER);
                    SoundManager.PlayWave(100, 128, 8000, SOUND::LILA);
                    SoundManager.StopWave(SOUND::BEAMLOAD2);
                }
            }

            if (AnimCount > 140.0f) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case SPIDERSHOT: {
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }

        } break;

        case SPIDERSHOT2: {
            AnimCount -= Timer.sync(1.0f);
            if (AnimCount <= 0.0f) {
                AnimCount = 0.3f;
                PartikelSystem.PushPartikel(xPos + 12, yPos + 12, LILA);
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND ||
                TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case TURRIEXTRAWURST:
        case GOLEMSCHUSS:  // Suchschuss des Golems
        {
            DirectGraphics.SetAdditiveMode();
            Projectiles.LavaFlare.RenderSpriteScaled(xPos - TileEngine.XOffset - 21, yPos - TileEngine.YOffset - 21, 62,
                                                     62, 0xAA88FF00);
            DirectGraphics.SetColorKeyMode();
            CurrentShotTexture = -1;

            if (ShotArt == GOLEMSCHUSS) {
                if ((yAcc > 0.0f && ySpeed > Counter) || (yAcc < 0.0f && ySpeed < -Counter))
                    yAcc *= -1.0f;
            } else {
                if (Counter > 0.0f) {
                    Counter -= Timer.sync(1.0f);

                    if (ySpeed >= 0.0f)
                        ySpeed = 0.0f;

                } else {
                    if (yAcc != 0.0f) {
                        PlayerClass *pAim;
                        yAcc = 0.0f;

                        if (pParent == nullptr)
                            pAim = ChooseAim();
                        else
                            pAim = pParent;

                        // DKS - Converted to float:
                        float absx = pAim->xpos + 35 - xPos;  // Differenz der x
                        float absy = pAim->ypos + 40 - yPos;  // und y Strecke

                        // DKS - Converted to float:
                        float speed = 1.0f / sqrtf(absx * absx + absy * absy);  // Länge der Strecke berechnen
                        speed = speed * 35;                                     // Geschwindigkeit ist 4 fach

                        absx = speed * absx;  // Und jeweilige Geschwindigkeit setzen
                        absy = speed * absy;  // (xSpeed*ySpeed ergibt 4)

                        xSpeed = absx;
                        ySpeed = absy;
                    }
                }
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case ROTZSHOT: {
            Counter -= Timer.sync(1.0f);

            if (Counter <= 0.0f) {
                Counter = 0.1f;
                PartikelSystem.PushPartikel(xPos + 4.0f, yPos, PHARAOSMOKE);
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND)  // ||
            // bu & BLOCKWERT_PLATTFORM)
            {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case BOMBE:
        case BOMBEBIG: {
            bu = TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);

            bool fahrstuhlunten = false;
            if (TileEngine.IsElevatorLevel && yPos + ShotRect[ShotArt].bottom + 4 > g_Fahrstuhl_yPos)
                fahrstuhlunten = true;

            // Kein Boden? Dann runterfallen
            //
            if (!(bu & BLOCKWERT_WAND) && !(bu & BLOCKWERT_PLATTFORM) && !fahrstuhlunten) {
                yAcc = 8.0f;
            }

            // Geschwindigkeit begrenzen
            //
            if (ySpeed > 40.0f)
                ySpeed = 40.0f;

            // an der Wand abprallen
            //
            if ((xSpeed < 0.0f && bl & BLOCKWERT_WAND) || (xSpeed > 0.0f && br & BLOCKWERT_WAND))
                xSpeed *= -1.0f;

            // fällt runter?
            if (ySpeed > 0.0f) {
                // Am Boden abprallen
                //
                if (bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM || fahrstuhlunten) {
                    ySpeed *= -0.75f;

                    // an der Schräge seitlich abprallen
                    //
                    if (bl & BLOCKWERT_SCHRAEGE_L)
                        xSpeed = 10.0f;
                    if (br & BLOCKWERT_SCHRAEGE_R)
                        xSpeed = -10.0f;
                }
            }

            if (AnimPhase == AnimEnde) {
                Damage = 0;
                ExplodeShot();
            }

        } break;

        case FEUERFALLE:
        case FEUERFALLE_LAVAMANN: {
            Winkel += Timer.sync(10.0f);
            if (AnimPhase >= 29) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case GOLEMSAEULE: {
            // Counter runterzählen
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount <= 0.0f) {
                AnimCount = 13.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND::FEUERFALLE);

                for (int i = 0; i < 25; i++)
                    Projectiles.PushProjectile(xPos, yPos + i * 5.0f, FEUERFALLE);
            }
        } break;

        case SPIDERFIRE: {
            Winkel += Timer.sync(10.0f);
            if (AnimPhase >= 29) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case ELEKTROSCHUSS: {
            Counter -= Timer.sync(100.0f);

            if (Counter < 750.0f) {
                xAcc = -6.0f;
                yAcc = -5.0f;

                if (xSpeed < -25.0f)
                    xSpeed = -25.0f;

                if (ySpeed < 4.0f)
                    ySpeed = 4.0f;
            }

            if (Counter < 350.0f) {
                yAcc = 7.0f;
                xAcc = 1.0f;
            }

            if (Counter <= -150.0f) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case ELEKTROPAMPE: {
            if (AnimPhase > AnimEnde)
                AnimPhase = 0;

            Counter -= Timer.sync(70.0f);

            if (Counter <= 0.0f)
                Damage = 0;

            OwnDraw = true;
        } break;

        case PLAYERFIRE: {
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND) {
                xSpeed = ySpeed = xAcc = yAcc = 0.0f;
            }

            if (AnimPhase > 10)
                Counter -= Timer.sync(50.0f);

            Winkel += Timer.sync(10.0f);
            if (AnimPhase >= 29 || Counter <= 0.0f) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case KRABBLERLASER1: {
            if (bu & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case KRABBLERLASER2: {
            if (br & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case KRABBLERLASER3: {
            if (bl & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case PHARAOLASER: {
            ySpeed = std::clamp(ySpeed, -20.0f, 20.0f);

            // Spieler verfolgen
            if (pParent->ypos + 40 < yPos + 24)
                yAcc = -5.0f;
            else if (pParent->ypos + 40 > yPos + 24)
                yAcc = 5.0f;

            for (int i = 0; i < static_cast<int>(200.0 * Timer.getElapsed()) + 1; i++)
                PartikelSystem.PushPartikel(xPos + 18, yPos + 18, PHARAOSMOKE);

            if (bu & BLOCKWERT_WAND && ySpeed > 0.0f)
                ySpeed *= -1;

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case ROCKETSPIDER: {
            // Flamme erzeugen
            for (int i = 0; i < static_cast<int>(200.0 * Timer.getElapsed()) + 1; i++) {
                switch (AnimPhase) {
                    case 0: {
                        PartikelSystem.PushPartikel(xPos + 33.0f, yPos + 70.0f, ROCKETSMOKE);
                    } break;

                    case 4: {
                        PartikelSystem.PushPartikel(xPos + 33.0f, yPos - 2.0f, ROCKETSMOKE);
                    } break;
                }
            }

            // Counter runzerzählen wann die Rakete von oben auf den Spieler runtersaust
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount <= 0.0f && AnimPhase == 0) {
                AnimPhase = 4;
                ySpeed = -ySpeed;
                xPos = pParent->xpos;
            }

            if (bu & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }

        } break;

        case ROCKETWERFER: {
            // Rakete fliegt noch runter bzw hoch
            if (xSpeed == 0.0f) {
                // Auf richtiger Höhe mit dem Spieler ? Dann seitlich losfliegen
                if (yPos > pParent->ypos + pParent->CollideRect.top &&
                    yPos < pParent->ypos + pParent->CollideRect.bottom) {
                    if (ySpeed < 0)
                        yAcc = 5.0f;
                    else
                        yAcc = -5.0f;

                    if (AnimPhase == 0) {
                        xSpeed = -10.0f;
                        xAcc = -5.0f;
                    } else {
                        xSpeed = 10.0f;
                        xAcc = 5.0f;
                    }

                    SoundManager.PlayWave(100, 128, 11025, SOUND::ROCKET);
                }
            }

            else

            {
                // Flamme erzeugen
                for (int i = 0; i < static_cast<int>(200.0 * Timer.getElapsed()) + 1; i++) {
                    switch (AnimPhase) {
                        case 0: {
                            PartikelSystem.PushPartikel(xPos + 40.0f, yPos, ROCKETSMOKE);
                        } break;

                        case 1: {
                            PartikelSystem.PushPartikel(xPos, yPos, ROCKETSMOKE);
                        } break;
                    }
                }

                // gerade fliegen
                if (yAcc < 0)
                    if (ySpeed < 0)
                        ySpeed = yAcc = 0.0f;
                if (yAcc > 0)
                    if (ySpeed > 0)
                        ySpeed = yAcc = 0.0f;
            }

            // Wand getroffen
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }

        } break;

        case TORPEDO: {
            // Spieler verfolgen
            if (yPos + 5 < pParent->ypos + 45)
                yAcc = 2.0f;
            if (yPos + 5 > pParent->ypos + 45)
                yAcc = -2.0f;

            ySpeed = std::clamp(ySpeed, -10.0f, 10.0f);

            // Blubbern
            AnimCount -= Timer.sync(1.0f);
            if (AnimCount < 0) {
                AnimCount += 0.5f;

                switch (AnimPhase) {
                    case 0: {
                        PartikelSystem.PushPartikel(xPos + 30.0f, yPos, BUBBLE);
                    } break;

                    case 1: {
                        PartikelSystem.PushPartikel(xPos, yPos, BUBBLE);
                    } break;
                }
            }

            // Wand getroffen
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }

        } break;

        case EVILBLITZ: {
            for (int i = 0; i < static_cast<int>(200.0 * Timer.getElapsed()) + 1; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(50)),
                                            yPos + 60.0f + static_cast<float>(random(20)), EVILFUNKE);

            if (ySpeed < -45.0f) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case EVILBLITZ2: {
            if (bu & BLOCKWERT_WAND && ySpeed > 0.0f) {
                ySpeed = 0.0f;
                AnimEnde = 0;
                AnimPhase = 0;
                AnimCount = 255.0f;

                SoundManager.PlayWave(100, 128, 11025, SOUND::BLITZHIT);
            }

            if (ySpeed == 0.0f) {
                AnimCount -= Timer.sync(40.0f);
                if (AnimCount < 0.0f) {
                    Damage = 0;
                    ExplodeShot();
                }
            }
        } break;

        case BLITZBEAM: {
            if (Damage > 0)
                Counter -= Timer.sync(1.0f);

            if (Counter <= 0.0f) {
                Counter = 0.1f;

                PartikelSystem.PushPartikel(xPos + static_cast<float>(Damage / 2 + 1 + random(3)),
                                            yPos + static_cast<float>(Damage / 2 + 1 + random(3)),
                                            BEAMSMOKE);

                PartikelSystem.PushPartikel(xPos + static_cast<float>(Damage / 2 - 2 + random(3)),
                                            yPos + static_cast<float>(Damage / 2 - 6 + random(3)),
                                            BEAMSMOKE2);
            }

            // Wand getroffen?
            //
            // DKS - Fixed indentation / replaced commas with semicolons on first two lines here:
            ShotRect[BLITZBEAM].left = Damage / 4;
            ShotRect[BLITZBEAM].top = Damage / 4;
            ShotRect[BLITZBEAM].right = ShotRect[BLITZBEAM].left + (Damage / 2);
            ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top + (Damage / 2);

            bo = TileEngine.BlockOben(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
            bu = TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
            bl = TileEngine.BlockLinks(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
            br = TileEngine.BlockRechts(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND) {
                // if (Damage > 200.0f)
                //	ShakeScreen((float)Damage / 20.0f);

                ExplodeShot();
            }
        } break;

        case EISZAPFENSHOT:
        case UFOLASER:
        case STELZLASER:
        case PFLANZESHOT:
        case BRATKLOPSSHOT:
        case DRONEBULLET:
        case SCHLEIMSHOT:
        case LAFASSSHOT:
        case BLUEBOMB: {
            // Wand getroffen
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case SKELETORGRANATE: {
            // Drehwinkel aus der Geschwindigkeit errechnen
            // DKS - Converted to float, new Rad/Deg macros:
            // float w = 90 + static_cast<float>(atan(ySpeed / xSpeed) * 360.0f / (static_cast<float>(M_PI) * 2));
            Winkel = 90.0f + RadToDeg(atanf(ySpeed / xSpeed));

            if (xSpeed < 0.0f)
                Winkel = Winkel + 180.0f;

            // Boden getroffen? Dann stoppen
            if (bu & BLOCKWERT_WAND) {
                xSpeed = 0.0f;
                ySpeed = 0.0f;
                xAcc = 0.0f;
                yAcc = 0.0f;
                Winkel = 180.0f;
            }

            Counter -= Timer.sync(2.0f);

            if (Counter <= 0.0f) {
                if (!(bu & BLOCKWERT_WAND)) {
                    if (xSpeed > 0.0f)
                        PartikelSystem.PushPartikel(xPos - 5, yPos - 5, SMOKE);
                    else
                        PartikelSystem.PushPartikel(xPos - 5, yPos, SMOKE);
                }

                Counter = 1.0f;
                Damage--;

                if (Damage <= 0)
                    ExplodeShot();
            }
        } break;

        case FLUGLASER: {
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM) {
                Damage = 0;
                ExplodeShot();
            }

        } break;

        case ARCSHOT: {
            // Rauch erzeugen
            AnimCount -= Timer.sync(1.0f);
            if (AnimCount <= 0.0f) {
                PartikelSystem.PushPartikel(xPos - 2, yPos - 2, ROCKETSMOKEBLUE);
                AnimCount = 0.1f;
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case SNOWBOMB: {
            // Rauch erzeugen
            //			for (int i=0; i < static_cast<int>(50 * Timer.getElapsed())+1; i++)
            //				PartikelSystem.PushPartikel(xPos - 2, yPos - 2, ROCKETSMOKEBLUE);

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case SNOWBOMBSMALL: {
            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM) {
                // Abhopsen
                //
                if (ySpeed > 0.0f)
                    ySpeed = -ySpeed / 1.5f;

                // Bis nich mehr geht ;)
                //
                if (ySpeed > -0.5f) {
                    Damage = 0;
                    ExplodeShot();
                }
            }
        } break;

        case FETTESPINNESHOT:
        case FETTESPINNESHOT2: {
            if (AnimPhase > AnimEnde)
                AnimPhase = 0;

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case EIERBOMBE: {
            // Links oder rechts rum kullern ?

            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = AnimCount - AnimSpeed;  // Dann wieder auf Null setzen

                // linksrum
                if (xSpeed < 0.0f) {
                    AnimPhase--;        // Und nächste Animationsphase
                    if (AnimPhase < 0)  // Animation von zu Ende	?
                    {
                        // DKS - off-by-one error:
                        // DKS - TODO double check I got this right
                        // AnimPhase = 19;
                        // AnimEnde  = 20;
                        AnimPhase = 18;
                        AnimEnde = 19;
                    }
                }

                // rechtsrum
                else {
                    AnimPhase++;               // Und nächste Animationsphase
                    if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
                    {
                        AnimPhase = 0;
                        AnimEnde = 19;
                    }
                }
            }

            // Kugel fällt runter ?
            //
            if (ySpeed > 30.0f)  // Maximale Fallgeschwindigkeit
                ySpeed = 30.0f;

            if (!(bu & BLOCKWERT_WAND) && !(bu & BLOCKWERT_PLATTFORM) && ySpeed == 0.0f) {
                ySpeed = 0.0f;
                yAcc = 4.0f;
            }

            // Kugel hopft am Boden ab ?
            //
            if (ySpeed > 0.0f && (bu & BLOCKWERT_WAND || bu & BLOCKWERT_PLATTFORM)) {
                ySpeed *= -0.75f;

                if (ySpeed > -1.0f) {
                    ySpeed = 0.0f;
                    yAcc = 0.0f;

                    // DKS - This statement had no effect, since default parameter 'resolv' is false,
                    //      so I disabled it:
                    // TileEngine.BlockUnten(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]);
                }
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }

        } break;

        case EVILROUND1: {
            AnimCount -= Timer.sync(1.0f);

            if (AnimCount < 0.0f) {
                // Rauch erzeugen
                for (int i = 0; i < 2; i++)
                    PartikelSystem.PushPartikel(xPos - 2.0f, yPos - 2.0f, EVILROUNDSMOKE);

                AnimCount += 0.1f;
            }

            if (bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND || bo & BLOCKWERT_WAND || bu & BLOCKWERT_WAND ||
                bu & BLOCKWERT_PLATTFORM) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        case SMARTBOMB: {
            AnimCount += Timer.sync(32.0f);
            xPos -= Timer.sync(32.0f);
            yPos -= Timer.sync(32.0f);
            if (AnimCount > 256.0f)
                AnimCount = 256.0f;

            Damage = static_cast<int>(255.0f - AnimCount) * 2;
        } break;

        case GRENADE: {
            // rauchen lassen
            //
            AnimCount -= Timer.sync(1.0f);

            while (AnimCount <= 0.0f) {
                AnimCount += 0.1f;
                PartikelSystem.PushPartikel(xPos - 4.0f, yPos - 4.0f + static_cast<float>(random(3)), SMOKE2);
            }

            if (TileEngine.BlockUntenNormal(xPos, yPos, xPosOld, yPosOld, ShotRect[ShotArt]) & BLOCKWERT_WAND ||
                bo & BLOCKWERT_WAND || bl & BLOCKWERT_WAND || br & BLOCKWERT_WAND) {
                Damage = 0;
                ExplodeShot();
            }
        } break;

        // Der Satellit, der das Schutzschild erzeugt, wird immer im Kreis um den Spieler bewegt. Da die Position
        // immer neu berechnet wird, werden xspeed und yspeed nicht benötigt. Daher können sie zweckentfremdet werden,
        // um die aktuelle Rotation zu speichern (xspeed) und den counter für die shield-partikel (yspeed)
        //
        case SHIELDSPAWNER: {
            // Schild um Spieler kreisen lassen
            if (pParent->Blickrichtung == DirectionEnum::LINKS)
                ySpeed += Timer.sync(0.5f);
            else
                ySpeed -= Timer.sync(0.5f);

            // Grenzen checken
            if (ySpeed > TWO_PI)
                ySpeed -= TWO_PI;
            if (ySpeed < 0.0f)
                ySpeed += TWO_PI;

            if (pParent->Handlung != PlayerActionEnum::RADELN &&
                    pParent->Handlung != PlayerActionEnum::RADELN_FALL) {
                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // xPos = float (pParent->xpos + 28 + sin (ySpeed) * 40);
                // yPos = float (pParent->ypos + 38 + cos (ySpeed) * 40);
                xPos = pParent->xpos + 28.0f + sin_rad(ySpeed) * 40.0f;
                yPos = pParent->ypos + 38.0f + cos_rad(ySpeed) * 40.0f;
            } else {
                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // xPos = float (pParent->xpos + 28 + sin (ySpeed) * 30);
                // yPos = float (pParent->ypos + 55 + cos (ySpeed) * 30);
                xPos = pParent->xpos + 28.0f + sin_rad(ySpeed) * 30.0f;
                yPos = pParent->ypos + 55.0f + cos_rad(ySpeed) * 30.0f;
            }

            // Schild Partikel spawnen
            xSpeed -= Timer.sync(1.0f);

            if (xSpeed < 0.0f) {
                xSpeed = 0.1f;
                PartikelSystem.PushPartikel(xPos, yPos, SHIELD, pParent);
            }

            // Spawner verschwinden lassen ?
            if (pParent->Shield <= 0.0f) {
                ExplodeShot();
                Damage = 0;
            }

        } break;

        default:
            break;
    }

    // Hat ein Gegnerschuss den Spieler getroffen ?
    // bool coll1 = false;
    // bool coll2 = false;

    for (int p = 0; p < NUMPLAYERS; p++)
        if (DamagePlayer &&
            SpriteCollision(xPos, yPos, ShotRect[ShotArt], Player[p].xpos, Player[p].ypos, Player[p].CollideRect)) {
            // Schüsse, die durch den Spieler durchgehen
            if (!ExplodeOnImpact)
                Player[p].DamagePlayer(Timer.sync(Damage));

            // Schüsse, die bei Berührung verschwinden
            else {
                Player[p].DamagePlayer(static_cast<float>(Damage));

                ExplodeShot();  // Schuss Explodieren lassen
                Damage = 0;     // und verschwinden lassen
            }

            // Spieler wurde gerendert -> Textur ging flöten =)
            CurrentShotTexture = -1;
        }

    // Schüsse Additiv rendern ?
    if (ShotArt >= FLAMME)
        DirectGraphics.SetAdditiveMode();
    else
        DirectGraphics.SetColorKeyMode();

    // D3DCOLOR Color;

    // Color = 0xFFFFFFFF;

    // Evil Blitz fadet auch aus
    // if (ShotArt == EVILBLITZ2 && ySpeed == 0.0f)
    //{
    // int a = int (AnimCount);
    // Color = D3DCOLOR_RGBA(255, 255, 255, a);
    //}

    // Alte Position sichern
    xPosOld = xPos;
    yPosOld = yPos;
}

// --------------------------------------------------------------------------------------
// Schuss "explodieren" lassen und Partikel erzeugen
// Bei Kollision mit Level oder Gegner
// --------------------------------------------------------------------------------------

void ProjectileClass::ExplodeShot() {
    CurrentShotTexture = -1;

    // Jeder Schuss erzeugt beim explodieren andere Partikel
    switch (ShotArt) {
        case FIREBALL: {
            SoundManager.PlayWave(80, 128, 15000 + random(2000), SOUND::EXPLOSION1);
            PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

            for (int i = 0; i < 4; i++) {
                PartikelSystem.PushPartikel(xPos + 5.0f + static_cast<float>(random(20)),
                                            yPos + 5.0f + static_cast<float>(random(10)), FUNKE);
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(20)),
                                            yPos - 5.0f + static_cast<float>(random(10)), SMOKE);
                PartikelSystem.PushPartikel(xPos + 5.0f + static_cast<float>(random(20)),
                                            yPos + 5.0f + static_cast<float>(random(10)), LASERFUNKE2);
            }

            PartikelSystem.PushPartikel(xPos - 10.0f, yPos, EXPLOSION_MEDIUM2);
        } break;

        case FIREBALL_BIG: {
            SoundManager.PlayWave(80, 128, 10000 + random(2000), SOUND::EXPLOSION1);
            PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

            for (int i = 0; i < 4; i++) {
                PartikelSystem.PushPartikel(xPos + 5.0f + static_cast<float>(random(20)),
                                            yPos + 5.0f + static_cast<float>(random(10)), FUNKE);
                PartikelSystem.PushPartikel(xPos + static_cast<float>(random(20)),
                                            yPos - 5.0f + static_cast<float>(random(10)), SMOKEBIG);
                PartikelSystem.PushPartikel(xPos + 5.0f + static_cast<float>(random(20)),
                                            yPos + 5.0f + static_cast<float>(random(10)), LASERFUNKE2);
            }

            PartikelSystem.PushPartikel(xPos - 10.0f, yPos - 20.0f, EXPLOSION_BIG);
        } break;

        case FIREBALL_BOMB: {
            SoundManager.PlayWave(80, 128, 10000 + random(2000), SOUND::EXPLOSION1);

            for (int i = 0; i < 2; i++)
                PartikelSystem.PushPartikel(xPos - 30.0f, yPos - 20.0f, EXPLOSIONFLARE);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(20)),
                                            yPos - 5.0f + static_cast<float>(random(10)), SMOKEBIG);

            for (int i = 0; i < 8; i++)
                Projectiles.PushProjectile(xPos - 20.0f + static_cast<float>(random(30)),
                                           yPos + 20.0f + static_cast<float>(random(30)), FEUERFALLE);

        } break;

        case ROTZSHOT: {
            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::SPREADHIT);
            PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

            for (int i = 0; i < 4; i++) {
                PartikelSystem.PushPartikel(xPos - 5.0f + static_cast<float>(random(10)),
                                            yPos - 5.0f + static_cast<float>(random(10)), FUNKE);
                PartikelSystem.PushPartikel(xPos - 5.0f + static_cast<float>(random(10)),
                                            yPos - 5.0f + static_cast<float>(random(10)), LASERFUNKE2);
            }
        } break;

        case DIAMONDSHOT: {
            PartikelSystem.PushPartikel(xPos, yPos, DIAMANTCOLLECTED);
            SoundManager.PlayWave(50, 128, 11025, SOUND::EXPLOSION1);
            SoundManager.PlayWave(100, 128, 11025, SOUND::COLLECT);
        } break;

        case SKELETOR_SHOT: {
            PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 5.0f, SHOTFLARE);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f, yPos + 10.0f, FUNKE);

            if (random(3) == 0)
                for (int i = 0; i < 3; i++)
                    if (random(2) == 0)
                        PartikelSystem.PushPartikel(xPos + 10.0f, yPos + 5.0f, LONGFUNKE);
        } break;

        case SPREADSHOT:
        case SPREADSHOT2: {
            PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 5.0f, FUNKE);

            if (random(3) == 0)
                for (int i = 0; i < 3; i++)
                    if (random(2) == 0)
                        // DKS - Fixed yPos offset here. If you turned off drawing of front
                        //      and back overlay tiles, you could see that many of these
                        //      particles were getting trapped in the wall, especially when
                        //      firing directly upwards and hence were never seen at all.
                        // PartikelSystem.PushPartikel(xPos+5, yPos-15, LONGFUNKE);
                        PartikelSystem.PushPartikel(xPos + 5, yPos + 5, LONGFUNKE);
        } break;

        case SPREADSHOTBIG:
        case SPREADSHOTBIG2: {
            SoundManager.PlayWave(50, 128, random(2000) + 11025, SOUND::SPREADHIT);
            for (int i = 0; i < 8; i++)
                PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 5.0f, FUNKE);

            for (int i = 0; i < 3; i++)
                PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 5.0f, LONGFUNKE);
        } break;

        case LASERSHOT:
        case LASERSHOT2: {
            PartikelSystem.PushPartikel(xPos - 50.0f, yPos - 30.0f, LASERFLARE);

            for (int i = 0; i < 3; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f, yPos + 5.0f, LASERFUNKE);

            if (xSpeed > 0.0f)
                PartikelSystem.PushPartikel(xPos + 25.f, yPos + 8.0f, BLUE_EXPLOSION);
            else
                PartikelSystem.PushPartikel(xPos - 5.f, yPos + 8.0f, BLUE_EXPLOSION);
        } break;

        case LASERSHOTBIG:
        case LASERSHOTBIG2: {
            for (int i = 0; i < 8; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(70)),
                                            yPos + 5.0f, LASERFUNKE);

            if (xSpeed > 0.0f)
                for (int i = 0; i < 5; i++)
                    PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(random(70)),
                                                yPos - 10.0f + static_cast<float>(random(20)), BLUE_EXPLOSION);
            else
                for (int i = 0; i < 5; i++)
                    PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(random(20)),
                                                yPos - 10.0f + static_cast<float>(random(20)), BLUE_EXPLOSION);
        } break;

        case BOUNCESHOT1: {
            PartikelSystem.PushPartikel(xPos + 4.0f, yPos + 4.0f, SHOTFLARE2);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos + 8.0f, yPos + 8.0f, FUNKE2);

            // Aufsplitten ?
            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] > 2) {
                if (bl & BLOCKWERT_WAND) {
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                } else if (br & BLOCKWERT_WAND) {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                } else if (bo & BLOCKWERT_WAND) {
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                } else {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 4.0f - Timer.sync(xSpeed), yPos + 4.0f - Timer.sync(ySpeed), BOUNCESHOT2, pParent);
                }
            }

        } break;

        case BOUNCESHOT2: {
            SoundManager.StopWave(SOUND::BOUNCESHOT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::BOUNCESHOT);

            PartikelSystem.PushPartikel(xPos, yPos, SHOTFLARE2);

            for (int i = 0; i < 2; i++)
                PartikelSystem.PushPartikel(xPos + 8.0f, yPos + 8.0f, FUNKE2);

            // Aufsplitten ?
            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 5) {
                if (bl & BLOCKWERT_WAND) {
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                } else if (br & BLOCKWERT_WAND) {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                } else if (bo & BLOCKWERT_WAND) {
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                } else {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 3.0f - Timer.sync(xSpeed), yPos + 3.0f - Timer.sync(ySpeed), BOUNCESHOT3, pParent);
                }
            }

        } break;

        case BOUNCESHOT3: {
            SoundManager.StopWave(SOUND::BOUNCESHOT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::BOUNCESHOT);

            for (int i = 0; i < 3; i++)
                PartikelSystem.PushPartikel(xPos + 4.0f, yPos + 4.0f, FUNKE2);

        } break;

        case BOUNCESHOTBIG1: {
            SoundManager.StopWave(SOUND::BOUNCESHOT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::BOUNCESHOT);

            PartikelSystem.PushPartikel(xPos - 32.0f, yPos - 32.0f, EXPLOSIONFLARE);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos + 8.0f, yPos + 8.0f, FUNKE2);

            // Aufsplitten ?
            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] > 2) {
                if (bl & BLOCKWERT_WAND) {
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                } else if (br & BLOCKWERT_WAND) {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                } else if (bo & BLOCKWERT_WAND) {
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                } else {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG2, pParent);
                }
            }

        } break;

        case BOUNCESHOTBIG2: {
            SoundManager.StopWave(SOUND::BOUNCESHOT);
            SoundManager.PlayWave(100, 128, 11025, SOUND::BOUNCESHOT);

            PartikelSystem.PushPartikel(xPos - 32.0f, yPos - 32.0f, EXPLOSIONFLARE);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos + 8.0f, yPos + 8.0f, FUNKE2);

            // Aufsplitten ?
            if (pParent->CurrentWeaponLevel[pParent->SelectedWeapon] >= 5) {
                if (bl & BLOCKWERT_WAND) {
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                } else if (br & BLOCKWERT_WAND) {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                } else if (bo & BLOCKWERT_WAND) {
                    WinkelUebergabe = 225;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                    WinkelUebergabe = 135;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                } else {
                    WinkelUebergabe = 315;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                    WinkelUebergabe = 45;
                    Projectiles.PushProjectile(xPos + 8.0f - Timer.sync(xSpeed), yPos + 8.0f - Timer.sync(ySpeed), BOUNCESHOTBIG3, pParent);
                }
            }

        } break;

        case CANONBALL:
        case SUCHSCHUSS: {
            SoundManager.PlayWave(25, 128, 11025, SOUND::SPREADHIT);
            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos + 5.0f, yPos + 5.0f, FUNKE);

            PartikelSystem.PushPartikel(xPos - 10.0f, yPos - 10.0f, SMOKE);
        } break;

        case SPIDERSHOT:
        case SPIDERSHOT2:
        case SPIDERSLOW: {
            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(random(40)),
                                            yPos - 10.0f + static_cast<float>(random(40)), LILA2);

            PartikelSystem.PushPartikel(xPos - 40.0f, yPos - 40.0f, EXPLOSIONFLARE2);
        } break;

        case ELEKTROSCHUSS: {
            SoundManager.PlayWave(100, 128, 6000, SOUND::BLITZENDE);
            SoundManager.PlayWave(100, 128, 6000, SOUND::BLITZENDE);
            SoundManager.PlayWave(100, 128, 9000, SOUND::BLITZENDE);
            SoundManager.PlayWave(100, 128, 9000, SOUND::BLITZENDE);

            Projectiles.PushProjectile(TileEngine.XOffset - 60.0f, TileEngine.YOffset + 480.0f - 128.0f, ELEKTROPAMPE);
        } break;

            /*
                    case SPIDERSHOT :
                    {
                        //Projectiles.PushProjectile(xPos, yPos, SPIDERSHOT3);
                    } break;
            */

        case SUCHSCHUSS2: {
            SoundManager.PlayWave(25, 128, 11025, SOUND::FUNKE2);
            SoundManager.PlayWave(25, 128, 11025, SOUND::FUNKE3);

            PartikelSystem.PushPartikel(xPos - 50.0f, yPos - 50.0f, LASERFLARE);

            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos + random(12), yPos + random(12), LASERFUNKE);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos - 4.0f + random(8), yPos - 4.0f + random(8), SMOKE3);
        } break;

        case TURRIEXTRAWURST:
        case GOLEMSCHUSS: {
            SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION3);

            for (int i = 0; i < 40; i++)
                PartikelSystem.PushPartikel(xPos + random(35), yPos + random(35), FUNKE2);
        } break;

        case WALKER_LASER:
        case SPIDER_LASER: {
            SoundManager.PlayWave(25, 128, 11025, SOUND::SPREADHIT);
            for (int i = 0; i < 8; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f, yPos, FUNKE);
        } break;

        case PHARAOLASER: {
            for (int i = 0; i < 50; i++)
                PartikelSystem.PushPartikel(xPos + random(48) - 6.0f, yPos + random(48) - 6.0f, PHARAOSMOKE);
        } break;

        case ROCKET:
        case ROCKETSPIDER: {
            PartikelSystem.PushPartikel(xPos + 8, yPos + 8, EXPLOSION_MEDIUM2);

            for (int i = 0; i < 100; i++) {
                PartikelSystem.PushPartikel(xPos + random(55) + 10.0f - 6.0f, yPos + random(55) + 10.0f - 6.0f, ROCKETSMOKE);
                PartikelSystem.PushPartikel(xPos + random(35) + 20.0f - 6.0f, yPos + random(35) + 20.0f - 6.0f, ROCKETSMOKE);
            }

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case ROCKETWERFER: {
            PartikelSystem.PushPartikel(xPos + 8, yPos - 20, EXPLOSION_MEDIUM2);

            for (int i = 0; i < 75; i++) {
                PartikelSystem.PushPartikel(xPos + random(40) + 18.0f, yPos + random(40) - 20.0f, ROCKETSMOKE);
                PartikelSystem.PushPartikel(xPos + random(20) + 26.0f, yPos + random(20) - 10.0f, ROCKETSMOKE);
            }

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case TORPEDO: {
            PartikelSystem.PushPartikel(xPos + 8.0f, yPos - 20.0f, EXPLOSION_MEDIUM2);

            for (int i = 0; i < 20; i++)
                PartikelSystem.PushPartikel(xPos + random(33), yPos + random(10), BUBBLE);

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case EVILSHOT:
        case EVILSHOT2: {
            PartikelSystem.PushPartikel(xPos, yPos - 20.0f, EXPLOSION_MEDIUM2);

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case FLUGLASER: {
            for (int i = 0; i < 12; i++)
                PartikelSystem.PushPartikel(xPos + random(4), yPos + 10.0f + random(14), FUNKE);

            for (int i = 0; i < 6; i++)
                PartikelSystem.PushPartikel(xPos + random(4), yPos + 10.0f + random(14), LASERFUNKE2);

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::SPREADHIT);
        } break;

        case EISZAPFENSHOT: {
            PartikelSystem.PushPartikel(xPos - 10.0f, yPos + 8.0f, SNOWFLUSH);

            SoundManager.PlayWave(25, 128, 8000 + random(4000), SOUND::SPREADHIT);
        } break;

        case EVILBLITZ: {
            Projectiles.PushProjectile(xPos - 290.0f, yPos - 300.0f, EVILBLITZ2);
            Projectiles.PushProjectile(xPos - 100.0f, yPos - 200.0f, EVILBLITZ2);
            Projectiles.PushProjectile(xPos + 100.0f, yPos - 100.0f, EVILBLITZ2);
            Projectiles.PushProjectile(xPos + 290.0f, yPos, EVILBLITZ2);

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case BLITZBEAM: {
            // Wirkung ähnlich einer schwachen Granate (Damage = 0 - 100)
            //
            Gegner.DamageEnemiesonScreen(xPos, yPos, Damage);

            // Partikel
            //
            for (int i = 0; i < 300; i++)
                PartikelSystem.PushPartikel(xPos + Damage / 2.0f, yPos + Damage / 2.0f, BEAMSMOKE3);
            for (int i = 0; i < 100; i++)
                PartikelSystem.PushPartikel(xPos + Damage / 2.0f - 12, yPos + Damage / 2.0f - 12, BEAMSMOKE4);

            // 2 Druckwellen (xPos um 1 Pixel verschoben, dadurch breitet sich die eine mit 30, die andere mit 10 Pixeln
            // aus)
            //

            PartikelSystem.PushPartikel(xPos - 32.0f + static_cast<int>(Damage / 2),
                                        yPos - 32.0f + static_cast<int>(Damage / 2), BEAMSMOKE5);
            PartikelSystem.PushPartikel(xPos - 31.0f + static_cast<int>(Damage / 2),
                                        yPos - 32.0f + static_cast<int>(Damage / 2), BEAMSMOKE5);

            // Screen aufblitzen
            PartikelSystem.ThunderAlpha = Damage * 2.0f;

            if (PartikelSystem.ThunderAlpha > 255.0f)
                PartikelSystem.ThunderAlpha = 255.0f;

            PartikelSystem.ThunderColor[0] = 96;
            PartikelSystem.ThunderColor[1] = 128;
            PartikelSystem.ThunderColor[2] = 255;

            Damage = 0;

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::BLITZENDE);
            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);

        } break;

            /*
                    case UFOLASER:
                    {
                        for (int i=0; i < 50; i++)
                            PartikelSystem.PushPartikel(xPos + rand()%42,      yPos + rand()%100 , LASERFUNKE);

                        for (i=0; i < 20; i++)
                            PartikelSystem.PushPartikel(xPos - 10 + rand()%42, yPos + 50 + rand()%20 , BLUE_EXPLOSION);

                        SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND::EXPLOSION1);
                    } break;
            */

        case STELZLASER: {
            for (int i = 0; i < 50; i++)
                PartikelSystem.PushPartikel(xPos + random(60) - 6.0f, yPos + random(35) + 5.0f, PHARAOSMOKE);

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::SPREADHIT);
        } break;

        case PFLANZESHOT: {
            for (int i = 0; i < 20; i++)
                PartikelSystem.PushPartikel(xPos + random(16), yPos + random(16), FUNKE);

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::SPREADHIT);
        } break;

        case BRATKLOPSSHOT: {
            for (int i = 0; i < 5; i++)
                PartikelSystem.PushPartikel(xPos - 20.0f + random(40), yPos - 20.0f + random(40), EXPLOSION_GREEN);

            for (int i = 0; i < 100; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f + random(40), yPos + 10.0f + random(40), FUNKE2);

            SoundManager.PlayWave(100, 128, 10000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case DRONEBULLET: {
            SoundManager.PlayWave(50, 128, random(2000) + 11025, SOUND::SPREADHIT);
            for (int i = 0; i < 12; i++)
                PartikelSystem.PushPartikel(xPos + 2.0f, yPos + 2.0f, FUNKE);

            PartikelSystem.PushPartikel(xPos, yPos, SMOKE3);

            if (random(3) == 0)
                for (int i = 0; i < 3; i++)
                    if (random(2) == 0)
                        PartikelSystem.PushPartikel(xPos + 2.0f, yPos - 12.0f, LONGFUNKE);
        } break;

        case SCHLEIMSHOT: {
            SoundManager.PlayWave(50, 128, random(2000) + 11025, SOUND::MADE);
            PartikelSystem.PushPartikel(xPos, yPos, MADEBLUT);
        } break;

        case SKELETORGRANATE: {
            SoundManager.PlayWave(50, 128, random(4000) + 8000, SOUND::EXPLOSION3);

            PartikelSystem.PushPartikel(xPos - 20.0f, yPos - 20.0f, EXPLOSION_MEDIUM2);

            for (int i = 0; i < 2; i++)
                PartikelSystem.PushPartikel(xPos - 10.0f + random(5), yPos - 10.0f, SPIDERSPLITTER);

            for (int i = 0; i < 15; i++)
                Projectiles.PushProjectile(xPos - 20.0f, yPos + i * 9.0f, FEUERFALLE);
        } break;

        case BLUEBOMB: {
            SoundManager.PlayWave(50, 128, random(4000) + 8000, SOUND::EXPLOSION3);

            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos - 10.0f + random(20), yPos - 10.0f + random(20), BLUE_EXPLOSION);

            for (int i = 0; i < 15; i++)
                PartikelSystem.PushPartikel(xPos - 10.0f + random(20), yPos - 10.0f + random(20), WATERFLUSH_HIGH2);

        } break;

        case LAFASSSHOT: {
            SoundManager.PlayWave(100, 128, random(2000) + 11025, SOUND::EXPLOSION3);
            PartikelSystem.PushPartikel(xPos, yPos, EXPLOSION_TRACE);

            for (int i = 0; i < 20; i++)
                PartikelSystem.PushPartikel(xPos + 10.0f + random(40), yPos + 10.0f + random(60), SPIDERSPLITTER);

            for (int i = 0; i < 5; i++)
                PartikelSystem.PushPartikel(xPos - 30.0f + random(66), yPos - 30.0f + random(83), EXPLOSION_MEDIUM2);
        } break;

        case ARCSHOT: {
            // PartikelSystem.PushPartikel(xPos + 8, yPos + 8, EXPLOSION_MEDIUM2);

            for (int i = 0; i < 50; i++) {
                PartikelSystem.PushPartikel(xPos + random(20) - 10.0f, yPos + random(20) - 10.0f, ROCKETSMOKEBLUE);
                PartikelSystem.PushPartikel(xPos + random(20) - 10.0f, yPos + random(20) - 10.0f, ROCKETSMOKEBLUE);
            }

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case SNOWBOMB: {
            for (int i = 0; i < 8; i++)
                Projectiles.PushProjectile(xPos + random(60), yPos + random(50), SNOWBOMBSMALL);

            for (int i = 0; i < 20; i++)
                PartikelSystem.PushPartikel(xPos + random(70) - 10.0f, yPos + random(70) - 10.0f, SNOWFLUSH);

            SoundManager.PlayWave(100, 128, 6000 + random(2000), SOUND::LANDEN);
        } break;

        case SNOWBOMBSMALL: {
            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos - 25.0f + random(20), yPos - 25.0f + random(20), SNOWFLUSH);

            SoundManager.PlayWave(100, 128, 6000 + random(2000), SOUND::LANDEN);
        } break;

        case FETTESPINNESHOT:
        case FETTESPINNESHOT2: {
            SoundManager.PlayWave(100, 128, 16000, SOUND::SPREADHIT);
            SoundManager.PlayWave(100, 128, 8000, SOUND::SPREADHIT);

            for (int i = 0; i < 100; i++)
                PartikelSystem.PushPartikel(xPos - 5.0f + random(40), yPos - 5.0f + random(50), SMOKE2);

            for (int i = 0; i < 30; i++)
                PartikelSystem.PushPartikel(xPos + random(40), yPos + random(50), FUNKE);

            for (int i = 0; i < 40; i++)
                PartikelSystem.PushPartikel(xPos + random(20) + 10.0f, yPos + random(30) + 10.0f, PHARAOSMOKE);

        } break;

        case EIERBOMBE: {
            SoundManager.PlayWave(100, 128, 11025, SOUND::EXPLOSION1);

            for (int i = 0; i < 30; i++)
                PartikelSystem.PushPartikel(xPos - 20.0f + random(60), yPos - 20.0f + random(60), SMOKE3);

            PartikelSystem.PushPartikel(xPos - 40.0f, yPos - 40.0f, EXPLOSION_GIANT);

            for (int i = 0; i < 4; i++)
                PartikelSystem.PushPartikel(xPos - 30.0f + random(40), yPos - 30.0f + random(40), EXPLOSION_MEDIUM2);

        } break;

        case EVILROUND1: {
            for (int i = 0; i < 50; i++) {
                PartikelSystem.PushPartikel(xPos + random(20) - 10.0f, yPos + random(20) - 10.0f, EVILROUNDSMOKE);
            }

            SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
        } break;

        case BOMBE:
        case BOMBEBIG: {
            float xdiff, ydiff, Abstand;

            // Prüfen, ob die Bombe eine zerstörbare Wand getroffen hat
            //
            TileEngine.ExplodeWalls(static_cast<int>(xPos) / 20, static_cast<int>(yPos) / 20);

            // Explodieren lassen
            SoundManager.PlayWave(80, 128, 11025, SOUND::EXPLOSION1);
            SoundManager.PlayWave(50, 128, 8000 + random(2000), SOUND::EXPLOSION3);

            int g = 2;

            if (ShotArt == BOMBEBIG)
                g = 5;

            for (int i = 0; i < g; i++) {
                PartikelSystem.PushPartikel(xPos - 26.0f, yPos - 26.0f, SMOKEBIG);
                PartikelSystem.PushPartikel(xPos + random(4), yPos + random(4), SPIDERSPLITTER);
            }

            PartikelSystem.PushPartikel(xPos - 24.0f, yPos - 24.0f, EXPLOSION_MEDIUM2);

            if (ShotArt == BOMBEBIG)
                PartikelSystem.PushPartikel(xPos - 55.0f, yPos - 55.0f, EXPLOSION_GIANT);

            // Prüfen, ob ein Gegner in der Nähe war und dann je nach Abstand
            // Energie abziehen
            GegnerClass *pEnemy;  // Gegner zum Kollision testen

            // je nach bombentyp mehr schaden
            int schaden = 80;

            if (ShotArt == BOMBEBIG)
                schaden = 250;

            pEnemy = Gegner.pStart;  // Anfang der Gegnerliste
            while (pEnemy != nullptr)   // Noch nicht alle durch ?
            {
                xdiff = (xPos + ShotRect[ShotArt].left + (ShotRect[ShotArt].right - ShotRect[ShotArt].left) / 2) -
                        (pEnemy->xPos + GegnerRect[pEnemy->GegnerArt].right / 2);
                ydiff = (yPos + ShotRect[ShotArt].top + (ShotRect[ShotArt].bottom - ShotRect[ShotArt].top) / 2) -
                        (pEnemy->yPos + GegnerRect[pEnemy->GegnerArt].bottom / 2);

                // DKS - Converted to float:
                Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

                if (pEnemy->Active &&               // Ist der Gegner überhaupt aktiv ?
                    pEnemy->Destroyable &&          // und zerstörbar ?
                    Abstand <= schaden &&           // Und der Abstand klein genug ?
                    pEnemy->GegnerArt != EXTRAS &&  // Und kein Extra ?
                    pEnemy->GegnerArt != ONEUP &&   // Und kein OneUp ?
                    pEnemy->GegnerArt != DIAMANT)   // Und kein Diamant ?
                {
                    // blinken lassen, wenn noch nicht blinkt
                    if (pEnemy->DamageTaken <= 0.0f)
                        pEnemy->DamageTaken = 255;

                    pEnemy->Energy -= (schaden - Abstand) * 1.5f;  // Dann Energy abziehen je nach Abstand
                }
                pEnemy = pEnemy->pNext;  // Nächsten Gegner testen
            }

            // Prüfen, ob der Spieler in der Nähe war, und falls ja, dann diesen
            // nach oben katapultieren
            for (int p = 0; p < NUMPLAYERS; p++) {
                xdiff = (xPos + ShotRect[ShotArt].left + (ShotRect[ShotArt].right - ShotRect[ShotArt].left) / 2) -
                        (Player[p].xpos + Player[p].CollideRect.right / 2);
                ydiff = (yPos + ShotRect[ShotArt].top + (ShotRect[ShotArt].bottom - ShotRect[ShotArt].top) / 2) -
                        (Player[p].ypos + Player[p].CollideRect.bottom / 2);

                // DKS - Converted to float:
                Abstand = sqrtf((xdiff * xdiff) + (ydiff * ydiff));

                if (((ShotArt == BOMBE && Abstand <= 60) ||
                        (ShotArt == BOMBEBIG && Abstand <= 80)) &&
                        (Player[p].Handlung == PlayerActionEnum::RADELN ||
                        Player[p].Handlung == PlayerActionEnum::RADELN_FALL)) {
                    if (ShotArt == BOMBE)
                        Player[p].yspeed -= 45.0f;  // Bombe
                    else
                        Player[p].yspeed -= 75.0f;  // SuperBombe

                    Player[p].JumpAdd = PLAYER_JUMPADDSPEED;  // lassen
                    Player[p].Handlung = PlayerActionEnum::RADELN_FALL;         // als Rad
                }
            }

        } break;

        case FLAMME:  // Flammen-Splitter
        {
            SoundManager.PlayWave(25, 128, 11025, SOUND::EXPLOSION1);
            PartikelSystem.PushPartikel(xPos - 16, yPos - 16, EXPLOSION_MEDIUM2);

            for (int i = 0; i < 8; i++)
                PartikelSystem.PushPartikel(xPos + 4.0f, yPos + 4.0f, FUNKE);

        } break;

        case KRABBLERLASER1: {
            SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::SPREADHIT);

            for (int i = 0; i < 24; i++)
                PartikelSystem.PushPartikel(xPos - 1.0f + random(2), yPos + static_cast<float>(i * 2) + random(2), LASERFUNKE2);
        } break;

        case KRABBLERLASER2: {
            SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::SPREADHIT);

            for (int i = 0; i < 8; i++)
                PartikelSystem.PushPartikel(xPos + 36.0f, yPos, LASERFUNKE2);
        } break;

        case KRABBLERLASER3: {
            SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::SPREADHIT);

            for (int i = 0; i < 8; i++)
                PartikelSystem.PushPartikel(xPos, yPos, LASERFUNKE2);
        } break;

        case GRENADE: {
            SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::EXPLOSION1);

            PartikelSystem.PushPartikel(xPos - 100.0f, yPos - 100.0f, EXPLOSION_GIGA);

            PartikelSystem.PushPartikel(xPos - 20.0f, yPos - 20.0f, EXPLOSION_MEDIUM2);

            for (int i = 0; i < 100; i++) {
                PartikelSystem.PushPartikel(xPos - 20.0f + random(40), yPos - 20.0f + random(40), ROCKETSMOKE);

                PartikelSystem.PushPartikel(xPos - 20.0f + random(40), yPos - 20.0f + random(40), FUNKE);
            }

            for (int i = 0; i < 50; i++)
                PartikelSystem.PushPartikel(xPos - 20.0f + random(40), yPos - 20.0f + random(40), LONGFUNKE);

            PartikelSystem.PushPartikel(xPos - 128.0f, yPos - 128.0f, GRENADEFLARE);

            ShakeScreen(5);
            Gegner.DamageEnemiesonScreen(xPos, yPos, 400);
            TileEngine.ExplodeWalls(static_cast<int>(xPos) / 20, static_cast<int>(yPos) / 20);

            for (int p = 0; p < NUMPLAYERS; p++)
                DirectInput.Joysticks[Player[p].JoystickIndex].ForceFeedbackEffect(FFE_MEDIUMRUMBLE);

        } break;

        case SHIELDSPAWNER: {
            SoundManager.PlayWave(75, 128, 15000 + random(2000), SOUND::EXPLOSION1);

            for (int i = 0; i < 10; i++)
                PartikelSystem.PushPartikel(xPos - 5.0f + random(10), yPos - 5.0f + random(10), SMOKE);
            for (int i = 0; i < 30; i++)
                PartikelSystem.PushPartikel(xPos - 5.0f + random(10), yPos - 5.0f + random(10), FUNKE);
        } break;

        default:
            break;
    }

    Damage = 0;
}

// --------------------------------------------------------------------------------------
// ProjectileListClass Funktionen
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Konstruktor : laden der Projektil Grafiken
// --------------------------------------------------------------------------------------
ProjectileListClass::ProjectileListClass() {
    pStart = nullptr;
    pEnd = nullptr;
    NumProjectiles = 0;
}

void ProjectileListClass::LoadSprites() {
    // DKS - All of these 5 sprites are no longer globals, I moved them here cleaning up big messes
    //      and fixing ambiguous orders of calls to destructors.
    // Grafiken für das Leuchten diverser Schüsse laden
    LaserSmoke.LoadImage("lasersmoke.png", 128, 128, 128, 128, 1, 1);
    LaserSmokeBig.LoadImage("lasersmoke_big.png", 256, 256, 256, 256, 1, 1);
    SpreadShotSmoke.LoadImage("spreadshotsmoke.png", 64, 64, 64, 64, 1, 1);
    PowerlineSmoke.LoadImage("powerlinesmoke.png", 160, 20, 80, 20, 2, 1);
    LavaFlare.LoadImage("lavaflare.png", 120, 120, 120, 120, 1, 1);

    // DKS - Moved these three here from being globals in Player.cpp (same reasoning as above note)
    Blitzstrahl[0].LoadImage("blitzstrahl1.png", 32, 32, 32, 32, 1, 1);
    Blitzstrahl[1].LoadImage("blitzstrahl2.png", 32, 32, 32, 32, 1, 1);
    Blitzstrahl[2].LoadImage("blitzstrahl3.png", 32, 32, 32, 32, 1, 1);
    Blitzstrahl[3].LoadImage("blitzstrahl4.png", 32, 32, 32, 32, 1, 1);
    Blitzflash[0].LoadImage("blitzflash1.png", 66, 64, 66, 64, 1, 1);
    Blitzflash[1].LoadImage("blitzflash2.png", 66, 64, 66, 64, 1, 1);
    Blitzflash[2].LoadImage("blitzflash3.png", 66, 64, 66, 64, 1, 1);
    Blitzflash[3].LoadImage("blitzflash4.png", 66, 64, 66, 64, 1, 1);
    BlitzTexture.LoadImage("blitztexture.png", 64, 64, 64, 64, 1, 1);
    // Spieler grafiken laden
    SchussFlamme[0].LoadImage("schussflamme.png", 76, 72, 38, 24, 2, 3);
    SchussFlamme[1].LoadImage("schussflamme2.png", 80, 108, 40, 36, 2, 3);
    SchussFlamme[2].LoadImage("schussflamme3.png", 48, 114, 24, 38, 2, 3);
    SchussFlammeFlare.LoadImage("schussflammeflare.png", 140, 140, 140, 140, 1, 1);

    // Diamant schuss
    ProjectileGrafix[DIAMONDSHOT].LoadImage("diamant.png", 261, 29, 29, 29, 9, 1);
    ShotRect[DIAMONDSHOT].left = 5;
    ShotRect[DIAMONDSHOT].right = 24;
    ShotRect[DIAMONDSHOT].top = 5;
    ShotRect[DIAMONDSHOT].bottom = 24;

    // Skeletor Schuss
    ProjectileGrafix[SKELETOR_SHOT].LoadImage("skeletor_shot.png", 5, 40, 5, 40, 1, 1);
    ShotRect[SKELETOR_SHOT].left = 10;
    ShotRect[SKELETOR_SHOT].right = 30;
    ShotRect[SKELETOR_SHOT].top = 10;
    ShotRect[SKELETOR_SHOT].bottom = 30;

    // Spreadshot
    ProjectileGrafix[SPREADSHOT].LoadImage("spreadshot.png", 16, 16, 16, 16, 1, 1);
    ShotRect[SPREADSHOT].left = 4;
    ShotRect[SPREADSHOT].right = 12;
    ShotRect[SPREADSHOT].top = 4;
    ShotRect[SPREADSHOT].bottom = 12;

    // Spreadshot2
    ProjectileGrafix[SPREADSHOT2].LoadImage("spreadshot2.png", 20, 20, 20, 20, 1, 1);
    ShotRect[SPREADSHOT2].left = 2;
    ShotRect[SPREADSHOT2].right = 16;
    ShotRect[SPREADSHOT2].top = 2;
    ShotRect[SPREADSHOT2].bottom = 16;

    // Riesen Spreadshot
    ProjectileGrafix[SPREADSHOTBIG].LoadImage("spreadshot_big.png", 32, 32, 32, 32, 1, 1);
    ShotRect[SPREADSHOTBIG].left = 4;
    ShotRect[SPREADSHOTBIG].right = 28;
    ShotRect[SPREADSHOTBIG].top = 4;
    ShotRect[SPREADSHOTBIG].bottom = 28;

    // Riesen Spreadshot doppelt
    ProjectileGrafix[SPREADSHOTBIG2].LoadImage("spreadshot_big2.png", 40, 40, 40, 40, 1, 1);
    ShotRect[SPREADSHOTBIG2].left = 6;
    ShotRect[SPREADSHOTBIG2].right = 34;
    ShotRect[SPREADSHOTBIG2].top = 6;
    ShotRect[SPREADSHOTBIG2].bottom = 34;

    // Bounceshot
    ProjectileGrafix[BOUNCESHOT1].LoadImage("bounce1.png", 24, 24, 24, 24, 1, 1);
    ShotRect[BOUNCESHOT1].left = 4;
    ShotRect[BOUNCESHOT1].right = 20;
    ShotRect[BOUNCESHOT1].top = 4;
    ShotRect[BOUNCESHOT1].bottom = 20;

    ProjectileGrafix[BOUNCESHOT2].LoadImage("bounce2.png", 16, 16, 16, 16, 1, 1);
    ShotRect[BOUNCESHOT2].left = 4;
    ShotRect[BOUNCESHOT2].right = 12;
    ShotRect[BOUNCESHOT2].top = 4;
    ShotRect[BOUNCESHOT2].bottom = 12;

    ProjectileGrafix[BOUNCESHOT3].LoadImage("bounce3.png", 10, 10, 10, 10, 1, 1);
    ShotRect[BOUNCESHOT3].left = 0;
    ShotRect[BOUNCESHOT3].right = 10;
    ShotRect[BOUNCESHOT3].top = 0;
    ShotRect[BOUNCESHOT3].bottom = 10;

    // Riesen Bounceshot
    ProjectileGrafix[BOUNCESHOTBIG1].LoadImage("bounce_big1.png", 48, 48, 48, 48, 1, 1);
    ShotRect[BOUNCESHOTBIG1].left = 8;
    ShotRect[BOUNCESHOTBIG1].right = 40;
    ShotRect[BOUNCESHOTBIG1].top = 8;
    ShotRect[BOUNCESHOTBIG1].bottom = 40;

    ProjectileGrafix[BOUNCESHOTBIG2].LoadImage("bounce_big2.png", 32, 32, 32, 32, 1, 1);
    ShotRect[BOUNCESHOTBIG2].left = 4;
    ShotRect[BOUNCESHOTBIG2].right = 28;
    ShotRect[BOUNCESHOTBIG2].top = 4;
    ShotRect[BOUNCESHOTBIG2].bottom = 28;

    ProjectileGrafix[BOUNCESHOTBIG3].LoadImage("bounce_big3.png", 20, 20, 20, 20, 1, 1);
    ShotRect[BOUNCESHOTBIG3].left = 2;
    ShotRect[BOUNCESHOTBIG3].right = 18;
    ShotRect[BOUNCESHOTBIG3].top = 2;
    ShotRect[BOUNCESHOTBIG3].bottom = 18;

    // Lasershot
    ProjectileGrafix[LASERSHOT].LoadImage("laser.png", 9, 48, 9, 48, 1, 1);
    ShotRect[LASERSHOT].left = -4;
    ShotRect[LASERSHOT].right = 15;
    ShotRect[LASERSHOT].top = 18;
    ShotRect[LASERSHOT].bottom = 30;

    // Lasershot2
    ProjectileGrafix[LASERSHOT2].LoadImage("laser2.png", 160, 48, 16, 48, 10, 1);
    ShotRect[LASERSHOT2].left = 0;
    ShotRect[LASERSHOT2].right = 20;
    ShotRect[LASERSHOT2].top = 12;
    ShotRect[LASERSHOT2].bottom = 38;

    // Riesen Lasershot
    ProjectileGrafix[LASERSHOTBIG].LoadImage("laserbig.png", 16, 80, 16, 80, 1, 1);
    ShotRect[LASERSHOTBIG].left = 0;
    ShotRect[LASERSHOTBIG].right = 30;
    ShotRect[LASERSHOTBIG].top = 30;
    ShotRect[LASERSHOTBIG].bottom = 50;

    // Riesen Lasershot
    ProjectileGrafix[LASERSHOTBIG2].LoadImage("laserbig2.png", 280, 80, 28, 80, 10, 1);
    ShotRect[LASERSHOTBIG2].left = 0;
    ShotRect[LASERSHOTBIG2].right = 30;
    ShotRect[LASERSHOTBIG2].top = 30;
    ShotRect[LASERSHOTBIG2].bottom = 50;

    // Walker Laser
    ProjectileGrafix[WALKER_LASER].LoadImage("walker-laser.png", 32, 3, 32, 3, 1, 1);
    ShotRect[WALKER_LASER].left = 0;
    ShotRect[WALKER_LASER].right = 32;
    ShotRect[WALKER_LASER].top = 0;
    ShotRect[WALKER_LASER].bottom = 3;

    // Geschützturm Kugel
    ProjectileGrafix[CANONBALL].LoadImage("canonball.png", 8, 8, 8, 8, 1, 1);
    ShotRect[CANONBALL].left = 0;
    ShotRect[CANONBALL].right = 8;
    ShotRect[CANONBALL].top = 0;
    ShotRect[CANONBALL].bottom = 8;

    // Powerline
    ProjectileGrafix[POWERLINE].LoadImage("powerline.png", 80, 20, 40, 20, 2, 1);
    ShotRect[POWERLINE].left = 0;
    ShotRect[POWERLINE].right = 20;
    ShotRect[POWERLINE].top = 0;
    ShotRect[POWERLINE].bottom = 20;

    // Deckenturm Kugel
    ProjectileGrafix[SUCHSCHUSS].LoadImage("suchschuss.png", 8, 8, 8, 8, 1, 1);
    ShotRect[SUCHSCHUSS].left = 0;
    ShotRect[SUCHSCHUSS].right = 8;
    ShotRect[SUCHSCHUSS].top = 0;
    ShotRect[SUCHSCHUSS].bottom = 8;

    // Blauer Schuss der Riesenwepse
    ProjectileGrafix[SUCHSCHUSS2].LoadImage("suchschuss2.png", 64, 16, 16, 16, 4, 1);
    ShotRect[SUCHSCHUSS2].left = 1;
    ShotRect[SUCHSCHUSS2].right = 15;
    ShotRect[SUCHSCHUSS2].top = 1;
    ShotRect[SUCHSCHUSS2].bottom = 15;

    // Grüner Golemschuss
    ProjectileGrafix[GOLEMSCHUSS].LoadImage("golemschuss.png", 100, 100, 20, 20, 5, 5);
    ShotRect[GOLEMSCHUSS].left = 2;
    ShotRect[GOLEMSCHUSS].right = 18;
    ShotRect[GOLEMSCHUSS].top = 2;
    ShotRect[GOLEMSCHUSS].bottom = 18;

    // Grüner Skeletor Schuss
    ProjectileGrafix[TURRIEXTRAWURST].LoadImage("golemschuss.png", 100, 100, 20, 20, 5, 5);
    ShotRect[TURRIEXTRAWURST].left = 2;
    ShotRect[TURRIEXTRAWURST].right = 18;
    ShotRect[TURRIEXTRAWURST].top = 2;
    ShotRect[TURRIEXTRAWURST].bottom = 18;

    // Lilaner langsamer Spinnenschuss
    ProjectileGrafix[SPIDERSLOW].LoadImage("spiderslow.png", 200, 200, 40, 40, 5, 5);
    ShotRect[SPIDERSLOW].left = 5;
    ShotRect[SPIDERSLOW].right = 35;
    ShotRect[SPIDERSLOW].top = 5;
    ShotRect[SPIDERSLOW].bottom = 35;

    // Lilaner Spinnenlaser
    ProjectileGrafix[SPIDERLASER].LoadImage("fetterspinnenlaser.png", 64, 128, 64, 128, 1, 1);
    ShotRect[SPIDERLASER].left = 0;
    ShotRect[SPIDERLASER].right = 500;
    ShotRect[SPIDERLASER].top = 0;
    ShotRect[SPIDERLASER].bottom = 10;

    // Spinnenschuss
    ProjectileGrafix[SPIDERSHOT].LoadImage("spidershot.png", 96, 48, 96, 48, 1, 1);
    ShotRect[SPIDERSHOT].left = 24;
    ShotRect[SPIDERSHOT].right = 72;
    ShotRect[SPIDERSHOT].top = 12;
    ShotRect[SPIDERSHOT].bottom = 36;

    // Spinnenschuss2
    ProjectileGrafix[SPIDERSHOT2].LoadImage("spidershot2.png", 48, 48, 48, 48, 1, 1);
    ShotRect[SPIDERSHOT2].left = 16;
    ShotRect[SPIDERSHOT2].right = 32;
    ShotRect[SPIDERSHOT2].top = 16;
    ShotRect[SPIDERSHOT2].bottom = 32;

    // Bombe des Spieler-Rads
    ProjectileGrafix[BOMBE].LoadImage("bombe_small.png", 128, 32, 16, 16, 8, 2);
    ShotRect[BOMBE].left = 0;
    ShotRect[BOMBE].right = 16;
    ShotRect[BOMBE].top = 0;
    ShotRect[BOMBE].bottom = 12;

    // Bombe des Spieler-Rads
    ProjectileGrafix[BOMBEBIG].LoadImage("bombe_big.png", 192, 48, 24, 24, 8, 2);
    ShotRect[BOMBEBIG].left = 0;
    ShotRect[BOMBEBIG].right = 24;
    ShotRect[BOMBEBIG].top = 0;
    ShotRect[BOMBEBIG].bottom = 20;

    // Flammen-Splitter
    ProjectileGrafix[FLAMME].LoadImage("flamme.png", 192, 96, 48, 48, 4, 2);
    ShotRect[FLAMME].left = 16;
    ShotRect[FLAMME].right = 32;
    ShotRect[FLAMME].top = 16;
    ShotRect[FLAMME].bottom = 32;

    // Flamme des Wand-Flammenwerfers
    ProjectileGrafix[FEUERFALLE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[FEUERFALLE].left = 30;
    ShotRect[FEUERFALLE].right = 54;
    ShotRect[FEUERFALLE].top = 30;
    ShotRect[FEUERFALLE].bottom = 70;

    // Feuerball des Minidrachen
    ProjectileGrafix[FIREBALL].LoadImage("fireball.png", 24, 48, 24, 48, 1, 1);
    ShotRect[FIREBALL].left = 0;
    ShotRect[FIREBALL].right = 24;
    ShotRect[FIREBALL].top = 12;
    ShotRect[FIREBALL].bottom = 36;

    // Feuerball des Riesendrachen
    ProjectileGrafix[FIREBALL_BIG].LoadImage("fireball_big.png", 60, 54, 60, 54, 1, 1);
    ShotRect[FIREBALL_BIG].left = 8;
    ShotRect[FIREBALL_BIG].right = 52;
    ShotRect[FIREBALL_BIG].top = 8;
    ShotRect[FIREBALL_BIG].bottom = 46;

    // Feuerball des Riesendrachen
    ProjectileGrafix[FIREBALL_BOMB].LoadImage("fireball_big.png", 60, 54, 60, 54, 1, 1);
    ShotRect[FIREBALL_BOMB].left = 8;
    ShotRect[FIREBALL_BOMB].right = 52;
    ShotRect[FIREBALL_BOMB].top = 8;
    ShotRect[FIREBALL_BOMB].bottom = 46;

    // Rotzschuss des Rotzpotts
    ProjectileGrafix[ROTZSHOT].LoadImage("rotzshot.png", 80, 80, 16, 16, 5, 5);
    ShotRect[ROTZSHOT].left = 2;
    ShotRect[ROTZSHOT].right = 14;
    ShotRect[ROTZSHOT].top = 2;
    ShotRect[ROTZSHOT].bottom = 14;

    // Flamme des Wand-Flammenwerfers
    ProjectileGrafix[GOLEMSAEULE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[GOLEMSAEULE].left = 30;
    ShotRect[GOLEMSAEULE].right = 54;
    ShotRect[GOLEMSAEULE].top = 30;
    ShotRect[GOLEMSAEULE].bottom = 70;

    // Flamme des Lavamanns
    ProjectileGrafix[FEUERFALLE_LAVAMANN].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[FEUERFALLE_LAVAMANN].left = 30;
    ShotRect[FEUERFALLE_LAVAMANN].right = 54;
    ShotRect[FEUERFALLE_LAVAMANN].top = 30;
    ShotRect[FEUERFALLE_LAVAMANN].bottom = 70;

    // Flamme der Feuer Spinne
    ProjectileGrafix[PLAYERFIRE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[PLAYERFIRE].left = 30;
    ShotRect[PLAYERFIRE].right = 54;
    ShotRect[PLAYERFIRE].top = 30;
    ShotRect[PLAYERFIRE].bottom = 70;

    // Flamme der Feuer Spinne
    ProjectileGrafix[SPIDERFIRE].LoadImage("flame.png", 504, 500, 84, 100, 6, 5);
    ShotRect[SPIDERFIRE].left = 30;
    ShotRect[SPIDERFIRE].right = 54;
    ShotRect[SPIDERFIRE].top = 30;
    ShotRect[SPIDERFIRE].bottom = 70;

    // Laser des Deckenkrabblers
    ProjectileGrafix[KRABBLERLASER1].LoadImage("krabblerlaser1.png", 4, 48, 4, 48, 1, 1);
    ShotRect[KRABBLERLASER1].left = 0;
    ShotRect[KRABBLERLASER1].right = 4;
    ShotRect[KRABBLERLASER1].top = 0;
    ShotRect[KRABBLERLASER1].bottom = 48;

    // Laser des Wandkrabblers links
    ProjectileGrafix[KRABBLERLASER2].LoadImage("krabblerlaser2.png", 48, 4, 48, 4, 1, 1);
    ShotRect[KRABBLERLASER2].left = 0;
    ShotRect[KRABBLERLASER2].right = 48;
    ShotRect[KRABBLERLASER2].top = 0;
    ShotRect[KRABBLERLASER2].bottom = 4;

    // Laser des Wandkrabblers rechts
    ProjectileGrafix[KRABBLERLASER3].LoadImage("krabblerlaser2.png", 48, 4, 48, 4, 1, 1);
    ShotRect[KRABBLERLASER3].left = 0;
    ShotRect[KRABBLERLASER3].right = 48;
    ShotRect[KRABBLERLASER3].top = 0;
    ShotRect[KRABBLERLASER3].bottom = 4;

    // Schuss des Pharao Kopfes
    ProjectileGrafix[PHARAOLASER].LoadImage("pharaolaser.png", 96, 96, 48, 48, 2, 2);
    ShotRect[PHARAOLASER].left = 0;
    ShotRect[PHARAOLASER].right = 48;
    ShotRect[PHARAOLASER].top = 0;
    ShotRect[PHARAOLASER].bottom = 48;

    // Granate
    ProjectileGrafix[GRENADE].LoadImage("grenade.png", 11, 14, 11, 14, 1, 1);
    ShotRect[GRENADE].left = 0;
    ShotRect[GRENADE].right = 11;
    ShotRect[GRENADE].top = 0;
    ShotRect[GRENADE].bottom = 14;

    // Rakete
    ProjectileGrafix[ROCKET].LoadImage("rocket.png", 225, 225, 75, 75, 3, 3);
    ShotRect[ROCKET].left = 20;
    ShotRect[ROCKET].right = 55;
    ShotRect[ROCKET].top = 20;
    ShotRect[ROCKET].bottom = 55;

    ProjectileGrafix[ROCKETSPIDER].LoadImage("rocket.png", 225, 225, 75, 75, 3, 3);
    ShotRect[ROCKETSPIDER].left = 20;
    ShotRect[ROCKETSPIDER].right = 55;
    ShotRect[ROCKETSPIDER].top = 20;
    ShotRect[ROCKETSPIDER].bottom = 55;

    // Rakete
    ProjectileGrafix[ROCKETWERFER].LoadImage("rocketwerfer.png", 45, 24, 45, 12, 1, 2);
    ShotRect[ROCKETWERFER].left = 0;
    ShotRect[ROCKETWERFER].right = 45;
    ShotRect[ROCKETWERFER].top = 0;
    ShotRect[ROCKETWERFER].bottom = 12;

    // Torpedo
    ProjectileGrafix[TORPEDO].LoadImage("torpedo.png", 33, 20, 33, 10, 1, 2);
    ShotRect[TORPEDO].left = 0;
    ShotRect[TORPEDO].right = 33;
    ShotRect[TORPEDO].top = 0;
    ShotRect[TORPEDO].bottom = 10;

    // EvilShot
    ProjectileGrafix[EVILSHOT].LoadImage("evilshot.png", 32, 16, 32, 16, 1, 1);
    ShotRect[EVILSHOT].left = 0;
    ShotRect[EVILSHOT].right = 32;
    ShotRect[EVILSHOT].top = 0;
    ShotRect[EVILSHOT].bottom = 16;

    // EvilShot2
    ProjectileGrafix[EVILSHOT2].LoadImage("evilshot2.png", 32, 16, 32, 16, 1, 1);
    ShotRect[EVILSHOT2].left = 0;
    ShotRect[EVILSHOT2].right = 32;
    ShotRect[EVILSHOT2].top = 0;
    ShotRect[EVILSHOT2].bottom = 16;

    // EvilBlitz
    ProjectileGrafix[EVILBLITZ].LoadImage("evilblitz.png", 140, 100, 70, 100, 2, 1);
    ShotRect[EVILBLITZ].left = 0;
    ShotRect[EVILBLITZ].right = 70;
    ShotRect[EVILBLITZ].top = 0;
    ShotRect[EVILBLITZ].bottom = 100;

    // BeamBlitz
    ProjectileGrafix[BLITZBEAM].LoadImage("blitzbeam.png", 320, 320, 160, 160, 2, 2);
    ShotRect[BLITZBEAM].left = 40;
    ShotRect[BLITZBEAM].right = 120;
    ShotRect[BLITZBEAM].top = 40;
    ShotRect[BLITZBEAM].bottom = 120;

    // Ufolaser
    ProjectileGrafix[UFOLASER].LoadImage("ufolaser.png", 340, 100, 34, 100, 10, 1);
    ShotRect[UFOLASER].left = 5;
    ShotRect[UFOLASER].right = 29;
    ShotRect[UFOLASER].top = 10;
    ShotRect[UFOLASER].bottom = 90;

    // Stelzlaser
    ProjectileGrafix[STELZLASER].LoadImage("stelzlaser.png", 120, 43, 60, 43, 2, 1);
    ShotRect[STELZLASER].left = 5;
    ShotRect[STELZLASER].right = 55;
    ShotRect[STELZLASER].top = 5;
    ShotRect[STELZLASER].bottom = 38;

    // Pflanzenschuss
    ProjectileGrafix[PFLANZESHOT].LoadImage("pflanzeschuss.png", 64, 16, 16, 16, 4, 1);
    ShotRect[PFLANZESHOT].left = 1;
    ShotRect[PFLANZESHOT].right = 15;
    ShotRect[PFLANZESHOT].top = 1;
    ShotRect[PFLANZESHOT].bottom = 15;

    // Bratklopsshot
    // DKS - There was only one frame in this image file, but four are specified here:
    //      I've changed it from 2,2 to 1,1. Also see my notes in Boss_Bratklops.cpp.
    //      I've also fixed the right and bottom coordinates of ShotRect:
    // ProjectileGrafix[BRATKLOPSSHOT].LoadImage("bratklopsshot.png", 60, 60, 60, 60, 2, 2);
    ProjectileGrafix[BRATKLOPSSHOT].LoadImage("bratklopsshot.png", 60, 60, 60, 60, 1, 1);
    ShotRect[BRATKLOPSSHOT].left = 10;
    ShotRect[BRATKLOPSSHOT].right = 50;
    ShotRect[BRATKLOPSSHOT].top = 10;
    ShotRect[BRATKLOPSSHOT].bottom = 50;

    // Dronebullet
    ProjectileGrafix[DRONEBULLET].LoadImage("dronebullet.png", 48, 12, 12, 12, 4, 1);
    ShotRect[DRONEBULLET].left = 2;
    ShotRect[DRONEBULLET].right = 10;
    ShotRect[DRONEBULLET].top = 2;
    ShotRect[DRONEBULLET].bottom = 10;

    // SchleimShot
    ProjectileGrafix[SCHLEIMSHOT].LoadImage("schleimshot.png", 12, 12, 12, 12, 1, 1);
    ShotRect[SCHLEIMSHOT].left = 2;
    ShotRect[SCHLEIMSHOT].right = 10;
    ShotRect[SCHLEIMSHOT].top = 2;
    ShotRect[SCHLEIMSHOT].bottom = 10;

    // LaFass
    ProjectileGrafix[LAFASSSHOT].LoadImage("lafass_fall.png", 264, 332, 66, 83, 4, 4);
    ShotRect[LAFASSSHOT].left = 10;
    ShotRect[LAFASSSHOT].right = 56;
    ShotRect[LAFASSSHOT].top = 10;
    ShotRect[LAFASSSHOT].bottom = 73;

    // EvilBlitz2
    ProjectileGrafix[EVILBLITZ2].LoadImage("evilblitz2.png", 140, 400, 70, 400, 2, 1);
    ShotRect[EVILBLITZ2].left = 0;
    ShotRect[EVILBLITZ2].right = 70;
    ShotRect[EVILBLITZ2].top = 0;
    ShotRect[EVILBLITZ2].bottom = 400;

    // FlugLaser
    ProjectileGrafix[FLUGLASER].LoadImage("flugkanonelaser.png", 8, 32, 8, 32, 1, 1);
    ShotRect[FLUGLASER].left = 0;
    ShotRect[FLUGLASER].right = 8;
    ShotRect[FLUGLASER].top = 0;
    ShotRect[FLUGLASER].bottom = 32;

    // Eiszapfengeschoss
    ProjectileGrafix[EISZAPFENSHOT].LoadImage("eiszapfenshot.png", 12, 48, 12, 48, 1, 1);
    ShotRect[EISZAPFENSHOT].left = 2;
    ShotRect[EISZAPFENSHOT].right = 10;
    ShotRect[EISZAPFENSHOT].top = 0;
    ShotRect[EISZAPFENSHOT].bottom = 48;

    // Schuss der im Bogen fliegt
    ProjectileGrafix[ARCSHOT].LoadImage("arcshot.png", 8, 8, 8, 8, 1, 1);
    ShotRect[ARCSHOT].left = 0;
    ShotRect[ARCSHOT].right = 8;
    ShotRect[ARCSHOT].top = 0;
    ShotRect[ARCSHOT].bottom = 8;

    // SchneeBombe
    ProjectileGrafix[SNOWBOMB].LoadImage("snowbomb.png", 480, 320, 80, 80, 6, 4);
    ShotRect[SNOWBOMB].left = 10;
    ShotRect[SNOWBOMB].right = 70;
    ShotRect[SNOWBOMB].top = 10;
    ShotRect[SNOWBOMB].bottom = 70;

    // SchneeBombe klein
    ProjectileGrafix[SNOWBOMBSMALL].LoadImage("snowbombsmall.png", 120, 80, 20, 20, 6, 4);
    ShotRect[SNOWBOMBSMALL].left = 5;
    ShotRect[SNOWBOMBSMALL].right = 15;
    ShotRect[SNOWBOMBSMALL].top = 5;
    ShotRect[SNOWBOMBSMALL].bottom = 15;

    // Schuss der fetten Spinne
    ProjectileGrafix[FETTESPINNESHOT].LoadImage("fettespinneshot.png", 160, 50, 40, 50, 4, 1);
    ShotRect[FETTESPINNESHOT].left = 5;
    ShotRect[FETTESPINNESHOT].right = 35;
    ShotRect[FETTESPINNESHOT].top = 5;
    ShotRect[FETTESPINNESHOT].bottom = 45;

    // Schuss der fetten Spinne
    ProjectileGrafix[FETTESPINNESHOT2].LoadImage("fettespinneshot2.png", 160, 50, 40, 50, 4, 1);
    ShotRect[FETTESPINNESHOT2].left = 5;
    ShotRect[FETTESPINNESHOT2].right = 35;
    ShotRect[FETTESPINNESHOT2].top = 5;
    ShotRect[FETTESPINNESHOT2].bottom = 45;

    // Eierbombe des Eiermanns
    ProjectileGrafix[EIERBOMBE].LoadImage("eierbombe.png", 125, 100, 25, 25, 5, 4);
    ShotRect[EIERBOMBE].left = 5;
    ShotRect[EIERBOMBE].right = 20;
    ShotRect[EIERBOMBE].top = 5;
    ShotRect[EIERBOMBE].bottom = 20;

    // Rundumschuss des evil hurri
    ProjectileGrafix[EVILROUND1].LoadImage("evilround.png", 10, 10, 10, 10, 1, 1);
    ShotRect[EVILROUND1].left = 0;
    ShotRect[EVILROUND1].right = 10;
    ShotRect[EVILROUND1].top = 0;
    ShotRect[EVILROUND1].bottom = 10;

    // SmartBomb laden
    ProjectileGrafix[SMARTBOMB].LoadImage("druckwelle.png", 256, 256, 256, 256, 1, 1);
    ShotRect[SMARTBOMB].left = 0;
    ShotRect[SMARTBOMB].right = 256;
    ShotRect[SMARTBOMB].top = 0;
    ShotRect[SMARTBOMB].bottom = 256;

    // Schutzschild Generator laden
    ProjectileGrafix[SHIELDSPAWNER].LoadImage("shieldspawner.png", 120, 12, 12, 12, 10, 1);
    ShotRect[SHIELDSPAWNER].left = 0;
    ShotRect[SHIELDSPAWNER].right = 12;
    ShotRect[SHIELDSPAWNER].top = 0;
    ShotRect[SHIELDSPAWNER].bottom = 12;

    // Blaue Bombe laden
    ProjectileGrafix[BLUEBOMB].LoadImage("blauebombe.png", 150, 120, 30, 30, 5, 4);
    ShotRect[BLUEBOMB].left = 4;
    ShotRect[BLUEBOMB].right = 26;
    ShotRect[BLUEBOMB].top = 4;
    ShotRect[BLUEBOMB].bottom = 26;

    // Blaue Bombe laden
    ProjectileGrafix[SKELETORGRANATE].LoadImage("skeletor_granate.png", 100, 60, 20, 30, 5, 2);
    ShotRect[SKELETORGRANATE].left = 4;
    ShotRect[SKELETORGRANATE].right = 16;
    ShotRect[SKELETORGRANATE].top = 4;
    ShotRect[SKELETORGRANATE].bottom = 15;

    // Blaue Bombe laden
    ProjectileGrafix[ELEKTROPAMPE].LoadImage("elektropampe.png", 640, 640, 640, 128, 1, 5);
    ShotRect[ELEKTROPAMPE].left = 0;
    ShotRect[ELEKTROPAMPE].right = 640;
    ShotRect[ELEKTROPAMPE].top = 0;
    ShotRect[ELEKTROPAMPE].bottom = 128;

    // Blaue Bombe laden
    ProjectileGrafix[ELEKTROSCHUSS].LoadImage("spidershot2.png", 48, 48, 48, 48, 1, 1);
    ShotRect[ELEKTROSCHUSS].left = 16;
    ShotRect[ELEKTROSCHUSS].right = 32;
    ShotRect[ELEKTROSCHUSS].top = 16;
    ShotRect[ELEKTROSCHUSS].bottom = 32;
}

// --------------------------------------------------------------------------------------
// Destruktor : Löschen der ganzen Liste und Freigabe der Projektil-Grafiken
// --------------------------------------------------------------------------------------

ProjectileListClass::~ProjectileListClass() {
    // Schuss-Liste komplett leeren
    ClearAll();
}

// --------------------------------------------------------------------------------------
// Schuss "Art" hinzufügen
// --------------------------------------------------------------------------------------

// DKS - Modified to support new pooled memory manager as well as the fact that
//      projectile list is now singly-linked
#if 0
bool ProjectileListClass::PushProjectile(float x, float y, int Art, PlayerClass* pTemp)
{
    if(NumProjectiles >= MAX_SHOTS)					// Grenze überschritten ?
        return false;

    ProjectileClass *pNew = new ProjectileClass;	// Neues zu erstellendes Projectile

    pNew->CreateShot(x, y, Art, pTemp);					// neues Projektil erzeugen

    if(pStart==nullptr)						// Liste leer ?
    {
        pStart = pNew;						// Ja, dann neues projektil gleich das erste
        pEnd   = pNew;						// und letzte Projektil

        pStart->pNext=nullptr;					// Next/Previous gibts nich, da wir
        pStart->pPrev=nullptr;					// nur 1 Projektil haben
    }
    else									// Liste ist NICHT leer
    {
        pEnd->pNext = pNew;					// Letztes Projektil zeigt auf das neue
        pNew->pPrev = pEnd;					// Letztes Projektil ist nicht mehr das letzte

        pNew->pNext = nullptr;					// Nach dem neuen Projektil kommt keines mehr
        pEnd		= pNew;					// da es jetzt das letzte in der Liste ist
    }

    NumProjectiles++;							// Projektilanzahl erhöhen
    return true;
}
#endif  // 0
bool ProjectileListClass::PushProjectile(float x, float y, int Art, PlayerClass *pTemp) {
    if (NumProjectiles >= MAX_SHOTS)
        return false;

        // DKS - added support for new, fast pooled mem-manager:
#ifdef USE_NO_MEMPOOLING
    ProjectileClass *pNew = new ProjectileClass;
#else
    ProjectileClass *pNew = projectile_pool.alloc();
#endif

    pNew->CreateShot(x, y, Art, pTemp);
    pNew->pNext = nullptr;

    if (pEnd)
        pEnd->pNext = pNew;  // If list isn't empty, insert this projectile on the end.
    else
        pStart = pNew;  // Or, if list is empty, make this the new head of the list.

    pEnd = pNew;  // Update end-of-list pointer

    NumProjectiles++;
    return true;
}

// --------------------------------------------------------------------------------------
// BlitzBeam hinzufügen
// --------------------------------------------------------------------------------------

bool ProjectileListClass::PushBlitzBeam(int Size, float Richtung, PlayerClass *pSource) {
    if (NumProjectiles >= MAX_SHOTS)  // Grenze überschritten ?
        return false;

        // DKS - added support for new, fast pooled mem-manager
#ifdef USE_NO_MEMPOOLING
    ProjectileClass *pNew = new ProjectileClass;  // Neues zu erstellendes Projectile
#else
    ProjectileClass *pNew = projectile_pool.alloc();
#endif

    pNew->ShotArt = BLITZBEAM;
    pNew->xPos = pSource->xpos - Size / 2 + 20;
    pNew->yPos = pSource->ypos - Size / 2 + 32;
    pNew->xPosOld = pNew->xPos;
    pNew->yPosOld = pNew->yPos;
    pNew->AnimPhase = 0;
    pNew->AnimCount = 0.0f;
    // DKS - off-by-one error:
    // pNew->AnimEnde     = 4;
    pNew->AnimEnde = 3;
    pNew->AnimSpeed = 0.5f;
    pNew->Damage = Size;
    pNew->DamagePlayer = false;
    pNew->BounceWalls = false;
    pNew->pParent = pSource;
    pNew->ExplodeOnImpact = false;
    pNew->OwnDraw = true;

    Richtung += 180;
    if (Richtung > 360.0f)
        Richtung -= 360.0f;

    // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
    // In Rad umwandeln
    // Richtung = PI * Richtung / 180.0f;
    // pNew->xSpeed	= static_cast<float>(-40.0f * sin(Richtung));
    // pNew->ySpeed	= static_cast<float>( 40.0f * cos(Richtung));
    pNew->xSpeed = -40.0f * sin_deg(Richtung);
    pNew->ySpeed = 40.0f * cos_deg(Richtung);

    // DKS - Since our ProjectileClass constructor is now empty, and the BLITZBEAM projectile
    //      does not use CreateShot(), we must insert the missing four pieces of the old
    //      constructor here:
    pNew->xAcc = 0.0f;
    pNew->yAcc = 0.0f;
    pNew->Counter = 0.0f;
    pNew->Winkel = -10000.0f;  // wird von den wenigsten Schüssen verwendet

    // DKS - Dividing an int by a float and then casting to an int again makes no sense at all:
    // ShotRect[BLITZBEAM].left   = static_cast<int>(Size / 4.0f),
    //                    ShotRect[BLITZBEAM].top    = static_cast<int>(Size / 4.0f),
    //                                        ShotRect[BLITZBEAM].right  = ShotRect[BLITZBEAM].left +
    //                                        static_cast<int>(Size / 2.0f);
    // ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top  + static_cast<int>(Size / 2.0f);
    ShotRect[BLITZBEAM].left = Size / 4;
    ShotRect[BLITZBEAM].top = Size / 4;
    ShotRect[BLITZBEAM].right = ShotRect[BLITZBEAM].left + Size / 2;
    ShotRect[BLITZBEAM].bottom = ShotRect[BLITZBEAM].top + Size / 2;

    pNew->pNext = nullptr;

    if (pEnd)
        pEnd->pNext = pNew;  // If list isn't empty, insert this projectile on the end.
    else
        pStart = pNew;  // Or, if list is empty, make this the new head of the list.

    pEnd = pNew;  // Update end-of-list pointer

    NumProjectiles++;  // Projektilanzahl erhöhen
    return true;
}

// --------------------------------------------------------------------------------------
// Bestimmtes Projektil der Liste löschen
// --------------------------------------------------------------------------------------

// DKS - Replaced DelSel() with DelNode(), which supports the now-singly-linked-list. It operates
//      a bit differently:
//      It is now up to the caller to splice the list, this blindly deletes what is passed to it
//      and returns the pointer that was in pPtr->pNext, or NULL if pPtr was NULL.
ProjectileClass *ProjectileListClass::DelNode(ProjectileClass *pPtr) {
    ProjectileClass *pNext = nullptr;
    if (pPtr != nullptr) {
        pNext = pPtr->pNext;

        if (pStart == pPtr)  // Are we deleting the first node in the list?
            pStart = pNext;

            // DKS - added support for new, fast pooled mem-manager:
#ifdef USE_NO_MEMPOOLING
        delete (pPtr);
#else
        projectile_pool.free(pPtr);
#endif

        NumProjectiles--;
    }
    return pNext;
}

// --------------------------------------------------------------------------------------
// Alle Projectile der Liste löschen
// --------------------------------------------------------------------------------------
// DKS - Converted ProjectileListClass to a singly-linked list (depends on DelNode() now).
//      and added support for new pooled memory manager.
void ProjectileListClass::ClearAll() {
    if (pStart) {
        ProjectileClass *pNext = pStart->pNext;

        while (pNext)  // Delete everything but the head of the list
            pNext = DelNode(pNext);

        DelNode(pStart);  // Finally, delete the head of the list
    }
    pStart = pEnd = nullptr;

#ifndef NDEBUG
    if (NumProjectiles != 0)
        Protokoll << "ERROR: poss. mem leak / corruption in linked list of projectiles" << std::endl;
#endif

        // DKS - added support for new, fast pooled mem-manager:
#ifndef USE_NO_MEMPOOLING
    projectile_pool.reinit();
#endif

    // Just to be safe:
    NumProjectiles = 0;
}

// --------------------------------------------------------------------------------------
// Alle Projectiles eines Typs löschen
// --------------------------------------------------------------------------------------

// DKS - Adapted after converting ProjectileListClass to a singly-linked list:
void ProjectileListClass::ClearType(int Type) {
    ProjectileClass *pPrev = nullptr;
    ProjectileClass *pCurr = pStart;

    while (pCurr != nullptr) {
        if (pCurr->ShotArt == Type) {
            // If this is the last node in the list, update the main class's pEnd pointer
            if (pEnd == pCurr)
                pEnd = pPrev;

            pCurr = DelNode(pCurr);  // pCurr now points to the node after the one deleted

            if (pPrev) {
                // This is not the first node in the list, so splice this node onto the previous one
                pPrev->pNext = pCurr;
            }
        } else {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        }
    }
}

// --------------------------------------------------------------------------------------
// Zahl der Projectile zurückliefern
// --------------------------------------------------------------------------------------

int ProjectileListClass::GetNumProjectiles() const {
    return NumProjectiles;
}

// --------------------------------------------------------------------------------------
// Alle Proectile der Liste animieren und bewegen
// --------------------------------------------------------------------------------------

// DKS - Adapted after converting projectile list to singly-linked one
void ProjectileListClass::DoProjectiles() {
    ProjectileClass *pPrev = nullptr;
    ProjectileClass *pCurr = pStart;

    CurrentShotTexture = -1;  // Aktuelle Textur gibt es noch keine

    while (pCurr != nullptr) {
        if (!Console.Showing)
            pCurr->Run();

        if (pCurr->Damage > 0)
            pCurr->Render();

        if (!Console.Showing) {
            if (pCurr->Damage <= 0)  // ggf Schuss löschen (bei Damage <= 0)
            {
                // Projectile's time to die..
                // If this is the last node in the list, update the class's pEnd pointer
                if (pEnd == pCurr)
                    pEnd = pPrev;

                pCurr = DelNode(pCurr);  // pCurr now points to the node after the one deleted

                // If this is not the first node in the list, splice this node onto the previous one:
                if (pPrev)
                    pPrev->pNext = pCurr;
            } else {
                pCurr->CheckCollision();
                pPrev = pCurr;
                pCurr = pCurr->pNext;
            }
        } else {
            pPrev = pCurr;
            pCurr = pCurr->pNext;
        }
    }
}
