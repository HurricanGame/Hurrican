// Datei : HUD.cpp

// --------------------------------------------------------------------------------------
//
// Funktionen für das Hurrican HUD
// Werte anzeigen usw
//
// (c) 2002 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Include Dateien
// --------------------------------------------------------------------------------------

#include "HUD.hpp"
#include <algorithm>
#include "DX8Font.hpp"
#include "DX8Graphics.hpp"
#include "Gameplay.hpp"
#include "GegnerClass.hpp"
#include "Player.hpp"
#include "Tileengine.hpp"
#include "Timer.hpp"
#include "enemies/Gegner_Stuff.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor : HUD Grafiken laden und Werte initialisieren
// --------------------------------------------------------------------------------------

HUDClass::HUDClass() {
    // HUD Werte initialisieren
    Alpha = 192;
    red = 0;
    green = 255;
    blue = 0;
    xpos = (RENDERWIDTH - 548) / 2;
    ypos = 2;
    BossHUDActive = 0.0f;
}

// --------------------------------------------------------------------------------------
// Konstruktor : HUD Grafiken freigeben
// --------------------------------------------------------------------------------------

HUDClass::~HUDClass() {}

// DKS - Added LoadSprites() function and moved these here from class constructor,
//      to allow statically-allocated HUD global var (used to be dynamically allocated)
void HUDClass::LoadSprites() {
    HUDGfx.LoadImage("hud.png", 548, 69, 548, 69, 1, 1);
    HUDBlitz.LoadImage("hudblitz.png", 6, 18, 6, 18, 1, 1);
    HUDBomb.LoadImage("hudbomb.png", 18, 18, 18, 18, 1, 1);
    HUDSuperShot.LoadImage("hudsupershot.png", 24, 24, 24, 24, 1, 1);
    HUDAutoFire.LoadImage("hudautofire.png", 24, 24, 24, 24, 1, 1);
    HUDBall[0].LoadImage("hudenergy.png", 69, 69, 69, 69, 1, 1);
    HUDBall[1].LoadImage("hudrad.png", 69, 69, 69, 69, 1, 1);
    HUDFontBig.LoadImage("hudfontbig.png", 128, 32, 12, 22, 10, 1);
    SelectedWeapon.LoadImage("hudselected.png", 68, 17, 17, 17, 4, 1);
    WeaponRahmen.LoadImage("hudbalken.png", 9, 18, 9, 18, 1, 1);
    WeaponPunkt.LoadImage("hudbalken_full.png", 7, 2, 7, 2, 1, 1);
    Arrow.LoadImage("hudarrow.png", 24, 33, 24, 33, 1, 1);

    BossHUD.LoadImage("bosshud.png", 130, 41, 130, 41, 1, 1);
    BossBar.LoadImage("bossbar.png", 113, 24, 113, 24, 1, 1);
}

// --------------------------------------------------------------------------------------
// Werte im HUD ändern
// --------------------------------------------------------------------------------------

void HUDClass::UpdateValues() {}

