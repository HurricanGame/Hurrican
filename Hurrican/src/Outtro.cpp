// Datei : Outtro.cpp

// --------------------------------------------------------------------------------------
//
// Klasse für das Hurrican Outtro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Outtro.hpp"
#include <stdio.h>
#include <string>
#include "Console.hpp"
#include "DX8Sound.hpp"
#include "Gameplay.hpp"
#include "Main.hpp"
#include "Menu.hpp"
#include "Partikelsystem.hpp"
#include "Projectiles.hpp"
#include "Tileengine.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor, lädt die Grafiken
// --------------------------------------------------------------------------------------

OuttroClass::OuttroClass() {
    Background.LoadImage("endscreen_background.png", 640, 480, 640, 480, 1, 1);
    Tower.LoadImage("endscreen_turm.png", 199, 315, 199, 315, 1, 1);
    Henry.LoadImage("endscreen_henry.png", 237, 88, 237, 88, 1, 1);
    Reiter[0].LoadImage("p1_endritt.png", 760, 520, 190, 260, 4, 2);
    Reiter[1].LoadImage("p2_endritt.png", 760, 520, 190, 260, 4, 2);

    // DKS - Player 2 textures are now loaded from disk already-converted
    // ConvertPlayerTexture(&Reiter[1]);

    Zustand = OutroStateEnum::FADEIN;

    Counter = 0.0f;
    SmokeDelay = 0.0f;
    TowerOffset = 0.0f;
    finished = false;
    PlayerSmoke = 0.0f;
    Snow = 0.0f;

    PartikelSystem.ClearAll();
    SoundManager.LoadSong("outtro.it", MUSIC::OUTTRO);

    // DKS -
    // SoundManager.StopAllSongs(false);
    // DKS - Why is this here? Commenting it out
    // MUSIC_StopAllSongs();
    SoundManager.StopSongs();
    SoundManager.StopSounds();

    TileEngine.ClearLevel();

    // Ein paar Schneepartikel adden
    for (int i = 0; i < 100; i++)
        PartikelSystem.PushPartikel(static_cast<float>(random(RENDERWIDTH)), static_cast<float>(random(RENDERHEIGHT)),
                                    SCHNEEFLOCKE_END);
}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

OuttroClass::~OuttroClass() {
    // DKS
    // MUSIC_StopAllSongs();
    SoundManager.StopSongs();

    // DKS - Game was not freeing music data, added this:
    SoundManager.UnloadSong(MUSIC::OUTTRO);
}

// --------------------------------------------------------------------------------------
// Wete für die beiden Spieler setzen
// --------------------------------------------------------------------------------------

void OuttroClass::InitPlayerPos() {
    px[0] = 330.0f;
    py[0] = 240.0f;
    xs[0] = 10.0f;
    ys[0] = 2.0f;

    px[1] = 340.0f;
    py[1] = 260.0f;
    xs[1] = 12.0f;
    ys[1] = -3.0f;
}

// --------------------------------------------------------------------------------------
// Outtro ablaufen lassen
// --------------------------------------------------------------------------------------

