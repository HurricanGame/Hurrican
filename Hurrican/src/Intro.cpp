// Datei : Intro.cpp

// --------------------------------------------------------------------------------------
//
// Klasse für das Hurrican Intro
//
// (c) 2006 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Intro.hpp"
#include <stdio.h>
#include <algorithm>
#include <string>
#include <vector>
#include "DX8Sound.hpp"
#include "Gameplay.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor, lädt die Grafiken
// --------------------------------------------------------------------------------------

IntroClass::IntroClass() {
    Background[0].LoadImage("intro1.png", 640, 480, 640, 480, 1, 1);
    Background[1].LoadImage("intro2.png", 640, 480, 640, 480, 1, 1);
    Background[2].LoadImage("intro3.png", 640, 480, 640, 480, 1, 1);
    Background[3].LoadImage("intro4.png", 640, 480, 640, 480, 1, 1);
    Background[4].LoadImage("intro5.png", 640, 480, 640, 480, 1, 1);
    Background[5].LoadImage("intro6.png", 640, 480, 640, 480, 1, 1);

    Zustand = IntroStateEnum::FADEIN;

    EntriesOff = 0;  // Offset into actual lines displayed on the screen (can vary based on font size)
    TextOff = 0;     // Offset into what text would be displayed with a normal-sized font
    BildNr = 0;
    HorizCounter = 0.0f;
    Counter = 0.0f;
    SoundManager.LoadSong("intro.it", MUSIC::INTRO);

    while (DirectInput.AreAllKeysReleased() == false)
        DirectInput.UpdateTastatur();

    // Generate a series of lines from the intro text that are split if too long to display:
    for (int line_ctr = 0; line_ctr <= 23; line_ctr++) {
        int off = TEXT::INTRO1 + line_ctr;
        const int max_length = RENDERWIDTH - 20;

        // There are two versions of one line, one after the other, depending on number of players:
        if (off == TEXT::INTRO1 + 22 && NUMPLAYERS == 2)
            off++;
        else if (off >= TEXT::INTRO1 + 23)
            off++;

        char buf1[255];
        char buf2[255];
        char *srcbuf = TextArray[off];

        // Copy as much as will fit into buf1, remainder without leading whitespace into buf2.
        bool was_split = ExtractStringOfLength(buf1, buf2, srcbuf, max_length, pDefaultFont);

        IntroEntry tmp_entry;
        if (was_split) {
            // Line was split, part that will fit is in dst1, remainder in dst2
            tmp_entry.should_pause = false;  // First entry is only part of a line, don't pause on it..
            tmp_entry.text = buf1;
            entries.push_back(tmp_entry);

            tmp_entry.should_pause = true;
            tmp_entry.text = buf2;
            entries.push_back(tmp_entry);
        } else {
            // Line did not need to be split, so nothing was placed in buf1 or buf2
            tmp_entry.should_pause = true;
            tmp_entry.text = srcbuf;
            entries.push_back(tmp_entry);
        }
    }
}

// --------------------------------------------------------------------------------------
// Desktruktor
// --------------------------------------------------------------------------------------

IntroClass::~IntroClass() {
    // DKS
    // MUSIC_StopAllSongs();
    SoundManager.StopSongs();

    // DKS - Game was not freeing music data, added this:
    SoundManager.UnloadSong(MUSIC::INTRO);
}

// --------------------------------------------------------------------------------------
// Intro beenden
// --------------------------------------------------------------------------------------

void IntroClass::EndIntro() {
    if (Zustand != IntroStateEnum::FADEOUT) {
        if (Zustand != IntroStateEnum::FADEIN) {
            Counter = 255.0f;
        }

        Zustand = IntroStateEnum::FADEOUT;
        SoundManager.FadeSong(MUSIC::INTRO, -1.5f, 0, false);
    }
}

// --------------------------------------------------------------------------------------
// Intro ablaufen lassen
// --------------------------------------------------------------------------------------