// --------------------------------------------------------------------------------------
// HUD anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::ShowHUD() {

    DirectGraphics.SetColorKeyMode();

    // Farbe des Huds
    D3DCOLOR Color = D3DCOLOR_RGBA(red, green, blue, Alpha);

    HUDGfx.RenderSprite(xpos, ypos, 0, Color);

    // Für die Balken-Offsets
    int BlitzOff = static_cast<int>(BLITZ_STAT_HEIGHT - Player[0].CurrentWeaponLevel[3] * BLITZ_STAT_HEIGHT / BLITZ_STAT_HEIGHT); // ???

    // DKS - Added support for font scaling
    int scale_factor = pDefaultFont->GetScaleFactor();

    if (CommandLineParams.LowRes)
        // DKS - Note: For the HUD GUI, font scaling is limited to 2, as the numbers need to fit
        pDefaultFont->SetScaleFactor(2);  // Anything more than 2 won't fit, force it

    if (NUMPLAYERS == 1) {
        float off = floor((MAX_ENERGY - Player[0].Energy) * 69.0f / MAX_ENERGY);
        HUDBall[0].SetRect(0, off, 69, 69);
        HUDBall[0].RenderSprite(xpos, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));

        // Radenergie
        off = floor((MAX_ARMOUR - Player[0].Armour) * 69.0f / MAX_ARMOUR);
        HUDBall[1].SetRect(0, off, 69, 69);
        HUDBall[1].RenderSprite(xpos + 548 - 69, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));
    } else
        for (int num = 0; num < NUMPLAYERS; num++) {
            PlayerClass *pCurrentPlayer;

            if (num == 0) {
                pCurrentPlayer = &Player[0];

                float off;
                // Energy
                off = floor((MAX_ENERGY - pCurrentPlayer->Energy) / MAX_ENERGY * 69);
                HUDBall[0].SetRect(0, off, 34, 69);
                HUDBall[0].RenderSprite(xpos, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));

                // Radenergie
                off = floor((MAX_ARMOUR - pCurrentPlayer->Armour) / MAX_ENERGY * 69);
                HUDBall[1].SetRect(34, off, 69, 69);
                HUDBall[1].RenderSprite(xpos + 34, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));
            } else {
                pCurrentPlayer = &Player[1];

                float off;
                // Energy
                off = floor((MAX_ENERGY - pCurrentPlayer->Energy) / MAX_ENERGY * 69);
                HUDBall[0].SetRect(0, off, 34, 69);
                HUDBall[0].RenderSprite(xpos + 548 - 69, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));

                // Radenergie
                off = floor((MAX_ARMOUR - pCurrentPlayer->Armour) / MAX_ARMOUR * 69);
                HUDBall[1].SetRect(34, off, 69, 69);
                HUDBall[1].RenderSprite(xpos + 548 - 69 + 34, ypos + off, D3DCOLOR_RGBA(255, 255, 255, 255));
            }
        }

    HUDBlitz.SetRect(0, BlitzOff, 6, 18);

    // Wenn geblitzt wird, die Blitzwaffe heller anzeigen
    if (Player[0].Handlung == PlayerActionEnum::BLITZEN)
        SelectedWeapon.RenderSprite(xpos + 212 + 3 * 32, ypos + 14, 3, Color);

    D3DCOLOR color;

    // Blitz-Level anzeigen
    if (Player[0].Handlung == PlayerActionEnum::BLITZEN)
        color = D3DCOLOR_RGBA(0, 255, 0, 192);
    else
        color = D3DCOLOR_RGBA(0, 255, 0, 64);

    WeaponRahmen.RenderSprite(xpos + 216 + 3 * 32, ypos + 35, 0, color);
    for (int j = 0; j < Player[0].CurrentWeaponLevel[3]; j++)
        WeaponPunkt.RenderSprite(xpos + 217 + 3 * 32, ypos + 50 - j, 0, color);

    D3DCOLOR playercol;

    // Anzahl verbleibender Powerlines anzeigen
    for (int p = 0; p < NUMPLAYERS; p++) {
        int off;

        if (NUMPLAYERS == 1) {
            playercol = Color;
            off = 0;
        } else {
            off = -6;
            if (p == 0)
                playercol = 0xFFFF4400;
            else
                playercol = 0xFF2266FF;
        }

        int xoff;

        if (NUMPLAYERS == 1)
            xoff = 0;
        else {
            if (p == 0)
                xoff = -25;
            else
                xoff = 15;
        }

        // AutoFire anzeigen, wenn vorhanden
        if (Player[p].AutoFireExtra > 0.0f) {
            HUDAutoFire.RenderSprite(xpos + 263 + xoff, ypos + 61, 0, Color);
            std::string buf = std::to_string(static_cast<int>(Player[p].AutoFireExtra));

            if (Player[p].AutoFireExtra < 10) {
                pDefaultFont->DrawText(xpos + 270 + xoff, ypos + 86, buf.c_str(), D3DCOLOR_RGBA(0, 0, 0, Alpha));
                pDefaultFont->DrawText(xpos + 269 + xoff, ypos + 85, buf.c_str(), playercol);
            } else if (Player[p].AutoFireExtra < 100) {
                pDefaultFont->DrawText(xpos + 271 + xoff, ypos + 86, buf.c_str(), D3DCOLOR_RGBA(0, 0, 0, Alpha));
                pDefaultFont->DrawText(xpos + 270 + xoff, ypos + 85, buf.c_str(), playercol);
            } else {
                pDefaultFont->DrawText(xpos + 269 + xoff, ypos + 86, buf.c_str(), D3DCOLOR_RGBA(0, 0, 0, Alpha));
                pDefaultFont->DrawText(xpos + 268 + xoff, ypos + 85, buf.c_str(), playercol);
            }
        }

        // Supershot anzeigen, wenn vorhanden (bei vorhandenem Autofire wird der Supershot tiefer angezeigt)
        if (Player[p].RiesenShotExtra > 0.0f) {
            int off2 = 0;

            if (Player[p].AutoFireExtra > 0.0f)
                off2 += 37;

            HUDSuperShot.RenderSprite(xpos + 264 + xoff, ypos + 61 + off2, 0, Color);
            std::string buf = std::to_string(static_cast<int>(Player[p].RiesenShotExtra));

            if (Player[p].RiesenShotExtra < 10) {
                pDefaultFont->DrawText(xpos + 270 + xoff, ypos + 87 + off2, buf.c_str(), D3DCOLOR_RGBA(0, 0, 0, Alpha));
                pDefaultFont->DrawText(xpos + 269 + xoff, ypos + 86 + off2, buf.c_str(), playercol);
            } else if (Player[p].RiesenShotExtra < 100) {
                pDefaultFont->DrawText(xpos + 271 + xoff, ypos + 87 + off2, buf.c_str(), D3DCOLOR_RGBA(0, 0, 0, Alpha));
                pDefaultFont->DrawText(xpos + 270 + xoff, ypos + 86 + off2, buf.c_str(), playercol);
            } else {
                pDefaultFont->DrawText(xpos + 269 + xoff, ypos + 87 + off2, buf.c_str(), D3DCOLOR_RGBA(0, 0, 0, Alpha));
                pDefaultFont->DrawText(xpos + 268 + xoff, ypos + 86 + off2, buf.c_str(), playercol);
            }
        }

        // DKS - Added support for font scaling
        if (CommandLineParams.LowRes) {
            // Adjust this portion of the HUD for low-resolution (scaled font)
            // Basically, draw the two numbers for the powerlines and grenades a bit to the left and a tad higher

            // Only ever display a max of 9, since we've no room
            int digit = std::clamp(Player[p].PowerLines, 0, 9);

            std::string buf = std::to_string(digit);

            pDefaultFont->DrawText(xpos + 350 + off + p * 10, ypos + 36, buf.c_str(), playercol);

            // Anzahl verbleibender Granaten anzeigen
            digit = std::clamp(Player[p].Grenades, 0, 9);
            buf = std::to_string(digit);

            pDefaultFont->DrawText(xpos + 372 + off + p * 10, ypos + 36, buf.c_str(), playercol);
        } else {
            // DKS - Original, normal-resolution code:
            std::string buf = std::to_string(Player[p].PowerLines);

            if (Player[p].PowerLines < 10)
                pDefaultFont->DrawText(xpos + 353 + off + p * 10, ypos + 40, buf.c_str(), playercol);
            else
                pDefaultFont->DrawText(xpos + 349 + off + p * 10, ypos + 40, buf.c_str(), playercol);

            // Anzahl verbleibender Granaten anzeigen
            buf = std::to_string(Player[p].Grenades);

            if (Player[p].Grenades < 10)
                pDefaultFont->DrawText(xpos + 376 + off + p * 10, ypos + 40, buf.c_str(), playercol);
            else
                pDefaultFont->DrawText(xpos + 372 + off + p * 10, ypos + 40, buf.c_str(), playercol);
        }

        // Befindet sich die Smartbombe im Besitz des Spielers ?
        if (Player[p].SmartBombs > 0) {
            if (NUMPLAYERS == 1)
                HUDBomb.RenderSprite(xpos + 391, ypos + 19, 0, playercol);
            else {
                if (p == 0) {
                    HUDBomb.SetRect(0, 0, 9, 18);
                    HUDBomb.RenderSprite(xpos + 391, ypos + 19, playercol);
                } else {
                    HUDBomb.SetRect(10, 0, 18, 18);
                    HUDBomb.RenderSprite(xpos + 391 + 9, ypos + 19, playercol);
                }
            }
        }

        // Gewählte Waffe heller darstellen
        if (Player[p].Handlung != PlayerActionEnum::BLITZEN) {
            if (NUMPLAYERS == 1)
                SelectedWeapon.RenderSprite(xpos + 212 + Player[p].SelectedWeapon * 32, ypos + 14,
                                            Player[p].SelectedWeapon, playercol);
            else {
                // Anderer Spieler hat eine andere Waffe? Dann ganz rendern
                if (Player[p].SelectedWeapon != Player[1 - p].SelectedWeapon)
                    SelectedWeapon.RenderSprite(xpos + 212 + Player[p].SelectedWeapon * 32, ypos + 14,
                                                Player[p].SelectedWeapon, playercol);
                // ansonsten halbieren
                else {
                    SelectedWeapon.itsRect = SelectedWeapon.itsPreCalcedRects[Player[p].SelectedWeapon];

                    if (p == 0)
                        SelectedWeapon.itsRect.right -= 8;
                    else
                        SelectedWeapon.itsRect.left += 8;

                    SelectedWeapon.RenderSprite(xpos + 212 + p * 8 + Player[p].SelectedWeapon * 32, ypos + 14,
                                                playercol);
                }
            }
        }

        // PowerLevel der Waffen darstellen
        for (int i = 0; i < 3; i++) {
            if (NUMPLAYERS == 2 && Player[p].SelectedWeapon != Player[1 - p].SelectedWeapon &&
                Player[1 - p].SelectedWeapon == i)
                continue;

            if (i == Player[p].SelectedWeapon) {
                if (NUMPLAYERS == 1)
                    playercol = D3DCOLOR_RGBA(0, 255, 0, 224);
                else {
                    if (p == 0)
                        playercol = 0xFFFF4400;
                    else
                        playercol = 0xFF2266FF;
                }
            } else {
                if (NUMPLAYERS == 1 || Player[p].SelectedWeapon != i)
                    playercol = D3DCOLOR_RGBA(0, 255, 0, 64);
                else {
                    if (p == 0)
                        playercol = 0x40FF4400;
                    else
                        playercol = 0x402266FF;
                }
            }

            if (NUMPLAYERS == 1 || Player[p].SelectedWeapon != Player[1 - p].SelectedWeapon)
                WeaponRahmen.RenderSprite(xpos + 216 + i * 32, ypos + 35, 0, playercol);
            else {
                WeaponRahmen.itsRect.left = 0;
                WeaponRahmen.itsRect.right = 9;
                WeaponRahmen.itsRect.top = 0;
                WeaponRahmen.itsRect.bottom = 18;

                if (p == 0)
                    WeaponRahmen.itsRect.right = 5;
                else
                    WeaponRahmen.itsRect.left = 5;

                WeaponRahmen.RenderSprite(xpos + static_cast<float>(p * 5 + 216 + i * 32),
                                          ypos + 35.0f, playercol);
            }

            // Waffen Punkte
            for (int j = 0; j < Player[p].CurrentWeaponLevel[i]; j++) {
                if (NUMPLAYERS == 1 || (Player[p].SelectedWeapon == i &&
                                        Player[p].SelectedWeapon != Player[1 - p].SelectedWeapon))

                    WeaponPunkt.RenderSprite(xpos + 217 + i * 32, ypos + 50 - j * 2, 0, playercol);
                else {
                    WeaponPunkt.itsRect.left = 0;
                    WeaponPunkt.itsRect.right = 7;
                    WeaponPunkt.itsRect.top = 0;
                    WeaponPunkt.itsRect.bottom = 2;

                    if (p == 0)
                        WeaponPunkt.itsRect.right = 4;
                    else
                        WeaponPunkt.itsRect.left = 4;

                    WeaponPunkt.RenderSprite(xpos + p * 4 + 217 + i * 32, ypos + 50 - j * 2, playercol);
                }
            }
        }
    }

    // Anzahl verbleibender Leben anzeigen
    int LivesToShow = std::max(0, Player[0].Lives);

    std::string buf = std::to_string(LivesToShow);

    for (std::size_t i = 0; i < buf.length(); i++) {
        char z = buf[i] - 48;
        HUDFontBig.SetRect(z * 12, 0, z * 12 + 12, 25);
        HUDFontBig.RenderSprite(xpos - buf.length() * 13 + i * 13 + 48, ypos + 22, Color);
    }

    // Punkte anzeigen
    if (HasCheated)
        Player[0].Score = 0;

    buf = std::to_string(Player[0].Score);

    for (std::size_t i = 0; i < buf.length(); i++) {
        char z = buf[i] - 48;
        HUDFontBig.SetRect(z * 12, 0, z * 12 + 12, 25);
        HUDFontBig.RenderSprite(xpos - buf.length() * 13 + i * 13 + 195, ypos + 22, Color);
    }

    // Diamanten anzeigen
    if (NUMPLAYERS == 1)
        buf = std::to_string(Player[0].CollectedDiamonds);

    // oder Leben Spieler 2
    else {
        LivesToShow = std::max(0, Player[1].Lives);
        buf = std::to_string(LivesToShow);
    }

    for (std::size_t i = 0; i < buf.length(); i++) {
        char z = buf[i] - 48;
        HUDFontBig.SetRect(z * 12, 0, z * 12 + 12, 25);
        HUDFontBig.RenderSprite(xpos - buf.length() * 13 + i * 13 + 527, ypos + 22, Color);
    }

    if (NUMPLAYERS == 2) {
        buf = std::to_string(Player[0].CollectedDiamonds);
        pGegnerGrafix[DIAMANT]->RenderSprite(xpos + 555, ypos + 15, 0, Color);
        pDefaultFont->DrawTextCenterAlign(xpos + 566, ypos + 50, buf.c_str(), Color);
    }

    // Verbleibende Zeit anzeigen
    buf = std::to_string(static_cast<int>(TileEngine.Timelimit));

    for (std::size_t i = 0; i < buf.length(); i++) {
        char z = buf[i] - 48;

        HUDFontBig.SetRect(z * 12, 0, z * 12 + 12, 25);
        HUDFontBig.RenderSprite(xpos - buf.length() * 13 + i * 13 + 467, ypos + 22, Color);

        // Zeit wird knapp? Dann Rote Zahlen rendern
        if (BossHUDActive <= 0.0f && TileEngine.Timelimit < 10.0f && TileEngine.Timelimit > 0.0f) {
            DirectGraphics.SetAdditiveMode();

            float size = (static_cast<float>(static_cast<int>(TileEngine.Timelimit + 1)) - TileEngine.Timelimit) * 255.0f;

            float xoff = 0.0f;
            if (static_cast<int>(TileEngine.Timelimit) == 1)
                xoff = -size / 2.4f;

            HUDFontBig.RenderSpriteScaled(320 - size / 2.0f + xoff, 240 - size / 2.0f, 12 + static_cast<int>(size),
                                          24 + static_cast<int>(size),
                                          D3DCOLOR_RGBA(255, 0, 0, 255 - static_cast<int>(size)));

            DirectGraphics.SetColorKeyMode();
        }
    }

    // DKS - Added support for font scaling
    // Reset font scale factor to what it was before drawing HUD:
    pDefaultFont->SetScaleFactor(scale_factor);
}