void OuttroClass::DoOuttro() {
    // Hintergrund rendern
    DirectGraphics.SetColorKeyMode();
    DirectGraphics.SetFilterMode(false);
    Background.RenderSprite(0, 0, 0, 0xFFFFFFFF);

    // Turm rendern
    float off = 0.0f;

    if (Zustand != OutroStateEnum::SCROLLER)
        off = static_cast<float>(sin(Counter / 2.0f)) / 2.0f;

    Tower.RenderSpriteRotated(299 + off, 61 + TowerOffset, TowerOffset / 10.0f, 0xFFFFFFFF);

    // Partikel
    PartikelSystem.DoPartikelSpecial(false);

    // Den Henry, die alte Maske, rendern
    DirectGraphics.SetFilterMode(false);
    Henry.RenderSprite(276, 291, 0xFFFFFFFF);

    // Und nochmal den unteren Teil vom Background drüber
    Background.SetRect(0, 360, RENDERWIDTH, RENDERHEIGHT);
    Background.RenderSprite(0, 360, 0xFFFFFFFF);

    // Auspuff Partikel =)
    PartikelSystem.DoPartikelSpecial(true);

    // Spieler, der rausfliegt
    if (Zustand == OutroStateEnum::PLAYER_FLEES) {
        DirectGraphics.SetFilterMode(true);

        D3DCOLOR col1;

        if (Counter < 12.8f)
            col1 = D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(Counter * 20.0f));
        else
            col1 = 0xFFFFFFFF;

        // Spieler 2
        int a = static_cast<int>((Counter - 15.0f) / 10);
        if (a > 8)
            a = 7;

        if (NUMPLAYERS == 2)
            if (a >= 0) {
                Reiter[1].itsRect = Reiter[1].itsPreCalcedRects[a];

                Reiter[1].itsRect = Reiter[1].itsPreCalcedRects[a];
                Reiter[1].RenderSpriteScaledRotated(px[1] - (190 / 150.0f * Counter / 2.0f),
                                                    py[1] - (260 / 150.0f * Counter / 2.0f), 190 / 180.0f * Counter,
                                                    260 / 180.0f * Counter, 0, col1);

                // Schatten
                Reiter[1].itsRect.bottom = Reiter[1].itsPreCalcedRects[a].top;
                Reiter[1].itsRect.top = Reiter[1].itsPreCalcedRects[a].bottom;
                Reiter[1].RenderSpriteScaledRotated(px[1] - (190 / 150.0f * Counter / 2.0f),
                                                    230.0f + py[1] / 2.0f + Counter * 2.0f, 190 / 180.0f * Counter,
                                                    260 / 180.0f * Counter / 3.0f, 0, 0x88000000);
            }

        // Spieler 1
        a = static_cast<int>(Counter / 10);
        Reiter[0].itsRect = Reiter[0].itsPreCalcedRects[a];

        Reiter[0].RenderSpriteScaledRotated(px[0] - (190 / 100.0f * Counter / 2.0f),
                                            py[0] - (260 / 100.0f * Counter / 2.0f), 190 / 100.0f * Counter,
                                            260 / 100.0f * Counter, 0, col1);

        // Schatten
        Reiter[0].itsRect.bottom = Reiter[0].itsPreCalcedRects[a].top;
        Reiter[0].itsRect.top = Reiter[0].itsPreCalcedRects[a].bottom;
        Reiter[0].RenderSpriteScaledRotated(px[0] - (190 / 100.0f * Counter / 2.0f),
                                            230.0f + py[0] / 2.0f + Counter * 2.0f, 190 / 180.0f * Counter,
                                            260 / 180.0f * Counter / 3.0f, 0, 0x88000000);

        DirectGraphics.SetFilterMode(false);
    }

    if (Console.Showing)
        return;

    // Schnee
    Snow -= Timer.sync(1.0f);

    if (Snow <= 0.0f) {
        Snow = 1.0f;

        PartikelSystem.PushPartikel(static_cast<float>(random(RENDERWIDTH)), -8, SCHNEEFLOCKE_END);
    }

    switch (Zustand) {
        case OutroStateEnum::FADEIN:  // Text scrollen
        {
            // DKS - Added function WaveIsPlaying() to SoundManagerClass:
            if (!SoundManager.WaveIsPlaying(SOUND::TAKEOFF)) {
                SoundManager.PlayWave(100, 128, 5000, SOUND::TAKEOFF);
                SoundManager.PlayWave(50, 128, 8000, SOUND::WIND);
            }

            Counter += Timer.sync(10.0f);

            if (Counter > 255.0f) {
                Counter = 255.0f;
                Zustand = OutroStateEnum::TOWER_EXPLODE;
            }

            D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255 - static_cast<int>(Counter));
            RenderRect(0, 0, RENDERWIDTH, RENDERHEIGHT, col);

            SmokeDelay -= Timer.sync(1.0f);

            // Explosionen und Qualm
            if (SmokeDelay < 0.0f) {
                SmokeDelay = 1.5f;
                PartikelSystem.PushPartikel(static_cast<float>(280 + random(180)),
                                            static_cast<float>(320 + random(10)), SMOKEBIG_OUTTRO);

                PartikelSystem.PushPartikel(static_cast<float>(280 + random(180)),
                                            static_cast<float>(150 + TowerOffset + random(100)), EXPLOSION_MEDIUM2);
            }

        } break;

        // Explosionen
        case OutroStateEnum::TOWER_EXPLODE:
        case OutroStateEnum::PLAYER_FLEES: {
            SmokeDelay -= Timer.sync(1.0f);
            TowerOffset += Timer.sync(0.5f);

            // Explosionen und Qualm
            if (SmokeDelay < 0.0f) {
                SmokeDelay = 0.5f;

                PartikelSystem.PushPartikel(static_cast<float>(280 + random(180)),
                                            static_cast<float>(320 + random(10)), SMOKEBIG_OUTTRO);

                PartikelSystem.PushPartikel(static_cast<float>(280 + random(180)),
                                            static_cast<float>(150 + TowerOffset + random(100)), EXPLOSION_MEDIUM2);
            }

            if (Zustand == OutroStateEnum::TOWER_EXPLODE) {
                if (TowerOffset >= 20.0f) {
                    Counter = 0.0f;
                    Zustand = OutroStateEnum::PLAYER_FLEES;
                    SoundManager.PlayWave(100, 128, 10000, SOUND::TAKEOFF);
                    InitPlayerPos();
                }
            }

            if (Zustand == OutroStateEnum::PLAYER_FLEES) {
                PlayerSmoke -= Timer.sync(1.0f);

                if (PlayerSmoke < 0.0f) {
                    WinkelUebergabe = (Counter) / 2.0f;
                    if (WinkelUebergabe >= 1.0f) {
                        PartikelSystem.PushPartikel(px[0] + Counter / 2.0f, py[0] + Counter / 1.5f,
                                                    EXPLOSION_TRACE_END);
                        PartikelSystem.PushPartikel(px[0] - Counter / 2.0f, py[0] + Counter / 1.5f,
                                                    EXPLOSION_TRACE_END);
                    }

                    if (NUMPLAYERS == 2) {
                        WinkelUebergabe = (Counter - 5.0f) / 2.0f;
                        if (WinkelUebergabe >= 1.0f) {
                            PartikelSystem.PushPartikel(px[1], py[1] + Counter / 5.0f, EXPLOSION_TRACE_END);
                            PartikelSystem.PushPartikel(px[1] - Counter / 2.0f, py[1] + Counter / 5.0f,
                                                        EXPLOSION_TRACE_END);
                        }
                    }

                    PlayerSmoke = 0.2f;
                }

                if (Counter < 40.0f) {
                    xs[0] -= Timer.sync(0.5f);
                    xs[1] -= Timer.sync(0.4f);
                } else if (Counter < 50.0f) {
                    xs[0] -= Timer.sync(2.0f);
                    ys[0] -= Timer.sync(1.0f);

                    xs[1] -= Timer.sync(1.5f);
                    ys[1] += Timer.sync(1.0f);
                } else {
                    xs[0] -= Timer.sync(4.0f);
                    ys[0] -= Timer.sync(2.0f);

                    xs[1] -= Timer.sync(3.0f);
                    ys[1] += Timer.sync(2.0f);
                }

                px[0] += Timer.sync(xs[0]);
                py[0] += Timer.sync(ys[0]);
                px[1] += Timer.sync(xs[1]);
                py[1] += Timer.sync(ys[1]);

                if (Counter < 20.0f)
                    Counter += Timer.sync(0.6f);
                else
                    Counter += Timer.sync(1.2f);

                // Spieler sind weg? Dann nur noch scroller anzeigen und fertig =)
                if (Counter > 80.0f) {
                    Zustand = OutroStateEnum::SCROLLER;
                    TextOff = 0;
                    Counter = 0.0f;

                    SoundManager.StopWave(SOUND::WIND);
                    SoundManager.PlayWave(100, 128, 6000, SOUND::DOORSTOP);
                    SoundManager.PlayWave(100, 128, 8000, SOUND::EXPLOSION2);
                    SoundManager.PlaySong(MUSIC::OUTTRO, false);
                }
            }
        } break;

        case OutroStateEnum::SCROLLER: {
            // DKS - modified to support scaled fonts (used on low-res devices)
            //      Note that, for here, we don't use the LowResCredts[] text array,
            //      as the logic here requires lines to be at specific locations.
            //      We end up with a little jerkiness when lines are split, but that's OK.

            // Explosionen und Qualm
            SmokeDelay -= Timer.sync(1.0f);
            if (SmokeDelay < 0.0f) {
                SmokeDelay = static_cast<float>(random(10)) + 5.0f;

                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(static_cast<float>(280 + random(180)),
                                                static_cast<float>(340 + random(10)), SMOKEBIG_OUTTRO);

                PartikelSystem.PushPartikel(static_cast<float>(280 + random(180)),
                                            static_cast<float>(150 + TowerOffset + random(100)), EXPLOSION_MEDIUM2);
            }

            // Note: original code had each line spaced by 20.0f, 30 lines drawn total, which is more than
            //      will fit on a screen. This code keeps this behavior, but supports low-resolution using
            //      scaled fonts.
            constexpr int MAX_DRAW_WIDTH = RENDERWIDTH - 20;  // Want a 10-pixel border minimum
            int num_lines;
            int yoff_inc = 20;
            if (CommandLineParams.LowRes) {
                // DKS - Only increase the line spacing a bit if running low-res, as it's already spaced quite a big:
                yoff_inc = 24;
                num_lines = 24;
            } else {
                yoff_inc = 30 * pDefaultFont->GetScaleFactor();
                num_lines = 30;
            }

            Counter += Timer.sync(1.5f);

            while (Counter >= static_cast<float>(yoff_inc)) {
                Counter -= static_cast<float>(yoff_inc);
                TextOff++;

                if (TEXT::OUTTRO1 + TextOff > TEXT::SEPERATOR_MARIO + CreditsCount - num_lines)
                    TextOff = 0;
            }

            for (int i = 0, display_line = 0; i < num_lines; i++, display_line++) {
                int AnzahlCredits = CreditsCount - 65;
                int off2 = TextOff + i - 27;
                if (off2 > 0) {
                    std::string text;
                    if (TEXT::OUTTRO1 + off2 < TEXT::SEPERATOR_MARIO)
                        text = TextArray[TEXT::OUTTRO1 + off2];
                    else if (TEXT::OUTTRO1 + off2 <= TEXT::SEPERATOR_MARIO + AnzahlCredits)
                        text = Credits[off2 + 25];
                    else if (TEXT::OUTTRO1 + off2 <= TEXT::SEPERATOR_MARIO + AnzahlCredits + 10)
                        text = TextArray[TEXT::OUTTRO1 + off2 - AnzahlCredits];

                    // rendern
                    if (CommandLineParams.LowRes && text.size() > 10 &&
                        pDefaultFont->StringLength(text.c_str(), 0) > MAX_DRAW_WIDTH) {
                        char text1[255];
                        char text2[255];
                        SplitLine(text1, text2, text.c_str());
                        pDefaultFont->DrawTextCenterAlign(320, static_cast<float>(display_line * yoff_inc) - Counter, text1,
                                                          0xFFEEFFFF, 0);
                        display_line++;
                        pDefaultFont->DrawTextCenterAlign(320, static_cast<float>(display_line * yoff_inc) - Counter, text2,
                                                          0xFFEEFFFF, 0);

                    } else {
                        if (!text.empty())
                            pDefaultFont->DrawTextCenterAlign(320, static_cast<float>(display_line * yoff_inc) - Counter, text.c_str(),
                                                              0xFFEEFFFF, 0);
                    }
                }
            }
        } break;

        default:
            break;

    }  // switch
}
