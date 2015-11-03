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

#include <stdio.h>
#include <vector>
#include <string>
#include "Intro.h"
#include "DX8Sound.h"
#include "Gameplay.h"

// --------------------------------------------------------------------------------------
// Konstruktor, lädt die Grafiken
// --------------------------------------------------------------------------------------

IntroClass::IntroClass(void)
{
    Background[0].LoadImage("intro1.png", 640, 480, 640, 480, 1, 1);
    Background[1].LoadImage("intro2.png", 640, 480, 640, 480, 1, 1);
    Background[2].LoadImage("intro3.png", 640, 480, 640, 480, 1, 1);
    Background[3].LoadImage("intro4.png", 640, 480, 640, 480, 1, 1);
    Background[4].LoadImage("intro5.png", 640, 480, 640, 480, 1, 1);
    Background[5].LoadImage("intro6.png", 640, 480, 640, 480, 1, 1);

    Zustand = INTRO_FADEIN;

    EntriesOff = 0;     // Offset into actual lines displayed on the screen (can vary based on font size)
    TextOff = 0;        // Offset into what text would be displayed with a normal-sized font
    BildNr = 0;
    HorizCounter = 0.0f;
    Counter = 0.0f;
    SoundManager.LoadSong("intro.it", MUSIC_INTRO);

    while (DirectInput.AreAllKeysReleased() == false)
        DirectInput.UpdateTastatur();

    //Generate a series of lines from the intro text that are split if too long to display:
    for (int line_ctr = 0; line_ctr <= 23; line_ctr++) {
        int off = TEXT_INTRO1 + line_ctr;
        const int max_length = RENDERWIDTH - 20;

        // There are two versions of one line, one after the other, depending on number of players:
        if (off == TEXT_INTRO1 + 22 && NUMPLAYERS == 2)
            off++;
        else if (off >= TEXT_INTRO1 + 23)
            off++;

        char buf1[255];
        char buf2[255];
        char *srcbuf = TextArray[off];

        // Copy as much as will fit into buf1, remainder without leading whitespace into buf2.
        bool was_split = ExtractStringOfLength(buf1, buf2, srcbuf, max_length, pDefaultFont);

        IntroEntry tmp_entry;
        if (was_split) {
            // Line was split, part that will fit is in dst1, remainder in dst2
            tmp_entry.should_pause = false;     // First entry is only part of a line, don't pause on it..
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

IntroClass::~IntroClass(void)
{
    //DKS
    //MUSIC_StopAllSongs();
    SoundManager.StopSongs();

    //DKS - Game was not freeing music data, added this:
    SoundManager.UnloadSong(MUSIC_INTRO);
}

// --------------------------------------------------------------------------------------
// Intro beenden
// --------------------------------------------------------------------------------------

void IntroClass::EndIntro(void)
{
    if (Zustand != INTRO_FADEOUT)
    {
        if (Zustand != INTRO_FADEIN) {
            Counter = 255.0f;
        }

        Zustand = INTRO_FADEOUT;
        SoundManager.FadeSong(MUSIC_INTRO, -1.5f, 0, false);
    }
}

// --------------------------------------------------------------------------------------
// Intro ablaufen lassen
// --------------------------------------------------------------------------------------

void IntroClass::DoIntro(void)
{
    // Hintergrund rendern
    DirectGraphics.SetColorKeyMode();

    int a = (TextOff - 1) / 4;
    if (a < 0) a = 0;
    if (a > 5) a = 5;

    Background[a].RenderSprite(0, 0, 0, 0xFFFFFFFF);

    // nächstes Bild überblenden?
    if ((TextOff - 1) % 4 == 3 &&
            a < 5)
    {
        int alpha = (int)(Counter / 1300.0f * 255.0f);
        D3DCOLOR fadecol = D3DCOLOR_RGBA(255, 255, 255, (uint8_t)alpha);
        Background[a+1].RenderSprite(0, 0, 0, fadecol);
    }

    // Balken unten
    RenderRect4(0, 360, 640, 60, 0x00000000, 0x00000000,
                0xFF000000, 0xFF000000);
    RenderRect(0, 420, 640, 60, 0xFF000000);

    // Intro laufen lassen
    switch(Zustand)
    {
    case INTRO_FADEIN :						// Text scrollen
    {
        // Mucke spielen
        //DKS - Added function SongIsPlaying() to SoundManagerClass:
        if (!SoundManager.SongIsPlaying(MUSIC_INTRO))
            SoundManager.PlaySong(MUSIC_INTRO, false);

        // und einfaden
        Counter += 10.0f SYNC;

        if (Counter > 255.0f)
        {
            Counter = 0.0f;
            Zustand = INTRO_RUN;
        }
        else
        {
            D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255-int(Counter));
            RenderRect(0, 0, 640, 480, col);
        }
    }
    break;

    case INTRO_FADEOUT :
    {
        // und ausfaden
        Counter -= 5.0f SYNC;

        if (Counter < 0.0f)
        {
            Counter = 0.0f;
            Zustand = INTRO_DONE;
        } else if (Counter > 255.0f)
        {
            Counter = 255.0f;
        }

        D3DCOLOR col = D3DCOLOR_RGBA(0, 0, 0, 255-int(Counter));
        RenderRect(0, 0, 640, 480, col);

    }
    break;

    // Scroller
    case INTRO_RUN:
    //DKS - Added low-resolution scaled-font support:
    {
        int scale_factor = pDefaultFont->GetScaleFactor();

        Counter += 25.5f SYNC;
        if (Counter > 1300.0f)
        {
            HorizCounter = 0.0f;
            Counter = 0.0f;

            TextOff++;
            EntriesOff++;

            if (TextOff > 23)
            {
                EndIntro();
                break;
            }
        }

        //DKS - Horizontal text scrolling now depends on an identical counter that doesn't get reset on skipped lines
        HorizCounter += 25.5f SYNC;
        if (HorizCounter > 1300.0f)
        {
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
        for (int t = 0; t <= num_entries; t++)
        {
            D3DCOLOR col;

            int alpha = 255 - (scale_factor * (EntriesOff - t) * 25);
            if (alpha < 0)
                alpha = 0;
            else if (alpha > 255)
                alpha = 255;

            col = D3DCOLOR_RGBA(0, 255, 0, (uint8_t)alpha);

            if (t <= EntriesOff && (t-EntriesOff) > -lines_displayed && t < num_entries)
            {

                const int tmp_h = 15 * scale_factor;
                pDefaultFont->DrawText(10, (float)(RENDERHEIGHT-tmp_h + t*line_y_off) - EntriesOff * line_y_off, 
                        entries[t].text.c_str(), col);

                // Teil des Textes verdecken
                if (t == EntriesOff)
                {
                    DirectGraphics.SetColorKeyMode();
                    const int tmp_h = 17;
                    RenderRect(xr, RENDERHEIGHT - (tmp_h-1)*scale_factor, RENDERWIDTH, tmp_h*scale_factor, 0xFF000000);
                }
            }
        }

        //grüne Rechtecke rendern
        if (EntriesOff < num_entries)
        {
            int l = pDefaultFont->StringLength(entries[EntriesOff].text.c_str(), 0) + 28;
            int tmp_h = 16;
            tmp_h *= scale_factor;
            if (xr < l)
            {
                for (int i = 0; i < 5; i++)
                {
                    D3DCOLOR col;

                    switch(i)
                    {
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

                    RenderRect(xr - i*12 - 12, RENDERHEIGHT - tmp_h, 12, 12*scale_factor, col);
                }
            }
            else
            {
                xr = (float)l;

                // blinken lassen
                if ((int)(Counter / 100.0f) % 2 == 0)
                    RenderRect(xr - 12, RENDERHEIGHT-tmp_h, 12, 12 * scale_factor, 0xFFFFFFFF);
            }

            // Advance immediately to next line if the line was split:
            if (entries[EntriesOff].should_pause == false && xr == l) {
                HorizCounter = 0.0f;
                EntriesOff++;
            }
        }
    }
    break;

    default :
        break;

    } // switch
}