void IntroClass::DoIntro() {
    // Hintergrund rendern
    DirectGraphics.SetColorKeyMode();

    int a = std::clamp((TextOff - 1) / 4, 0, 5);

    Background[a].RenderSprite(0, 0, 0, 0xFFFFFFFF);

    // nächstes Bild überblenden?
    if ((TextOff - 1) % 4 == 3 && a < 5) {
        int alpha = static_cast<int>(Counter / 1300.0f * 255.0f);
        D3DCOLOR fadecol = D3DCOLOR_RGBA(255, 255, 255, static_cast<uint8_t>(alpha));
        Background[a + 1].RenderSprite(0, 0, 0, fadecol);
    }

    // Balken unten
    RenderRect4(0, 360, 640, 60, 0x00000000, 0x00000000, 0xFF000000, 0xFF000000);
    RenderRect(0, 420, 640, 60, 0xFF000000);

    // Intro laufen lassen
    switch (Zustand) {
        case IntroStateEnum::FADEIN:  // Text scrollen
        {
            // Mucke spielen
            // DKS - Added function SongIsPlaying() to SoundManagerClass:
            if (!SoundManager.SongIsPlaying(MUSIC::INTRO))
                SoundManager.PlaySong(MUSIC::INTRO, false);

            // und einfaden
            Counter += Timer.sync(10.0f);

            if (Counter > 255.0f) {
                Counter = 0.0f;
                Zustand = IntroStateEnum::RUN;
            } else {
                D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255 - static_cast<int>(Counter));
                RenderRect(0, 0, RENDERWIDTH, RENDERHEIGHT, col);
            }
        } break;

        case IntroStateEnum::FADEOUT: {
            // und ausfaden
            Counter -= Timer.sync(5.0f);

            if (Counter < 0.0f) {
                Counter = 0.0f;
                Zustand = IntroStateEnum::DONE;
            } else if (Counter > 255.0f) {
                Counter = 255.0f;
            }

            D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255 - static_cast<int>(Counter));
            RenderRect(0, 0, RENDERWIDTH, RENDERHEIGHT, col);

        } break;

        // Scroller
        case IntroStateEnum::RUN:
            // DKS - Added low-resolution scaled-font support:
            {
                int scale_factor = pDefaultFont->GetScaleFactor();

                Counter += Timer.sync(25.5f);
                if (Counter > 1300.0f) {
                    HorizCounter = 0.0f;
                    Counter = 0.0f;

                    TextOff++;
                    EntriesOff++;

                    if (TextOff > 23) {
                        EndIntro();
                        break;
                    }
                }

                // DKS - Horizontal text scrolling now depends on an identical counter that doesn't get reset on skipped
                // lines
                HorizCounter += Timer.sync(25.5f);
                if (HorizCounter > 1300.0f) {
                    HorizCounter = 0.0f;
                }

                // Text rendern
                float xr = HorizCounter;
                if (xr > 700.0f)
                    xr = 700.0f;

                DirectGraphics.SetAdditiveMode();

                const int lines_displayed = 10 / scale_factor;
                const int line_y_off = 12 * scale_factor;

                int num_entries = entries.size();
                for (int t = 0; t <= num_entries; t++) {

                    int alpha = 255 - (scale_factor * (EntriesOff - t) * 25);

                    D3DCOLOR col = D3DCOLOR_RGBA(0, 255, 0, static_cast<uint8_t>(std::clamp(alpha, 0, 255)));

                    if (t <= EntriesOff && (t - EntriesOff) > -lines_displayed && t < num_entries) {
                        const int tmp_h = 15 * scale_factor;
                        pDefaultFont->DrawText(
                            10, static_cast<float>(RENDERHEIGHT - tmp_h + t * line_y_off) - EntriesOff * line_y_off,
                            entries[t].text.c_str(), col);

                        // Teil des Textes verdecken
                        if (t == EntriesOff) {
                            DirectGraphics.SetColorKeyMode();
                            const int tmp_h2 = 17;
                            RenderRect(xr, RENDERHEIGHT - (tmp_h2 - 1) * scale_factor, RENDERWIDTH,
                                       tmp_h2 * scale_factor, 0xFF000000);
                        }
                    }
                }

                // grüne Rechtecke rendern
                if (EntriesOff < num_entries) {
                    int l = pDefaultFont->StringLength(entries[EntriesOff].text.c_str(), 0) + 28;
                    int tmp_h = 16 * scale_factor;
                    if (xr < l) {
                        for (int i = 0; i < 5; i++) {
                            D3DCOLOR col;

                            switch (i) {
                                case 0:
                                    col = D3DCOLOR_RGBA(255, 255, 255, 255);
                                    break;
                                case 1:
                                    col = D3DCOLOR_RGBA(0, 255, 0, 255);
                                    break;
                                case 2:
                                    col = D3DCOLOR_RGBA(0, 180, 0, 255);
                                    break;
                                case 3:
                                    col = D3DCOLOR_RGBA(0, 90, 0, 255);
                                    break;
                                case 4:
                                    col = D3DCOLOR_RGBA(0, 50, 0, 255);
                                    break;
                            }

                            RenderRect(xr - i * 12 - 12, RENDERHEIGHT - tmp_h, 12, 12 * scale_factor, col);
                        }
                    } else {
                        xr = l;

                        // blinken lassen
                        if (static_cast<int>(Counter / 100.0f) % 2 == 0)
                            RenderRect(xr - 12, RENDERHEIGHT - tmp_h, 12, 12 * scale_factor, 0xFFFFFFFF);
                    }

                    // Advance immediately to next line if the line was split:
                    if (entries[EntriesOff].should_pause == false && xr == l) {
                        HorizCounter = 0.0f;
                        EntriesOff++;
                    }
                }
            }
            break;

        default:
            break;

    }  // switch
}