// --------------------------------------------------------------------------------------
// Boss HUD anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::RenderBossHUD() {
    // Boss HUD anzeigen
    if (BossHUDActive > 0.0f) {
        int a = static_cast<int>(BossHUDActive * 0.75f);
        D3DCOLOR Color = D3DCOLOR_RGBA(red, green, blue, a);

        BossHUDActive -= Timer.sync(20.0f);  // Ausfaden lassen

        BossHUD.RenderSprite((RENDERWIDTH - 130) / 2, ypos + 434, Color);
        BossBar.RenderSprite((RENDERWIDTH - 130) / 2 + 9, ypos + 434 + 13, Color);
    } else
        BossHUDActive = 0.0f;
}

// --------------------------------------------------------------------------------------
// Boss HUD mit Boss Energie anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::ShowBossHUD(float max, float act) {
    // Boss HUD einfaden
    BossHUDActive += Timer.sync(40.0f);

    if (BossHUDActive > Alpha)
        BossHUDActive = static_cast<float>(Alpha);

    // Balken länge berechnen
    int off = static_cast<int>((act - 100.0f) * 113.0f / (max - 100.0f));

    if (off < 0)
        off = 0;

    BossBar.SetRect(0, 0, off, 24);
}

// --------------------------------------------------------------------------------------
// Alles machen, was da HUD betrifft, sprich, werte ändern und dann anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::DoHUD() {
    // Hud Werte aktualisieren
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        Player[p].BlitzLength = Player[p].CurrentWeaponLevel[3] + 1;

    UpdateValues();

    // Einheitsmatrix setzen, da das HUD nie rotiert gerendert wird
    //
    glm::mat4x4 matView = glm::mat4x4(1.0f);
    g_matView = matView;

    // Hud anzeigen
    //
    ShowHUD();

    // Tutorial Pfeil anzeigen?
    //
    if (bShowArrow)
        RenderArrow();

    // und evtl wieder rotierten Screen setzen
    //
    SetScreenShake();
}

// --------------------------------------------------------------------------------------
// Pfeil in diesem Frame anzeigen
// --------------------------------------------------------------------------------------

void HUDClass::ShowArrow(float x, float y) {
    bShowArrow = true;
    ArrowX = x;
    ArrowY = y;
}

// --------------------------------------------------------------------------------------
// Pfeil rendern
// --------------------------------------------------------------------------------------

void HUDClass::RenderArrow() {
    static float alpha = 0.0f;
    static float alphadir = 50.0f;

    alpha += Timer.sync(alphadir);

    if ((alphadir > 0.0f && alpha > 255.0f) || (alphadir < 0.0f && alpha < 0.0f)) {
        alphadir *= -1.0f;

        alpha = std::clamp(alpha, 0.0f, 255.0f);
    }

    D3DCOLOR Color = D3DCOLOR_RGBA(0, 255, 0, static_cast<int>(alpha));

    DirectGraphics.SetColorKeyMode();
    Arrow.RenderSprite(ArrowX, ArrowY, 0, Color);
}
